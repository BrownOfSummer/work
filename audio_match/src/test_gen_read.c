/*************************************************************************
    > File Name: test_skew.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-13 09:59:07
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include "afpWrap.h"
#include "utils.h"

void help()
{
    fprintf(stdout, "Demonstrate the difference between read and generate adna.\n");
    fprintf(stdout, "Input a pcm, generate adna, some process done in function-> AfpWriteFeatures.\n");
    fprintf(stdout, "When do read, the process is reverse, with the help of ctx.\n");
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
    int maxNum = size / 16 / 32 * 100; // size /2/8000/0.032 * 100
    unsigned char *pcmBuf = (unsigned char *)malloc(size);
    unsigned char *featureBuf = (unsigned char *)malloc( maxNum * sizeof(LANDMARK) );

    if(!pcmBuf || !featureBuf) {
        fclose(fpcm);
        fprintf(stderr, "ERROR, malloc buffer.\n");
        return -1;
    }

    size = fread(pcmBuf, 1, size, fpcm);
    fclose(fpcm);

    // set default parameters
    int channels = 1;
    int sampleRate = 8000;
    int sampleSize = 2;
    int bufLen = AFP_BLOCK_LENGTH;
    fprintf(stdout, "channels = %d, sampleRate = %d, sampleSize = %d, pcmSize = %lu\n", channels, sampleRate, sampleSize, size);

    // generate and save adna
    AfpCtx *ctx = NULL;
    double time = 0.0;
    ctx = AfpInit(sampleRate, sampleSize, channels, bufLen, AFP_BLOCK_STEP);
    FILE *fout = fopen("temp.adna", "wb");
    int nFeatures = AfpPushFrame(pcmBuf, size, time, ctx, featureBuf);
    if (nFeatures != AfpWriteFeatures(ctx, featureBuf, nFeatures, fout))
        fprintf(stderr, "write adna mistakes!\n");
    fprintf(stdout, "features num = %d\n", nFeatures);
    fclose(fout);
    // analysis the read process and the origin adna.

    FILE *flm = fopen("temp.adna", "rb");
    if( flm == NULL ) {
        fprintf(stderr, "ERROR, Open adna file error !\n");
        return -1;
    }
    fseek(flm, 0, SEEK_END);
    size = ftell(flm);
    fseek(flm, 0, SEEK_SET);
    unsigned int num = size / ADNA_SIZE;
    LANDMARK *LM = (LANDMARK *)malloc( num * sizeof(LANDMARK) );
    unsigned int nL = AfpReadFeatures( (unsigned char *)LM, num, flm );

    fprintf(stdout, "Generate %d features, Load %d features.\n", nFeatures, nL);
    if( nFeatures != nL ) {
        fprintf(stderr, "features not equal!\n");
        return -1;
    }
    
    // print adna
    //////////////////////////////////////////////////////////////////////////////////////////
    // Some process in done when AfpWriteFeatures, we do recerse in AfpReadFeatures;//////////
    // When the ctx in init with AfpInit instead of Load from file, Do not need to reverse.///
    // ///////////////////////////////////////////////////////////////////////////////////////
    /*
    LANDMARK *lm = (LANDMARK *)featureBuf;
    int32_t ret = 0;
    for(ret = 0; ret < nFeatures; ret++){
        uint64_t fourMs = lm[ret].t1 * 8 + (uint64_t)ctx->secTimestamp * 250;
        uint8_t fourMsRemind = fourMs % 250;
        uint32_t sec = fourMs / 250;
        lm[ret].t1 = ( (uint64_t)sec *250 + (uint64_t)fourMsRemind ) / 8;
    }
    lm=NULL;
    */
    LANDMARK *GenLm = (LANDMARK *)featureBuf;
    for( int i = 0; i < nL; i ++ ) {
        if( GenLm[i].t1 != LM[i].t1 ||
            GenLm[i].dt != LM[i].dt ||
            GenLm[i].f1 != LM[i].f1 ||
            GenLm[i].f2 != LM[i].f2) {
            fprintf(stdout, "----%llu %d %u %u \t\t %llu %d %u %u\n",
                    GenLm[i].t1, GenLm[i].dt, GenLm[i].f1, GenLm[i].f2,
                    LM[i].t1, LM[i].dt, LM[i].f1, LM[i].f2);
        }
        else{
            fprintf(stdout, "\t%llu %d %u %u \t\t %llu %d %u %u\n",
                    GenLm[i].t1, GenLm[i].dt, GenLm[i].f1, GenLm[i].f2,
                    LM[i].t1, LM[i].dt, LM[i].f1, LM[i].f2);
        }
    }

    AfpClose(&ctx);
    free(pcmBuf);
    free(featureBuf);
    free(LM);
    return 0;
}

