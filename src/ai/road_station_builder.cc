#include "road_station_builder.hh"
#include "openttd_functions.hh"
#include "script_map.hpp"

using namespace EmpireAI;


RoadStationBuilder::RoadStationBuilder(Path& path)
: m_path(path)
{

}


bool RoadStationBuilder::build_bus_stations()
{
    // Create an array of TileIndexes corresponding to N,S,E,W offsets
    std::array<TileIndex, 4> offsets;
    offsets[0] = ScriptMap::GetTileIndex(1, 0);
    offsets[1] = ScriptMap::GetTileIndex(-1, 0);
    offsets[2] = ScriptMap::GetTileIndex(0, 1);
    offsets[3] = ScriptMap::GetTileIndex(0, -1);

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
            if(can_build_road_building(*iterator, offset))
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

    // Build first bus station
    EmpireAI::build_bus_station(first_station_tile, first_station_offset);

    // Build second bus station
    EmpireAI::build_bus_station(second_station_tile, second_station_offset);

    // Build road depot
    EmpireAI::build_road_depot(road_depot_tile, road_depot_offset);

    return true;
}
