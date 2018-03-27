/*************************************************************************
    > File Name: flow_match_adna.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-23 08:59:32
 ************************************************************************/

#include<stdio.h>
#include "match_utils.h"

#define MAX_NUM_ADNA 1000
void help()
{
    fprintf(stderr, "Demostrate the process of flow match.\n");
    fprintf(stderr, "Split sample.adna into segments then genadna to match in the table.\n");
    fprintf(stderr, "Input master.list and sample.adna file.\n");
    fprintf(stderr,"\t./bin master.list sample.adna\n");
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
    
    unsigned int num = size / ADNA_SIZE;
    LANDMARK *Lm = (LANDMARK *)malloc( num * sizeof(LANDMARK) );
    LANDMARK *tmpLm = (LANDMARK *)malloc( num * sizeof(LANDMARK) );
    unsigned int nL = AfpReadFeatures((unsigned char *)Lm, num, fsmp);

    unsigned int oneStepFrames = 32; // 32 * 0.032 = 1.024s
    unsigned int oneSegFrames = 156; // 156 * 0.032 = 4.992s
    unsigned int maxMatch = (Lm[nL - 1].t1 - Lm[0].t1) / oneStepFrames + 1;
    unsigned int startIndex = 0, last_index = 0, stepCnt = 0;
    int flag = 1;
    MATCH_RESULT *R = (MATCH_RESULT *)calloc( 10, sizeof(MATCH_RESULT) );
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

    while(flag)
    {
        memset(R, 0, 10 * sizeof(MATCH_RESULT));
        unsigned int bufLen = cnt_forward(Lm, nL, startIndex, oneSegFrames);
        if( bufLen < 1 ) break;
        if( startIndex + bufLen >= nL - 1 ) flag = 0;
        for( unsigned int i = 0; i < bufLen; i ++ ) {
            tmpLm[i] = Lm[startIndex + i];
        }
        double seg_len = (tmpLm[bufLen - 1].t1 - tmpLm[0].t1) * 0.032;
        double step_len = (Lm[startIndex].t1 - Lm[last_index].t1) * 0.032;
        double sseg = tmpLm[0].t1 * 0.032;
        double eseg = tmpLm[bufLen - 1].t1 * 0.032;

        int nR = match_query(tmpLm, bufLen, R, table);
        if( nR < 1 ) {
            flowR[stepCnt].ID = -1;
            flowR[stepCnt].score = 0;
        }
        else {
            int diff_best = nR > 1 ? R[0].best - R[1].best : R[0].best;
            int duration = (int)( seg_len + 0.5 );
            int score = genScore(R[0].best, diff_best, duration, wea_thresh, abs_thresh, inc_thresh, rel_thresh);
            flowR[stepCnt].ID = R[0].ID;
            flowR[stepCnt].score = score;
        }
        flowR[stepCnt].sseg = sseg;
        flowR[stepCnt].eseg = eseg;

        fprintf(fout,"[%9.3lf, %9.3lf]; seg_len = %6.3lf; step_len = %6.3lf; ID = %4d; score = %d\n", sseg, eseg, seg_len, step_len, flowR[stepCnt].ID, flowR[stepCnt].score);

        last_index = startIndex;
        stepCnt ++;
        startIndex = cnt_forward(Lm, nL, 0, stepCnt * oneStepFrames);
    }
    fclose(fout);

    /* Match from start */
    //int audioLen = (int)(oneSegFrames * 0.032 + 0.5);
    if( flowR[0].score > 4 ) {
        fprintf(stdout, "Match from start: From %.3lfs to %.3lfs, ID = %d, continues = 1, score = %d.\n", flowR[0].sseg, flowR[0].eseg, flowR[0].ID, flowR[0].score );
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
            fprintf(stdout, "Match from start: From %.3lfs to %.3lfs, ID = %d, continues = %d, score = %d.\n", flowR[start].sseg, flowR[n].eseg, ID, n - start + 1, score);
        else
            fprintf(stdout, "No match!\n");
    }

    /* Find all match */
    /*
    int *costTime = (int *)malloc(stepCnt * sizeof(int));
    for( int i = 0; i < stepCnt; i ++ ) {
        if( flowR[i].score > 4 ) costTime[i] = audioLen;
        else {
            int n = i;
            int score = flowR[i].score;
            while((n < stepCnt - 1) && ( score + flowR[n+1].score < 5 )) {
                n++;
                score += flowR[n+1].score;
            }
            costTime[i] = audioLen + n - i + 1;
        }
    }
    */

    free(Lm);
    free(tmpLm);
    free(R);
    free(flowR);
    del_hashtable(table);
    free(table);
    return 0;
}
