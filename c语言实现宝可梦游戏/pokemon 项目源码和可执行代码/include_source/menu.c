#include"../include/main.h"
#include<stdio.h>
#include <conio.h>
#include <string.h>

// 开始新游戏：重置所有游戏数据
static void start_new_game(gameContext* context)
{
    system("cls");
    printf("═══════════════════════════════════════════\n");
    printf("            ⚠️ 开始新游戏\n");
    printf("═══════════════════════════════════════════\n\n");
    printf("这将会清除所有现有的存档数据！\n\n");
    printf("确定要开始新游戏吗？(y/n): ");
    fflush(stdout);
    
    char confirm = _getch();
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("\n已取消。\n");
        Sleep(500);
        return;
    }
    
    printf("\n\n正在初始化新游戏...\n");
    
    // 重置游戏进度
    context->stage = 0;
    context->initialized = true;
    context->opponent = COMMON;
    context->exitGame = false;
    context->has_used_item_this_turn = false;
    context->has_used_skill_this_turn = false;
    
    // 重置地图数据
    context->mapExplorationData.playerX = 10;
    context->mapExplorationData.playerY = 10;
    context->mapExplorationData.playerSprite = L'你';
    context->mapExplorationData.trampOnSomething = false;
    context->mapExplorationData.exit = false;
    context->mapExplorationData.currentMap.empty = true;
    strcpy(context->mapExplorationData.currentMap.filePath, "../maps/right_map.txt");
    
    // 重新初始化宝可梦数据库
    initialize_pokemons(context);
    
    // 重新初始化玩家背包（清空宝可梦和道具）
    initialize_player_bag(context);
    
    printf("新游戏初始化完成！\n");
    printf("\n按任意键开始冒险...\n");
    _getch();
}

GameState main_menu_state(gameContext* context)
{
    system("cls");
    printf("             === 主菜单 ===\n");
    printf("               开始新游戏\n");
    printf("                继续游戏\n");
    printf("                查看说明\n");
    printf("                退出游戏\n\n");
    printf("按w和s切换选项，按enter确认\n");
    int row = 1;
    while(1)
    {
        char input = _getch();
        show_arrow(2, row);
        if (input == 'w' && row > 1) 
        {
            int pre_row = row;
            row--;
            show_arrow(pre_row, row);
        } 
        else if (input == 's' && row < 4) 
        {
            int pre_row = row;
            row++;
            show_arrow(pre_row, row);
        } 
        else if (input == '\r') 
        {
            switch(row)
            {
                case 1:
                    // 开始新游戏
                    start_new_game(context);
                    return EXPLORE_MAP;
                case 2:
                    // 继续游戏
                    return EXPLORE_MAP;
                case 3:
                    system("cls");
                    printf("说明：这是一个宝可梦游戏，玩家可以探索地图、与宝可梦战斗、购买和使用道具等。\n");
                    printf("按WASD移动，按F交互，按B打开背包\n");
                    printf("温馨提示：请确保输入法设置为英文，以避免无法即时识别输入。\n");
                    printf("按任意键返回...\n");
                    _getch();
                    return MENU;
                case 4:
                    context->exitGame = true;
                    return EXIT_GAME;
            }
        }
    }

}