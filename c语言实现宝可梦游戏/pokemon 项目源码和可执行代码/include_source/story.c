#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include<conio.h>
#include "../include/main.h"
#include <windows.h>
void print_story_text(const char* text) {
    if (text == NULL || strlen(text) == 0) {
        return;
    }
    printf("\n");
    for (int i = 0; text[i] != '\0'; i++) {
        printf("%c", text[i]);
        fflush(stdout);
        Sleep(30);
    }
    printf("\n\n");
}
void add_player_money(gameContext* context, int amount) {
    context->backpackData.player_money += amount;
}
GameState stage_1(gameContext* context,int battle_result){
        if (battle_result == 1) {
        add_player_money(context,1000000);
        print_story_text("大木博士一脸震惊的望着你,眼睛里充满了不可置信");
        print_story_text("虽然你战胜了大木博士,但是你并不是嗜血之人,放过了他");
        print_story_text("并且,怀着对宝可梦世界的好奇与憧憬,依旧顺着大木博士的指引,踏上了宝可梦的世界");
        print_story_text("🏆 达成成就||念诗成尊||");
        add_player_money(context,1000000);
        context->stage = 1;
        print_story_text("===== Stage 1 完成!前往常青森林踏上你的征程吧! =====\n");
    } else {
        print_story_text("💥 对战失败！请提升实力后再来挑战~");
        print_story_text("ps:不会真有人元婴修为打不过大木博士吧");
    }
    print_story_text("(点击F返回)");
        char f;
        while(1){
            f=_getch();
            if(f=='F'||f=='f'){
                return EXPLORE_MAP;
            }
    }
}
GameState stage_5(gameContext* context,int battle_result){
    if(battle_result==1){
        print_story_text("大木博士一脸恐惧的看着你,一边后退,一边求饶");
        print_story_text("你看着如同丧家之犬的大木,哀叹一声,手起刀落,结束了大木博士罪恶的一生");
        print_story_text("看着被大木博士关押起来的宝可梦,你把他们用精灵球收集了起来,准备送给新的训练家,为他们指引冒险的方向");
        print_story_text("🏆 达成成就||扫黑除恶||");
        print_story_text("The end");
        print_story_text("ps:主线剧情方面已经结束,你可以继续游玩收集增强宝可梦,打造一支属于你的宝可梦最强战队");
        context->stage=5;
    }
    else {
        print_story_text("你终究是棋差一招,大木博士阴冷一笑,虽然放过了你,但是让你成为了火箭队的一员,替他卖力");
    }
        print_story_text("(点击F返回)");
        char f;
        while(1){
            f=_getch();
            if(f=='F'||f=='f'){
                return EXPLORE_MAP;
            }
    }
}
void Dr_DaMu(gameContext* context) {
    if (context->stage==1||context->stage==2||context->stage==3) {
        print_story_text("大木博士:去挑战这个新的宝可梦世界吧!!!");
    }
    else if(context->stage == 0){
        print_story_text("🎐 大木博士:欢迎来到关都地区!我是大木博士,是冒险家的向导哦");
        print_story_text("大木博士:emm...鉴于你还没有宝可梦,我会送你一只初始宝可梦作为伙伴!");
        printf("1.🔥 小火龙（火属性）\n2.💧 杰尼龟（水属性）\n3.🌱 妙蛙种子（草属性）\n(0.不相信大木口中的宝可梦)");
        char choice;
        while (1) {
            choice = _getch();
            if(choice == '1'||choice=='2'||choice=='3'||choice=='0')break;
        }
        switch (choice) {
            case '1':
                add_player_pokemon(context,context->pokemon_database[4]);
                print_story_text("大木博士:哇哦!你选择了小火龙！它会成为你可靠的伙伴~(小心小火龙点燃森林牢底坐穿)");
                add_player_money(context,500);
                print_story_text("大木博士:给你500块钱,置办一些道具,前往常青森林去尝试一下挑战野生宝可梦吧!");
                print_story_text("===== Stage 1 完成！前往常青森林开启下一段冒险吧～ =====\n");
                break;
            case '2':
                add_player_pokemon(context,context->pokemon_database[6]);
                print_story_text("大木博士:哇哦!你选择了杰尼龟！它会成为你可靠的伙伴~(龟人的最佳搭档)");
                print_story_text("大木博士:给你500块钱,置办一些道具,前往常青森林去尝试一下挑战野生宝可梦吧!");
                print_story_text("===== Stage 1 完成！前往常青森林开启下一段冒险吧～ =====\n");
                break;
            case '3':
                add_player_pokemon(context,context->pokemon_database[0]);
                print_story_text("大木博士:哇哦!你选择了妙蛙种子！它会成为你可靠的伙伴~(下毒的好手)");
                print_story_text("大木博士:给你500块钱,置办一些道具,前往常青森林去尝试一下挑战野生宝可梦吧!");
                print_story_text("===== Stage 1 完成！前往常青森林开启下一段冒险吧～ =====\n");
                break;
            case '0':
                add_player_pokemon(context,context->pokemon_database[18]);
                print_story_text("我一把抓住了精灵球,顷刻炼化");
                print_story_text("我:小小宝可梦,请走好");
                print_story_text("大木博士(十分惊愕):训练家!!你到底干了什么!!没有自己的宝可梦,你如何抗衡火箭队");
                print_story_text("我(淡笑一声):很简单,我成尊不就是了");
                print_story_text("说完我的气息终于不再掩饰显露而出,元!婴!巅!峰!");
                print_story_text("我:早岁已知世事艰,仍许飞鸿荡云间.一路寒风身如絮,命海沉浮客独行.千磨万击心铸铁,殚精竭虑铸一剑.今朝剑指叠云处,炼蛊炼人还炼天.哈哈哈哈哈");
                print_story_text("大木博士:你....你真是自取灭亡!就让我用我的宝可梦来打醒你吧!");
                context->opponent = DAMU;
                context->mapExplorationData.exit = true;
                context->mapExplorationData.nextState = FIGHT;
                break;
            }

            context->stage = 1;
            _getch();
            system("cls");
            return;
    }
    else if(context->stage==4){
        print_story_text("大木博士:哦?你居然活着回来了");
        print_story_text("大木博士:看你的样子,也应该知道真相了");
        print_story_text("大木博士:我给予你们初始宝可梦,不是为了让你们成为什么狗屁训练师,而是为我搜集宝可梦");
        print_story_text("大木博士:从本质上来说,你和那些火箭队没什么区别,他们也不过是激励你们替我收集更多宝可梦来挑战而已");
        print_story_text("大木博士:你很强");
        print_story_text("大木博士:不过很可惜,那也没什么用,因为你马上就会输在我手里了");
        print_story_text("大木博士:不过我可以给你一个机会,继续为我效力,替我收集宝可梦,我可以放你一马,怎么样");
        print_story_text("我:");
        print_story_text("1:什么狗屁大木博士,看招");
        print_story_text("2:好,我们合作(忍辱负重)");
        char choice;
        while (1)
        {
            choice = _getch();
            if (choice == '1' || choice == '2')
                break;
        }
        if (choice == '1')
        {
            print_story_text("大木博士:冥顽不灵,就让你看看我真正的实力吧!");
            context->opponent = DAMU_MAX;
            context->mapExplorationData.exit = true;
            context->mapExplorationData.nextState = FIGHT;
        }
        else if (choice == '2')
        {
            print_story_text("大木博士:桀桀,这才对嘛,现在你就是火箭队的第一干部了");
        }
    }
    else if (context->stage == 5)
    {
        print_story_text("大木博士:(死了)");
    }
    _getch();
    system("cls");
    return;
}

void Event_Caterpie(gameContext* context) {
    if(context->stage==1){
        print_story_text("突然!头顶上掉下来了一只野生绿毛虫");
        print_story_text("我:");
        print_story_text("1:区区小虫,有何可惧(进入战斗)");
        print_story_text("2:啊啊啊啊啊!是虫虫啊,我好害怕(逃跑)");
    
        char choice;
        while (1) {
            choice = _getch();
            if(choice == '1'||choice=='2')break;
        }
    
        if (choice =='1') {
            print_story_text("⚔️你发起了对战!野生绿毛虫");
            context->opponent = CATERPIE;
            context->mapExplorationData.exit = true;
            context->mapExplorationData.nextState = FIGHT;
        } else {
            print_story_text("我:咦~大虫子太恐怖了,打不过打不过,我溜了");
            print_story_text("🚶 你夹着尾巴逃走了");
            print_story_text("绿毛虫:桀桀桀,这就逃走了");
            print_story_text("🏆 达成成就||不称职的训练师||");
        }
    }
    else
    {
        print_story_text("熟悉的绿毛虫掉了下来,看到你,却是什么都不敢说逃走了");
    }
    _getch();
    system("cls");
    return;
}

GameState story_2(gameContext* context,int battle_result) {
    if (battle_result == 1) {
        add_player_money(context,1000);
        print_story_text("🏆 对战胜利!获得1000金币");
        print_story_text("🏆 达成成就||牛刀小试||");
        context->stage = 2;
        print_story_text("===== Stage 1 完成！前往尼比市挑战道馆吧~ =====\n");
    } else {
        print_story_text("💥 对战失败!可以通过捕捉野生宝可梦,挑战宝可梦来升级进化,商城购买道具来提升自己的实力");
    }
        print_story_text("(点击F返回)");
        char f;
        while(1){
            f=_getch();
            if(f=='F'||f=='f'){
                return EXPLORE_MAP;
            }
        }
}

void XiaoGang(gameContext* context) {
    if(context->stage==0){
        print_story_text("小刚:hi,你好呀冒险者,还没有宝可梦吗,可以找大木博士领取你的宝可梦哦");
    }
    if(context->stage==1){
        print_story_text("小刚:你现在还太弱小了,继续提升自己的实力吧");
    }
    if(context->stage==2){
        print_story_text("小刚:嗯?强大的训练家,你要挑战道馆吗?");
        print_story_text("我:");
        print_story_text("1:哼哼,我已经准备好拿下你了");
        print_story_text("2:嗯....这个....那个...我还没有准备好");
        char choice;
        while(1){
            choice=_getch();
            if(choice=='1'||choice=='2')break;
    }
    if(choice=='1'){
        context->opponent = XIAOGANG;
        context->mapExplorationData.exit=true;
        context->mapExplorationData.nextState=FIGHT;
    }
    else if (choice == '2')
    {
        print_story_text("小刚:还没准备好吗.....下次再来挑战我吧");
        _getch();
        system("cls");
        return;
    }
    }
    else if (context->stage == 3)
    {

        print_story_text("小刚:英雄!就靠你战胜火箭队了");
        _getch();
        system("cls");
        return;
    }
    else if (context->stage == 4)
    {
        print_story_text("小刚:奇怪...明明火箭队被干掉了,消失的宝可梦还是没有找到");
        _getch();
        system("cls");
        return;
    }
    else if (context->stage == 5)
    {

        print_story_text("小刚:大木博士...居然是这种人吗....伟大的训练家,还好有您");
        _getch();
        system("cls");
        return;
    }
    _getch();
    system("cls");
    return;
}

GameState story_3(gameContext* context,int battle_result) {
    if (battle_result == 1) {
        add_player_money(context,5000);
        print_story_text("🏆 对战胜利!你获得了5000元奖励~");
        print_story_text("🏆 小刚：不错的训练家!你说多年来唯一一个通过道馆试炼的训练师");
        print_story_text("你对此表示不解,宝可梦道馆不应该是十分容易挑战的吗?");
        print_story_text("小刚告诉你,在以前,宝可梦王国鼎盛,但是自从火箭队兴起,宝可梦被大肆捕捉,现在宝可梦的数量早已十不存一,而这里是最后一家道馆了");
        print_story_text("或许是看上了你的强大,小刚请求你去挑战火箭队,解放那些被抓的宝可梦");
        print_story_text("你当然同意,毕竟在一路上你也见证了宝可梦对人们的益处");
        context->stage = 3;
        print_story_text("===== Stage 3 完成！前往华莱市开启与火箭队的决斗吧~ =====\n");
        print_story_text("ps:可以先前往常青森林捕捉宝可梦来提升自己的实力");
        print_story_text("(点击F返回)");
        char f;
        while(1){
            f=_getch();
            if(f=='F'||f=='f'){
                return EXPLORE_MAP;
            }
        }
    } else {
        print_story_text("💥 对战失败!小刚:年轻的训练师,你还不够强,再去练练吧");
        print_story_text("(点击F返回)");
        char f;
        while(1){
            f=_getch();
            if(f=='F'||f=='f'){
                return EXPLORE_MAP;
            }
        }
    }
}

void Rocket(gameContext* context) {
    if (context->stage<3) {
        print_story_text("火箭队干部:你是什么垃圾,快走开,杀你都脏了我的手");
    }
    if (context->stage==4) {
        print_story_text("火箭队干部:哼,大木老大是不会放过你的");
    }
    
   if(context->stage==3){
    print_story_text("看到火箭队干部正在暴力抓捕宝可梦,你怒从心生,上去阻止");
    print_story_text("火箭队干部(暴怒):小子,你干什么");
    print_story_text("我:");
    print_story_text("1:简单,看你们不顺眼,来阻止你们喽");
    print_story_text("2:啊..哈哈..对不起大哥,我手滑了");
    char choice;
        while(1){
            choice=_getch();
            if(choice=='1'||choice=='2')break;
    }
    if (choice == '1') {
        print_story_text("🐍 火箭队干部(生气):哪来的小子!大言不惭,让我来干掉你");
        context->opponent = ROCKET;
        context->mapExplorationData.exit = true;
        context->mapExplorationData.nextState = FIGHT;
    }
    else if (choice == '2')
    {
        print_story_text("火箭队干部:多管闲事(踹了你一脚)");
    }
   }
   _getch();
   system("cls");
   return;
}

GameState story_4(gameContext *context, int battle_result)
{
    if (battle_result == 1) {
        print_story_text("💥 惊天一击!火箭队干部被击倒在地");
        print_story_text("✨ 刚刚在被抓的皮卡丘扑入你的怀中,用脸颊蹭着你,满是感激");
        print_story_text("旁边紧闭的房门缓缓打开,露出来一双双眼睛");
        print_story_text("人们对你击败了火箭队干部感到震惊");
        print_story_text("火箭队干部趴倒在地,浑身无力,凶狠的看着你");
        print_story_text("火箭队干部:可....可恶...,你以为..这就结束了吗...我..的老大...大木博士..一定会为我报仇的");
        print_story_text("大木博士吗");
        print_story_text("怪不得");
        print_story_text("怎么会有人免费送人宝可梦,教会别人战斗,原来是他!");
        print_story_text("地上趴着的火箭队干部没了生机,但是你的心里却不平静");
        print_story_text("你下定决心,要铲除最后的毒瘤,迎关东一个光明");
        add_player_money(context,5000);
        print_story_text("🏆 奖励5000元");
        context->stage = 4;
        print_story_text("===== 🔥 Stage 4完成!=====");
        print_story_text("🏆 达成成就||火箭队终结者||");
    } else {
        print_story_text("💀 火箭队干部带着被抓的宝可梦扬长而去,你被打倒在地,陪伴你的只有无尽的嘲笑......");
    }
    print_story_text("(点击F返回)");
        char f;
        while(1){
            f=_getch();
            if(f=='F'||f=='f'){
                return EXPLORE_MAP;
            }
    }
}
GameState show_story_progress(gameContext* context) {
    print_story_text("===== 剧情进度 =====\n");
    char stage_status[5][20] = {"未完成", "未完成", "未完成", "未完成", "未完成"};
    for (int i = 0; i < context->stage; i++) {
        strcpy(stage_status[i], "已完成");
    }
    printf("Stage 1:初始启程 → %s\n", stage_status[0]);
    printf("Stage 2:森林探索 → %s\n", stage_status[1]);
    printf("Stage 3:道馆挑战 → %s\n", stage_status[2]);
    printf("Stage 4:火箭队决战 → %s\n", stage_status[3]);
    printf("Stage 5:大木终局 → %s\n", stage_status[4]);
    print_story_text("====================\n");
    print_story_text("(点击F返回)");
    char f;
    while(1){
        f=_getch();
        if(f=='F'||f=='f'){
            Transform(EXPLORE_MAP, context);
            return EXPLORE_MAP;
        }
    }
}