/*************************************************************************
    > File Name: match_utils.h
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-14 13:24:41
 ************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "genafp.h"
#include "afpWrap.h"

#include "hashtable.h"
#include "global.h"

#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif
#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

#define WEA_THRESH 3
#define ABS_THRESH 7
#define INC_THRESH 2
#define REL_THRESH 4
typedef struct {
    int id;
    int lv;
} Rcd;

typedef struct {
    int ID;
    int score;
    double sseg; // start segment
    double eseg; // end segment
} FLOW_MATCH_RESULT;

int match_or_not( const LANDMARK *LM, const unsigned int nL,  MATCH_RESULT *R, const HASHTABLE *table, int wea_thresh, int abs_thresh, int inc_thresh, int rel_thresh);
unsigned int cnt_forward(const LANDMARK *LM, const unsigned int nL, unsigned int startIndex, unsigned int oneStepFrames);
int cmp_seg_match(const void *a1, const void *b1);
uint64_t print_table_cnt(const HASHTABLE *table, char *out);
int genScore(int best, int diff_best, int duration, int wea_thresh, int abs_thresh, int inc_thresh, int rel_thresh);
