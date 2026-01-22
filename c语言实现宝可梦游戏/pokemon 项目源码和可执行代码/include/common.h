#pragma once
#include <stdbool.h>
#define SHOP_NUM_ITEMS 14      // 商店商品总数
#define MAX_TEAM_SIZE 6        // 队伍最大数量
#define MAX_CAUGHT_POKEMON 50  // 盒子最大数量
typedef enum GameState {
    MENU,
    EXPLORE_MAP,
    SHOP,
    BACKPACK,
    FIGHT,
    EXIT_GAME
} GameState;

typedef enum SpecialOpponent{
    COMMON,
    DAMU,
    DAMU_MAX,
    CATERPIE,
    XIAOGANG,
    ROCKET
} SpecialOpponent;

typedef struct gameContext gameContext;

// 函数声明
void Transform(GameState newState, gameContext *gameContext);
