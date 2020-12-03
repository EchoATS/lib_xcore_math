
#include <stdint.h>
#include <stdio.h>

#include "xs3_math.h"
#include "../vpu_helper.h"


/////////////////////////////////////////////
/////  Rotation Tables (for Magnitude)  /////
/////////////////////////////////////////////

const unsigned rot_table32_rows = 30;

const complex_s32_t rot_table32[30][4] = {
  { {759250125, -759250125}, {759250125, -759250125}, {759250125, -759250125}, {759250125, -759250125} },
  { {992008094, -410903207}, {992008094, -410903207}, {992008094, -410903207}, {992008094, -410903207} },
  { {1053110176, -209476638}, {1053110176, -209476638}, {1053110176, -209476638}, {1053110176, -209476638} },
  { {1068571464, -105245103}, {1068571464, -105245103}, {1068571464, -105245103}, {1068571464, -105245103} },
  { {1072448455, -52686014}, {1072448455, -52686014}, {1072448455, -52686014}, {1072448455, -52686014} },
  { {1073418433, -26350943}, {1073418433, -26350943}, {1073418433, -26350943}, {1073418433, -26350943} },
  { {1073660973, -13176464}, {1073660973, -13176464}, {1073660973, -13176464}, {1073660973, -13176464} },
  { {1073721611, -6588356}, {1073721611, -6588356}, {1073721611, -6588356}, {1073721611, -6588356} },
  { {1073736771, -3294193}, {1073736771, -3294193}, {1073736771, -3294193}, {1073736771, -3294193} },
  { {1073740561, -1647099}, {1073740561, -1647099}, {1073740561, -1647099}, {1073740561, -1647099} },
  { {1073741508, -823550}, {1073741508, -823550}, {1073741508, -823550}, {1073741508, -823550} },
  { {1073741745, -411775}, {1073741745, -411775}, {1073741745, -411775}, {1073741745, -411775} },
  { {1073741804, -205887}, {1073741804, -205887}, {1073741804, -205887}, {1073741804, -205887} },
  { {1073741819, -102944}, {1073741819, -102944}, {1073741819, -102944}, {1073741819, -102944} },
  { {1073741823, -51472}, {1073741823, -51472}, {1073741823, -51472}, {1073741823, -51472} },
  { {1073741824, -25736}, {1073741824, -25736}, {1073741824, -25736}, {1073741824, -25736} },
  { {1073741824, -12868}, {1073741824, -12868}, {1073741824, -12868}, {1073741824, -12868} },
  { {1073741824, -6434}, {1073741824, -6434}, {1073741824, -6434}, {1073741824, -6434} },
  { {1073741824, -3217}, {1073741824, -3217}, {1073741824, -3217}, {1073741824, -3217} },
  { {1073741824, -1608}, {1073741824, -1608}, {1073741824, -1608}, {1073741824, -1608} },
  { {1073741824, -804}, {1073741824, -804}, {1073741824, -804}, {1073741824, -804} },
  { {1073741824, -402}, {1073741824, -402}, {1073741824, -402}, {1073741824, -402} },
  { {1073741824, -201}, {1073741824, -201}, {1073741824, -201}, {1073741824, -201} },
  { {1073741824, -101}, {1073741824, -101}, {1073741824, -101}, {1073741824, -101} },
  { {1073741824, -50}, {1073741824, -50}, {1073741824, -50}, {1073741824, -50} },
  { {1073741824, -25}, {1073741824, -25}, {1073741824, -25}, {1073741824, -25} },
  { {1073741824, -13}, {1073741824, -13}, {1073741824, -13}, {1073741824, -13} },
  { {1073741824, -6}, {1073741824, -6}, {1073741824, -6}, {1073741824, -6} },
  { {1073741824, -3}, {1073741824, -3}, {1073741824, -3}, {1073741824, -3} },
  { {1073741824, -2}, {1073741824, -2}, {1073741824, -2}, {1073741824, -2} },
};

const unsigned rot_table16_rows = 14;
const int16_t rot_table16[14][2][16] = {
  { {  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82,  0x5A82 },
    { -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82, -0x5A82 } },
  { {  0x7642,  0x7642,  0x7642,  0x7642,  0x7642,  0x7642,  0x7642,  0x7642,  0x7642,  0x7642,  0x7642,  0x7642,  0x7642,  0x7642,  0x7642,  0x7642 },
    { -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC, -0x30FC } },
  { {  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A,  0x7D8A },
    { -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9, -0x18F9 } },
  { {  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62,  0x7F62 },
    { -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C, -0x0C8C } },
  { {  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9,  0x7FD9 },
    { -0x0648, -0x0648, -0x0648, -0x0648, -0x0648, -0x0648, -0x0648, -0x0648, -0x0648, -0x0648, -0x0648, -0x0648, -0x0648, -0x0648, -0x0648, -0x0648 } },
  { {  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6,  0x7FF6 },
    { -0x0324, -0x0324, -0x0324, -0x0324, -0x0324, -0x0324, -0x0324, -0x0324, -0x0324, -0x0324, -0x0324, -0x0324, -0x0324, -0x0324, -0x0324, -0x0324 } },
  { {  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE,  0x7FFE },
    { -0x0192, -0x0192, -0x0192, -0x0192, -0x0192, -0x0192, -0x0192, -0x0192, -0x0192, -0x0192, -0x0192, -0x0192, -0x0192, -0x0192, -0x0192, -0x0192 } },
  { {  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF },
    { -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9, -0x00C9 } },
  { {  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF },
    { -0x0065, -0x0065, -0x0065, -0x0065, -0x0065, -0x0065, -0x0065, -0x0065, -0x0065, -0x0065, -0x0065, -0x0065, -0x0065, -0x0065, -0x0065, -0x0065 } },
  { {  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF },
    { -0x0032, -0x0032, -0x0032, -0x0032, -0x0032, -0x0032, -0x0032, -0x0032, -0x0032, -0x0032, -0x0032, -0x0032, -0x0032, -0x0032, -0x0032, -0x0032 } },
  { {  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF },
    { -0x0019, -0x0019, -0x0019, -0x0019, -0x0019, -0x0019, -0x0019, -0x0019, -0x0019, -0x0019, -0x0019, -0x0019, -0x0019, -0x0019, -0x0019, -0x0019 } },
  { {  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF },
    { -0x000D, -0x000D, -0x000D, -0x000D, -0x000D, -0x000D, -0x000D, -0x000D, -0x000D, -0x000D, -0x000D, -0x000D, -0x000D, -0x000D, -0x000D, -0x000D } },
  { {  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF },
    { -0x0006, -0x0006, -0x0006, -0x0006, -0x0006, -0x0006, -0x0006, -0x0006, -0x0006, -0x0006, -0x0006, -0x0006, -0x0006, -0x0006, -0x0006, -0x0006 } },
  { {  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF,  0x7FFF },
    { -0x0003, -0x0003, -0x0003, -0x0003, -0x0003, -0x0003, -0x0003, -0x0003, -0x0003, -0x0003, -0x0003, -0x0003, -0x0003, -0x0003, -0x0003, -0x0003 } },
};


/////////////////////////////////////
/////       Magnitude           /////
/////////////////////////////////////

WEAK_FUNC
headroom_t xs3_vect_complex_s16_mag(
    int16_t a[],
    const int16_t b_real[],
    const int16_t b_imag[],
    const unsigned length,
    const right_shift_t b_shr,
    const int16_t* rot_table,
    const unsigned table_rows)
{
    for(int k = 0; k < length; k++){
        
        complex_s32_t B = {
            ASHR(16)(b_real[k], b_shr), 
            ASHR(16)(b_imag[k], b_shr)
        };

        B.re = (B.re >= 0)? B.re : -B.re;
        B.im = (B.im >= 0)? B.im : -B.im;

        for(int iter = 0; iter < table_rows; iter++){

            complex_s32_t rot = {
                rot_table[32 * iter],
                rot_table[32 * iter + 16 ],
            };

            int32_t q1 = B.re * rot.re;
            int32_t q2 =-B.im * rot.im;
            int32_t q3 = B.re * rot.im;
            int32_t q4 = B.im * rot.re;

            B.re = SAT(16)(ROUND_SHR(q1 + q2, 15));
            B.im = SAT(16)(ROUND_SHR(q3 + q4, 15));

            B.re = (B.re >= 0)? B.re : -B.re;
            B.im = (B.im >= 0)? B.im : -B.im;

        }

        a[k] = B.re;
    }


    return xs3_vect_s16_headroom(a, length);
}


WEAK_FUNC
headroom_t xs3_vect_complex_s32_mag(
    int32_t a[],
    const complex_s32_t b[],
    const unsigned length,
    const right_shift_t b_shr,
    const complex_s32_t* rot_table,
    const unsigned table_rows)
{

    for(int k = 0; k < length; k++){
        
        complex_s32_t B = {
            ASHR(32)(b[k].re, b_shr), 
            ASHR(32)(b[k].im, b_shr),
        };

        B.re = (B.re >= 0)? B.re : -B.re;
        B.im = (B.im >= 0)? B.im : -B.im;

        for(int iter = 0; iter < table_rows; iter++){

            complex_s32_t rot = {
                rot_table[iter * 4].re,
                rot_table[iter * 4].im
            };

            // printf("%d:\t%ld + i*%ld\n", iter, rot.re, rot.im);

            int64_t q1 = ROUND_SHR( ((int64_t)B.re) * rot.re, 30 );
            int64_t q2 = ROUND_SHR( ((int64_t)B.im) * rot.im, 30 );
            int64_t q3 = ROUND_SHR( ((int64_t)B.re) * rot.im, 30 );
            int64_t q4 = ROUND_SHR( ((int64_t)B.im) * rot.re, 30 );

            B.re = SAT(32)(q1 - q2);
            B.im = SAT(32)(q3 + q4);

            B.re = (B.re >= 0)? B.re : -B.re;
            B.im = (B.im >= 0)? B.im : -B.im;

        }

        a[k] = B.re;

    }

    return xs3_vect_s32_headroom( (int32_t*) a, length);
}


/////////////////////////////////////
/////  Squared Magnitude        /////
/////////////////////////////////////

WEAK_FUNC
headroom_t xs3_vect_complex_s16_squared_mag(
    int16_t a[],
    const int16_t b_real[],
    const int16_t b_imag[],
    const unsigned length,
    const right_shift_t sat)
{
    for(int k = 0; k < length; k++){
        
        complex_s32_t B = { b_real[k], b_imag[k] };

        int32_t acc = B.re*B.re + B.im*B.im;
        a[k] = SAT(16)(ROUND_SHR(acc, sat));
    }

    return xs3_vect_s16_headroom(a, length);
}


WEAK_FUNC
headroom_t xs3_vect_complex_s32_squared_mag(
    int32_t a[],
    const complex_s32_t b[],
    const unsigned length,
    const right_shift_t b_shr)
{

    for(int k = 0; k < length; k++){
        
        complex_s32_t B = {
            ASHR(32)(b[k].re, b_shr), 
            ASHR(32)(b[k].im, b_shr),
        };

        B.re = SAT(32)(ROUND_SHR(((int64_t)B.re)*B.re, 30));
        B.im = SAT(32)(ROUND_SHR(((int64_t)B.im)*B.im, 30));

        a[k] = B.re + B.im;
    }

    return xs3_vect_s32_headroom(a, length);
}