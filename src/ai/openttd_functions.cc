/// \file

#include "openttd_functions.hh"
#include "script_station.hpp"
#include "script_road.hpp"
#include "script_testmode.hpp"
#include "script_map.hpp"

#include "stdafx.h"
#include "command_func.h"
#include "townname_func.h"

#include <iostream>


void EmpireAI::rename_company(const char* name)
{
    DoCommandPInternal(0, 0, 0, CMD_RENAME_COMPANY, nullptr, name, false, false);
}


void EmpireAI::get_money(uint32_t amount)
{
    DoCommandP(0, amount, 0, CMD_MONEY_CHEAT);
}


void EmpireAI::print_town_name(Town* town)
{
    if(town == nullptr)
    {
        return;
    }

    char buf[MAX_LENGTH_TOWN_NAME_CHARS * MAX_CHAR_LENGTH];
    char *end = GetTownName(buf, town, lastof(buf));
    (void)end;

    std::cout << "\n" << buf << std::flush;
}


bool EmpireAI::build_road(TileIndex start, TileIndex end)
{
    ScriptRoad::SetCurrentRoadType(ScriptRoad::ROADTYPE_ROAD);

    try
    {
        if(!ScriptRoad::BuildRoad(start, end))
        {
            return false;
        }
    }
    catch (Script_Suspend &e)
    {
    }

    return true;
}


bool EmpireAI::build_bus_station(TileIndex tile, TileIndex front)
{
	// Build a station on the first available adjacent tile
    try
    {
        if(!ScriptRoad::BuildRoadStation(tile, front, ScriptRoad::ROADVEHTYPE_BUS, ScriptStation::STATION_NEW))
        {
        	return false;
        }
    }
    catch (Script_Suspend &e)
    {
    }

    return true;
}


bool EmpireAI::build_road_depot(TileIndex tile, TileIndex front)
{
	try
	{
		if(!ScriptRoad::BuildRoadDepot(tile, front))
		{
			return false;
		}
	}
	catch (Script_Suspend &e)
	{
	}

	return true;
}


bool EmpireAI::can_build_road_building(TileIndex tile, TileIndex direction_offset)
{
	ScriptTestMode test_mode;
	return build_bus_station(tile, direction_offset);
}


bool EmpireAI::can_build_road(TileIndex start, TileIndex end)
{
    ScriptTestMode test_mode;
    return build_road(start, end);
}


bool EmpireAI::tile_provides_passengers(TileIndex tile)
{
	if(ScriptTile::GetCargoProduction(tile, CT_PASSENGERS, 1, 1, 3) == 0)
	{
		return false;
	}

	return true;
}


TileIndex EmpireAI::get_tile_index(uint32_t x, uint32_t y)
{
    return ScriptMap::GetTileIndex(x, y);
}
