/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include <string.h>
#include "liblte/phy/phy.h"
#include "liblte/mex/mexutils.h"

/** MEX function to be called from MATLAB to test the channel estimator 
 */

#define INPUT   prhs[0]
#define NITERS  prhs[1]
#define NOF_INPUTS 1


void help()
{
  mexErrMsgTxt
    ("[decoded_bits] = liblte_turbodecoder(input_llr, nof_iterations)\n\n");
}

/* the gateway function */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

  tdec_t tdec;
  float *input_llr;
  uint8_t *output_data; 
  uint32_t nof_bits; 
  uint32_t nof_iterations; 
  
  if (nrhs < NOF_INPUTS) {
    help();
    return;
  }
  
  // Read input symbols
  uint32_t nof_symbols = mexutils_read_f(INPUT, &input_llr);
  if (nof_symbols < 40) {
    mexErrMsgTxt("Minimum block size is 40\n");
    return; 
  }
  nof_bits = (nof_symbols-12)/3;
  
  if (!lte_cb_size_isvalid(nof_bits)) {
    mexErrMsgTxt("Invalid codeblock size\n");
    return; 
  }


  // read number of iterations 
  if (nrhs > NOF_INPUTS) {
    nof_iterations = (uint32_t) mxGetScalar(prhs[1]);
    if (nof_iterations > 50) {
      mexErrMsgTxt("Maximum number of iterations is 50\n");
      return; 
    }
  } else {
    nof_iterations = 5; // set the default nof iterations to 5 as in matlab 
  }
  
  // allocate memory for output bits
  output_data = vec_malloc(nof_bits * sizeof(uint8_t));

  if (tdec_init(&tdec, nof_bits)) {
    mexErrMsgTxt("Error initiating Turbo decoder\n");
    return;
  }

  tdec_run_all(&tdec, input_llr, output_data, nof_iterations, nof_bits);

  if (nlhs >= 1) { 
    mexutils_write_uint8(output_data, &plhs[0], nof_bits, 1);  
  }

  tdec_free(&tdec);

  free(input_llr);
  free(output_data);

  return;
}

