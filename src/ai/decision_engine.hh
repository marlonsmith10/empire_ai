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

        static Init* _instance;
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

        static FindPath* _instance;

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

        static BuildRoad* _instance;

        RoadBuilder* m_road_builder;
    };
}


#endif // DECISION_ENGINE_HH
