#include "gsc_bots.hpp"

extern customPlayerState_t customPlayerState[MAX_CLIENTS];

void gsc_bots_setbotstance(scr_entref_t ref)
{
    int id = ref.entnum;
    char *stance;

    if ( !stackGetParams("s", &stance) )
    {
        stackError("gsc_bots_setbotstance() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if ( id >= MAX_CLIENTS )
    {
        stackError("gsc_bots_setbotstance() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    client_t *client = &svs.clients[id];

    if ( client->netchan.remoteAddress.type != NA_BOT )
    {
        stackError("gsc_bots_setbotstance() player %i is not a bot", id);
        stackPushUndefined();
        return;
    }

    if ( strcmp(stance, "stand") == 0 )
        customPlayerState[id].botButtons &= ~(KEY_MASK_CROUCH | KEY_MASK_PRONE | KEY_MASK_JUMP);
    else if ( strcmp(stance, "crouch") == 0 )
        customPlayerState[id].botButtons |= KEY_MASK_CROUCH;
    else if ( strcmp(stance, "prone") == 0 )
        customPlayerState[id].botButtons |= KEY_MASK_PRONE;
    else if ( strcmp(stance, "jump") == 0 )
        customPlayerState[id].botButtons |= KEY_MASK_JUMP;
    else
    {
        stackError("gsc_bots_setbotstance() invalid argument '%s'. Valid arguments are: 'stand' 'crouch' 'prone' 'jump'", stance);
        stackPushUndefined();
        return;
    }

    stackPushBool(qtrue);
}