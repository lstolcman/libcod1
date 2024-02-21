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

typedef int (*Scr_GetFunctionHandle_t)(const char* scriptName, const char* labelName);
extern Scr_GetFunctionHandle_t Scr_GetFunctionHandle;

typedef int (*Scr_GetNumParam_t)(void);
extern Scr_GetNumParam_t Scr_GetNumParam;

typedef char * (*Cmd_Argv_t)(int arg);
#if COD_VERSION == COD1_1_1
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x0805b258;
#elif COD_VERSION == COD1_1_5
#endif

typedef int (*Cmd_Argc_t)();
#if COD_VERSION == COD1_1_1
static const Cmd_Argc_t Cmd_Argc = (Cmd_Argc_t)0x0805b24c;
#elif COD_VERSION == COD1_1_5
#endif

typedef void (*SV_Cmd_ArgvBuffer_t)(int arg, char *buffer, int bufferLength);
extern SV_Cmd_ArgvBuffer_t SV_Cmd_ArgvBuffer;

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

typedef void (*ClientCommand_t)(int clientNum);
extern ClientCommand_t ClientCommand;

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

typedef void (*SV_GameSendServerCommand_t)(int clientnum, svscmd_type type, const char *text);
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

typedef short (*Scr_ExecEntThread_t)(gentity_t* ent, int callbackHook, unsigned int numArgs);
extern Scr_ExecEntThread_t Scr_ExecEntThread;

typedef short (*Scr_FreeThread_t)(short thread_id);
extern Scr_FreeThread_t Scr_FreeThread;

typedef void (*Scr_Error_t)(const char *string);
extern Scr_Error_t Scr_Error;

typedef const char * (*SV_GetConfigstringConst_t)(int index);
extern SV_GetConfigstringConst_t SV_GetConfigstringConst;

typedef int (*Scr_IsSystemActive_t)();
extern Scr_IsSystemActive_t Scr_IsSystemActive;

typedef int (*Scr_GetInt_t)(unsigned int param);
extern Scr_GetInt_t Scr_GetInt;

typedef const char * (*Scr_GetString_t)(unsigned int param);
extern Scr_GetString_t Scr_GetString;

typedef int (*Scr_GetType_t)(unsigned int param);
extern Scr_GetType_t Scr_GetType;

typedef gentity_t * (*Scr_GetEntity_t)(unsigned int index);
extern Scr_GetEntity_t Scr_GetEntity;

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

typedef void (*Scr_MakeArray_t)(void);
extern Scr_MakeArray_t Scr_MakeArray;

typedef void (*Scr_AddArray_t)(void);
extern Scr_AddArray_t Scr_AddArray;




typedef unsigned int (*Scr_LoadScript_t)(const char *filename);
extern Scr_LoadScript_t Scr_LoadScript;





typedef playerState_t * (*SV_GameClientNum_t)(int num);
#if COD_VERSION == COD1_1_1
static const SV_GameClientNum_t SV_GameClientNum = (SV_GameClientNum_t)0x08089270;
#elif COD_VERSION == COD1_1_5
#endif

typedef char * (*I_strlwr_t)(char *s1);
extern I_strlwr_t I_strlwr;

/*
typedef char * (*Cvar_VariableString_t)(const char *var_name);
#if COD_VERSION == COD1_1_1
static const Cvar_VariableString_t Cvar_VariableString = (Cvar_VariableString_t)0x0806f910;
#elif COD_VERSION == COD1_1_5
#endif
*/

//typedef void (*Scr_GetVector_t)(unsigned int param, vec3_t /***/vec);
//extern Scr_GetVector_t Scr_GetVector;

/*
typedef int (*Player_GetUseList_t)(gentity_t *player, useList_t *useList);
extern Player_GetUseList_t Player_GetUseList;*/

#endif