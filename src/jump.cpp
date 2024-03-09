#include "jump.hpp"

#if COMPILE_JUMP == 1

extern cvar_t *jump_slowdownEnable;

extern pmove_t *pm;

extern cHook *hook_pm_walkmove;

void custom_PM_WalkMove()
{
    if(!jump_slowdownEnable->integer)
    {
        playerState_t *ps = ((pmove_t*)*((int*)pm))->ps;
        ps->pm_flags &= ~0x2000u;
        ps->pm_time = 0;
    }
    
    hook_pm_walkmove->unhook();
    void (*PM_WalkMove)();
    *(int *)&PM_WalkMove = hook_pm_walkmove->from;
    PM_WalkMove();
    hook_pm_walkmove->hook();
}

#endif