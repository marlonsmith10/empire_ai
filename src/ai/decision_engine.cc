/// \file

#include "decision_engine.hh"
#include "openttd_functions.hh"
#include "road_station_builder.hh"

#include <iostream>


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
    // For testing, get some free money
    get_money(100000000);

    // Set company name
    rename_company("Empire Transport");

    std::cout << "\nChoosing cargo route" << std::flush;
    NewCargoRoute* new_cargo_route = static_cast<NewCargoRoute*>(NewCargoRoute::instance());
    change_state(decision_engine, new_cargo_route);
}


NewCargoRoute* NewCargoRoute::m_instance = nullptr;


DecisionEngineState* NewCargoRoute::instance()
{
    if(m_instance == nullptr)
    {
        m_instance = new NewCargoRoute();
    }

    return m_instance;
}


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
    m_path = nullptr;
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
    RoadStationBuilder road_station_builder(*m_path);
    road_station_builder.build_bus_stations();

    change_state(decision_engine, Init::instance());
}


void BuildStations::set_locations(TileIndex location_1, TileIndex location_2)
{
	m_location_1 = location_1;
	m_location_2 = location_2;
}
