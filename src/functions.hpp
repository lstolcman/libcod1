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






typedef char * (*SL_ConvertToString_t)(unsigned int index);
#if COD_VERSION == COD1_1_1
static const SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x0809cac4;
#elif COD_VERSION == COD1_1_5

#endif








typedef int (*Scr_GetNumParam_t)(void);
extern Scr_GetNumParam_t Scr_GetNumParam;







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




typedef void (*Com_DPrintf_t)(const char *format, ...);
#if COD_VERSION == COD1_1_1
static const Com_DPrintf_t Com_DPrintf = (Com_DPrintf_t)0x0806b79c;
#elif COD_VERSION == COD1_1_5

#endif





typedef int (*Com_sprintf_t)(char *dest, int size, const char *format, ...);
#if COD_VERSION == COD1_1_1
static const Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x080823a0;
#elif COD_VERSION == COD1_1_5

#endif



typedef void (*Com_PrintMessage_t)(int channel, const char *message);
#if COD_VERSION == COD1_1_1
static const Com_PrintMessage_t Com_PrintMessage = (Com_PrintMessage_t)0x0806b530;
#elif COD_VERSION == COD1_1_5

#endif





typedef xfunction_t (*Scr_GetFunction_t)(const char** v_functionName, qboolean *v_developer);
extern Scr_GetFunction_t Scr_GetFunction;





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





typedef void (*SV_GameSendServerCommand_t)(int clientnum, int svscmd_type, const char *text);
extern SV_GameSendServerCommand_t SV_GameSendServerCommand;





typedef void (QDECL *SV_SendServerCommand_t)(client_t *cl, int type, const char *fmt, ...);
#if COD_VERSION == COD1_1_1
static const SV_SendServerCommand_t SV_SendServerCommand = (SV_SendServerCommand_t)0x0808b900;
#elif COD_VERSION == COD1_1_5

#endif





typedef void (*SV_DropClient_t)(client_t *drop, const char *reason);
#if COD_VERSION == COD1_1_1
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x08085cf4;
#elif COD_VERSION == COD1_1_5

#endif









typedef void (*Scr_Error_t)(const char *string);
extern Scr_Error_t Scr_Error;





typedef const char * (*SV_GetConfigstringConst_t)(int index);
extern SV_GetConfigstringConst_t SV_GetConfigstringConst;




//typedef void (*Scr_GetVector_t)(unsigned int param, vec3_t /***/vec);
//extern Scr_GetVector_t Scr_GetVector;



typedef void (*Scr_AddBool_t)(qboolean value);
extern Scr_AddBool_t Scr_AddBool;

typedef void (*Scr_AddInt_t)(int value);
extern Scr_AddInt_t Scr_AddInt;


typedef void (*Scr_AddString_t)(const char *string);
extern Scr_AddString_t Scr_AddString;


typedef void (*Scr_AddUndefined_t)(void);
extern Scr_AddUndefined_t Scr_AddUndefined;



typedef void (*Scr_AddVector_t)(vec3_t vec);
extern Scr_AddVector_t Scr_AddVector;




typedef playerState_t * (*SV_GameClientNum_t)(int num);
#if COD_VERSION == COD1_1_1
static const SV_GameClientNum_t SV_GameClientNum = (SV_GameClientNum_t)0x08089270;
#elif COD_VERSION == COD1_1_5

#endif






typedef char * (*I_strlwr_t)(char *s1);
extern I_strlwr_t I_strlwr;






/*
typedef int (*Player_GetUseList_t)(gentity_t *player, useList_t *useList);
extern Player_GetUseList_t Player_GetUseList;*/

#endif