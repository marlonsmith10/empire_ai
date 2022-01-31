#ifndef ROAD_STATION_BUILDER_HH
#define ROAD_STATION_BUILDER_HH

#include "path.hh"

namespace EmpireAI
{

    class RoadStationBuilder
    {
    public:

        RoadStationBuilder(Path& path);

        // Iterates through a path and builds a bus station at the start and end
        bool build_bus_stations();

        // Builds a road depot accessible to the provided path
        bool build_road_depot();

    private:

        Path& m_path;
    };
}


#endif // ROAD_STATION_BUILDER_HH
