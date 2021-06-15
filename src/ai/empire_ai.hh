#ifndef EMPIRE_AI_HH
#define EMPIRE_AI_HH

#include "stdafx.h"
#include "../../../ai_instance.hpp"

#include "decision_engine.hh"

namespace EmpireAI
{
	class AI : public AIInstance
	{
	public:

        AI();

		void game_loop();

	private:

		DecisionEngine m_decision_engine;
	};
};


#endif // EMPIRE_AI_HH
