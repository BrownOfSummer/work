/*************************************************************************
    > File Name: similar.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-15 14:21:03
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
    fprintf(stderr, "Demonstrate the similarity between two adna.\n");
    fprintf(stderr, "Input two adna, first is the origin one, the second one is copy.\n");
    fprintf(stderr, "Split the origin and copy into segments, and do match one on one.\n");
    fprintf(stderr, "\t./bin origin.adna copy.adna segment_len(s)\n");
}

int main(int argc, char* argv[])
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
    /* Align the two adna */
    /* omission here */
    
    double secs = atof(argv[3]);
    double time_len = (Lm1[nL1 - 1].t1 < Lm2[nL2 - 1].t1 ? Lm1[nL1 - 1].t1 : Lm2[nL2 - 1].t1) * 0.032;
    if( secs > ceil(time_len) ){
        fprintf(stderr, "ERROR, Intend to get %.3lfs each, but only has %.3lfs\n", secs, time_len);
        return -1;
    }

    fprintf(stdout, "First  audio: [%.3lf]s, %u landmarks.\n", Lm1[nL1 - 1].t1 * 0.032, nL1);
    fprintf(stdout, "Second audio: [%.3lf]s, %u landmarks.\n", Lm2[nL2 - 1].t1 * 0.032, nL2);

    /* Build HASHTABLE */
    HASHTABLE * table = (HASHTABLE *)malloc( sizeof(HASHTABLE) );
    build_hashtable(table);

    /* Split two adna into same length segment and do match */
    unsigned int oneSegFrames = (unsigned int)( secs / 0.032 );
    unsigned int oneStepFrames = ONE_STEP_FRAMES;
    MATCH_RESULT *R = (MATCH_RESULT *)calloc(10, sizeof(MATCH_RESULT));
    unsigned int tmpLen = nL1 > nL2 ? nL1 : nL2;
    LANDMARK *tmpLm = (LANDMARK *)malloc(tmpLen * sizeof(LANDMARK));
    unsigned int startIndex1 = 0, startIndex2 = 0;
    unsigned int last_index1 = 0, last_index2 = 0;
    unsigned int stepCnt = 0;
    bool flag = true;
    
    /* Output match result */
    FILE *fout = fopen("Similarity.txt","w");
    if( fout == NULL ) {
        fprintf(stderr, "Open Dissimilarity.txt error !\n");
        return - 1;
    }

    while(flag)
    {
        clear_hashtable(table);
        unsigned int bufLen1 = cnt_forward(Lm1, nL1, startIndex1, oneSegFrames);
        if( bufLen1 < 1 ) break;
        if( startIndex1 + bufLen1 >= nL1 - 1 ) flag = false; //last segment
        for( unsigned int i = 0; i < bufLen1; i ++ ) {
            tmpLm[i] = Lm1[startIndex1 + i];
        }
        add_track(tmpLm, bufLen1, 1, table);
        double seg_len1 = (tmpLm[bufLen1 - 1].t1 - tmpLm[0].t1) * 0.032;
        double step_len1 = (Lm1[startIndex1].t1 - Lm1[last_index1].t1) * 0.032;
        double sseg1 = tmpLm[0].t1 * 0.032;
        double eseg1 = tmpLm[bufLen1 - 1].t1 * 0.032;

        unsigned int bufLen2 = cnt_forward(Lm2, nL2, startIndex2, oneSegFrames);
        if( bufLen2 < 1 ) break;
        if( startIndex2 + bufLen2 >= nL2 - 1 ) flag = false; //last segment
        for(unsigned int i = 0; i < bufLen2; i ++) {
            tmpLm[i] = Lm2[startIndex2 + i];
        }
        int nR = match_query(tmpLm, bufLen2, R, table);
        double seg_len2 = (tmpLm[bufLen2 - 1].t1 - tmpLm[0].t1) *0.032;
        double step_len2 = (Lm2[startIndex2].t1 - Lm2[last_index2].t1) * 0.032;
        double sseg2 = tmpLm[0].t1 * 0.032;
        double eseg2 = tmpLm[bufLen2 - 1].t1 * 0.032;
        double start_diff = Lm1[startIndex1].t1 * 0.032 - Lm2[startIndex2].t1 * 0.032;
        double step_diff = step_len1 - step_len2;

        int index_hit = 0;
        if( nR > 0 ) index_hit = R[0].best;
        //double score = (double)(index_hit) / MAX(bufLen1, bufLen2) * 100;
        double score = (double)(index_hit) / (bufLen2) * 100;
        memset(R, 0, 10 * sizeof(MATCH_RESULT));

        //unsigned int steps_forward1 = cnt_forward(Lm1, nL1, startIndex1, oneStepFrames);
        //unsigned int steps_forward2 = cnt_forward(Lm2, nL2, startIndex2, oneStepFrames);
        //if( steps_forward1 < 1 || steps_forward2 < 1 ) break;
        //else {
            last_index1 = startIndex1;
            last_index2 = startIndex2;
            //startIndex1 += steps_forward1;
            //startIndex2 += steps_forward2;
            stepCnt ++;
            startIndex1 = cnt_forward(Lm1, nL1, 0, stepCnt * oneStepFrames);
            startIndex2 = cnt_forward(Lm2, nL2, 0, stepCnt * oneStepFrames);
        //}
        fprintf(fout,"[%9.3lf, %9.3lf]; seg_len1 = %6.3lf; step_len1 = %6.3lf; [%9.3lf, %9.3lf]; seg_len2 = %6.3lf; step_len2 = %6.3lf; start_diff = %6.3lf; step_diff = %6.3lf; score = %7.4lf; %4d\n", 
                sseg1, eseg1, seg_len1, step_len1, sseg2, eseg2, seg_len2, step_len2, start_diff, step_diff, score, index_hit);
    }
    fprintf(stdout, "Step forward %u times, %lfs.\n", stepCnt, stepCnt * ONE_STEP_FRAMES * 0.032);
    
    fclose(fout);
    free(Lm1);
    free(Lm2);
    free(tmpLm);
    free(R);
    del_hashtable(table);
    free(table);
    return 0;
}

