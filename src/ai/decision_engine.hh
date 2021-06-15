/// \file

#ifndef DECISION_ENGINE_HH
#define DECISION_ENGINE_HH

#include "path.hh"
#include "road_builder.hh"

namespace EmpireAI
{

    class DecisionEngine
    {
    public:

        DecisionEngine();

        void update();

    private:

        enum State
        {
            INIT,
            FIND_PATH,
            BUILD_ROAD,
            DONE
        } m_state;

        Path* m_path;

        RoadBuilder* m_road_builder;
    };

}


#endif // DECISION_ENGINE_HH
