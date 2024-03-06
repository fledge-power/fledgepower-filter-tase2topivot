/*
 * FledgePower TASE.2 <-> pivot filter plugin.
 *
 * Copyright (c) 2023, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *

 *
 */

#include <datapoint.h>
#include <sys/time.h>

#include "tase2_pivot_filter_config.hpp"
#include "tase2_pivot_object.hpp"
#include "tase2_pivot_utility.hpp"

static Datapoint*
createDp (const std::string& name)
{
    vector<Datapoint*>* datapoints = new vector<Datapoint*>;

    DatapointValue dpv (datapoints, true);

    Datapoint* dp = new Datapoint (name, dpv);

    return dp;
}

template <class T>
static Datapoint*
createDpWithValue (const std::string& name, const T value)
{
    DatapointValue dpv (value);

    Datapoint* dp = new Datapoint (name, dpv);

    return dp;
}

static Datapoint*
addElement (Datapoint* dp, const std::string& name)
{
    DatapointValue& dpv = dp->getData ();

    std::vector<Datapoint*>* subDatapoints = dpv.getDpVec ();

    Datapoint* element = createDp (name);

    if (element)
    {
        subDatapoints->push_back (element);
    }

    return element;
}

template <class T>
static Datapoint*
addElementWithValue (Datapoint* dp, const std::string& name, const T value)
{
    DatapointValue& dpv = dp->getData ();

    std::vector<Datapoint*>* subDatapoints = dpv.getDpVec ();

    Datapoint* element = createDpWithValue (name, value);

    if (element)
    {
        subDatapoints->push_back (element);
    }

    return element;
}

static Datapoint*
getChild (Datapoint* dp, const std::string& name)
{
    Datapoint* childDp = nullptr;

    DatapointValue& dpv = dp->getData ();

    if (dpv.getType () == DatapointValue::T_DP_DICT)
    {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec ();

        for (Datapoint* child : *datapoints)
        {
            if (child->getName () == name)
            {
                childDp = child;
                break;
            }
        }
    }

    return childDp;
}

static const string
getValueStr (Datapoint* dp)
{
    DatapointValue& dpv = dp->getData ();

    if (dpv.getType () == DatapointValue::T_STRING)
    {
        return dpv.toStringValue ();
    }
    else
    {
        throw PivotObjectException ("datapoint " + dp->getName ()
                                    + " has not a string value");
    }
}

static const string
getChildValueStr (Datapoint* dp, const std::string& name)
{
    Datapoint* childDp = getChild (dp, name);

    if (childDp)
    {
        return getValueStr (childDp);
    }
    else
    {
        throw PivotObjectException ("No such child: " + name);
    }
}

static long
getValueInt (Datapoint* dp)
{
    DatapointValue& dpv = dp->getData ();

    if (dpv.getType () == DatapointValue::T_INTEGER)
    {
        return dpv.toInt ();
    }
    else
    {
        throw PivotObjectException ("datapoint " + dp->getName ()
                                    + " has not an int value");
    }
}

static int
getChildValueInt (Datapoint* dp, const std::string& name)
{
    Datapoint* childDp = getChild (dp, name);

    if (childDp)
    {
        return getValueInt (childDp);
    }
    else
    {
        throw PivotObjectException ("No such child: " + name);
    }
}

static float
getValueFloat (Datapoint* dp)
{
    DatapointValue& dpv = dp->getData ();

    if (dpv.getType () == DatapointValue::T_FLOAT)
    {
        return (float)dpv.toDouble ();
    }
    else
    {
        throw PivotObjectException ("datapoint " + dp->getName ()
                                    + " has not a float value");
    }
}

void
PivotTimestamp::setTimeInMs (long ms)
{
    uint32_t timeval32 = (uint32_t)(ms / 1000LL);

    m_valueArray[0] = (timeval32 / 0x1000000 & 0xff);
    m_valueArray[1] = (timeval32 / 0x10000 & 0xff);
    m_valueArray[2] = (timeval32 / 0x100 & 0xff);
    m_valueArray[3] = (timeval32 & 0xff);

    uint32_t remainder = (ms % 1000LL);
    uint32_t fractionOfSecond
        = (remainder) * 16777 + ((remainder * 216) / 1000);

    m_valueArray[4] = ((fractionOfSecond >> 16) & 0xff);
    m_valueArray[5] = ((fractionOfSecond >> 8) & 0xff);
    m_valueArray[6] = (fractionOfSecond & 0xff);
}

uint64_t
PivotTimestamp::getTimeInMs ()
{
    uint32_t timeval32;

    timeval32 = m_valueArray[3];
    timeval32 += m_valueArray[2] * 0x100;
    timeval32 += m_valueArray[1] * 0x10000;
    timeval32 += m_valueArray[0] * 0x1000000;

    uint32_t fractionOfSecond = 0;

    fractionOfSecond = (m_valueArray[4] << 16);
    fractionOfSecond += (m_valueArray[5] << 8);
    fractionOfSecond += (m_valueArray[6]);

    uint32_t remainder = fractionOfSecond / 16777;

    uint64_t msVal = (timeval32 * 1000LL) + remainder;

    return (uint64_t)msVal;
}

int
PivotTimestamp::FractionOfSecond ()
{
    uint32_t fractionOfSecond = 0;

    fractionOfSecond = (m_valueArray[4] << 16);
    fractionOfSecond += (m_valueArray[5] << 8);
    fractionOfSecond += (m_valueArray[6]);

    return fractionOfSecond;
}

int
PivotTimestamp::SecondSinceEpoch ()
{
    int32_t timeval32;

    timeval32 = m_valueArray[3];
    timeval32 += m_valueArray[2] * 0x100;
    timeval32 += m_valueArray[1] * 0x10000;
    timeval32 += m_valueArray[0] * 0x1000000;

    return timeval32;
}

void
PivotTimestamp::handleTimeQuality (Datapoint* timeQuality)
{
    DatapointValue& dpv = timeQuality->getData ();

    if (dpv.getType () == DatapointValue::T_DP_DICT)
    {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec ();

        for (Datapoint* child : *datapoints)
        {
            if (child->getName () == "clockFailure")
            {
                if (getValueInt (child) > 0)
                    m_clockFailure = true;
                else
                    m_clockFailure = false;
            }
            else if (child->getName () == "clockNotSynchronized")
            {
                if (getValueInt (child) > 0)
                    m_clockNotSynchronized = true;
                else
                    m_clockNotSynchronized = false;
            }
            else if (child->getName () == "leapSecondKnown")
            {
                if (getValueInt (child) > 0)
                    m_leapSecondKnown = true;
                else
                    m_leapSecondKnown = false;
            }
            else if (child->getName () == "timeAccuracy")
            {
                m_timeAccuracy = getValueInt (child);
            }
        }
    }
}

PivotTimestamp::PivotTimestamp (Datapoint* timestampData)
{
    DatapointValue& dpv = timestampData->getData ();
    m_valueArray = new uint8_t[7];

    if (dpv.getType () == DatapointValue::T_DP_DICT)
    {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec ();

        for (Datapoint* child : *datapoints)
        {
            if (child->getName () == "SecondSinceEpoch")
            {
                uint32_t secondSinceEpoch = getValueInt (child);

                m_valueArray[0] = (secondSinceEpoch / 0x1000000 & 0xff);
                m_valueArray[1] = (secondSinceEpoch / 0x10000 & 0xff);
                m_valueArray[2] = (secondSinceEpoch / 0x100 & 0xff);
                m_valueArray[3] = (secondSinceEpoch & 0xff);
            }
            else if (child->getName () == "FractionOfSecond")
            {
                uint32_t fractionOfSecond = getValueInt (child);

                m_valueArray[4] = ((fractionOfSecond >> 16) & 0xff);
                m_valueArray[5] = ((fractionOfSecond >> 8) & 0xff);
                m_valueArray[6] = (fractionOfSecond & 0xff);
            }
            else if (child->getName () == "TimeQuality")
            {
                handleTimeQuality (child);
            }
        }
    }
}

PivotTimestamp::PivotTimestamp (long ms)
{
    m_valueArray = new uint8_t[7];
    uint32_t timeval32 = (uint32_t)(ms / 1000LL);

    m_valueArray[0] = (timeval32 / 0x1000000 & 0xff);
    m_valueArray[1] = (timeval32 / 0x10000 & 0xff);
    m_valueArray[2] = (timeval32 / 0x100 & 0xff);
    m_valueArray[3] = (timeval32 & 0xff);

    uint32_t remainder = (ms % 1000LL);
    uint32_t fractionOfSecond
        = (remainder) * 16777 + ((remainder * 216) / 1000);

    m_valueArray[4] = ((fractionOfSecond >> 16) & 0xff);
    m_valueArray[5] = ((fractionOfSecond >> 8) & 0xff);
    m_valueArray[6] = (fractionOfSecond & 0xff);
}

PivotTimestamp::~PivotTimestamp () { delete[] m_valueArray; }

uint64_t
PivotTimestamp::GetCurrentTimeInMs ()
{
    struct timeval now;

    gettimeofday (&now, nullptr);

    return ((uint64_t)now.tv_sec * 1000LL) + (now.tv_usec / 1000);
}

Datapoint*
PivotObject::getCdc (Datapoint* dp)
{
    Datapoint* cdcDp = nullptr;
    std::vector<std::string> unknownChildrenNames;

    DatapointValue& dpv = dp->getData ();
    if (dpv.getType () != DatapointValue::T_DP_DICT)
    {
        throw PivotObjectException ("CDC type missing");
    }

    std::vector<Datapoint*>* datapoints = dpv.getDpVec ();
    for (Datapoint* child : *datapoints)
    {
        if (child->getName () == "SpsTyp")
        {
            cdcDp = child;
            m_pivotCdc = PivotCdc::SPS;
            break;
        }
        else if (child->getName () == "MvTyp")
        {
            cdcDp = child;
            m_pivotCdc = PivotCdc::MV;
            break;
        }
        else if (child->getName () == "DpsTyp")
        {
            cdcDp = child;
            m_pivotCdc = PivotCdc::DPS;
            break;
        }
        else if (child->getName () == "SpcTyp")
        {
            cdcDp = child;
            m_pivotCdc = PivotCdc::SPC;
            break;
        }
        else if (child->getName () == "DpcTyp")
        {
            cdcDp = child;
            m_pivotCdc = PivotCdc::DPC;
            break;
        }
        else if (child->getName () == "IncTyp")
        {
            cdcDp = child;
            m_pivotCdc = PivotCdc::INC;
            break;
        }
        else if (child->getName () == "ApcTyp")
        {
            cdcDp = child;
            m_pivotCdc = PivotCdc::APC;
            break;
        }
        else if (child->getName () == "IngTyp")
        {
            cdcDp = child;
            m_pivotCdc = PivotCdc::ING;
            break;
        }
        else if (child->getName () == "AsgTyp")
        {
            cdcDp = child;
            m_pivotCdc = PivotCdc::ASG;
            break;
        }
        else
        {
            unknownChildrenNames.push_back (child->getName ());
        }
        if (cdcDp != nullptr)
        {
            break;
        }
    }
    if (cdcDp == nullptr)
    {
        throw PivotObjectException (
            "CDC type unknown: "
            + TASE2PivotUtility::join (unknownChildrenNames));
    }

    return cdcDp;
}

void
PivotDataObject::handleDetailQuality (Datapoint* detailQuality)
{
    DatapointValue& dpv = detailQuality->getData ();

    if (dpv.getType () == DatapointValue::T_DP_DICT)
    {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec ();

        for (Datapoint* child : *datapoints)
        {
            if (child->getName () == "badReference")
            {
                if (getValueInt (child) > 0)
                    m_badReference = true;
                else
                    m_badReference = false;
            }
            else if (child->getName () == "failure")
            {
                if (getValueInt (child) > 0)
                    m_failure = true;
                else
                    m_failure = false;
            }
            else if (child->getName () == "inconsistent")
            {
                if (getValueInt (child) > 0)
                    m_inconsistent = true;
                else
                    m_inconsistent = false;
            }
            else if (child->getName () == "inacurate")
            {
                if (getValueInt (child) > 0)
                    m_inacurate = true;
                else
                    m_inacurate = false;
            }
            else if (child->getName () == "oldData")
            {
                if (getValueInt (child) > 0)
                    m_oldData = true;
                else
                    m_oldData = false;
            }
            else if (child->getName () == "oscillatory")
            {
                if (getValueInt (child) > 0)
                    m_oscillatory = true;
                else
                    m_oscillatory = false;
            }
            else if (child->getName () == "outOfRange")
            {
                if (getValueInt (child) > 0)
                    m_outOfRange = true;
                else
                    m_outOfRange = false;
            }
            else if (child->getName () == "overflow")
            {
                if (getValueInt (child) > 0)
                    m_overflow = true;
                else
                    m_overflow = false;
            }
        }
    }
}

void
PivotDataObject::handleQuality (Datapoint* q)
{
    DatapointValue& dpv = q->getData ();

    if (dpv.getType () == DatapointValue::T_DP_DICT)
    {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec ();

        for (Datapoint* child : *datapoints)
        {
            if (child->getName () == "Validity")
            {
                string validityStr = getValueStr (child);

                if (validityStr != "good")
                {
                    if (validityStr == "invalid")
                    {
                        m_validity = Validity::INVALID;
                    }
                    else if (validityStr == "questionable")
                    {
                        m_validity = Validity::QUESTIONABLE;
                    }
                    else if (validityStr == "reserved")
                    {
                        m_validity = Validity::RESERVED;
                    }
                    else
                    {
                        throw PivotObjectException (
                            "Validity has invalid value " + validityStr);
                    }
                }
            }
        }
    }
}

PivotDataObject::PivotDataObject (Datapoint* pivotData)
{
    if (pivotData->getName () != "PIVOT")
    {
        throw PivotObjectException ("No pivot object");
    }

    m_dp = pivotData;
    m_ln = nullptr;
    std::vector<std::string> unknownChildrenNames;

    Datapoint* childDp = nullptr;

    DatapointValue& dpv = pivotData->getData ();
    if (dpv.getType () != DatapointValue::T_DP_DICT)
    {
        throw PivotObjectException ("pivot object not found");
    }

    std::vector<Datapoint*>* datapoints = dpv.getDpVec ();
    for (Datapoint* child : *datapoints)
    {
        if (child->getName () == "GTIS")
        {
            m_pivotClass = PivotClass::GTIS;
            m_ln = child;
            break;
        }
        else if (child->getName () == "GTIM")
        {
            m_pivotClass = PivotClass::GTIM;
            m_ln = child;
            break;
        }
        else if (child->getName () == "GTIC")
        {
            m_pivotClass = PivotClass::GTIC;
            m_ln = child;
            break;
        }
        else
        {
            unknownChildrenNames.push_back (child->getName ());
        }
    }

    if (m_ln == nullptr)
    {
        throw PivotObjectException (
            "pivot object type not supported: "
            + TASE2PivotUtility::join (unknownChildrenNames));
    }

    m_identifier = getChildValueStr (m_ln, "Identifier");

    Datapoint* comingFromDp = getChild (m_ln, "ComingFrom");

    if (comingFromDp)
    {
        m_comingFrom = getChildValueStr (m_ln, "ComingFrom");
    }

    Datapoint* tmValidity = getChild (m_ln, "TmValidity");

    if (tmValidity)
    {
        string tmValidityValue = getChildValueStr (tmValidity, "stVal");

        if (tmValidityValue == "invalid")
        {
            m_timestampInvalid = true;
        }
        else
        {
            m_timestampInvalid = false;
        }
    }

    Datapoint* cdc = getCdc (m_ln);

    if (cdc)
    {
        Datapoint* q = getChild (cdc, "q");

        if (q)
        {
            handleQuality (q);
        }

        Datapoint* t = getChild (cdc, "t");

        if (t)
        {
            m_timestamp = new PivotTimestamp (t);
        }

        switch (m_pivotCdc)
        {
        case PivotCdc::INS:
        case PivotCdc::ENS: {
            Datapoint* stVal = getChild (cdc, "stVal");
            if (stVal)
            {
                hasIntVal = true;
                intVal = getValueInt (stVal);
            }
            break;
        }
        case PivotCdc::SPS:
        case PivotCdc::SPC: {
            Datapoint* stVal = getChild (
                cdc, (m_pivotCdc == PivotCdc::SPS) ? "stVal" : "ctlVal");
            if (stVal)
            {
                hasIntVal = true;
                intVal = getValueInt (stVal) > 0 ? 1 : 0;
            }
            break;
        }

        case PivotCdc::DPS:
        case PivotCdc::DPC: {
            Datapoint* stVal = getChild (
                cdc, (m_pivotCdc == PivotCdc::DPS) ? "stVal" : "ctlVal");
            if (stVal)
            {
                hasIntVal = true;
                string stValStr = getValueStr (stVal);

                if (stValStr == "intermediate-state")
                    intVal = 0;
                else if (stValStr == "off")
                    intVal = 1;
                else if (stValStr == "on")
                    intVal = 2;
                else if (stValStr == "bad-state")
                    intVal = 3;
            }
            break;
        }

        case PivotCdc::MV: {
            Datapoint* mag = getChild (cdc, "mag");
            if (mag)
            {
                Datapoint* mag_f = getChild (mag, "f");
                if (mag_f)
                {
                    hasIntVal = false;
                    floatVal = getValueFloat (mag_f);
                }
                else
                {
                    Datapoint* mag_i = getChild (mag, "i");
                    if (mag_i)
                    {
                        hasIntVal = true;
                        intVal = getValueInt (mag_i);
                    }
                }
            }
            break;
        }

        case PivotCdc::ASG:
        case PivotCdc::ING:
        case PivotCdc::INC:
        case PivotCdc::APC: {
            Datapoint* value = getChild (cdc, "ctlVal");
            if (value)
            {
                hasIntVal = (m_pivotCdc == PivotCdc::INC);
                if (hasIntVal)
                {
                    intVal = getValueInt (value);
                }
                else
                {
                    floatVal = getValueFloat (value);
                }
            }
            break;
        }

        default:
            throw PivotObjectException (
                "CDC element not found or CDC type unknown");
        }
    }
}

PivotDataObject::~PivotDataObject ()
{
    if (m_timestamp)
        delete m_timestamp;
}

PivotDataObject::PivotDataObject (const std::string& pivotLN,
                                  const std::string& valueType)
{
    m_dp = createDp ("PIVOT");

    m_ln = addElement (m_dp, pivotLN);

    addElementWithValue (m_ln, "ComingFrom", "tase2");

    m_cdc = addElement (m_ln, valueType);
}

PivotOperationObject::~PivotOperationObject ()
{
    if (m_timestamp)
        delete m_timestamp;
}

PivotOperationObject::PivotOperationObject (const std::string& pivotLN,
                                            const std::string& valueType)
{
    m_dp = createDp ("PIVOT");

    m_ln = addElement (m_dp, pivotLN);

    addElementWithValue (m_ln, "ComingFrom", "tase2");

    m_cdc = addElement (m_ln, valueType);
}

PivotOperationObject::PivotOperationObject (Datapoint* pivotData)
{
    if (pivotData->getName () != "PIVOT")
    {
        throw PivotObjectException ("No pivot object");
    }

    m_dp = pivotData;
    m_ln = nullptr;
    std::vector<std::string> unknownChildrenNames;

    Datapoint* childDp = nullptr;

    DatapointValue& dpv = pivotData->getData ();

    if (dpv.getType () == DatapointValue::T_DP_DICT)
    {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec ();

        for (Datapoint* child : *datapoints)
        {
            if (child->getName () == "GTIC")
            {
                m_pivotClass = PivotClass::GTIC;
                m_ln = child;
                break;
            }
            else
            {
                unknownChildrenNames.push_back (child->getName ());
            }
        }
    }

    if (m_ln == nullptr)
    {
        throw PivotObjectException (
            "pivot object type not supported: "
            + TASE2PivotUtility::join (unknownChildrenNames));
    }

    m_identifier = getChildValueStr (m_ln, "Identifier");

    Datapoint* comingFromDp = getChild (m_ln, "ComingFrom");

    if (comingFromDp)
    {
        m_comingFrom = getChildValueStr (m_ln, "ComingFrom");
    }

    Datapoint* confirmation = getChild (m_ln, "Confirmation");

    Datapoint* select = getChild (m_ln, "Select");

    if (select)
    {
        Datapoint* stVal = getChild (select, "stVal");
        if (stVal)
        {
            if (getValueInt (stVal) > 0)
            {
                m_select = 1;
            }
            else
            {
                m_select = 0;
            }
        }
    }

    Datapoint* cdc = getCdc (m_ln);

    if (cdc)
    {
        Datapoint* q = getChild (cdc, "q");

        if (q)
        {
            m_test = getChildValueInt (q, "test");
        }

        Datapoint* t = getChild (cdc, "t");

        if (t)
        {
            m_timestamp = new PivotTimestamp (t);
        }

        switch (m_pivotCdc)
        {
        case PivotCdc::SPC: {
            Datapoint* stVal = getChild (cdc, "ctlVal");
            if (stVal)
            {
                hasIntVal = true;
                intVal = getValueInt (stVal) > 0 ? 1 : 0;
            }
            break;
        }

        case PivotCdc::DPC: {
            Datapoint* stVal = getChild (cdc, "ctlVal");
            if (stVal)
            {
                hasIntVal = true;
                string stValStr = getValueStr (stVal);

                if (stValStr == "intermediate-state")
                    intVal = 0;
                else if (stValStr == "off")
                    intVal = 1;
                else if (stValStr == "on")
                    intVal = 2;
                else if (stValStr == "bad-state")
                    intVal = 3;
            }
            break;
        }

        case PivotCdc::ING:
        case PivotCdc::INC: {
            Datapoint* value = getChild (cdc, "ctlVal");
            if (value)
            {
                hasIntVal = true;
                intVal = getValueInt (value);
            }
            break;
        }

        case PivotCdc::ASG:
        case PivotCdc::APC: {
            Datapoint* value = getChild (cdc, "ctlVal");
            if (value)
            {
                hasIntVal = false;
                floatVal = getValueFloat (value);
            }
            break;
        }

        default:
            throw PivotObjectException ("CDC type unknown");
        }
    }
    else
    {
        throw PivotObjectException (
            "CDC element not found or CDC type unknown");
    }
}

void
PivotObject::setIdentifier (const std::string& identifier)
{
    addElementWithValue (m_ln, "Identifier", identifier);
}

void
PivotOperationObject::setSelect (int select)
{
    Datapoint* selectDp = addElement (m_ln, "Select");
    addElementWithValue (selectDp, "stVal", (long)select);
}

void
PivotObject::setTest (bool value)
{
    Datapoint* q = addElement (m_cdc, "q");

    addElementWithValue (q, "test", (long)value);
}

void
PivotObject::setCause (int cause)
{
    Datapoint* causeDp = addElement (m_ln, "Cause");

    addElementWithValue (causeDp, "stVal", (long)cause);
}

void
PivotDataObject::setStVal (bool value)
{
    addElementWithValue (m_cdc, "stVal", (long)(value ? 1 : 0));
}

void
PivotDataObject::setStValStr (const std::string& value)
{
    addElementWithValue (m_cdc, "stVal", value);
}

void
PivotObject::setCtlValBool (bool value)
{
    addElementWithValue (m_cdc, "ctlVal", (long)(value ? 1 : 0));
}

void
PivotObject::setCtlValStr (const std::string& value)
{
    addElementWithValue (m_cdc, "ctlVal", value);
}

void
PivotObject::setCtlValI (int value)
{
    addElementWithValue (m_cdc, "ctlVal", (long)value);
}

void
PivotObject::setCtlValF (float value)
{
    addElementWithValue (m_cdc, "ctlVal", (float)value);
}

void
PivotDataObject::setMagF (float value)
{
    Datapoint* mag = addElement (m_cdc, "mag");

    addElementWithValue (mag, "f", value);
}

void
PivotDataObject::setMagI (int value)
{
    Datapoint* mag = addElement (m_cdc, "mag");

    addElementWithValue (mag, "i", (long)value);
}

void
PivotDataObject::addQuality (const std::string& validity,
                             const std::string& cs, const std::string& nv)
{
    Datapoint* q = addElement (m_cdc, "q");

    bool abnormal = nv != "" && nv != "normal";

    if (abnormal)
    {
        Datapoint* dq = addElement (q, "DetailQuality");
        addElementWithValue (dq, "Failure", (long)true);
    }

    if (cs == "telemetered" || cs == "calculated")
    {
        m_source = Source::PROCESS;
        addElementWithValue (q, "Source", "process");
    }
    else if (cs == "entered" || cs == "estimated")
    {
        m_source = Source::SUBSTITUTED;
        addElementWithValue (q, "Source", "substituted");
    }

    if (validity == "valid" || validity == "")
    {
        m_validity = Validity::GOOD;
        addElementWithValue (q, "Validity", "good");
    }
    if (validity == "invalid")
    {
        m_validity = Validity::INVALID;
        addElementWithValue (q, "Validity", "invalid");
    }
    if (validity == "held")
    {
        m_validity = Validity::RESERVED;
        addElementWithValue (q, "Validity", "reserved");
    }
    if (validity == "suspect")
    {
        m_validity = Validity::QUESTIONABLE;
        addElementWithValue (q, "Validity", "questionable");
    }
}

void
PivotDataObject::addTimestamp (long ts, const std::string& tsValidity)
{
    Datapoint* t = addElement (m_cdc, "t");

    m_timestamp = new PivotTimestamp (ts);

    addElementWithValue (t, "SecondSinceEpoch",
                         (long)m_timestamp->SecondSinceEpoch ());
    addElementWithValue (t, "FractionOfSecond",
                         (long)m_timestamp->FractionOfSecond ());

    Datapoint* tmValidity = addElement (m_ln, "TmValidity");

    if (tmValidity)
    {
        if (tsValidity == "invalid")
        {
            m_timestampInvalid = true;
            addElementWithValue (tmValidity, "stVal", "invalid");
        }
        else
        {
            m_timestampInvalid = false;
            addElementWithValue (tmValidity, "stVal", "good");
        }
    }
}

void
PivotOperationObject::addTimestamp (long ts)
{
    Datapoint* t = addElement (m_cdc, "t");

    m_timestamp = new PivotTimestamp (ts);

    addElementWithValue (t, "SecondSinceEpoch",
                         (long)m_timestamp->SecondSinceEpoch ());
    addElementWithValue (t, "FractionOfSecond",
                         (long)m_timestamp->FractionOfSecond ());
}

Datapoint*
PivotDataObject::toTASE2DataObject (TASE2PivotDataPoint* exchangeConfig)
{
    Datapoint* dataObject = createDp ("data_object");

    if (dataObject)
    {
        addElementWithValue (dataObject, "do_type",
                             exchangeConfig->getTypeId ());
        addElementWithValue (dataObject, "do_domain",
                             (exchangeConfig->getDomain ()));
        addElementWithValue (dataObject, "do_name",
                             (exchangeConfig->getName ()));

        bool isQualityType
            = exchangeConfig->getTypeId ().find ("Q") != std::string::npos;

        std::string validity;

        switch (getValidity ())
        {
        case INVALID: {
            validity = "invalid";
            break;
        }
        case GOOD: {
            validity = "valid";
            break;
        }
        case RESERVED: {
            validity = "held";
            break;
        }
        case QUESTIONABLE: {
            validity = "suspect";
            break;
        }
        }

        if (isQualityType)
        {
            addElementWithValue (dataObject, "do_validity", validity);

            if (getSource () == Source::SUBSTITUTED)
            {
                addElementWithValue (dataObject, "do_cs", "estimated");
            }
            else if (getSource () == Source::PROCESS)
            {
                addElementWithValue (dataObject, "do_cs", "telemetered");
            }

            if (OldData () || Overflow () || OutOfRange () || BadReference ()
                || Oscillatory () || Failure () || Inconsistent ()
                || m_inacurate)
            {
                addElementWithValue (dataObject, "do_quality_normal_value",
                                     "abnormal");
            }

            else
            {
                addElementWithValue (dataObject, "do_quality_normal_value",
                                     "normal");
            }
        }

        if (hasIntVal)
        {
            addElementWithValue (dataObject, "do_value", intVal);
        }
        else
        {
            addElementWithValue (dataObject, "do_value", (double)floatVal);
        }

        if (m_timestamp)
        {
            addElementWithValue (
                dataObject, "do_ts",
                ((long)(uint64_t)m_timestamp->getTimeInMs ()));

            if (IsTimestampInvalid ())
            {
                addElementWithValue (dataObject, "do_ts_validity", "invalid");
            }
            else
            {
                addElementWithValue (dataObject, "do_ts_validity", "valid");
            }
        }
        else
        {
            addElementWithValue (dataObject, "do_ts",
                                 (long)PivotTimestamp::GetCurrentTimeInMs ());
            addElementWithValue (dataObject, "do_ts_validity", "valid");
        }
    }

    return dataObject;
}

std::vector<Datapoint*>
PivotOperationObject::toTASE2OperationObject (
    TASE2PivotDataPoint* exchangeConfig)
{
    std::vector<Datapoint*> commandObject;

    string coType = exchangeConfig->getTypeId ();
    Datapoint* type = createDpWithValue ("co_type", coType);
    commandObject.push_back (type);

    Datapoint* scope = createDpWithValue (
        "co_scope", exchangeConfig->getDomain () == "vcc" ? "vcc" : "domain");
    commandObject.push_back (scope);

    Datapoint* domain
        = createDpWithValue ("co_domain", exchangeConfig->getDomain ());
    commandObject.push_back (domain);

    Datapoint* name
        = createDpWithValue ("co_name", exchangeConfig->getName ());
    commandObject.push_back (name);

    Datapoint* se = createDpWithValue ("co_se", (long)getSelect ());
    commandObject.push_back (se);

    long time = 0;

    if (m_timestamp)
    {
        time = m_timestamp->getTimeInMs ();
    }

    Datapoint* ts = createDpWithValue ("co_ts", (long)time);
    commandObject.push_back (ts);

    Datapoint* value = nullptr;

    if (hasIntVal)
        value = createDpWithValue ("co_value", intVal);
    else
        value = createDpWithValue ("co_value", (double)floatVal);

    commandObject.push_back (value);

    return commandObject;
}
