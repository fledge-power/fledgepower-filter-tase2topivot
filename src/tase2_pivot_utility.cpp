/*
 * FledgePower HNZ <-> pivot filter utility functions.
 *
 * Copyright (c) 2023, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *

 *
 */

#include "tase2_pivot_utility.hpp"
#include <sstream>

std::string
TASE2PivotUtility::join (const std::vector<std::string>& list,
                         const std::string& sep /*= ", "*/)
{
    std::string ret;
    for (const auto& str : list)
    {
        if (!ret.empty ())
        {
            ret += sep;
        }
        ret += str;
    }
    return ret;
}
