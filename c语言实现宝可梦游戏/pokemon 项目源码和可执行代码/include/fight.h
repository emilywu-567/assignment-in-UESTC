#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <stdbool.h>
#include <windows.h>
#include "common.h"
typedef struct gameContext gameContext;
void setUTF8();
// 战斗结果枚举
typedef enum {
    BATTLE_ONGOING,
    BATTLE_WIN,
    BATTLE_LOSE,
    BATTLE_RUN,
    BATTLE_CAPTURE
} BattleResult;

//状态//
typedef enum {
    dizzy,    //晕眩//
    frozen,   //冰冻//
    burn,     //灼烧//
    poison,   //中毒//
    paralyze, //疲软//
    none,     //无//
} State;

//属性//
typedef enum Pokemontype{
    normal,
    fire,
    grass,
    water,
    electricity,
    wrestle,
    flight,
    soil,
} Pokemontype;

//道具效果类型
typedef enum EffectType{
    EFFECT_NONE,
    EFFECT_HP_HEAL,
    EFFECT_XP_GAIN,
    EFFECT_PP_RESTORE,
    EFFECT_SHIELD,
    EFFECT_ATK_UP,
    EFFECT_EVADE_UP,
    EFFECT_SPEED_UP,
    EFFECT_HP_UP,
    EFFECT_CRIT_UP,
    EFFECT_DEFENCE_UP,
    EFFECT_CAPTURE
} EffectType;

//道具类型
typedef enum ItemType{
    ITEM_CONSUMABLE,
    ITEM_PERMANENT
} ItemType;

//技能效果//
typedef struct Skill{
    char skill_name[40];
    int damage;
    int max_pp;
    int current_pp;
    int accuracy;
    int crit;
    State effect;
} Skill;

//宝可梦数据//
typedef struct Pokemondata{
    bool empty;
    int number;
    char name[40];
    Pokemontype type;
    int level;
    int current_xp;
    int max_xp;
    int current_hp;
    int max_hp;
    int dodge;
    int defence;
    State current_state;
    int speed;
    int carried_skill[4];
    Skill all_skill[6];
    int skill_count;
} Pokemondata;

//道具结构体
typedef struct BagItem{
    char name[50];
    int amount;
    int buy_price;
    int sell_price;
    ItemType item_type;
    EffectType effect_type;
    int effect_value;
} BagItem;

// 玩家背包已整合到 gameContext->backpackData (见 shop_and_backpack.h)
// helper functions are implemented as static in source files and are not exposed here
bool use_item_from_bag(gameContext* context, int item_index, Pokemondata* target);
bool show_battle_bag_menu(gameContext* context, Pokemondata* player);
bool use_item_from_bag_in_battle(gameContext* context, int item_index, Pokemondata* target);

double get_type_multiplier(Pokemontype attacker_type, Pokemontype defender_type);
bool hit_check(int dodge, int accuracy);
bool crit_check(int crit);
int calc_damage(Pokemondata* attacker, Pokemondata* defender, int chosen_skill);
void apply_status_effect(Pokemondata* target);
void recover_paralyze(Pokemondata* target);
void recover_frozen(Pokemondata* pokemon);
Pokemondata* get_first_attacker(Pokemondata* p1, Pokemondata* p2);
void gain_xp(Pokemondata* player, int xp);
void display_pokemon_status(Pokemondata* pokemon, bool is_player);
void display_skills(Pokemondata* pokemon);
void show_turn_status(gameContext* context);
bool try_to_run(Pokemondata* player, Pokemondata* enemy);
BattleResult check_battle_end(gameContext* context, Pokemondata* player, Pokemondata* enemy);
BattleResult player_turn(gameContext* context, Pokemondata* player, Pokemondata* enemy);
void enemy_turn(Pokemondata* player, Pokemondata* enemy);
void apply_turn_end_effects(Pokemondata* player, Pokemondata* enemy);
BattleResult battle_loop(gameContext* context, Pokemondata player, Pokemondata enemy);
// 封装的战斗初始化与结果处理函数
void prepare_battle(gameContext* context, Pokemondata* player, Pokemondata* enemy);
GameState handle_battle_result(gameContext* context, BattleResult result);
void main_menu();
GameState fight(gameContext* context);





