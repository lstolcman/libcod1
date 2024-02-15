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
    {"setVelocity", gsc_player_setvelocity, 0},
    {"getVelocity", gsc_player_getvelocity, 0},
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

int stackGetParams(const char *params, ...)
{
	va_list args;
	va_start(args, params);

	int errors = 0;

	for ( size_t i = 0; i < strlen(params); i++ )
	{
		switch ( params[i] )
		{
		case ' ': // Ignore param
			break;

		case 'i':
		{
			int *tmp = va_arg(args, int *);
			if ( !stackGetParamInt(i, tmp) )
			{
				Com_DPrintf("\nstackGetParams() Param %i is not an int\n", i);
				errors++;
			}
			break;
		}

		case 'v':
		{
			float *tmp = va_arg(args, float *);
			if ( !stackGetParamVector(i, tmp) )
			{
				Com_DPrintf("\nstackGetParams() Param %i is not a vector\n", i);
				errors++;
			}
			break;
		}

		case 'f':
		{
			float *tmp = va_arg(args, float *);
			if ( ! stackGetParamFloat(i, tmp) )
			{
				Com_DPrintf("\nstackGetParams() Param %i is not a float\n", i);
				errors++;
			}
			break;
		}

		case 's':
		{
			char **tmp = va_arg(args, char **);
			if ( !stackGetParamString(i, tmp) )
			{
				Com_DPrintf("\nstackGetParams() Param %i is not a string\n", i);
				errors++;
			}
			break;
		}

		case 'c':
		{
			unsigned int *tmp = va_arg(args, unsigned int *);
			if ( !stackGetParamConstString(i, tmp) )
			{
				Com_DPrintf("\nstackGetParams() Param %i is not a const string\n", i);
				errors++;
			}
			break;
		}

		case 'l':
		{
			char **tmp = va_arg(args, char **);
			if ( !stackGetParamLocalizedString(i, tmp) )
			{
				Com_DPrintf("\nstackGetParams() Param %i is not a localized string\n", i);
				errors++;
			}
			break;
		}

		default:
			errors++;
			Com_DPrintf("\nUnknown identifier [%c] passed to stackGetParams()\n", params[i]);
			break;
		}
	}

	va_end(args);
	return errors == 0; // success if no errors
}




int stackGetParamInt(int param, int *value)
{
	if ( param >= Scr_GetNumParam() )
		return 0;

	VariableValue *var;
	var = &scrVmPub.top[-param];

	if ( var->type == STACK_FLOAT )
	{
		*value = var->u.floatValue;
		return 1;
	}

	if ( var->type != STACK_INT )
		return 0;

	*value = var->u.intValue;

	return 1;
}

int stackGetParamString(int param, char **value)
{
	if ( param >= Scr_GetNumParam() )
		return 0;

	VariableValue *var;
	var = &scrVmPub.top[-param];

	if ( var->type != STACK_STRING )
		return 0;

	*value = SL_ConvertToString(var->u.stringValue);

	return 1;
}

int stackGetParamConstString(int param, unsigned int *value)
{
	if ( param >= Scr_GetNumParam() )
		return 0;

	VariableValue *var;
	var = &scrVmPub.top[-param];

	if ( var->type != STACK_STRING )
		return 0;

	*value = var->u.stringValue;

	return 1;
}

int stackGetParamLocalizedString(int param, char **value)
{
	if ( param >= Scr_GetNumParam() )
		return 0;

	VariableValue *var;
	var = &scrVmPub.top[-param];

	if ( var->type != STACK_LOCALIZED_STRING )
		return 0;

	*value = SL_ConvertToString(var->u.stringValue);

	return 1;
}

int stackGetParamVector(int param, vec3_t value)
{
	if ( param >= Scr_GetNumParam() )
		return 0;

	//VariableValue *var;
	//var = &scrVmPub.top[-param];

	
	
	printf("####### TEST INCOMING \n");



	unsigned int* test = &scrVmPub.inparamcount;

	printf("####### test = %u \n", *test);

	



	return 1;


	//if ( var->type != STACK_VECTOR )
		//return 0;

	//printf("####### if ( var->type PASS \n");

	//VectorCopy(var->u.vectorValue, value);

	//return 1;
}

int stackGetParamFloat(int param, float *value)
{
	if ( param >= Scr_GetNumParam() )
		return 0;

	VariableValue *var;
	var = &scrVmPub.top[-param];

	if ( var->type == STACK_INT )
	{
		*value = var->u.intValue;
		return 1;
	}

	if ( var->type != STACK_FLOAT )
		return 0;

	*value = var->u.floatValue;

	return 1;
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