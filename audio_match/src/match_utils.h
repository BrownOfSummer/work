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
//#include "afpWrap.h"
#include "afp4_wrap.h"

#include "hashtable.h"
#include "global.h"
#include "adna_utils.h"

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
#define END_THRESH 5
#define CHANNEL_TABLE_SIZE 200
/*
typedef struct {
    int id;
    int lv;
} Rcd;
*/
typedef struct {
    int ID;
    int score;
    double sseg; // start segment
    double eseg; // end segment
} FLOW_MATCH_RESULT;

typedef struct _stringList {
    char element[200];
    struct _stringList *next;
} stringList;

typedef struct _CHANNEL_INFO{
    int ID;
    int adnaFileNum;
    char channelName[200];
    stringList *adnaFilePath;
}CHANNEL_INFO;

typedef struct {
    CHANNEL_INFO **items;
} CHANNEL_TABLE;

int match_or_not( const LANDMARK *LM, const unsigned int nL,  MATCH_RESULT *R, const HASHTABLE *table, int wea_thresh, int abs_thresh, int inc_thresh, int rel_thresh);
unsigned int cnt_forward(const LANDMARK *LM, const unsigned int nL, unsigned int startIndex, unsigned int oneStepFrames);
int cmp_seg_match(const void *a1, const void *b1);
uint64_t print_table_cnt(const HASHTABLE *table, char *out);
int genScore(int best, int diff_best, int duration, int wea_thresh, int abs_thresh, int inc_thresh, int rel_thresh);


void init_channelTable(CHANNEL_TABLE *channelTable);
void add_channel_dict(CHANNEL_TABLE *channelTable, const char * channelInfoList);
void add_adna_path(CHANNEL_TABLE *channelTable, const char *pathList);
void del_channelTable(CHANNEL_TABLE *channelTable);
void build_channelTable(CHANNEL_TABLE *channelTable, const char* channelInfoList, const char *pathList);
void print_channelTable(CHANNEL_TABLE *channelTable);
void traversalOncePrint(CHANNEL_TABLE *channelTable, int startPoint);
void traversalOnce(CHANNEL_TABLE *channelTable, int startPoint, const stringList ** paths);
