/*
 * FledgePower TASE.2 <-> pivot filter plugin.
 *
 * Copyright (c) 2023, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *

 *
 */

#ifndef PIVOT_TASE2_CONFIG_H
#define PIVOT_TASE2_CONFIG_H

#include <map>
#include <memory>

using namespace std;

class TASE2PivotDataPoint
{
  public:
    TASE2PivotDataPoint (string label, string pivotId, string pivotType,
                         string typeIdString, std::string domain,
                         std::string name, string altMappingRule);
    ~TASE2PivotDataPoint ();

    std::string&
    getLabel ()
    {
        return m_label;
    };
    std::string&
    getPivotId ()
    {
        return m_pivotId;
    };
    std::string&
    getPivotType ()
    {
        return m_pivotType;
    };
    std::string&
    getTypeId ()
    {
        return m_typeIdStr;
    };
    std::string&
    getDomain ()
    {
        return m_domain;
    };
    std::string&
    getName ()
    {
        return m_name;
    };

  private:
    std::string m_label;
    std::string m_pivotId;
    std::string m_pivotType;

    std::string m_typeIdStr;
    std::string m_domain;
    std::string m_name;
    std::string m_alternateMappingRule;
};

class TASE2PivotConfig
{
  public:
    void importExchangeConfig (const string& exchangeConfig);

    TASE2PivotDataPoint* getExchangeDefinitionsByLabel (std::string label);
    TASE2PivotDataPoint* getExchangeDefinitionsByAddress (std::string address);
    TASE2PivotDataPoint* getExchangeDefinitionsByPivotId (std::string pivotid);

  private:
    void m_deleteExchangeDefinitions ();

    static bool m_check_string (const rapidjson::Value& json, const char* key);
    static bool m_check_array (const rapidjson::Value& json, const char* key);
    static bool m_check_object (const rapidjson::Value& json, const char* key);

    static bool m_retrieve (const rapidjson::Value& json, const char* key,
                            std::string* target);

    bool m_exchangeConfigComplete = false;

    std::map<std::string, std::shared_ptr<TASE2PivotDataPoint> >
        m_exchangeDefinitionsLabel;
    std::map<std::string, std::shared_ptr<TASE2PivotDataPoint> >
        m_exchangeDefinitionsAddress;
    std::map<std::string, std::shared_ptr<TASE2PivotDataPoint> >
        m_exchangeDefinitionsPivotId;
};

#endif /* PIVOT_TASE2_CONFIG_H */