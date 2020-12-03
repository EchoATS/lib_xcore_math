
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "xs3_math.h"

#include "../tst_common.h"

#include "unity.h"

#if DEBUG_ON || 0
#undef DEBUG_ON
#define DEBUG_ON    (1)
#endif


#define REPS        1000
#define MAX_LEN     1024 


static unsigned seed = 666;


static void test_bfp_vect_s16_abs_sum()
{
    PRINTF("%s...\t(random vectors)\n", __func__);

    seed = 67765974;

    int16_t dataB[MAX_LEN];
    bfp_s16_t B;

    B.data = dataB;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        B.length = pseudo_rand_uint(&seed, 1, MAX_LEN+1);
        B.exp = pseudo_rand_int(&seed, -100, 100);
        B.hr = pseudo_rand_uint(&seed, 0, 15);

        for(int i = 0; i < B.length; i++){
            B.data[i] = pseudo_rand_int16(&seed) >> B.hr;
        }

        bfp_vect_s16_headroom(&B);

        int32_t result = bfp_vect_s16_abs_sum(&B);

        int32_t exp = 0;
        for(int i = 0; i < B.length; i++)
            exp += abs(B.data[i]);

        TEST_ASSERT_EQUAL(exp, result);
    }
}

static void test_bfp_vect_s32_abs_sum()
{
    PRINTF("%s...\t(random vectors)\n", __func__);

    seed = 8789;

    int32_t dataB[MAX_LEN];
    bfp_s32_t B;

    B.data = dataB;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        B.length = pseudo_rand_uint(&seed, 1, MAX_LEN+1);
        B.exp = pseudo_rand_int(&seed, -100, 100);
        B.hr = pseudo_rand_uint(&seed, 0, 31);
        
        int64_t sum = 0;

        for(int i = 0; i < B.length; i++){
            B.data[i] = pseudo_rand_int32(&seed) >> B.hr;

            sum += abs( B.data[i] );
        }

        bfp_vect_s32_headroom(&B);

        int64_t result = bfp_vect_s32_abs_sum(&B);

        TEST_ASSERT_EQUAL(sum, result);
    }
}




void test_bfp_abs_sum()
{
    SET_TEST_FILE();
    RUN_TEST(test_bfp_vect_s16_abs_sum);
    RUN_TEST(test_bfp_vect_s32_abs_sum);
}