#include "gsc.hpp"
#include "libcod.hpp"

#include <stdint.h>
#include <sys/time.h>

const char *getParamTypeAsString(int type)
{
	switch ( type )
	{
		case 0: return "UNDEFINED";
		case 1: return "OBJECT";
		case 2: return "STRING";
		case 3: return "LOCALIZED_STRING";
		case 4: return "VECTOR";
		case 5: return "FLOAT";
		case 6: return "INT";
		case 7: return "CODEPOS";
		case 8: return "PRECODEPOS";
		case 9: return "FUNCTION";
		case 10: return "STACK";
		case 11: return "ANIMATION";
		case 12: return "DEVELOPER_CODEPOS";
		case 13: return "INCLUDE_CODEPOS";
		case 14: return "THREAD_LIST";
		case 15: return "THREAD_1";
		case 16: return "THREAD_2";
		case 17: return "THREAD_3";
		case 18: return "THREAD_4";
		case 19: return "STRUCT";
		case 20: return "REMOVED_ENTITY";
		case 21: return "ENTITY";
		case 22: return "ARRAY";
		case 23: return "REMOVED_THREAD";
		default: return "UNKNOWN TYPE";
	}
}






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

	{"getStance", gsc_player_getstance, 0},

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