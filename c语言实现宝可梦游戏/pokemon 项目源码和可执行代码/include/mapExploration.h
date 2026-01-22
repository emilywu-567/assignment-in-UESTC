#pragma once
#include "common.h"
#include"my_random.h"
#include <stdbool.h>
#include <string.h>
#define MIDDLE_MAP "../maps/middle_map.txt"
#define UP_MAP "../maps/up_map.txt"
#define DOWN_MAP "../maps/down_map.txt"
#define LEFT_MAP "../maps/left_map.txt"
#define RIGHT_MAP "../maps/right_map.txt"
typedef struct gameContext gameContext;
typedef struct MapExplorationData MapExplorationData;
typedef struct map map;
typedef struct object object;
typedef struct int2 int2;
struct object
{
    int x;
    int y;
    int posIndex;
    wchar_t sprite;
    void (*event)(gameContext *context);
    bool active;
};
struct int2
{
    int x;
    int y;
};
struct map
{
    char filePath[256];
    wchar_t *baseMap;
    wchar_t *renderingMap;
    bool *isBlocked;
    object objects[20];

    int width;
    int height;
    int2 portalPoints[8];
    char **neighborPaths;

    bool empty;
    bool canMetCaterPie;

    int battle_result;
};
struct MapExplorationData
{
    int playerX;//玩家在当前地图上的位置
    int playerY;
    bool trampOnSomething;//玩家是否踩到某个object
    wchar_t playerSprite;//玩家长什么样
    map currentMap;//当前玩家所在的地图是哪一张

    bool exit;//当前状态是否到了要退出的时候
    GameState nextState;//如果退出，下一个状态应该去哪里
};
GameState mapExplorationFunction(gameContext *context);
void LoadMap(gameContext *context);

void LoadMapObjects(gameContext *context);
void AddObject(object* objects,int posIndex,int width,wchar_t sprite,void(*event)(gameContext* context) );
void LocateMapEntries(gameContext *context);
void IdentifyMapNeighbors(gameContext *context);
void IdentifyMapBlocks(gameContext *context);
void CalculateMapSize(int stringLength, gameContext *context);
int GetPosIndex(int x, int y, gameContext *context);
void RenderMap(gameContext *Context);
void GetUserInput(char *input);
void RenderPlayer(gameContext *context);
void RenderObjects(gameContext *context);
void TryUpdatePlayerPosition(char input, gameContext *context);

bool TryEncounterCaterpie(gameContext* context);
bool TryEncounterWildPokemon(gameContext* context);
void TriggerWildPokemonBattle(gameContext* context);

void gotoXY(int x, int y);
bool IsBlocked(int x, int y, gameContext *context);
bool IsValidPosition(int x, int y, gameContext *context);
bool IsOnBoarder(int x, int y, gameContext *context);
int CheckObject(gameContext *context);
void TryRaiseObjectEvent(int i, gameContext *context);
bool CheckIfEnteredDoor(gameContext *context, int *choice);
void ChangeMap(int choice, gameContext *context);
void AdjustPosition(int choice, gameContext *context);
void hide_cursor();
void FreeMap(gameContext *context);
void ExitGame(gameContext *context);
void ProcessUserInput(char input, gameContext *context);

void Event_Chicken(gameContext *context);
void Event_Tree(gameContext* context);
