#ifndef _FUNCTIONS_HPP_
#define _FUNCTIONS_HPP_

/* MAKE FUNCTIONS STATIC, SO THEY CAN BE IN EVERY FILE */

typedef unsigned int (*GetVariableName_t)(unsigned int a1);
#if COD_VERSION == COD1_1_1
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x080a3060;
#elif COD_VERSION == COD1_1_5

#endif

typedef unsigned int (*GetNextVariable_t)(unsigned int a1);
#if COD_VERSION == COD1_1_1
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x080a3028;
#elif COD_VERSION == COD1_1_5

#endif





typedef char * (*Cmd_Argv_t)(int arg);
#if COD_VERSION == COD1_1_1
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x0805b258;
#elif COD_VERSION == COD1_1_5

#endif






typedef void (*Com_Printf_t)(const char *format, ...);
#if COD_VERSION == COD1_1_1
static const Com_Printf_t Com_Printf = (Com_Printf_t)0x0806b760;
#elif COD_VERSION == COD1_1_5

#endif




typedef void (*Com_PrintMessage_t)(int channel, const char *message);
#if COD_VERSION == COD1_1_1
static const Com_PrintMessage_t Com_PrintMessage = (Com_PrintMessage_t)0x0806b530;
#elif COD_VERSION == COD1_1_5

#endif





typedef xmethod_t (*Scr_GetMethod_t)(const char** v_methodName, qboolean *v_developer);
extern Scr_GetMethod_t Scr_GetMethod;




typedef cvar_t * (*Cvar_FindVar_t)(const char *var_name);
#if COD_VERSION == COD1_1_1
static const Cvar_FindVar_t Cvar_FindVar = (Cvar_FindVar_t)0x0806e9b4;
#elif COD_VERSION == COD1_1_5

#endif








typedef cvar_t * (*Cvar_Get_t)(const char *var_name, const char *var_value, unsigned short flags);
#if COD_VERSION == COD1_1_1
static const Cvar_Get_t Cvar_Get = (Cvar_Get_t)0x0806ea34;
#elif COD_VERSION == COD1_1_5

#endif


/*
typedef char * (*Cvar_VariableString_t)(const char *var_name);
#if COD_VERSION == COD1_1_1
static const Cvar_VariableString_t Cvar_VariableString = (Cvar_VariableString_t)0x0806f910;
#elif COD_VERSION == COD1_1_5

#endif
*/




typedef void (*Scr_Error_t)(const char *string);
extern Scr_Error_t Scr_Error;








typedef void (*Scr_AddString_t)(const char *string);
extern Scr_AddString_t Scr_AddString;


typedef void (*Scr_AddUndefined_t)(void);
extern Scr_AddUndefined_t Scr_AddUndefined;






typedef playerState_t * (*SV_GameClientNum_t)(int num);
#if COD_VERSION == COD1_1_1
static const SV_GameClientNum_t SV_GameClientNum = (SV_GameClientNum_t)0x08089270;
#elif COD_VERSION == COD1_1_5

#endif







#endif