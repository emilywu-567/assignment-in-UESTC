#pragma once
#include "common.h"
#include <stdbool.h>
#include <string.h>
extern int player_money;
void print_story_text(const char* text);
void add_player_money(gameContext* context, int amount);
GameState stage_1(gameContext* context,int battle_result);
GameState story_2(gameContext* context,int battle_result);
GameState story_3(gameContext* context,int battle_result);
GameState story_4(gameContext* context,int battle_result);
GameState stage_5(gameContext* context,int battle_result);
GameState show_story_progress(gameContext* context);
void Dr_DaMu(gameContext* context);
void XiaoGang(gameContext* context);
void Rocket(gameContext* context);
void Event_Caterpie(gameContext* context);
