#include "empire_ai.hh"

#include "script_object.hpp"


using namespace EmpireAI;


AI::AI()
{
}


void AI::game_loop()
{
	ScriptObject::ActiveInstance active(this);

	m_decision_engine.update();
}
