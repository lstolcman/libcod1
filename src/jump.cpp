/*
Inline asm notes

__attribute__ ((naked)): no prologue/epilogue
extern "C": disable name mangling

pushal: save registers
popal: restore registers

asm volatile (
    // instructions
    : // output
    : // input (m = memory, r = register)
    : // clobbered list
);
*/

#include "jump.hpp"

// Custom cvars
extern cvar_t *jump_height;

// Resume addresses
extern uintptr_t resume_addr_Jump_Check;
extern uintptr_t resume_addr_Jump_Check_2;

// This applies the height
__attribute__ ((naked)) void hook_Jump_Check_Naked()
{
    asm volatile (
        "pushal\n"
        "call setJumpHeight\n"
        "popal\n"
        "jmp *%0\n"
        :
        : "r"(resume_addr_Jump_Check)
        : "%eax"
    );
}
extern "C" void setJumpHeight()
{
    float height = jump_height->value * 2;
    asm volatile (
        "fmul %0\n"
        :
        : "m"(height)
        :
    );
}

// This updates ps->jumpTime
__attribute__ ((naked)) void hook_Jump_Check_Naked_2()
{
    asm volatile (
        "pushal\n"
        "call setJumpHeight_2\n"
        "popal\n"
        "jmp *%0\n"
        :
        : "r"(resume_addr_Jump_Check_2)
        : "%eax"
    );
}
extern "C" void setJumpHeight_2()
{
    float height = jump_height->value;
    asm volatile (
        "fadd %0\n"
        :
        : "m"(height)
        :
    );
}