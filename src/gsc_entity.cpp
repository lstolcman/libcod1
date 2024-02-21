#include "gsc_entity.hpp"

#if COMPILE_ENTITY == 1

void gsc_entity_setalive(scr_entref_t ref)
{
    int id = ref.entnum;
    int isAlive;

    if ( !stackGetParams("i", &isAlive) )
    {
        stackError("gsc_entity_setalive() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    gentity_t *entity = &g_entities[id];

    entity->takedamage = isAlive;
    stackPushBool(qtrue);
}

void gsc_entity_showtoplayer(scr_entref_t ref)
{
    int id = ref.entnum;
    int playerNum;
    
    if ( !stackGetParams("i", &playerNum) )
    {
        stackError("gsc_entity_showtoplayer() argument is undefined or has a wrong type");
        stackPushUndefined();
        return;
    }

    if ( id >= MAX_CLIENTS )
    {
        stackError("gsc_entity_showtoplayer() entity %i is not a player", id);
        stackPushUndefined();
        return;
    }

    if ( playerNum >= MAX_GENTITIES )
    {
        stackError("gsc_entity_showtoplayer() %i is not a valid entity number", playerNum);
        stackPushUndefined();
        return;
    }
    
    gentity_t *entity = &g_entities[id];
    gentity_t *player = NULL;

    if(Scr_GetType(0) != STACK_UNDEFINED)
        player = Scr_GetEntity(0);

    if(player == NULL)
    {
        entity->svFlags &= ~SVF_SINGLECLIENT;
        entity->singleClient = 0;
        return;
    }

    if(!player->client)
    {
        stackError("gsc_entity_showtoplayer() entity %i is not a player", player->s.number);
        return;
    }

    entity->svFlags |= SVF_SINGLECLIENT;
    entity->singleClient = player->s.number;

    stackPushBool(qtrue);
}

#endif