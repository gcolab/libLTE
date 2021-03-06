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
#include <strings.h>
#include <assert.h>
#include <unistd.h>

#include "liblte/phy/ue/ue_cell_search.h"

#include "liblte/phy/utils/debug.h"
#include "liblte/phy/utils/vector.h"

float tmp_pss_corr[32*10000];
float tmp_sss_corr[31*10000];

int ue_cell_search_init(ue_cell_search_t * q, int (recv_callback)(void*, void*, uint32_t), void *stream_handler) 
{
  return ue_cell_search_init_max(q, CS_DEFAULT_MAXFRAMES_TOTAL, recv_callback, stream_handler); 
}

int ue_cell_search_init_max(ue_cell_search_t * q, uint32_t max_frames, 
                           int (recv_callback)(void*, void*, uint32_t), void *stream_handler) 
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;

  if (q != NULL) {
    ret = LIBLTE_ERROR;
    lte_cell_t cell;

    bzero(q, sizeof(ue_cell_search_t));    
    
    bzero(&cell, sizeof(lte_cell_t));
    cell.id = CELL_ID_UNKNOWN; 
    cell.nof_prb = CS_NOF_PRB; 

    if (ue_sync_init(&q->ue_sync, cell, recv_callback, stream_handler)) {
      fprintf(stderr, "Error initiating ue_sync\n");
      goto clean_exit; 
    }
    
    q->candidates = calloc(sizeof(ue_cell_search_result_t), max_frames);
    if (!q->candidates) {
      perror("malloc");
      goto clean_exit; 
    }
    q->mode_ntimes = calloc(sizeof(uint32_t), max_frames);
    if (!q->mode_ntimes) {
      perror("malloc");
      goto clean_exit;  
    }
    q->mode_counted = calloc(sizeof(uint8_t), max_frames);
    if (!q->mode_counted) {
      perror("malloc");
      goto clean_exit;  
    }

    q->max_frames = max_frames;
    q->nof_frames_to_scan = CS_DEFAULT_NOFFRAMES_TOTAL; 

    ret = LIBLTE_SUCCESS;
  }

clean_exit:
  if (ret == LIBLTE_ERROR) {
    ue_cell_search_free(q);
  }
  return ret;
}

void ue_cell_search_free(ue_cell_search_t * q)
{
  if (q->candidates) {
    free(q->candidates);
  }
  if (q->mode_counted) {
    free(q->mode_counted);
  }
  if (q->mode_ntimes) {
    free(q->mode_ntimes);
  }
  ue_sync_free(&q->ue_sync);
  
  bzero(q, sizeof(ue_cell_search_t));

}

void ue_cell_search_set_threshold(ue_cell_search_t * q, float threshold)
{
  q->detect_threshold = threshold;
}

int ue_cell_search_set_nof_frames_to_scan(ue_cell_search_t * q, uint32_t nof_frames)
{
  if (nof_frames <= q->max_frames) {
    q->nof_frames_to_scan = nof_frames;    
    return LIBLTE_SUCCESS; 
  } else {
    return LIBLTE_ERROR;
  }
}

/* Decide the most likely cell based on the mode */
static void get_cell(ue_cell_search_t * q, uint32_t nof_detected_frames, ue_cell_search_result_t *found_cell)
{
  uint32_t i, j;
  
  bzero(q->mode_counted, nof_detected_frames);
  bzero(q->mode_ntimes, sizeof(uint32_t) * nof_detected_frames);
  
  /* First find mode of CELL IDs */
  for (i = 0; i < nof_detected_frames; i++) {
    uint32_t cnt = 1;
    for (j=i+1;j<nof_detected_frames;j++) {
      if (q->candidates[j].cell_id == q->candidates[i].cell_id && !q->mode_counted[j]) {
        q->mode_counted[j]=1;
        cnt++;
      }
    }
    q->mode_ntimes[i] = cnt; 
  }
  uint32_t max_times=0, mode_pos=0; 
  for (i=0;i<nof_detected_frames;i++) {
    if (q->mode_ntimes[i] > max_times) {
      max_times = q->mode_ntimes[i];
      mode_pos = i;
    }
  }
  found_cell->cell_id = q->candidates[mode_pos].cell_id;
  /* Now in all these cell IDs, find most frequent CP */
  uint32_t nof_normal = 0;
  found_cell->peak = 0; 
  for (i=0;i<nof_detected_frames;i++) {
    if (q->candidates[i].cell_id == found_cell->cell_id) {
      if (CP_ISNORM(q->candidates[i].cp)) {
        nof_normal++;
      } 
    }
    // average absolute peak value 
    found_cell->peak += q->candidates[i].peak; 
  }
  found_cell->peak /= nof_detected_frames;
  
  if (nof_normal > q->mode_ntimes[mode_pos]/2) {
    found_cell->cp = CPNORM;
  } else {
    found_cell->cp = CPEXT; 
  }
  found_cell->mode = (float) q->mode_ntimes[mode_pos]/nof_detected_frames;  
  
  // PSR is already averaged so take the last value 
  found_cell->psr = q->candidates[nof_detected_frames-1].psr;
}

/** Finds up to 3 cells, one per each N_id_2=0,1,2 and stores ID and CP in the structure pointed by found_cell.
 * Each position in found_cell corresponds to a different N_id_2. 
 * Saves in the pointer max_N_id_2 the N_id_2 index of the cell with the highest PSR
 * Returns the number of found cells or a negative number if error
 */
int ue_cell_search_scan(ue_cell_search_t * q, ue_cell_search_result_t found_cells[3], uint32_t *max_N_id_2)
{
  int ret = 0; 
  float max_peak_value = -1.0;
  uint32_t nof_detected_cells = 0; 
  for (uint32_t N_id_2=0;N_id_2<3 && ret >= 0;N_id_2++) {
    ret = ue_cell_search_scan_N_id_2(q, N_id_2, &found_cells[N_id_2]);
    if (ret < 0) {
      fprintf(stderr, "Error searching cell\n");
      return ret; 
    }
    nof_detected_cells += ret;
    if (max_N_id_2) {
      if (found_cells[N_id_2].peak > max_peak_value) {
        max_peak_value = found_cells[N_id_2].peak;
        *max_N_id_2 = N_id_2;
      }      
    }
  }
  return nof_detected_cells;
}

/** Finds a cell for a given N_id_2 and stores ID and CP in the structure pointed by found_cell. 
 * Returns 1 if the cell is found, 0 if not or -1 on error
 */
int ue_cell_search_scan_N_id_2(ue_cell_search_t * q, uint32_t N_id_2, ue_cell_search_result_t *found_cell)
{
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  cf_t *sf_buffer = NULL; 
  uint32_t nof_detected_frames = 0; 
  uint32_t nof_scanned_frames = 0; 

  if (q != NULL) 
  {
    ret = LIBLTE_SUCCESS; 
    
    ue_sync_set_N_id_2(&q->ue_sync, N_id_2);
    ue_sync_reset(&q->ue_sync);
    do {
      
      ret = ue_sync_get_buffer(&q->ue_sync, &sf_buffer);
      if (ret < 0) {
        fprintf(stderr, "Error calling ue_sync_work()\n");       
        break; 
      } else if (ret == 1) {
        /* This means a peak was found and ue_sync is now in tracking state */
        ret = sync_get_cell_id(&q->ue_sync.strack);
        if (ret >= 0) {
          /* Save cell id, cp and peak */
          q->candidates[nof_detected_frames].cell_id = (uint32_t) ret;
          q->candidates[nof_detected_frames].cp = sync_get_cp(&q->ue_sync.strack);
          q->candidates[nof_detected_frames].peak = q->ue_sync.strack.pss.peak_value;
          q->candidates[nof_detected_frames].psr = sync_get_peak_value(&q->ue_sync.strack);
          INFO
            ("CELL SEARCH: [%3d/%3d/%d]: Found peak PSR=%.3f, Cell_id: %d CP: %s\n",
              nof_detected_frames, nof_scanned_frames, q->nof_frames_to_scan,
              q->candidates[nof_detected_frames].peak, q->candidates[nof_detected_frames].cell_id,
              lte_cp_string(q->candidates[nof_detected_frames].cp));
          memcpy(&tmp_pss_corr[nof_detected_frames*32], 
                &q->ue_sync.strack.pss.conv_output_avg[128], 32*sizeof(float));
          memcpy(&tmp_sss_corr[nof_detected_frames*31], 
                &q->ue_sync.strack.sss.corr_output_m0, 31*sizeof(float));
          nof_detected_frames++; 
        }
      } else if (ret == 0) {
        /* This means a peak is not yet found and ue_sync is in find state 
         * Do nothing, just wait and increase nof_scanned_frames counter. 
         */
      }
    
      nof_scanned_frames++;
      
    } while ((sync_get_peak_value(&q->ue_sync.strack)  < q->detect_threshold  ||
              nof_detected_frames                       < 4)                  &&
              nof_scanned_frames                       < q->nof_frames_to_scan);
    
    /*
    vec_save_file("sss_corr",tmp_sss_corr, nof_detected_frames*sizeof(float)*31);
    vec_save_file("track_corr",tmp_pss_corr, nof_detected_frames*sizeof(float)*32);
    vec_save_file("find_corr",q->ue_sync.sfind.pss.conv_output_avg, 
                  sizeof(float)*(9600+127));
    */
    
    /* In either case, check if the mean PSR is above the minimum threshold */
    if (nof_detected_frames > 0) {
      ret = 1;      // A cell has been found.  
      if (found_cell) {
        get_cell(q, nof_detected_frames, found_cell);        
        printf("Found CELL PHYID: %d, CP: %s, PSR: %.1f, Absolute Peak: %.1f dBm, Reliability: %.0f \%\n", 
              found_cell->cell_id, lte_cp_string(found_cell->cp), 
              found_cell->psr, 10*log10(found_cell->peak*1000), 100*found_cell->mode);          
      }
    } else {
      ret = 0;      // A cell was not found. 
    }
  }

  return ret;
}
