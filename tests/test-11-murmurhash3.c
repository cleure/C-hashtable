#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <limits.h>

#include "MurmurHash3.h"

int main(int argc, char **argv)
{
    uint32_t input, hash32;
    uint32_t input128[4], hash128[4];
    uint64_t hash128_64[2];
    uint32_t step = UINT_MAX/500;
    uint32_t seed;
    
    srand(1024);
    seed = rand();
    
    /* Test MurmurHash3_x86_32 */
    for (input = 0; input < UINT_MAX-step; input += step) {
        MurmurHash3_x86_32(&input, sizeof(input), seed, &hash32);
        printf("%u\n", hash32);
    }
    
    /* Test MurmurHash3_x86_128 */
    for (input = 0; input < UINT_MAX-step; input += step) {
        input128[0] = input;
        input128[1] = input * 2;
        input128[2] = input * 2 / 4;
        input128[3] = input * 4;
        
        MurmurHash3_x86_128(&input128, sizeof(input128), seed, &hash128);
        printf("%u %u %u %u\n", hash128[0], hash128[1], hash128[2], hash128[3]);
    }
    
    /* Test MurmurHash3_x86_128_64 */
    for (input = 0; input < UINT_MAX-step; input += step) {
        input128[0] = input;
        input128[1] = input * 2;
        input128[2] = input * 2 / 4;
        input128[3] = input * 4;
        
        MurmurHash3_x64_128(&input128, sizeof(input128), seed, &hash128_64);
        printf("%u %u %u %u\n",
                (uint32_t)(hash128_64[0] >> 32 & 0xffffffff),
                (uint32_t)(hash128_64[0] & 0xffffffff),
                (uint32_t)(hash128_64[1] >> 32 & 0xffffffff),
                (uint32_t)(hash128_64[1] & 0xffffffff));
    }
    
    return 0;
}
