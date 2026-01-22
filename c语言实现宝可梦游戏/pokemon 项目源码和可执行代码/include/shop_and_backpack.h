// shop.h
#pragma once
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fight.h"
#define ITEM_TYPE_NUM 14
#define SHOP_MAX_NAME_LEN 50
#define SKILL_MAX_COUNT 6
#define CARRIED_SKILL_COUNT 4
#define POKEMON_DATABASE_SIZE 20
typedef struct BackpackData
{
    int player_money;
    int pokemon_count;
    int chosen_pokemon_index;
    int shop_inventory[14];
    BagItem items[14];
    Pokemondata pokemons[18];
}BackpackData;
//杂项
bool choose_true_or_false();
void ChangeBackPackItemAmount(ItemType index,int chaneAmount,gameContext* context);
// 商店状态函数（状态机入口）
GameState shop_state(gameContext* context);
GameState backpack_state(gameContext* context);
int show_and_choose_item(BagItem* items,gameContext* context, void(* show_info_callback)(BagItem* item,int index));
//打印物品信息的回调函数
void show_item_when_buying(BagItem* item,int index);
void show_item_when_selling(BagItem* item,int index);
void show_item_when_checking(BagItem* item,int index);
void show_item_amount(BagItem* item,int index);
void show_item_buy_price(BagItem* item,int index);
void show_item_sell_price(BagItem* item,int index);
void print_nothing(BagItem* item,int index);
void endline(BagItem* item,int index);
//商店操作相关
void choose_and_buy_item(gameContext* context);
void choose_and_sell_item(gameContext* context);
// 查看物品说明
void choose_and_check_item(gameContext* context);
//显示箭头表示选中状态
void show_arrow(int pre,int row);
//宝可梦相关
void add_player_pokemon(gameContext* context,Pokemondata* pokemon);
void choose_and_interact_with_pokemons(gameContext* context);
void show_all_pokemon_skills(Pokemondata *data);
//选择宝可梦
int show_and_choose_pokemon(gameContext* context);
//宝可梦升级
void try_update_pokemon(Pokemondata* pokemon);
void calculate_pokemon_attribute_according_to_level(Pokemondata* pokemon);
double get_the_corresponding_factor_according_to_level(int level);
//释放宝可梦数据内存
void free_pokemondata(Pokemondata* pokemon);

//初始化技能系统
void initialize_skills(void);

//初始化宝可梦数据库
void initialize_pokemon_database(void);

// 从战斗模块迁移过来的函数声明（load 与初始化相关）
Skill load_skill(const char* name, int damage, int pp, int accuracy, int crit, State effect);
Pokemondata load_Pokemondata(int number, char* name, Pokemontype type, int level, int xp, int hp, int dodge,  int speed, Skill skill[6]);
// 初始化玩家背包与宝可梦数据库
void initialize_player_bag(gameContext* context);
void initialize_pokemons(gameContext* context);

//宝可梦模块函数
void choose_starter_pokemon(gameContext* gameContext);
void init_pokemon_game(void);
void show_all_pokemon(gameContext* gameContext);
void select_active_pokemon(gameContext* gameContext);
void select_skills_for_pokemon(gameContext* gameContext);

void show_main_menu(void);