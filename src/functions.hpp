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

typedef long (*FS_SV_FOpenFileRead_t)(const char *filename, fileHandle_t *fp);
#if COD_VERSION == COD1_1_1
static const FS_SV_FOpenFileRead_t FS_SV_FOpenFileRead = (FS_SV_FOpenFileRead_t)0x0806ffb8;
#elif COD_VERSION == COD1_1_5
#endif

typedef int (*FS_idPak_t)(const char *a1, const char *a2);
#if COD_VERSION == COD1_1_1
static const FS_idPak_t FS_idPak = (FS_idPak_t)0x080709c0;
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

typedef void (*Info_SetValueForKey_t)(char *s, const char *key, const char *value);
#if COD_VERSION == COD1_1_1
static const Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x080827d4;
#elif COD_VERSION == COD1_1_5
#endif

typedef char * (*Info_ValueForKey_t)(const char *s, const char *key);
#if COD_VERSION == COD1_1_1
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x08082460;
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

typedef void (*SV_GetConfigstring_t)( int index, char *buffer, int bufferSize );
extern SV_GetConfigstring_t SV_GetConfigstring;

typedef void (*SV_SetConfigstring_t)(int index, const char *val);
#if COD_VERSION == COD1_1_1
static const SV_SetConfigstring_t SV_SetConfigstring = (SV_SetConfigstring_t)0x08089bf0;
#elif COD_VERSION == COD1_1_5
#endif

typedef void * (*Z_MallocInternal_t)(int size);
#if COD_VERSION == COD1_1_1
static const Z_MallocInternal_t Z_MallocInternal = (Z_MallocInternal_t)0x080681e8;
#elif COD_VERSION == COD1_1_5
#endif

typedef int (*FS_Read_t)(void *buffer, int len, fileHandle_t f);
#if COD_VERSION == COD1_1_1
static const FS_Read_t FS_Read = (FS_Read_t)0x080628f4;
#elif COD_VERSION == COD1_1_5
#endif

typedef void (*MSG_WriteByte_t)(msg_t *msg, int c);
#if COD_VERSION == COD1_1_1
static const MSG_WriteByte_t MSG_WriteByte = (MSG_WriteByte_t)0x0807f090;
#elif COD_VERSION == COD1_1_5
#endif

typedef void (*MSG_WriteShort_t)(msg_t *msg, int c);
#if COD_VERSION == COD1_1_1
static const MSG_WriteShort_t MSG_WriteShort = (MSG_WriteShort_t)0x0807f0bc;
#elif COD_VERSION == COD1_1_5
#endif

typedef void (*MSG_WriteLong_t)(msg_t *msg, int c);
#if COD_VERSION == COD1_1_1
static const MSG_WriteLong_t MSG_WriteLong = (MSG_WriteLong_t)0x0807f0ec;
#elif COD_VERSION == COD1_1_5
#endif

typedef void (*MSG_WriteString_t)(msg_t *msg, const char *s);
#if COD_VERSION == COD1_1_1
static const MSG_WriteString_t MSG_WriteString = (MSG_WriteString_t)0x0807a620;
#elif COD_VERSION == COD1_1_5
#endif

typedef void (*MSG_WriteData_t)(msg_t *msg, const void *data, int length);
#if COD_VERSION == COD1_1_1
static const MSG_WriteData_t MSG_WriteData = (MSG_WriteData_t)0x0807eef0;
#elif COD_VERSION == COD1_1_5
#endif

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

typedef char * (*Q_strlwr_t)(char *s1);
extern Q_strlwr_t Q_strlwr;

typedef void (*Q_strcat_t)(char *dest, int size, const char *src);
extern Q_strcat_t Q_strcat;

typedef void (*Com_Error_t)(errorParm_t code, const char *format, ...);
#if COD_VERSION == COD1_1_1
static const Com_Error_t Com_Error = (Com_Error_t)0x0806b93c;
#elif COD_VERSION == COD1_1_5
#endif

#endif