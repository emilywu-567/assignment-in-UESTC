# 项目构建指南 (How to Build)

本项目包含多个模块文件，需要在 Windows 环境下使用 GCC 联合编译。

## 1. 运行环境
*   **操作系统**: Windows 10 / Windows 11
*   **编译器**: GCC
*   **编辑器**: VSCode

## 2. 准备工作
确保以下源文件都在同一目录include_source下：
*   main.c
*   fight.c
*   mapExploration.c
*   menu.c
*   my_random.c
*   shop_and_backpack.c
*   story.c

确保include_source同级有一个include文件夹，其中存放
*  common.h
*  fight.h
*  main.h
*  mapExploration.h
*  menu.h
*  my_random.h
*  shop_and_backpack.h
*  story.h
同时同级还应该有一个maps文件夹，其中存放：
*  down_map.txt
*  left_map.txt
*  middle_map.txt
*  right_map.txt
*  up_map.txt
最后还有一个同级文件夹saving，存放：
*  saving.dat

## 3. 构建步骤 (Build)

1.  在 VSCode 中打开终端 (Terminal)。
2.  复制并输入以下完整命令进行编译：

    ```bash
    gcc main.c fight.c mapExploration.c menu.c my_random.c shop_and_backpack.c story_3.c -o main.exe
    ```

3.  如果没有报错，输入 `.\main.exe` 运行程序。