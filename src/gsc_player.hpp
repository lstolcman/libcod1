#ifndef _GSC_PLAYER_HPP_
#define _GSC_PLAYER_HPP_

#include "gsc.hpp"


void gsc_player_setvelocity(scr_entref_t ref);
void gsc_player_getvelocity(scr_entref_t ref);
void gsc_player_button_ads(scr_entref_t ref);
void gsc_player_button_leanleft(scr_entref_t ref);
void gsc_player_button_leanright(scr_entref_t ref);
void gsc_player_button_reload(scr_entref_t ref);
void gsc_player_gettagangles(scr_entref_t ref);
void gsc_player_getstance(scr_entref_t ref);
void gsc_player_getip(scr_entref_t ref);
void gsc_player_getping(scr_entref_t ref);

void gsc_player_dropclient(scr_entref_t ref);


#endif