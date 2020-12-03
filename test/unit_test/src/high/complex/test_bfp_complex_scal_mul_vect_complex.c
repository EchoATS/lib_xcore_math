
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


#define REPS        IF_QUICK_TEST(100, 100)
#define MAX_LEN     40 


static unsigned seed = 666;


static char msg_buff[200];

#define TEST_ASSERT_EQUAL_MSG(EXPECTED, ACTUAL, EXTRA, LINE_NUM)   do{          \
    if((EXPECTED)!=(ACTUAL)) {                                                  \
      sprintf(msg_buff, "%s (test vector @ rep %u)", (EXTRA), (LINE_NUM));     \
      TEST_ASSERT_EQUAL_MESSAGE((EXPECTED), (ACTUAL), msg_buff);                \
    }} while(0)







void test_bfp_complex_scal_mul_vect_complex_s16()
{
    PRINTF("%s...\n", __func__);

    seed = 0x157BD692;

    struct {
        int16_t real[MAX_LEN];
        int16_t imag[MAX_LEN];
    } A_data, B_data;

    bfp_complex_s16_t A, B;

    struct {
        double real[MAX_LEN];
        double imag[MAX_LEN];
    } Af, Bf;

    struct {
        int16_t real[MAX_LEN];
        int16_t imag[MAX_LEN];
    } expA;


    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        bfp_init_vect_complex_s16(&B, B_data.real, B_data.imag,
            pseudo_rand_int(&seed, -100, 100),
            pseudo_rand_int(&seed, 1, MAX_LEN+1), 0);

        bfp_init_vect_complex_s16(&A, A_data.real, A_data.imag, 0, B.length, 0);

        B.hr = pseudo_rand_uint(&seed, 0, 12);

        const exponent_t c_exp = pseudo_rand_int(&seed, -100, 100);
        const headroom_t c_hr = pseudo_rand_uint(&seed, 0, 12);

        complex_s16_t C = {
            pseudo_rand_int16(&seed) >> c_hr,
            pseudo_rand_int16(&seed) >> c_hr };

        for(int i = 0; i < B.length; i++){
            B.real[i] = pseudo_rand_int16(&seed) >> B.hr;
            B.imag[i] = pseudo_rand_int16(&seed) >> B.hr;

            Af.real[i] = ldexp(B.real[i], B.exp) * ldexp(C.re, c_exp) 
                       - ldexp(B.imag[i], B.exp) * ldexp(C.im, c_exp);
            Af.imag[i] = ldexp(B.real[i], B.exp) * ldexp(C.im, c_exp) 
                       + ldexp(B.imag[i], B.exp) * ldexp(C.re, c_exp);
        }

        bfp_headroom_vect_complex_s16(&B);

        bfp_complex_scal_mul_vect_complex_s16(&A, &B, C, c_exp);
        
        // PRINTF("\t    B.length = %u\n", B.length);
        // PRINTF("\t    B.exp    = %d\n", B.exp);
        // PRINTF("\t    B.hr     = %u\n", B.hr);

        // for(int i = 0; i < B.length; i++){
        //     PRINTF("\t        B.real[% 3d] = % 10d    (0x%04X)\n", i, B.real[i], (unsigned) B.real[i]);
        //     PRINTF("\t        B.imag[% 3d] = % 10d    (0x%04X)\n", i, B.imag[i], (unsigned) B.imag[i]);
        // }

        // PRINTF("\t    C.exp = %d\n", c_exp);
        // PRINTF("\t        C.re  = % 10d    (0x%04X)\n", C.re, (unsigned) C.re);
        // PRINTF("\t        C.im  = % 10d    (0x%04X)\n", C.im, (unsigned) C.im);
        
        
        // PRINTF("\t    A.length = %u\n", A.length);
        // PRINTF("\t    A.exp    = %d\n", A.exp);
        // PRINTF("\t    A.hr     = %u\n", A.hr);

        // for(int i = 0; i < A.length; i++){
        //     PRINTF("\t        A.real[% 3d] = % 10d    (0x%04X)\n", i, A.real[i], (unsigned) A.real[i]);
        //     PRINTF("\t        A.imag[% 3d] = % 10d    (0x%04X)\n", i, A.imag[i], (unsigned) A.imag[i]);
        // }

        test_complex_s16_from_double(expA.real, expA.imag, Af.real, Af.imag, MAX_LEN, A.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_INT16_WITHIN(1, expA.real[i], A.real[i]);
            TEST_ASSERT_INT16_WITHIN(1, expA.imag[i], A.imag[i]);
        }
    }
}






void test_bfp_complex_scal_mul_vect_complex_s32_calc_params()
{
    PRINTF("%s...\n", __func__);

    seed = 0xF9D64BC5;

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);
        
        exponent_t b_exp = pseudo_rand_int(&seed, -30, 30);
        headroom_t b_hr  = pseudo_rand_uint(&seed, 0, 31);
        exponent_t c_exp = pseudo_rand_int(&seed, -30, 30);
        headroom_t c_hr  = pseudo_rand_uint(&seed, 0, 31);

        complex_s32_t WORD_ALIGNED B = {((int32_t)-0x80000000) >> b_hr, ((int32_t)-0x80000000) >> b_hr};
        complex_s32_t WORD_ALIGNED C = {((int32_t)-0x80000000) >> c_hr, ((int32_t)-0x80000000) >> c_hr};
        complex_s32_t WORD_ALIGNED A;

        for(unsigned allow_sat = 0; allow_sat <= 1; allow_sat ++){

            exponent_t a_exp;
            right_shift_t b_shr, c_shr;

            // printf("!!!! b_exp = %d\n", b_exp);
            // printf("!!!! c_exp = %d\n", c_exp);
            // printf("!!!! b_hr = %d\n", b_hr);
            // printf("!!!! c_hr = %d\n\n", c_hr);

            xs3_complex_scal_mul_vect_complex_s32_calc_params(&a_exp, &b_shr, &c_shr, b_exp, c_exp, b_hr, c_hr, allow_sat);

            C.re = SIGNED_ASHR(C.re, c_shr);
            C.im = SIGNED_ASHR(C.im, c_shr);
            c_exp += c_shr;
            c_hr += c_shr;

            xs3_complex_scal_mul_vect_complex_s32(&A, &B, C.re, C.im, 1, b_shr);

            
            // printf("!!!! A.im = %ld   (0x%08X)\n", A.im, (unsigned) A.im);
            // printf("!!!! a_exp = %d\n", a_exp);
            // printf("!!!! b_exp = %d\n", b_exp);
            // printf("!!!! c_exp = %d\n", c_exp);
            // printf("!!!! b_hr = %d\n", b_hr);
            // printf("!!!! c_hr = %d\n", c_hr);
            // printf("!!!! b_shr = %d\n", b_shr);
            // printf("!!!! c_shr = %d\n", c_shr);

            if(allow_sat){
                TEST_ASSERT_EQUAL_MSG(0x7FFFFFFF, A.im, "A is wrong (sat)", r);

            } else {
                double q = ldexp(B.re, b_exp) * ldexp(C.im, c_exp) + ldexp(B.im, b_exp) * ldexp(C.re, c_exp);
                double p = ldexp(A.im, a_exp);

                // printf("!!! %f  %f\n", q, p);

                TEST_ASSERT_EQUAL_MSG(0x40000000, A.im, "A is wrong (no sat)", r);

                // When saturation is prevented, these should be exactly the same (for the mantissas used)
                TEST_ASSERT( q == p);
            }

        }
        
    }
}



void test_bfp_complex_scal_mul_vect_complex_s32()
{
    PRINTF("%s...\n", __func__);

    seed = 5636;

    complex_s32_t A_data[MAX_LEN], B_data[MAX_LEN];

    bfp_complex_s32_t A, B;

    struct {
        double real[MAX_LEN];
        double imag[MAX_LEN];
    } Af, Bf;

    complex_s32_t expA[MAX_LEN];

    for(int r = 0; r < REPS; r++){
        PRINTF("\trep % 3d..\t(seed: 0x%08X)\n", r, seed);

        bfp_init_vect_complex_s32(&B, B_data,
            pseudo_rand_int(&seed, -100, 100),
            pseudo_rand_int(&seed, 1, MAX_LEN+1), 0);

        bfp_init_vect_complex_s32(&A, A_data, 0, B.length, 0);

        B.hr = pseudo_rand_uint(&seed, 0, 28);

        const exponent_t c_exp = pseudo_rand_int(&seed, -100, 100);
        const headroom_t c_hr = pseudo_rand_uint(&seed, 0, 28);

        complex_s32_t C = {
            pseudo_rand_int32(&seed) >> c_hr,
            pseudo_rand_int32(&seed) >> c_hr };

        for(int i = 0; i < B.length; i++){
            B.data[i].re = pseudo_rand_int32(&seed) >> B.hr;
            B.data[i].im = pseudo_rand_int32(&seed) >> B.hr;

            Af.real[i] = ldexp(B.data[i].re, B.exp) * ldexp(C.re, c_exp) 
                       - ldexp(B.data[i].im, B.exp) * ldexp(C.im, c_exp);
            Af.imag[i] = ldexp(B.data[i].re, B.exp) * ldexp(C.im, c_exp) 
                       + ldexp(B.data[i].im, B.exp) * ldexp(C.re, c_exp);
        }

        bfp_headroom_vect_complex_s32(&B);

        bfp_complex_scal_mul_vect_complex_s32(&A, &B, C, c_exp);
        
        // PRINTF("\t    B.length = %u\n", B.length);
        // PRINTF("\t    B.exp    = %d\n", B.exp);
        // PRINTF("\t    B.hr     = %u\n", B.hr);

        // for(int i = 0; i < B.length; i++){
        //     PRINTF("\t        B.real[% 3d] = % 10d    (0x%04X)\n", i, B.real[i], (unsigned) B.real[i]);
        //     PRINTF("\t        B.imag[% 3d] = % 10d    (0x%04X)\n", i, B.imag[i], (unsigned) B.imag[i]);
        // }

        // PRINTF("\t    C.exp = %d\n", c_exp);
        // PRINTF("\t    C.hr  = %u\n", HR_C32(C));
        // PRINTF("\t        C.re  = % 15ld    (0x%08X)\n", C.re, (unsigned) C.re);
        // PRINTF("\t        C.im  = % 15ld    (0x%08X)\n", C.im, (unsigned) C.im);
        
        
        // PRINTF("\t    A.length = %u\n", A.length);
        // PRINTF("\t    A.exp    = %d\n", A.exp);
        // PRINTF("\t    A.hr     = %u\n", A.hr);

        // for(int i = 0; i < A.length; i++){
        //     PRINTF("\t        A.real[% 3d] = % 10d    (0x%04X)\n", i, A.real[i], (unsigned) A.real[i]);
        //     PRINTF("\t        A.imag[% 3d] = % 10d    (0x%04X)\n", i, A.imag[i], (unsigned) A.imag[i]);
        // }

        test_complex_s32_from_double(expA, Af.real, Af.imag, MAX_LEN, A.exp);

        for(int i = 0; i < A.length; i++){
            TEST_ASSERT_INT32_WITHIN(2, expA[i].re, A.data[i].re);
            TEST_ASSERT_INT32_WITHIN(2, expA[i].im, A.data[i].im);
        }
    }
}




void test_bfp_complex_scal_mul_vect_complex()
{
    SET_TEST_FILE();
    RUN_TEST(test_bfp_complex_scal_mul_vect_complex_s16);
    RUN_TEST(test_bfp_complex_scal_mul_vect_complex_s32_calc_params);
    RUN_TEST(test_bfp_complex_scal_mul_vect_complex_s32);
}