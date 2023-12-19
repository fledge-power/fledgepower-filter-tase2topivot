/*
 * FledgePower TASE.2 <-> pivot filter plugin.
 *
 * Copyright (c) 2023, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *

 *
 */

#include <config_category.h>

#include "tase2_pivot_filter.hpp"
#include "tase2_pivot_object.hpp"
#include "tase2_pivot_utility.hpp"

TASE2PivotFilter::TASE2PivotFilter (const std::string& filterName,
                                    ConfigCategory* filterConfig,
                                    OUTPUT_HANDLE* outHandle,
                                    OUTPUT_STREAM output)
{
    (void)filterName; /* ignore parameter */

    m_outHandle = outHandle;
    m_output = output;

    reconfigure (filterConfig);
}

TASE2PivotFilter::~TASE2PivotFilter () {}

static bool
checkTypeMatch (std::string& incomingType, TASE2PivotDataPoint* exchangeConfig)
{
    return incomingType == exchangeConfig->getTypeId ();
}

static bool
checkValueRange (const std::string& beforeLog, int value, int min, int max,
                 const std::string& type)
{
    if (value < min || value > max)
    {
        TASE2PivotUtility::log_warn (
            "%s do_value out of range [%d..%d] for %s: %d", beforeLog.c_str (),
            min, max, type.c_str (), value);
        return false;
    }
    return true;
}

static bool
checkValueRange (const std::string& beforeLog, long value, long min, long max,
                 const std::string& type)
{
    if (value < min || value > max)
    {
        TASE2PivotUtility::log_warn (
            "%s do_value out of range [%ld..%ld] for %s: %ld",
            beforeLog.c_str (), min, max, type.c_str (), value);
        return false;
    }
    return true;
}

static bool
checkValueRange (const std::string& beforeLog, double value, double min,
                 double max, const std::string& type)
{
    if (value < min || value > max)
    {
        TASE2PivotUtility::log_warn (
            "%s do_value out of range [%x..%x] for %s: %x", beforeLog.c_str (),
            min, max, type.c_str (), value);
        return false;
    }
    return true;
}

static void
appendTimestampDataObject (PivotDataObject& pivot, bool hasDoTs, long doTs,
                           const std::string& doTsValidity)
{
    if (hasDoTs)
    {
        pivot.addTimestamp (doTs, doTsValidity);
    }
    else
    {
        doTs = (long)PivotTimestamp::GetCurrentTimeInMs ();
        pivot.addTimestamp (doTs, "valid");
    }
}

static void
appendTimestampOperationObject (PivotOperationObject& pivot, bool hasCoTs,
                                long coTs)
{
    if (hasCoTs)
    {
        pivot.addTimestamp (coTs);
    }
    else
    {
        coTs = (long)PivotTimestamp::GetCurrentTimeInMs ();
        pivot.addTimestamp (coTs);
    }
}

template <typename T>
void
TASE2PivotFilter::readAttribute (std::map<std::string, bool>& attributeFound,
                                 Datapoint* dp, const std::string& targetName,
                                 T& out)
{
    const auto& name = dp->getName ();
    if (name != targetName)
    {
        return;
    }
    if (attributeFound[name])
    {
        return;
    }

    if (dp->getData ().getType () == DatapointValue::T_INTEGER)
    {
        out = static_cast<T> (dp->getData ().toInt ());
        attributeFound[name] = true;
    }
}

void
TASE2PivotFilter::readAttribute (std::map<std::string, bool>& attributeFound,
                                 Datapoint* dp, const std::string& targetName,
                                 Datapoint*& out)
{
    const auto& name = dp->getName ();
    if (name != targetName)
    {
        return;
    }
    if (attributeFound[name])
    {
        return;
    }

    out = dp;
    attributeFound[name] = true;
}

void
TASE2PivotFilter::readAttribute (std::map<std::string, bool>& attributeFound,
                                 Datapoint* dp, const std::string& targetName,
                                 std::string& out)
{
    const auto& name = dp->getName ();
    if (name != targetName)
    {
        return;
    }
    if (attributeFound[name])
    {
        return;
    }

    if (dp->getData ().getType () == DatapointValue::T_STRING)
    {
        out = dp->getData ().toStringValue ();
        attributeFound[name] = true;
    }
}

Datapoint*
TASE2PivotFilter::convertDataObjectToPivot (
    Datapoint* sourceDp, TASE2PivotDataPoint* exchangeConfig)
{
    std::string beforeLog
        = TASE2PivotUtility::PluginName
          + " - TASE2PivotFilter::convertDataObjectToPivot -";
    Datapoint* convertedDatapoint = nullptr;

    DatapointValue& dpv = sourceDp->getData ();

    if (dpv.getType () != DatapointValue::T_DP_DICT)
        return nullptr;

    std::vector<Datapoint*>* datapoints = dpv.getDpVec ();
    std::map<std::string, bool> attributeFound
        = { { "do_type", false },        { "do_value", false },
            { "do_cs", false },          { "do_ts", false },
            { "do_ts_validity", false }, { "do_quality_normal_value", false },
            { "do_comingfrom", false } };

    TASE2DataObject dataObject;

    for (Datapoint* dp : *datapoints)
    {
        readAttribute (attributeFound, dp, "do_type", dataObject.doType);
        readAttribute (attributeFound, dp, "do_value", dataObject.doValue);
        readAttribute (attributeFound, dp, "do_validity",
                       dataObject.doValidity);
        readAttribute (attributeFound, dp, "do_cs",
                       dataObject.doCurrentSource);
        readAttribute (attributeFound, dp, "do_quality_normal_value",
                       dataObject.doNormalValue);
        readAttribute (attributeFound, dp, "do_ts", dataObject.doTs);
        readAttribute (attributeFound, dp, "do_ts_validity",
                       dataObject.doTsValidity);
        readAttribute (attributeFound, dp, "do_comingfrom",
                       dataObject.comingFromValue);
    }

    if (!attributeFound["do_type"])
    {
        TASE2PivotUtility::log_error ("%s Missing do_type",
                                      beforeLog.c_str ());
        return nullptr;
    }

    if (dataObject.comingFromValue != "TASE2")
    {
        TASE2PivotUtility::log_warn (
            "%s data_object for %s is not from TASE.2 plugin -> ignore",
            beforeLog.c_str (), exchangeConfig->getLabel ().c_str ());
        return nullptr;
    }
    if (!checkTypeMatch (dataObject.doType, exchangeConfig))
    {
        TASE2PivotUtility::log_warn ("%s Input type (%s) does not match "
                                     "configured type (%s) for label %s",
                                     beforeLog.c_str (),
                                     dataObject.doType.c_str (),
                                     exchangeConfig->getTypeId ().c_str (),
                                     exchangeConfig->getLabel ().c_str ());
        return nullptr;
    }

    if (!attributeFound["do_ts"] && isTimestampType (dataObject.doType))
    {
        TASE2PivotUtility::log_warn (
            "%s Data object has type with timestamp (%s), but no "
            "timestamp was received",
            beforeLog.c_str (), dataObject.doType.c_str ());
    }

    std::string pivotType = exchangeConfig->getPivotId ();

    if (dataObject.doType == "State" || dataObject.doType == "StateQ"
        || dataObject.doType == "StateQTime"
        || dataObject.doType == "StateQTimeExt"
        || dataObject.doType == "StateSupplemental"
        || dataObject.doType == "StateSupplementalQ"
        || dataObject.doType == "StateSupplementalQTime"
        || dataObject.doType == "StateSupplementalQTimeExt")
    {

        // Pivot conversion
        PivotDataObject pivot ("GTIS", pivotType);

        pivot.setIdentifier (exchangeConfig->getPivotId ());

        if (attributeFound["do_value"] && dataObject.doValue != nullptr)
        {
            bool spsValue = false;
            if (dataObject.doValue->getData ().getType ()
                == DatapointValue::T_INTEGER)
            {
                int value = static_cast<int> (
                    dataObject.doValue->getData ().toInt ());

                if (pivotType == "SpsTyp")
                {
                    checkValueRange (beforeLog, value, 0, 1, "SP");
                    spsValue = (value > 0);
                    pivot.setStVal (spsValue);
                }
                else if (pivotType == "DpsTyp")
                {
                    int dpsValue = static_cast<int> (
                        dataObject.doValue->getData ().toInt ());
                    checkValueRange (beforeLog, dpsValue, 0, 3, "DP");

                    if (dpsValue == 0)
                    {
                        pivot.setStValStr ("intermediate-state");
                    }
                    else if (dpsValue == 1)
                    {
                        pivot.setStValStr ("off");
                    }
                    else if (dpsValue == 2)
                    {
                        pivot.setStValStr ("on");
                    }
                    else
                    {
                        pivot.setStValStr ("bad-state");
                    }
                }
                else
                {
                    TASE2PivotUtility::log_error (
                        "Invalid Pivot Type %s, Original Type -> %s",
                        pivotType.c_str (),
                        exchangeConfig->getTypeId ().c_str ());
                }
            }
        }

        pivot.addQuality (dataObject.doValidity, dataObject.doCurrentSource,
                          dataObject.doNormalValue);

        appendTimestampDataObject (pivot, attributeFound["do_ts"],
                                   dataObject.doTs, dataObject.doTsValidity);

        convertedDatapoint = pivot.toDatapoint ();
    }
    else if (dataObject.doType == "Discrete"
             || dataObject.doType == "DiscreteQ"
             || dataObject.doType == "DiscreteQTime"
             || dataObject.doType == "DiscreteQTimeExt")
    {

        // Pivot conversion
        PivotDataObject pivot ("GTIS", pivotType);

        pivot.setIdentifier (exchangeConfig->getPivotId ());

        if (attributeFound["do_value"] && dataObject.doValue != nullptr)
        {
            if (pivotType != "InsTyp" && pivotType != "EnsTyp")
            {
                TASE2PivotUtility::log_error (
                    "Invalid Pivot Type %s, Original Type -> %s",
                    pivotType.c_str (), exchangeConfig->getTypeId ().c_str ());
                return nullptr;
            }
            if (dataObject.doValue->getData ().getType ()
                == DatapointValue::T_INTEGER)
            {
                int intValue = static_cast<int> (
                    dataObject.doValue->getData ().toInt ());

                pivot.setStVal (intValue);
            }
        }

        pivot.addQuality (dataObject.doValidity, dataObject.doCurrentSource,
                          dataObject.doNormalValue);

        appendTimestampDataObject (pivot, attributeFound["do_ts"],
                                   dataObject.doTs, dataObject.doTsValidity);

        convertedDatapoint = pivot.toDatapoint ();
    }
    else if (dataObject.doType == "Real" || dataObject.doType == "RealQ"
             || dataObject.doType == "RealQTime"
             || dataObject.doType == "RealQTimeExt")
    {
        // Pivot conversion
        PivotDataObject pivot ("GTIM", "MvTyp");

        pivot.setIdentifier (exchangeConfig->getPivotId ());

        if (attributeFound["do_value"] && dataObject.doValue != nullptr)
        {
            if (pivotType != "MvTyp")
            {
                TASE2PivotUtility::log_error (
                    "Invalid Pivot Type %s, Original Type -> %s",
                    pivotType.c_str (), exchangeConfig->getTypeId ().c_str ());
                return nullptr;
            }
            if (dataObject.doValue->getData ().getType ()
                == DatapointValue::T_FLOAT)
            {
                double value = dataObject.doValue->getData ().toDouble ();
                pivot.setMagF (static_cast<float> (value));
            }
        }

        pivot.addQuality (dataObject.doValidity, dataObject.doCurrentSource,
                          dataObject.doNormalValue);

        appendTimestampDataObject (pivot, attributeFound["do_ts"],
                                   dataObject.doTs, dataObject.doTsValidity);

        convertedDatapoint = pivot.toDatapoint ();
    }
    else
    {
        TASE2PivotUtility::log_warn ("%s Unknown do_type: %s -> ignore",
                                     beforeLog.c_str (),
                                     dataObject.doType.c_str ());
    }

    return convertedDatapoint;
}

Datapoint*
TASE2PivotFilter::convertOperationObjectToPivot (
    std::vector<Datapoint*> datapoints)
{
    std::string beforeLog
        = TASE2PivotUtility::PluginName
          + " - TASE2PivotFilter::convertOperationObjectToPivot -";

    Datapoint* convertedDatapoint = nullptr;
    std::map<std::string, bool> attributeFound
        = { { "co_scope", false },  { "co_name", false },
            { "co_domain", false }, { "co_type", false },
            { "co_value", false },  { "co_ts", false },
            { "co_se", false },     { "co_comingfrom", false } };

    TASE2CommandObject commandObject;

    for (Datapoint* dp : datapoints)
    {
        readAttribute (attributeFound, dp, "co_scope", commandObject.coScope);
        readAttribute (attributeFound, dp, "co_domain",
                       commandObject.coDomain);
        readAttribute (attributeFound, dp, "co_name", commandObject.coName);
        readAttribute (attributeFound, dp, "co_type", commandObject.coType);
        readAttribute (attributeFound, dp, "co_value", commandObject.coValue);
        readAttribute (attributeFound, dp, "co_ts", commandObject.coTs);
        readAttribute (attributeFound, dp, "co_se", commandObject.coSe);
        readAttribute (attributeFound, dp, "co_comingfrom",
                       commandObject.comingFromValue);
    }

    if (attributeFound["co_ts"] && commandObject.coTs == 0)
    {
        attributeFound["co_ts"] = false;
    }

    if (!attributeFound["co_scope"])
    {
        TASE2PivotUtility::log_error ("%s Missing co_scope",
                                      beforeLog.c_str ());
        return nullptr;
    }

    if (commandObject.coScope != "vcc" && !attributeFound["co_domain"])
    {
        TASE2PivotUtility::log_error ("%s Missing co_domain",
                                      beforeLog.c_str ());
        return nullptr;
    }
    if (!attributeFound["co_name"])
    {
        TASE2PivotUtility::log_error ("%s Missing co_name",
                                      beforeLog.c_str ());
        return nullptr;
    }

    std::string domain
        = commandObject.coScope == "vcc" ? "vcc" : commandObject.coDomain;
    std::string address (domain + ":" + commandObject.coName);
    TASE2PivotDataPoint* exchangeConfig
        = m_config.getExchangeDefinitionsByAddress (address);

    if (!exchangeConfig)
    {
        TASE2PivotUtility::log_error (
            "%s Domain (%s) and Name (%s) not found in exchange data",
            beforeLog.c_str (), commandObject.coDomain.c_str (),
            commandObject.coName.c_str ());
        return nullptr;
    }

    if (!attributeFound["co_type"])
    {
        TASE2PivotUtility::log_error ("%s Missing co_type",
                                      beforeLog.c_str ());
        return nullptr;
    }

    if (!checkTypeMatch (commandObject.coType, exchangeConfig))
    {
        TASE2PivotUtility::log_warn (
            "%s Input type (%s) does not match configured type (%s) for "
            "address %s",
            beforeLog.c_str (), commandObject.coType.c_str (),
            exchangeConfig->getTypeId ().c_str (), address.c_str ());
        return nullptr;
    }

    if (commandObject.comingFromValue != "tase2")
    {
        TASE2PivotUtility::log_warn (
            "%s data_object for %s is not from TASE.2 plugin -> ignore",
            beforeLog.c_str (), exchangeConfig->getLabel ().c_str ());
        return nullptr;
    }

    if (!attributeFound["co_ts"] && isTimestampType (commandObject.coType))
    {
        TASE2PivotUtility::log_error (
            "%s Command has type with timestamp (%s), but no timestamp "
            "was received -> ignore",
            beforeLog.c_str (), commandObject.coType.c_str ());
        return nullptr;
    }

    std::string pivotType = exchangeConfig->getPivotType ();

    if (commandObject.coType == "Command")
    {
        PivotOperationObject pivot ("GTIC", pivotType);

        pivot.setIdentifier (exchangeConfig->getPivotId ());
        if (attributeFound["co_se"])
            pivot.setSelect (commandObject.coSe);

        if (attributeFound["co_value"] && commandObject.coValue != nullptr)
        {
            if (pivotType == "SpcTyp")
            {
                bool spsValue = false;
                if (commandObject.coValue->getData ().getType ()
                    == DatapointValue::T_INTEGER)
                {
                    bool value = commandObject.coValue->getData ().toInt ();
                    checkValueRange (beforeLog, value, 0, 1, "SC");
                    spsValue = (value > 0);
                }

                pivot.setCtlValBool (spsValue);
            }
            else if (pivotType == "DpcTyp")
            {
                int dpsValue = commandObject.coValue->getData ().toInt ();

                checkValueRange (beforeLog, dpsValue, 0, 3, "DC");

                if (dpsValue == 0)
                {
                    pivot.setCtlValStr ("intermediate-state");
                }
                else if (dpsValue == 1)
                {
                    pivot.setCtlValStr ("off");
                }
                else if (dpsValue == 2)
                {
                    pivot.setCtlValStr ("on");
                }
                else
                {
                    pivot.setCtlValStr ("bad-state");
                }
            }
            else if (pivotType == "IncTyp")
            {
                int incValue = commandObject.coValue->getData ().toInt ();

                pivot.setCtlValI (incValue);
            }
        }

        appendTimestampOperationObject (pivot, attributeFound["co_ts"],
                                        commandObject.coTs);

        convertedDatapoint = pivot.toDatapoint ();
    }
    else if (commandObject.coType == "SetPointReal")
    {
        PivotOperationObject pivot ("GTIC", pivotType);

        pivot.setIdentifier (exchangeConfig->getPivotId ());
        if (attributeFound["co_se"])
            pivot.setSelect (commandObject.coSe);

        if (attributeFound["co_value"] && commandObject.coValue != nullptr)
        {

            if (pivotType == "ApcTyp" || pivotType == "AsgTyp")
            {
                double value = commandObject.coValue->getData ().toDouble ();
                float fValue = static_cast<float> (value);

                if (static_cast<double> (fValue) != value)
                {
                    TASE2PivotUtility::log_warn (
                        "%s do_value out of range (float) for SE floating: %f",
                        beforeLog.c_str (), value);
                }

                pivot.setCtlValF (fValue);
            }

            else if (pivotType == "IncTyp")
            {
                int incValue = commandObject.coValue->getData ().toInt ();

                pivot.setCtlValI (incValue);
            }
        }

        appendTimestampOperationObject (pivot, attributeFound["co_ts"],
                                        commandObject.coTs);

        convertedDatapoint = pivot.toDatapoint ();
    }
    else if (commandObject.coType == "SetPointDiscrete")
    {
        PivotOperationObject pivot ("GTIC", pivotType);

        pivot.setIdentifier (exchangeConfig->getPivotId ());
        if (attributeFound["co_se"])
            pivot.setSelect (commandObject.coSe);

        if (attributeFound["co_value"] && commandObject.coValue != nullptr)
        {
            if (pivotType == "IncTyp" || pivotType == "IngTyp")
            {
                int intValue = commandObject.coValue->getData ().toInt ();

                pivot.setCtlValI (intValue);
            }
        }

        appendTimestampOperationObject (pivot, attributeFound["co_ts"],
                                        commandObject.coTs);

        convertedDatapoint = pivot.toDatapoint ();
    }

    return convertedDatapoint;
}

Datapoint*
TASE2PivotFilter::convertDatapointToTASE2DataObject (
    Datapoint* sourceDp, TASE2PivotDataPoint* exchangeConfig)
{
    std::string beforeLog
        = TASE2PivotUtility::PluginName
          + " - TASE2PivotFilter::convertDatapointToTASE2DataObject -";
    Datapoint* convertedDatapoint = nullptr;

    try
    {
        PivotDataObject pivotObject (sourceDp);

        convertedDatapoint = pivotObject.toTASE2DataObject (exchangeConfig);
    }
    catch (PivotObjectException& e)
    {
        TASE2PivotUtility::log_error ("%s Failed to convert pivot object: %s",
                                      beforeLog.c_str (),
                                      e.getContext ().c_str ());
    }

    return convertedDatapoint;
}

std::vector<Datapoint*>
TASE2PivotFilter::convertReadingToTASE2OperationObject (Datapoint* sourceDp)
{
    std::string beforeLog
        = TASE2PivotUtility::PluginName
          + " - TASE2PivotFilter::convertReadingToTASE2OperationObject -";
    std::vector<Datapoint*> convertedDatapoints;

    try
    {
        PivotOperationObject pivotOperationObject (sourceDp);
        const std::string& pivotId = pivotOperationObject.getIdentifier ();
        TASE2PivotDataPoint* exchangeConfig
            = m_config.getExchangeDefinitionsByPivotId (pivotId);

        if (!exchangeConfig)
        {
            TASE2PivotUtility::log_error (
                "%s Pivot ID not in exchangedData: %s", beforeLog.c_str (),
                pivotId.c_str ());
        }
        else
        {
            convertedDatapoints
                = pivotOperationObject.toTASE2OperationObject (exchangeConfig);
        }
    }
    catch (PivotObjectException& e)
    {
        TASE2PivotUtility::log_error (
            "%s Failed to convert pivot operation object: %s",
            beforeLog.c_str (), e.getContext ().c_str ());
    }

    return convertedDatapoints;
}

void
TASE2PivotFilter::ingest (READINGSET* readingSet)
{
    std::string beforeLog
        = TASE2PivotUtility::PluginName + " - TASE2PivotFilter::ingest -";
    /* apply transformation */
    std::vector<Reading*>* readings = readingSet->getAllReadingsPtr ();

    std::vector<Reading*>::iterator readIt = readings->begin ();

    while (readIt != readings->end ())
    {
        Reading* reading = *readIt;

        std::string assetName = reading->getAssetName ();

        std::vector<Datapoint*>& datapoints = reading->getReadingData ();

        std::vector<Datapoint*> convertedDatapoints;

        TASE2PivotUtility::log_debug ("%s original Reading: (%s)",
                                      beforeLog.c_str (),
                                      reading->toJSON ().c_str ());

        if (assetName == "TASE2Command")
        {
            Datapoint* convertedOperation
                = convertOperationObjectToPivot (datapoints);

            if (!convertedOperation)
            {
                TASE2PivotUtility::log_error (
                    "%s Failed to convert TASE2 command object",
                    beforeLog.c_str ());
            }
            else
            {
                convertedDatapoints.push_back (convertedOperation);
            }

            reading->setAssetName ("PivotCommand");
        }

        else if (assetName == "PivotCommand")
        {
            std::vector<Datapoint*> convertedReadingDatapoints
                = convertReadingToTASE2OperationObject (datapoints[0]);

            if (convertedReadingDatapoints.empty ())
            {
                TASE2PivotUtility::log_error (
                    "%s Failed to convert Pivot operation object",
                    beforeLog.c_str ());
            }

            for (Datapoint* dp : convertedReadingDatapoints)
                convertedDatapoints.push_back (dp);

            reading->setAssetName ("TASE2Command");
        }

        else
        {
            TASE2PivotDataPoint* exchangeConfig
                = m_config.getExchangeDefinitionsByLabel (assetName);

            if (exchangeConfig)
            {
                for (Datapoint* dp : datapoints)
                {
                    if (dp->getName () == "data_object")
                    {
                        Datapoint* convertedDp
                            = convertDataObjectToPivot (dp, exchangeConfig);

                        if (convertedDp)
                        {
                            convertedDatapoints.push_back (convertedDp);
                        }
                        else
                        {
                            TASE2PivotUtility::log_error (
                                "%s Failed to convert object",
                                beforeLog.c_str ());
                        }
                    }

                    else if (dp->getName () == "PIVOT")
                    {
                        Datapoint* convertedDp
                            = convertDatapointToTASE2DataObject (
                                dp, exchangeConfig);

                        if (convertedDp)
                        {
                            convertedDatapoints.push_back (convertedDp);
                        }
                    }
                    else
                    {
                        TASE2PivotUtility::log_debug (
                            "%s Unhandled datapoint type '%s', forwarding "
                            "reading unchanged",
                            beforeLog.c_str (), dp->getName ().c_str ());
                        Datapoint* dpCopy
                            = new Datapoint (dp->getName (), dp->getData ());
                        convertedDatapoints.push_back (dpCopy);
                    }
                }
            }
            else
            {
                TASE2PivotUtility::log_debug (
                    "%s Asset '%s' not found in exchangedData, forwarding "
                    "reading unchanged",
                    beforeLog.c_str (), assetName.c_str ());
                for (Datapoint* dp : datapoints)
                {
                    Datapoint* dpCopy
                        = new Datapoint (dp->getName (), dp->getData ());
                    convertedDatapoints.push_back (dpCopy);
                }
            }
        }

        reading->removeAllDatapoints ();

        for (Datapoint* convertedDatapoint : convertedDatapoints)
        {
            reading->addDatapoint (convertedDatapoint);
        }

        TASE2PivotUtility::log_debug ("%s converted Reading: (%s)",
                                      beforeLog.c_str (),
                                      reading->toJSON ().c_str ());

        if (reading->getReadingData ().size () == 0)
        {
            readIt = readings->erase (readIt);
        }
        else
        {
            readIt++;
        }
    }

    if (readings->empty () == false)
    {
        if (m_output)
        {
            TASE2PivotUtility::log_debug ("%s Send %lu converted readings",
                                          beforeLog.c_str (),
                                          readings->size ());

            m_output (m_outHandle, readingSet);
        }
        else
        {
            TASE2PivotUtility::log_error (
                "%s No function to call, discard %lu converted readings",
                beforeLog.c_str (), readings->size ());
        }
    }
}

void
TASE2PivotFilter::reconfigure (ConfigCategory* config)
{
    std::string beforeLog
        = TASE2PivotUtility::PluginName + " - TASE2PivotFilter::reconfigure -";
    TASE2PivotUtility::log_debug ("%s (re)configure called",
                                  beforeLog.c_str ());

    if (config)
    {
        if (config->itemExists ("exchanged_data"))
        {
            const std::string exchangedData
                = config->getValue ("exchanged_data");

            m_config.importExchangeConfig (exchangedData);
        }
        else
        {
            TASE2PivotUtility::log_error (
                "%s Missing exchanged_data configuation", beforeLog.c_str ());
        }
    }
    else
    {
        TASE2PivotUtility::log_error ("%s No configuration provided",
                                      beforeLog.c_str ());
    }
}

bool
TASE2PivotFilter::isTimestampType (const std::string& type)
{
    return type.find ("Time") != std::string::npos;
}