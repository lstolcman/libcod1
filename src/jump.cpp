#include "jump.hpp"

#if COMPILE_JUMP == 1

extern cvar_t *jump_slowdownEnable;

#define JUMP_LAND_SLOWDOWN_TIME 1800

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
        ps->pm_flags &= ~0x2000u;
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
        ps->pm_flags &= ~0x2008u;
        ps->pm_flags |= 8;
    }
}

void custom_PM_WalkMove()
{
    if(!jump_slowdownEnable->integer)
    {
        playerState_t *ps = ((pmove_t*)*((int*)pm))->ps;
        if(ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME)
        {
            if (!ps->pm_time)
            {
                if ((float)(ps->jumpOriginZ + 18.0) <= ps->origin[2])
                {
                    ps->pm_time = 1200;
                }
                else
                {
                    ps->pm_time = JUMP_LAND_SLOWDOWN_TIME;
                }
            }
        }
        else
        {
            // Jump_ClearState
            ps->pm_flags &= ~0x2000u;
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