/// \file

#ifndef DECISION_ENGINE_HH
#define DECISION_ENGINE_HH

#include "path.hh"
#include "road_builder.hh"

namespace EmpireAI
{

    class DecisionEngineState;

    class DecisionEngine
    {
    public:

        DecisionEngine();
        void update();

    private:

        friend class DecisionEngineState;
        void change_state(DecisionEngineState* state);

        DecisionEngineState* m_state;
    };


    class DecisionEngineState
    {
    public:

        virtual ~DecisionEngineState(){}

        virtual void update(DecisionEngine* decision_engine);

    protected:

        void change_state(DecisionEngine* decision_engine, DecisionEngineState* state);
    };


    class Init : public DecisionEngineState
    {
    public:

        static DecisionEngineState* instance();
        void update(DecisionEngine* decision_engine);

    protected:

        Init(){}

    private:

        static Init* m_instance;
    };


    class NewCargoRoute : public DecisionEngineState
    {
    public:

        static DecisionEngineState* instance();
        void update(DecisionEngine* decision_engine);

    protected:

        NewCargoRoute(){}

    private:

        static NewCargoRoute* m_instance;
    };


    class FindPath : public DecisionEngineState
    {
    public:

        static DecisionEngineState* instance();
        void update(DecisionEngine* decision_engine);

        void set_source_and_destination(TileIndex source, TileIndex destination);

    protected:

        FindPath();

    private:

        static const uint16_t SEARCH_NODES_PER_ITERATION = 100;

        static FindPath* m_instance;

        Path* m_path;
    };


    class BuildRoad : public DecisionEngineState
    {
    public:

        static DecisionEngineState* instance();
        void update(DecisionEngine* decision_engine);

        void set_path(Path* path);

    protected:

        BuildRoad();

    private:

        static BuildRoad* m_instance;

        RoadBuilder* m_road_builder;
        Path* m_path;
    };


    class BuildStations : public DecisionEngineState
    {
    public:

        static DecisionEngineState* instance();
        void update(DecisionEngine* decision_engine);

        void set_locations(TileIndex location_1, TileIndex location_2);
        void set_path(Path* path);

    protected:

        BuildStations(){}

    private:

        static BuildStations* m_instance;
        TileIndex m_location_1;
        TileIndex m_location_2;
        Path* m_path;
    };
}


#endif // DECISION_ENGINE_HH
