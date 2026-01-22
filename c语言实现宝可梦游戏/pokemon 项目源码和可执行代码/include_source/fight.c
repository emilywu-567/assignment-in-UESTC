
#include"../include/main.h"
#include"conio.h"
#include <windows.h>
#include <stdbool.h>
#include <locale.h>
// 设置UTF-8编码
void setUTF8() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, "zh_CN.UTF-8");
}

// 玩家背包已移入 gameContext->backpackData



// 清屏函数（跨平台）
void clear_screen() {
        system("cls");
}

// 清空输入缓冲区（非阻塞式）
void clear_input_buffer() {
    while (_kbhit()) _getch();
}

// 等待用户按Enter键//
void wait_for_enter() {
    printf("\n按任意键继续...");
    clear_input_buffer();
    _getch(); // 等待用户按任意键
}




// 尝试捕捉宝可梦
bool try_capture_pokemon(BagItem* ball_item, Pokemondata* target_pokemon) {
    printf("使用了 %s！\n", ball_item->name);
    
    // 大师球直接成功（通过检查道具名称中是否包含"大师球"）
    if (strstr(ball_item->name, "大师球") != NULL || strstr(ball_item->name, "Master Ball") != NULL) { 
        printf("大师球发出耀眼的光芒！\n");
        printf("\n捕捉成功！\n");
        return true;
    }
    // 普通球有一定概率捕捉成功
    else {
        // 计算捕捉成功率：基础50%，宝可梦HP越低成功率越高
        int capture_rate = 50;
        if (target_pokemon != NULL && target_pokemon->max_hp > 0) {
            int hp_percent = (target_pokemon->current_hp * 100) / target_pokemon->max_hp;
            capture_rate += (100 - hp_percent) / 2;  // HP越低，成功率越高
        }
        
        if ((rand() % 100 + 1) <= capture_rate) {
            printf("\n捕捉成功！\n");
            return true;
        } else {
            printf("\n宝可梦挣脱了！\n");
            return false;
        }
    }
}



// 战斗中显示道具信息的回调函数
void show_item_during_battle(BagItem* item, int index) {
    if (item == NULL) return;
    printf("×%d ", item->amount);
    
    // 根据道具效果显示信息
    switch (item->effect_type) {
        case EFFECT_HP_HEAL:
            printf("（恢复HP %d）", item->effect_value);
            break;
        case EFFECT_PP_RESTORE:
            printf("（恢复PP %d）", item->effect_value);
            break;
        case EFFECT_ATK_UP:
            printf("（攻击+%d）", item->effect_value);
            break;
        case EFFECT_SPEED_UP:
            printf("（速度+%d）", item->effect_value);
            break;
        case EFFECT_DEFENCE_UP:
            printf("（防御+%d）", item->effect_value);
            break;
        default:
            printf("（未知效果）");
            break;
    }
    // 在战斗界面显示时，标注为“本场战斗有效”的提示（消耗品及战斗使用类道具在战斗结束后不影响存档）
    if (item->item_type == ITEM_CONSUMABLE) {
        printf("（本场战斗有效）");
    }
    printf("\n");
}

// 使用道具
bool use_item_from_bag(gameContext* context, int item_index, Pokemondata* target) {
    if (context == NULL) return false;
    BackpackData* bag = &context->backpackData;

    // item_index 已经是实际的数组索引（来自show_and_choose_item的返回值）
    if (item_index < 0 || item_index >= ITEM_TYPE_NUM) {
        printf("无效的道具编号！\n");
        return false;
    }
    
    BagItem* item = &bag->items[item_index];
    
    // 检查道具是否有效（数量 > 0）
    if (item->amount <= 0) {
        printf("无效的道具编号！\n");
        return false;
    }
    
    bool item_used = true;
    
    switch (item->effect_type) {
        case EFFECT_HP_HEAL:
            target->current_hp += item->effect_value;
            if (target->current_hp > target->max_hp) {
                target->current_hp = target->max_hp;
            }
            printf("💚 %s 恢复了 %d HP！\n", target->name, item->effect_value);
            break;
        
        case EFFECT_XP_GAIN:
            target->current_xp += item->effect_value;
            printf("🔹 %s 获得了 %d 经验值！\n", target->name, item->effect_value);
            // 尝试升级
            try_update_pokemon(target);
            break;
            
        case EFFECT_PP_RESTORE:
            for (int i = 0; i < 4; i++) {
                int si = target->carried_skill[i];
                target->all_skill[si].current_pp += item->effect_value;
                if (target->all_skill[si].current_pp > target->all_skill[si].max_pp) {
                    target->all_skill[si].current_pp = target->all_skill[si].max_pp;
                }
            }
            printf("🔋 %s 的所有技能恢复了 %d PP！\n", target->name, item->effect_value);
            break;
        
        case EFFECT_SHIELD:
            target->defence += item->effect_value * 10;  // 盾牌提供临时防御
            printf("🛡️ %s 获得了护盾，防御力提升！\n", target->name);
            break;
            
        case EFFECT_ATK_UP:
            // 攻击力提升：增加所有技能的伤害
            for (int i = 0; i < 6; i++) {
                target->all_skill[i].damage += item->effect_value;
            }
            printf("⚔️ %s 的攻击力提升了 %d 点！\n", target->name, item->effect_value);
            break;
        
        case EFFECT_EVADE_UP:
            target->dodge += item->effect_value;
            printf("✨ %s 的闪避率提升了 %d 点！\n", target->name, item->effect_value);
            break;
            
        case EFFECT_SPEED_UP:
            target->speed += item->effect_value;
            printf("💨 %s 的速度提升了 %d 点！\n", target->name, item->effect_value);
            break;
        
        case EFFECT_HP_UP:
            target->max_hp += item->effect_value;
            target->current_hp += item->effect_value;  // 同时恢复等量HP
            printf("❤️ %s 的最大生命值提升了 %d 点！\n", target->name, item->effect_value);
            break;
        
        case EFFECT_CRIT_UP:
            // 暴击率提升：增加所有技能的暴击率
            for (int i = 0; i < 6; i++) {
                target->all_skill[i].crit += item->effect_value;
                if (target->all_skill[i].crit > 100) {
                    target->all_skill[i].crit = 100;
                }
            }
            printf("💥 %s 的暴击率提升了 %d%%！\n", target->name, item->effect_value);
            break;
            
        case EFFECT_DEFENCE_UP:
            target->defence += item->effect_value;
            printf("🛡️ %s 的防御提升了 %d 点！\n", target->name, item->effect_value);
            break;
        
        case EFFECT_NONE:
            // 无效果的道具
            printf("❓ 这个道具没有可用效果。\n");
            item_used = false;
            break;
            
        case EFFECT_CAPTURE:
            if (context->opponent == COMMON)
            {
                if (try_capture_pokemon(item, target)) {
                    // 捕捉成功，添加宝可梦到背包
                    add_player_pokemon(context, target);
                    return true; // 捕捉成功会结束战斗
                }
            }
            else
            {
                printf("该宝可梦无法被捕捉！\n");
                item_used = false;
            }
            break;

        default:
            printf("❓ 这个道具没有可用效果。\n");
            item_used = false;
            return false;
    }
    
    // 如果是消耗品，数量减少
    if (item->item_type == ITEM_CONSUMABLE && item_used) {
        printf("使用了消耗品: %s\n", item->name);
        if (item->amount > 0) item->amount--;
    }
    
    return item_used;
}

// 在战斗中使用道具：包装原有函数并在战斗中明确提示“本场生效”
bool use_item_from_bag_in_battle(gameContext* context, int item_index, Pokemondata* target) {
    if (context == NULL) return false;
    BackpackData* bag = &context->backpackData;
    if (item_index < 0 || item_index >= ITEM_TYPE_NUM) return false;
    BagItem* item = &bag->items[item_index];

    bool used = use_item_from_bag(context, item_index, target);
    return used;
}

// 在战斗中显示背包选项 - 返回true表示使用了道具，false表示取消
bool show_battle_bag_menu(gameContext* context, Pokemondata* player) {
    if (context == NULL || player == NULL) return false;
    
    BackpackData* bag = &(context->backpackData);

    // 统计背包中可用物品的数量
    int visible_count = 0;
    for (int i = 0; i < ITEM_TYPE_NUM; i++) {
        if (bag->items[i].amount > 0) visible_count++;
    }
    
    // 背包为空时返回
    if (visible_count == 0) {
        printf("\n🎒 背包是空的！\n");
        printf("按Enter返回...");
        clear_input_buffer();
        getchar(); 
        return false;
    }
    
    // 显示战斗背包标题和当前宝可梦信息
    system("cls");
    printf("\n═══════════════════════════════════════════\n");
    printf("          🎒 战斗背包 - 选择道具\n");
    printf("═══════════════════════════════════════════\n");
    printf("当前宝可梦: %s\n", player->name);
    printf("HP: %d/%d  |  金钱: %d\n", 
           player->current_hp, 
           player->max_hp,
           bag->player_money);
    printf("───────────────────────────────────────────\n\n");
    
    // 使用show_and_choose_item显示并选择道具
    int selected_item_index = show_and_choose_item(bag->items,context, show_item_during_battle);
    
    // 如果用户取消选择（返回-1），则返回false
    if (selected_item_index == -1) {
        return false;
    }
    
    // 获取选中的道具
    BagItem* selected_item = &bag->items[selected_item_index];
    
    // 二次确认
    system("cls");
    printf("═══════════════════════════════════════════\n");
    printf("            确认使用道具\n");
    printf("═══════════════════════════════════════════\n");
    printf("道具名称: %s\n", selected_item->name);
    printf("剩余数量: %d\n", selected_item->amount);
    printf("效果: ");
    
    switch (selected_item->effect_type) {
        case EFFECT_HP_HEAL:
            printf("恢复HP %d点\n", selected_item->effect_value);
            break;
        case EFFECT_XP_GAIN:
            printf("获得经验值 %d点\n", selected_item->effect_value);
            break;
        case EFFECT_PP_RESTORE:
            printf("恢复PP %d点\n", selected_item->effect_value);
            break;
        case EFFECT_SHIELD:
            printf("获得护盾保护\n");
            break;
        case EFFECT_ATK_UP:
            printf("攻击力+%d\n", selected_item->effect_value);
            break;
        case EFFECT_EVADE_UP:
            printf("闪避率+%d\n", selected_item->effect_value);
            break;
        case EFFECT_SPEED_UP:
            printf("速度+%d\n", selected_item->effect_value);
            break;
        case EFFECT_HP_UP:
            printf("最大生命值+%d\n", selected_item->effect_value);
            break;
        case EFFECT_CRIT_UP:
            printf("暴击率+%d%%\n", selected_item->effect_value);
            break;
        case EFFECT_DEFENCE_UP:
            printf("防御+%d\n", selected_item->effect_value);
            break;
        case EFFECT_NONE:
        case EFFECT_CAPTURE:
            printf("尝试捕捉宝可梦\n");
            break;
        default:
            printf("未知效果\n");
            break;
    }
    
    printf("\n确定使用吗? (y/n): ");
    fflush(stdout);
    
    char confirm;
    scanf("%c", &confirm);
    clear_input_buffer();
    
    if (confirm != 'y' && confirm != 'Y') {
        return false;
    }
    
    // 使用道具（selected_item_index是show_and_choose_item的直接返回值）
    return use_item_from_bag_in_battle(context, selected_item_index, player);
}



//属性克制系数//
double get_type_multiplier(Pokemontype attacker_type, Pokemontype defender_type) {
    if (attacker_type == defender_type) return 1.0;
    
    switch (attacker_type) {
        case fire:
            if (defender_type == grass) return 2.0;
            if (defender_type == water || defender_type == soil) return 0.5;
            break;
        case grass:
            if (defender_type == water || defender_type == soil) return 2.0;
            if (defender_type == fire || defender_type == flight) return 0.5;
            break;
        case water:
            if (defender_type == fire || defender_type == soil) return 2.0;
            if (defender_type == grass || defender_type == electricity) return 0.5;
            break;
        case electricity:
            if (defender_type == water || defender_type == flight) return 2.0;
            if (defender_type == grass || defender_type == soil) return 0.5;
            break;
        case wrestle:
            if (defender_type == normal || defender_type == soil) return 2.0;
            if (defender_type == flight) return 0.5;
            break;
        case flight:
            if (defender_type == grass || defender_type == wrestle) return 2.0;
            if (defender_type == electricity || defender_type == soil) return 0.5;
            break;
        case soil:
            if (defender_type == fire || defender_type == electricity) return 2.0;
            if (defender_type == grass || defender_type == flight) return 0.5;
            break;
        default:
            return 1.0;
    }
    return 1.0;
}

//命中率计算//
bool hit_check(int dodge, int accuracy) {
    int hit_chance = accuracy - dodge;
    if (hit_chance < 0) hit_chance = 0;
    if (hit_chance > 100) hit_chance = 100;
    
    return (rand() % 100) < hit_chance;
}

//暴击率计算//
bool crit_check(int crit) {
    return (rand() % 100) < crit;
}

//计算伤害//
int calc_damage(Pokemondata* attacker, Pokemondata* defender, int chosen_skill) {
    Skill* skill = &attacker->all_skill[attacker->carried_skill[chosen_skill]];
    double multiplier = get_type_multiplier(attacker->type, defender->type);
    double random_factor = 0.90 + (rand() % 21) / 100.0;
    double crit_mult = crit_check(skill->crit) ? 1.5 : 1.0;
    
    int damage = (int)(skill->damage * multiplier * random_factor * crit_mult);
    damage = damage * (100 - defender->defence) / 100;
    
    return damage >= 0 ? damage : 0;
}

//结算额外影响//
void apply_status_effect(Pokemondata* target) {
    switch (target->current_state) {
        case poison:
        case burn:
            target->current_hp -= (target->max_hp / 12);
            printf("%s 受到持续伤害！\n", target->name);
            if (target->current_hp <= 0) {
                target->current_hp = 0;
            }
            break;
        case paralyze:
            // 准确度惩罚在攻击时处理
            break;
        default:
            break;
    }
}

//恢复疲软//
void recover_paralyze(Pokemondata* target) {
    if (target->current_state == paralyze) {
        // 恢复原始准确度
        target->current_state = none;
    }
}

//清除冰冻状态//
void recover_frozen(Pokemondata* pokemon) {
    if (pokemon->current_state == frozen) {
        pokemon->current_state = none;
        pokemon->speed += 1; // 恢复减少的速度
    }
}

//判断先手//
Pokemondata* get_first_attacker(Pokemondata* p1, Pokemondata* p2) {
    // 检查冰冻状态对速度的影响
    int speed1 = p1->current_state == frozen ? p1->speed - 1 : p1->speed;
    int speed2 = p2->current_state == frozen ? p2->speed - 1 : p2->speed;
    
    if (speed1 >= speed2) {
        return p1;
    } else {
        return p2;
    }
}

//经验//
void gain_xp(Pokemondata* player, int xp) {
    player->current_xp += xp;
    printf("%s 获得了 %d 经验值！\n", player->name, xp);
    
    // 使用背包模块的升级函数检查并处理升级
    int old_level = player->level;
    try_update_pokemon(player);
    if (player->level > old_level) {
        printf("🎉 恭喜！%s 升级到 %d 级！\n", player->name, player->level);
    }
}

//显示宝可梦状态//
void display_pokemon_status(Pokemondata* pokemon, bool is_player) {
    char* state_str;
    switch (pokemon->current_state) {
        case dizzy: state_str = "晕眩"; break;
        case frozen: state_str = "冰冻"; break;
        case burn: state_str = "灼烧"; break;
        case poison: state_str = "中毒"; break;
        case paralyze: state_str = "疲软"; break;
        case none: state_str = "正常"; break;
        default: state_str = "未知"; break;
    }
    
    char* type_str;
    switch (pokemon->type) {
        case normal: type_str = "一般"; break;
        case fire: type_str = "火"; break;
        case grass: type_str = "草"; break;
        case water: type_str = "水"; break;
        case electricity: type_str = "电"; break;
        case wrestle: type_str = "格斗"; break;
        case flight: type_str = "飞行"; break;
        case soil: type_str = "地面"; break;
        default: type_str = "未知"; break;
    }
    
    if (is_player) {
        printf("\n=== 你的宝可梦 ===\n");
    } else {
        printf("\n=== 敌方宝可梦 ===\n");
    }
    
    printf("名称: %s\n", pokemon->name);
    printf("等级: %d\n", pokemon->level);
    printf("属性: %s\n", type_str);
    printf("状态: %s\n", state_str);
    printf("HP: %d/%d\n", pokemon->current_hp, pokemon->max_hp);
    printf("经验: %d/%d\n", pokemon->current_xp, pokemon->max_xp);
    printf("速度: %d\n", pokemon->speed);
}

//显示技能列表//
void display_skills(Pokemondata* pokemon) {
    printf("\n=== 可用技能 ===\n");
    for (int i = 0; i < 4; i++) {
        Skill* skill = &pokemon->all_skill[pokemon->carried_skill[i]];
        char* effect_str;
        switch (skill->effect) {
            case dizzy: effect_str = "晕眩"; break;
            case frozen: effect_str = "冰冻"; break;
            case burn: effect_str = "灼烧"; break;
            case poison: effect_str = "中毒"; break;
            case paralyze: effect_str = "疲软"; break;
            case none: effect_str = "无"; break;
            default: effect_str = "未知"; break;
        }
        
        printf("%d. %s (伤害: %d, PP: %d/%d, 命中: %d%%, 暴击: %d%%, 效果: %s)\n",
               i + 1,
               skill->skill_name,
               skill->damage,
               skill->current_pp,
               skill->max_pp,
               skill->accuracy,
               skill->crit,
               effect_str);
    }
}

//显示回合状态
void show_turn_status(gameContext* context) {
    printf("\n=== 本回合状态 ===\n");
    printf("已使用道具: %s\n", context->has_used_item_this_turn ? "✓" : "✗");
    printf("已使用技能: %s\n", context->has_used_skill_this_turn ? "✓" : "✗");
    
    // 特殊提示：如果用了技能就不能用道具
    if (context->has_used_skill_this_turn) {
        printf("注意：已使用技能，本回合不能再使用道具！\n");
    }
    if (context->has_used_item_this_turn) {
        printf("注意：已使用道具，本回合不能再使用道具！\n");
    }
    printf("=====================\n");
}

//逃跑函数//
bool try_to_run(Pokemondata* player, Pokemondata* enemy) {
    printf("\n%s 尝试逃跑...\n", player->name);
    
    // 简单的逃跑逻辑：成功率基于速度差异
    int speed_diff = player->speed - enemy->speed;
    int escape_chance = 50 + speed_diff * 5; // 基础50% + 速度差异加成
    
    // 确保概率在0-100之间
    if (escape_chance < 0) escape_chance = 0;
    if (escape_chance > 100) escape_chance = 100;
    
    printf("逃跑成功率: %d\n", escape_chance);
    
    if ((my_rand_range(1,100)) <= escape_chance) {
        printf("成功逃脱了！\n");
        return true;
    } else {
        printf("逃跑失败了！\n");
        return false;
    }
}
//检查战斗是否结束//
BattleResult check_battle_end(gameContext* context, Pokemondata* player, Pokemondata* enemy) {
    if (player->current_hp <= 0) {
        printf("\n%s 倒下了！\n", player->name);
        printf("你输了这场战斗...\n");
        return BATTLE_LOSE;
    }
    
    if (enemy->current_hp <= 0) {
        printf("\n%s 倒下了！\n", enemy->name);
        printf("你赢得了这场战斗！\n");
        
        // 计算经验值和金钱奖励
        int xp_gain = enemy->level * 50;
        int money_gain = enemy->level * 100;
        
        // 给战斗中的宝可梦增加经验
        gain_xp(player, xp_gain);
        
        if (context != NULL) {
            // 获得金钱奖励
            context->backpackData.player_money += money_gain;
            printf("💰 获得了 %d 金钱！\n", money_gain);
            
            // 同步经验到背包中的宝可梦
            if (context->backpackData.pokemon_count > 0 &&
                context->backpackData.chosen_pokemon_index >= 0 &&
                context->backpackData.chosen_pokemon_index < context->backpackData.pokemon_count) {
                
                Pokemondata* backpack_pokemon = &context->backpackData.pokemons[context->backpackData.chosen_pokemon_index];
                int old_level = backpack_pokemon->level;
                backpack_pokemon->current_xp += xp_gain;
                
                // 使用背包模块的升级函数
                try_update_pokemon(backpack_pokemon);
                
                if (backpack_pokemon->level > old_level) {
                    printf("📈 你的 %s 升级到了 %d 级！\n", backpack_pokemon->name, backpack_pokemon->level);
                }
            }
        }
        
        return BATTLE_WIN;
    }
    
    return BATTLE_ONGOING;
}
//玩家回合//
BattleResult player_turn(gameContext* context, Pokemondata* player, Pokemondata* enemy) {
    // 重置回合状态
    context->has_used_item_this_turn = false;
    context->has_used_skill_this_turn = false;
    
    bool turn_finished = false;
    while (!turn_finished) {
        clear_screen();
        
        // 显示状态
        display_pokemon_status(player, true);
        display_pokemon_status(enemy, false);
        
        printf("\n═══════════════════════════════════════════\n");
        printf("              你的回合\n");
        printf("═══════════════════════════════════════════\n");
        show_turn_status(context);
        display_skills(player);
        
        // 显示回合菜单并处理选择
        int choice;
        bool valid_choice = false;
        
        while (!valid_choice) {
            printf("\n选择行动 (1-6):\n");
            printf("  1-4: 选择技能\n");
            
            // 根据状态显示道具选项
            if (context->has_used_skill_this_turn || context->has_used_item_this_turn) {
                printf("  5: 使用道具 [已禁用]\n");
            } else {
                printf("  5: 使用道具\n");
            }
            
            printf("  6: 尝试逃跑\n");
            printf("\n请选择 (1-6): ");
            fflush(stdout);
            
            if (scanf("%d", &choice) != 1) {
                clear_input_buffer();
                printf("❌ 无效输入，请输入数字 1-6。\n");
                continue;
            }
            clear_input_buffer();
            
            if (choice < 1 || choice > 6) {
                printf("❌ 请选择 1-6 之间的数字。\n");
                continue;
            }
            
            // 验证道具选项是否可用
            if (choice == 5) {
                if (context->has_used_skill_this_turn) {
                    printf("❌ 已使用技能，本回合不能再使用道具！\n");
                    printf("   规则：只能先用道具，再用技能。\n");
                    printf("按Enter继续...");
                    getchar();
                    continue;
                }
                if (context->has_used_item_this_turn) {
                    printf("❌ 本回合已经使用过道具了！\n");
                    printf("按Enter继续...");
                    getchar();
                    continue;
                }
            }
            
            valid_choice = true;
        }
        
        switch (choice) {
            case 1:
            case 2:
            case 3:
            case 4: {
                // 技能选择已在菜单中验证，这里只需执行
                Skill* skill = &player->all_skill[player->carried_skill[choice - 1]];
                
                // 再次确认PP充足（防御状态改变）
                if (skill->current_pp <= 0) {
                    printf("❌ %s 的PP不足！\n", skill->skill_name);
                    printf("按Enter返回菜单...");
                    getchar();
                    turn_finished = false;
                    break;
                }
                
                skill->current_pp--;
                
                // 检查命中
                if (!hit_check(enemy->dodge, skill->accuracy)) {
                    printf("❌ %s 使用了 %s... 但是Miss了！\n", player->name, skill->skill_name);
                    context->has_used_skill_this_turn = true;
                    break;
                }
                
                // 计算伤害
                int damage = calc_damage(player, enemy, choice - 1);
                enemy->current_hp -= damage;
                
                printf("✓ %s 使用了 %s！对 %s 造成了 %d 点伤害！\n",
                       player->name,
                       skill->skill_name,
                       enemy->name,
                       damage);
                
                // 检查暴击
                if (crit_check(skill->crit)) {
                    printf("暴击！\n");
                }
                
                // 检查属性克制
                double multiplier = get_type_multiplier(player->type, enemy->type);
                if (multiplier > 1.0) {
                    printf("效果拔群！\n");
                } else if (multiplier < 1.0) {
                    printf("效果不佳...\n");
                }
                
                // 应用状态效果
                if (skill->effect != none) {
                    enemy->current_state = skill->effect;
                    printf("%s 被附加了状态效果！\n", enemy->name);
                }
                
                context->has_used_skill_this_turn = true;
                break;
            }
            case 5: {
                // 显示背包菜单选择道具
                bool item_used = show_battle_bag_menu(context, player);
                
                if (item_used) {
                    // 道具使用成功，标记已使用过道具
                    context->has_used_item_this_turn = true;
                    
                    // 询问是否继续使用技能
                    printf("\n═══════════════════════════════════════════\n");
                    printf("是否要继续使用技能？ (y/n): ");
                    fflush(stdout);
                    
                    char continue_choice;
                    scanf("%c", &continue_choice);
                    clear_input_buffer();
                    
                    if (continue_choice == 'y' || continue_choice == 'Y') {
                        // 用户选择继续使用技能，返回外层循环继续选择技能
                        continue;
                    } else {
                        // 用户选择不使用技能，结束本回合
                        printf("═══════════════════════════════════════════\n");
                        turn_finished = true;
                    }
                }
                // 用户取消了道具使用，返回菜单继续选择其他操作
                break;
            }
            case 6: {
                // 尝试逃跑
                if (try_to_run(player, enemy)) {
                    printf("\n✓ %s 成功逃离了战斗！\n", player->name);
                    return BATTLE_RUN;
                } else {
                    printf("\n❌ %s 逃跑失败！\n", player->name);
                    turn_finished = true;
                }
                break;
            }
        }
        
        // 统一处理回合结束逻辑
        if (!turn_finished) {
            if (context->has_used_skill_this_turn) {
                // 使用了技能后，回合自动结束
                turn_finished = true;
                printf("\n═══════════════════════════════════════════\n");
                printf("回合结束，按Enter继续敌方回合...");
                clear_input_buffer();
                getchar();
            } else if (context->has_used_item_this_turn && !context->has_used_skill_this_turn) {
                // 仅使用了道具但未使用技能，按Enter继续敌方回合
                printf("\n═══════════════════════════════════════════\n");
                printf("按Enter继续敌方回合...");
                clear_input_buffer();
                getchar();
                turn_finished = true;
            }
        }
        
        // 检查战斗是否结束
        BattleResult result = check_battle_end(context, player, enemy);
        if (result != BATTLE_ONGOING) {
            return result;
        }
    }
    
    return BATTLE_ONGOING;
}

//敌人回合（简单AI）//
void enemy_turn(Pokemondata* player, Pokemondata* enemy) {
    printf("\n=== 敌方回合 ===\n");
    
    // 简单AI：随机选择一个有PP的技能
    int available_skills[4];
    int available_count = 0;
    
    for (int i = 0; i < 4; i++) {
        int si = enemy->carried_skill[i];
        if (enemy->all_skill[si].current_pp > 0) {
            available_skills[available_count] = i;
            available_count++;
        }
    }
    
    if (available_count == 0) {
        printf("%s 没有可用的技能了！\n", enemy->name);
        return;
    }
    
    int choice = available_skills[rand() % available_count];
    Skill* skill = &enemy->all_skill[enemy->carried_skill[choice]];

    skill->current_pp--;
    
    // 检查命中
    if (!hit_check(player->dodge, skill->accuracy)) {
        printf("%s 使用了 %s... 但是Miss了！\n", enemy->name, skill->skill_name);
        return;
    }
    
    // 计算伤害
    int damage = calc_damage(enemy, player, choice);
    player->current_hp -= damage;
    
    printf("%s 使用了 %s！对 %s 造成了 %d 点伤害！\n",
           enemy->name,
           skill->skill_name,
           player->name,
           damage);
    
    // 检查暴击
    if (crit_check(skill->crit)) {
        printf("暴击！\n");
    }
    
    // 检查属性克制
    double multiplier = get_type_multiplier(enemy->type, player->type);
    if (multiplier > 1.0) {
        printf("效果拔群！\n");
    } else if (multiplier < 1.0) {
        printf("效果不佳...\n");
    }
    
    // 应用状态效果
    if (skill->effect != none) {
        player->current_state = skill->effect;
        printf("%s 被附加了状态效果！\n", player->name);
    }
}



//应用回合结束效果//
void apply_turn_end_effects(Pokemondata* player, Pokemondata* enemy) {
    // 应用状态效果
    apply_status_effect(player);
    apply_status_effect(enemy);
    
    // 恢复一些状态
    recover_frozen(player);
    recover_frozen(enemy);
}

// 准备战斗：初始化宝可梦数据库、玩家/敌方数据和背包
void prepare_battle(gameContext* context, Pokemondata* player, Pokemondata* enemy) {
    if (context == NULL || player == NULL || enemy == NULL) return;

    // 初始化宝可梦数据库
    initialize_pokemons(context);

    // 从背包中拷贝选定的宝可梦
    if (context->backpackData.pokemon_count > 0 &&
        context->backpackData.chosen_pokemon_index >= 0 &&
        context->backpackData.chosen_pokemon_index < context->backpackData.pokemon_count) {
        *player = context->backpackData.pokemons[context->backpackData.chosen_pokemon_index];
    } else if (context->database_initialized && context->pokemon_database[0] != NULL) {
        *player = *context->pokemon_database[0];
    } else {
        memset(player, 0, sizeof(Pokemondata));
    }

    // 根据opponent类型选择敌方宝可梦
    if (context->database_initialized) {
        int enemy_index = 0; // 默认索引
        
        switch (context->opponent) {
            case COMMON:
                {
                    int max_index = POKEMON_DATABASE_SIZE - 4; // 排除最后3个
                    if (max_index < 0) max_index = 0;
                    enemy_index = my_rand_range(0, max_index);
                    // 确保选中的宝可梦存在
                    while (context->pokemon_database[enemy_index] == NULL && enemy_index > 0) {
                        enemy_index--;
                    }
                }
                break;
            case DAMU:
                // 大木博士（初期战斗）：妙蛙花 (index 1)
                enemy_index = 1;
                break;
            case DAMU_MAX:
                // 大木博士（最终战斗）：宝石海星 (index 17)
                enemy_index = 17;
                break;
            case CATERPIE:
                // 绿毛虫剧情：绿毛虫 (index 18)
                enemy_index = 18;
                break;
            case XIAOGANG:
                // 小刚道馆主：大岩蛇 (index 16)
                enemy_index = 16;
                break;
            case ROCKET:
                // 火箭队干部：拉达 (index 15)
                enemy_index = 15;
                break;
            default:
                enemy_index = 0;
                break;
        }
        
        // 设置敌方宝可梦
        if (context->pokemon_database[enemy_index] != NULL) {
            *enemy = *context->pokemon_database[enemy_index];
        } else if (context->pokemon_database[0] != NULL) {
            *enemy = *context->pokemon_database[0];
        } else {
            memset(enemy, 0, sizeof(Pokemondata));
        }
        
        // 根据对手类型和玩家进度调整敌方宝可梦等级
        int target_level = 1;
        switch (context->opponent) {
            case COMMON:
                {
                    int base_level = context->stage + 1;
                    int level_range = 2;
                    target_level = my_rand_range(base_level, base_level + level_range);
                    if (target_level < 1) target_level = 1;
                    if (target_level > 10) target_level = 10;
                }
                break;
            case CATERPIE:
                // 绿毛虫剧情（Stage 1）：等级1-2
                target_level = my_rand_range(1, 2);
                break;
            case DAMU:
                // 大木博士初期战斗：等级3
                target_level = 3;
                break;
            case XIAOGANG:
                // 小刚道馆主（Stage 2）：等级4
                target_level = 4;
                break;
            case ROCKET:
                // 火箭队干部（Stage 3）：等级5
                target_level = 5;
                break;
            case DAMU_MAX:
                // 大木博士最终战斗（Stage 4）：等级7
                target_level = 7;
                break;
            default:
                target_level = 1;
                break;
        }
        
        // 应用等级调整（从等级1升级到目标等级）
        enemy->level = 1;
        enemy->current_xp = 0;
        for (int i = 1; i < target_level; i++) {
            enemy->current_xp = enemy->max_xp; 
            try_update_pokemon(enemy);
        }
        enemy->current_xp = 0; // 重置经验
        
    } else {
        memset(enemy, 0, sizeof(Pokemondata));
    }
}

// 处理战斗结果并返回下一个游戏状态
GameState handle_battle_result(gameContext* context, BattleResult result) {
    SpecialOpponent opponent = context->opponent;
    switch (result) {
        case BATTLE_WIN:
            if (opponent == XIAOGANG) return story_3(context, 1);
            else if (opponent == ROCKET) return story_4(context, 1);
            else if (opponent == CATERPIE) return story_2(context, 1);
            else if (opponent == DAMU_MAX) return stage_5(context, 1);
            else if (opponent == DAMU) return stage_1(context, 1);
            else return EXPLORE_MAP;
        case BATTLE_LOSE:
            if (opponent == XIAOGANG) return story_3(context, 2);
            else if (opponent == ROCKET) return story_4(context, 2);
            else if (opponent == CATERPIE) return story_2(context, 2);
            else if (opponent == DAMU_MAX) return stage_5(context, 2);
            else if (opponent == DAMU) return stage_1(context, 2);
            else return EXPLORE_MAP;
        case BATTLE_RUN:
            return EXPLORE_MAP;
        case BATTLE_CAPTURE:
            return EXPLORE_MAP;
        default:
            return EXPLORE_MAP;
    }
}

//主战斗循环（改为值传递入参：battle_loop 现在接收宝可梦数据副本，函数内部仍使用局部指针）//
BattleResult battle_loop(gameContext* context, Pokemondata player_data, Pokemondata enemy_data) {
    // 在函数内部使用指向局部副本的指针以保持与原实现兼容
    Pokemondata* player = &player_data;
    Pokemondata* enemy = &enemy_data;

    printf("=== 宝可梦对战开始！ ===\n");
    printf("%s VS %s\n\n", player->name, enemy->name);

    bool battle_ended = false;
    BattleResult battle_result = BATTLE_ONGOING;

    // 重置回合状态
    context->has_used_item_this_turn = false;
    context->has_used_skill_this_turn = false;

    while (!battle_ended) {
        clear_screen();
        // 显示状态
        display_pokemon_status(player, true);
        display_pokemon_status(enemy, false);
        
        // 判断先手
        Pokemondata* first = get_first_attacker(player, enemy);
        Pokemondata* second = (first == player) ? enemy : player;
        
        // 第一只宝可梦行动
        if (first == player) {
            battle_result = player_turn(context, player, enemy);
        } else {
            enemy_turn(player, enemy);
            printf("\n");
            wait_for_enter();
        }
        
        // 检查战斗是否结束
        battle_ended = check_battle_end(context, player, enemy) != BATTLE_ONGOING;
        if (battle_ended) break;
        
        // 第二只宝可梦行动
        if (second == player) {
            battle_result = player_turn(context, player, enemy);
        } else {
            enemy_turn(player, enemy);
            printf("\n");
            wait_for_enter();
        }
        
        // 检查战斗是否结束
        battle_ended = check_battle_end(context, player, enemy) != BATTLE_ONGOING;
        if (battle_ended) break;
        
        // 应用回合结束效果
        apply_turn_end_effects(player, enemy);
        
        // 再次检查战斗是否结束
        battle_ended = check_battle_end(context, player, enemy) != BATTLE_ONGOING;

         if (!battle_ended) {
            wait_for_enter(); // 添加这里：战斗未结束时等待
        }
    }
    
    // 战斗结束后的处理
    switch (battle_result) {
        case BATTLE_WIN:
            printf("\n战斗胜利！\n");
            return BATTLE_WIN;
        case BATTLE_LOSE:
            printf("\n战斗失败！\n");
            return BATTLE_LOSE;
        case BATTLE_RUN:
            printf("\n成功逃跑！\n");
            return BATTLE_RUN;
        case BATTLE_CAPTURE:
            printf("\n成功捕捉！\n");
            return BATTLE_CAPTURE;
        case BATTLE_ONGOING:
            break;;
    }
}

//主菜单//
void main_menu() {
    printf("=== 宝可梦战斗系统 ===\n");
    printf("1. 开始战斗\n");
    printf("2. 查看宝可梦状态\n");
    printf("3. 查看背包\n");
    printf("请选择: ");
    fflush(stdout);
}

// 主函数//
GameState fight(gameContext *context)
{
    setUTF8();         // 设置UTF-8编码
    srand(time(NULL)); // 初始化随机种子

    Pokemondata player_pokemon;
    Pokemondata enemy_pokemon;
    prepare_battle(context, &player_pokemon, &enemy_pokemon);

    while (1)
    {
        char choice;
        clear_screen();
        main_menu();
        
        while (_kbhit()) _getch();
        choice = _getch();

        switch (choice)
        {
        case '1':
            // 重置战斗状态
            player_pokemon.current_hp = player_pokemon.max_hp;
            enemy_pokemon.current_hp = enemy_pokemon.max_hp;
            player_pokemon.current_state = none;
            enemy_pokemon.current_state = none;

            // 重置所有技能的PP
            for (int i = 0; i < 4; i++)
            {
                int psi = player_pokemon.carried_skill[i];
                int esi = enemy_pokemon.carried_skill[i];
                player_pokemon.all_skill[psi].current_pp = player_pokemon.all_skill[psi].max_pp;
                enemy_pokemon.all_skill[esi].current_pp = enemy_pokemon.all_skill[esi].max_pp;
            }

            clear_screen();
            // 开始战斗（传入宝可梦副本，战斗内的修改不会直接改变存档内的数据）
            BattleResult battle_result = battle_loop(context, player_pokemon, enemy_pokemon);

            wait_for_enter();
            GameState next = handle_battle_result(context, battle_result);
            system("cls");
            return next; // 战斗结束后返回

        case '2':
            clear_screen();
            display_pokemon_status(&player_pokemon, true);
            display_pokemon_status(&enemy_pokemon, false);
            wait_for_enter();
            break; // 继续循环

        case '3':
            clear_screen();
            while (1) {
                system("cls");
                printf("                    背包\n");
                printf("              1.查看拥有的道具\n");
                printf("              2.查看拥有的宝可梦\n");
                printf("              3.查看商店\n");
                printf("              4.返回\n");
                char bag_choice;
                while (1)
                {
                    bag_choice = _getch();
                    if (bag_choice == '1' || bag_choice == '2' || bag_choice == '3' || bag_choice == '4')
                        break;
                }
                if (bag_choice == '1')
                {
                    choose_and_check_item(context);
                }
                else if (bag_choice == '2')
                {
                    choose_and_interact_with_pokemons(context);
                }
                else if (bag_choice == '3')
                {
                    while (1)
                    {
                        GameState state = shop_state(context);
                        if (state == EXPLORE_MAP)
                            break;
                    }
                }
                else if (bag_choice == '4')
                {
                    break;
                }
            }
            break; // 继续循环

        default:
            printf("无效选择，请选择1-3！\n");
            wait_for_enter();
            break; // 继续循环
        }
    }
}