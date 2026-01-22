//首先需要包含所有其他声明了自定义结构体的头文件，否则存储游戏数据的自定义结构体无法在GameContext中声明，这里之后需要添加内容
//还需要包含其他模块函数的声明
#pragma once

#include"menu.h"
#include"story.h"
#include "mapExploration.h"
#include"common.h"
#include"fight.h"
#include"shop_and_backpack.h"
#include"my_random.h"

struct gameContext
{
    bool initialized;
    GameState (*currentState)(gameContext *gameContext);
    MapExplorationData mapExplorationData;
    BackpackData backpackData;
    BagItem shop_items[ITEM_TYPE_NUM];
    int stage;

    SpecialOpponent opponent;

    Pokemondata *pokemon_database[POKEMON_DATABASE_SIZE];
    int database_initialized;

    bool exitGame;
    bool has_used_item_this_turn;
    bool has_used_skill_this_turn;
};
GameState Exit(gameContext* context);