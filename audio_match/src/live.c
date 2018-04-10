/*************************************************************************
    > File Name: test_recycle2.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-31 10:19:03
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "match_utils.h"
void help()
{
    fprintf(stderr,"Simulate the live match !\n");
    fprintf(stderr,"Paras:\n");
    fprintf(stderr,"\tDictionary: contains the channel_name, ID, and adnaFileNum for the channel.\n");
    fprintf(stderr,"\tpathList:   direct to the adnaFile list.\n");
    fprintf(stderr,"\tsample.adna:the channel to be determined.\n");
    fprintf(stderr,"Usage:\n");
    fprintf(stderr,"\t./bin Dictionary.list pathList.list sample.adna\n");
}
int main(int argc, char *argv[])
{
    if( argc != 4 ) {
        fprintf(stderr, "ERROR, input error !\n");
        help();
        exit(-1);
    }

    /*
     * Prepare for adna path.
     */

    CHANNEL_TABLE *channelTable = (CHANNEL_TABLE *)malloc(sizeof(CHANNEL_TABLE));
    build_channelTable(channelTable, argv[1], argv[2]);
    int maxAdnaFileNum = -1;
    int channelCnt = 0;
    for(int i = 0; i < CHANNEL_TABLE_SIZE; i ++) {
        if( channelTable->items[i] != NULL ) {
            channelCnt ++;
            int adnaFileNum = channelTable->items[i]->adnaFileNum;
            if( adnaFileNum > maxAdnaFileNum ) 
                maxAdnaFileNum = adnaFileNum;
        }
    }
    fprintf(stdout, "Build CHANNEL_TABLE done. %d channels, maxAdnaFileNum = %d\n", channelCnt, maxAdnaFileNum);

    /*
     * Load sample adna
     */
    const char *sample = argv[3];
    int wea_thresh = WEA_THRESH;
    int abs_thresh = ABS_THRESH;
    int inc_thresh = INC_THRESH;
    int rel_thresh = REL_THRESH;
    int end_thresh = END_THRESH;
    /*
     * Build HASHTABLE.
     */
    const int STABLE = 0;
    const int RECYCLED = 1;
    Context ctx = init_context(RECYCLED, 1); // already init a table in recycle_table_list[0]
    //add_table(&ctx); // add [1]
    //recycle_table(&ctx);

    fprintf(stdout,"Push Adnas to HASHTABLE ...\n");
    char name[200];
    const stringList **adnaPathes = ( const stringList **)malloc( CHANNEL_TABLE_SIZE * sizeof(stringList*) );
    for(int j = 0; j < maxAdnaFileNum; j ++) {
        memset(adnaPathes, 0, CHANNEL_TABLE_SIZE * sizeof(stringList *));
        int cnt = 0;
        traversalOnce(channelTable, j, adnaPathes);
        for(int id = 0; id < CHANNEL_TABLE_SIZE; id ++) {
            if(adnaPathes[id] != NULL) 
            {
                //fprintf(stdout,"\t%s\n",adnaPathes[k]->element);
                strcpy(name, adnaPathes[id]->element);
                //memset(name + strlen(name)-1, 0, 1);
                cnt ++;
                INGEST_INFO info = add_adna(name, id, 0, 0, &ctx);
            }
        }
        fprintf(stdout,"%dth traversal, %d adnaFilePath.\n\n", j, cnt);

        /*
        MATCH_INFO res = match(sample, abs_thresh, inc_thresh, rel_thresh, wea_thresh, end_thresh, &ctx);
        if( res.dna_id >= 0 ) {
            fprintf(stdout,"ID = %d, skew = %lf, [%s] --- [%s]\n", res.dna_id, res.skew, channelTable->items[res.dna_id]->channelName, sample);
        }
        else fprintf(stdout,"No Match !\n");
        */
        Rcd acc_his;
        MATCH_INFO res2 = match2(sample, abs_thresh, inc_thresh, rel_thresh, wea_thresh, end_thresh, &ctx, &acc_his);
        if( acc_his.id >= 0 )
            fprintf(stdout,"ID = %d, score = %d, [%s] --- [%s]\n", acc_his.id, acc_his.lv, channelTable->items[acc_his.id]->channelName, sample);
        else
            fprintf(stdout,"ID = %d, score = %d, [NULL] --- [%s]\n", acc_his.id, acc_his.lv, sample);
        
        //break;
    }
    del_channelTable(channelTable);
    free(channelTable);
    free(adnaPathes);
    destroy_table_list(&ctx);
    return 0;
}
