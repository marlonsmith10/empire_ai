/// \file

#include "decision_engine.hh"
#include "openttd_functions.hh"

#include <iostream>
#include <vector>

#include "stdafx.h"
#include "town.h"

#include "script_map.hpp"
#include "script_road.hpp"


using namespace EmpireAI;


DecisionEngine::DecisionEngine()
{
    m_state = Init::instance();
}


void DecisionEngine::change_state(DecisionEngineState* state)
{
    m_state = state;
}


void DecisionEngine::update()
{
    m_state->update(this);
}


void DecisionEngineState::update(DecisionEngine* decision_engine)
{

}


void DecisionEngineState::change_state(DecisionEngine* decision_engine, DecisionEngineState* state)
{
    decision_engine->change_state(state);
}


Init* Init::m_instance = nullptr;


DecisionEngineState* Init::instance()
{
    if(m_instance == nullptr)
    {
        m_instance = new Init();
    }

    return m_instance;
}


void Init::update(DecisionEngine* decision_engine)
{
    std::cout << "\nInit" << std::flush;

    // For testing, get some free money
    get_money(100000000);

    // Set company name
    rename_company("Empire Transport");

    std::cout << "\nChoosing cargo" << std::flush;

    NewCargoRoute* new_cargo_route = static_cast<NewCargoRoute*>(NewCargoRoute::instance());
    change_state(decision_engine, new_cargo_route);
}


NewCargoRoute* NewCargoRoute::m_instance = nullptr;


/// \todo: Use a template for all the instance functions?
DecisionEngineState* NewCargoRoute::instance()
{
    if(m_instance == nullptr)
    {
        m_instance = new NewCargoRoute();
    }

    return m_instance;
}


#include "script_cargolist.hpp"
#include "script_industrylist.hpp"
#include "script_tilelist.hpp"

void NewCargoRoute::update(DecisionEngine* decision_engine)
{
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

    std::cout << "\nFinding path" << std::flush;

    // Pass the source and destination into BuildStations, to be used once a path is found
    BuildStations* build_stations = static_cast<BuildStations*>(BuildStations::instance());
    build_stations->set_locations(town1_location, town2_location);

    FindPath* find_path = static_cast<FindPath*>(FindPath::instance());
    find_path->set_source_and_destination(town1_location, town2_location);
    change_state(decision_engine, find_path);
}


FindPath* FindPath::m_instance = nullptr;


FindPath::FindPath()
{
    m_path = nullptr;
}


DecisionEngineState* FindPath::instance()
{
    if(m_instance == nullptr)
    {
        m_instance = new FindPath();
    }

    return m_instance;
}


void FindPath::set_source_and_destination(TileIndex source, TileIndex destination)
{
    if(m_path != nullptr)
    {
        delete m_path;
    }

    m_path = new Path(source, destination);
}


void FindPath::update(DecisionEngine* decision_engine)
{
    Path::Status find_status = m_path->find(100);
    if(find_status == Path::FOUND)
    {
        std::cout << "\nPath found, building road" << std::flush;

        BuildRoad* build_road = static_cast<BuildRoad*>(BuildRoad::instance());
        build_road->set_path(m_path);
        change_state(decision_engine, build_road);
    }
    if(find_status == Path::UNREACHABLE)
    {
        std::cout << "\nDestination unreachable" << std::flush;
        change_state(decision_engine, Init::instance());
    }
}


BuildRoad* BuildRoad::m_instance = nullptr;


BuildRoad::BuildRoad()
{
    m_road_builder = nullptr;
}


DecisionEngineState* BuildRoad::instance()
{
    if(m_instance == nullptr)
    {
        m_instance = new BuildRoad();
    }

    return m_instance;
}


void BuildRoad::set_path(Path* path)
{
    if(m_road_builder != nullptr)
    {
        delete m_road_builder;
    }

    m_road_builder = new RoadBuilder(*path);
    m_path = path;
}


void BuildRoad::update(DecisionEngine* decision_engine)
{
    for(uint8_t count = 0; count < 2; count++)
    {
        if(m_road_builder->build_road_segment())
        {
            std::cout << "\nRoad construction complete, building stations" << std::flush;

            BuildStations* build_stations = static_cast<BuildStations*>(BuildStations::instance());
            build_stations->set_path(m_path);
            change_state(decision_engine, build_stations);
            break;
        }
    }
}


BuildStations* BuildStations::m_instance = nullptr;


/// \todo: Use a template for all the instance functions?
DecisionEngineState* BuildStations::instance()
{
    if(m_instance == nullptr)
    {
        m_instance = new BuildStations();
    }

    return m_instance;
}


void BuildStations::set_path(Path* path)
{
	m_path = path;
}


void BuildStations::update(DecisionEngine* decision_engine)
{
	std::cout << "\nLocation 1:" << TileX(m_location_1) << "," << TileY(m_location_1) << std::flush;
	//look into ScriptTile::GetCargoProduction to see how it searches a radius of tiles, since we need to search a radius out from the town center or industry center to find
	//suitable places to build a station


	/// \todo: Spiral outwards from town center and find the first available tile to build a station. Or better, do a tree search following roads away from the town
	/// center to find the first available tile.

	// Create an array of TileIndexes corresponding to N,S,E,W offsets
	std::array<TileIndex, 4> offsets;
	offsets[0] = ScriptMap::GetTileIndex(1, 0);
	offsets[1] = ScriptMap::GetTileIndex(-1, 0);
	offsets[2] = ScriptMap::GetTileIndex(0, 1);
	offsets[3] = ScriptMap::GetTileIndex(0, -1);

	TileIndex first_station_tile = 0;
	TileIndex first_station_offset = 0;
	TileIndex last_station_tile = 0;
	TileIndex last_station_offset = 0;

	// Follow the path between the two towns and first the first available tile for each side that can handle a station
	for(Path::Iterator iterator = m_path->begin(); iterator != m_path->end(); iterator++)
	{
		std::cout << "\nIterate tile: " << TileX(*iterator) << "," << TileY(*iterator) << std::flush;

		// Check adjacent tiles to see if one is available for a station and also provides passengers
		for(uint8_t index = 0; index < 4; index++)
		{
			if(parse_adjacent_tile(*iterator, offsets[index]) == true)
			{
				if(first_station_tile == 0)
				{
					first_station_tile = *iterator;
					first_station_offset = offsets[index];
				}

				last_station_tile = *iterator;
				last_station_offset = offsets[index];
			}
		}
	}

	// Build a station on the first available adjacent tile
	try
	{
		ScriptRoad::BuildRoadStation(first_station_tile + first_station_offset, first_station_tile, ScriptRoad::ROADVEHTYPE_BUS, ScriptStation::STATION_NEW);
	}
	catch (Script_Suspend &e)   /// \todo: Figure out what other exceptions to watch for
	{

	}

	// Build a station on the last available adjacent tile
	try
	{
		ScriptRoad::BuildRoadStation(last_station_tile + last_station_offset, last_station_tile, ScriptRoad::ROADVEHTYPE_BUS, ScriptStation::STATION_NEW);
	}
	catch (Script_Suspend &e)   /// \todo: Figure out what other exceptions to watch for
	{

	}

	std::cout << "\nConstruction complete!" << std::flush;
    change_state(decision_engine, Init::instance());
}


bool BuildStations::parse_adjacent_tile(const TileIndex road_tile_index, const TileIndex offset_tile_index)
{
    TileIndex station_tile_index = road_tile_index + offset_tile_index;
    TileIndex previous_tile_index = station_tile_index + offset_tile_index;

    std::cout << "\nCheck tile support station: " << TileX(station_tile_index) << "," << TileY(station_tile_index) << std::flush;

    std::cout << "\nCheck connect tiles: "
    		<< "\n" << TileX(station_tile_index) << "," << TileY(station_tile_index)
			<< "\n" << TileX(previous_tile_index) << "," << TileY(previous_tile_index)
			<< "\n" << TileX(road_tile_index) << "," << TileY(road_tile_index);

	int32 can_connect = ScriptRoad::CanBuildConnectedRoadPartsHere(station_tile_index, previous_tile_index, road_tile_index);

	if(can_connect <= 0)
	{
		std::cout << "\nCan't connect" << std::flush;
		return false;
	}

	if(!ScriptTile::IsBuildable(station_tile_index) || ScriptRoad::IsRoadTile(station_tile_index))
	{
		std::cout << "\nNot buildable" << std::flush;
		return false;
	}

	if(ScriptTile::GetCargoProduction(station_tile_index, CT_PASSENGERS, 1, 1, 3) == 0)
	{
		std::cout << "\nNo passengers" << std::flush;
		return false;
	}

	std::cout << "\nGood to build station!" << std::flush;

	return true;
}


void BuildStations::set_locations(TileIndex location_1, TileIndex location_2)
{
	m_location_1 = location_1;
	m_location_2 = location_2;
}
