/*
 * FledgePower TASE.2 <-> pivot filter plugin.
 *
 * Copyright (c) 2023, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *

 *
 */

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include "tase2_pivot_filter_config.hpp"
#include "tase2_pivot_utility.hpp"

using namespace rapidjson;

TASE2PivotDataPoint::TASE2PivotDataPoint (std::string label,
                                          std::string pivotId,
                                          std::string pivotType,
                                          std::string typeIdString,
                                          std::string domain, std::string name,
                                          std::string altMappingRule)
{
    m_label = label;
    m_pivotId = pivotId;
    m_pivotType = pivotType;
    m_typeIdStr = typeIdString;
    m_domain = domain;
    m_name = name;
    m_alternateMappingRule = altMappingRule;
}

TASE2PivotDataPoint::~TASE2PivotDataPoint () {}

#define PROTOCOL_TASE2 "tase2"
#define JSON_PROT_NAME "name"
#define JSON_PROT_ADDR "address"
#define JSON_PROT_TYPEID "typeid"

TASE2PivotDataPoint*
TASE2PivotConfig::getExchangeDefinitionsByLabel (std::string label)
{
    auto it = m_exchangeDefinitionsLabel.find (label);
    if (it != m_exchangeDefinitionsLabel.end ())
    {
        return it->second.get ();
    }
    else
    {
        return nullptr;
    }
}

TASE2PivotDataPoint*
TASE2PivotConfig::getExchangeDefinitionsByAddress (std::string address)
{
    auto it = m_exchangeDefinitionsAddress.find (address);
    if (it != m_exchangeDefinitionsAddress.end ())
    {
        return it->second.get ();
    }
    else
    {
        return nullptr;
    }
}

TASE2PivotDataPoint*
TASE2PivotConfig::getExchangeDefinitionsByPivotId (std::string pivotid)
{
    auto it = m_exchangeDefinitionsPivotId.find (pivotid);
    if (it != m_exchangeDefinitionsPivotId.end ())
    {
        return it->second.get ();
    }
    else
    {
        return nullptr;
    }
}

void
TASE2PivotConfig::importExchangeConfig (const string& exchangeConfig)
{
    std::string beforeLog = TASE2PivotUtility::PluginName
                            + " - TASE2PivotConfig::importExchangeConfig -";
    m_exchangeConfigComplete = false;

    m_deleteExchangeDefinitions ();

    Document document;

    if (document.Parse (const_cast<char*> (exchangeConfig.c_str ()))
            .HasParseError ())
    {
        TASE2PivotUtility::log_fatal (
            "%s Parsing error in exchanged_data json, offset %u: %s",
            beforeLog.c_str (),
            static_cast<unsigned> (document.GetErrorOffset ()),
            GetParseError_En (document.GetParseError ()));
        return;
    }

    if (!document.IsObject ())
        return;

    if (!m_check_object (document, "exchanged_data"))
        return;

    const Value& exchangeData = document["exchanged_data"];

    if (!m_check_array (exchangeData, "datapoints"))
        return;

    const Value& datapoints = exchangeData["datapoints"];

    for (const Value& datapoint : datapoints.GetArray ())
    {
        if (!datapoint.IsObject ())
            return;

        string label;
        if (!m_retrieve (datapoint, "label", &label))
            return;

        string pivotId;
        if (!m_retrieve (datapoint, "pivot_id", &pivotId))
            return;

        string pivotType;
        if (!m_retrieve (datapoint, "pivot_type", &pivotType))
            return;

        if (!m_check_array (datapoint, "protocols"))
            return;

        for (const Value& protocol : datapoint["protocols"].GetArray ())
        {

            if (!protocol.IsObject ())
                return;

            string protocolName;
            if (!m_retrieve (protocol, JSON_PROT_NAME, &protocolName))
                return;

            if (protocolName == PROTOCOL_TASE2)
            {
                string ref;
                if (!m_retrieve (protocol, "ref", &ref))
                    return;

                string typeIdStr;
                if (!m_retrieve (protocol, JSON_PROT_TYPEID, &typeIdStr))
                    return;

                string alternateMappingRule;
                if (protocol.HasMember ("alternate_mapping_rule"))
                {
                    m_retrieve (protocol, "alternate_mapping_rule",
                                &alternateMappingRule);
                }

                size_t sepPos = ref.find (":");

                if (sepPos != std::string::npos)
                {
                    std::string domain = ref.substr (0, sepPos);
                    std::string name = ref.substr (sepPos + 1);

                    auto newDp = std::make_shared<TASE2PivotDataPoint> (
                        label, pivotId, pivotType, typeIdStr, domain, name,
                        alternateMappingRule);

                    m_exchangeDefinitionsLabel[label] = newDp;
                    m_exchangeDefinitionsAddress[ref] = newDp;
                    m_exchangeDefinitionsPivotId[pivotId] = newDp;
                }
            }
        }
    }

    m_exchangeConfigComplete = true;
}

void
TASE2PivotConfig::m_deleteExchangeDefinitions ()
{
    m_exchangeDefinitionsLabel.clear ();
    m_exchangeDefinitionsAddress.clear ();
    m_exchangeDefinitionsPivotId.clear ();
}

bool
TASE2PivotConfig::m_check_string (const rapidjson::Value& json,
                                  const char* key)
{
    std::string beforeLog = TASE2PivotUtility::PluginName
                            + " - TASE2PivotConfig::m_check_string -";
    if (!json.HasMember (key) || !json[key].IsString ())
    {
        TASE2PivotUtility::log_error (
            "%s Error with the field %s, the value does not exist or is not a "
            "std::string.",
            beforeLog.c_str (), key);
        return false;
    }
    return true;
}

bool
TASE2PivotConfig::m_check_array (const rapidjson::Value& json, const char* key)
{
    std::string beforeLog = TASE2PivotUtility::PluginName
                            + " - TASE2PivotConfig::m_check_array -";
    if (!json.HasMember (key) || !json[key].IsArray ())
    {
        TASE2PivotUtility::log_error (
            "%s The array %s is required but not found.", beforeLog.c_str (),
            key);
        return false;
    }
    return true;
}

bool
TASE2PivotConfig::m_check_object (const rapidjson::Value& json,
                                  const char* key)
{
    std::string beforeLog = TASE2PivotUtility::PluginName
                            + " - TASE2PivotConfig::m_check_object -";
    if (!json.HasMember (key) || !json[key].IsObject ())
    {
        TASE2PivotUtility::log_error (
            "%s The array %s is required but not found.", beforeLog.c_str (),
            key);
        return false;
    }
    return true;
}

bool
TASE2PivotConfig::m_retrieve (const rapidjson::Value& json, const char* key,
                              std::string* target)
{
    std::string beforeLog
        = TASE2PivotUtility::PluginName + " - TASE2PivotConfig::m_retrieve -";
    if (!json.HasMember (key) || !json[key].IsString ())
    {
        TASE2PivotUtility::log_error (
            "%s Error with the field %s, the value does not exist or is not a "
            "std::string.",
            beforeLog.c_str (), key);
        return false;
    }
    *target = json[key].GetString ();
    return true;
}