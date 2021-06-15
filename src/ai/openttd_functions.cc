/// \file

#include "openttd_functions.hh"

#include <iostream>

#include "stdafx.h"
#include "command_func.h"
#include "townname_func.h"


void EmpireAI::rename_company(std::string name)
{
    DoCommandPInternal(0, 0, 0, CMD_RENAME_COMPANY, nullptr, "Empire", false, false);
}

void EmpireAI::get_money(uint32_t amount)
{
    DoCommandP(0, amount, 0, CMD_MONEY_CHEAT);
}

void EmpireAI::print_town_name(Town* town)
{
    if(town == nullptr)
    {
        return;
    }

    char buf[MAX_LENGTH_TOWN_NAME_CHARS * MAX_CHAR_LENGTH];
    char *end = GetTownName(buf, town, lastof(buf));
    (void)end;

    std::cout << "\n" << buf << std::flush;
}
