#ifndef ROAD_STATION_BUILDER_HH
#define ROAD_STATION_BUILDER_HH

#include "path.hh"

namespace EmpireAI
{
    /**
     * Class to build road stations and depots along a provided path.
     */
    class RoadStationBuilder
    {
    public:

        RoadStationBuilder(Path& path);

        bool build_bus_stations();

    private:

        Path& m_path;
    };
}


#endif // ROAD_STATION_BUILDER_HH
