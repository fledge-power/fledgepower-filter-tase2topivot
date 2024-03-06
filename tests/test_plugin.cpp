#include <filter.h>
#include <gtest/gtest.h>
#include <rapidjson/document.h>
#include <reading.h>
#include <reading_set.h>
#include <string>

#include "tase2_pivot_object.hpp"

using namespace std;
using namespace rapidjson;

extern "C"
{
    PLUGIN_INFORMATION* plugin_info ();

    PLUGIN_HANDLE plugin_init (ConfigCategory* config,
                               OUTPUT_HANDLE* outHandle, OUTPUT_STREAM output);

    void plugin_shutdown (PLUGIN_HANDLE handle);

    void plugin_ingest (PLUGIN_HANDLE handle, READINGSET* readingSet);
};

static string exchanged_data = QUOTE ({
    "exchanged_data" : {
        "description" : "exchanged data list",
        "type" : "JSON",
        "displayName" : "Exchanged data list",
        "order" : "1",
        "default" : {
            "exchanged_data" : {
                "name" : "TASE2pivot",
                "version" : "1.0",
                "datapoints" : [
                    {
                        "label" : "TS1",
                        "pivot_id" : "TS1",
                        "pivot_type" : "InsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointDiscrete",
                            "typeid" : "Discrete"
                        } ]
                    },
                    {
                        "label" : "TS2",
                        "pivot_id" : "TS2",
                        "pivot_type" : "MvTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointRealQTimeExt",
                            "typeid" : "RealQTimeExt"
                        } ]
                    },
                    {
                        "label" : "TS3",
                        "pivot_id" : "TS3",
                        "pivot_type" : "MvTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointReal",
                            "typeid" : "Real"
                        } ]
                    },
                    {
                        "label" : "TS4",
                        "pivot_id" : "TS4",
                        "pivot_type" : "MvTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointRealQ",
                            "typeid" : "RealQ"
                        } ]
                    },
                    {
                        "label" : "TS5",
                        "pivot_id" : "TS5",
                        "pivot_type" : "MvTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointRealQTime",
                            "typeid" : "RealQTime"
                        } ]
                    },
                    {
                        "label" : "TS6",
                        "pivot_id" : "TS6",
                        "pivot_type" : "EnsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointDiscreteQTimeExt",
                            "typeid" : "DiscreteQTimeExt"
                        } ]
                    },
                    {
                        "label" : "TS7",
                        "pivot_id" : "TS7",
                        "pivot_type" : "InsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointDiscreteQTime",
                            "typeid" : "DiscreteQTime"
                        } ]
                    },
                    {
                        "label" : "TS8",
                        "pivot_id" : "TS8",
                        "pivot_type" : "EnsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointDiscreteQ",
                            "typeid" : "DiscreteQ"
                        } ]
                    },
                    {
                        "label" : "TS9",
                        "pivot_id" : "TS9",
                        "pivot_type" : "SpsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointStateSupQTimeExt",
                            "typeid" : "StateSupQTimeExt"
                        } ]
                    },
                    {
                        "label" : "TS10",
                        "pivot_id" : "TS10",
                        "pivot_type" : "SpsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointStateSupQTime",
                            "typeid" : "StateSupQTime"
                        } ]
                    },
                    {
                        "label" : "TS11",
                        "pivot_id" : "TS11",
                        "pivot_type" : "DpsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointStateSupQ",
                            "typeid" : "StateSupQ"
                        } ]
                    },
                    {
                        "label" : "TS24",
                        "pivot_id" : "TS24",
                        "pivot_type" : "DpsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointStateSup",
                            "typeid" : "StateSup"
                        } ]
                    },
                    {
                        "label" : "TS12",
                        "pivot_id" : "TS12",
                        "pivot_type" : "SpsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointStateQTimeExt",
                            "typeid" : "StateQTimeExt"
                        } ]
                    },
                    {
                        "label" : "TS13",
                        "pivot_id" : "TS13",
                        "pivot_type" : "SpsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointStateQTime",
                            "typeid" : "StateQTime"
                        } ]
                    },
                    {
                        "label" : "TS14",
                        "pivot_id" : "TS14",
                        "pivot_type" : "DpsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointStateQ",
                            "typeid" : "StateQ"
                        } ]
                    },
                    {
                        "label" : "TS15",
                        "pivot_id" : "TS15",
                        "pivot_type" : "DpsTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:datapointState",
                            "typeid" : "State"
                        } ]
                    },
                    {
                        "label" : "TASE2Command",
                        "pivot_id" : "Command1",
                        "pivot_type" : "SpcTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:command1",
                            "typeid" : "Command"
                        } ]
                    },
                    {
                        "label" : "PivotCommand",
                        "pivot_id" : "Command2",
                        "pivot_type" : "SpcTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:command2",
                            "typeid" : "Command"
                        } ]
                    },
                    {
                        "label" : "TASE2Command",
                        "pivot_id" : "Command3",
                        "pivot_type" : "DpcTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:command3",
                            "typeid" : "Command"
                        } ]
                    },
                    {
                        "label" : "PivotCommand",
                        "pivot_id" : "Command4",
                        "pivot_type" : "IncTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:command4",
                            "typeid" : "Command"
                        } ]
                    },
                    {
                        "label" : "TASE2Command",
                        "pivot_id" : "SetpointDiscrete1",
                        "pivot_type" : "IncTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:setpointDiscrete1",
                            "typeid" : "SetPointDiscrete"
                        } ]
                    },
                    {
                        "label" : "PivotCommand",
                        "pivot_id" : "SetpointDiscrete2",
                        "pivot_type" : "IncTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:setpointDiscrete2",
                            "typeid" : "SetPointDiscrete"
                        } ]
                    },
                    {
                        "label" : "TASE2Command",
                        "pivot_id" : "SetpointDiscrete3",
                        "pivot_type" : "IngTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:setpointDiscrete3",
                            "typeid" : "SetPointDiscrete"
                        } ]
                    },
                    {
                        "label" : "TASE2Command",
                        "pivot_id" : "SetpointReal1",
                        "pivot_type" : "ApcTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:setpointReal1",
                            "typeid" : "SetPointReal"
                        } ]
                    },
                    {
                        "label" : "PivotCommand",
                        "pivot_id" : "SetpointReal2",
                        "pivot_type" : "ApcTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:setpointReal2",
                            "typeid" : "SetPointReal"
                        } ]
                    },
                    {
                        "label" : "TASE2Command",
                        "pivot_id" : "SetpointReal3",
                        "pivot_type" : "AsgTyp",
                        "protocols" : [ {
                            "name" : "tase2",
                            "ref" : "icc1:setpointReal3",
                            "typeid" : "SetPointReal"
                        } ]
                    }
                ]
            }
        }
    }
});

static string exchanged_data_1 = QUOTE ({
    "exchanged_data" : {
        "description" : "exchanged data list",
        "displayName" : "Exchanged data list",
        "order" : "1",
        "default" : {}
    }
});

static string exchanged_data_2 = QUOTE ({

    "exchanged_data" : {

        "description" : "exchanged data list",

        "displayName" : "Exchanged data list",

        "order" : "1",

        "default" : {

            "exchanged_data" : {

                "name" : "TASE2pivot",

                "version" : "1.0",

                "datapoints" : 1

            }

        }

    }

});

static int outputHandlerCalled = 0;
static Reading* lastReading = nullptr;

template <class T>
static Datapoint*
createDatapoint (const std::string& dataname, const T value)
{
    DatapointValue dp_value = DatapointValue (value);
    return new Datapoint (dataname, dp_value);
}

template <class T>
static std::vector<Datapoint*>
createCommandObject (std::string type, std::string scope, std::string domain,
                     std::string name, long se, long ts, const T value)
{
    std::vector<Datapoint*> commandObject;

    Datapoint* type1 = createDatapoint ("co_type", (std::string)type);
    commandObject.push_back (type1);

    Datapoint* scope1 = createDatapoint ("co_scope", scope);
    commandObject.push_back (scope1);

    Datapoint* domain1 = createDatapoint ("co_domain", domain);
    commandObject.push_back (domain1);

    Datapoint* name1 = createDatapoint ("co_name", name);
    commandObject.push_back (name1);

    Datapoint* se1 = createDatapoint ("co_se", (long)se);
    commandObject.push_back (se1);

    Datapoint* ts1 = createDatapoint ("co_ts", (long)ts);
    commandObject.push_back (ts1);

    Datapoint* value1 = createDatapoint ("co_value", value);
    commandObject.push_back (value1);

    return commandObject;
}

typedef enum
{
    REAL,
    REALQ,
    REALQTIME,
    REALQTIMEEXT,
    STATE,
    STATEQ,
    STATEQTIME,
    STATEQTIMEEXT,
    DISCRETE,
    DISCRETEQ,
    DISCRETEQTIME,
    DISCRETEQTIMEEXT,
    STATESUP,
    STATESUPQ,
    STATESUPQTIME,
    STATESUPQTIMEEXT,
    COMMAND,
    SETPOINTREAL,
    SETPOINTDISCRETE,
    DP_TYPE_UNKNOWN = -1
} DPTYPE;

const static std::unordered_map<std::string, DPTYPE> dpTypeMap
    = { { "State", STATE },
        { "StateQ", STATEQ },
        { "StateQTime", STATEQTIME },
        { "StateQTimeExt", STATEQTIMEEXT },
        { "Real", REAL },
        { "RealQ", REALQ },
        { "RealQTime", REALQTIME },
        { "RealQTimeExt", REALQTIMEEXT },
        { "Discrete", DISCRETE },
        { "DiscreteQ", DISCRETEQ },
        { "DiscreteQTime", DISCRETEQTIME },
        { "DiscreteQTimeExt", DISCRETEQTIMEEXT },
        { "StateSup", STATESUP },
        { "StateSupQ", STATESUPQ },
        { "StateSupQTime", STATESUPQTIME },
        { "StateSupQTimeExt", STATESUPQTIMEEXT },
        { "Command", COMMAND },
        { "SetPointReal", SETPOINTREAL },
        { "SetPointDiscrete", SETPOINTDISCRETE } };

DPTYPE
getDpTypeFromString (const std::string& type)
{
    auto it = dpTypeMap.find (type);
    if (it != dpTypeMap.end ())
    {
        return it->second;
    }
    return DP_TYPE_UNKNOWN;
}

template <class T>
static Datapoint*
createDataObject (const char* type, const char* domain, const char* name,
                  const T value, const char* iv, const char* cs,
                  const char* nv, uint64_t ts, const char* tsv)
{
    auto* datapoints = new vector<Datapoint*>;

    datapoints->push_back (createDatapoint ("do_type", type));
    datapoints->push_back (createDatapoint ("do_domain", domain));
    datapoints->push_back (createDatapoint ("do_name", name));

    DPTYPE dpType = getDpTypeFromString (type);

    switch (dpType)
    {
    case DISCRETE:
    case DISCRETEQ:
    case DISCRETEQTIME:
    case DISCRETEQTIMEEXT:
    case STATESUP:
    case STATESUPQ:
    case STATESUPQTIME:
    case STATESUPQTIMEEXT:
    case STATE:
    case STATEQ:
    case STATEQTIME:
    case STATEQTIMEEXT: {
        datapoints->push_back (createDatapoint ("do_value", (int64_t)value));
        break;
    }
    case REAL:
    case REALQ:
    case REALQTIME:
    case REALQTIMEEXT: {
        datapoints->push_back (createDatapoint ("do_value", (double)value));
        break;
    }
    default: {
        break;
    }
    }

    datapoints->push_back (createDatapoint ("do_validity", iv));
    datapoints->push_back (createDatapoint ("do_cs", cs));
    datapoints->push_back (createDatapoint ("do_quality_normal_value", nv));
    if (ts)
    {
        datapoints->push_back (createDatapoint ("do_ts", (long)ts));
        datapoints->push_back (createDatapoint ("do_ts_validity", tsv));
    }

    DatapointValue dpv (datapoints, true);

    Datapoint* dp = new Datapoint ("data_object", dpv);

    return dp;
}

static Datapoint*
getDatapoint (Reading* reading, const string& key)
{
    std::vector<Datapoint*>& datapoints = reading->getReadingData ();

    for (Datapoint* dp : datapoints)
    {
        if (dp->getName () == key)
            return dp;
    }

    return nullptr;
}

static std::vector<Datapoint*>
getDatapoints (Reading* reading)
{
    std::vector<Datapoint*>& datapoints = reading->getReadingData ();
    return datapoints;
}

static Datapoint*
getChild (Datapoint* dp, const string& name)
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

static bool
isValueInt (Datapoint* dp)
{
    DatapointValue& dpv = dp->getData ();

    if (dpv.getType () == DatapointValue::T_INTEGER)
    {
        return true;
    }

    return false;
}

static long
getValueInt (Datapoint* dp)
{
    DatapointValue& dpv = dp->getData ();

    if (dpv.getType () == DatapointValue::T_INTEGER)
    {
        return dpv.toInt ();
    }

    return 0;
}

static bool
isValueStr (Datapoint* dp)
{
    DatapointValue& dpv = dp->getData ();

    if (dpv.getType () == DatapointValue::T_STRING)
    {
        return true;
    }

    return false;
}

static const string
getValueStr (Datapoint* dp)
{
    DatapointValue& dpv = dp->getData ();

    if (dpv.getType () == DatapointValue::T_STRING)
    {
        return dpv.toStringValue ();
    }

    return "";
}

static bool
isValueFloat (Datapoint* dp)
{
    DatapointValue& dpv = dp->getData ();

    if (dpv.getType () == DatapointValue::T_FLOAT)
    {
        return true;
    }

    return false;
}

static float
getValueFloat (Datapoint* dp)
{
    DatapointValue& dpv = dp->getData ();

    if (dpv.getType () == DatapointValue::T_FLOAT)
    {
        return (float)dpv.toDouble ();
    }

    return 0.f;
}

TEST (PivotTASE2Plugin, PluginInfo)
{
    PLUGIN_INFORMATION* info = plugin_info ();
    ASSERT_STREQ (info->name, "tase2_pivot_filter");
    ASSERT_EQ (info->type, PLUGIN_TYPE_FILTER);
}

static void
testOutputStream (OUTPUT_HANDLE* handle, READINGSET* readingSet)
{
    const std::vector<Reading*> readings = readingSet->getAllReadings ();

    for (Reading* reading : readings)
    {
        printf ("output: Reading: %s\n", reading->getAssetName ().c_str ());

        std::vector<Datapoint*>& datapoints = reading->getReadingData ();

        for (Datapoint* dp : datapoints)
        {
            printf ("output:   datapoint: %s -> %s\n", dp->getName ().c_str (),
                    dp->getData ().toString ().c_str ());
        }

        if (lastReading != nullptr)
        {
            delete lastReading;
            lastReading = nullptr;
        }

        lastReading = new Reading (*reading);
    }

    outputHandlerCalled++;
}

TEST (PivotTASE2Plugin, PluginInitShutdown)
{
    ConfigCategory config ("TASE2pivot", exchanged_data);

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, PluginInitNoConfig)
{
    PLUGIN_HANDLE handle = plugin_init (NULL, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, Plugin_ingest)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (
        createDataObject ("Discrete", "icc1", "datapointDiscrete", 1, "valid",
                          "telemetered", "normal", (uint64_t)123456, "valid"));
    dataobjects.push_back (
        createDataObject ("Discrete", "icc1", "datapointDiscrete", 1, "valid",
                          "telemetered", "normal", (uint64_t)123456, "valid"));
    dataobjects.push_back (
        createDataObject ("Discrete", "icc1", "datapointDiscrete", 1, "valid",
                          "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS1"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, OperationPlugin_ingest_1)
{
    outputHandlerCalled = 0;

    vector<vector<Datapoint*> > commandobjects;

    commandobjects.push_back (createCommandObject (
        "Command", "domain", "icc1", "command1", 0, 2421512, (long)1));

    Reading* reading
        = new Reading (std::string ("TASE2Command"), commandobjects[0]);

    reading->setId (1);
    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (2, outputHandlerCalled);

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, SetPointDiscrete)
{
    outputHandlerCalled = 0;

    vector<vector<Datapoint*> > commandobjects;

    commandobjects.push_back (
        createCommandObject ("SetPointDiscrete", "domain", "icc1",
                             "setpointDiscrete1", 0, 2421512, (long)5));

    Reading* reading
        = new Reading (std::string ("TASE2Command"), commandobjects[0]);

    reading->setId (1);
    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIC");
    ASSERT_NE (nullptr, gtis);
    Datapoint* incTyp = getChild (gtis, "IncTyp");
    ASSERT_NE (nullptr, incTyp);
    Datapoint* ctlVal = getChild (incTyp, "ctlVal");
    ASSERT_NE (nullptr, ctlVal);
    ASSERT_TRUE (isValueInt (ctlVal));
    ASSERT_EQ (5, getValueInt (ctlVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, SetPointReal)
{
    outputHandlerCalled = 0;

    vector<vector<Datapoint*> > commandobjects;

    commandobjects.push_back (createCommandObject ("SetPointReal", "domain",
                                                   "icc1", "setpointReal1", 0,
                                                   2421512, (double)2.1));

    Reading* reading
        = new Reading (std::string ("TASE2Command"), commandobjects[0]);

    reading->setId (1);
    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIC");
    ASSERT_NE (nullptr, gtis);
    Datapoint* incTyp = getChild (gtis, "ApcTyp");
    ASSERT_NE (nullptr, incTyp);
    Datapoint* ctlVal = getChild (incTyp, "ctlVal");
    ASSERT_NE (nullptr, ctlVal);
    ASSERT_TRUE (isValueFloat (ctlVal));
    ASSERT_NEAR (2.1, getValueFloat (ctlVal), 0.0001);

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, Discrete)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (
        createDataObject ("Discrete", "icc1", "datapointDiscrete", 1, "valid",
                          "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS1"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "InsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueInt (stVal));
    ASSERT_EQ (1, getValueInt (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, DiscreteQ)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (createDataObject (
        "DiscreteQ", "icc1", "datapointDiscreteQ", 1, "valid", "telemetered",
        "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS8"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "EnsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueInt (stVal));
    ASSERT_EQ (1, getValueInt (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, DiscreteQTime)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (createDataObject (
        "DiscreteQTime", "icc1", "datapointDiscreteQTime", 1, "valid",
        "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS7"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "InsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueInt (stVal));
    ASSERT_EQ (1, getValueInt (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, DiscreteQTimeExt)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (createDataObject (
        "DiscreteQTimeExt", "icc1", "datapointDiscreteQTimeExt", 1, "valid",
        "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS6"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "EnsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueInt (stVal));
    ASSERT_EQ (1, getValueInt (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, Real)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (
        createDataObject ("Real", "icc1", "datapointReal", 1, "valid",
                          "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS3"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIM");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "MvTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* magVal = getChild (spsTyp, "mag");
    ASSERT_NE (nullptr, magVal);
    Datapoint* fVal = getChild (magVal, "f");
    ASSERT_NE (nullptr, fVal);
    ASSERT_TRUE (isValueFloat (fVal));
    ASSERT_NEAR (1, getValueFloat (fVal), 0.001);

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, RealQ)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (
        createDataObject ("RealQ", "icc1", "datapointRealQ", 1, "valid",
                          "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS4"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIM");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "MvTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* magVal = getChild (spsTyp, "mag");
    ASSERT_NE (nullptr, magVal);
    Datapoint* fVal = getChild (magVal, "f");
    ASSERT_NE (nullptr, fVal);
    ASSERT_TRUE (isValueFloat (fVal));
    ASSERT_NEAR (1, getValueFloat (fVal), 0.001);

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, RealQTime)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (createDataObject (
        "RealQTime", "icc1", "datapointRealQTime", 1, "valid", "telemetered",
        "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS5"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIM");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "MvTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* magVal = getChild (spsTyp, "mag");
    ASSERT_NE (nullptr, magVal);
    Datapoint* fVal = getChild (magVal, "f");
    ASSERT_NE (nullptr, fVal);
    ASSERT_TRUE (isValueFloat (fVal));
    ASSERT_NEAR (1, getValueFloat (fVal), 0.001);

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, RealQTimeExt)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (createDataObject (
        "RealQTimeExt", "icc1", "datapointRealQTimeExt", 1, "valid",
        "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS2"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIM");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "MvTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* magVal = getChild (spsTyp, "mag");
    ASSERT_NE (nullptr, magVal);
    Datapoint* fVal = getChild (magVal, "f");
    ASSERT_NE (nullptr, fVal);
    ASSERT_TRUE (isValueFloat (fVal));
    ASSERT_NEAR (1, getValueFloat (fVal), 0.001);

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, MvTyp_to_RealQTimeExt)
{
    outputHandlerCalled = 0;

    PivotDataObject* mvTyp = new PivotDataObject ("GTIM", "MvTyp");

    ASSERT_NE (nullptr, mvTyp);

    mvTyp->setIdentifier ("TS2");
    mvTyp->setMagF (0.1f);
    mvTyp->addQuality ("valid", "telemetered", "normal");
    mvTyp->addTimestamp (1669123796250, "valid");

    Datapoint* dp = mvTyp->toDatapoint ();

    delete mvTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (dp);

    Reading* reading = new Reading (std::string ("TS2"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* dataobject = getDatapoint (lastReading, "data_object");
    ASSERT_NE (nullptr, dataobject);

    Datapoint* doType = getChild (dataobject, "do_type");
    ASSERT_NE (nullptr, doType);
    ASSERT_TRUE (isValueStr (doType));
    ASSERT_EQ ("RealQTimeExt", getValueStr (doType));

    Datapoint* doDomain = getChild (dataobject, "do_domain");
    ASSERT_NE (nullptr, doDomain);
    ASSERT_TRUE (isValueStr (doDomain));
    ASSERT_EQ ("icc1", getValueStr (doDomain));

    Datapoint* doName = getChild (dataobject, "do_name");
    ASSERT_NE (nullptr, doName);
    ASSERT_TRUE (isValueStr (doName));
    ASSERT_EQ ("datapointRealQTimeExt", getValueStr (doName));

    Datapoint* doValue = getChild (dataobject, "do_value");
    ASSERT_NE (nullptr, doValue);
    ASSERT_TRUE (isValueFloat (doValue));
    ASSERT_EQ (0.1f, getValueFloat (doValue));

    Datapoint* validityFlag;

    validityFlag = getChild (dataobject, "do_validity");
    ASSERT_NE (nullptr, validityFlag);
    ASSERT_TRUE (isValueStr (validityFlag));
    ASSERT_EQ ("valid", getValueStr (validityFlag));

    Datapoint* cs = getChild (dataobject, "do_cs");
    ASSERT_NE (nullptr, cs);
    ASSERT_TRUE (isValueStr (cs));
    ASSERT_EQ ("telemetered", getValueStr (cs));

    Datapoint* nv = getChild (dataobject, "do_quality_normal_value");
    ASSERT_NE (nullptr, nv);
    ASSERT_TRUE (isValueStr (nv));
    ASSERT_EQ ("normal", getValueStr (nv));

    Datapoint* doTs = getChild (dataobject, "do_ts");
    ASSERT_NE (nullptr, doTs);
    ASSERT_TRUE (isValueInt (doTs));
    ASSERT_EQ (1669123796250, getValueInt (doTs));

    Datapoint* doTsSub = getChild (dataobject, "do_ts_validity");
    ASSERT_NE (nullptr, doTsSub);

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, MvTyp_to_RealQTimeExtDefaultQuality)
{
    outputHandlerCalled = 0;

    PivotDataObject* mvTyp = new PivotDataObject ("GTIM", "MvTyp");

    ASSERT_NE (nullptr, mvTyp);

    mvTyp->setIdentifier ("TS2");
    mvTyp->setMagF (0.1f);
    mvTyp->addTimestamp (1669123796250, "valid");

    Datapoint* dp = mvTyp->toDatapoint ();

    delete mvTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (dp);

    Reading* reading = new Reading (std::string ("TS2"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* dataobject = getDatapoint (lastReading, "data_object");
    ASSERT_NE (nullptr, dataobject);

    Datapoint* doType = getChild (dataobject, "do_type");
    ASSERT_NE (nullptr, doType);
    ASSERT_TRUE (isValueStr (doType));
    ASSERT_EQ ("RealQTimeExt", getValueStr (doType));

    Datapoint* doDomain = getChild (dataobject, "do_domain");
    ASSERT_NE (nullptr, doDomain);
    ASSERT_TRUE (isValueStr (doDomain));
    ASSERT_EQ ("icc1", getValueStr (doDomain));

    Datapoint* doName = getChild (dataobject, "do_name");
    ASSERT_NE (nullptr, doName);
    ASSERT_TRUE (isValueStr (doName));
    ASSERT_EQ ("datapointRealQTimeExt", getValueStr (doName));

    Datapoint* doValue = getChild (dataobject, "do_value");
    ASSERT_NE (nullptr, doValue);
    ASSERT_TRUE (isValueFloat (doValue));
    ASSERT_EQ (0.1f, getValueFloat (doValue));

    Datapoint* validityFlag;

    validityFlag = getChild (dataobject, "do_validity");
    ASSERT_NE (nullptr, validityFlag);
    ASSERT_TRUE (isValueStr (validityFlag));
    ASSERT_EQ ("valid", getValueStr (validityFlag));

    Datapoint* cs = getChild (dataobject, "do_cs");
    ASSERT_NE (nullptr, cs);
    ASSERT_TRUE (isValueStr (cs));
    ASSERT_EQ ("telemetered", getValueStr (cs));

    Datapoint* nv = getChild (dataobject, "do_quality_normal_value");
    ASSERT_NE (nullptr, nv);
    ASSERT_TRUE (isValueStr (nv));
    ASSERT_EQ ("normal", getValueStr (nv));

    Datapoint* doTs = getChild (dataobject, "do_ts");
    ASSERT_NE (nullptr, doTs);
    ASSERT_TRUE (isValueInt (doTs));
    ASSERT_EQ (1669123796250, getValueInt (doTs));

    Datapoint* doTsSub = getChild (dataobject, "do_ts_validity");
    ASSERT_NE (nullptr, doTsSub);

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, StateQTimeExt)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (createDataObject (
        "StateQTimeExt", "icc1", "datapointStateQTimeExt", 1, "valid",
        "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS12"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "SpsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueInt (stVal));
    ASSERT_EQ (1, getValueInt (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}
TEST (PivotTASE2Plugin, StateQTime)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (createDataObject (
        "StateQTime", "icc1", "datapointStateQTime", 1, "valid", "telemetered",
        "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS13"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "SpsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueInt (stVal));
    ASSERT_EQ (1, getValueInt (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}
TEST (PivotTASE2Plugin, StateQ)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (
        createDataObject ("StateQ", "icc1", "datapointStateQ", 1, "valid",
                          "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS14"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "DpsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueStr (stVal));
    ASSERT_EQ ("off", getValueStr (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}
TEST (PivotTASE2Plugin, State)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (
        createDataObject ("State", "icc1", "datapointState", 1, "valid",
                          "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS15"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "DpsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueStr (stVal));
    ASSERT_EQ ("off", getValueStr (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}

TEST (PivotTASE2Plugin, StateSupQTimeExt)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (createDataObject (
        "StateSupQTimeExt", "icc1", "datapointStateSupQTimeExt", 1, "valid",
        "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS9"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "SpsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueInt (stVal));
    ASSERT_EQ (1, getValueInt (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}
TEST (PivotTASE2Plugin, StateSupQTime)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (createDataObject (
        "StateSupQTime", "icc1", "datapointStateSupQTime", 1, "valid",
        "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS10"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "SpsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueInt (stVal));
    ASSERT_EQ (1, getValueInt (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}
TEST (PivotTASE2Plugin, StateSupQ)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (createDataObject (
        "StateSupQ", "icc1", "datapointStateSupQ", 1, "valid", "telemetered",
        "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS11"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "DpsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueStr (stVal));
    ASSERT_EQ ("off", getValueStr (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}
TEST (PivotTASE2Plugin, StateSup)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back (
        createDataObject ("StateSup", "icc1", "datapointStateSup", 1, "valid",
                          "telemetered", "normal", (uint64_t)123456, "valid"));

    Reading* reading = new Reading (std::string ("TS24"), dataobjects);

    reading->setId (1);

    // unitilized value" error

    vector<Reading*> readings;

    readings.push_back (reading);

    ReadingSet readingSet;

    readingSet.append (readings);

    ConfigCategory config ("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault ();

    string configValue = config.getValue ("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init (&config, NULL, testOutputStream);

    ASSERT_TRUE (handle != nullptr);

    plugin_ingest (handle, &readingSet);

    ASSERT_EQ (1, outputHandlerCalled);

    ASSERT_NE (nullptr, lastReading);

    Datapoint* pivot = getDatapoint (lastReading, "PIVOT");
    ASSERT_NE (nullptr, pivot);
    Datapoint* gtis = getChild (pivot, "GTIS");
    ASSERT_NE (nullptr, gtis);
    Datapoint* spsTyp = getChild (gtis, "DpsTyp");
    ASSERT_NE (nullptr, spsTyp);
    Datapoint* stVal = getChild (spsTyp, "stVal");
    ASSERT_NE (nullptr, stVal);
    ASSERT_TRUE (isValueStr (stVal));
    ASSERT_EQ ("off", getValueStr (stVal));

    Datapoint* comingFrom = getChild (gtis, "ComingFrom");
    ASSERT_NE (nullptr, comingFrom);
    ASSERT_TRUE (isValueStr (comingFrom));
    ASSERT_EQ ("tase2", getValueStr (comingFrom));

    plugin_shutdown (handle);
}
