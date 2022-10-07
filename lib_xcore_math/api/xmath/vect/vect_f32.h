// Copyright 2020-2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include "../xmath_conf.h"
#include "../types.h"
#include "../util.h"

/**
 * @defgroup vect_f32_api   32-bit Vector IEEE 754 Floating-Point Functions
 */

#ifdef __XC__
extern "C" {
#endif




/** 
 * @brief Get the maximum (32-bit BFP) exponent from a vector of IEEE754 floats.
 * 
 * This function is used to determine the BFP exponent to use when converting a vector of IEEE754
 * single-precision floats into a 32-bit BFP vector.
 * 
 * The exponent returned, if used with `vect_f32_to_vect_s32()`, is the one which will result in
 * no headroom in the BFP vector -- that is, the @a minimum permissible exponent for the BFP vector.
 * The minimum permissible exponent is derived from the @a maximum exponent found in the `float`
 * elements themselves.
 * 
 * More specifically, the `FSEXP` instruction is used on each element to determine its exponent. The
 * value returned is the maximum exponent given by the `FSEXP` instruction plus `30`.
 * 
 * `b[]` must begin at a double-word-aligned address.
 * 
 * @note If required, when converting to a 32-bit BFP vector, additional headroom can be included by
 *       adding the amount of required headroom to the exponent returned by this function.
 * 
 * @param[in]   b        Input vector of IEEE754 single-precision floats @vector{b}
 * @param[in]   length   Number of elements in @vector{b}
 * 
 * @returns Exponent used for converting to 32-bit BFP vector.
 * 
 * @see vect_f32_to_vect_s32
 * @see vect_s32_to_vect_f32
 * 
 * @exception ET_LOAD_STORE Raised if`b` is not double-word-aligned (See @ref note_vector_alignment)
 * @exception ET_ARITHMETIC Raised if Any element of `b` is infinite or not-a-number.
 * 
 * @ingroup vect_f32_api
 */
C_API
exponent_t vect_f32_max_exponent(
    const float b[], 
    const unsigned length);

/**
 * @brief Convert a vector of IEEE754 single-precision floats into a 32-bit BFP vector.
 * 
 * This function converts a vector of IEEE754 single-precision floats @vector{b} into the mantissa
 * vector @vector{a} of a 32-bit BFP vector, given BFP vector exponent @math{a\_exp}. Conceptually,
 * the elements of output vector @math{\bar{a} \cdot 2^{a\_exp}} represent the same values as those
 * of the input vector.
 * 
 * Because the output exponent @math{a\_exp} is shared by all elements of the output vector, even
 * though the output vector has 32-bit mantissas, precision may be lost on some elements if the
 * exponents of the input elements @math{b_k} span a wide range.
 * 
 * The function `vect_f32_max_exponent()` can be used to determine the value for @math{a\_exp}
 * which minimizes headroom of the output vector.
 * 
 * @operation{ 
 * &     a_k \leftarrow round(\frac{b_k}{2^{b\_exp}})          \\
 * &         \qquad\text{ for }k\in 0\ ...\ (length-1) 
 * }
 * 
 * @par Parameter Details
 * @parblock
 * 
 * `a[]` represents the 32-bit output mantissa vector @vector{a}.
 * 
 * `b[]` represents the IEEE754 float input vector @vector{b}.
 * 
 * `a[]` and `b[]` must each begin at a double-word-aligned address.
 * 
 * `b[]` can be safely updated in-place.
 * 
 * `length` is the number of elements in each of the vectors.
 * 
 * `a_exp` is the exponent associated with the output vector @vector{a}.
 * @endparblock
 * 
 * @param[out]  a        Output vector @vector{a}
 * @param[in]   b        Input vector @vector{b}
 * @param[in]   length   Number of elements in vectors @vector{a} and @vector{b}
 * @param[in]   a_exp    Exponent @math{a\_exp} of output vector @vector{a}
 * 
 * @see vect_f32_max_exponent
 * @see vect_s32_to_vect_f32
 * 
 * @exception ET_LOAD_STORE Raised if `a` or `b` is not double-word-aligned (See @ref note_vector_alignment)
 * @exception ET_ARITHMETIC Raised if Any element of `b` is infinite or not-a-number.
 * 
 * @ingroup vect_f32_api
 */
C_API
void vect_f32_to_vect_s32(
    int32_t a[],
    const float b[], 
    const unsigned length,
    const exponent_t a_exp);

    
/**
 * @brief Compute the inner product of two IEEE754 float vectors.
 * 
 * This function takes two vectors of IEEE754 single-precision floats and computes their inner
 * product -- the sum of the elementwise products. The `FMACC` instruction is used, granting full
 * precision in the addition.
 * 
 * The inner product @math{a} is returned.
 * 
 * @operation{ 
 * &     a   \leftarrow \sum_{k=0}^{length-1} ( b_k \cdot c_k )       
 * }
 * 
 * @param[in]   b        Input vector @vector{b}
 * @param[in]   c        Input vector @vector{c}
 * @param[in]   length   Number of elements in vectors @vector{b} and @vector{c}
 * 
 * @returns The inner product
 * 
 * @ingroup vect_f32_api
 */
C_API
float vect_f32_dot(
    const float b[],
    const float c[],
    const unsigned length);

#ifdef __XC__
}   //extern "C"
#endif

