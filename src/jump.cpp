#include "jump.hpp"

#if COMPILE_JUMP == 1

extern cvar_t *jump_slowdownEnable;
extern pmove_t *pm;
extern cHook *hook_pm_walkmove;

void Jump_ActivateSlowdown()
{
    if(!jump_slowdownEnable->integer)
    {
        // Disable jump_slowdown

        playerState_t *ps = ((pmove_t*)*((int*)pm))->ps;
        if(ps->pm_flags & 0x2000)
        {
            if(ps->pm_time < 0x709)
            {
                ps->pm_flags &= ~0x2000;
            }
            else
            {
                ps->pm_flags &= 0xffffdfff;
                ps->jumpOriginZ = 0.0;
            }
        }
    }

    hook_pm_walkmove->unhook();
    void (*PM_WalkMove)();
    *(int *)&PM_WalkMove = hook_pm_walkmove->from;
    PM_WalkMove();
    hook_pm_walkmove->hook();
}

#endif