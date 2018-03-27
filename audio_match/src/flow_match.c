/*************************************************************************
    > File Name: flow_match.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-22 14:46:41
 ************************************************************************/

#include<stdio.h>
#include "match_utils.h"

#define MAX_NUM_ADNA 1000

void help()
{
    fprintf(stderr, "Demostrate the process of flow match.\n");
    fprintf(stderr, "Split pcm into segments then genadna to match in the table.\n");
    fprintf(stderr, "Input master.list and pcm file.\n");
    fprintf(stderr,"\t./bin master.list pcm\n");
}
int main( int argc, char *argv[] )
{
    if( argc != 3 ) {
        fprintf(stderr,"ERROR, Input error !\n");
        help();
        exit(EXIT_FAILURE);
    }
    FILE *fm = fopen(argv[1], "r");
    if (fm == NULL) {
        fprintf(stderr, "Open %s failed !\n", argv[1]);
        return -1;
    }
    char str[200];
    char master[MAX_NUM_ADNA][200];
    int cnt1 = 0;
    while (fgets(str, 200, fm) != NULL && cnt1 < MAX_NUM_ADNA) {
        memset(str + strlen(str)-1, 0, 1);
        strcpy(master[cnt1], str);
        cnt1 ++;
    }
    fclose(fm);
    fprintf(stdout,"Find %d Adnas !\n", cnt1);

    fprintf(stdout,"Push Adnas to HASHTABLE ...\n");
    
    //  1 Build HASHTABLE.
    HASHTABLE *table = (HASHTABLE *)malloc( sizeof(HASHTABLE) );
    build_hashtable(table);

    unsigned int max_num = 10000;
    LANDMARK *LM = (LANDMARK *)malloc(max_num * sizeof(LANDMARK));
    
    //  2 Read adna and add to table.
    for(int i = 0; i < cnt1; i ++) {
        FILE *flm = fopen(master[i], "rb");
        if( flm == NULL ) {
            fprintf(stderr,"ERROR, Open %s error !\n", master[i]);
            return -1;
        }
        
        fseek(flm, 0, SEEK_END);
        unsigned int size = ftell(flm);
        fseek(flm, 0, SEEK_SET);
        unsigned int num = size / ADNA_SIZE;
    
        while( num > max_num ) {
            max_num *= 2;
            LM = reallocf(LM, max_num * sizeof(LANDMARK));
            if( LM == NULL ) {
                fprintf(stderr,"Realloc memory error, %s\n", master[i]);
                exit(EXIT_FAILURE);
            }
        }
        unsigned int nLmst = AfpReadFeatures( (unsigned char *)LM, num, flm );
        if( nLmst != num ) {
            fprintf(stderr, "Load %s faild !", master[i]);
            exit(EXIT_FAILURE);
        }
        add_track(LM, nLmst, i, table);
        fclose(flm);
    }
    fprintf(stdout,"Push To HASHTABLE Done.\n");

    FILE *fsmp = fopen(argv[2], "rb");
    fseek(fsmp, 0, SEEK_END);
    unsigned int size = ftell(fsmp);
    fseek(fsmp, 0, SEEK_SET);
    unsigned char *pcmBuf = (unsigned char *)malloc( size );
    fread(pcmBuf, 1, size, fsmp);
    fclose(fsmp);

    int oneStep = 16000; // 1 second.
    int oneStepSec = oneStep / 8000 / 2;
    int oneLen = 5 * 16000; // 5 second.
    int audioLen = oneLen / 8000 / 2; // 5 second;
    int maxNum = oneLen / 16 / 32 * 100; // LANDMARKs.
    int maxMatch = size / oneStep + 1;
    unsigned char *featureBuf = (unsigned char *)malloc( maxNum * ADNA_SIZE );

    int channels = 1;
    int sampleRate = 8000;
    int sampleSize = 2;
    int bufLen = AFP_BLOCK_LENGTH;

    int stepCnt = 0;
    MATCH_RESULT *R = (MATCH_RESULT *)calloc(10, sizeof(MATCH_RESULT));
    FLOW_MATCH_RESULT *flowR = (FLOW_MATCH_RESULT *)calloc(maxMatch, sizeof(FLOW_MATCH_RESULT));
    int wea_thresh = WEA_THRESH;
    int abs_thresh = ABS_THRESH;
    int inc_thresh = INC_THRESH;
    int rel_thresh = REL_THRESH;
    
    /* Output match result */
    FILE *fout = fopen("FlowScore.txt","w");
    if( fout == NULL ) {
        fprintf(stderr, "Open FlowScore.txt error !\n");
        return - 1;
    }

    int flag = 1;
    while(flag)
    {
        if( stepCnt * oneStep + oneLen > size) break;
        AfpCtx *ctx = AfpInit(sampleRate, sampleSize, channels, bufLen, AFP_BLOCK_STEP);
        if( ctx == NULL ) {
            perror("afp4_open.\n");
            exit(EXIT_FAILURE);
        }
        int nFeatures = AfpPushFrame(pcmBuf+stepCnt*oneStep, oneLen, 0.0, ctx, featureBuf);
        LANDMARK *LM = (LANDMARK *)featureBuf;
        memset(R, 0, 10 * sizeof(MATCH_RESULT));
        int nR = match_query(LM, nFeatures, R, table);
        if( nR < 1 ) {
            flowR[stepCnt].ID = -1;
            flowR[stepCnt].score = 0;
        }
        else {
            int diff_best = nR > 1 ? R[0].best - R[1].best : R[0].best;
            //int audioLen = (int)( ( LM[nFeatures - 1].t1 - LM[0].t1 ) * 0.032 + 0.5);
            int score = genScore(R[0].best, diff_best, audioLen, wea_thresh, abs_thresh, inc_thresh, rel_thresh);
            flowR[stepCnt].ID = R[0].ID;
            flowR[stepCnt].score = score;
        }
        flowR[stepCnt].sseg = stepCnt * oneStepSec;
        flowR[stepCnt].eseg = stepCnt * oneStepSec + audioLen;
        fprintf(fout,"[%9.3lf, %9.3lf]; seg_len = %d; step_len = %d; ID = %4d; score = %d\n", 
                flowR[stepCnt].sseg, flowR[stepCnt].eseg, audioLen, oneStepSec, flowR[stepCnt].ID, flowR[stepCnt].score);
        stepCnt ++;
        AfpClose(&ctx);
    }

    /*
     * last segment
     */
    if( stepCnt * oneStep < size ) {
        int lastPushLen = size - stepCnt * oneStep;
        int duration = lastPushLen / 8000 / 2;
        AfpCtx *ctx = AfpInit(sampleRate, sampleSize, channels, bufLen, AFP_BLOCK_STEP);
        if( ctx == NULL ) {
            perror("afp4_open.\n");
            exit(EXIT_FAILURE);
        }
        int nFeatures = AfpPushFrame(pcmBuf+stepCnt*oneStep, lastPushLen, 0.0, ctx, featureBuf);
        LANDMARK *LM = (LANDMARK *)featureBuf;
        memset(R, 0, 10 * sizeof(MATCH_RESULT));
        int nR = match_query(LM, nFeatures, R, table);
        //int duration = (int)( (LM[nFeatures - 1].t1 - LM[0].t1) * 0.032 + 0.5);
        if( nR < 1 ) {
            flowR[stepCnt].ID = -1;
            flowR[stepCnt].score = 0;
        }
        else {
            int diff_best = nR > 1 ? R[0].best - R[1].best : R[0].best;
            int score = genScore(R[0].best, diff_best, duration, wea_thresh, abs_thresh, inc_thresh, rel_thresh);
            flowR[stepCnt].ID = R[0].ID;
            flowR[stepCnt].score = score;
        }
        flowR[stepCnt].sseg = stepCnt * oneStepSec;
        flowR[stepCnt].eseg = stepCnt * oneStepSec + duration;
        fprintf(fout,"[%9.3lf, %9.3lf]; seg_len = %d; step_len = %d; ID = %4d; score = %d\n", 
                flowR[stepCnt].sseg, flowR[stepCnt].eseg, duration, oneStepSec, flowR[stepCnt].ID, flowR[stepCnt].score);
        stepCnt ++;
        AfpClose(&ctx);
    }
    fclose(fout);

    /* Match from start */
    /*
    if( flowR[0].score > 4 ) {
        fprintf(stdout, "Match from start: ID = %d, steps = 0, time = %ds.\n", flowR[0].ID, audioLen );
    }
    else {
        int n = 1;
        int score = flowR[0].score;
        for( int i = 1; i < stepCnt; i ++, n ++ ) {
            score += flowR[i].score;
            if( score > 4 ) break;
        }
        if( score > 4 )
            fprintf(stdout, "Match from start: ID = %d, steps = %d, time = %ds.\n", flowR[0].ID, n, audioLen + n );
        else
            fprintf(stdout, "No match!\n");
    }
    */
    /* Match from start */
    if( flowR[0].score > 4 ) {
        fprintf(stdout, "Match from start: From %.3lfs to %.3lfs, ID = %d, continues = 1, score = %d.\n", 
                flowR[0].sseg, flowR[0].eseg, flowR[0].ID, flowR[0].score );
    }
    else {
        int n = 1, start = 0;
        int score = flowR[0].score;
        int ID = flowR[0].ID;
        for( int i = 1; i < stepCnt; i ++, n ++) {
            //score += flowR[i].score;
            if( flowR[i].ID == ID ) {
                score += flowR[i].score;
            }
            else {
                score = flowR[i].score;
                ID = flowR[i].ID;
                start = i;
            }
            if( score > 4 ) break;
        }
        if( score > 4 )
            fprintf(stdout, "Match from start: From %.3lfs to %.3lfs, ID = %d, continues = %d, score = %d.\n", 
                    flowR[start].sseg, flowR[n].eseg, ID, n - start + 1, score);
        else
            fprintf(stdout, "No match!\n");
    }
    /* Find all match */


    free(pcmBuf);
    free(featureBuf);
    free(R);
    free(flowR);
    return 0;
}
