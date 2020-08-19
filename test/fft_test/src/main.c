

#include <stdio.h>

#include "unity.h"
#include "test_cases.h"

int main(int argc, char** argv)
{
    UNITY_BEGIN();


    test_xs3_fft_helpers();
    test_xs3_fft_dit();
    test_xs3_fft_dif();
    test_xs3_fft_mono_adjust();

    test_bfp_fft();


    return UNITY_END();
}