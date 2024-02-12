#include "gsc.hpp"
#include "libcod.hpp"

#include <signal.h>





// Stock cvars
cvar_t *cl_paused;
cvar_t *com_dedicated;
cvar_t *com_logfile;
cvar_t *com_sv_running;
cvar_t *sv_serverid;



cHook *hook_sys_loaddll;

cHook *hook_add_opcode;
cHook *hook_com_initdvars;



// Stock callbacks
int codecallback_startgametype = 0;
int codecallback_playerconnect = 0;
int codecallback_playerdisconnect = 0;
int codecallback_playerdamage = 0;
int codecallback_playerkilled = 0;



callback_t callbacks[] =
{
	{ &codecallback_startgametype, "CodeCallback_StartGameType" }, // g_scr_data.gametype.startupgametype
	{ &codecallback_playerconnect, "CodeCallback_PlayerConnect" }, // g_scr_data.gametype.playerconnect
	{ &codecallback_playerdisconnect, "CodeCallback_PlayerDisconnect" }, // g_scr_data.gametype.playerdisconnect
	{ &codecallback_playerdamage, "CodeCallback_PlayerDamage" }, // g_scr_data.gametype.playerdamage
	{ &codecallback_playerkilled, "CodeCallback_PlayerKilled" }, // g_scr_data.gametype.playerkilled
};



// Game lib function declarations
Scr_GetMethod_t Scr_GetMethod;
//Scr_AddString_t Scr_AddString;
//Scr_AddUndefined_t Scr_AddUndefined;


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


	Scr_GetMethod = (Scr_GetMethod_t)dlsym(ret, "Scr_GetMethod");
	//Scr_AddString = (Scr_AddString_t)dlsym(ret, "Scr_AddString");
	//Scr_AddUndefined = (Scr_AddUndefined_t)dlsym(ret, "Scr_AddUndefined");


	return ret;
}









void custom_Com_InitDvars(void)
{
	Com_Printf("####### custom_Com_InitDvars \n");


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
	Cvar_Get("sv_cracked", "0", CVAR_ARCHIVE);


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
		//cracking_hook_call(0x08091D0C, (int)hook_sv_spawnserver);
		cracking_hook_call(0x08085213, (int)hook_AuthorizeState);
		cracking_hook_call(0x080951c4, (int)Scr_GetCustomMethod);



		hook_sys_loaddll = new cHook(0x080c5fe4, (int)custom_Sys_LoadDll);
		hook_sys_loaddll->hook();



		hook_com_initdvars = new cHook(0x080c6b90, (int)custom_Com_InitDvars);
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