#include "empire_ai.hh"

#include <iostream>
#include "stdafx.h"
#include "script_object.hpp"

#include "command_func.h"

#include "town.h"
#include "townname_func.h"
#include "town_type.h"
#include "debug.h"



#include <vector>

using namespace EmpireAI;


void PrintTownName(Town* town)
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


AI::AI()
: m_state(INIT), m_path(nullptr), m_road_builder(nullptr)
{

}


void AI::game_loop()
{
	ScriptObject::ActiveInstance active(this);

	switch(m_state)
	{
	case INIT:
	{
	    // Set company name and get a bunch of money
	    DoCommandPInternal(0, 0, 0, CMD_RENAME_COMPANY, nullptr, "Empire Transport", false, false);
	    DoCommandP(0, 100000000, 0, CMD_MONEY_CHEAT);

	    // Choose two random towns and create a new Path object between them
	    Town* town1 = Town::GetRandom();
	    Town* town2 = Town::GetRandom();

	    // If the second town is the same as the first, choose a different second town
	    while(town2 == town1)
	    {
	        town2 = Town::GetRandom();
	    }

        PrintTownName(town1);
        PrintTownName(town2);

	    TileIndex town1_location = town1->xy;
	    TileIndex town2_location = town2->xy;

	    if(m_path == nullptr)
	    {
	        m_path = new Path(town1_location, town2_location);
	    }

	    std::cout << "\nFinding path..." << std::flush;
	    m_state = FIND_PATH;
	    break;
	}
	case FIND_PATH:
	{
	    Path::Status find_status = m_path->find(100);
	    if(find_status == Path::FOUND)
	    {
		    std::cout << "\nBuilding road..." << std::flush;
            m_state = BUILD_ROAD;
	    }
	    if(find_status == Path::UNREACHABLE)
	    {
	    	std::cout << "\nDestination unreachable!" << std::flush;
	    	m_state = DONE;
	    }
	    break;
	}
	case BUILD_ROAD:
	    if(m_road_builder == nullptr)
	    {
	        m_road_builder = new RoadBuilder(*m_path);
	    }

	    for(uint8_t count = 0; count < 2; count++)
	    {
			if(m_road_builder->build_road_segment())
			{
				std::cout << "\nConstruction complete!" << std::flush;
				m_state = DONE;
				break;
			}
	    }

	    break;
	case DONE:
	    if(m_path != nullptr)
	    {
	        delete m_path;
	        m_path = nullptr;
	    }
        if(m_road_builder != nullptr)
        {
            delete m_road_builder;
            m_road_builder = nullptr;
        }
	    break;
	default:
	    break;
	}

	return;
}
