/// \file

#include "decision_engine.hh"
#include "openttd_functions.hh"

#include <iostream>
#include <vector>

#include "stdafx.h"
#include "town.h"


using namespace EmpireAI;


DecisionEngine::DecisionEngine()
: m_state(INIT), m_path(nullptr), m_road_builder(nullptr)
{

}


void DecisionEngine::update()
{
    switch(m_state)
    {
    case INIT:
    {
        // For testing, get some free money
        get_money(100000000);

        // Set company name
        rename_company("Empire Transport");

        // Choose two random towns and create a new Path object between them
        Town* town1 = Town::GetRandom();
        Town* town2 = Town::GetRandom();

        // If the second town is the same as the first, choose a different second town
        while(town2 == town1)
        {
            town2 = Town::GetRandom();
        }

        print_town_name(town1);
        print_town_name(town2);

        TileIndex town1_location = town1->xy;
        TileIndex town2_location = town2->xy;

        if(m_path == nullptr)
        {
            m_path = new Path(town1_location, town2_location);
        }

        std::cout << "\nFinding path...!" << std::flush;
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
