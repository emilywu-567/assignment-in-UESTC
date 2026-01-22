#include "../include/main.h"
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#include <direct.h>
#include <locale.h>
#include <stdio.h>
#include <stdint.h>
static void InitializeBackPack(gameContext* context);
static void InitializeMapExplorationData(gameContext* context);
static void InitializeForFirstGame(gameContext* context);
void Save(gameContext* context,const char* filePath);
static void Load(gameContext* context,const char* filePath);
static void init_context_defaults(gameContext* context);
static void init_shop_items(BagItem* shop_items);
static void set_workdir_to_exe_dir(void);

typedef struct SaveFileV1
{
    uint32_t magic; 
    uint32_t version; 

    bool initialized;
    int stage;
    SpecialOpponent opponent;

    // Map exploration
    int playerX;
    int playerY;
    bool trampOnSomething;
    wchar_t playerSprite;
    bool map_exit;
    GameState nextState;
    char currentMapPath[256];

    BackpackData backpackData;
    BagItem shop_items[ITEM_TYPE_NUM];

    bool exitGame;
    bool has_used_item_this_turn;
    bool has_used_skill_this_turn;
} SaveFileV1;

static void FreePokemonDatabase(gameContext* context)
{
    if (context == NULL) return;
    for (int i = 0; i < POKEMON_DATABASE_SIZE; i++)
    {
        if (context->pokemon_database[i] != NULL)
        {
            free(context->pokemon_database[i]);
            context->pokemon_database[i] = NULL;
        }
    }
    context->database_initialized = 0;
}
int main()
{
    set_workdir_to_exe_dir();
    system("chcp 65001");
    setlocale(LC_ALL, ".UTF8");
    char buffer[1024];

    if (getcwd(buffer, sizeof(buffer)) != NULL)
    {
        printf("当前工作目录: %s\n", buffer);
    }
    else
    {
        perror("获取目录失败");
        return 1;
    }
    const char* save_path = "../saving/saving.dat";

    gameContext context;
    init_context_defaults(&context);

    // 先尝试读档；若存档不存在，就创建空存档并将 initialized 设为为 false
    Load(&context, save_path);

    // 初始化宝可梦数据库与背包，只在首次进行
    if (!context.initialized)
    {
        initialize_pokemons(&context);
        initialize_player_bag(&context);
        context.initialized = true;
    }

    while (!context.exitGame)
    {
        Transform(context.currentState(&context), &context);
    }

    // 退出循环时自动存档
    Save(&context, save_path);

    return 0;
}
void Transform(GameState newState, gameContext *context)
{
    switch (newState)
    {
    case MENU:
        context->currentState = main_menu_state;
        break;
    case EXPLORE_MAP:
        context->currentState = mapExplorationFunction;
        break;
    case FIGHT:
        context->currentState = fight;
        break;
    case BACKPACK:
        context->currentState = backpack_state;
        break;
    case SHOP:
        context->currentState = shop_state;
        break;
    case EXIT_GAME:
        context->exitGame = true;
        break;
    default:
        context->currentState = mapExplorationFunction;
        break;
    }
}
void Save(gameContext* context,const char* filePath)
{
    FILE* file = fopen(filePath,"wb");
    if(file == NULL)
    {
        printf("无法保存，请检查路径和文件名是否合法\n");
        return;
    }

    SaveFileV1 save = {0};
    save.magic = 0x56534B50u;
    save.version = 1u;

    save.initialized = context->initialized;
    save.stage = context->stage;
    save.opponent = context->opponent;

    save.playerX = context->mapExplorationData.playerX;
    save.playerY = context->mapExplorationData.playerY;
    save.trampOnSomething = context->mapExplorationData.trampOnSomething;
    save.playerSprite = context->mapExplorationData.playerSprite;
    save.map_exit = context->mapExplorationData.exit;
    save.nextState = context->mapExplorationData.nextState;
    // 只保存地图路径
    strcpy(save.currentMapPath, context->mapExplorationData.currentMap.filePath);

    save.backpackData = context->backpackData;
    for (int i = 0; i < ITEM_TYPE_NUM; i++)
    {
        save.shop_items[i] = context->shop_items[i];
    }

    save.exitGame = context->exitGame;
    save.has_used_item_this_turn = context->has_used_item_this_turn;
    save.has_used_skill_this_turn = context->has_used_skill_this_turn;

    // 调试输出：保存时的宝可梦数量
    printf("保存中... 宝可梦数量=%d, stage=%d\n", context->backpackData.pokemon_count, context->stage);
    for (int i = 0; i < 18; i++)
    {
        if (!context->backpackData.pokemons[i].empty)
        {
            printf("  宝可梦[%d]: %s, 等级=%d\n", i, context->backpackData.pokemons[i].name, context->backpackData.pokemons[i].level);
        }
    }

    size_t written = fwrite(&save, sizeof(save), 1, file);
    if (written != 1)
    {
        printf("保存失败：写入不完整\n");
    }
    else
    {
        printf("保存成功！写入 %zu 字节\n", sizeof(save));
    }

    fclose(file);
    return;

}
static void Load(gameContext* context,const char* filePath)
{
    FILE* file = fopen(filePath,"rb");
    if(file == NULL)
    {
        FILE* file1 = fopen(filePath,"wb");
        if(file1 == NULL)
        {
            printf("创建新存档失败，请检查文件路径和文件名是否合法");
            return;
        }
        printf("已创建新存档\n");
        fclose(file1);
        context->initialized = false;
        return;
    }
    
    // 检查文件大小
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (fileSize < (long)sizeof(SaveFileV1))
    {
        printf("新游戏开始\n");
        fclose(file);
        context->initialized = false;
        return;
    }
    
    SaveFileV1 save = {0};
    size_t read = fread(&save, sizeof(save), 1, file);
    fclose(file);

    if (read != 1 || save.magic != 0x56534B50u || save.version != 1u)
    {
        printf("读取存档失败：格式不匹配或文件损坏，将作为新游戏开始\n");
        context->initialized = false;
        return;
    }

    FreePokemonDatabase(context);

    context->initialized = save.initialized;
    context->stage = save.stage;
    context->opponent = save.opponent;
    
    // 先初始化宝可梦数据库（这不会影响背包中的宝可梦）
    initialize_pokemons(context);
    
    // 然后恢复背包数据（包括玩家拥有的宝可梦）
    context->backpackData = save.backpackData;
    
    for (int i = 0; i < ITEM_TYPE_NUM; i++)
    {
        context->shop_items[i] = save.shop_items[i];
    }
    context->exitGame = false;
    context->has_used_item_this_turn = save.has_used_item_this_turn;
    context->has_used_skill_this_turn = save.has_used_skill_this_turn;

    //读档后统一回到菜单
    context->currentState = main_menu_state;

    // 恢复地图
    context->mapExplorationData.playerX = save.playerX;
    context->mapExplorationData.playerY = save.playerY;
    context->mapExplorationData.trampOnSomething = save.trampOnSomething;
    context->mapExplorationData.playerSprite = save.playerSprite;
    context->mapExplorationData.exit = save.map_exit;
    context->mapExplorationData.nextState = save.nextState;

    //让 LoadMap 重新加载指针数据
    context->mapExplorationData.currentMap.empty = true;
    context->mapExplorationData.currentMap.baseMap = NULL;
    context->mapExplorationData.currentMap.renderingMap = NULL;
    context->mapExplorationData.currentMap.isBlocked = NULL;
    context->mapExplorationData.currentMap.neighborPaths = NULL;
    strcpy(context->mapExplorationData.currentMap.filePath, save.currentMapPath);
    if (save.currentMapPath[0] != '\0')
    {
        LoadMap(context);
    }
    
    printf("存档读取成功！stage=%d, 宝可梦数量=%d\n", context->stage, context->backpackData.pokemon_count);
}
static void InitializeBackPack(gameContext* context)
{
    BackpackData* backPack = &(context->backpackData);
    for(int i = 0;i<ITEM_TYPE_NUM;i++)
    {
        backPack->items[i].amount = 0;
    }
}
static void InitializeForFirstGame(gameContext* context)
{
    context->initialized = true;
    context->currentState = mapExplorationFunction;
    InitializeBackPack(context);
}
static void InitializeMapExplorationData(gameContext* context)
{
    context->mapExplorationData.playerSprite = L'你';
    context->mapExplorationData.currentMap.empty = true;
    context->mapExplorationData.playerX = 10;
    context->mapExplorationData.playerY = 10;
}

static void init_shop_items(BagItem* shop_items)
{
    BagItem default_shop_items[ITEM_TYPE_NUM] = {
        {"🔹 小XP药（Small XP Potion）", 1, 150, 75, ITEM_CONSUMABLE, EFFECT_XP_GAIN, 50},
        {"🔹 大XP药（Large XP Potion）", 1, 300, 150, ITEM_CONSUMABLE, EFFECT_XP_GAIN, 150},
        {"💧 小HP恢复药水（Small HP Potion）", 1, 120, 60, ITEM_CONSUMABLE, EFFECT_HP_HEAL, 30},
        {"💧 大HP恢复药水（Large HP Potion）", 1, 250, 125, ITEM_CONSUMABLE, EFFECT_HP_HEAL, 80},
        {"🛡️ 盾牌（Shield）", 1, 400, 200, ITEM_CONSUMABLE, EFFECT_SHIELD, 1},
        {"🔋 大PP药水（Large PP Potion）", 1, 350, 175, ITEM_CONSUMABLE, EFFECT_PP_RESTORE, 5},
        {"🔴 普通球（Pokeball）", 1, 200, 100, ITEM_CONSUMABLE, EFFECT_CAPTURE, 0},
        {"🟣 大师球（Master Ball）", 1, 2000, 1000, ITEM_CONSUMABLE, EFFECT_CAPTURE, 0},
        {"⚔️ 攻击药（Attack Boost）", 1, 600, 300, ITEM_PERMANENT, EFFECT_ATK_UP, 5},
        {"✨ 闪避药（Evasion Boost）", 1, 600, 300, ITEM_PERMANENT, EFFECT_EVADE_UP, 5},
        {"💨 速度药（Speed Boost）", 1, 600, 300, ITEM_PERMANENT, EFFECT_SPEED_UP, 5},
        {"❤️ 生命药（HP Boost）", 1, 600, 300, ITEM_PERMANENT, EFFECT_HP_UP, 20},
        {"⚔️ 攻击药（Attack Boost）", 1, 600, 300, ITEM_PERMANENT, EFFECT_ATK_UP, 5},
        {"💥 暴击药（Critical Boost）", 1, 600, 300, ITEM_PERMANENT, EFFECT_CRIT_UP, 5}
    };
    for (int i = 0; i < ITEM_TYPE_NUM; i++)
    {
        shop_items[i] = default_shop_items[i];
    }
}

static void init_context_defaults(gameContext* context)
{
    memset(context, 0, sizeof(*context));
    context->currentState = main_menu_state;
    context->mapExplorationData.playerSprite = L'你';
    context->mapExplorationData.currentMap.empty = true;
    context->mapExplorationData.currentMap.filePath[0] = '\0';
    context->mapExplorationData.playerX = 10;
    context->mapExplorationData.playerY = 10;
    context->opponent = COMMON;
    context->stage = 0;
    context->exitGame = false;
    context->has_used_item_this_turn = false;
    context->has_used_skill_this_turn = false;
    init_shop_items(context->shop_items);
}

static void set_workdir_to_exe_dir(void)
{
    char exe_path[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, exe_path, (DWORD)sizeof(exe_path));
    if (len == 0 || len >= sizeof(exe_path)) return;
    char* slash = strrchr(exe_path, '\\');
    if (slash == NULL) return;
    *slash = '\0';
    SetCurrentDirectoryA(exe_path);
}
