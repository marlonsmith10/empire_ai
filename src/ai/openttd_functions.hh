/** @file
 * Provides wrappers for OpenTTD command functions as well as helper functions that are tightly coupled to the OpenTTD
 * source code.
 */

#ifndef OPENTTD_FUNCTIONS_HH
#define OPENTTD_FUNCTIONS_HH

#include <string>

#include "stdafx.h"
#include "town.h"
#include "script_testmode.hpp"


namespace EmpireAI
{
    void rename_company(std::string name);
    void get_money(uint32_t amount);
    void print_town_name(Town* town);

    bool build_bus_station(TileIndex tile, TileIndex direction_offset);
    bool build_road_depot(TileIndex tile, TileIndex direction_offset);
    bool build_road(TileIndex tile, TileIndex direction_offset);

    bool can_build_road_building(TileIndex tile, TileIndex direction_offset);

    bool tile_provides_passengers(TileIndex tile);
}


#endif // OPENTTD_FUNCTIONS_HH
