#include "jump.hpp"

#if COMPILE_JUMP == 1

extern cvar_t *jump_slowdownEnable;

extern cHook *hook_pm_checkduck;
extern cHook *hook_jump_start;
extern cHook *hook_pm_walkmove;

extern pmove_t *pm;

void custom_PM_CheckDuck()
{
    hook_pm_checkduck->unhook();
    void (*PM_CheckDuck)();
    *(int *)&PM_CheckDuck = hook_pm_checkduck->from;
    PM_CheckDuck();
    hook_pm_checkduck->hook();

    if(!jump_slowdownEnable->integer)
    {
        playerState_t *ps = ((pmove_t*)*((int*)pm))->ps;
        ps->pm_flags &= ~0x2000;
    }
}

void custom_Jump_Start(float height)
{
    hook_jump_start->unhook();
    void (*Jump_Start)(float height);
    *(int *)&Jump_Start = hook_jump_start->from;
    Jump_Start(height);
    hook_jump_start->hook();

    if(!jump_slowdownEnable->integer)
    {
        playerState_t *ps = ((pmove_t*)*((int*)pm))->ps;
        ps->pm_flags &= ~0x2008;
    }
}

void custom_PM_WalkMove()
{
    if(!jump_slowdownEnable->integer)
    {
        playerState_t *ps = ((pmove_t*)*((int*)pm))->ps;
        if(ps->pm_time < 0x709)
        {
            if (ps->pm_time == 0)
            {
                if (ps->origin[2] < ps->jumpOriginZ + 18.0)
                {
                    ps->pm_time = 0x708;
                }
                else
                {
                    ps->pm_time = 0x4b0;
                }
            }
        }
        else
        {
            // Jump_ClearState
            ps->pm_flags &= ~0xffffdfff;
            ps->jumpOriginZ = 0;
        }
    }

    hook_pm_walkmove->unhook();
    void (*PM_WalkMove)();
    *(int *)&PM_WalkMove = hook_pm_walkmove->from;
    PM_WalkMove();
    hook_pm_walkmove->hook();
}

#endif