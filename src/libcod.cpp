#if COMPILE_LIBCURL == 1
#include <curl/curl.h>
#include "vendor/json.hpp"
#include <string>
#include <thread>
#include <map>

using json = nlohmann::json;
#endif

#include <signal.h>

#include "gsc.hpp"

// Stock cvars
cvar_t *com_cl_running;
cvar_t *com_dedicated;
cvar_t *com_logfile;
cvar_t *com_sv_running;
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
cvar_t *sv_cracked;
cvar_t *sv_antiVpn;
cvar_t *sv_antiVpn_apiKey;

cHook *hook_clientendframe;
cHook *hook_com_init;
cHook *hook_cvar_set2;
cHook *hook_g_localizedstringindex;
cHook *hook_gametype_scripts;
cHook *hook_play_movement;
cHook *hook_sv_addoperatorcommands;
cHook *hook_sv_spawnserver;
cHook *hook_sv_begindownload_f;
cHook *hook_sv_sendclientgamestate;
cHook *hook_sys_loaddll;

// Stock callbacks
int codecallback_startgametype = 0;
int codecallback_playerconnect = 0;
int codecallback_playerdisconnect = 0;
int codecallback_playerdamage = 0;
int codecallback_playerkilled = 0;

// Custom callbacks
int codecallback_client_spam = 0;
int codecallback_playercommand = 0;

callback_t callbacks[] =
{
    { &codecallback_startgametype, "CodeCallback_StartGameType" }, // g_scr_data.gametype.startupgametype
    { &codecallback_playerconnect, "CodeCallback_PlayerConnect" }, // g_scr_data.gametype.playerconnect
    { &codecallback_playerdisconnect, "CodeCallback_PlayerDisconnect" }, // g_scr_data.gametype.playerdisconnect
    { &codecallback_playerdamage, "CodeCallback_PlayerDamage" }, // g_scr_data.gametype.playerdamage
    { &codecallback_playerkilled, "CodeCallback_PlayerKilled" }, // g_scr_data.gametype.playerkilled

    { &codecallback_client_spam, "CodeCallback_CLSpam"},
    { &codecallback_playercommand, "CodeCallback_PlayerCommand"},
};

customPlayerState_t customPlayerState[MAX_CLIENTS];

// Game lib objects
gentity_t* g_entities;
gclient_t* g_clients;
level_locals_t* level;
pmove_t* pm;
stringIndex_t* scr_const;

// Game lib functions
G_Say_t G_Say;
G_RegisterCvars_t G_RegisterCvars;
SV_GameSendServerCommand_t SV_GameSendServerCommand;
SV_GetConfigstringConst_t SV_GetConfigstringConst;
SV_GetConfigstring_t SV_GetConfigstring;
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
Cmd_ArgvBuffer_t Cmd_ArgvBuffer;
ClientCommand_t ClientCommand;
Com_SkipRestOfLine_t Com_SkipRestOfLine;

void custom_Com_Init(char *commandLine)
{
    hook_com_init->unhook();
    void (*Com_Init)(char *commandLine);
    *(int *)&Com_Init = hook_com_init->from;
    Com_Init(commandLine);
    hook_com_init->hook();
    
    // Get references to stock cvars
    com_cl_running = Cvar_FindVar("cl_running");
    com_dedicated = Cvar_FindVar("dedicated");
    com_logfile = Cvar_FindVar("logfile");
    com_sv_running = Cvar_FindVar("sv_running");
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
    Cvar_Get("sv_wwwDownload", "0", CVAR_SYSTEMINFO | CVAR_ARCHIVE);
    Cvar_Get("sv_wwwBaseURL", "", CVAR_SYSTEMINFO | CVAR_ARCHIVE);
    
    fs_callbacks = Cvar_Get("fs_callbacks", "", CVAR_ARCHIVE);
    fs_callbacks_additional = Cvar_Get("fs_callbacks_additional", "", CVAR_ARCHIVE);
    fs_svrPaks = Cvar_Get("fs_svrPaks", "", CVAR_ARCHIVE);
    g_deadChat = Cvar_Get("g_deadChat", "0", CVAR_ARCHIVE);
    g_debugCallbacks = Cvar_Get("g_debugCallbacks", "0", CVAR_ARCHIVE);
    g_playerEject = Cvar_Get("g_playerEject", "1", CVAR_ARCHIVE);
    g_resetSlide = Cvar_Get("g_resetSlide", "0", CVAR_ARCHIVE);
    sv_antiVpn = Cvar_Get("sv_antiVpn", "0", CVAR_ARCHIVE);
    sv_antiVpn_apiKey = Cvar_Get("sv_antiVpn_apiKey", "", CVAR_ARCHIVE);
    sv_cracked = Cvar_Get("sv_cracked", "0", CVAR_ARCHIVE);

    /*
    Force cl_allowDownload on client, otherwise 1.1x can't download to join the server
    TODO: Force only for 1.1x clients
    */
    Cvar_Get("cl_allowDownload", "1", CVAR_SYSTEMINFO);
}

void hook_G_Say(gentity_s *ent, gentity_s *target, int mode, const char *chatText)
{
    // 1.1 deadchat support
    /* See:
    - https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/sv_client.c#L940
    */

    int unknown_var = *(int*)((int)ent->client + 8400);
    if(unknown_var && !g_deadChat->integer)
        return;

    G_Say(ent, NULL, mode, chatText);
}

qboolean FS_svrPak(const char *base)
{
    if (strstr(base, "_svr_"))
        return qtrue;

    if (*fs_svrPaks->string)
    {
        bool isSvrPak = false;
        size_t lenString = strlen(fs_svrPaks->string) +1;
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
    
    for (search = fs_searchpaths ; search ; search = search->next)
    {
        if (!search->pak)
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

    for (search = fs_searchpaths ; search ; search = search->next)
    {
        if (!search->pak)
            continue;
        
        if(FS_svrPak(search->pak->pakBasename))
            continue;
        
        Q_strcat(info, sizeof( info ), custom_va("%i ", search->pak->checksum));
    }

    return info;
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


    if(*fs_callbacks_additional->string)
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
        if(!strcmp(callbacks[i].name, "CodeCallback_PlayerCommand")) // Custom callback: PlayerCommand
            *callbacks[i].pos = Scr_GetFunctionHandle(fs_callbacks_additional->string, callbacks[i].name);
        else
            *callbacks[i].pos = Scr_GetFunctionHandle(path_for_cb, callbacks[i].name);
        
        /*if ( *callbacks[i].pos && g_debugCallbacks->integer )
            Com_Printf("%s found @ %p\n", callbacks[i].name, scrVarPub.programBuffer + *callbacks[i].pos);*/ //TODO: verify scrVarPub_t
    }

    return ret;
}

static int localized_string_index = 128;
int custom_G_LocalizedStringIndex(const char *string)
{
    // See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/script.c#L944
    
    int i;
    char s[MAX_STRINGLENGTH];

    if(localized_string_index >= 256)
        localized_string_index = 128;

    if(!string || !*string)
        return 0;
    
    int start = 1244;

    for(i = 1; i < 256; i++)
    {
        SV_GetConfigstring(start + i, s, sizeof(s));
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
      
    if(!codecallback_playercommand)
    {
        ClientCommand(clientNum);
        return;
    }

    stackPushArray();
    int args = Cmd_Argc();
    for(int i = 0; i < args; i++)
    {
        char tmp[MAX_STRINGLENGTH];
        Cmd_ArgvBuffer(i, tmp, sizeof(tmp));
        if(i == 1 && tmp[0] >= 20 && tmp[0] <= 22)
        {
            char *part = strtok(tmp + 1, " ");
            while(part != NULL)
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
    hook_sv_spawnserver->unhook();
    void (*SV_SpawnServer)(char *server);
    *(int *)&SV_SpawnServer = hook_sv_spawnserver->from;
    SV_SpawnServer(server);
    hook_sv_spawnserver->hook();

#if COMPILE_SQLITE == 1
    free_sqlite_db_stores_and_tasks();
#endif
}

qboolean isBannedIp(char* ip)
{
    char *file;
    int banned;
    const char *token;
    const char *text;

    if (FS_ReadFile("ban.txt", (void **)&file) < 0)
        return 0;

    text = file;
    banned = 0;

    while (1)
    {
        token = Com_Parse(&text);
        if (!token[0])
            break;

        if (!strcmp(token, ip))
        {
            banned = 1;
            break;
        }
        Com_SkipRestOfLine(&text);
    }
    FS_FreeFile(file);
    return banned;
}

static void ban()
{
    client_t *cl;
    int file;
    char cleanName[64];
    char ip[16];

    if (!com_sv_running->integer)
    {
        Com_Printf("Server is not running.\n");
        return;
    }

    if (Cmd_Argc() != 2)
    {
        Com_Printf("Usage: ban <client number>\n");
        return;
    }

    cl = SV_GetPlayerByNum();
    if (cl)
    {
        snprintf(ip, sizeof(ip), "%d.%d.%d.%d", cl->netchan.remoteAddress.ip[0], cl->netchan.remoteAddress.ip[1], cl->netchan.remoteAddress.ip[2], cl->netchan.remoteAddress.ip[3]);
        if (isBannedIp(ip))
        {
            Com_Printf("This IP (%s) is already banned\n", NET_AdrToString(cl->netchan.remoteAddress));
            return;
        }

        if (FS_FOpenFileByMode("ban.txt", &file, FS_APPEND) < 0)
            return;

        Q_strncpyz(cleanName, cl->name, sizeof(cleanName));
        FS_Write(file, "%s %s\r\n", ip, cleanName);
        FS_FCloseFile(file);
        SV_DropClient(cl, "EXE_PLAYERKICKED");
        cl->lastPacketTime = svs.time;
    }
}

static void unban()
{
    if (Cmd_Argc() != 2)
    {
        Com_Printf("Usage: unban <client number>\n");
        return;
    }

    char *file;
    int fileSize;
    char *line;
    const char *token;
    char *ip;
    bool found;
    char *text;

    fileSize = FS_ReadFile("ban.txt", (void **)&file);
    if (fileSize < 0)
        return;
    
    ip = Cmd_Argv(1);
    found = false;
    text = file;

    while (1)
    {
        line = text;
        token = Com_Parse((const char **)&text);
        if (!token[0])
            break;

        if (!strcmp(token, ip))
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
        Com_Printf("unbanned IP %s\n", ip);
    else
        Com_Printf("IP %s not found\n", ip);
}

void custom_SV_AddOperatorCommands()
{
    hook_sv_addoperatorcommands->unhook();
    void (*SV_AddOperatorCommands)();
    *(int *)&SV_AddOperatorCommands = hook_sv_addoperatorcommands->from;
    SV_AddOperatorCommands();

    Cmd_AddCommand("ban", ban);
    Cmd_AddCommand("unban", unban);

    hook_sv_addoperatorcommands->hook();
}

void custom_SV_SendClientGameState(client_t *client)
{
    hook_sv_sendclientgamestate->unhook();
    void (*SV_SendClientGameState)(client_t *client);
    *(int *)&SV_SendClientGameState = hook_sv_sendclientgamestate->from;
    SV_SendClientGameState(client);
    hook_sv_sendclientgamestate->hook();

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

qboolean shouldServeFile(const char *requestedFilePath)
{
    static char localFilePath[MAX_OSPATH*2+5];
    searchpath_t* search;

    localFilePath[0] = 0;

    for(search = fs_searchpaths; search; search = search->next)
    {
        if(search->pak)
        {
            snprintf(localFilePath, sizeof(localFilePath), "%s/%s.pk3", search->pak->pakGamename, search->pak->pakBasename);
            if(!strcmp(localFilePath, requestedFilePath))
                if(!FS_svrPak(search->pak->pakBasename))
                    return qtrue;
        }
    }
    return qfalse;
}

void custom_SV_BeginDownload_f(client_t *cl)
{
    // Patch q3dirtrav
    int args = Cmd_Argc();
    if(args > 1)
    {
        const char* arg1 = Cmd_Argv(1);
        if(!shouldServeFile(arg1))
        {
            char ip[16];
            snprintf(ip, sizeof(ip), "%d.%d.%d.%d", cl->netchan.remoteAddress.ip[0], cl->netchan.remoteAddress.ip[1], cl->netchan.remoteAddress.ip[2], cl->netchan.remoteAddress.ip[3]);
            printf("####### WARNING: %s (%s) tried to download %s.\n", cl->name, ip, arg1);
            return;
        }
    }

    hook_sv_begindownload_f->unhook();
    void (*SV_BeginDownload_f)(client_t *cl);
    *(int *)&SV_BeginDownload_f = hook_sv_begindownload_f->from;
    SV_BeginDownload_f(cl);
    hook_sv_begindownload_f->hook();
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
        do {
            c = MSG_ReadBits(&decompressMsg, 2);
            if (c == clc_EOF || c != clc_clientCommand)
            {
                if(sv_pure->integer && cl->pureAuthentic == 2)
                {
                    cl->nextSnapshotTime = -1;
                    SV_DropClient(cl, "EXE_UNPURECLIENTDETECTED");
                    cl->state = CS_ACTIVE;
                    SV_SendClientSnapshot(cl);
                    cl->state = CS_ZOMBIE;
                }
                if(c == clc_move)
                {
                    SV_UserMove(cl, &decompressMsg, 1);
                }
                else if(c == clc_moveNoDelta)
                {
                    SV_UserMove(cl, &decompressMsg, 0);
                }
                else if(c != clc_EOF)
                {
                    Com_Printf("WARNING: bad command byte %i for client %i\n", c, cl - svs.clients);
                }
                return;
            }

            if (!SV_ClientCommand(cl, &decompressMsg))
                return;

        } while (cl->state != CS_ZOMBIE);
    }
    else if((cl->serverId & 0xF0) == (sv_serverId_value & 0xF0))
    {
        if (cl->state == CS_PRIMED)
        {
            SV_ClientEnterWorld(cl, &cl->lastUsercmd);
        }
    }
    else
    {
        if(cl->gamestateMessageNum < cl->messageAcknowledge)
        {
            Com_DPrintf("%s : dropped gamestate, resending\n", cl->name);
            SV_SendClientGameState(cl);
        }
    }
}

char *custom_va(const char *format, ...)
{
    // See https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/shared.c#L632

    #define MAX_VA_STRING 32000
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
    hook_play_movement->unhook();
    void (*ClientThink)(int clientNum);
    *(int *)&ClientThink = hook_play_movement->from;
    ClientThink(clientNum);
    hook_play_movement->hook();

    customPlayerState[clientNum].frames++;

    if ( Sys_Milliseconds64() - customPlayerState[clientNum].frameTime >= 1000 )
    {
        if ( customPlayerState[clientNum].frames > 1000 )
            customPlayerState[clientNum].frames = 1000;

        customPlayerState[clientNum].fps = customPlayerState[clientNum].frames;
        customPlayerState[clientNum].frameTime = Sys_Milliseconds64();
        customPlayerState[clientNum].frames = 0;
    }
}

int custom_ClientEndFrame(gentity_t *ent)
{
    hook_clientendframe->unhook();
    int (*ClientEndFrame)(gentity_t *ent);
    *(int *)&ClientEndFrame = hook_clientendframe->from;
    int ret = ClientEndFrame(ent);
    hook_clientendframe->hook();

    if (ent->client->sess.sessionState == STATE_PLAYING)
    {
        int num = ent - g_entities;

        if (customPlayerState[num].speed > 0)
            ent->client->ps.speed = customPlayerState[num].speed;

        if (customPlayerState[num].ufo == 1)
            ent->client->ps.pm_type = PM_UFO;

        // Experimental slide bug fix
        if (g_resetSlide->integer)
        {
            if ((ent->client->ps.pm_flags & PMF_SLIDING) != 0 /*&& (ent->client->ps).pm_time == 0*/)
            {
                //printf("##### SLIDING\n");
                //printf("##### pm_time = %i\n", (ent->client->ps).pm_time);
                ent->client->ps.pm_flags = ent->client->ps.pm_flags & ~PMF_SLIDING;
            }
        }
    }

    return ret;
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
        if (!SVC_callback("RCON:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf("SVC_RemoteCommand: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return true;
    }
    
    if (badRconPassword)
    {
        static leakyBucket_t bucket;

        // Make DoS via rcon impractical
        if (SVC_RateLimit(&bucket, 10, 1000))
        {
            if (!SVC_callback("RCON:GLOBAL", NET_AdrToString(from)))
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
        if (!SVC_callback("STATUS:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf("SVC_Status: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return true;
    }

    // Allow getstatus to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if (SVC_RateLimit(&outboundLeakyBucket, 10, 100))
    {
        if ( !SVC_callback("STATUS:GLOBAL", NET_AdrToString(from)) )
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
        if (!SVC_callback("CHALLENGE:ADDRESS", NET_AdrToString(from)))
            Com_DPrintf("SV_GetChallenge: rate limit from %s exceeded, dropping request\n", NET_AdrToString(from));
        return;
    }

    // Allow getchallenge to be DoSed relatively easily, but prevent
    // excess outbound bandwidth usage when being flooded inbound
    if ( SVC_RateLimit(&outboundLeakyBucket, 10, 100) )
    {
        if (!SVC_callback("CHALLENGE:GLOBAL", NET_AdrToString(from)))
            Com_DPrintf("SV_GetChallenge: rate limit exceeded, dropping request\n");
        return;
    }

    SV_GetChallenge(from);
}

std::map<std::string, bool> vpnIpsMap;
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
void SV_DirectConnect_checkVpn(std::string ip, netadr_t from)
{
    CURL *curl = curl_easy_init();
    CURLcode res;
    if (curl)
    {
        std::string provider = "https://vpnapi.io/api/";
        std::string provider_parameter = "?key=";
        std::string apiKey = sv_antiVpn_apiKey->string;
        std::string url = provider + ip + provider_parameter + apiKey;

        std::string response_string;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (res == CURLE_OK)
        {
            json j = json::parse(response_string);
            if (j.contains("security"))
            {
                json security = j["security"];
                if (security.contains("vpn") && security["vpn"].is_boolean())
                {
                    bool isVpn = security["vpn"].get<bool>();
                    if(isVpn)
                    {
                        Com_Printf("rejected connection from VPN %s\n", NET_AdrToString(from));
                        NET_OutOfBandPrint(NS_SERVER, from, "error\nVPN connection rejected");
                        vpnIpsMap[ip] = true;
                        return;
                    }
                    else
                    {
                        vpnIpsMap[ip] = false;
                    }
                }
            }
        }
    }
    SV_DirectConnect(from);
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

    char ip[16];
    snprintf(ip, sizeof(ip), "%d.%d.%d.%d", from.ip[0], from.ip[1], from.ip[2], from.ip[3]);

    if (isBannedIp(ip))
    {
        Com_Printf("rejected connection from banned IP %s\n", NET_AdrToString(from));
        NET_OutOfBandPrint(NS_SERVER, from, "error\nYou are banned from this server");
        return;
    }

    if(sv_antiVpn->integer)
    {
        if(*sv_antiVpn_apiKey->string)
        {
            std::string ipString(ip);
            auto it = vpnIpsMap.find(ipString);
            if (it != vpnIpsMap.end())
            {
                bool isVpn = it->second;
                if (isVpn)
                {
                    Com_Printf("rejected connection from VPN %s\n", NET_AdrToString(from));
                    NET_OutOfBandPrint(NS_SERVER, from, "error\nVPN connection rejected");
                    return;
                }
            }
            else
            {
                std::thread myThread(SV_DirectConnect_checkVpn, ipString, from);
                myThread.detach();
                return;
            }
        }
        else
        {
            Com_Printf("sv_antiVpn is enabled but sv_antiVpn_apiKey is empty\n");
        }
    }

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
        if (!SVC_callback("INFO:GLOBAL", NET_AdrToString(from)))
            Com_DPrintf("SVC_Info: rate limit exceeded, dropping request\n");
        return;
    }

    SVC_Info(from);
}

void hook_SVC_Status(netadr_t from)
{
    if (SVC_ApplyStatusLimit(from))
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
    
    if (SVC_ApplyRconLimit(from, badRconPassword))
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
    exit(1);
}

void* custom_Sys_LoadDll(const char *name, char *fqpath, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...))
{
    hook_sys_loaddll->unhook();
    void* (*Sys_LoadDll)(const char *name, char *fqpath, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...));
    *(int *)&Sys_LoadDll = hook_sys_loaddll->from;
    void* ret = Sys_LoadDll(name, fqpath, entryPoint, systemcalls);
    hook_sys_loaddll->hook();

    // Unprotect the game lib
    char libPath[512];
    cvar_t* fs_game = Cvar_FindVar("fs_game");
    if(*fs_game->string)
        sprintf(libPath, "%s/game.mp.i386.so", fs_game->string);
    else
        sprintf(libPath, "main/game.mp.i386.so");
    char buf[512];
    char flags[4];
    void *low, *high;
    FILE *fp;
    fp = fopen("/proc/self/maps", "r");
    if(!fp)
        return 0;
    while(fgets(buf, sizeof(buf), fp))
    {
        if(!strstr(buf, libPath))
            continue;
        if(sscanf (buf, "%p-%p %4c", &low, &high, flags) != 3)
            continue;
        mprotect((void *)low, (int)high-(int)low, PROT_READ | PROT_WRITE | PROT_EXEC);
    }
    fclose(fp);
    
    g_entities = (gentity_t*)dlsym(ret, "g_entities");
    g_clients = (gclient_t*)dlsym(ret, "g_clients");
    level = (level_locals_t*)dlsym(ret, "level");
    pm = (pmove_t*)dlsym(ret, "pm");
    scr_const = (stringIndex_t*)dlsym(ret, "scr_const");

    Scr_GetFunctionHandle = (Scr_GetFunctionHandle_t)dlsym(ret, "Scr_GetFunctionHandle");
    Scr_GetNumParam = (Scr_GetNumParam_t)dlsym(ret, "Scr_GetNumParam");
    Cmd_ArgvBuffer = (Cmd_ArgvBuffer_t)dlsym(ret, "trap_Argv");
    ClientCommand = (ClientCommand_t)dlsym(ret, "ClientCommand");
    Com_SkipRestOfLine = (Com_SkipRestOfLine_t)dlsym(ret, "Com_SkipRestOfLine");
    Scr_GetFunction = (Scr_GetFunction_t)dlsym(ret, "Scr_GetFunction");
    Scr_GetMethod = (Scr_GetMethod_t)dlsym(ret, "Scr_GetMethod");
    SV_GameSendServerCommand = (SV_GameSendServerCommand_t)dlsym(ret, "trap_SendServerCommand");
    Scr_ExecThread = (Scr_ExecThread_t)dlsym(ret, "Scr_ExecThread");
    Scr_ExecEntThread = (Scr_ExecEntThread_t)dlsym(ret, "Scr_ExecEntThread");
    Scr_ExecEntThreadNum = (Scr_ExecEntThreadNum_t)dlsym(ret, "Scr_ExecEntThreadNum");
    Scr_FreeThread = (Scr_FreeThread_t)dlsym(ret, "Scr_FreeThread");
    Scr_Error = (Scr_Error_t)dlsym(ret, "Scr_Error");
    Scr_ObjectError = (Scr_ObjectError_t)dlsym(ret, "Scr_ObjectError");
    Scr_GetConstString = (Scr_GetConstString_t)dlsym(ret, "Scr_GetConstString");
    Scr_ParamError = (Scr_ParamError_t)dlsym(ret, "Scr_ParamError");
    G_Say = (G_Say_t)dlsym(ret, "G_Say");
    G_RegisterCvars = (G_RegisterCvars_t)dlsym(ret, "G_RegisterCvars");
    SV_GetConfigstringConst = (SV_GetConfigstringConst_t)dlsym(ret, "trap_GetConfigstringConst");
    SV_GetConfigstring = (SV_GetConfigstring_t)dlsym(ret, "trap_GetConfigstring");
    BG_GetNumWeapons = (BG_GetNumWeapons_t)dlsym(ret, "BG_GetNumWeapons");
    BG_GetInfoForWeapon = (BG_GetInfoForWeapon_t)dlsym(ret, "BG_GetInfoForWeapon");
    BG_GetWeaponIndexForName = (BG_GetWeaponIndexForName_t)dlsym(ret, "BG_GetWeaponIndexForName");
    BG_AnimationIndexForString = (BG_AnimationIndexForString_t)dlsym(ret, "BG_AnimationIndexForString");
    Scr_IsSystemActive = (Scr_IsSystemActive_t)dlsym(ret, "Scr_IsSystemActive");
    Scr_GetInt = (Scr_GetInt_t)dlsym(ret, "Scr_GetInt");
    Scr_GetString = (Scr_GetString_t)dlsym(ret, "Scr_GetString");
    Scr_GetType = (Scr_GetType_t)dlsym(ret, "Scr_GetType");
    Scr_GetEntity = (Scr_GetEntity_t)dlsym(ret, "Scr_GetEntity");
    Scr_AddBool = (Scr_AddBool_t)dlsym(ret, "Scr_AddBool");
    Scr_AddInt = (Scr_AddInt_t)dlsym(ret, "Scr_AddInt");
    Scr_AddFloat = (Scr_AddFloat_t)dlsym(ret, "Scr_AddFloat");
    Scr_AddString = (Scr_AddString_t)dlsym(ret, "Scr_AddString");
    Scr_AddUndefined = (Scr_AddUndefined_t)dlsym(ret, "Scr_AddUndefined");
    Scr_AddVector = (Scr_AddVector_t)dlsym(ret, "Scr_AddVector");
    Scr_MakeArray = (Scr_MakeArray_t)dlsym(ret, "Scr_MakeArray");
    Scr_AddArray = (Scr_AddArray_t)dlsym(ret, "Scr_AddArray");
    Scr_AddObject = (Scr_AddObject_t)dlsym(ret, "Scr_AddObject");
    Scr_LoadScript = (Scr_LoadScript_t)dlsym(ret, "Scr_LoadScript");
    StuckInClient = (StuckInClient_t)dlsym(ret, "StuckInClient");
    Q_strlwr = (Q_strlwr_t)dlsym(ret, "Q_strlwr");
    Q_strupr = (Q_strupr_t)dlsym(ret, "Q_strupr");
    Q_strcat = (Q_strcat_t)dlsym(ret, "Q_strcat");
    Q_strncpyz = (Q_strncpyz_t)dlsym(ret, "Q_strncpyz");
    Q_CleanStr = (Q_CleanStr_t)dlsym(ret, "Q_CleanStr");

    hook_call((int)dlsym(ret, "vmMain") + 0xB0, (int)hook_ClientCommand);
    hook_call((int)dlsym(ret, "ClientEndFrame") + 0x311, (int)hook_StuckInClient);

    hook_jmp((int)dlsym(ret, "G_LocalizedStringIndex"), (int)custom_G_LocalizedStringIndex);

    // Patch codmsgboom
    /* See:
    - https://aluigi.altervista.org/adv/codmsgboom-adv.txt
    - https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/librarymodule.c#L146
    */
    *(int*)((int)dlsym(ret, "G_Say") + 0x50e) = 0x37f;
    *(int*)((int)dlsym(ret, "G_Say") + 0x5ca) = 0x37f;
    // end

    // 1.1 deadchat support
    /* See:
    - https://github.com/xtnded/codextended/blob/855df4fb01d20f19091d18d46980b5fdfa95a712/src/librarymodule.c#L161
    */
    *(byte*)((int)dlsym(ret, "G_Say") + 0x2B3) = 0xeb;
    *(byte*)((int)dlsym(ret, "G_Say") + 0x3B6) = 0xeb;
    hook_call((int)dlsym(ret, "G_Say") + 0x5EA, (int)hook_G_Say);
    hook_call((int)dlsym(ret, "G_Say") + 0x77D, (int)hook_G_Say);
    hook_call((int)dlsym(ret, "G_Say") + 0x791, (int)hook_G_Say);
    // end

    hook_gametype_scripts = new cHook((int)dlsym(ret, "GScr_LoadGameTypeScript"), (int)custom_GScr_LoadGameTypeScript);
    hook_gametype_scripts->hook();
    hook_play_movement = new cHook((int)dlsym(ret, "ClientThink"), (int)custom_SV_ClientThink);
    hook_play_movement->hook();
    hook_clientendframe = new cHook((int)dlsym(ret, "ClientEndFrame"), (int)custom_ClientEndFrame);
    hook_clientendframe->hook();

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

        printf("> [LIBCOD] Compiled for: CoD1 1.1\n");

        printf("> [LIBCOD] Compiled %s %s using GCC %s\n", __DATE__, __TIME__, __VERSION__);

        // Allow to write in executable memory
        mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);

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

        hook_sys_loaddll = new cHook(0x080c5fe4, (int)custom_Sys_LoadDll);
        hook_sys_loaddll->hook();
        hook_com_init = new cHook(0x0806c654, (int)custom_Com_Init);
        hook_com_init->hook();
        hook_sv_spawnserver = new cHook(0x0808a220, (int)custom_SV_SpawnServer);
        hook_sv_spawnserver->hook();
        hook_sv_begindownload_f = new cHook(0x08087a64, (int)custom_SV_BeginDownload_f);
        hook_sv_begindownload_f->hook();
        hook_sv_sendclientgamestate = new cHook(0x08085eec, (int)custom_SV_SendClientGameState);
        hook_sv_sendclientgamestate->hook();
        hook_sv_addoperatorcommands = new cHook(0x08084a3c, (int)custom_SV_AddOperatorCommands);
        hook_sv_addoperatorcommands->hook();

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