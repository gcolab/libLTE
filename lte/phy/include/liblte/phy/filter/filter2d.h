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



#ifndef FILTER2D_
#define FILTER2D_

#include "liblte/config.h"
#include <stdint.h>

/* 2-D real filter of complex input
 *
 */
typedef _Complex float cf_t;

typedef struct LIBLTE_API{
  uint32_t sztime; // Output signal size in the time domain
  uint32_t szfreq;  // Output signal size in the freq domain
  uint32_t ntime;  // 2-D Filter size in time domain
  uint32_t nfreq;  // 2-D Filter size in frequency domain
  float **taps;  // 2-D filter coefficients
  float norm; //normalization factor
  cf_t *output; // Output signal
} filter2d_t;

LIBLTE_API int filter2d_init (filter2d_t* q, 
                              float **taps, 
                              uint32_t ntime, 
                              uint32_t nfreq, 
                              uint32_t sztime, 
                              uint32_t szfreq);

LIBLTE_API int filter2d_init_ones (filter2d_t* q, 
                                      uint32_t ntime, 
                                      uint32_t nfreq, 
                                      uint32_t sztime, 
                                      uint32_t szfreq);

LIBLTE_API void filter2d_free(filter2d_t *q);

LIBLTE_API void filter2d_step(filter2d_t *q); 

LIBLTE_API void filter2d_reset(filter2d_t *q);

LIBLTE_API void filter2d_add(filter2d_t *q, 
                             cf_t h, 
                             uint32_t time_idx, 
                             uint32_t freq_idx);

LIBLTE_API void filter2d_add_out(filter2d_t *q, cf_t x, int time_idx, int freq_idx);

LIBLTE_API void filter2d_get_symbol(filter2d_t *q, 
                                    uint32_t nsymbol, 
                                    cf_t *output);
#endif // FILTER2D_
