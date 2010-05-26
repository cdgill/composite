/**
 * Copyright 2009 by Andrew Sweeney, ajs86@gwu.edu
 *
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 */

#ifndef STKMGR_H_
#define STKMGR_H_

/** 
 * In the future we may want to change this too
 * cos_asm_server_stub_spdid
 */
void * stkmgr_grant_stack(spdid_t d_spdid);
void stkmgr_return_stack(spdid_t s_spdid, vaddr_t addr);

void stkmgr_stack_report(void);
int stkmgr_set_concurrency(spdid_t spdid, int concur_lvl, int remove_spare);
int stkmgr_spd_concurrency_estimate(spdid_t spdid);
unsigned long stkmgr_thd_blk_time(unsigned short int tid, spdid_t spdid, int reset);
int stkmgr_thd_blk_cnt(unsigned short int tid, spdid_t spdid, int reset);


#endif




