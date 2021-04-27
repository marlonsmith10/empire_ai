#ifndef EMPIRE_AI_HH
#define EMPIRE_AI_HH

#include "stdafx.h"
#include "../../../ai_instance.hpp"

#include "path.hh"
#include "road_builder.hh"

namespace EmpireAI
{

    enum State
    {
        INIT,
        FIND_PATH,
        BUILD_ROAD,
        DONE
    };

	class AI : public AIInstance
	{
	public:

        AI();

		void game_loop();

	private:

		State m_state;

		Path* m_path;

		RoadBuilder* m_road_builder;
	};

};


#endif // EMPIRE_AI_HH
