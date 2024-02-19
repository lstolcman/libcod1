#include "gsc_player.hpp"



#if COMPILE_PLAYER == 1










void gsc_player_setvelocity(scr_entref_t ref)
{
	int id = ref.entnum;
	vec3_t velocity;

	if ( !stackGetParams("v", &velocity) )
	{
		stackError("gsc_player_setvelocity() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_player_setvelocity() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	playerState_t *ps = SV_GameClientNum(id);
	VectorCopy(velocity, ps->velocity);

	stackPushBool(qtrue);
}








void gsc_player_getvelocity(scr_entref_t ref)
{
	int id = ref.entnum;

	if ( id >= MAX_CLIENTS )
	{
		stackError("gsc_player_getvelocity() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	playerState_t *ps = SV_GameClientNum(id);

	stackPushVector(ps->velocity);
}










void gsc_player_button_leanleft(scr_entref_t ref)
{
    int id = ref.entnum;

    if ( id >= MAX_CLIENTS )
    {
        stackError("gsc_player_button_leanleft() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    
    stackPushBool(client->lastUsercmd.wbuttons & KEY_MASK_LEANLEFT ? qtrue : qfalse);
}

void gsc_player_button_leanright(scr_entref_t ref)
{
    int id = ref.entnum;

    if ( id >= MAX_CLIENTS )
    {
        stackError("gsc_player_button_leanright() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    stackPushBool(client->lastUsercmd.wbuttons & KEY_MASK_LEANRIGHT ? qtrue : qfalse);
}




void gsc_player_button_reload(scr_entref_t ref)
{
    int id = ref.entnum;

    if ( id >= MAX_CLIENTS )
    {
        stackError("gsc_player_button_reload() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    stackPushBool(client->lastUsercmd.wbuttons & KEY_MASK_RELOAD ? qtrue : qfalse);
}







void gsc_player_gettagangles(scr_entref_t ref)
{
    int id = ref.entnum;

    if ( id >= MAX_CLIENTS )
    {
        stackError("gsc_player_gettagangles() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }
    
    playerState_t *ps = SV_GameClientNum(id);

    stackPushVector(ps->viewangles);
}








void gsc_player_getstance(scr_entref_t ref)
{
    int id = ref.entnum;
    
    if ( id >= MAX_CLIENTS )
    {
        stackError("gsc_player_getstance() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }
    
    playerState_t *ps = SV_GameClientNum(id);
    if ( ps->eFlags & EF_CROUCHING )
        stackPushString("crouch");
    else if ( ps->eFlags & EF_PRONE )
        stackPushString("prone");
    else
        stackPushString("stand");
}







void gsc_player_getip(scr_entref_t ref)
{
    int id = ref.entnum;

    if ( id >= MAX_CLIENTS )
    {
        stackError("gsc_player_getip() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];
    char tmp[16];
    snprintf(tmp, sizeof(tmp), "%d.%d.%d.%d", client->netchan.remoteAddress.ip[0], client->netchan.remoteAddress.ip[1], client->netchan.remoteAddress.ip[2], client->netchan.remoteAddress.ip[3]);

    stackPushString(tmp);
}








#endif