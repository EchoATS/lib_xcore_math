
#include "xs3_math.h"
#include "vpu_const_vects.h"

#define VEC_S8(X)       {X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,}
#define VEC_S16(X)      {X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,X,}
#define VEC_S32(X)      {X,X,X,X,X,X,X,X,}
#define VEC_C32(X,Y)    {{X,Y},{X,Y},{X,Y},{X,Y},}


const xs3_vpu_vect_t vpu_vec_zero = {.s8 = VEC_S8(0) };
const xs3_vpu_vect_t vpu_vec_neg_1 = {.s8 = VEC_S8(-1) };

const complex_s32_t vpu_vec_complex_pos_j[4]    = VEC_C32( 0x00000000, 0x40000000);
const complex_s32_t vpu_vec_complex_ones[4]     = VEC_C32( 0x40000000, 0x00000000);
const complex_s32_t vpu_vec_complex_conj_op[4]  = VEC_C32( 0x40000000,-0x40000000);
const complex_s32_t vpu_vec_complex_neg_j[4]    = VEC_C32( 0x00000000,-0x40000000);
const complex_s32_t vpu_vec_complex_neg_ones[4] = VEC_C32(-0x40000000, 0x00000000);

const int32_t vpu_vec_ones_s32[8] = VEC_S32(0x40000000);

const int16_t vpu_vec_0x0001[16]     = VEC_S16( 0x0001);
const int16_t vpu_vec_0x4000[16]     = VEC_S16( 0x4000);
const int16_t vpu_vec_neg_0x4000[16] = VEC_S16(-0x4000);

const int8_t vpu_vec_0x01[32] = VEC_S8(0x01);
const int8_t vpu_vec_0x40[32] = VEC_S8(0x40);