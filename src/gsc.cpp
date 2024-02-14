#include "gsc.hpp"
#include "libcod.hpp"

#include <stdint.h>
#include <sys/time.h>







void NULL_FUNC_ENTITY(scr_entref_t id) {}
void NULL_FUNC(void) {}

scr_function_t scriptFunctions[] =
{


    //{"sqrt", gsc_utils_sqrt, 0},
    

    {NULL, NULL, 0} // Terminator
};

scr_method_t scriptMethods[] =
{


    #if COMPILE_ENTITY == 1

    #if 0
    {"addEntityVelocity", gsc_entity_addentityvelocity, 0},
    {"disableBounce", gsc_entity_disablebounce, 0},
    {"setLight", gsc_entity_setlight, 0},
    #endif

    #endif


    #if COMPILE_PLAYER == 1

    
    {"leanleftButtonPressed", gsc_player_button_leanleft, 0},
    {"leanrightButtonPressed", gsc_player_button_leanright, 0},
    {"reloadButtonPressed", gsc_player_button_reload, 0},
    {"getPlayerAngles", gsc_player_gettagangles, 0},
    {"getStance", gsc_player_getstance, 0},
    {"getIP", gsc_player_getip, 0},

    #endif


    {NULL, NULL, 0} // Terminator
};







xmethod_t Scr_GetCustomMethod(const char **fname, qboolean *fdev)
{
    xmethod_t m = Scr_GetMethod(fname, fdev);
    
    if ( m )
        return m;

    for ( int i = 0; scriptMethods[i].name; i++ )
    {
        if ( strcasecmp(*fname, scriptMethods[i].name) )
            continue;

        scr_method_t func = scriptMethods[i];

        *fname = func.name;
        *fdev = func.developer;

        return func.call;
    }


    return NULL;
}






void stackError(const char *format, ...)
{
    char s[MAX_STRINGLENGTH];
    int len = 0;
    va_list va;

    va_start(va, format);
    Q_vsnprintf(s, sizeof(s) - 1, format, va);
    va_end(va);

    len = strlen(s);
    s[len] = '\n';
    s[len + 1] = '\0';
    Com_PrintMessage(0, s);
    //Scr_CodeCallback_Error(qfalse, qfalse, "stackError", s);
    Scr_Error(s); //TODO: check if need call Scr_CodeCallback_Error instead
}






/**
 * @brief Base time in seconds
 */
time_t sys_timeBase = 0;

/**
 * @brief Current time in ms, using sys_timeBase as origin
 */
uint64_t Sys_MilliSeconds64(void)
{
    struct timeval tp;

    gettimeofday(&tp, NULL);

    if ( !sys_timeBase )
    {
        sys_timeBase = tp.tv_sec;
        return tp.tv_usec / 1000;
    }

    return (tp.tv_sec - sys_timeBase) * 1000 + tp.tv_usec / 1000;
}