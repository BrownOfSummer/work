/*************************************************************************
    > File Name: dissimilar.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-14 16:55:03
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include "match_utils.h"

#define ONE_STEP_FRAMES 31 // 31 * 0.032 = 0.992s

void help()
{
    fprintf(stderr, "Demonstrate the dissimilarity between two adna.\n");
    fprintf(stderr, "Input two adna, usually the first is more longer.\n");
    fprintf(stderr, "Get some adna from second and push into table, according to 3rd input.\n");
    fprintf(stderr, "Split the first adna and match !\n");
    fprintf(stderr, "\t./bin long.adna short.adna segment_len(s).\n");
}

int main(int argc, char *argv[])
{
    
    ///////////////* Load ADNA *//////////////
    if( (argc != 4)  ) {
        fprintf(stdout, "\nERROR, input error !\n\n");
        help();
        return -1;
    }
    FILE *fl = fopen(argv[1], "rb");
    if( !fl ) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        help();
        return -1;
    }
    FILE *fs = fopen(argv[2], "rb");
    if( !fs ) {
        fprintf(stderr, "Cannot open %s\n", argv[2]);
        help();
        return -1;
    }

    fseek(fl, 0, SEEK_END);
    unsigned int size = ftell(fl);
    fseek(fl, 0, SEEK_SET);
    unsigned int numl = size / ADNA_SIZE;

    fseek(fs, 0, SEEK_END);
    size = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    unsigned int nums = size / ADNA_SIZE;

    LANDMARK *Lm1 = (LANDMARK *)malloc( numl * sizeof( LANDMARK ) );
    LANDMARK *Lm2 = (LANDMARK *)malloc( nums * sizeof( LANDMARK ) );

    unsigned int nL1 = AfpReadFeatures((unsigned char *)Lm1, numl, fl);
    unsigned int nL2 = AfpReadFeatures((unsigned char *)Lm2, nums, fs);
    fclose(fl);
    fclose(fs);

    /* Crop Short adna according argv[3] */
    unsigned int num_lm_to_push = nL2;
    double secs = atof(argv[3]);
    double time_len = Lm2[nL2 - 1].t1 * 0.032;
    if( secs > ceil(time_len) ) {
        fprintf(stderr, "ERROR, Intend to get %.3lfs from sample, but only has %.3lfs\n", secs, time_len);
        return - 1;
    }
    else {
        for( unsigned int i = 0; i < nL2; i ++ ) {
            if(Lm2[i].t1 * 0.032 >= secs) {
                num_lm_to_push = i + 1;
                break;
            }
        }
    }
    num_lm_to_push = nL2 > num_lm_to_push ? num_lm_to_push : nL2;
    fprintf(stdout, "Long audio : [%.3lf]s, %u landmarks.\n", Lm1[nL1 - 1].t1 * 0.032, nL1);
    fprintf(stdout, "Short audio: [%.3lf]s, %u landmarks.\n", Lm2[nL2 - 1].t1 * 0.032, nL2);
    fprintf(stdout, "Crop Short into [%.3lf]s from Short audio, Push %u landmarks.\n", Lm2[num_lm_to_push - 1].t1 * 0.032, num_lm_to_push);

    if( num_lm_to_push < 5 ) {
        fprintf(stderr, "Too few landmarks in segment audio !\n");
        return -1;
    }

    /* Build HASHTABLE */
    HASHTABLE *table = (HASHTABLE *)malloc( sizeof(HASHTABLE) );
    build_hashtable(table);
    add_track(Lm2, num_lm_to_push, 1, table);

    /* Split long.adna and match */
    unsigned int oneSegFrames = (unsigned int)(secs / 0.032); // frames for each segment.
    unsigned int oneStepFrames = ONE_STEP_FRAMES; // step forward frames once, 31 * 0.032 = 0.992s.
    //unsigned int max_seg_num = Lm1[ nL1 - 1 ].t1 / oneStepFrames + 1;
    //MATCH_RESULT *seg_match_result = (MATCH_RESULT *)malloc( max_seg_num * sizeof(MATCH_RESULT) );
    MATCH_RESULT *R = (MATCH_RESULT *)calloc(10, sizeof(MATCH_RESULT));
    LANDMARK * tmpLm = (LANDMARK *)malloc(nL1 * sizeof(LANDMARK) );
    unsigned int start_Index = 0; 
    unsigned int last_index = 0;
    unsigned int stepCnt = 0;
    bool flag = true;
    /* Output match result */
    FILE *fout = fopen("Dissimilarity.txt","w");
    if( fout == NULL ) {
        fprintf(stderr, "Open Dissimilarity.txt error !\n");
        return - 1;
    }
    while( flag )
    {
        unsigned int bufLen = cnt_forward(Lm1, nL1, start_Index, oneSegFrames);
        if( bufLen < 1 ) break;
        /*
        if( start_Index + bufLen - 1 > nL1 - 1 ) {
            fprintf(stderr, "ERROR, cnt_forward error, out of bounds !\n");
            break;
        }
        */
        if( start_Index + bufLen >= nL1 - 1 ) flag = false;
        for( unsigned int i = 0; i < bufLen; i ++ ) {
            tmpLm[i] = Lm1[start_Index + i];
        }
        int nR = match_query(tmpLm, bufLen, R, table);
        double seg_len = ( tmpLm[bufLen - 1].t1 - tmpLm[0].t1 ) * 0.032; 
        double step_len = ( Lm1[start_Index].t1 - Lm1[last_index].t1 ) * 0.032; 
        int index_hit = 0;
        if( nR > 0 ) {
            index_hit = R[0].best;
        }
        double score = (double)(index_hit) / num_lm_to_push * 100;
        memset(R, 0, 10 * sizeof(MATCH_RESULT));
        last_index = start_Index;
        //start_Index += steps_forward;
        stepCnt ++;
        start_Index = cnt_forward(Lm1, nL1, 0, stepCnt * oneStepFrames);
        fprintf(fout,"[%9.3lf, %9.3lf]; seg_len = %6.3lf; step_len = %6.3lf; score = %6.3lf; %4d\n", 
                tmpLm[0].t1 * 0.032, tmpLm[bufLen - 1].t1 * 0.032, seg_len, step_len,  score, index_hit);
    }
    

    fclose(fout);
    del_hashtable(table);
    free(table);
    free(Lm1);
    free(Lm2);
    //free(seg_match_result);
    free(R);
    free(tmpLm);
    return 0;
}

