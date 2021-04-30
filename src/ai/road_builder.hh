#ifndef ROAD_BUILDER_HH
#define ROAD_BUILDER_HH

#include "path.hh"

namespace EmpireAI
{

    class RoadBuilder
    {
    public:

        RoadBuilder(Path& path);

        // Iterates through a path object and builds a road along the path
        bool build_road_segment();

    private:

        Path& m_path;

        Path::Iterator m_previous_path_iterator;
        Path::Iterator m_current_path_iterator;
    };
}


#endif // ROAD_BUILDER_HH
