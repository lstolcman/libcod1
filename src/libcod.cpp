#include "gsc.hpp"
#include "libcod.hpp"

#include <signal.h>

// Stock cvars
cvar_t *cl_paused;
cvar_t *com_dedicated;
cvar_t *com_logfile;
cvar_t *com_sv_running;
cvar_t *sv_serverid;

// Custom cvars
cvar_t *fs_callbacks;
cvar_t *g_debugCallbacks;

cHook *hook_sys_loaddll;
cHook *hook_com_initdvars;
cHook *hook_gametype_scripts;

// Stock callbacks
int codecallback_startgametype = 0;
int codecallback_playerconnect = 0;
int codecallback_playerdisconnect = 0;
int codecallback_playerdamage = 0;
int codecallback_playerkilled = 0;

// Custom callbacks
int codecallback_playercommand = 0;

callback_t callbacks[] =
{
    { &codecallback_startgametype, "CodeCallback_StartGameType" }, // g_scr_data.gametype.startupgametype
    { &codecallback_playerconnect, "CodeCallback_PlayerConnect" }, // g_scr_data.gametype.playerconnect
    { &codecallback_playerdisconnect, "CodeCallback_PlayerDisconnect" }, // g_scr_data.gametype.playerdisconnect
    { &codecallback_playerdamage, "CodeCallback_PlayerDamage" }, // g_scr_data.gametype.playerdamage
    { &codecallback_playerkilled, "CodeCallback_PlayerKilled" }, // g_scr_data.gametype.playerkilled

    { &codecallback_playercommand, "CodeCallback_PlayerCommand"},
};

// Game lib variables declarations
gentity_t *g_entities;

// Game lib functions declarations
Scr_GetFunctionHandle_t Scr_GetFunctionHandle;
Scr_GetNumParam_t Scr_GetNumParam;
SV_Cmd_ArgvBuffer_t SV_Cmd_ArgvBuffer;
ClientCommand_t ClientCommand;
Scr_GetFunction_t Scr_GetFunction;
Scr_GetMethod_t Scr_GetMethod;
SV_GameSendServerCommand_t SV_GameSendServerCommand;
Scr_ExecEntThread_t Scr_ExecEntThread;
Scr_FreeThread_t Scr_FreeThread;
Scr_Error_t Scr_Error;
SV_GetConfigstringConst_t SV_GetConfigstringConst;
Scr_IsSystemActive_t Scr_IsSystemActive;
Scr_GetType_t Scr_GetType;
Scr_GetEntity_t Scr_GetEntity;
Scr_AddBool_t Scr_AddBool;
Scr_AddInt_t Scr_AddInt;
Scr_AddString_t Scr_AddString;
Scr_AddUndefined_t Scr_AddUndefined;
Scr_AddVector_t Scr_AddVector;
Scr_MakeArray_t Scr_MakeArray;
Scr_AddArray_t Scr_AddArray;
I_strlwr_t I_strlwr;
//Scr_GetVector_t Scr_GetVector;
//Player_GetUseList_t Player_GetUseList;

void custom_Com_InitCvars(void)
{
    printf("####### custom_Com_InitCvars \n");


    hook_com_initdvars->unhook();
    void (*Com_InitDvars)(void);
    *(int *)&Com_InitDvars = hook_com_initdvars->from;
    Com_InitDvars();
    hook_com_initdvars->hook();



    // Get references to early loaded stock dvars
    cl_paused = Cvar_FindVar("cl_paused");
    com_dedicated = Cvar_FindVar("dedicated");
    com_logfile = Cvar_FindVar("logfile");
    com_sv_running = Cvar_FindVar("sv_running");


}

void common_init_complete_print(const char *format, ...)
{
    Com_Printf("--- Common Initialization Complete ---\n");

    // Get references to stock cvars
    sv_serverid = Cvar_FindVar("sv_serverid");



    // Register custom cvars
    Cvar_Get("libcod", "1", CVAR_SERVERINFO);
    Cvar_Get("sv_cracked", "0", CVAR_ARCHIVE);

    fs_callbacks = Cvar_Get("fs_callbacks", "", CVAR_ARCHIVE);
    g_debugCallbacks = Cvar_Get("g_debugCallbacks", "0", CVAR_ARCHIVE);


}





int custom_GScr_LoadGameTypeScript()
{
	unsigned int i;
	char path_for_cb[512] = "maps/mp/gametypes/_callbacksetup";

	hook_gametype_scripts->unhook();
	int (*GScr_LoadGameTypeScript)();
	*(int *)&GScr_LoadGameTypeScript = hook_gametype_scripts->from;
	int ret = GScr_LoadGameTypeScript();
	hook_gametype_scripts->hook();

	if ( strlen(fs_callbacks->string) )
		strncpy(path_for_cb, fs_callbacks->string, sizeof(path_for_cb));

	for ( i = 0; i < sizeof(callbacks)/sizeof(callbacks[0]); i++ )
	{
		*callbacks[i].pos = Scr_GetFunctionHandle(path_for_cb, callbacks[i].name, 0);
		if ( *callbacks[i].pos && g_debugCallbacks->integer )
			Com_Printf("%s found @ %p\n", callbacks[i].name, scrVarPub.programBuffer + *callbacks[i].pos);
	}

	return ret;
}








void hook_ClientCommand(int clientNum)
{
	if ( !Scr_IsSystemActive() )
		return;
			
	if ( !codecallback_playercommand )
	{	
		ClientCommand(clientNum);
		return;
	}

	stackPushArray();
	int args = Cmd_Argc();
	for ( int i = 0; i < args; i++ )
	{
		char tmp[MAX_STRINGLENGTH];
		SV_Cmd_ArgvBuffer(i, tmp, sizeof(tmp));
		if( i == 1 && tmp[0] >= 20 && tmp[0] <= 22 )
		{
			char *part = strtok(tmp + 1, " ");
			while( part != NULL )
			{
				stackPushString(part);
				stackPushArrayLast();
				part = strtok(NULL, " ");
			}
		}
		else
		{
			stackPushString(tmp);
			stackPushArrayLast();
		}
	}

	short ret = Scr_ExecEntThread(&g_entities[clientNum], codecallback_playercommand, 1);
	Scr_FreeThread(ret);
}







const char* hook_AuthorizeState(int arg)
{
    const char* s = Cmd_Argv(arg);
    cvar_t* sv_cracked = Cvar_FindVar("sv_cracked");

    if (sv_cracked->integer && strcmp(s, "deny") == 0)
        return "accept";

    return s;
}






void ServerCrash(int sig)
{
    int fd;
    FILE *fp;
    void *array[20];
    size_t size = backtrace(array, 20);

    // Write to crash log ...
    fp = fopen("./crash.log", "a");
    if ( fp )
    {
        fd = fileno(fp);
        fseek(fp, 0, SEEK_END);
        fprintf(fp, "Error: Server crashed with signal 0x%x {%d}\n", sig, sig);
        fflush(fp);
        backtrace_symbols_fd(array, size, fd);
    }
    // ... and stderr
    fprintf(stderr, "Error: Server crashed with signal 0x%x {%d}\n", sig, sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}




void *custom_Sys_LoadDll(const char *name, char *fqpath, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...))
{
    hook_sys_loaddll->unhook();
    void *(*Sys_LoadDll)(const char *name, char *fqpath, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...));
    *(int *)&Sys_LoadDll = hook_sys_loaddll->from;
    void *ret = Sys_LoadDll(name, fqpath, entryPoint, systemcalls);
    hook_sys_loaddll->hook();

    // Unprotect the game lib
    char libPath[512];
    cvar_t* fs_game = Cvar_FindVar("fs_game");
    if (*fs_game->string)
        sprintf(libPath, "%s/game.mp.i386.so", fs_game->string);
    else
        sprintf(libPath, "main/game.mp.i386.so");

    char buf[512];
    char flags[4];
    void *low, *high;
    FILE *fp;
    fp = fopen("/proc/self/maps", "r");
    if (!fp)
        return 0;
    while (fgets(buf, sizeof(buf), fp))
    {
        if (!strstr(buf, libPath))
            continue;
        if (sscanf (buf, "%p-%p %4c", &low, &high, flags) != 3)
            continue;
        mprotect((void *)low, (int)high-(int)low, PROT_READ | PROT_WRITE | PROT_EXEC);
    }
    fclose(fp);

    // Game lib variables initializations
    g_entities = (gentity_t*)dlsym(ret, "g_entities");

    // Game lib functions initializations
    Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)dlsym(ret, "Scr_GetFunctionHandle");
    Scr_GetNumParam = (Scr_GetNumParam_t)dlsym(ret, "Scr_GetNumParam");
    SV_Cmd_ArgvBuffer = (SV_Cmd_ArgvBuffer_t)dlsym(ret, "trap_Argv");
    ClientCommand = (ClientCommand_t)dlsym(ret, "ClientCommand");
    Scr_GetFunction = (Scr_GetFunction_t)dlsym(ret, "Scr_GetFunction");
    Scr_GetMethod = (Scr_GetMethod_t)dlsym(ret, "Scr_GetMethod");
    SV_GameSendServerCommand = (SV_GameSendServerCommand_t)dlsym(ret, "trap_SendServerCommand");
    Scr_ExecEntThread = (Scr_ExecEntThread_t)dlsym(ret, "Scr_ExecEntThread");
    Scr_FreeThread = (Scr_FreeThread_t)dlsym(ret, "Scr_FreeThread");
    Scr_Error = (Scr_Error_t)dlsym(ret, "Scr_Error");
    SV_GetConfigstringConst = (SV_GetConfigstringConst_t)dlsym(ret, "trap_GetConfigstringConst");
    Scr_IsSystemActive = (Scr_IsSystemActive_t)dlsym(ret, "Scr_IsSystemActive");
    Scr_GetType = (Scr_GetType_t)dlsym(ret, "Scr_GetType");
    Scr_GetEntity = (Scr_GetEntity_t)dlsym(ret, "Scr_GetEntity");
    Scr_AddBool = (Scr_AddBool_t)dlsym(ret, "Scr_AddBool");
    Scr_AddInt = (Scr_AddInt_t)dlsym(ret, "Scr_AddInt");
    Scr_AddString = (Scr_AddString_t)dlsym(ret, "Scr_AddString");
    Scr_AddUndefined = (Scr_AddUndefined_t)dlsym(ret, "Scr_AddUndefined");
    Scr_AddVector = (Scr_AddVector_t)dlsym(ret, "Scr_AddVector");
    Scr_MakeArray = (Scr_MakeArray_t)dlsym(ret, "Scr_MakeArray");
    Scr_AddArray = (Scr_AddArray_t)dlsym(ret, "Scr_AddArray");
    I_strlwr = (I_strlwr_t)dlsym(ret, "Q_strlwr");
    //Scr_GetVector = (Scr_GetVector_t)dlsym(ret, "Scr_GetVector");
    //Player_GetUseList = (Player_GetUseList_t)dlsym(ret, "G_GetActivateEnt");

    // Game lib call instructions redirections
    cracking_hook_call((int)dlsym(ret, "vmMain") + 0xB0, (int)hook_ClientCommand);

    // Game lib functions replacements
    hook_gametype_scripts = new cHook((int)dlsym(ret, "GScr_LoadGameTypeScript"), (int)custom_GScr_LoadGameTypeScript);
	hook_gametype_scripts->hook();




    return ret;
}

class cCallOfDuty1Pro
{
public:
    cCallOfDuty1Pro()
    {
        // Don't inherit lib of parent
        unsetenv("LD_PRELOAD");

        // Crash handlers for debugging
        signal(SIGSEGV, ServerCrash);
        signal(SIGABRT, ServerCrash);
        
        // Otherwise the printf()'s are printed at crash/end on older os/compiler versions
        setbuf(stdout, NULL);

        #if COD_VERSION == COD1_1_1
        printf("> [LIBCOD] Compiled for: CoD1 1.1\n");
        #elif COD_VERSION == COD1_1_5
        printf("> [LIBCOD] Compiled for: CoD1 1.5\n");
        #endif

        printf("> [LIBCOD] Compiled %s %s using GCC %s\n", __DATE__, __TIME__, __VERSION__);

        // Allow to write in executable memory
        mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);

        #if COD_VERSION == COD1_1_1




        cracking_hook_call(0x0806ce77, (int)common_init_complete_print);


        cracking_hook_call(0x08090BA0, (int)hook_ClientCommand);


        cracking_hook_call(0x08085213, (int)hook_AuthorizeState);
        cracking_hook_call(0x08094c54, (int)Scr_GetCustomFunction);
        cracking_hook_call(0x080951c4, (int)Scr_GetCustomMethod);


        hook_sys_loaddll = new cHook(0x080c5fe4, (int)custom_Sys_LoadDll);
        hook_sys_loaddll->hook();

        
        hook_com_initdvars = new cHook(0x080c6b90, (int)custom_Com_InitCvars);
        hook_com_initdvars->hook();




        #if COMPILE_PLAYER == 1

        #endif

        //cracking_hook_function(0x080E97F0, (int)custom_BG_IsWeaponValid);
        //cracking_hook_call(0x08094107, (int)hook_SV_DirectConnect);



        #elif COD_VERSION == COD1_1_5

        #if COMPILE_PLAYER == 1

        #endif

        #endif

        printf("> [PLUGIN LOADED]\n");
        
        
        
        printf("############## sizeof client_t = %i \n", sizeof(client_t));
        printf("############## sizeof clientSnapshot_t = %i \n", sizeof(clientSnapshot_t));
        



    }

    ~cCallOfDuty1Pro()
    {
        printf("> [PLUGIN UNLOADED]\n");
    }
};

cCallOfDuty1Pro *pro;
void __attribute__ ((constructor)) lib_load(void)
{
    pro = new cCallOfDuty1Pro;
}
void __attribute__ ((destructor)) lib_unload(void)
{
    delete pro;
}