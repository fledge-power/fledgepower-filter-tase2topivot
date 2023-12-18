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
#include <plugin_api.h>
#include <version.h>

#include "tase2_pivot_filter.hpp"
#include "tase2_pivot_utility.hpp"

using namespace std;
using namespace rapidjson;

extern "C"
{

    /**
     * Plugin specific default configuration
     */
    static const char* default_config = QUOTE ({
        "plugin" : {
            "description" : "TASE.2 to pivot filter plugin",
            "type" : "string",
            "default" : PLUGIN_NAME,
            "readonly" : "true"
        },
        "exchanged_data" : {
            "description" : "exchanged data list",
            "type" : "JSON",
            "displayName" : "Exchanged data list",
            "order" : "1",
            "default" : QUOTE ({
                "exchanged_data" : {
                    "name" : "TASE2pivot",
                    "version" : "1.0",
                    "datapoints" : [
                        {
                            "label" : "TS7",
                            "pivot_id" : "TS7",
                            "pivot_type" : "DpsTyp",
                            "protocols" : [ {
                                "name" : "TASE2",
                                "ref" : "icc1:datapointState",
                                "typeid" : "State"
                            } ]
                        },
                        {
                            "label" : "TC1",
                            "pivot_id" : "TC1",
                            "pivot_type" : "SpcTyp",
                            "protocols" : [ {
                                "name" : "TASE2",
                                "ref" : "icc1:command1",
                                "typeid" : "Command"
                            } ]
                        }
                    ]
                }
            })
        }
    });

    /**
     * The C API plugin information structure
     */
    static PLUGIN_INFORMATION info = {
        PLUGIN_NAME,        // Name
        VERSION,            // Version
        0,                  // Flags
        PLUGIN_TYPE_FILTER, // Type
        "1.0.0",            // Interface version
        default_config      // Configuration
    };

    /**
     * Return the information about this plugin
     */
    PLUGIN_INFORMATION*
    plugin_info ()
    {
        return &info;
    }

    /**
     * Initialise the plugin with configuration.
     *
     * This function is called to get the plugin handle.
     */
    PLUGIN_HANDLE
    plugin_init (ConfigCategory* config, OUTPUT_HANDLE* outHandle,
                 OUTPUT_STREAM output)
    {
        std::string beforeLog
            = TASE2PivotUtility::PluginName + " - plugin_init -";
        TASE2PivotUtility::log_info ("%s Initializing the plugin",
                                     beforeLog.c_str ());

        TASE2PivotFilter* pivotFilter
            = new TASE2PivotFilter (PLUGIN_NAME, config, outHandle, output);

        return (PLUGIN_HANDLE)pivotFilter;
    }

    /**
     * Ingest a set of readings into the plugin for processing
     *
     * @param handle     The plugin handle returned from plugin_init
     * @param readingSet The readings to process
     */
    void
    plugin_ingest (PLUGIN_HANDLE handle, READINGSET* readingSet)
    {
        TASE2PivotFilter* pivotFilter = (TASE2PivotFilter*)handle;
        pivotFilter->ingest (readingSet);
    }

    /**
     * Plugin reconfiguration method
     *
     * @param handle     The plugin handle
     * @param newConfig  The updated configuration
     */
    void
    plugin_reconfigure (PLUGIN_HANDLE handle, const std::string& newConfig)
    {
        TASE2PivotFilter* pivotFilter = (TASE2PivotFilter*)handle;

        ConfigCategory config ("TASE2pivot", newConfig);

        pivotFilter->reconfigure (&config);
    }

    /**
     * Call the shutdown method in the plugin
     */
    void
    plugin_shutdown (PLUGIN_HANDLE handle)
    {
        TASE2PivotFilter* pivotFilter = (TASE2PivotFilter*)handle;
        delete pivotFilter;
    }

    // End of extern "C"
};
