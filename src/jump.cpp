#include "jump.hpp"

#if COMPILE_JUMP == 1

extern cvar_t *jump_slowdownEnable;

#define JUMP_LAND_SLOWDOWN_TIME 1800

extern cHook *hook_pm_checkduck;
extern cHook *hook_jump_start;

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
        ps->pm_time -= JUMP_LAND_SLOWDOWN_TIME;
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
        ps->pm_flags &= ~0x2000u;
    }
}

#endif