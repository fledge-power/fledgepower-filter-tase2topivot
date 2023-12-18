/*
 * FledgePower TASE.2 <-> pivot filter plugin.
 *
 * Copyright (c) 2023, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *

 *
 */

#ifndef _TASE2_PIVOT_FILTER_H
#define _TASE2_PIVOT_FILTER_H

#include "tase2_pivot_filter_config.hpp"
#include <filter.h>

using namespace std;

class TASE2PivotFilter
{

  public:
    /*
     * Struct used to store fields of a data object during processing
     */
    struct TASE2DataObject
    {
        std::string doType = "";
        std::string doValidity = "";
        std::string doCurrentSource = "";
        std::string doNormalValue = "";
        long doTs = 0;
        std::string doTsValidity = "";
        std::string comingFromValue = "TASE2";
        Datapoint* doValue = nullptr;
    };
    /*
     * Struct used to store fields of a command object during processing
     */
    struct TASE2CommandObject
    {
        std::string coType = "";
        std::string coScope = "";
        std::string coDomain = "";
        std::string coName = "";
        int coSe = 0;
        long coTs = 0;
        std::string comingFromValue = "TASE2";
        Datapoint* coValue = nullptr;
    };

    TASE2PivotFilter (const std::string& filterName,
                      ConfigCategory* filterConfig, OUTPUT_HANDLE* outHandle,
                      OUTPUT_STREAM output);

    ~TASE2PivotFilter ();

    void ingest (READINGSET* readingSet);
    void reconfigure (ConfigCategory* config);

  private:
    Datapoint* addElement (Datapoint* dp, string elementPath);

    void addQuality (Datapoint* dp, const std::string& validity, const std::string& cs, const std::string& nv);

    void addTimestamp (Datapoint* dp, long timestamp, const std::string& validity);

    template <class T>
    Datapoint* addElementWithValue (Datapoint* dp, string elementPath,
                                    const T value);

    Datapoint* createDp (string name);

    template <typename T>
    void static readAttribute (std::map<std::string, bool>& attributeFound,
                               Datapoint* dp, const std::string& targetName,
                               T& out);
    void static readAttribute (std::map<std::string, bool>& attributeFound,
                               Datapoint* dp, const std::string& targetName,
                               Datapoint*& out);
    void static readAttribute (std::map<std::string, bool>& attributeFound,
                               Datapoint* dp, const std::string& targetName,
                               std::string& out);

    Datapoint* convertDataObjectToPivot (Datapoint* sourceDp,
                                         TASE2PivotDataPoint* exchangeConfig);

    Datapoint*
    convertOperationObjectToPivot (std::vector<Datapoint*> sourceDp);

    Datapoint*
    convertDatapointToTASE2DataObject (Datapoint* sourceDp,
                                       TASE2PivotDataPoint* exchangeConfig);

    std::vector<Datapoint*>
    convertReadingToTASE2OperationObject (Datapoint* datapoints);

    bool isTimestampType (const std::string& type);

    OUTPUT_HANDLE* m_outHandle = nullptr;
    OUTPUT_STREAM m_output = nullptr;

    TASE2PivotConfig m_config;
};

#endif /* _TASE2_PIVOT_FILTER_H */