#include "road_station_builder.hh"
#include "openttd_functions.hh"

using namespace EmpireAI;


RoadStationBuilder::RoadStationBuilder(Path& path)
: m_path(path)
{

}


/// Iterates through a path and builds a bus station as close to the start and end
/// as possible on tiles that provide and accept passengers. Also builds a road
/// depot along the same path.
bool RoadStationBuilder::build_bus_stations()
{
    // Create an array of TileIndexes corresponding to N,S,E,W offsets
    std::array<TileIndex, 4> offsets;
    offsets[0] = EmpireAI::get_tile_index(1, 0);
    offsets[1] = EmpireAI::get_tile_index(-1, 0);
    offsets[2] = EmpireAI::get_tile_index(0, 1);
    offsets[3] = EmpireAI::get_tile_index(0, -1);

    TileIndex first_station_tile = INVALID_TILE;
    TileIndex first_station_offset = INVALID_TILE;
    TileIndex road_depot_tile = INVALID_TILE;
    TileIndex road_depot_offset = INVALID_TILE;
    TileIndex second_station_tile = INVALID_TILE;
    TileIndex second_station_offset = INVALID_TILE;

    // Follow the path between the two towns and find available tiles for building stations and depot
    for(Path::Iterator iterator = m_path.begin(); iterator != m_path.end(); iterator++)
    {
        // Check tiles adjacent to the road for ability to support stations and provide passengers
        for(const TileIndex offset : offsets)
        {
            if(can_build_road_building(*iterator + offset, *iterator) && can_build_road(*iterator + offset, *iterator))
            {
                // Build first station on the first available space
                if(first_station_tile == INVALID_TILE && tile_provides_passengers(*iterator))
                {
                    first_station_tile = *iterator;
                    first_station_offset = offset;
                    continue;
                }

                // Build road depot on the next available space
                if(road_depot_tile == INVALID_TILE)
                {
                    road_depot_tile = *iterator;
                    road_depot_offset = offset;
                    continue;
                }

                // Build second station on the last available space
                if(tile_provides_passengers(*iterator))
                {
                    second_station_tile = *iterator;
                    second_station_offset = offset;
                }
            }
        }
    }

    // If any of the stations can't be built, don't build any of them
    if(first_station_tile == INVALID_TILE ||
        road_depot_tile == INVALID_TILE ||
        second_station_tile == INVALID_TILE)
    {
        return false;
    }

    // Build roads, stations, and depot. Note that return values aren't checked since
    // can_build_road_building() and can_build_road() were called previously
    EmpireAI::build_bus_station(first_station_tile + first_station_offset, first_station_tile);
    EmpireAI::build_road(first_station_tile + first_station_offset, first_station_tile);
    EmpireAI::build_bus_station(second_station_tile + second_station_offset, second_station_tile);
    EmpireAI::build_road(second_station_tile + second_station_offset, second_station_tile);
    EmpireAI::build_road_depot(road_depot_tile + road_depot_offset, road_depot_tile);
    EmpireAI::build_road(road_depot_tile + road_depot_offset, road_depot_tile);

    return true;
}
