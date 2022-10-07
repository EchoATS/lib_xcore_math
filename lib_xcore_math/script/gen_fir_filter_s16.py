# Copyright 2021-2022 XMOS LIMITED.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
# This Software is subject to the terms of the XMOS Public Licence: Version 1.
import numpy as np
import argparse
import io
import os

import lib_xcore_math.script.xmath_script as xms

def main():
  
  parser = argparse.ArgumentParser()

  parser.add_argument("filter_name",
                      type=xms.filter_id,
                      help=
"""Name of the generated filter. 
This name will be used to initialize and invoke the filter from user code.""")

  parser.add_argument("filter_coefficients",
                      type=xms.fir_coefs_file,
                      help=
"""File containing the filter coefficients. 

This is the path to a file which contains the floating-point coefficients for the filter to be created as plain text.
The order of the coefficients is b[0], b[1], ... b[N_taps-1]. Coefficients may be separated by whitespace and/or 
commas. N_taps, if not explicitly specified, will be derived from the number of coefficients found here.""")

  parser.add_argument("--taps",
                      type=int,
                      default=-1,
                      help=
"""The number of filter taps.

Default behavior is to derive this value from the number of coefficients found in the filter coefficients file. If this
option is used, this script will verify that the number of coefficients found in the file matches the number specified
here.
""")

  parser.add_argument("--out-dir",
                      type=str,
                      default=".",
                      help=
"""
(optional) Output directory into which generated files are placed.
""")

  parser.add_argument("--input-headroom",
                      type=int,
                      default=0,
                      help=
"""Guaranteed headroom of input signal. (Default: 0)

The filter coefficients are calculated to maximize precision without saturating the internal accumulators. If the input
signal going into the filter is known a priori to always have some minimum amount of headroom, this information can be
used to help improve the precision of the filter.

Specifically, this field is the minimum number of _redundant_ sign bits known to be present in the 16-bit input signal 
to be filtered. If unknown, no headroom will be assumed.
""")

  parser.add_argument("--output-headroom",
                      type=int,
                      default=0,
                      help=
"""Guaranteed headroom of output signal. (Default: 0)

In some cases it is desirable to ensure a certain number of bits of headroom of the output signal from a filter. This
option can be used to achieve this. Note that this is the _minimum_ (i.e. worst-case) number of headroom bits; for some
filters there may in practice usually be more.

Note also that the number of significant bits in the output signal is directly decreased by this option.
""")

  args = extra_process_args(parser.parse_args())


  mse = find_filter_parameters(args)

  header_text = generate_header(args)
  source_text = generate_source(*mse, args)

  dir = os.path.dirname(args.header_fpath)
  if not os.path.exists(dir):
    os.makedirs(dir)

  with open(args.header_fpath, "w+") as header_file:
    header_file.write(header_text.getvalue())

  with open(args.source_fpath, "w+") as source_file:
    source_file.write(source_text.getvalue())

### Process some extra stuff to put in args
def extra_process_args(args):

  #Filter tap count
  if args.taps == -1:
    args.taps = args.filter_coefficients.shape[0]

  if args.taps != args.filter_coefficients.shape[0]:
    raise Exception(f"Loaded coefficient count ({args.filter_coefficients.shape[0]}) doesn't match specified tap count ({args.taps}).")

  print(f"Filter tap count: {args.taps}")

  # header and source filenames
  args.header_filename = f"{args.filter_name}.h"
  args.source_filename = f"{args.filter_name}.c"
  args.header_fpath = os.path.join(args.out_dir, args.header_filename)
  args.source_fpath = os.path.join(args.out_dir, args.source_filename)

  print(f"Files to be written:")
  print(f"  {args.header_fpath}")
  print(f"  {args.source_fpath}")

  return args


### Convert user's floating-point filter coefficients to the parameters
### required for filter_fir_s16_t
def find_filter_parameters(args):
  coefs = args.filter_coefficients

  # Find least-headroom 16-bit BFP representation of coefs
  largest_coef = np.max(np.abs(coefs))
  scale = np.ldexp(1,15) / largest_coef
  scale_log2 = np.floor(np.log2(scale))
  scale = 2**scale_log2

  scaled_coefs = np.round(scale * coefs).astype(np.int64)
  exponent = -scale_log2
  
  # Find maximum possible dot-product of coefs with input (given known input headroom)
  dot_prod = max_dot_product(scaled_coefs, args.input_headroom)
  dot_prod_log2 = np.log2(dot_prod)

  # If that dot-product doesn't fit in 32 bits, add headroom to coefficients until it does.
  if dot_prod_log2 > 31.0:
    tmp = np.ceil(dot_prod_log2)-31
    scaled_coefs = np.round(scaled_coefs * 2**-tmp).astype(np.int64)
    exponent = exponent + tmp
    dot_prod = max_dot_product(scaled_coefs, args.input_headroom)
    dot_prod_log2 = np.log2(dot_prod)
  
  # Find the shift value that makes the output fit in 16 bits
  shift = 0
  if dot_prod_log2 > 15.0:
    shift = int(np.ceil(dot_prod_log2 - 15.0))

  # If the user wants guaranteed output headroom, include that as well.
  shift = shift + args.output_headroom

  return scaled_coefs.astype(np.int16), int(shift), int(exponent)

# Compute maximum possible dot-product of coefficients with an input signal (taking
# input headroom into account)
def max_dot_product(coefs, input_hr):
  max_pos_input = ((2**15)-1)>>input_hr
  max_neg_input = (-(2**15))>>input_hr

  inputs = [(max_pos_input if x else max_neg_input) for x in (coefs >= 0)]
  dot_prod = np.sum([np.round( (x*y) ) for x,y in zip(coefs, inputs)])
  return dot_prod


### Generate C header file code using filter parameters ###
def generate_header(args):
  filter = args.filter_name
  header_text = io.StringIO()
  header_text.write(f"""
#pragma once
#include "xmath/xmath.h"

// Number of filter coefficients 
#define TAP_COUNT_{filter}\t({args.taps})

// Exponent associated with filter outputs
extern const exponent_t {filter}_exp;

// Call once to initialize the filter
C_API
void {filter}_init();

// Call to add a sample to the filter without computing an output sample
C_API
void {filter}_add_sample(int16_t new_sample);

// Call to process an input sample and generate an output sample
C_API
int16_t {filter}(int16_t new_sample);
  """)
  return header_text


### Generate C source file code using filter parameters ###
def generate_source(coefs, shift, exponent, args):
  filter = args.filter_name
  coef_string = xms.array_to_str(coefs)
  
  source_text = io.StringIO()

  source_text.write(f"""
#include "{filter}.h"

const right_shift_t {filter}_shift = {shift};
const exponent_t {filter}_exp = {exponent};
const int16_t WORD_ALIGNED {filter}_coefs[TAP_COUNT_{filter}] = {{
  {coef_string}
}};

int16_t WORD_ALIGNED {filter}_state[TAP_COUNT_{filter}] = {{0}};

filter_fir_s16_t _{filter};

void {filter}_init()
{{
  filter_fir_s16_init(&_{filter}, {filter}_state, TAP_COUNT_{filter},
                          {filter}_coefs, {filter}_shift);
}}

void {filter}_add_sample(int16_t new_sample)
{{
  filter_fir_s16_add_sample(&_{filter}, new_sample);
}}

int16_t {filter}(int16_t new_sample)
{{
  return filter_fir_s16(&_{filter}, new_sample);
}}
""")

  return source_text

### Execute script's main() function ###
if __name__ == "__main__":
    main()