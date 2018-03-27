/*************************************************************************
    > File Name: mSyncDistinc_adna.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn 
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2016-11-22 14:18:14
 ************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "match_utils.h"
void help()
{
    fprintf(stderr, "Demonstrate the seg_match metheod of two adna.\n");
    fprintf(stderr, "Input long.adna and short.adna, push the short into table.\n");
    fprintf(stderr, "The long one is split into segment to search in table.\n");
    fprintf(stderr, "\t./bin long.adna short.adna\n");
}
int main(int argc, char *argv[])
{
    if( argc < 3 ){
        fprintf(stderr, "Input file error\n");
        help();
        return -1;
    }
    int wea_thresh = 3;
    int abs_thresh = 7;
    int inc_thresh = 2;
    int rel_thresh = 4;
    //int start = 3;
    //int end = 5;

    HASHTABLE *table = (HASHTABLE *)malloc(sizeof(HASHTABLE));
    build_hashtable(table);
    FILE *fmst = fopen(argv[1], "rb");
    if (fmst == NULL){
        fprintf(stderr, "cannot open firstSample\n");
        return -1;
    }
    fseek(fmst, 0, SEEK_END);
    unsigned int size = ftell(fmst);
    fseek(fmst, 0, SEEK_SET);
    int num_mst = size / ADNA_SIZE;
    LANDMARK *Lmst = malloc(num_mst*sizeof(LANDMARK));
    LANDMARK *tmpLm = malloc(num_mst * sizeof(LANDMARK));
    unsigned int nLmst = AfpReadFeatures((unsigned char *)Lmst, num_mst, fmst);
    fclose(fmst);

    FILE *fsmp = fopen(argv[2], "rb");
    if (fsmp == NULL) {
        fprintf(stderr, "cannot open secondSample\n");
        return -1;
    }
    fseek(fsmp, 0, SEEK_END);
    size = ftell(fsmp);
    fseek(fsmp, 0, SEEK_SET);
    int num = size / ADNA_SIZE;
    LANDMARK *Lsmp = (LANDMARK *)malloc(num*sizeof(LANDMARK));
    unsigned int nLsmp = AfpReadFeatures((unsigned char *)Lsmp, num, fsmp);
    add_track(Lsmp, nLsmp, 1, table);
    fclose(fsmp);

    unsigned int time_len = 100; //100 frames is for 3.2s
    //unsigned int time_len = Lsmp[nLsmp - 1].t1 - Lsmp[0].t1;
    //unsigned int msttime_len = Lmst[nLmst - 1].t1 - Lmst[0].t1;
    //fprintf(stdout, "first_sample_len = %.3lfs;\nsecond_sample_len = %.3lfs\n", msttime_len * 0.032, time_len * 0.032);
    unsigned int i = 0, start_Index = 0, flag = 1;
    unsigned max_seg_num = Lmst[nLmst - 1].t1 / 32 + 1;
    MATCH_RESULT *seg_match_result = (MATCH_RESULT *)malloc(max_seg_num * sizeof(MATCH_RESULT));
    unsigned count_match = 0;

    MATCH_RESULT *R = (MATCH_RESULT *)calloc(10, sizeof(MATCH_RESULT));
    while( flag )
    {
        unsigned int bufLen = cnt_forward(Lmst, nLmst, start_Index, time_len);
        if( start_Index + bufLen >= nLmst - 1 )
            flag = 0;
        for( i = 0; i < bufLen; i ++ ){
            tmpLm[i] = Lmst[start_Index + i];
        }
        //MATCH_RESULT *R = (MATCH_RESULT *)calloc(10, sizeof(MATCH_RESULT));
        int n = match_or_not( tmpLm, bufLen, R, table, wea_thresh, abs_thresh, inc_thresh, rel_thresh );
        if( n > 0 ){
            seg_match_result[count_match].best = R[0].best;
            seg_match_result[count_match].skew = R[0].skew;
            count_match ++;
        }
        //free(R);
        memset(R, 0, 10*sizeof(MATCH_RESULT));
        unsigned int steps_forward = cnt_forward(Lmst, nLmst, start_Index, 32); //32 frames for 32*32/1000 = 1.024s
        if( steps_forward == 0 )
            break;
        else
            start_Index += steps_forward;
    }
    if(count_match > 1)
        qsort(seg_match_result, count_match, sizeof(MATCH_RESULT), cmp_seg_match);
    if(count_match > 0)
        for(i = 0; i < count_match; i ++)
            fprintf(stdout, "score = %d, skew = %lld, skew_seconds = %.3lfs\n", seg_match_result[i].best, seg_match_result[i].skew, seg_match_result[i].skew*0.032);
    else
        fprintf(stdout, "No match\n");
 
    free(R);
    free(Lmst);
    free(Lsmp);
    free(tmpLm);
    del_hashtable(table);
    free(table);
    free(seg_match_result);
    return 0;
}
