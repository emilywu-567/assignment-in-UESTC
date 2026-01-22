/* my_random.c */
#include "../include/my_random.h"

/* 内部状态，保持四个 64 位整数 */
static uint64_t s[4];

/* 辅助函数：左循环移位 */
static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

/* 初始化种子。简单的 SplitMix64 算法用于初始化 xoshiro 的状态 */
void my_srand(uint64_t seed) {
    // 确保种子不全为0
    for(int i = 0; i < 4; i++) {
        uint64_t z = (seed += 0x9e3779b97f4a7c15);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
        z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
        s[i] = z ^ (z >> 31);
    }
}

/* xoshiro256** 算法核心实现 */
uint64_t my_rand_uint64(void) {
    const uint64_t result = rotl(s[1] * 5, 7) * 9;

    const uint64_t t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;

    s[3] = rotl(s[3], 45);

    return result;
}

/* 生成 [0, 1) 的浮点数 */
double my_rand_double(void) {
    // 将 64 位整数的高 53 位转换由于 double 的精度限制
    return (my_rand_uint64() >> 11) * 0x1.0p-53;
}

/* 生成 [min, max] 范围内的整数 */
int64_t my_rand_range(int64_t min, int64_t max) {
    uint64_t range = (uint64_t)(max - min + 1);
    // 这里使用了取模，虽然有极其微小的偏差，但对于初学者足够且高效
    return min + (my_rand_uint64() % range);
}
