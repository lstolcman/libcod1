#include <signal.h>
#include <arpa/inet.h>

#include <memory>
#include <tuple>
#include <array>
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <cstring>
#include <vector>

#include "gsc.hpp"

// Stock cvars
cvar_t *com_cl_running;
cvar_t *com_dedicated;
cvar_t *com_logfile;
cvar_t *com_sv_running;
cvar_t *fs_game;
cvar_t *sv_allowDownload;
cvar_t *sv_floodProtect;
cvar_t *sv_gametype;
cvar_t *sv_maxclients;
cvar_t *sv_mapRotation;
cvar_t *sv_mapRotationCurrent;
cvar_t *sv_pure;
cvar_t *sv_rconPassword;
cvar_t *sv_serverid;
cvar_t *sv_showCommands;

// Custom cvars
cvar_t *fs_callbacks;
cvar_t *fs_callbacks_additional;
cvar_t *fs_svrPaks;
cvar_t *g_deadChat;
cvar_t *g_debugCallbacks;
cvar_t *g_playerEject;
cvar_t *g_resetSlide;
cvar_t *jump_height;
cvar_t *jump_height_airScale;
cvar_t *sv_cracked;
cvar_t *player_sprint;
cvar_t *player_sprintSpeedScale;
cvar_t *player_sprintTime;

cHook *hook_ClientEndFrame;
cHook *hook_ClientThink;
cHook *hook_Com_Init;
cHook *hook_GScr_LoadGameTypeScript;
cHook *hook_PM_AirMove;
cHook *hook_PM_CrashLand;
cHook *hook_PmoveSingle;
cHook *hook_SV_AddOperatorCommands;
cHook *hook_SV_BeginDownload_f;
cHook *hook_SV_SendClientGameState;
cHook *hook_SV_SpawnServer;
cHook *hook_Sys_LoadDll;

// Stock callbacks
int codecallback_startgametype = 0;
int codecallback_playerconnect = 0;
int codecallback_playerdisconnect = 0;
int codecallback_playerdamage = 0;
int codecallback_playerkilled = 0;

// Custom callbacks
int codecallback_client_spam = 0;
int codecallback_playercommand = 0;
int codecallback_playerairjump = 0;

callback_t callbacks[] =
{
    { &codecallback_startgametype, "CodeCallback_StartGameType" }, // g_scr_data.gametype.startupgametype
    { &codecallback_playerconnect, "CodeCallback_PlayerConnect" }, // g_scr_data.gametype.playerconnect
    { &codecallback_playerdisconnect, "CodeCallback_PlayerDisconnect" }, // g_scr_data.gametype.playerdisconnect
    { &codecallback_playerdamage, "CodeCallback_PlayerDamage" }, // g_scr_data.gametype.playerdamage
    { &codecallback_playerkilled, "CodeCallback_PlayerKilled" }, // g_scr_data.gametype.playerkilled

    { &codecallback_client_spam, "CodeCallback_CLSpam"},
    { &codecallback_playercommand, "CodeCallback_PlayerCommand"},
    { &codecallback_playerairjump, "CodeCallback_PlayerAirJump"},
};

void UCMD_custom_sprint(client_t *cl);
// See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/sv_client.c#L98
static ucmd_t ucmds[] =
{
    {"userinfo",        SV_UpdateUserinfo_f,     },
    {"disconnect",      SV_Disconnect_f,         },
    {"cp",              SV_VerifyPaks_f,         },
    {"vdr",             SV_ResetPureClient_f,    },
    {"download",        SV_BeginDownload_f,      },
    {"nextdl",          SV_NextDownload_f,       },
    {"stopdl",          SV_StopDownload_f,       },
    {"donedl",          SV_DoneDownload_f,       },
    {"retransdl",       SV_RetransmitDownload_f, },
    {"sprint",          UCMD_custom_sprint, },
    {NULL, NULL}
};

customPlayerState_t customPlayerState[MAX_CLIENTS];

// Game lib objects
gentity_t* g_entities;
gclient_t* g_clients;
level_locals_t* level;
pmove_t* pm;
pml_t* pml;
stringIndex_t* scr_const;

// Game lib functions
G_Say_t G_Say;
G_RegisterCvars_t G_RegisterCvars;
G_AddEvent_t G_AddEvent;
G_AddPredictableEvent_t G_AddPredictableEvent;
trap_SendServerCommand_t trap_SendServerCommand;
trap_GetConfigstringConst_t trap_GetConfigstringConst;
trap_GetConfigstring_t trap_GetConfigstring;
BG_GetNumWeapons_t BG_GetNumWeapons;
BG_GetInfoForWeapon_t BG_GetInfoForWeapon;
BG_GetWeaponIndexForName_t BG_GetWeaponIndexForName;
BG_AnimationIndexForString_t BG_AnimationIndexForString;
Scr_GetFunctionHandle_t Scr_GetFunctionHandle;
Scr_GetNumParam_t Scr_GetNumParam;
Scr_IsSystemActive_t Scr_IsSystemActive;
Scr_GetInt_t Scr_GetInt;
Scr_GetString_t Scr_GetString;
Scr_GetType_t Scr_GetType;
Scr_GetEntity_t Scr_GetEntity;
Scr_AddBool_t Scr_AddBool;
Scr_AddInt_t Scr_AddInt;
Scr_AddFloat_t Scr_AddFloat;
Scr_AddString_t Scr_AddString;
Scr_AddUndefined_t Scr_AddUndefined;
Scr_AddVector_t Scr_AddVector;
Scr_MakeArray_t Scr_MakeArray;
Scr_AddArray_t Scr_AddArray;
Scr_AddObject_t Scr_AddObject;
Scr_LoadScript_t Scr_LoadScript;
Scr_ExecThread_t Scr_ExecThread;
Scr_ExecEntThread_t Scr_ExecEntThread;
Scr_ExecEntThreadNum_t Scr_ExecEntThreadNum;
Scr_FreeThread_t Scr_FreeThread;
Scr_GetFunction_t Scr_GetFunction;
Scr_GetMethod_t Scr_GetMethod;
Scr_Error_t Scr_Error;
Scr_ObjectError_t Scr_ObjectError;
Scr_GetConstString_t Scr_GetConstString;
Scr_ParamError_t Scr_ParamError;
Q_strlwr_t Q_strlwr;
Q_strupr_t Q_strupr;
Q_strcat_t Q_strcat;
Q_strncpyz_t Q_strncpyz;
Q_CleanStr_t Q_CleanStr;
StuckInClient_t StuckInClient;
trap_Argv_t trap_Argv;
ClientCommand_t ClientCommand;
Com_SkipRestOfLine_t Com_SkipRestOfLine;
Com_ParseRestOfLine_t Com_ParseRestOfLine;
Com_ParseInt_t Com_ParseInt;
Jump_Check_t Jump_Check;
PM_GetEffectiveStance_t PM_GetEffectiveStance;

// Resume addresses
uintptr_t resume_addr_Jump_Check;
uintptr_t resume_addr_Jump_Check_2;

void custom_Com_Init(char *commandLine)
{
    hook_Com_Init->unhook();
    void (*Com_Init)(char *commandLine);
    *(int*)&Com_Init = hook_Com_Init->from;
    Com_Init(commandLine);
    hook_Com_Init->hook();
    
    // Get references to stock cvars
    com_cl_running = Cvar_FindVar("cl_running");
    com_dedicated = Cvar_FindVar("dedicated");
    com_logfile = Cvar_FindVar("logfile");
    com_sv_running = Cvar_FindVar("sv_running");
    fs_game = Cvar_FindVar("fs_game");
    sv_allowDownload = Cvar_FindVar("sv_allowDownload");
    sv_floodProtect = Cvar_FindVar("sv_floodProtect");
    sv_gametype = Cvar_FindVar("g_gametype");
    sv_maxclients = Cvar_FindVar("sv_maxclients");
    sv_mapRotation = Cvar_FindVar("sv_mapRotation");
    sv_mapRotationCurrent = Cvar_FindVar("sv_mapRotationCurrent");
    sv_pure = Cvar_FindVar("sv_pure");
    sv_rconPassword = Cvar_FindVar("rconpassword");
    sv_serverid = Cvar_FindVar("sv_serverid");
    sv_showCommands = Cvar_FindVar("sv_showCommands");

    // Register custom cvars
    Cvar_Get("libcod", "1", CVAR_SERVERINFO);
    Cvar_Get("sv_wwwBaseURL", "", CVAR_SYSTEMINFO | CVAR_ARCHIVE);
    Cvar_Get("sv_wwwDownload", "0", CVAR_SYSTEMINFO | CVAR_ARCHIVE);
    
    fs_callbacks = Cvar_Get("fs_callbacks", "", CVAR_ARCHIVE);
    fs_callbacks_additional = Cvar_Get("fs_callbacks_additional", "", CVAR_ARCHIVE);
    fs_svrPaks = Cvar_Get("fs_svrPaks", "", CVAR_ARCHIVE);
    g_deadChat = Cvar_Get("g_deadChat", "0", CVAR_ARCHIVE);
    g_debugCallbacks = Cvar_Get("g_debugCallbacks", "0", CVAR_ARCHIVE);
    g_playerEject = Cvar_Get("g_playerEject", "1", CVAR_ARCHIVE);
    g_resetSlide = Cvar_Get("g_resetSlide", "0", CVAR_ARCHIVE);
    jump_height = Cvar_Get("jump_height", "39.0", CVAR_ARCHIVE);
    jump_height_airScale = Cvar_Get("jump_height_airScaleScale", "1.5", CVAR_ARCHIVE);
    player_sprint = Cvar_Get("player_sprint", "0", CVAR_ARCHIVE);
    player_sprintSpeedScale = Cvar_Get("player_sprintSpeedScale", "1.5", CVAR_ARCHIVE);
    player_sprintTime = Cvar_Get("player_sprintTime", "4.0", CVAR_ARCHIVE);
    sv_cracked = Cvar_Get("sv_cracked", "0", CVAR_ARCHIVE);

    /*
    Force cl_allowDownload on client, otherwise 1.1x can't download to join the server
    TODO: Force only for 1.1x clients
    */
    Cvar_Get("cl_allowDownload", "1", CVAR_SYSTEMINFO);
}

// 1.1 deadchat support
// See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/sv_client.c#L940
void hook_G_Say(gentity_s *ent, gentity_s *target, int mode, const char *chatText)
{
    int unknown_var = *(int*)((int)ent->client + 8400);
    if(unknown_var && !g_deadChat->integer)
        return;

    G_Say(ent, NULL, mode, chatText);
}

qboolean FS_svrPak(const char *base)
{
    if(strstr(base, "_svr_"))
        return qtrue;

    if (*fs_svrPaks->string)
    {
        bool isSvrPak = false;
        size_t lenString = strlen(fs_svrPaks->string) + 1;
        char* stringCopy = (char*)malloc(lenString);
        strcpy(stringCopy, fs_svrPaks->string);

        const char* separator = ";";
        char* strToken = strtok(stringCopy, separator);

        while (strToken != NULL)
        {
            if (!strcmp(base, strToken))
            {
                isSvrPak = true;
                break;
            }
            strToken = strtok(NULL, separator);
        }

        free(stringCopy);
        if (isSvrPak)
            return qtrue;
    }

    return qfalse;
}

const char* custom_FS_ReferencedPakNames(void)
{
    static char info[BIG_INFO_STRING];
    searchpath_t *search;

    info[0] = 0;
    
    for (search = fs_searchpaths; search; search = search->next)
    {
        if(!search->pak)
            continue;
        if(FS_svrPak(search->pak->pakBasename))
            continue;

        if(*info)
            Q_strcat(info, sizeof( info ), " ");
        Q_strcat(info, sizeof( info ), search->pak->pakGamename);
        Q_strcat(info, sizeof( info ), "/");
        Q_strcat(info, sizeof( info ), search->pak->pakBasename);
    }

    return info;
}

const char* custom_FS_ReferencedPakChecksums(void)
{
    static char info[BIG_INFO_STRING];
    searchpath_t *search;
    
    info[0] = 0;

    for (search = fs_searchpaths; search; search = search->next)
    {
        if(!search->pak)
            continue;
        if(FS_svrPak(search->pak->pakBasename))
            continue;
        
        Q_strcat(info, sizeof( info ), custom_va("%i ", search->pak->checksum));
    }

    return info;
}

void custom_GScr_LoadGameTypeScript()
{
    hook_GScr_LoadGameTypeScript->unhook();
    void (*GScr_LoadGameTypeScript)();
    *(int*)&GScr_LoadGameTypeScript = hook_GScr_LoadGameTypeScript->from;
    GScr_LoadGameTypeScript();
    hook_GScr_LoadGameTypeScript->hook();

    unsigned int i;
    char path_for_cb[512] = "maps/mp/gametypes/_callbacksetup";

    if (*fs_callbacks_additional->string)
    {
        if(!Scr_LoadScript(fs_callbacks_additional->string))
            Com_DPrintf("custom_GScr_LoadGameTypeScript: Scr_LoadScript for fs_callbacks_additional cvar failed.\n");
    }
    else
    {
        Com_DPrintf("custom_GScr_LoadGameTypeScript: No custom callback file specified in fs_callbacks_additional cvar.\n");
    }

    if(*fs_callbacks->string)
        strncpy(path_for_cb, fs_callbacks->string, sizeof(path_for_cb));
        
    for (i = 0; i < sizeof(callbacks)/sizeof(callbacks[0]); i++)
    {
        if (!strcmp(callbacks[i].name, "CodeCallback_PlayerCommand")
            || !strcmp(callbacks[i].name, "CodeCallback_PlayerAirJump"))
        {
            *callbacks[i].pos = Scr_GetFunctionHandle(fs_callbacks_additional->string, callbacks[i].name);
        }
        else
            *callbacks[i].pos = Scr_GetFunctionHandle(path_for_cb, callbacks[i].name);
        
        /*if ( *callbacks[i].pos && g_debugCallbacks->integer )
            Com_Printf("%s found @ %p\n", callbacks[i].name, scrVarPub.programBuffer + *callbacks[i].pos);*/ //TODO: verify scrVarPub_t
    }
}

// See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/script.c#L944
static int localized_string_index = 128;
int custom_G_LocalizedStringIndex(const char *string)
{
    int i;
    char s[MAX_STRINGLENGTH];

    if(localized_string_index >= 256)
        localized_string_index = 128;

    if(!string || !*string)
        return 0;
    
    int start = 1244;

    for (i = 1; i < 256; i++)
    {
        trap_GetConfigstring(start + i, s, sizeof(s));
        if(!*s)
            break;
        if (!strcmp(s, string))
            return i;
    }
    if(i == 256)
        i = localized_string_index;

    SV_SetConfigstring(i + 1244, string);
    ++localized_string_index;
    
    return i;
}

void hook_ClientCommand(int clientNum)
{
    if(!Scr_IsSystemActive())
        return;

    char* cmd = Cmd_Argv(0);
    if(!strcmp(cmd, "gc"))
        return; // Prevent server crash
      
    if (!codecallback_playercommand)
    {
        ClientCommand(clientNum);
        return;
    }

    stackPushArray();
    int args = Cmd_Argc();
    for (int i = 0; i < args; i++)
    {
        char tmp[MAX_STRINGLENGTH];
        trap_Argv(i, tmp, sizeof(tmp));
        if (i == 1 && tmp[0] >= 20 && tmp[0] <= 22)
        {
            char *part = strtok(tmp + 1, " ");
            while (part != NULL)
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
    if(sv_cracked->integer && !strcmp(s, "deny"))
        return "accept";
    return s;
}

void custom_SV_SpawnServer(char *server)
{
    hook_SV_SpawnServer->unhook();
    void (*SV_SpawnServer)(char *server);
    *(int*)&SV_SpawnServer = hook_SV_SpawnServer->from;
    SV_SpawnServer(server);
    hook_SV_SpawnServer->hook();

#if COMPILE_SQLITE == 1
    free_sqlite_db_stores_and_tasks();
#endif
}

std::tuple<bool, int, int, std::string> banInfoForIp(char* ip)
{
    char *file;
    std::string token;
    const char *text;
    std::tuple<bool, int, int, std::string> banInfo;

    banInfo = std::make_tuple(false, 0, 0, "");

    if(FS_ReadFile("ban.txt", (void **)&file) < 0)
        return banInfo;

    text = file;
    while (1)
    {
        token = Com_Parse(&text);
        if(token.empty())
            break;

        if (!strcmp(token.c_str(), ip))
        {
            std::get<0>(banInfo) = true;                // banned
            Com_Parse(&text);                           // player name
            std::get<1>(banInfo) = Com_ParseInt(&text); // duration
            std::get<2>(banInfo) = Com_ParseInt(&text); // ban date
            std::get<3>(banInfo) = Com_Parse(&text);    // reason
            break;
        }
        Com_SkipRestOfLine(&text);
    }
    FS_FreeFile(file);
    return banInfo;
}

void sendMessageTo_inGameAdmin_orServerConsole(client_t *cl, std::string message)
{
    std::string finalMessage;
    if (cl)
    {
        finalMessage = "e \"";
        finalMessage.append(message);
        finalMessage.append("\"");
        SV_SendServerCommand(cl, SV_CMD_CAN_IGNORE, finalMessage.c_str());
    }
    else
    {
        finalMessage = message;
        finalMessage.append("\n");
        Com_Printf(finalMessage.c_str());
    }
}

const std::array<std::string, 5> banParameters = {"-i", "-n", "-r", "-d", "-a"};
const std::array<std::string, 2> unbanParameters = {"-i", "-a"};
/*
-i: ip
-n: banned client number
-r: reason
-d: duration
-a: admin client number
*/

template <std::size_t N>
bool isValidParameter(std::string toCheck, std::array<std::string, N> parameters)
{
    for (const std::string&parameter : parameters)
    {
        if(toCheck == parameter)
            return true;
    }
    return false;
}
static void ban()
{
    if (!com_sv_running->integer)
    {
        Com_Printf("Server is not running.\n");
        return;
    }

    if (Cmd_Argc() < 3)
    {
        Com_Printf("Usage: ban (-i <IP address> | -n <client number>) [-r reason] [-d duration]\n");
        Com_Printf("Notes: Use \"h\" for hours or \"d\" for days\n");
        return;
    }

    std::vector<std::string> argvList;
    std::map<std::string, std::string> parsedParameters;
    std::string infoMessage;
    bool useIp = false;
    bool useClientnum = false;
    int file;
    bool clAdmin_searched = false;
    client_t *clToBan;
    client_t *clAdmin;
    char ip[16];
    char cleanName[64] = "n/a";
    time_t current_time;
    int duration = -1;
    std::string duration_drop;
    std::string reason_log = "none";
    std::string reason_drop;

    // Directly store all the argv to be able to use Cmd_TokenizeString before the end of the parse
    for (int i = 1; i < Cmd_Argc(); i++)
    {
        std::string argv = Cmd_Argv(i);
        argvList.push_back(argv);
    }
    
    //// Parse and store the parameters
    for (std::size_t i = 0; i < argvList.size(); i++)
    {
        std::string argv = argvList[i];
        if (isValidParameter(argv, banParameters)) // Found an option
        {
            if (parsedParameters.find(argv) == parsedParameters.end())
            {
                // Parse the argument
                std::string value;
                for (std::size_t j = i+1; j < argvList.size(); j++)
                {
                    std::string argv_next = argvList[j];
                    if (!isValidParameter(argv_next, banParameters))
                    {
                        if(j != i+1)
                            value.append(" ");
                        value.append(argv_next);
                    }
                    else
                        break;
                }
                // Store the pair
                if(!value.empty())
                    parsedParameters[argv] = value;

                /*
                Check if got admin client after first storage and only once
                because it should be passed as first parameter from gsc
                so you can redirect the error messages since the beginning
                */
                if (!clAdmin_searched)
                {
                    auto adminParam = parsedParameters.find("-a");
                    if (adminParam != parsedParameters.end())
                    {
                        clAdmin = &svs.clients[std::stoi(adminParam->second)];
                    }
                    clAdmin_searched = true;
                }
            }
            else
            {
                infoMessage = "Duplicated option " + argv;
                sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
                return;
            }
        }
        else if (argv[0] == '-' && !isValidParameter(argv, banParameters))
        {
            infoMessage = "Unrecognized option " + argv;
            sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
            return;
        }
    }
    ////
    
    //// Check the parameters
    // Client number
    if (parsedParameters.find("-n") != parsedParameters.end())
    {
        // Check if specified both an IP and a client number
        if(parsedParameters.find("-i") != parsedParameters.end())
        {
            infoMessage = "Don't use both an IP and a client number";
            sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
            return;
        }
        useClientnum = true;
    }

    // IP
    auto ipParam = parsedParameters.find("-i");
    if (ipParam != parsedParameters.end())
    {
        struct sockaddr_in sa;
        if(!inet_pton(AF_INET, ipParam->second.c_str(), &(sa.sin_addr)))
        {
            infoMessage = "Invalid IP address " + ipParam->second;
            sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
            return;
        }
        useIp = true;
        std::strcpy(ip, ipParam->second.c_str());
    }

    if(!useClientnum && !useIp)
    {
        infoMessage = "Use an IP or a client number";
        sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
        return;
    }

    // Duration
    auto durationParam = parsedParameters.find("-d");
    if (durationParam != parsedParameters.end())
    {
        char durationParam_lastChar = durationParam->second.back();
        if (durationParam_lastChar != 'h' && durationParam_lastChar != 'd')
        {
            infoMessage = "Invalid duration parameter " + durationParam->second;
            sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
            return;
        }
        else
        {
            durationParam->second.pop_back(); // Remove unit indicator
            if (durationParam->second.empty())
            {
                infoMessage = "Invalid duration parameter " + durationParam->second;
                sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
                return;
            }
            else
            {
                for (int i = 0; durationParam->second[i]; i++)
                {
                    if (durationParam->second[i] < '0' || durationParam->second[i] > '9')
                    {
                        infoMessage = "Invalid duration parameter " + durationParam->second;
                        sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
                        return;
                    }
                }
                duration = std::stoi(durationParam->second);
            }
        }
        if(durationParam_lastChar == 'h')
        {
            duration_drop = durationParam->second + " hour";
            if(duration > 1)
                duration_drop.append("s");
            duration *= 3600;
        }
        else if(durationParam_lastChar == 'd')
        {
            duration_drop = durationParam->second + " day";
            if(duration > 1)
                duration_drop.append("s");
            duration *= 86400;
        }
    }

    // Reason
    auto reasonParam = parsedParameters.find("-r");
    if (reasonParam != parsedParameters.end())
    {
        reason_log = reasonParam->second.c_str();
        reason_drop = "Ban reason: " + reasonParam->second;
    }

    // Add duration to drop message after reason
    if (!duration_drop.empty())
    {
        if(reason_drop.empty())
        {
            reason_drop = "Ban duration: " + duration_drop;
        }
        else
        {
            reason_drop.append(" - ");
            reason_drop.append("Duration: ");
            reason_drop.append(duration_drop);
        }
    }
    else if (reason_drop.empty())
    {
        reason_drop = "EXE_PLAYERKICKED";
    }
    ////

    // Find the player
    if (useClientnum)
    {
        clToBan = &svs.clients[std::stoi(parsedParameters.find("-n")->second)];
        if(!clToBan)
        {
            infoMessage = "Couldn't find player by num " + parsedParameters.find("-n")->second;
            sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
            return;
        }
        else
        {
            snprintf(ip, sizeof(ip), "%d.%d.%d.%d", clToBan->netchan.remoteAddress.ip[0], clToBan->netchan.remoteAddress.ip[1], clToBan->netchan.remoteAddress.ip[2], clToBan->netchan.remoteAddress.ip[3]);
        }
    }
    else if (useIp)
    {
        int i;
        client_t *clCheck;
        for (i = 0, clCheck = svs.clients; i < sv_maxclients->integer; i++, clCheck++)
        {
            if (clCheck->state > CS_CONNECTED)
            {
                char ip_check[16];
                snprintf(ip_check, sizeof(ip_check), "%d.%d.%d.%d", clCheck->netchan.remoteAddress.ip[0], clCheck->netchan.remoteAddress.ip[1], clCheck->netchan.remoteAddress.ip[2], clCheck->netchan.remoteAddress.ip[3]);
                if (!strcmp(ip_check, ip))
                {
                    clToBan = clCheck;
                    break;
                }
            }
        }
    }
    
    auto banInfo = banInfoForIp(ip);
    if(std::get<0>(banInfo) == true) // banned
    {
        std::ostringstream oss;
        oss << "This IP (" << ip << ") is already banned";
        infoMessage = oss.str();
        sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
        return;
    }
    
    if (clToBan)
        Q_strncpyz(cleanName, clToBan->name, sizeof(cleanName));
    
    // Add IP to ban.txt
    if (FS_FOpenFileByMode("ban.txt", &file, FS_APPEND) < 0)
    {
        infoMessage = "Couldn't open ban.txt";
        sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
        return;
    }
    else
    {
        current_time = time(NULL);
        FS_Write(file, "\"%s\" \"%s\" \"%i\" \"%i\" \"%s\"\r\n", ip, cleanName, duration, current_time, reason_log.c_str());
        FS_FCloseFile(file);
    }

    // Disconnect the player
    if (clToBan)
    {
        SV_DropClient(clToBan, reason_drop.c_str());
        clToBan->lastPacketTime = svs.time;
    }
}

static void unban()
{
    if (!com_sv_running->integer)
    {
        Com_Printf("Server is not running.\n");
        return;
    }

    if (Cmd_Argc() < 2)
    {
        Com_Printf("Usage: unban -i <IP address>\n");
        return;
    }
    
    std::vector<std::string> argvList;
    std::map<std::string, std::string> parsedParameters;
    std::string infoMessage;
    bool clAdmin_searched = false;
    client_t *clAdmin;
    char *file;
    int fileSize;
    char *line;
    std::string token;
    bool found = false;
    char *text;
    std::string ip;
    
    // Directly store all the argv to be able to use Cmd_TokenizeString before the end of the parse
    for (int i = 1; i < Cmd_Argc(); i++)
    {
        std::string argv = Cmd_Argv(i);
        argvList.push_back(argv);
    }

    //// Parse and store the parameters
    for (std::size_t i = 0; i < argvList.size(); i++)
    {
        std::string argv = argvList[i];
        if (isValidParameter(argv, unbanParameters)) // Found an option
        {
            if (parsedParameters.find(argv) == parsedParameters.end())
            {
                // Parse the argument
                std::string value;
                for (std::size_t j = i+1; j < argvList.size(); j++)
                {
                    std::string argv_next = argvList[j];
                    if (!isValidParameter(argv_next, unbanParameters))
                    {
                        if(j != i+1)
                            value.append(" ");
                        value.append(argv_next);
                    }
                    else
                        break;
                }
                // Store the pair
                if(!value.empty())
                    parsedParameters[argv] = value;

                /*
                Check if got admin client after first storage and only once
                because it should be passed as first parameter from gsc
                so you can redirect the error messages since the beginning
                */
                if (!clAdmin_searched)
                {
                    auto adminParam = parsedParameters.find("-a");
                    if (adminParam != parsedParameters.end())
                    {
                        clAdmin = &svs.clients[std::stoi(adminParam->second)];
                    }
                    clAdmin_searched = true;
                }
            }
            else
            {
                infoMessage = "Duplicated option " + argv;
                sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
                return;
            }
        }
        else if (argv[0] == '-' && !isValidParameter(argv, unbanParameters))
        {
            infoMessage = "Unrecognized option " + argv;
            sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
            return;
        }
    }
    ////

    //// Check the parameters
    // IP
    auto ipParam = parsedParameters.find("-i");
    if (ipParam != parsedParameters.end())
    {
        struct sockaddr_in sa;
        if(!inet_pton(AF_INET, ipParam->second.c_str(), &(sa.sin_addr)))
        {
            infoMessage = "Invalid IP address " + ipParam->second;
            sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
            return;
        }
        ip = ipParam->second;
    }
    else
    {
        infoMessage = "Specify an IP address";
        sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
        return;
    }
    ////
    
    // Remove IP from ban.txt
    fileSize = FS_ReadFile("ban.txt", (void **)&file);
    if (fileSize < 0)
    {
        infoMessage = "Couldn't read ban.txt";
        sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
        return;        
    }
    
    text = file;
    while (1)
    {
        line = text;
        token = Com_Parse((const char **)&text);
        if(token.empty())
            break;

        if(token == ip)
            found = true;

        Com_SkipRestOfLine((const char **)&text);

        if (found)
        {
            memmove((unsigned char *)line, (unsigned char *)text, fileSize - (text - file) + 1);
            fileSize -= text - line;
            text = line;
            break;
        }
    }

    FS_WriteFile("ban.txt", file, fileSize);
    FS_FreeFile(file);

    if (found)
    {
        infoMessage = "Unbanned IP " + ip;
        sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
    }
    else
    {
        std::ostringstream oss;
        oss << "IP " << ip << " not found";
        infoMessage = oss.str();
        sendMessageTo_inGameAdmin_orServerConsole(clAdmin, infoMessage);
    }
}

void custom_SV_AddOperatorCommands()
{
    hook_SV_AddOperatorCommands->unhook();
    void (*SV_AddOperatorCommands)();
    *(int*)&SV_AddOperatorCommands = hook_SV_AddOperatorCommands->from;
    SV_AddOperatorCommands();
    hook_SV_AddOperatorCommands->hook();

    Cmd_AddCommand("ban", ban);
    Cmd_AddCommand("unban", unban);
}

void custom_SV_SendClientGameState(client_t *client)
{
    hook_SV_SendClientGameState->unhook();
    void (*SV_SendClientGameState)(client_t *client);
    *(int*)&SV_SendClientGameState = hook_SV_SendClientGameState->from;
    SV_SendClientGameState(client);
    hook_SV_SendClientGameState->hook();

    // Reset custom player state to default values
    int id = client - svs.clients;
    memset(&customPlayerState[id], 0, sizeof(customPlayerState_t));
}

qboolean hook_StuckInClient(gentity_s *self)
{
    if(!g_playerEject->integer)
        return qfalse;
    return StuckInClient(self);
}

bool shouldServeFile(const char *requestedFilePath)
{
    static char localFilePath[MAX_OSPATH*2+5];
    searchpath_t* search;

    localFilePath[0] = 0;

    for (search = fs_searchpaths; search; search = search->next)
    {
        if (search->pak)
        {
            snprintf(localFilePath, sizeof(localFilePath), "%s/%s.pk3", search->pak->pakGamename, search->pak->pakBasename);
            if(!strcmp(localFilePath, requestedFilePath))
                if(!FS_svrPak(search->pak->pakBasename))
                    return true;
        }
    }
    return false;
}

void custom_SV_BeginDownload_f(client_t *cl)
{
    // Patch q3dirtrav
    int args = Cmd_Argc();
    if (args > 1)
    {
        const char* arg1 = Cmd_Argv(1);
        if (!shouldServeFile(arg1))
        {
            char ip[16];
            snprintf(ip, sizeof(ip), "%d.%d.%d.%d",
                cl->netchan.remoteAddress.ip[0],
                cl->netchan.remoteAddress.ip[1],
                cl->netchan.remoteAddress.ip[2],
                cl->netchan.remoteAddress.ip[3]);
            Com_Printf("WARNING: %s (%s) tried to download %s.\n", cl->name, ip, arg1);
            return;
        }
    }

    hook_SV_BeginDownload_f->unhook();
    void (*SV_BeginDownload_f)(client_t *cl);
    *(int*)&SV_BeginDownload_f = hook_SV_BeginDownload_f->from;
    SV_BeginDownload_f(cl);
    hook_SV_BeginDownload_f->hook();
}

void custom_SV_ExecuteClientMessage(client_t *cl, msg_t *msg)
{
    byte msgBuf[MAX_MSGLEN];
    msg_t decompressMsg;
    int c;

    MSG_Init(&decompressMsg, msgBuf, sizeof(msgBuf));
    decompressMsg.cursize = MSG_ReadBitsCompress(&msg->data[msg->readcount], msgBuf, msg->cursize - msg->readcount);
    
    if ((cl->serverId == sv_serverId_value || cl->downloadName[0])
        || (!cl->downloadName[0] && strstr(cl->lastClientCommandString, "nextdl")))
    {
        do
        {
            c = MSG_ReadBits(&decompressMsg, 2);
            if (c == clc_EOF || c != clc_clientCommand)
            {
                if (sv_pure->integer && cl->pureAuthentic == 2)
                {
                    cl->nextSnapshotTime = -1;
                    SV_DropClient(cl, "EXE_UNPURECLIENTDETECTED");
                    cl->state = CS_ACTIVE;
                    SV_SendClientSnapshot(cl);
                    cl->state = CS_ZOMBIE;
                }
                if(c == clc_move)
                    SV_UserMove(cl, &decompressMsg, 1);
                else if(c == clc_moveNoDelta)
                    SV_UserMove(cl, &decompressMsg, 0);
                else if(c != clc_EOF)
                    Com_Printf("WARNING: bad command byte %i for client %i\n", c, cl - svs.clients);
                return;
            }
            if(!SV_ClientCommand(cl, &decompressMsg))
                return;

        } while (cl->state != CS_ZOMBIE);
    }
    else if ((cl->serverId & 0xF0) == (sv_serverId_value & 0xF0))
    {
        if(cl->state == CS_PRIMED)
            SV_ClientEnterWorld(cl, &cl->lastUsercmd);
    }
    else
    {
        if (cl->gamestateMessageNum < cl->messageAcknowledge)
        {
            Com_DPrintf("%s : dropped gamestate, resending\n", cl->name);
            SV_SendClientGameState(cl);
        }
    }
}

// See https://github.com/voron00/CoD2rev_Server/blob/b012c4b45a25f7f80dc3f9044fe9ead6463cb5c6/src/server/sv_client_mp.cpp#L2128
void custom_SV_ExecuteClientCommand(client_t *cl, const char *s, qboolean clientOK)
{
    ucmd_t *u;

    ((void(*)(int))0x080bfea0)(1); // Unknown function
    Cmd_TokenizeString(s);

    for (u = ucmds; u->name; u++)
    {
        if (!strcmp(Cmd_Argv(0), u->name))
        {
            u->func(cl);
            break;
        }
    }

    if(clientOK)
        if(!u->name && sv.state == SS_GAME)
            VM_Call(gvm, GAME_CLIENT_COMMAND, cl - svs.clients);
}

void UCMD_custom_sprint(client_t *cl)
{
    int clientNum = cl - svs.clients;
    if (!player_sprint->integer)
    {
        std::string message = "e \"";
        message.append("Sprint is not enabled on this server.");
        message.append("\"");
        SV_SendServerCommand(cl, SV_CMD_CAN_IGNORE, message.c_str());
        return;
    }
    
    if(customPlayerState[clientNum].sprintActive)
        customPlayerState[clientNum].sprintActive = false;
    else
        customPlayerState[clientNum].sprintRequestPending = true;
}

// See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/shared.c#L632
#define MAX_VA_STRING 32000
char *custom_va(const char *format, ...)
{
    va_list argptr;
    static char temp_buffer[MAX_VA_STRING];
    static char string[MAX_VA_STRING];
    static int index = 0;
    char *buf;
    int len;

    va_start( argptr, format );
    vsprintf( temp_buffer, format, argptr );
    va_end( argptr );

    if ( ( len = strlen( temp_buffer ) ) >= MAX_VA_STRING )
        Com_Error( ERR_DROP, "Attempted to overrun string in call to va()\n" );

    if ( len + index >= MAX_VA_STRING - 1 )
        index = 0;

    buf = &string[index];
    memcpy( buf, temp_buffer, len + 1 );

    index += len + 1;

    return buf;
}

void custom_SV_ClientThink(int clientNum)
{
    hook_ClientThink->unhook();
    void (*ClientThink)(int clientNum);
    *(int*)&ClientThink = hook_ClientThink->from;
    ClientThink(clientNum);
    hook_ClientThink->hook();

    customPlayerState[clientNum].frames++;

    if (Sys_Milliseconds64() - customPlayerState[clientNum].frameTime >= 1000)
    {
        if(customPlayerState[clientNum].frames > 1000)
            customPlayerState[clientNum].frames = 1000;

        customPlayerState[clientNum].fps = customPlayerState[clientNum].frames;
        customPlayerState[clientNum].frameTime = Sys_Milliseconds64();
        customPlayerState[clientNum].frames = 0;
    }
}

void custom_ClientEndFrame(gentity_t *ent)
{
    hook_ClientEndFrame->unhook();
    void (*ClientEndFrame)(gentity_t *ent);
    *(int*)&ClientEndFrame = hook_ClientEndFrame->from;
    ClientEndFrame(ent);
    hook_ClientEndFrame->hook();

    if (ent->client->sess.sessionState == STATE_PLAYING)
    {
        int clientNum = ent - g_entities;

        if(customPlayerState[clientNum].speed > 0)
            ent->client->ps.speed = customPlayerState[clientNum].speed;
        
        if(customPlayerState[clientNum].sprintActive)
            ent->client->ps.speed *= player_sprintSpeedScale->value;
        
        if(customPlayerState[clientNum].ufo)
            ent->client->ps.pm_type = PM_UFO;

        // Stop slide after fall damage
        if(g_resetSlide->integer)
            if(ent->client->ps.pm_flags & PMF_SLIDING)
                ent->client->ps.pm_flags &= ~PMF_SLIDING;
    }
}

void custom_PM_CrashLand()
{
    int clientNum = pm->ps->clientNum;
    if (customPlayerState[clientNum].overrideJumpHeight_air)
    {
        // Player landed an airjump, disable overrideJumpHeight_air
        customPlayerState[clientNum].overrideJumpHeight_air = false;
    }
    
    hook_PM_CrashLand->unhook();
    void (*PM_CrashLand)();
    *(int*)&PM_CrashLand = hook_PM_CrashLand->from;
    PM_CrashLand();
    hook_PM_CrashLand->hook();
}

void custom_PM_AirMove()
{
    // Player is in air
    int clientNum = pm->ps->clientNum;
    if (customPlayerState[clientNum].airJumpsAvailable > 0)
    {
        // Player is allowed to jump, enable overrideJumpHeight_air
        customPlayerState[clientNum].overrideJumpHeight_air = true;
        customPlayerState[clientNum].jumpHeight = jump_height->value * jump_height_airScale->value;
        
        if (Jump_Check())
        {
            // Air jump initiated
            customPlayerState[clientNum].airJumpsAvailable--;
            if (codecallback_playerairjump)
            {
                gentity_t *gentity = &g_entities[pm->ps->clientNum];
                short ret = Scr_ExecEntThread(gentity, codecallback_playerairjump, 0);
                Scr_FreeThread(ret);
            }
        }
    }
    
    hook_PM_AirMove->unhook();
    void (*PM_AirMove)();
    *(int*)&PM_AirMove = hook_PM_AirMove->from;
    PM_AirMove();
    hook_PM_AirMove->hook();
}

/* See:
- https://github.com/voron00/CoD2rev_Server/blob/b012c4b45a25f7f80dc3f9044fe9ead6463cb5c6/src/bgame/bg_weapons.cpp#L481
- CoD4 1.7: 080570ae
*/
void PM_UpdateSprint()
{
    int timerMsec;
    int clientNum;
    int sprint_time;
    gentity_t *gentity;
    client_t *client;

    clientNum = pm->ps->clientNum;
    gentity = &g_entities[clientNum];
    client = &svs.clients[clientNum];
    sprint_time = (int)(player_sprintTime->value * 1000.0);




    
    if (sprint_time > 0)
    {
        if (customPlayerState[clientNum].sprintRequestPending)
        {
            // Player requested to sprint
            if (/*PM_GetEffectiveStance(&gentity->client->ps) != 0*/(gentity->client->ps.eFlags & EF_CROUCHING) || (gentity->client->ps.eFlags & EF_PRONE)) // "pm->ps->eFlags" didn't work
            {
                // but he is not standing
                // so raise him up
                G_AddPredictableEvent(gentity, EV_STANCE_FORCE_STAND, 0);
            }

            if (client->lastUsercmd.forwardmove == KEY_MASK_FORWARD)
            {
                // he is moving forward
                if(!customPlayerState[clientNum].sprintTimer)
                {
                    printf("#### sprintActive = true\n");
                    customPlayerState[clientNum].sprintActive = true;
                }
            }
            customPlayerState[clientNum].sprintRequestPending = false;
        }
        
        if (customPlayerState[clientNum].sprintActive)
        {
            // Player is sprinting
            if (pm->ps->pm_flags & PMF_CROUCH || pm->ps->pm_flags & PMF_PRONE/*PM_GetEffectiveStance(&gentity->client->ps) != 0*//*((gentity->client->ps.eFlags & EF_CROUCHING) || (gentity->client->ps.eFlags & EF_PRONE))*//*&& raisedPlayer*/)
            {
                printf("#### but he crouched/proned\n");
                // but he crouched/proned
                // so stop his sprint
                customPlayerState[clientNum].sprintActive = false;
            }
            else if (client->lastUsercmd.forwardmove != KEY_MASK_FORWARD)
            {
                // but he stopped moving forward
                // so stop his sprint
                customPlayerState[clientNum].sprintActive = false;                
            }

            timerMsec = customPlayerState[clientNum].sprintTimer + pml->msec;
        }
        else
        {
            timerMsec = customPlayerState[clientNum].sprintTimer - pml->msec;
        }

        if(timerMsec < 0)
            timerMsec = 0;
        customPlayerState[clientNum].sprintTimer = timerMsec;
        
        if (customPlayerState[clientNum].sprintActive && customPlayerState[clientNum].sprintTimer > sprint_time)
        {
            customPlayerState[clientNum].sprintActive = false;
        }
    }
    else
    {
        customPlayerState[clientNum].sprintActive = false;
        customPlayerState[clientNum].sprintTimer = 0;
    }
}

void custom_PmoveSingle(pmove_t *pmove)
{
    hook_PmoveSingle->unhook();
    void (*PmoveSingle)(pmove_t *pmove);
    *(int*)&PmoveSingle = hook_PmoveSingle->from;
    PmoveSingle(pmove);
    hook_PmoveSingle->hook();

    PM_UpdateSprint();
}

// ioquake3 rate limit connectionless requests
// https://github.com/ioquake/ioq3/blob/master/code/server/sv_main.c

// This is deliberately quite large to make it more of an effort to DoS
#define MAX_BUCKETS	16384
#define MAX_HASHES 1024

static leakyBucket_t buckets[MAX_BUCKETS];
static leakyBucket_t* bucketHashes[MAX_HASHES];
leakyBucket_t outboundLeakyBucket;

static long SVC_HashForAddress(netadr_t address)
{
    unsigned char *ip = address.ip;
    int	i;
    long hash = 0;

    for ( i = 0; i < 4; i++ )
    {
        hash += (long)( ip[i] ) * ( i + 119 );
    }

    hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
    hash &= ( MAX_HASHES - 1 );

    return hash;
}

static leakyBucket_t * SVC_BucketForAddress(netadr_t address, int burst, int period)
{
    leakyBucket_t *bucket = NULL;
    int i;
    long hash = SVC_HashForAddress(address);
    uint64_t now = Sys_Milliseconds64();

    for ( bucket = bucketHashes[hash]; bucket; bucket = bucket->next )
    {
        if ( memcmp(bucket->adr, address.ip, 4) == 0 )
            return bucket;
    }

    for ( i = 0; i < MAX_BUCKETS; i++ )
    {
        int interval;

        bucket = &buckets[i];
        interval = now - bucket->lastTime;

        // Reclaim expired buckets
        if ( bucket->lastTime > 0 && ( interval > ( burst * period ) ||
                                       interval < 0 ) )
        {
            if ( bucket->prev != NULL )
                bucket->prev->next = bucket->next;
            else
                bucketHashes[bucket->hash] = bucket->next;

            if ( bucket->next != NULL )
                bucket->next->prev = bucket->prev;

            memset(bucket, 0, sizeof(leakyBucket_t));
        }

        if ( bucket->type == 0 )
        {
            bucket->type = address.type;
            memcpy(bucket->adr, address.ip, 4);

            bucket->lastTime = now;
            bucket->burst = 0;
            bucket->hash = hash;

            // Add to the head of the relevant hash chain
            bucket->next = bucketHashes[hash];
            if ( bucketHashes[hash] != NULL )
                bucketHashes[hash]->prev = bucket;

            bucket->prev = NULL;
            bucketHashes[hash] = bucket;

            return bucket;
        }
    }

    // Couldn't allocate a bucket for this address
    return NULL;
}

bool SVC_RateLimit(leakyBucket_t *bucket, int burst, int period)
{
    if (bucket != NULL)
    {
        uint64_t now = Sys_Milliseconds64();
        int interval = now - bucket->lastTime;
        int expired = interval / period;
        int expiredRemainder = interval % period;

        if ( expired > bucket->burst || interval < 0 )
        {
            bucket->burst = 0;
            bucket->lastTime = now;
        }
        else
        {
            bucket->burst -= expired;
            bucket->lastTime = now - expiredRemainder;
        }

        if ( bucket->burst < burst )
        {
            bucket->burst++;
            return false;
        }
    }
    return true;
}

bool SVC_RateLimitAddress(netadr_t from, int burst, int period)
{
    leakyBucket_t *bucket = SVC_BucketForAddress(from, burst, period);
    return SVC_RateLimit(bucket, burst, period);
}

bool SVC_callback(const char *str, const char *ip)
{
    if (codecallback_client_spam && Scr_IsSystemActive())
    {
        stackPushString(ip);
        stackPushString(str);
        short ret = Scr_ExecThread(codecallback_client_spam, 2);
        Scr_FreeThread(ret);

        return true;
    }
    return false;
}

bool SVC_ApplyRconLimit(netadr_t from, qboolean badRconPassword)
{
    // Prevent using rcon as an amplifier and make dictionary attacks impractical
    if (SVC_RateLimitAddress(from, 10, 1000))
    {
        if(!SVC_callback("RCON:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf("SVC_RemoteCommand: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return true;
    }
    
    if (badRconPassword)
    {
        static leakyBucket_t bucket;

        // Make DoS via rcon impractical
        if (SVC_RateLimit(&bucket, 10, 1000))
        {
            if(!SVC_callback("RCON:GLOBAL", NET_AdrToString(from)))
                Com_DPrintf("SVC_RemoteCommand: rate limit exceeded, dropping request\n");
            return true;
        }
    }
    
    return false;
}

bool SVC_ApplyStatusLimit(netadr_t from)
{
    // Prevent using getstatus as an amplifier
    if (SVC_RateLimitAddress(from, 10, 1000))
    {
        if(!SVC_callback("STATUS:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf("SVC_Status: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return true;
    }

    // Allow getstatus to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if (SVC_RateLimit(&outboundLeakyBucket, 10, 100))
    {
        if(!SVC_callback("STATUS:GLOBAL", NET_AdrToString(from)))
            Com_DPrintf("SVC_Status: rate limit exceeded, dropping request\n");
        return true;
    }

    return false;
}

void hook_SV_GetChallenge(netadr_t from)
{
    // Prevent using getchallenge as an amplifier
    if (SVC_RateLimitAddress(from, 10, 1000))
    {
        if(!SVC_callback("CHALLENGE:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf("SV_GetChallenge: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return;
    }

    // Allow getchallenge to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if (SVC_RateLimit(&outboundLeakyBucket, 10, 100))
    {
        if(!SVC_callback("CHALLENGE:GLOBAL", NET_AdrToString(from)))
            Com_DPrintf("SV_GetChallenge: rate limit exceeded, dropping request\n");
        return;
    }

    SV_GetChallenge(from);
}

void hook_SV_DirectConnect(netadr_t from)
{
    // Prevent using connect as an amplifier
    if (SVC_RateLimitAddress(from, 10, 1000))
    {
        Com_DPrintf("SV_DirectConnect: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return;
    }

    // Allow connect to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if (SVC_RateLimit(&outboundLeakyBucket, 10, 100))
    {
        Com_DPrintf("SV_DirectConnect: rate limit exceeded, dropping request\n");
        return;
    }
    
    bool unbanned;
    char* userinfo;
    char ip[16];
    std::ostringstream oss;
    std::string argBackup;
    
    unbanned = false;
    userinfo = Cmd_Argv(1);
    oss << "connect \"" << userinfo << "\"";
    argBackup = oss.str();
    snprintf(ip, sizeof(ip), "%d.%d.%d.%d", from.ip[0], from.ip[1], from.ip[2], from.ip[3]);

    auto banInfo = banInfoForIp(ip);
    if(std::get<0>(banInfo) == true) // banned
    {
        time_t current_time = time(NULL);
        std::string remainingTime;
        
        if(std::get<1>(banInfo) != -1) // duration
        {
            int elapsed_seconds = difftime(current_time, std::get<2>(banInfo)); // ban date
            int remaining_seconds = std::get<1>(banInfo) - elapsed_seconds;
            if (remaining_seconds <= 0)
            {
                Cbuf_ExecuteText(EXEC_APPEND, custom_va("unban %s\n", ip));
                unbanned = true;
            }
            else
            {
                int days = remaining_seconds / (60 * 60 * 24);
                int hours = (remaining_seconds % (60 * 60 * 24)) / (60 * 60);
                int minutes = (remaining_seconds % (60 * 60)) / 60;
                int seconds = remaining_seconds % 60;

                oss.str(std::string());
                oss.clear();

                if (days > 0)
                {
                    oss << days << " day" << (days > 1 ? "s" : "");
                    if(hours > 0)
                        oss << ", " << hours << " hour" << (hours > 1 ? "s" : "");
                }
                else if (hours > 0)
                {
                    oss << hours << " hour" << (hours > 1 ? "s" : "");
                    if(minutes > 0)
                        oss << ", " << minutes << " minute" << (minutes > 1 ? "s" : "");
                }
                else if(minutes > 0)
                    oss << minutes << " minute" << (minutes > 1 ? "s" : "");
                else
                    oss << seconds << " second" << (seconds > 1 ? "s" : "");

                remainingTime = oss.str();
            }
        }

        if (!unbanned)
        {
            std::string banInfoMessage = "error\nBanned IP";
            if(std::get<3>(banInfo) != "none")
            {
                banInfoMessage.append(" - Reason: ");
                banInfoMessage.append(std::get<3>(banInfo));
            }
            if(!remainingTime.empty())
            {
                banInfoMessage.append(" - Remaining: ");
                banInfoMessage.append(remainingTime);
            }
            
            Com_Printf("rejected connection from banned IP %s\n", NET_AdrToString(from));
            NET_OutOfBandPrint(NS_SERVER, from, banInfoMessage.c_str());
            return;
        }
    }

    if(unbanned)
        Cmd_TokenizeString(argBackup.c_str());
    SV_DirectConnect(from);
}

void hook_SV_AuthorizeIpPacket(netadr_t from)
{
    // Prevent ipAuthorize log spam DoS
    if (SVC_RateLimitAddress(from, 20, 1000))
    {
        Com_DPrintf("SV_AuthorizeIpPacket: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return;
    }

    // Allow ipAuthorize to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if (SVC_RateLimit(&outboundLeakyBucket, 10, 100))
    {
        Com_DPrintf("SV_AuthorizeIpPacket: rate limit exceeded, dropping request\n");
        return;
    }

    SV_AuthorizeIpPacket(from);
}

void hook_SVC_Info(netadr_t from)
{
    // Prevent using getinfo as an amplifier
    if (SVC_RateLimitAddress(from, 10, 1000))
    {
        if (!SVC_callback("INFO:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf("SVC_Info: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return;
    }

    // Allow getinfo to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if (SVC_RateLimit(&outboundLeakyBucket, 10, 100))
    {
        if(!SVC_callback("INFO:GLOBAL", NET_AdrToString(from)))
            Com_DPrintf("SVC_Info: rate limit exceeded, dropping request\n");
        return;
    }

    SVC_Info(from);
}

void hook_SVC_Status(netadr_t from)
{
    if(SVC_ApplyStatusLimit(from))
        return;
    SVC_Status(from);
}

// See https://nachtimwald.com/2017/04/02/constant-time-string-comparison-in-c/
bool str_iseq(const char *s1, const char *s2)
{
    int             m = 0;
    volatile size_t i = 0;
    volatile size_t j = 0;
    volatile size_t k = 0;

    if (s1 == NULL || s2 == NULL)
        return false;

    while (1) {
        m |= s1[i]^s2[j];

        if (s1[i] == '\0')
            break;
        i++;

        if (s2[j] != '\0')
            j++;
        if (s2[j] == '\0')
            k++;
    }

    return m == 0;
}
void hook_SVC_RemoteCommand(netadr_t from, msg_t *msg)
{
    char* password = Cmd_Argv(1);
    qboolean badRconPassword = !strlen(sv_rconPassword->string) || !str_iseq(password, sv_rconPassword->string);
    
    if(SVC_ApplyRconLimit(from, badRconPassword))
        return;
    
    SVC_RemoteCommand(from, msg);
}

void ServerCrash(int sig)
{
    int fd;
    FILE *fp;
    void *array[20];
    size_t size = backtrace(array, 20);

    // Write to crash log
    fp = fopen("./crash.log", "a");
    if (fp)
    {
        fd = fileno(fp);
        fseek(fp, 0, SEEK_END);
        fprintf(fp, "Error: Server crashed with signal 0x%x {%d}\n", sig, sig);
        fflush(fp);
        backtrace_symbols_fd(array, size, fd);
    }
    
    // Write to stderr
    fprintf(stderr, "Error: Server crashed with signal 0x%x {%d}\n", sig, sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    
    system("stty sane");
    exit(1);
}

void* custom_Sys_LoadDll(const char *name, char *fqpath, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...))
{
    hook_Sys_LoadDll->unhook();
    void*(*Sys_LoadDll)(const char *name, char *fqpath, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...));
    *(int*)&Sys_LoadDll = hook_Sys_LoadDll->from;
    void* libHandle = Sys_LoadDll(name, fqpath, entryPoint, systemcalls);
    hook_Sys_LoadDll->hook();
    
    char libPath[512];
    char buf[512];
    char flags[4];
    void *low, *high;
    FILE *fp;
    
    if(*fs_game->string)
        sprintf(libPath, "%s/game.mp.i386.so", fs_game->string);
    else
        sprintf(libPath, "main/game.mp.i386.so");
    
    //// Unprotect game.mp.i386.so
    // See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/librarymodule.c#L55
    fp = fopen("/proc/self/maps", "r");
    if(!fp)
        return 0;
    while (fgets(buf, sizeof(buf), fp))
    {
        if(!strstr(buf, libPath))
            continue;
        if(sscanf (buf, "%p-%p %4c", &low, &high, flags) != 3)
            continue;
        mprotect((void *)low, (int)high-(int)low, PROT_READ | PROT_WRITE | PROT_EXEC);
    }
    fclose(fp);
    ////
    
    // Objects
    g_clients = (gclient_t*)dlsym(libHandle, "g_clients");
    g_entities = (gentity_t*)dlsym(libHandle, "g_entities");
    level = (level_locals_t*)dlsym(libHandle, "level");
    pm = (pmove_t*)dlsym(libHandle, "pm");
    pml = (pml_t*)dlsym(libHandle, "pml");
    scr_const = (stringIndex_t*)dlsym(libHandle, "scr_const");

    // Functions
    Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)dlsym(libHandle, "Scr_GetFunctionHandle");
    Scr_GetNumParam = (Scr_GetNumParam_t)dlsym(libHandle, "Scr_GetNumParam");
    trap_Argv = (trap_Argv_t)dlsym(libHandle, "trap_Argv");
    ClientCommand = (ClientCommand_t)dlsym(libHandle, "ClientCommand");
    Com_SkipRestOfLine = (Com_SkipRestOfLine_t)dlsym(libHandle, "Com_SkipRestOfLine");
    Com_ParseRestOfLine = (Com_ParseRestOfLine_t)dlsym(libHandle, "Com_ParseRestOfLine");
    Com_ParseInt = (Com_ParseInt_t)dlsym(libHandle, "Com_ParseInt");
    Scr_GetFunction = (Scr_GetFunction_t)dlsym(libHandle, "Scr_GetFunction");
    Scr_GetMethod = (Scr_GetMethod_t)dlsym(libHandle, "Scr_GetMethod");
    trap_SendServerCommand = (trap_SendServerCommand_t)dlsym(libHandle, "trap_SendServerCommand");
    Scr_ExecThread = (Scr_ExecThread_t)dlsym(libHandle, "Scr_ExecThread");
    Scr_ExecEntThread = (Scr_ExecEntThread_t)dlsym(libHandle, "Scr_ExecEntThread");
    Scr_ExecEntThreadNum = (Scr_ExecEntThreadNum_t)dlsym(libHandle, "Scr_ExecEntThreadNum");
    Scr_FreeThread = (Scr_FreeThread_t)dlsym(libHandle, "Scr_FreeThread");
    Scr_Error = (Scr_Error_t)dlsym(libHandle, "Scr_Error");
    Scr_ObjectError = (Scr_ObjectError_t)dlsym(libHandle, "Scr_ObjectError");
    Scr_GetConstString = (Scr_GetConstString_t)dlsym(libHandle, "Scr_GetConstString");
    Scr_ParamError = (Scr_ParamError_t)dlsym(libHandle, "Scr_ParamError");
    G_Say = (G_Say_t)dlsym(libHandle, "G_Say");
    G_RegisterCvars = (G_RegisterCvars_t)dlsym(libHandle, "G_RegisterCvars");
    G_AddEvent = (G_AddEvent_t)dlsym(libHandle, "G_AddEvent");
    G_AddPredictableEvent = (G_AddPredictableEvent_t)dlsym(libHandle, "G_AddPredictableEvent");
    trap_GetConfigstringConst = (trap_GetConfigstringConst_t)dlsym(libHandle, "trap_GetConfigstringConst");
    trap_GetConfigstring = (trap_GetConfigstring_t)dlsym(libHandle, "trap_GetConfigstring");
    BG_GetNumWeapons = (BG_GetNumWeapons_t)dlsym(libHandle, "BG_GetNumWeapons");
    BG_GetInfoForWeapon = (BG_GetInfoForWeapon_t)dlsym(libHandle, "BG_GetInfoForWeapon");
    BG_GetWeaponIndexForName = (BG_GetWeaponIndexForName_t)dlsym(libHandle, "BG_GetWeaponIndexForName");
    BG_AnimationIndexForString = (BG_AnimationIndexForString_t)dlsym(libHandle, "BG_AnimationIndexForString");
    Scr_IsSystemActive = (Scr_IsSystemActive_t)dlsym(libHandle, "Scr_IsSystemActive");
    Scr_GetInt = (Scr_GetInt_t)dlsym(libHandle, "Scr_GetInt");
    Scr_GetString = (Scr_GetString_t)dlsym(libHandle, "Scr_GetString");
    Scr_GetType = (Scr_GetType_t)dlsym(libHandle, "Scr_GetType");
    Scr_GetEntity = (Scr_GetEntity_t)dlsym(libHandle, "Scr_GetEntity");
    Scr_AddBool = (Scr_AddBool_t)dlsym(libHandle, "Scr_AddBool");
    Scr_AddInt = (Scr_AddInt_t)dlsym(libHandle, "Scr_AddInt");
    Scr_AddFloat = (Scr_AddFloat_t)dlsym(libHandle, "Scr_AddFloat");
    Scr_AddString = (Scr_AddString_t)dlsym(libHandle, "Scr_AddString");
    Scr_AddUndefined = (Scr_AddUndefined_t)dlsym(libHandle, "Scr_AddUndefined");
    Scr_AddVector = (Scr_AddVector_t)dlsym(libHandle, "Scr_AddVector");
    Scr_MakeArray = (Scr_MakeArray_t)dlsym(libHandle, "Scr_MakeArray");
    Scr_AddArray = (Scr_AddArray_t)dlsym(libHandle, "Scr_AddArray");
    Scr_AddObject = (Scr_AddObject_t)dlsym(libHandle, "Scr_AddObject");
    Scr_LoadScript = (Scr_LoadScript_t)dlsym(libHandle, "Scr_LoadScript");
    StuckInClient = (StuckInClient_t)dlsym(libHandle, "StuckInClient");
    Q_strlwr = (Q_strlwr_t)dlsym(libHandle, "Q_strlwr");
    Q_strupr = (Q_strupr_t)dlsym(libHandle, "Q_strupr");
    Q_strcat = (Q_strcat_t)dlsym(libHandle, "Q_strcat");
    Q_strncpyz = (Q_strncpyz_t)dlsym(libHandle, "Q_strncpyz");
    Q_CleanStr = (Q_CleanStr_t)dlsym(libHandle, "Q_CleanStr");
    Jump_Check = (Jump_Check_t)((int)dlsym(libHandle, "_init") + 0x76F4);
    PM_GetEffectiveStance = (PM_GetEffectiveStance_t)dlsym(libHandle, "PM_GetEffectiveStance");

    //// Patch codmsgboom
    /* See:
    - https://aluigi.altervista.org/adv/codmsgboom-adv.txt
    - https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/librarymodule.c#L146
    */
    *(int*)((int)dlsym(libHandle, "G_Say") + 0x50e) = 0x37f;
    *(int*)((int)dlsym(libHandle, "G_Say") + 0x5ca) = 0x37f;
    ////

    //// 1.1 deadchat support
    // See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/librarymodule.c#L161

    *(byte*)((int)dlsym(libHandle, "G_Say") + 0x2B3) = 0xeb;
    *(byte*)((int)dlsym(libHandle, "G_Say") + 0x3B6) = 0xeb;
    hook_call((int)dlsym(libHandle, "G_Say") + 0x5EA, (int)hook_G_Say);
    hook_call((int)dlsym(libHandle, "G_Say") + 0x77D, (int)hook_G_Say);
    hook_call((int)dlsym(libHandle, "G_Say") + 0x791, (int)hook_G_Say);
    ////
    
    hook_call((int)dlsym(libHandle, "vmMain") + 0xB0, (int)hook_ClientCommand);
    hook_call((int)dlsym(libHandle, "ClientEndFrame") + 0x311, (int)hook_StuckInClient);

    hook_jmp((int)dlsym(libHandle, "G_LocalizedStringIndex"), (int)custom_G_LocalizedStringIndex);

    // Jump height override
    hook_jmp((int)dlsym(libHandle, "BG_PlayerTouchesItem") + 0x88C, (int)hook_Jump_Check_Naked);
    resume_addr_Jump_Check = (uintptr_t)dlsym(libHandle, "BG_PlayerTouchesItem") + 0x892;
    hook_jmp((int)dlsym(libHandle, "BG_PlayerTouchesItem") + 0x899, (int)hook_Jump_Check_Naked_2);
    resume_addr_Jump_Check_2 = (uintptr_t)dlsym(libHandle, "BG_PlayerTouchesItem") + 0x8A4;
    
    hook_GScr_LoadGameTypeScript = new cHook((int)dlsym(libHandle, "GScr_LoadGameTypeScript"), (int)custom_GScr_LoadGameTypeScript);
    hook_GScr_LoadGameTypeScript->hook();
    hook_ClientThink = new cHook((int)dlsym(libHandle, "ClientThink"), (int)custom_SV_ClientThink);
    hook_ClientThink->hook();
    hook_ClientEndFrame = new cHook((int)dlsym(libHandle, "ClientEndFrame"), (int)custom_ClientEndFrame);
    hook_ClientEndFrame->hook();

    hook_PmoveSingle = new cHook((int)dlsym(libHandle, "PmoveSingle"), (int)custom_PmoveSingle);
    hook_PmoveSingle->hook();
    
    //// Air jumping
    hook_PM_AirMove = new cHook((int)dlsym(libHandle, "_init") + 0x7B98, (int)custom_PM_AirMove);
    hook_PM_AirMove->hook();
    hook_PM_CrashLand = new cHook((int)dlsym(libHandle, "_init") + 0x88C4, (int)custom_PM_CrashLand);
    hook_PM_CrashLand->hook();
    // TODO: Ignore the JLE only for players allowed to air jump
    int addr_Jump_Check_JLE = (int)dlsym(libHandle, "BG_PlayerTouchesItem") + 0x7FD; // if ( pm->cmd.serverTime - pm->ps->jumpTime <= 499 )
    hook_nop(addr_Jump_Check_JLE, addr_Jump_Check_JLE + 2);
    ////
    
    return libHandle;
}

class libcod
{
    public:
    libcod()
    {
        printf("------------- libcod -------------\n");
        printf("Compiled on %s %s using g++ %s\n", __DATE__, __TIME__, __VERSION__);

        // Don't inherit lib of parent
        unsetenv("LD_PRELOAD");

        // Crash handlers for debugging
        signal(SIGSEGV, ServerCrash);
        signal(SIGABRT, ServerCrash);
        
        // Otherwise the printf()'s are printed at crash/end on older os/compiler versions
        // See https://github.com/M-itch/libcod/blob/e58d6a01b11c911fbf886659b6ea67795776cf4a/libcod.cpp#L1346
        setbuf(stdout, NULL);

        // Allow to write in executable memory
        mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);

        // Patch q3infoboom
        /* See:
        - https://aluigi.altervista.org/adv/q3infoboom-adv.txt
        - https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/codextended.c#L295
        */
        *(byte*)0x807f459 = 1;

        // Patch RCON half-second limit //TODO: Do like zk_libcod instead
        /* See:
        - https://aluigi.altervista.org/patches/q3rconz.lpatch
        - https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/codextended.c#L291
        - https://github.com/ibuddieat/zk_libcod/blob/0f07cacf303d104a0375bf6235b8013e30b668ca/code/libcod.cpp#L3486
        */
        *(unsigned char*)0x808C41F = 0xeb;
        
        hook_call(0x08085213, (int)hook_AuthorizeState);
        hook_call(0x08094c54, (int)Scr_GetCustomFunction);
        hook_call(0x080951c4, (int)Scr_GetCustomMethod);
        hook_call(0x0808c780, (int)hook_SV_GetChallenge);
        hook_call(0x0808c7b8, (int)hook_SV_DirectConnect);
        hook_call(0x0808c7ea, (int)hook_SV_AuthorizeIpPacket);
        hook_call(0x0808c74e, (int)hook_SVC_Info);
        hook_call(0x0808c71c, (int)hook_SVC_Status);
        hook_call(0x0808c81d, (int)hook_SVC_RemoteCommand);

        hook_jmp(0x080717a4, (int)custom_FS_ReferencedPakChecksums);
        hook_jmp(0x080716cc, (int)custom_FS_ReferencedPakNames);
        hook_jmp(0x080872ec, (int)custom_SV_ExecuteClientMessage);
        hook_jmp(0x08086d58, (int)custom_SV_ExecuteClientCommand);

        hook_Sys_LoadDll = new cHook(0x080c5fe4, (int)custom_Sys_LoadDll);
        hook_Sys_LoadDll->hook();
        hook_Com_Init = new cHook(0x0806c654, (int)custom_Com_Init);
        hook_Com_Init->hook();
        hook_SV_SpawnServer = new cHook(0x0808a220, (int)custom_SV_SpawnServer);
        hook_SV_SpawnServer->hook();
        hook_SV_BeginDownload_f = new cHook(0x08087a64, (int)custom_SV_BeginDownload_f);
        hook_SV_BeginDownload_f->hook();
        hook_SV_SendClientGameState = new cHook(0x08085eec, (int)custom_SV_SendClientGameState);
        hook_SV_SendClientGameState->hook();
        hook_SV_AddOperatorCommands = new cHook(0x08084a3c, (int)custom_SV_AddOperatorCommands);
        hook_SV_AddOperatorCommands->hook();

        printf("Loading complete\n");
        printf("-----------------------------------\n");
    }

    ~libcod()
    {
        printf("Libcod unloaded\n");
        system("stty sane");
    }
};

libcod *lc;
void __attribute__ ((constructor)) lib_load(void)
{
    lc = new libcod;
}
void __attribute__ ((destructor)) lib_unload(void)
{
    delete lc;
}