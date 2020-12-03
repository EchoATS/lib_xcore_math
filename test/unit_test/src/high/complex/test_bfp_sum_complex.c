
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "xs3_math.h"

#include "../../tst_common.h"

#include "unity.h"

#if DEBUG_ON || 0
#undef DEBUG_ON
#define DEBUG_ON    (1)
#endif


#define REPS        IF_QUICK_TEST(10, 100)
#define MAX_LEN     1024 


static unsigned seed = 666;





void test_bfp_sum_complex_s16()
{
    PRINTF("%s...\t(random vectors)\n", __func__);

    seed = 67765974;

    int16_t WORD_ALIGNED B_real[MAX_LEN];
    int16_t WORD_ALIGNED B_imag[MAX_LEN];
    
    bfp_complex_s16_t B;


    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        bfp_init_vect_complex_s16(&B, B_real, B_imag, 
            pseudo_rand_int(&seed, -100, 100),
            pseudo_rand_uint(&seed, 0, MAX_LEN+1), 0);

        B.hr = pseudo_rand_uint(&seed, 0, 15);

        complex_s32_t expected = {0, 0};

        for(int i = 0; i < B.length; i++){
            B.real[i] = pseudo_rand_int16(&seed) >> B.hr;
            B.imag[i] = pseudo_rand_int16(&seed) >> B.hr;

            expected.re += B.real[i];
            expected.im += B.imag[i];
        }

        bfp_headroom_vect_complex_s16(&B);

        complex_s32_t result = bfp_sum_complex_s16(&B);

        TEST_ASSERT_EQUAL_INT32(expected.re, result.re);
        TEST_ASSERT_EQUAL_INT32(expected.im, result.im);
    }
}





void test_bfp_sum_complex_s32()
{
    PRINTF("%s...\t(random vectors)\n", __func__);

    seed = 67765974;

    complex_s32_t B_data[MAX_LEN];
    
    bfp_complex_s32_t B;


    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        bfp_init_vect_complex_s32(&B, B_data, pseudo_rand_int(&seed, -100, 100),
            pseudo_rand_uint(&seed, 0, MAX_LEN+1), 0);

        B.hr = pseudo_rand_uint(&seed, 0, 6);

        complex_double_t expected = {0, 0};

        for(int i = 0; i < B.length; i++){

            // Use only positive values so that they tend not to cancel out (which they might in practice)
            B.data[i].re = pseudo_rand_int(&seed, 0, INT32_MAX) >> B.hr;
            B.data[i].im = pseudo_rand_int(&seed, 0, INT32_MAX) >> B.hr;

            expected.re += ldexp(B.data[i].re, B.exp);
            expected.im += ldexp(B.data[i].im, B.exp);
        }

        exponent_t a_exp;

        bfp_headroom_vect_complex_s32(&B);

        complex_s64_t result = bfp_sum_complex_s32(&a_exp, &B);

        TEST_ASSERT_GREATER_OR_EQUAL(B.exp, a_exp);

        complex_double_t fl = { ldexp(result.re, a_exp), ldexp(result.im, a_exp) };

        complex_double_t ds = { 
            fabs((expected.re - fl.re) / expected.re),
            fabs((expected.im - fl.im) / expected.im) };

        TEST_ASSERT(ds.re <= ldexp(1, -20));
        TEST_ASSERT(ds.im <= ldexp(1, -20));
    }
}




void test_bfp_sum_complex()
{
    SET_TEST_FILE();
    RUN_TEST(test_bfp_sum_complex_s16);
    RUN_TEST(test_bfp_sum_complex_s32);
}