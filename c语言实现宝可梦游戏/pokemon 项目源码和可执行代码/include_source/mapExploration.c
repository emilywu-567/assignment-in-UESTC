#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#include <direct.h>
#include "../include/main.h"
#include "../include/mapExploration.h"
static void clear_input_buffer(void) {
    while (_kbhit()) _getch();
}
GameState mapExplorationFunction(gameContext *context)
{
    system("cls");
    my_srand((uint64_t)time(NULL));
    hide_cursor();

    context->mapExplorationData.trampOnSomething = false;
    bool* exit = &(context->mapExplorationData.exit);
    (*exit) = false;
    LoadMap(context);
    //加载一些bool变量，用来标记地图状态
    if(!(*exit))
    {
    RenderMap(context);
    }
    while(!(*exit))
    {
        char input;
        input = _getch();
        ProcessUserInput(input, context);
        // 渲染地图
        RenderMap(context);
    }
    return context->mapExplorationData.nextState;
}
//处理用户输入
void ProcessUserInput(char input, gameContext *context)
{
    // 记录移动前的位置，用于判断是否移动了
    int oldX = context->mapExplorationData.playerX;
    int oldY = context->mapExplorationData.playerY;
    
    TryUpdatePlayerPosition(input, context);
    
    // 判断玩家是否实际移动了
    bool playerMoved = (oldX != context->mapExplorationData.playerX || 
                        oldY != context->mapExplorationData.playerY);
    
    // 检查是否踩到物体
    int objectIndex = CheckObject(context);
    if (objectIndex != -1 && (input == 'f' || input == 'F'))
    {
        context->mapExplorationData.currentMap.objects[objectIndex].event(context);
    }
    // 检测是否进入某个门
    int direction;
    if (CheckIfEnteredDoor(context, &direction))
    {
        ChangeMap(direction, context);
        RenderMap(context);
    }
    // 检测是否会遇到绿毛虫（特定剧情）
    if (context->mapExplorationData.currentMap.canMetCaterPie)
    {
        if (TryEncounterCaterpie(context))
        {
            Event_Caterpie(context);
        }
    }
    // 检测是否随机遇到野生宝可梦（stage > 0 且玩家移动了）
    if (context->stage > 0 && playerMoved)
    {
        if (TryEncounterWildPokemon(context))
        {
            TriggerWildPokemonBattle(context);
        }
    }
    //检测是否打开背包
    if (input == 'b' || input == 'B')
    {
        context->mapExplorationData.exit = true;
        context->mapExplorationData.nextState = BACKPACK;
    }
    //  检测是否退出
    if (input == 27)
    {
        ExitGame(context);
    }
}
//渲染地图
void RenderMap(gameContext *context)
{
    gotoXY(0, 0);
    RenderObjects(context);
    RenderPlayer(context);
    fputws(context->mapExplorationData.currentMap.renderingMap,stdout);
}
//根据当前地图currentMap目前文件路径加载地图，确定地图各个参数
void LoadMap(gameContext *context)
{
    map* currentMap = &(context->mapExplorationData.currentMap);
    if (currentMap->empty)//游戏退出时要格外注意将empty设为true
    {
        currentMap->baseMap = (wchar_t *)calloc(5000,2);
        currentMap->renderingMap = (wchar_t*)calloc(5000,2);
        currentMap->isBlocked = (bool *)malloc(5000);
        currentMap->neighborPaths = (char**)malloc(4*sizeof(char*));
        if (currentMap->filePath[0] == '\0')
        {
            strcpy(currentMap->filePath,RIGHT_MAP);
        }
        currentMap->empty = false;
        for(int i = 0;i<4;i++)
        {
            currentMap->neighborPaths[i] = (char *)malloc(50 * sizeof(char));
        }
    }
    FILE *file = fopen(currentMap->filePath, "r, ccs=UTF-8");
    if (file == NULL)
    {
        printf("fail loading currentMap: %s\n", currentMap->filePath);
        return;
    }
    
    rewind(file);
    int mapLength = fread(currentMap->baseMap, sizeof(wchar_t), 4999, file);
    currentMap->baseMap[mapLength] = '\0';
    wcscpy(currentMap->renderingMap,currentMap->baseMap);
    fclose(file);
    CalculateMapSize(mapLength,context);
    IdentifyMapBlocks(context);
    LoadMapObjects(context);
    IdentifyMapNeighbors(context);
    LocateMapEntries(context);
        //检查是不是能够遇到CaterPie
    currentMap->canMetCaterPie = (strcmp(context->mapExplorationData.currentMap.filePath,LEFT_MAP)==0)&&(context->stage == 1);
}
//根据当前地图的地图字符分布加载当前地图的object
void LoadMapObjects(gameContext* context)
{
    map* currentMap =&(context->mapExplorationData.currentMap);
    for(int i = 0;i<20;i++)
    {
        currentMap->objects[i].active = false;
    }

    for(int i = 0;currentMap->baseMap[i]!=L'\0';i++)
    {
        if(currentMap->baseMap[i]==L'鸡')
        {
            AddObject(currentMap->objects,i,currentMap->width,L'鸡',Event_Chicken);
        }
        else if(currentMap->baseMap[i]==L'树')
        {
            AddObject(currentMap->objects,i,currentMap->width,L'树',Event_Tree);
        }
        else if(currentMap->baseMap[i]==L'博')
        {
            AddObject(currentMap->objects,i,currentMap->width,L'博',Dr_DaMu);
        }
        else if(currentMap->baseMap[i]==L'刚')
        {
            AddObject(currentMap->objects,i,currentMap->width,L'刚',XiaoGang);
        }
        else if(currentMap->baseMap[i]==L'敌')
        {
            AddObject(currentMap->objects,i,currentMap->width,L'敌',Rocket);
        }
    }
}
void AddObject(object* arr,int posIndex,int width,wchar_t sprite,void(*event)(gameContext* context))
{
    int i = 0;
    while(arr[i].active)
    {
        i++;
    }
    arr[i].posIndex = posIndex;
    arr[i].x = posIndex%(width+1);
    arr[i].y = posIndex/(width+1);
    arr[i].sprite = sprite;
    arr[i].event = event;
    arr[i].active = true;
    
}
//根据当前地图currentMap的文件路径，辨认其是哪个地图，并据此指定其邻居地图们的文件路径，用于ChangeMap
void IdentifyMapNeighbors(gameContext* context)
{
    map* currentMap = &(context->mapExplorationData.currentMap);
    if(strcmp(currentMap->filePath,MIDDLE_MAP)==0)
    {
        strcpy(currentMap->neighborPaths[0],LEFT_MAP);
        strcpy(currentMap->neighborPaths[1],UP_MAP);
        strcpy(currentMap->neighborPaths[2],RIGHT_MAP);
        strcpy(currentMap->neighborPaths[3],DOWN_MAP);
    }
    else if(strcmp(currentMap->filePath,UP_MAP)==0)
    {
        strcpy(currentMap->neighborPaths[3],MIDDLE_MAP);
    }
    else if(strcmp(currentMap->filePath,LEFT_MAP)==0)
    {
        strcpy(currentMap->neighborPaths[2],MIDDLE_MAP);
    }
    else if(strcmp(currentMap->filePath,RIGHT_MAP)==0)
    {
        strcpy(currentMap->neighborPaths[0],MIDDLE_MAP);
    }
    else if(strcmp(currentMap->filePath,DOWN_MAP)==0)
    {
        strcpy(currentMap->neighborPaths[1],MIDDLE_MAP);
    }
}
//根据地图四个方向上围墙的空缺，识别并记录当前地图的四个方向上的门户坐标
void LocateMapEntries(gameContext* context)
{
    map*currentMap = &(context->mapExplorationData.currentMap);
    for(int i = 0;i<8;i++)
    {
        currentMap->portalPoints[i].x = -1;
        currentMap->portalPoints[i].y = -1;
    }

    wchar_t temp = L'＃';
    wchar_t current;
    
    for(int i = 0;i<currentMap->height-1;i++)
    {
        current = currentMap->baseMap[GetPosIndex(0,i,context)];
        if(temp == L'＃'&&current == L'　')
        {
            currentMap->portalPoints[1].x = 0;
            currentMap->portalPoints[1].y = i;
        }
        if(temp == L'　'&&current == L'＃')
        {
            currentMap->portalPoints[0].x = 0;
            currentMap->portalPoints[0].y = i-1;
        }
        temp = current;
    }
    temp = L'＃';
    current = L'　';
    for(int i = 0;i<currentMap->width-1;i++)
    {
        current = currentMap->baseMap[GetPosIndex(i,0,context)];
        if(temp == L'＃'&&current == L'　')
        {
            currentMap->portalPoints[2].x = i;
            currentMap->portalPoints[2].y = 0;
        }
        if(temp == L'　'&&current == L'＃')
        {
            currentMap->portalPoints[3].x = i-1;
            currentMap->portalPoints[3].y = 0;
        }
        temp = current;
    }
    temp = L'＃';
    current = L'　';
    for(int i = 0;i<currentMap->height-1;i++)
    {
        current = currentMap->baseMap[GetPosIndex(currentMap->width-1,i,context)];
        if(temp == L'＃'&&current == L'　')
        {
            currentMap->portalPoints[4].x = currentMap->width-1;
            currentMap->portalPoints[4].y = i;
        }
        if(temp == L'　'&&current == L'＃')
        {
            currentMap->portalPoints[5].x = currentMap->width-1;
            currentMap->portalPoints[5].y = i-1;
        }
        temp = current;
    }
    temp = L'＃';
    current = L'　';
    for(int i = 0;i<currentMap->width-1;i++)
    {
        current = currentMap->baseMap[GetPosIndex(i,currentMap->height-1,context)];
        if(temp == L'＃'&&current == L'　')
        {
            currentMap->portalPoints[7].x = i;
            currentMap->portalPoints[7].y = currentMap->height-1;
        }
        if(temp == L'　'&&current == L'＃')
        {
            currentMap->portalPoints[6].x = i-1;
            currentMap->portalPoints[6].y = currentMap->height-1;
        }
        temp = current;
    }

}
//根据地图障碍物字符，确认每个坐标是否被阻挡，并将结果储存到bool数组isBlocked中
void IdentifyMapBlocks(gameContext* context)
{
    for(int i = 0;context->mapExplorationData.currentMap.baseMap[i]!=L'\0';i++)
    {
        context->mapExplorationData.currentMap.isBlocked[i] = context->mapExplorationData.currentMap.baseMap[i]==L'＃'?true:false;
    }
}
//根据当前地图的字符数和字符分布计算当前地图的长宽并保存作为currentMap的参数
void CalculateMapSize(int stringLength,gameContext* context)
{
    context->mapExplorationData.currentMap.width = 0;
    context->mapExplorationData.currentMap.height = 0;
    for(int i = 0;context->mapExplorationData.currentMap.baseMap[i]!=L'\n';i++)
    {
        context->mapExplorationData.currentMap.width++;
    }
    context->mapExplorationData.currentMap.height = (stringLength/(context->mapExplorationData.currentMap.width+1))+1;

}

int GetPosIndex(int x,int y,gameContext* context)
{
    if(x<0||y<0||x>context->mapExplorationData.currentMap.width-1||y>context->mapExplorationData.currentMap.height-1)return 0;
    return y*(context->mapExplorationData.currentMap.width+1)+x;
}
void GetUserInput(char *target)
{
    *target = _getch();
}
//在renderingMap上渲染玩家和物品的函数
void RenderPlayer(gameContext *context)
{
    context->mapExplorationData.currentMap.renderingMap[GetPosIndex(context->mapExplorationData.playerX,context->mapExplorationData.playerY,context)] = context->mapExplorationData.trampOnSomething?L'Ｆ':context->mapExplorationData.playerSprite;
    
}
void RenderObjects(gameContext *context)
{
    map* currentMap = &(context->mapExplorationData.currentMap);
    for(int i = 0;currentMap->objects[i].active;i++)
    {
        currentMap->renderingMap[GetPosIndex(currentMap->objects[i].x,currentMap->objects[i].y,context)] = currentMap->objects[i].sprite;
    }
}
//根据用户输入更新玩家坐标的函数
void TryUpdatePlayerPosition(char input, gameContext *context)
{
    context->mapExplorationData.currentMap.renderingMap[GetPosIndex(context->mapExplorationData.playerX,context->mapExplorationData.playerY,context)] = 
    context->mapExplorationData.currentMap.baseMap[GetPosIndex(context->mapExplorationData.playerX,context->mapExplorationData.playerY,context)];

    if (input == 'a' || input == 'A')
    {
        if (IsBlocked(context->mapExplorationData.playerX - 1, context->mapExplorationData.playerY, context))
            return;
        context->mapExplorationData.playerX -= 1;
    }
    if (input == 'd' || input == 'D')
    {
        if (IsBlocked(context->mapExplorationData.playerX + 1, context->mapExplorationData.playerY, context))
            return;
        context->mapExplorationData.playerX += 1;
    }
    if (input == 'w' || input == 'W')
    {
        if (IsBlocked(context->mapExplorationData.playerX, context->mapExplorationData.playerY - 1, context))
            return;
        context->mapExplorationData.playerY -= 1;
    }
    if (input == 's' || input == 'S')
    {
        if (IsBlocked(context->mapExplorationData.playerX, context->mapExplorationData.playerY + 1, context))
            return;
        context->mapExplorationData.playerY += 1;
    }
}

//检查玩家是否穿过了地图边界的某个门（判断站到了地图边界上即可，此举说明站到了边界上障碍物空缺的地方），返回bool，以及一个choice用于表示穿过的门在地图的上下左右哪个方位
bool CheckIfEnteredDoor(gameContext* context,int* choice)
{
    map* currentMap = &(context->mapExplorationData.currentMap);
    int playerX = context->mapExplorationData.playerX;
    int playerY = context->mapExplorationData.playerY;
    if(playerX == 0)
    {
        *choice = 0;
        return true;
    }
    else if(playerX == currentMap->width-1)
    {
        *choice = 2;
        return true;
    }
    else if(playerY == 0)
    {
        *choice = 1;
        return true;
    }
    else if(playerY == currentMap->height-1)
    {
        *choice = 3;
        return true;
    }
    return false;
}

//地图转换函数，根据CheckIfEnteredDoor函数的返回值choice转换地图，转换当前地图currentMap所读取的文件路径，并且重新加载一次地图，最后根据choice改变玩家坐标使之出现在地图相应的门口。
void ChangeMap(int choice,gameContext* context)
{
    if(choice<0||choice>3)return;
    strcpy(context->mapExplorationData.currentMap.filePath,context->mapExplorationData.currentMap.neighborPaths[choice]);
    LoadMap(context);
    int frontPortal = 2*((choice+2)%4);
    int2 point1 = context->mapExplorationData.currentMap.portalPoints[frontPortal];
    int2 point2 = context->mapExplorationData.currentMap.portalPoints[frontPortal+1];
    context->mapExplorationData.playerX = (point1.x+point2.x)/2;
    context->mapExplorationData.playerY = (point1.y+point2.y)/2;
    AdjustPosition(choice,context);
    system("cls");
    
    
}

//用来调整玩家穿越门后出现在新地图时的位置，直接放到新地图门户坐标上会导致刚穿越地图就穿回去，往复循环卡死
void AdjustPosition(int choice,gameContext* context)
{
    map* currentMap = &(context->mapExplorationData.currentMap);
    MapExplorationData* data = &(context->mapExplorationData);
    int* playerX = &(data->playerX);
    int* playerY = &(data->playerY);
    switch(choice)
    {
        case 0:
        *playerX-=1;
        break;
        case 1:
        *playerY-=1;
        break;
        case 2:
        *playerX+=1;
        break;
        case 3:
        *playerY+=1;
        break;
    }

}

//用来判断当前玩家位置是否与当前地图的某个物品重合，如果是，返回该物体在objects数组里的编号
int CheckObject(gameContext *context)
{
    for(int i = 0;context->mapExplorationData.currentMap.objects[i].active;i++)
    {
        if(context->mapExplorationData.playerX == context->mapExplorationData.currentMap.objects[i].x
        &&context->mapExplorationData.playerY == context->mapExplorationData.currentMap.objects[i].y)
        {
        context->mapExplorationData.trampOnSomething = true;
        return i;
        }
    }
    context->mapExplorationData.trampOnSomething = false;
    return -1;
}
//尝试根据当前传入的数组标号和用户输入决定是否触发物品事件

//用来判断指定坐标是否超出当前地图边界
bool IsValidPosition(int x,int y,gameContext* context)
{
    map* currentMap = &(context->mapExplorationData.currentMap);
    if(x<0||y<0||x>currentMap->width-1||y>currentMap->height-1)
    {
        return false;
    }
    return true;
}
//用来判断指定坐标是否在边界上
bool IsOnBoarder(int x,int y,gameContext* context)
{
    map* currentMap = &(context->mapExplorationData.currentMap);
    if(x == 0||y == 0||x == currentMap->width-1||y == currentMap->height-1)return true;
    return false;
}
//用来判断指定坐标是否被阻挡
bool IsBlocked(int x, int y, gameContext *context)
{
    if (context->mapExplorationData.currentMap.isBlocked[y*(context->mapExplorationData.currentMap.width+1)+x])
    {
        return true;
    }
    return false;
}
//移动光标到某个位置
void gotoXY(int x, int y)
{
    COORD pos = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
// 隐藏光标w
void hide_cursor()
{
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = FALSE;
    cursor.dwSize = 1;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}
void ExitGame(gameContext* context)
{
    printf("\n确定要退出游戏吗？\n按1确定，按2取消");
    char input;
    while(1)
    {
        input = _getch();
        if(input == '1')
        {
            context->mapExplorationData.exit = true;
            context->mapExplorationData.nextState = MENU;
            FreeMap(context);
            break;
        }
        else if(input == '2')
        {
            system("cls");
            RenderMap(context);
            break;
        }
    }

}
void FreeMap(gameContext* context)
{
    free(context->mapExplorationData.currentMap.baseMap);
    free(context->mapExplorationData.currentMap.renderingMap);
    free(context->mapExplorationData.currentMap.isBlocked);
    free(context->mapExplorationData.currentMap.neighborPaths);
}

void Event_Chicken(gameContext* context)
{
    int64_t n = my_rand_range(1,100);
    if(n >=95)
    {
        printf("\n鸡：你干嘛 嗨嗨哎哟。");
    }
    else if(n>=90)
    {
        printf("\n鸡：全民制作人们大家好。");
    }
    else if(n>=85)
    {
        printf("\n鸡：食不食油饼");
    }
    else
    {
            printf("\n鸡：咯咯！");
    }
    _getch();
    system("cls");
    return;
}
void Event_Tree(gameContext* context)
{
    printf("\n树：沙沙...");
    _getch();
    system("cls");
    return;
}
bool TryEncounterCaterpie(gameContext* context)
{
    int64_t n = my_rand_range(1,100);
    if(n>90)return true;
    return false; 
}

// 尝试随机遇到野生宝可梦
bool TryEncounterWildPokemon(gameContext* context)
{
    int64_t n = my_rand_range(1, 100);
    if (n <= 2) return true;  // 2%的概率遇敌
    return false;
}

// 触发野生宝可梦战斗
void TriggerWildPokemonBattle(gameContext* context)
{
    // 草丛震动效果
    printf("\n");
    printf("    沙沙...\n");
    Sleep(300);
    printf("    沙沙沙...\n");
    Sleep(500);
    
    system("cls");
    printf("\n\n");
    printf("        ✦  ✦  ✦\n");
    Sleep(200);
    printf("\n");
    printf("   ═══════════════════════════════\n");
    printf("   ║                             ║\n");
    printf("   ║    ！野生宝可梦出现了！     ║\n");
    printf("   ║                             ║\n");
    printf("   ═══════════════════════════════\n");
    printf("\n");
    printf("        ✦  ✦  ✦\n");
    printf("\n\n");
    
    Sleep(800);
    printf("   按任意键进入战斗...\n");
    
    while (_kbhit()) _getch();
    _getch();
    
    // 设置为普通敌人（可以被捕捉）
    context->opponent = COMMON;
    context->mapExplorationData.exit = true;
    context->mapExplorationData.nextState = FIGHT;
}