#ifndef MY_RANDOM_H
#define MY_RANDOM_H

#include <stdint.h>
#include<time.h>

// 初始化随机数生成器（种子）
void my_srand(uint64_t seed);

// 获取一个 64 位的随机整数
uint64_t my_rand_uint64(void);

// 获取一个 [0, 1) 范围内的双精度浮点数
double my_rand_double(void);

// 获取一个范围在 [min, max] 之间的整数
int64_t my_rand_range(int64_t min, int64_t max);

#endif
