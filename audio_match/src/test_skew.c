/*************************************************************************
    > File Name: test_skew.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-13 09:59:07
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "afpWrap.h"
#include "utils.h"
#include "genafp.h"

#define APPEND_SEC 6
//#define SMALL_APPEND_SEC  APPEND_SEC / 3
#define SMALL_APPEND_SEC 1

void help()
{
    fprintf(stdout, "Demonstrate the meaning of skew in match result.\n");
    fprintf(stdout, "Input a pcm, appending zero before or after the pcm.\n");
    fprintf(stdout, "\t\t./bin f.pcm\n");
}

int main(int argc, char *argv[])
{
    if( argc != 2 ) {
        help();
        return -1;
    }
    FILE *fpcm = fopen(argv[1], "rb");
    fseek(fpcm, 0, SEEK_END);
    unsigned long size = ftell(fpcm);
    fseek(fpcm, 0, SEEK_SET);
    unsigned char *pcmBuf = (unsigned char *)malloc(size);
    size = fread(pcmBuf, 1, size, fpcm);
    unsigned long append_bytes = APPEND_SEC * 8000 * 2;
    unsigned long small_append_bytes = SMALL_APPEND_SEC * 8000 * 2;
    unsigned char *before = (unsigned char *)calloc( append_bytes + size, 1 ); // [3s zeros --- pcm]
    unsigned char *after = (unsigned char *)calloc( append_bytes + size, 1 ); // [pcm --- 3s zeros]
    unsigned char *master = (unsigned char *)calloc( small_append_bytes*3 + size, 1 ); // [1s zeros -- pcm -- 2s zeros]
    for( uint64_t i = 0; i < size; i ++ ) {
        before[ append_bytes + i ] = pcmBuf[i];
        after[i] = pcmBuf[i];
        master[ small_append_bytes + i ] = pcmBuf[i];
    }
    
    int maxNum = ( append_bytes + size ) / 16 / 32 * 100; // size /2/8000/0.032 * 100
    unsigned char *featureBuf = (unsigned char *)malloc( maxNum * sizeof(LANDMARK) );
    //fprintf(stdout,"%X\n",&featureBuf[0]);

    // set default parameters
    int channels = 1;
    int sampleRate = 8000;
    int sampleSize = 2;
    int bufLen = AFP_BLOCK_LENGTH;
    fprintf(stdout, "channels = %d, sampleRate = %d, sampleSize = %d, pcmSize = %lu\n", channels, sampleRate, sampleSize, size);
    AfpCtx *ctx = NULL;
    double time = 0.0;
    ctx = AfpInit(sampleRate, sampleSize, channels, bufLen, AFP_BLOCK_STEP);
    //int nFeatures = AfpPushFrame(pcmBuf, size, time, ctx, featureBuf);
    int nFeatures = AfpPushFrame(master, size + small_append_bytes * 3, time, ctx, featureBuf);
    fprintf(stdout, "\tMaster[%ds zeros--pcm-- %ds zeros] Get %d features.\n",SMALL_APPEND_SEC, SMALL_APPEND_SEC * 2, nFeatures);
    
    
    // build HASHTABLE
    HASHTABLE *table = (HASHTABLE *)malloc( sizeof(HASHTABLE) );
    build_hashtable(table);
    LANDMARK *LM = (LANDMARK *)featureBuf;
    add_track(LM, nFeatures, 1, table);
    memset(featureBuf, 0, maxNum * sizeof(LANDMARK));
    //fprintf(stdout,"%X\n",&featureBuf[0]);
    
    LM = NULL;
    AfpClose(&ctx);
    ctx=NULL;
    ctx = AfpInit(sampleRate, sampleSize, channels, bufLen, AFP_BLOCK_STEP);
    int nL1 = AfpPushFrame(before, append_bytes + size, time, ctx, featureBuf);
    MATCH_RESULT *R = (MATCH_RESULT *)calloc( 10, sizeof(MATCH_RESULT) );
    LM = (LANDMARK *)featureBuf;
    int nR = match_query(LM, nL1, R, table);
    if( nR > 0 ) {
        for(int i = 0; i < nR; i ++)
            fprintf(stdout, "\tBeforePcm[%ds zeros--pcm] get %d features, best = %d, skew(s) = %.3lfs.\n", APPEND_SEC, nL1, R[0].best, R[0].skew * 0.032);
    }
    memset(featureBuf, 0, maxNum * sizeof(LANDMARK));
    //fprintf(stdout,"%X\n",&featureBuf[0]);

    LM = NULL;
    AfpClose(&ctx);
    ctx=NULL;
    memset(R, 0, 10*sizeof(MATCH_RESULT));
    ctx = AfpInit(sampleRate, sampleSize, channels, bufLen, AFP_BLOCK_STEP);
    int nL2 = AfpPushFrame(after, append_bytes + size, time, ctx, featureBuf);
    LM = (LANDMARK *)featureBuf;
    nR = match_query(LM, nL2, R, table);
    if( nR > 0 ) {
        for(int i = 0; i < nR; i ++)
            fprintf(stdout, "\tAfterPcm [pcm--%ds zeros] get %d features, best = %d, skew(s) = %.3lfs.\n", APPEND_SEC, nL2, R[0].best, R[0].skew * 0.032);
    }

    //fprintf(stdout,"%X\n",&featureBuf[0]);
    free(before);
    free(after);
    free(pcmBuf);
    free(featureBuf);
    free(R);
    del_hashtable(table);
    free(table);
    return 0;
}

