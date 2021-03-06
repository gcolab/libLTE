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


#include <stdlib.h>
#include <string.h>

#include "liblte/phy/utils/dft.h"
#include "liblte/phy/utils/vector.h"
#include "liblte/phy/utils/convolution.h"


int conv_fft_cc_init(conv_fft_cc_t *q, uint32_t input_len, uint32_t filter_len) {
  q->input_len = input_len;
  q->filter_len = filter_len;
  q->output_len = input_len+filter_len;
  q->input_fft = vec_malloc(sizeof(cf_t)*q->output_len);
  q->filter_fft = vec_malloc(sizeof(cf_t)*q->output_len);
  q->output_fft = vec_malloc(sizeof(cf_t)*q->output_len);
  if (!q->input_fft || !q->filter_fft || !q->output_fft) {
    return LIBLTE_ERROR;
  }
  if (dft_plan(&q->input_plan,q->output_len,FORWARD,COMPLEX)) {
    return LIBLTE_ERROR;
  }
  if (dft_plan(&q->filter_plan,q->output_len,FORWARD,COMPLEX)) {
    return LIBLTE_ERROR;
  }
  if (dft_plan(&q->output_plan,q->output_len,BACKWARD,COMPLEX)) {
    return LIBLTE_ERROR;
  }
  dft_plan_set_norm(&q->input_plan, true);
  dft_plan_set_norm(&q->filter_plan, true);
  dft_plan_set_norm(&q->output_plan, false);
  return LIBLTE_SUCCESS;
}

void conv_fft_cc_free(conv_fft_cc_t *q) {
  if (q->input_fft) {
    free(q->input_fft);
  }
  if (q->filter_fft) {
    free(q->filter_fft);
  }
  if (q->output_fft) {
    free(q->output_fft);
  }
  dft_plan_free(&q->input_plan);
  dft_plan_free(&q->filter_plan);
  dft_plan_free(&q->output_plan);
  
  bzero(q, sizeof(conv_fft_cc_t));

}

uint32_t conv_fft_cc_run(conv_fft_cc_t *q, cf_t *input, cf_t *filter, cf_t *output) {

  dft_run_c(&q->input_plan, input, q->input_fft);
  dft_run_c(&q->filter_plan, filter, q->filter_fft);

  vec_prod_ccc(q->input_fft,q->filter_fft,q->output_fft,q->output_len);

  dft_run_c(&q->output_plan, q->output_fft, output);

  return q->output_len-1;

}

uint32_t conv_cc(cf_t *input, cf_t *filter, cf_t *output, uint32_t input_len, uint32_t filter_len) {
  uint32_t i;
  uint32_t M = filter_len; 
  uint32_t N = input_len; 

  for (i=0;i<M;i++) {
    output[i]=vec_dot_prod_ccc(&input[i],&filter[i],i);
  }
  for (;i<M+N-1;i++) {
    output[i] = vec_dot_prod_ccc(&input[i-M], filter, M);
  }
  return M+N-1;
}

/* Centered convolution. Returns the same number of input elements. Equivalent to conv(x,h,'same') in matlab. 
 * y(n)=sum_i x(n+i-M/2)*h(i) for n=1..N with N input samples and M filter len 
 */
uint32_t conv_same_cc(cf_t *input, cf_t *filter, cf_t *output, uint32_t input_len, uint32_t filter_len) {
  uint32_t i;
  uint32_t M = filter_len; 
  uint32_t N = input_len; 
  
  for (i=0;i<M/2;i++) {
    output[i]=vec_dot_prod_ccc(&input[i],&filter[M/2-i],M-M/2+i);
  }
  for (;i<N-M/2;i++) {
    output[i]=vec_dot_prod_ccc(&input[i-M/2],filter,M);
  }
  for (;i<N;i++) {
    output[i]=vec_dot_prod_ccc(&input[i-M/2],filter,N-i+M/2);    
  }
  return N;
}

uint32_t conv_same_cf(cf_t *input, float *filter, cf_t *output, 
                      uint32_t input_len, uint32_t filter_len) {
  uint32_t i;
  uint32_t M = filter_len; 
  uint32_t N = input_len; 
  
  for (i=0;i<M/2;i++) {
    output[i]=vec_dot_prod_cfc(&input[i],&filter[M/2-i],M-M/2+i);
  }
  for (;i<N-M/2;i++) {
    output[i]=vec_dot_prod_cfc(&input[i-M/2],filter,M);
  }
  for (;i<N;i++) {
    output[i]=vec_dot_prod_cfc(&input[i-M/2],filter,N-i+M/2);    
  }
  return N;
}
