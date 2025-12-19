
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

static uint64_t g_rand_seed = 1;

void my_srand(uint64_t n)
{
    g_rand_seed = n == 0 ? 1 : n;
}

uint64_t my_rand64()
{
    /* 
     * Xorshift RNG [1] + multiply
     * [1]: https://www.jstatsoft.org/article/view/v008i14/916 */
    uint64_t x = g_rand_seed;
    x ^= x >> 12ULL;
    x ^= x << 25ULL;
    x ^= x >> 27ULL;
    g_rand_seed = x;
    return x * 2685821657736338717ULL;
}

static int my_tolower(int ch)
{
    return ch >= 'A' && ch <= 'Z'
        ? ch - 'A' + 'a'
        : ch;
}

static int my_isdigit(int ch)
{
    return ch >= '0' && ch <= '9';
}

void* my_memset(void *b, int c, size_t len)
{
    unsigned char* x = b;
    unsigned char ch = (unsigned char)c;
    for (size_t i = 0; i < len; ++i) {
        x[i] = ch;
    }
    return b;
}

void* my_memcpy(void* restrict dst, void const* restrict src, size_t n)
{
    uint8_t* d = dst;
    uint8_t const* s = src;

    for (size_t i = 0; i < n; ++i) {
        d[i] = s[i];
    }

    return dst;
}

int my_memcmp(void const* s1, void const* s2, size_t n)
{
    /* todo: SIMD optimize or use 64 bit strides initially if optimizer
       doesn't catch this */
    int8_t const* a = s1;
    int8_t const* b = s2;
    for (size_t i = 0; i < n; ++i) {
        int8_t diff = a[i] - b[i];
        if (diff)
            return diff;
    }
    return 0;
}

size_t strlen(char const* s)
{
    size_t n = 0;
    while (s[n++])
        ;
    return n;
}
