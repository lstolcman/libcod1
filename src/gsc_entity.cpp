#include "gsc_entity.hpp"

#if COMPILE_ENTITY == 1



#if 0
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

void gsc_entity_setlight(scr_entref_t ref)
{
	int id = ref.entnum;
	gentity_t *ent = &g_entities[id];

	if ( ent->classname == scr_const.script_model )
	{
		int r, g, b, i;

		if ( !stackGetParams("iiii", &r, &g, &b, &i) )
		{
			stackError("gsc_entity_setlight() argument is undefined or has a wrong type");
			stackPushUndefined();
			return;
		}

		if ( r < 0 )
		{
			r = 0;
		}
		else if ( r > 255 )
		{
			r = 255;
		}
		if ( g < 0 )
		{
			g = 0;
		}
		else if ( g > 255 )
		{
			g = 255;
		}
		if ( b < 0 )
		{
			b = 0;
		}
		else if ( b > 255 )
		{
			b = 255;
		}
		if ( i < 0 )
		{
			i = 0;
		}
		else if ( i > 255 )
		{
			i = 255;
		}
		ent->s.constantLight = r | ( g << 8 ) | ( b << 16 ) | ( i << 24 );

		stackPushBool(qtrue);
	}
	else
	{
		stackError("gsc_entity_setlight() entity is not a script_model");
		stackPushUndefined();
	}
}
#endif



#endif