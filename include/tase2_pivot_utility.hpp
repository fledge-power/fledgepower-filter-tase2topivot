/*
 * FledgePower TASE2 <-> pivot filter utility functions.
 *
 * Copyright (c) 2023, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *

 *
 */

#ifndef _TASE2_PIVOT_UTILITY_H
#define _TASE2_PIVOT_UTILITY_H

#include <logger.h>
#include <string>
#include <vector>

#define PLUGIN_NAME "tase2_pivot_filter"

namespace TASE2PivotUtility {

static const std::string PluginName = PLUGIN_NAME;

/**
 * Join a list of strings into a single string with the given separator
 * @param list : List of strings to join
 * @param sep : Separator to put bewteen each list element
 * @return String containing the concatenation of all strings in the list with
 * separator inbetween
 */
std::string join (const std::vector<std::string>& list,
                  const std::string& sep = ", ");

/*
 * Log helper function that will log both in the Fledge syslog file and in
 * stdout for unit tests
 */
template <class... Args>
void
log_debug (const std::string& format, Args&&... args)
{
#ifdef UNIT_TEST
    printf (std::string (format).append ("\n").c_str (),
            std::forward<Args> (args)...);
    fflush (stdout);
#endif
    Logger::getLogger ()->debug (format.c_str (),
                                 std::forward<Args> (args)...);
}

template <class... Args>
void
log_info (const std::string& format, Args&&... args)
{
#ifdef UNIT_TEST
    printf (std::string (format).append ("\n").c_str (),
            std::forward<Args> (args)...);
    fflush (stdout);
#endif
    Logger::getLogger ()->info (format.c_str (), std::forward<Args> (args)...);
}

template <class... Args>
void
log_warn (const std::string& format, Args&&... args)
{
#ifdef UNIT_TEST
    printf (std::string (format).append ("\n").c_str (),
            std::forward<Args> (args)...);
    fflush (stdout);
#endif
    Logger::getLogger ()->warn (format.c_str (), std::forward<Args> (args)...);
}

template <class... Args>
void
log_error (const std::string& format, Args&&... args)
{
#ifdef UNIT_TEST
    printf (std::string (format).append ("\n").c_str (),
            std::forward<Args> (args)...);
    fflush (stdout);
#endif
    Logger::getLogger ()->error (format.c_str (),
                                 std::forward<Args> (args)...);
}

template <class... Args>
void
log_fatal (const std::string& format, Args&&... args)
{
#ifdef UNIT_TEST
    printf (std::string (format).append ("\n").c_str (),
            std::forward<Args> (args)...);
    fflush (stdout);
#endif
    Logger::getLogger ()->fatal (format.c_str (),
                                 std::forward<Args> (args)...);
}
}

#endif /* _TASE2_PIVOT_UTILITY_H */