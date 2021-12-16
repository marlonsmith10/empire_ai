/** @file
 * Provides wrappers for OpenTTD command functions as well as helper functions that are tightly coupled to the OpenTTD
 * source code.
 */

#ifndef OPENTTD_FUNCTIONS_HH
#define OPENTTD_FUNCTIONS_HH

#include <string>

#include "stdafx.h"
#include "town.h"


namespace EmpireAI
{

    void rename_company(std::string name);
    void get_money(uint32_t amount);
    void print_town_name(Town* town);

}


#endif // OPENTTD_FUNCTIONS_HH
