#ifndef _GSC_JUMP_HPP_
#define _GSC_JUMP_HPP_

/* gsc functions */
#include "gsc.hpp"

__attribute__ ((naked)) void Jump_ApplySlowdown_Stub();
__attribute__ ((naked)) void hook_PM_SlideMove_Stub();

extern "C" void Jump_ApplySlowdown();
extern "C" void hook_PM_SlideMove(float primal_velocity_1, float primal_velocity_2, float primal_velocity_3);

double custom_Jump_GetLandFactor();
double custom_PM_GetReducedFriction();

#endif