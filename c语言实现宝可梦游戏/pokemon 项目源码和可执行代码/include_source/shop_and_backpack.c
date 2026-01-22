#pragma execution_character_set("utf-8")
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include<math.h>
#include"../include/main.h"
#include "../include/shop_and_backpack.h"
static void clear_input_buffer(void);
static void clear_screen(void);
static void wait_for_enter(void);
GameState shop_state(gameContext* context)
{
    system("cls");
    printf("                    商店\n");
    printf("              1.购买道具\n");
    printf("              2.出售道具\n");
    printf("              3.退出\n");
    int choice;
    while(1)
    {
        char input = _getch();
        if(input == '1')
        {
            choice = 1;
            break;
        }
        else if(input == '2')
        {
            choice = 2;
            break;
        }
        else if(input == '3')
        {
            choice = 3;
            break;
        }
    }
    if(choice == 1)
    {
        choose_and_buy_item(context);
        return SHOP;
    }
    else if(choice == 2)
    {
        choose_and_sell_item(context);
        return SHOP;
    }
    else if(choice == 3)
    {
        return BACKPACK;
    }
}
GameState backpack_state(gameContext* context)
{
    system("cls");
    printf("                    背包\n");
    printf("              1.查看拥有的道具\n");
    printf("              2.查看拥有的宝可梦\n");
    printf("              3.查看金钱\n");
    printf("              4.查看商店\n");
    printf("              5.返回\n");
    char choice;
    while(1)
    {
        choice = _getch();
        if(choice == '1'||choice == '2'||choice == '3'||choice == '4'||choice == '5')break;
    }
    if(choice == '1')
    {
        choose_and_check_item(context);
        return BACKPACK;
    }
    else if(choice == '2')
    {
        choose_and_interact_with_pokemons(context);
        return BACKPACK;
    }
    else if(choice == '3')
    {
        system("cls");
        printf("当前金钱：%d \n",context->backpackData.player_money);
        printf("按任意键返回...\n");
        fflush(stdout);
        _getch();
        return BACKPACK;
    }
    else if(choice == '4')
    {
        return SHOP;
    }
    else
    {
        return EXPLORE_MAP;
    }
}

// ==================== 工具函数 ====================
static void clear_input_buffer(void) {
    while (_kbhit()) _getch();
}
static void wait_for_enter(void) {
    printf("\n按任意键继续...");
    clear_input_buffer();
    _getch();
}
bool choose_true_or_false()
{
    while(1)
    {
        int input = _getch();
        if(input=='1')
        {
            return true;
        }
        else if(input == '2')
        {
            return false;
        }
    }
}
int show_and_choose_item(BagItem* items, gameContext *context, void(*show_info_callback)(BagItem* item,int index))
{
    system("cls");
    int baseY = 0;
    int available_indices[ITEM_TYPE_NUM];
    int avail_count = 0;

    for (int i = 0; i < ITEM_TYPE_NUM; i++) {
        if (items[i].amount > 0) {
            available_indices[avail_count++] = i;
            printf("   %d.%s ", i+1, items[i].name);
            show_info_callback(&(context->backpackData.items[i]), i);
        }
    }

    if (avail_count == 0) {
        printf("   目前没有道具。\n按任意键返回...\n");
        fflush(stdout);
        _getch();
        return -1;
    }

    printf("按w和s切换选中的道具\n按enter确定。\n按esc退出\n");
    fflush(stdout);
    int pre_row = 1;
    int row = baseY; 
    show_arrow(pre_row, row);

    while (1) {
        char input = _getch();
        if (input == 'w' && row > 0) 
        {
            pre_row = row;
            row--;
            show_arrow(pre_row, row);
        } 
        else if (input == 's' && row < avail_count-1) 
        {
            pre_row = row;
            row++;
            show_arrow(pre_row, row);
        } 
        else if (input == '\r') 
        {
            return available_indices[row - baseY];
        } 
        else if (input == 27) 
        {
            return -1;
        }
    }
}
void choose_and_buy_item(gameContext* context)
{
    system("cls");
    int index = 0;
    while(1)
    {
        index = show_and_choose_item(context->shop_items,context,show_item_when_buying);
        if(index==-1)break;
        system("cls");
        BagItem *item = &(context->shop_items[index]);
        show_item_when_buying(context->shop_items,index);
        printf("确定购买 %s 吗？\n按1确定，按2取消。\n",item->name);
        bool confirm = choose_true_or_false();
        if (!confirm)
        {
            continue;
        }
        int quantity;
        while (1)
        {
            system("cls");
            printf("请输入购买的 %s 数量：\n",item->name);
            fflush(stdout);
            while (scanf("%d", &quantity) == 0 || quantity < 0)
            {
                clear_input_buffer();
                printf("\033[1A\r\033[2K");
                printf("输入错误!");
                fflush(stdout);
                Sleep(200);
                printf("\r\033[2K");
            }
            int total_cost = quantity * (item->buy_price);
            int *player_money = &(context->backpackData.player_money);
            printf("购买 %s ，数量：%d，\n总花费：%d，\n当前拥有金钱：%d\n\n确定购买吗？\n按1确定，按2取消,按3返回选择界面\n",item->name,quantity,total_cost,*player_money);
            int confirm1 = 0;
            while (1)
            {
                char input = _getch();
                if (input == '1')
                {
                    confirm1 = 1;
                    break;
                }
                else if (input == '2')
                {
                    confirm1 = 2;
                    break;
                }
                else if (input == '3')
                {
                    confirm1 = 3;
                    break;
                }
            }
            if (confirm1 == 2)
            {
                continue;
            }
            else if(confirm1 == 3)
            {
                break;
            }
            if (*player_money < total_cost)
            {
                printf("金钱不足！\n所需金钱：%d\n当前金钱：%d", total_cost, *player_money);
                fflush(stdout);
                Sleep(500);
            }
            else
            {
                *player_money -= total_cost;
                item->amount += quantity;
                printf("购买成功！花费：%d，\n当前剩余金钱：%d");
                Sleep(800);
                break;
            }
        }
    }
}
void choose_and_sell_item(gameContext* context)
{
    while(1)
    {
        int index = show_and_choose_item(context->backpackData.items,context,show_item_when_selling);
        if(index==-1)break;
        BagItem* chosen_item = &(context->backpackData.items[index]);
        system("cls");
        show_item_when_selling(context->backpackData.items,index);
        printf("确认要售出 %s 吗？\n按1确定，按2取消\n",chosen_item->name);
        bool confirmed = choose_true_or_false();
        if(!confirmed)continue;
        printf("请输入售出数量：\n");
        int quantity;
        while(scanf("%d",&quantity)==0||quantity<0)
        {
            clear_input_buffer();
            printf("\r\033[1A\033[2K");
            printf("输入错误！");
            Sleep(200);
            printf("\r\033[2K");
        }
        int* amount = &(chosen_item->amount);
        quantity = min(quantity,*amount);
        int total_gain = *amount*quantity;
        printf("售出：%s，数量：%d ,获得总金额：%d，\n按1确认，按2取消，按3退出到选择界面");
        int choice;
        while(1)
        {
            char input = _getch();
            if(input=='1')
            {
                choice = 1;
                break;
            }
            else if(input == '2')
            {
                choice = 2;
                break;
            }
            else if(input == '3')
            {
                choice = 3;
                break;
            }
        }
        if(choice == 2)continue;
        else if(choice == 3)break;
        context->backpackData.player_money+=total_gain;
        chosen_item->amount-=quantity;
        printf("售出成功！当前持有 %s 数量：%d，当前拥有金钱：%d",chosen_item->name,chosen_item->amount,context->backpackData.player_money);
        Sleep(800);
    }

}

void choose_and_check_item(gameContext* context)
{
    static const char* item_descriptions[ITEM_TYPE_NUM] = {
        "为宝可梦提供少量经验，适合低等级练级时使用。",
        "提供大量经验，能帮助快速提升等级，但较为稀有。",
        "回复少量HP，适合战斗中小幅恢复。",
        "回复大量HP，能显著恢复宝可梦生命值。",
        "提供一次性护盾，能抵挡一次攻击或减少伤害（一次性道具）。",
        "恢复技能的PP值，适用于技能次数不足时使用。",
        "用于尝试捕捉野生宝可梦，普通捕获率。",
        "高级捕捉球，极高或保证捕获（按设定）。",
        "提升攻击力的道具，可增强宝可梦输出。",
        "提升闪避率的道具，提高回避能力。",
        "提升速度的道具，提高先手概率。",
        "提升最大生命值的道具，增加生存能力。",
        "提升攻击力的道具（高级），提高更高的攻击力。",
        "提升暴击率的道具，提高造成暴击的概率。"
    };

    while (1)
    {
        int index = show_and_choose_item(context->backpackData.items, context, show_item_when_checking);
        if (index == -1) break;
        system("cls");
        BagItem *item = &(context->backpackData.items[index]);
        printf("物品：%s\n", item->name);
        printf("数量：%d\n", item->amount);
        printf("买入价：%d，卖出价：%d\n", item->buy_price, item->sell_price);
        if (index >= 0 && index < ITEM_TYPE_NUM)
        {
            printf("说明：%s\n", item_descriptions[index]);
        }
        else
        {
            printf("说明：无\n");
        }
        printf("\n按任意键返回...");
        fflush(stdout);
        _getch();
    }
}
//打印物品信息的回调函数

void show_item_when_buying(BagItem* item,int index)
{
    show_item_amount(item,index);
    show_item_buy_price(item,index);
    endline(item,index);
}
void show_item_when_selling(BagItem* item,int index)
{
    show_item_amount(item,index);
    show_item_sell_price(item,index);
    endline(item,index);
}
void show_item_when_checking(BagItem* item,int index)
{
    show_item_amount(item,index);
    endline(item,index);
}
void show_item_amount(BagItem* item,int index)
{
    printf(" 当前持有数量：%d   ",item->amount,item->amount);
}
void show_item_buy_price(BagItem* item,int index)
{
    printf("买入价格：%d",item->buy_price);
}
void show_item_sell_price(BagItem* item,int index)
{
    printf("卖出价格：%d",item->sell_price);
}
void print_nothing(BagItem* item,int index)
{
    printf("");
}
void endline(BagItem* item,int index)
{
    printf("\n");
}

void show_arrow(int pre,int row)
{
    gotoXY(0, pre);
    printf("  ");
    gotoXY(0, row);
    printf("->");
}


//宝可梦相关
void add_player_pokemon(gameContext* context,Pokemondata* pokemon)
{
    BackpackData* backpack = &(context->backpackData);
    if(backpack->pokemon_count>=18)
    {
        printf("宝可梦数量已达上限，无法添加更多宝可梦。\n");
        Sleep(1000);
        return;
    }
    for(int i = 0;i<18;i++)
    {
        if(backpack->pokemons[i].empty)
        {
            backpack->pokemons[i] = *pokemon;
            backpack->pokemons[i].empty = false;
            backpack->pokemon_count++;
            printf("成功添加宝可梦：%s\n",pokemon->name);
            Sleep(1000);
            return;
        }
    }
}
void choose_and_interact_with_pokemons(gameContext* context)
{
    while (1)
    {
        int index = show_and_choose_pokemon(context);
        if (index == -1)
            break;
        Pokemondata *chosen_pokemon = &(context->backpackData.pokemons[index]);
        while (1)
        {
            system("cls");
            printf("名称：%s\n", chosen_pokemon->name);
            printf("等级：%d\n", chosen_pokemon->level);
            printf("当前积累经验：%d\n", chosen_pokemon->current_xp);
            printf("升级所需经验：%d\n", chosen_pokemon->max_xp);
            printf("最大生命值：%d\n", chosen_pokemon->max_hp);
            printf("闪避值：%d\n", chosen_pokemon->dodge);
            printf("防御值：%d\n", chosen_pokemon->defence);
            printf("速度值：%d\n", chosen_pokemon->speed);
            printf("按1查看当前技能，按2设为参战宝可梦,按3返回\n");
            int choice;
            while (1)
            {
                char input = _getch();
                if (input == '1')
                {
                    choice = 1;
                    break;
                }
                else if (input == '2')
                {
                    choice = 2;
                    break;
                }
                else if (input == '3')
                {
                    choice = 3;
                    break;
                }
            }
            if (choice == 3)
                break;
            else if (choice == 2)
            {
                context->backpackData.chosen_pokemon_index = index;
                printf("\n设置成功！\n");
                Sleep(1000);
                continue;
            }
            else if (choice == 1)
            {
                show_all_pokemon_skills(chosen_pokemon);
                continue;
            }
        }
    }
}
void show_all_pokemon_skills(Pokemondata *data)
{
    while (1)
    {
        system("cls");
        printf("所有技能：\n");
        for (int i = 0; i < SKILL_MAX_COUNT; i++)
        {
            printf("  技能 %d：%s\n", i + 1, data->all_skill[i].skill_name);
            printf("   技能伤害：%d\n", data->all_skill[i].damage);
            printf("   技能PP：%d\n", data->all_skill[i].max_pp);
            printf("   技能精准度：%d\n", data->all_skill[i].accuracy);
            printf("   技能暴击率：%d\n\n", data->all_skill[i].crit);
        }
        printf("已携带技能：\n");
        for (int i = 0; i < CARRIED_SKILL_COUNT; i++)
        {
            int skill_index = data->carried_skill[i];
            printf("   %d：%s\n", i + 1, data->all_skill[skill_index].skill_name);
        }
        printf("按1修改携带技能，按2返回\n");
        bool choice = choose_true_or_false();
        if (!choice)
            return;
        system("cls"); // 重新清屏
        
        printf("选择要携带的技能（共需选择%d个）：\n", CARRIED_SKILL_COUNT);
        for (int i = 0; i < SKILL_MAX_COUNT; i++)
        {
            printf("  技能 %d：%s\n", i + 1, data->all_skill[i].skill_name);
            printf("   技能伤害：%d\n", data->all_skill[i].damage);
            printf("   技能PP：%d\n", data->all_skill[i].max_pp);
            printf("   技能精准度：%d\n", data->all_skill[i].accuracy);
            printf("   技能暴击率：%d\n\n", data->all_skill[i].crit);
        }
        
        int carried_skill_indices[CARRIED_SKILL_COUNT] = {0, 0, 0, 0};
        
        while (1)
        {
            printf("请输入%d个技能编号（1-%d），用空格分隔：\n", CARRIED_SKILL_COUNT, SKILL_MAX_COUNT);
            fflush(stdout);
            
            int input_indices[CARRIED_SKILL_COUNT];
            int valid_input = 1;
            
            // 读取四个编号
            for (int i = 0; i < CARRIED_SKILL_COUNT; i++)
            {
                if (scanf("%d", &input_indices[i]) != 1)
                {
                    valid_input = 0;
                    clear_input_buffer();
                    break;
                }
                // 检查编号是否在有效范围内
                if (input_indices[i] < 1 || input_indices[i] > SKILL_MAX_COUNT)
                {
                    valid_input = 0;
                    clear_input_buffer();
                    break;
                }
            }
            clear_input_buffer();
            
            if (!valid_input)
            {
                printf("输入无效，请输入%d个1-%d之间的数字！\n", CARRIED_SKILL_COUNT, SKILL_MAX_COUNT);
                Sleep(500);
                continue;
            }
            
            // 检查是否有重复的编号
            int has_duplicate = 0;
            for (int i = 0; i < CARRIED_SKILL_COUNT && !has_duplicate; i++)
            {
                for (int j = i + 1; j < CARRIED_SKILL_COUNT; j++)
                {
                    if (input_indices[i] == input_indices[j])
                    {
                        has_duplicate = 1;
                        break;
                    }
                }
            }
            
            if (has_duplicate)
            {
                printf("无效输入。\n");
                Sleep(500);
                continue;
            }
            
            // 转换为0基索引并存储
            for (int i = 0; i < CARRIED_SKILL_COUNT; i++)
            {
                carried_skill_indices[i] = input_indices[i] - 1;
            }
            
            // 显示选择结果并确认
            printf("\n您选择的技能：\n");
            for (int i = 0; i < CARRIED_SKILL_COUNT; i++)
            {
                printf("   %d：%s\n", i + 1, data->all_skill[carried_skill_indices[i]].skill_name);
            }
            printf("\n按1确认，按2重新选择\n");
            
            bool confirm = choose_true_or_false();
            if (confirm)
            {
                for (int i = 0; i < CARRIED_SKILL_COUNT; i++)
                {
                    data->carried_skill[i] = carried_skill_indices[i];
                }
                printf("修改成功！");
                Sleep(1000);
                break;
            }
        }
    }
}
int show_and_choose_pokemon(gameContext* context)
{
    system("cls");
    Pokemondata* pokemons = context->backpackData.pokemons;
    int max_depth = 0;
    int available_indices[18];
    int indices_cursor = 0;
    for(int i = 0;i<18;i++)
    {
        if(pokemons[i].empty)continue;
        printf("   %d 名称%s",i+1,pokemons[i].name);
        printf("   等级：%d\n", pokemons[i].level);
        available_indices[indices_cursor++] = i;
        max_depth++;
    }
    if(max_depth==0)
    {
        printf("  现在还没有宝可梦，可以去找大木博士，兴许会有收获哦！\n");
        printf("   按任意键返回...\n");
        _getch();
        return -1;
    }
    else
    {
        Pokemondata* fight_pokemon =&(context->backpackData.pokemons[context->backpackData.chosen_pokemon_index]);
        printf("按w和s切换选定宝可梦，按enter确定，按esc退出\n\n当前出战宝可梦：%s，等级：%d",fight_pokemon->name,fight_pokemon->level);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        int baseY = 0;
        int row = baseY;
        show_arrow(row, row);
        while(1)
        {
            char input = _getch();
            if (input == 'w' && row > 1) 
            {
                int pre_row = row;
                if (row > baseY) row--;
                show_arrow(pre_row, row);
            } 
            else if (input == 's' && row < max_depth) 
            {
                int pre_row = row;
                if (row < baseY + max_depth - 1) row++;
                show_arrow(pre_row, row);
            } 
            else if (input == '\r') 
            {
                return available_indices[row - baseY];
            }
            else if (input == 27)  // ESC键退出
            {
                return -1;
            }
        }
    }
}
void try_update_pokemon(Pokemondata* pokemon)
{
    if(pokemon->current_xp>=pokemon->max_xp)
    {
        pokemon->current_xp-=pokemon->max_xp;
        pokemon->level++;
        calculate_pokemon_attribute_according_to_level(pokemon);
    }
}
void calculate_pokemon_attribute_according_to_level(Pokemondata* pokemon)
{
    double factor = get_the_corresponding_factor_according_to_level(pokemon->level);
    pokemon->max_xp = round(pokemon->max_xp*factor);
    pokemon->max_hp = round(pokemon->max_hp*factor);
    pokemon->defence = round(pokemon->defence*factor);
    pokemon->speed = round(pokemon->speed*factor);
    pokemon->dodge = round(pokemon->dodge*factor);
    Skill* all_skill = pokemon->all_skill;
    for(int i = 0;i<6;i++)
    {
        all_skill[i].accuracy = round(all_skill[i].accuracy*factor);
        all_skill[i].damage = round(all_skill[i].damage*factor);
        double ratio = (100-all_skill[i].crit)/100.0;
        ratio/=factor;
        ratio = 100.0 - ratio;
        all_skill[i].crit = round(100.0*ratio);
    }

}
double get_the_corresponding_factor_according_to_level(int level)
{
    return (level*level+level+25)/27.0;
}
void ChangeBackPackItemAmount(ItemType index,int chaneAmount,gameContext* context)
{
    if(index<0||index>ITEM_TYPE_NUM-1)return;
    context->backpackData.items[index].amount += chaneAmount;
    context->backpackData.items[index].amount = max(context->backpackData.items[index].amount,0);
}

//初始化宝可梦技能数值
Skill load_skill(const char* name, int damage, int pp, int accuracy, int crit, State effect) {
    Skill skill;
    strcpy(skill.skill_name, name);
    skill.damage = damage;
    skill.max_pp = pp;
    skill.current_pp = pp;
    skill.accuracy = accuracy;
    skill.crit = crit;
    skill.effect = effect;
    return skill;
}

//初始化宝可梦基础数值
Pokemondata load_Pokemondata(int number, char* name, Pokemontype type, int level, int xp, int hp, int dodge,  int speed, Skill skill[6]) {
    Pokemondata property;
    property.empty = false;
    property.number = number;
    strcpy(property.name, name);
    property.type = type;
    property.level = level;
    property.current_xp = 0;
    property.max_xp = xp;
    property.current_hp = hp;
    property.max_hp = hp;
    property.dodge = dodge;
    property.defence = 0;
    property.current_state = none;
    property.speed = speed;
    for (int i = 0; i < 6; i++) { property.all_skill[i] = skill[i]; }
    for (int i = 0; i < 4; i++) { property.carried_skill[i] = i; }
    return property;
}

// 初始化玩家背包
void initialize_player_bag(gameContext* context) {
    if (context == NULL) return;
    context->backpackData.player_money = 5000;

    // 初始化宝可梦背包槽为 empty，并重置计数与选中索引
    context->backpackData.pokemon_count = 0;
    context->backpackData.chosen_pokemon_index = 0;
    for (int i = 0; i < 18; i++) {
        context->backpackData.pokemons[i].empty = true;
        context->backpackData.pokemons[i].name[0] = '\0';
    }

    // 将玩家背包中的道具条目按商店道具初始化，但将数量设为 0
    for (int i = 0; i < ITEM_TYPE_NUM; i++) {
        // 复制商店定义的道具（包含价格与效果）
        context->backpackData.items[i] = context->shop_items[i];
        // 初始持有数量为 0
        context->backpackData.items[i].amount = 0;
    }

    // 初始化全部宝可梦等级为1
    for (int i = 0; i < POKEMON_DATABASE_SIZE; i++) {
        if (context->pokemon_database[i] != NULL) {
            context->pokemon_database[i]->level = 1;
        }
    }
}

// 初始化一些示例宝可梦
void initialize_pokemons(gameContext* context) {
    srand(time(NULL));
    
    //妙蛙种子

Skill bulbasaur_1 = load_skill("藤鞭", 15, 25, 100, 5, none);
Skill bulbasaur_2 = load_skill("飞叶快刀", 25, 20, 95, 25, none);
Skill bulbasaur_3 = load_skill("寄生种子", 10, 10, 90, 0, poison);
Skill bulbasaur_4 = load_skill("撕咬", 25, 40, 80, 40, none);
Skill bulbasaur_5 = load_skill("催眠粉", 0, 15, 75, 0, dizzy);
Skill bulbasaur_6 = load_skill("毒粉", 20, 35, 75, 0, poison);

Skill bulbasaur_skills[] = {bulbasaur_1, bulbasaur_2, bulbasaur_3, bulbasaur_4, bulbasaur_5, bulbasaur_6};

Pokemondata bulbasaur_starter = load_Pokemondata(1, "妙蛙种子", grass, 1, 105, 65, 35, 45, bulbasaur_skills);

//妙蛙花

Skill venusaur_1 = load_skill("藤鞭", 25, 20, 100, 5, none);
Skill venusaur_2 = load_skill("飞叶快刀", 35, 20, 95, 25, none);
Skill venusaur_3 = load_skill("日光束", 60, 10, 100, 10, none);
Skill venusaur_4 = load_skill("地震", 50, 10, 100, 5, none);
Skill venusaur_5 = load_skill("污泥炸弹", 45, 15, 100, 10, poison);
Skill venusaur_6 = load_skill("催眠粉", 0, 15, 75, 0, dizzy);

Skill venusaur_skills[] = {venusaur_1, venusaur_2, venusaur_3, venusaur_4, venusaur_5, venusaur_6};

Pokemondata venusaur = load_Pokemondata(1, "妙蛙花", grass, 2, 9999, 95, 30, 80, venusaur_skills);

//皮卡丘

Skill pikachu_1 = load_skill("电击", 12, 30, 100, 5, none); 
Skill pikachu_2 = load_skill("电光一闪", 18, 30, 100, 5, none);       
Skill pikachu_3 = load_skill("十万伏特", 35, 15, 100, 10, paralyze);
Skill pikachu_4 = load_skill("铁尾", 25, 15, 85, 15, none);           
Skill pikachu_5 = load_skill("电球", 15, 20, 100, 5, none);         
Skill pikachu_6 = load_skill("叫声", 10, 40, 120, 10, paralyze);

Skill pikachu_skills[] = {pikachu_1, pikachu_2, pikachu_3, pikachu_4, pikachu_5, pikachu_6};

Pokemondata pikachu_starter = load_Pokemondata(2, "皮卡丘", electricity,1, 112, 55, 40, 70,pikachu_skills);

//雷丘

Skill raichu_1 = load_skill("电击", 20, 30, 100, 5, none);
Skill raichu_2 = load_skill("十万伏特", 45, 15, 100, 10, paralyze);
Skill raichu_3 = load_skill("打雷", 70, 10, 70, 15, paralyze);
Skill raichu_4 = load_skill("光墙", 0, 30, 100, 0, none);
Skill raichu_5 = load_skill("高速移动", 0, 30, 100, 0, none);
Skill raichu_6 = load_skill("铁尾", 35, 15, 90, 15, none);

Skill raichu_skills[] = {raichu_1, raichu_2, raichu_3, raichu_4, raichu_5, raichu_6};

Pokemondata raichu = load_Pokemondata(2, "雷丘", electricity, 2, 9999, 80, 50, 110, raichu_skills);

//小火龙

Skill charmander_1 = load_skill("火花", 20, 25, 100, 5, burn);
Skill charmander_2 = load_skill("抓", 15, 35, 100, 5, none);
Skill charmander_3 = load_skill("烟雾", 0, 20, 100, 0, paralyze);
Skill charmander_4 = load_skill("龙之怒", 40, 10, 100, 0, none);
Skill charmander_5 = load_skill("劈开", 25, 20, 100, 10, none);
Skill charmander_6 = load_skill("火焰旋涡", 15, 15, 85, 0, burn);

Skill charmander_skills[] = {charmander_1, charmander_2, charmander_3, charmander_4, charmander_5, charmander_6};

Pokemondata charmander = load_Pokemondata(3, "小火龙", fire, 1, 105, 60, 45, 65, charmander_skills);

//火恐龙

Skill charmeleon_1 = load_skill("火花", 25, 25, 100, 5, burn);
Skill charmeleon_2 = load_skill("抓", 20, 35, 100, 5, none);
Skill charmeleon_3 = load_skill("喷射火焰", 30, 15, 80, 10, burn);
Skill charmeleon_4 = load_skill("龙之怒", 40, 10, 100, 0, none);
Skill charmeleon_5 = load_skill("劈开", 30, 20, 100, 10, none);
Skill charmeleon_6 = load_skill("鬼面", 55, 15, 60, 0, burn);

Skill charmeleon_skills[] = {charmeleon_1, charmeleon_2, charmeleon_3, charmeleon_4, charmeleon_5, charmeleon_6};

Pokemondata charmeleon = load_Pokemondata(3, "火恐龙", fire, 2, 9999, 90, 50, 80, charmeleon_skills);

//杰尼龟

Skill squirtle_1 = load_skill("水枪", 20, 25, 100, 5, frozen);
Skill squirtle_2 = load_skill("撞击", 15, 35, 110, 5, none);
Skill squirtle_3 = load_skill("凝固", 0, 40, 90, 0, dizzy);
Skill squirtle_4 = load_skill("泡沫光线", 30, 20, 100, 10, none);
Skill squirtle_5 = load_skill("咬住", 25, 20, 100, 10, paralyze);
Skill squirtle_6 = load_skill("水之波动", 35, 15, 100, 5, none);

Skill squirtle_skills[] = {squirtle_1, squirtle_2, squirtle_3, squirtle_4, squirtle_5, squirtle_6};

Pokemondata squirtle = load_Pokemondata(4, "杰尼龟", water, 1, 105, 55, 40, 63, squirtle_skills);

//卡咪龟

Skill wartortle_1 = load_skill("水枪", 25, 25, 100, 5, none);
Skill wartortle_2 = load_skill("水炮", 60, 10, 80, 10, none);
Skill wartortle_3 = load_skill("凝固", 0, 40, 110, 0, dizzy);
Skill wartortle_4 = load_skill("泡沫光线", 35, 20, 100, 10, none);
Skill wartortle_5 = load_skill("咬住", 30, 20, 100, 10, none);
Skill wartortle_6 = load_skill("高速旋转", 20, 40, 100, 5, none);

Skill wartortle_skills[] = {wartortle_1, wartortle_2, wartortle_3, wartortle_4, wartortle_5, wartortle_6};

Pokemondata wartortle = load_Pokemondata(4, "卡咪龟", water, 2, 9999,90, 55, 58, wartortle_skills);

//猴怪
Skill mankey_1 = load_skill("抓", 18, 30, 100, 5, none);
Skill mankey_2 = load_skill("瞪眼", 0, 30, 80, 0, dizzy);
Skill mankey_3 = load_skill("踢倒", 25, 20, 100, 5, paralyze);
Skill mankey_4 = load_skill("地球上投", 30, 20, 100, 5, none);
Skill mankey_5 = load_skill("十字劈", 35, 20, 80, 15, none);
Skill mankey_6 = load_skill("愤怒", 20, 20, 100, 5, none);

Skill mankey_skills[] = {mankey_1, mankey_2, mankey_3, mankey_4, mankey_5, mankey_6};

Pokemondata mankey = load_Pokemondata(5, "猴怪", wrestle, 1, 105,60, 55, 70, mankey_skills);

//火爆猴
Skill primeape_1 = load_skill("抓", 22, 30, 100, 5, none);
Skill primeape_2 = load_skill("十字劈", 55, 20, 80, 15, none);
Skill primeape_3 = load_skill("地球上投", 35, 20, 100, 5, none);
Skill primeape_4 = load_skill("逆鳞", 55, 15, 80, 5, none);
Skill primeape_5 = load_skill("大闹一番",40, 10, 100, 5, none);
Skill primeape_6 = load_skill("愤怒", 25, 20, 100, 5, paralyze);

Skill primeape_skills[] = {primeape_1, primeape_2, primeape_3, primeape_4, primeape_5, primeape_6};

Pokemondata primeape = load_Pokemondata(5, "火爆猴", wrestle, 2, 9999,95, 65, 95, primeape_skills);

//波波

Skill pidgey_1 = load_skill("撞击", 15, 35, 100, 5, none);
Skill pidgey_2 = load_skill("起风", 20, 35, 100, 5, none);
Skill pidgey_3 = load_skill("拨沙", 10, 15, 100, 0, paralyze);
Skill pidgey_4 = load_skill("电光一闪", 20, 30, 110, 5, none);
Skill pidgey_5 = load_skill("龙卷风", 25, 20, 100, 5, none);
Skill pidgey_6 = load_skill("羽毛舞", 0, 15, 100, 0, dizzy);

Skill pidgey_skills[] = {pidgey_1, pidgey_2, pidgey_3, pidgey_4, pidgey_5, pidgey_6};

Pokemondata pidgey = load_Pokemondata(6, "波波", flight, 1, 105, 50, 50, 56, pidgey_skills);

//比比鸟

Skill pidgeotto_1 = load_skill("撞击", 20, 35, 100, 10, paralyze);
Skill pidgeotto_2 = load_skill("起风", 25, 35, 100, 5, none);
Skill pidgeotto_3 = load_skill("电光一闪", 25, 30, 110, 5, none);
Skill pidgeotto_4 = load_skill("龙卷风", 25, 20, 110, 10, none);
Skill pidgeotto_5 = load_skill("翅膀攻击", 35, 35, 100, 5, none);
Skill pidgeotto_6 = load_skill("羽毛舞", 0, 15, 120, 0, dizzy);

Skill pidgeotto_skills[] = {pidgeotto_1, pidgeotto_2, pidgeotto_3, pidgeotto_4, pidgeotto_5, pidgeotto_6};

Pokemondata pidgeotto = load_Pokemondata(6, "比比鸟", flight, 2, 9999, 85, 60, 71, pidgeotto_skills);

//小拳石
Skill geodude_1 = load_skill("撞击", 20, 30, 80, 5, none);
Skill geodude_2 = load_skill("地震", 10, 30, 80, 0,dizzy);
Skill geodude_3 = load_skill("落石", 25, 15, 80, 5, none);
Skill geodude_4 = load_skill("震级", 30, 10, 70, 5, none);
Skill geodude_5 = load_skill("滚动", 25, 20, 90, 5, none);
Skill geodude_6 = load_skill("自爆", 60, 1, 50, 0, none);

Skill geodude_skills[] = {geodude_1, geodude_2, geodude_3, geodude_4, geodude_5, geodude_6};

Pokemondata geodude = load_Pokemondata(7, "小拳石", soil, 1, 105, 60, 30, 20, geodude_skills);

//隆隆石
Skill graveler_1 = load_skill("撞击", 25, 30, 100, 5, none);
Skill graveler_2 = load_skill("地震", 20, 30, 90, 0,dizzy);
Skill graveler_3 = load_skill("落石", 30, 15, 90, 5, none);
Skill graveler_4 = load_skill("震级", 50, 10, 100, 5, none);
Skill graveler_5 = load_skill("滚动", 40, 20, 90, 5, none);
Skill graveler_6 = load_skill("自爆", 100, 1, 60, 0, none);

Skill graveler_skills[] = {graveler_1, graveler_2, graveler_3, graveler_4, graveler_5, graveler_6};

Pokemondata graveler = load_Pokemondata(7, "隆隆石", soil, 2, 9999,95 ,40, 35, graveler_skills);

//小拉达
Skill rattata_1 = load_skill("撞击", 15, 35, 100, 5, none);
Skill rattata_2 = load_skill("摇尾巴", 0, 30, 100, 0, dizzy);
Skill rattata_3 = load_skill("电光一闪", 20, 30, 100, 5, none);
Skill rattata_4 = load_skill("咬住", 25, 20, 100, 10, none);
Skill rattata_5 = load_skill("必杀门牙", 40, 5, 80, 15, none);
Skill rattata_6 = load_skill("愤怒门牙", 20, 10, 90, 0, none);

Skill rattata_skills[] = {rattata_1, rattata_2, rattata_3, rattata_4, rattata_5, rattata_6};

Pokemondata rattata = load_Pokemondata(8, "小拉达", normal, 1, 105, 50, 55, 72, rattata_skills);

//拉达
Skill raticate_1 = load_skill("撞击", 20, 35, 100, 5, none);
Skill raticate_2 = load_skill("电光一闪", 25, 30, 100, 5, none);
Skill raticate_3 = load_skill("咬住", 30, 20, 100, 10, paralyze);
Skill raticate_4 = load_skill("必杀门牙", 50, 5, 90, 15, none);
Skill raticate_5 = load_skill("愤怒门牙", 25, 10, 90, 0, none);
Skill raticate_6 = load_skill("追打", 40, 20, 100, 5, paralyze);

Skill raticate_skills[] = {raticate_1, raticate_2, raticate_3, raticate_4, raticate_5, raticate_6};

Pokemondata raticate = load_Pokemondata(8 ,"拉达", normal, 2, 9999,80, 70, 97, raticate_skills);

//大岩蛇
Skill onix_1 = load_skill("撞击", 25, 35, 100, 5, none);
Skill onix_2 = load_skill("地震", 0, 30, 100, 0, dizzy);
Skill onix_3 = load_skill("落石", 35, 15, 90, 5, none);
Skill onix_4 = load_skill("绑紧", 15, 20, 85, 0, paralyze);
Skill onix_5 = load_skill("地震", 50, 10, 100, 5, none);
Skill onix_6 = load_skill("铁尾", 35, 15, 85, 10, none);

Skill onix_skills[] = {onix_1, onix_2, onix_3, onix_4, onix_5, onix_6};

Pokemondata onix = load_Pokemondata(61, "大岩蛇", soil, 1, 9999, 110, 30, 70, onix_skills);

//宝石海星
Skill starmie_1 = load_skill("水枪", 30, 25, 100, 5, none);
Skill starmie_2 = load_skill("高速旋转", 25, 40, 100, 5, none);
Skill starmie_3 = load_skill("潮汐", 25, 15, 100, 5, paralyze);
Skill starmie_4 = load_skill("幻象光线", 35, 20, 100, 10, paralyze);
Skill starmie_5 = load_skill("冲浪", 50, 15, 100, 5, none);
Skill starmie_6 = load_skill("迷惑冲击", 0, 10, 100, 0, dizzy);

Skill starmie_skills[] = {starmie_1, starmie_2, starmie_3, starmie_4, starmie_5, starmie_6};

Pokemondata starmie = load_Pokemondata(62, "宝石海星", water, 1, 9999, 95, 85, 115, starmie_skills);

//绿毛虫
Skill caterpie_1 = load_skill("撞击", 15, 35, 100, 5, none);
Skill caterpie_2 = load_skill("吐丝", 0, 40, 95, 0, paralyze);
Skill caterpie_3 = load_skill("虫咬", 20, 25, 100, 5, none);
Skill caterpie_4 = load_skill("挣扎", 25, 20, 100, 5, none);
Skill caterpie_5 = load_skill("硬化", 0, 30, 100, 0, none);
Skill caterpie_6 = load_skill("电光一闪", 18, 30, 100, 5, none);

Skill caterpie_skills[] = {caterpie_1, caterpie_2, caterpie_3, caterpie_4, caterpie_5, caterpie_6};

Pokemondata caterpie = load_Pokemondata(10, "绿毛虫", grass, 1, 80, 45, 35, 45, caterpie_skills);

//元婴期//
Skill kobe_1 = load_skill("平A", 350234, 50, 120, 0, none);
Skill kobe_2 = load_skill("平A", 350234, 50, 120, 0, none);
Skill kobe_3 = load_skill("平A", 350234, 50, 120, 0, none);
Skill kobe_4 = load_skill("平A", 350234, 50, 120, 0, none);
Skill kobe_5 = load_skill("平A", 350234, 50, 120, 0, none);
Skill kobe_6 = load_skill("平A", 350234, 50, 120, 0, none);

Skill kobe_skills[] = {kobe_1, kobe_2, kobe_3,kobe_4, kobe_5, kobe_6};

Pokemondata kobe = load_Pokemondata(63, "元婴期", normal, 1, 9999,300, 60,80, kobe_skills);

    // 将初始化完成的宝可梦数据加入到context->pokemon_database中
    int index = 0;
    
    // 为每个宝可梦分配内存并添加到数据库
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = bulbasaur_starter;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = venusaur;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = pikachu_starter;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = raichu;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = charmander;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = charmeleon;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = squirtle;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = wartortle;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = mankey;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = primeape;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = pidgey;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = pidgeotto;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = geodude;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = graveler;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = rattata;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = raticate;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = onix;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = starmie;
    context->pokemon_database[index]->skill_count = 6;
    index++;

    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = caterpie;
    context->pokemon_database[index]->skill_count = 6;
    index++;

    context->pokemon_database[index] = (Pokemondata*)malloc(sizeof(Pokemondata));
    *context->pokemon_database[index] = kobe;
    context->pokemon_database[index]->skill_count = 6;
    index++;
    
    // 标记数据库已初始化
    context->database_initialized = 1;
}

