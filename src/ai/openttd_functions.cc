/// \file

#include "openttd_functions.hh"

#include <iostream>

#include "stdafx.h"
#include "command_func.h"
#include "townname_func.h"

#include "script_station.hpp"
#include "script_road.hpp"


void EmpireAI::rename_company(std::string name)
{
    DoCommandPInternal(0, 0, 0, CMD_RENAME_COMPANY, nullptr, "Empire", false, false);
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


bool EmpireAI::build_road(TileIndex tile, TileIndex direction_offset)
{
    try
    {
        if(!ScriptRoad::BuildRoad(tile + direction_offset, tile))
        {
        	return false;
		}
    }
    catch (Script_Suspend &e)
    {
    }

    return true;
}


bool EmpireAI::build_bus_station(TileIndex tile, TileIndex direction_offset)
{
	// Build a station on the first available adjacent tile
    try
    {
        if(!ScriptRoad::BuildRoadStation(tile + direction_offset, tile, ScriptRoad::ROADVEHTYPE_BUS, ScriptStation::STATION_NEW))
        {
        	return false;
        }
    }
    catch (Script_Suspend &e)
    {
    }

    if(!build_road(tile, direction_offset))
    {
    	return false;
    }

    return true;
}


bool EmpireAI::build_road_depot(TileIndex tile, TileIndex direction_offset)
{
	try
	{
		if(!ScriptRoad::BuildRoadDepot(tile + direction_offset, tile))
		{
			return false;
		}
	}
	catch (Script_Suspend &e)
	{
	}

	if(!build_road(tile, direction_offset))
	{
		return false;
	}

	return true;
}


bool EmpireAI::can_build_road_building(TileIndex tile, TileIndex direction_offset)
{
	ScriptTestMode test_mode;
	return build_bus_station(tile, direction_offset);
}


bool EmpireAI::tile_provides_passengers(TileIndex tile)
{
	if(ScriptTile::GetCargoProduction(tile, CT_PASSENGERS, 1, 1, 3) == 0)
	{
		return false;
	}

	return true;
}

