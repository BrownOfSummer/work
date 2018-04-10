/*************************************************************************
    > File Name: match_utils.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-14 13:18:28
 ************************************************************************/

#include "match_utils.h"

/*
 * Moving int the LANDMARKs, Find the end index, return the forward cnt according the startIndex.
 * Paras:
 *      LM: Load from adna, the true feature get from algorithm.
 *      nL: number of LANDMARKs in LM.
 *      startIndex: LM[startIndex].t1 is the start frame.
 *      oneStepFrames: how many frames to step forward.
 *Return:
 *      cnt: startIndex + cnt <= nL - 1; and LM[startIndex + cnt].t1 - LM[startIndex].t1 >= oneStepFrames
*/
unsigned cnt_forward(const LANDMARK *LM, const unsigned int nL,  unsigned int startIndex, unsigned int oneStepFrames){
    unsigned int cnt = 0 , i = 0;
    int flag = 1;
    while(flag){
        if( LM[nL - 1].t1 - LM[startIndex].t1 <= oneStepFrames ){
            cnt = nL - 1 - startIndex;
            flag = 0;
            break;
        }
        else
        {
            for( i = startIndex + 1; i < nL; i ++ ){
                if( LM[i].t1 - LM[startIndex].t1 < oneStepFrames )
                    continue;
                else{
                    cnt = i - startIndex;
                    startIndex = i;
                    flag = 0;
                    break;
                }
            }
        }
    }
    return cnt;
}

int match_or_not( const LANDMARK *LM, const unsigned int nL,  MATCH_RESULT *R, const HASHTABLE *table,
        int wea_thresh, int abs_thresh, int inc_thresh, int rel_thresh)
{
    Rcd acc_his = {-1, 0};
    uint32_t nR = match_query(LM, nL, R, table);

    int duration = (LM[nL - 1].t1 -LM[0].t1) * 32 / 1000;
    //int duration = LM[nL - 1].t1 * 32 / 1000;
    int diff_best = nR > 1 ? R[0].best - R[1].best : R[0].best;
    if (diff_best >= wea_thresh) {
        if (acc_his.id == R[0].ID)
            acc_his.lv ++;
        else {
            acc_his.id = R[0].ID;
            acc_his.lv = 1;
        }
        if (diff_best >= rel_thresh && R[0].best >= inc_thresh + abs_thresh) {
            acc_his.lv += 2;
            if (R[0].best >= duration * inc_thresh + abs_thresh) {
                //acc_his.lv ++;
                acc_his.lv += 2;
                if (R[0].best >= (duration + 1) * inc_thresh + abs_thresh)
                    acc_his.lv ++;
            }
        }
    }
    else {
        acc_his.id = -1;
        acc_his.lv = 0;
    }
    if (acc_his.lv > 5){
        //fprintf(stdout, "matching best %d skew %.3lf.\n", R[0].best, R[0].skew * 0.032);
        return 1;
    }
    else
        return 0;
}
/*
 * Generate a score with thresholds.
 * Paras:
 *      best: MATCH_RESULT R[0].best;
 *      diff_best: for difference, nR > 1 ? R[0].best - R[1].best : R[0].best;
 *      four thresholds to calc score.
 *  Return:
 *      score in [0, 5]
 * */
int genScore(int best, int diff_best, int duration, int wea_thresh, int abs_thresh, int inc_thresh, int rel_thresh) {
    int score = 0;
    if (diff_best >= wea_thresh) {
        score ++;
        if (diff_best >= rel_thresh && best >= inc_thresh + abs_thresh) {
            score += 2;
            if (best >= duration * inc_thresh + abs_thresh) {
                score ++;
                if (best >= (duration + 1) * inc_thresh + abs_thresh)
                    score ++;
            }
        }
    }
    return score;
}

int cmp_seg_match(const void *a1, const void *b1) {
    MATCH_RESULT *a = (MATCH_RESULT *)a1, *b = (MATCH_RESULT *)b1;
    if(a->best < b->best) return 1;
    else if(a->best > b->best) return -1;
    else if(a->skew < b->skew) return -1;
    else if(a->skew > b->skew) return 1;
    else return 0;
}

/*
 * Print how many elements in each hash table slots.
 * Paras:
 *      table: HASHTABLE
 *      out: FILE point to save number count of slot.
 * Return:
 *      sum of all slot counts.
 * */
uint64_t print_table_cnt(const HASHTABLE *table, char *out) {
    if (table == NULL || table->items == NULL) {
        fprintf(stderr, "Null pointer assignment.\n");
        exit(-1);
    }
    uint32_t i;
    uint64_t table_cnt = 0;
    int flag = 0;    
    FILE *fout = NULL;
    if(out != NULL) flag = 1;
    if(flag) fout = fopen(out,"w");
    for(i = 0; i < HASHTABLE_SIZE; i ++) {
        uint64_t cnt = 0;
        ListNode *p = table->items[i];
        for(;p != NULL; p=p->next) {
            //free(p);
            cnt ++;
        }
        if(flag) fprintf(fout,"%llu\n", cnt);
        table_cnt += cnt;
    }
    if(flag) fclose(fout);
    return table_cnt;
}
/*
uint64_t print_table_cnt(HASHTABLE *table, char *out) {
    if (table == NULL || table->items == NULL) {
        fprintf(stderr, "Null pointer assignment.\n");
        exit(-1);
    }
    ListNode *p;
    uint32_t i;
    uint64_t table_cnt = 0;
    int flag = 0;    
    FILE *fout = NULL;
    if(out != NULL) flag = 1;
    if(flag) fout = fopen(out,"w");
    for(i = 0; i < HASHTABLE_SIZE; i ++) {
        uint64_t cnt = 0;
        for(;table->items[i] != NULL;) {
            p = table->items[i];
            table->items[i] = p->next;
            //free(p);
            cnt ++;
        }
        if(flag) fprintf(fout,"%llu\n", cnt);
        table_cnt += cnt;
    }
    if(flag) fclose(fout);
    return table_cnt;
}
*/


void init_channelTable(CHANNEL_TABLE *channelTable)
{
    if (channelTable == NULL) {
        fprintf(stderr, "Null pointer assignment.\n");
        exit(-1);
    }
    channelTable->items = (CHANNEL_INFO **)malloc(CHANNEL_TABLE_SIZE * sizeof(CHANNEL_INFO *));
    for(int i = 0; i < CHANNEL_TABLE_SIZE; i ++) channelTable->items[i] = NULL;
}

void add_channel_dict(CHANNEL_TABLE *channelTable, const char * channelInfoList)
{
    if (channelTable == NULL) {
        fprintf(stderr, "Null pointer assignment.\n");
        exit(-1);
    }
    char tmp[200], name[200];
    int cnt1=0, cnt2=0, ID, adnaFileNum, pre = -1673;
    FILE *fp = fopen(channelInfoList, "r");
    if( !fp ) {
        fprintf(stdout,"ERROR, Open %s Faild\n",channelInfoList);
        exit(-1);
    }
    while( fgets(tmp, 200, fp) != NULL ) {
        sscanf(tmp, "%s %d %d", name, &ID, &adnaFileNum);
        cnt1 ++;
        if(ID != pre) {
            cnt2 ++;
            pre = ID;
        }
    }
    if(cnt1 != cnt2) {
        fprintf(stderr,"Channel Dict have duplicate !\n");
        exit(-1);
    }
    //else *channelNum = cnt1;
    rewind(fp);
    while( fgets(tmp, 200, fp) != NULL ) {
        sscanf(tmp, "%s %d %d", name, &ID, &adnaFileNum);
        CHANNEL_INFO *p = (CHANNEL_INFO *)malloc( sizeof(CHANNEL_INFO) );
        p->ID = ID;
        p->adnaFileNum = adnaFileNum;
        strcpy(p->channelName, name);
        p->adnaFilePath = NULL;
        channelTable->items[ID] = p;
    }
}

void add_adna_path(CHANNEL_TABLE *channelTable, const char *pathList)
{
    char tmp[200], list[200], path[200];
    int ID;
    FILE *fp = fopen(pathList, "r");
    if( !fp ) {
        fprintf(stdout,"ERROR, Open %s Faild\n", pathList);
        exit(-1);
    }
    while( fgets(tmp, 200, fp) != NULL ) {
        memset(tmp + strlen(tmp)-1, 0, 1);
        FILE *fpath = fopen(tmp, "r");
        if( !fpath ) {
            fprintf(stderr, "ERROR, Open %s Faild\n", tmp);
        }
        while( fgets(list, 200, fpath) ) {
            sscanf(list, "%s %d", path, &ID);
            
            // add to table
            stringList *s = (stringList *)malloc(sizeof(stringList));
            strcpy(s->element, path);
            s->next = channelTable->items[ID]->adnaFilePath;
            channelTable->items[ID]->adnaFilePath = s;
        }
        fclose(fpath);
    }
    fclose(fp);
}
void del_channelTable(CHANNEL_TABLE *channelTable)
{
    if( channelTable == NULL || channelTable->items == NULL ) {
        fprintf(stderr, "Del ERROR, Null pointer assignment.\n");
        exit(-1);
    }
    stringList *s;
    CHANNEL_INFO *p;
    for(int i = 0; i < CHANNEL_TABLE_SIZE; i ++) {
        p = channelTable->items[i];
        if(p != NULL) {
            for(; channelTable->items[i]->adnaFilePath != NULL; ) {
                s = channelTable->items[i]->adnaFilePath;
                channelTable->items[i]->adnaFilePath = s->next;;
                free(s);
            }
            free(p);
            channelTable->items[i] = NULL;
        }
    }
    free(channelTable->items);
    channelTable->items = NULL;
}
void build_channelTable(CHANNEL_TABLE *channelTable, const char* channelInfoList, const char *pathList)
{
    init_channelTable(channelTable);
    add_channel_dict(channelTable, channelInfoList);
    add_adna_path(channelTable, pathList);
}

void print_channelTable(CHANNEL_TABLE *channelTable)
{
    if( channelTable == NULL || channelTable->items == NULL ) {
        fprintf(stderr, "Print ERROR, Null pointer assignment.\n");
        exit(-1);
    }
    for(int i = 0; i < CHANNEL_TABLE_SIZE; i ++)
    {
        CHANNEL_INFO *p = channelTable->items[i];
        if(p != NULL) {
            fprintf(stdout,"%s: ID = %d; adnaFileNum = %d\n", p->channelName, p->ID, p->adnaFileNum);
            const stringList *s = channelTable->items[i]->adnaFilePath;
            if( s != NULL ) {
                for(;s != NULL; s = s->next) { fprintf(stdout,"\t%s\n",s->element);}
            }
        }
    }
}

void traversalOncePrint(CHANNEL_TABLE *channelTable, int startPoint)
{
    const stringList *s;
    int pos = -1;
    for(int i = 0; i < CHANNEL_TABLE_SIZE; i ++) {
        if( channelTable->items[i] != NULL ) {
            if( channelTable->items[i]->adnaFileNum <= startPoint ) s = NULL;
            else
            {
                pos = 0;
                s = channelTable->items[i]->adnaFilePath;
                for(; (s != NULL) && (pos < startPoint); pos ++ ) {
                    s = s->next;
                }
                if( pos !=startPoint ) s = NULL;
            }
            if( s != NULL ) fprintf(stdout, "\t%s\n",s->element);
            else fprintf(stdout,"\tNULL\n");
        }
    }
}
void traversalOnce(CHANNEL_TABLE *channelTable, int startPoint, const stringList ** paths)
{
    //const stringList **paths = ( const stringList **)malloc( CHANNEL_TABLE_SIZE * sizeof(stringList*) );
    const stringList *s;
    int pos = -1;
    for(int i = 0; i < CHANNEL_TABLE_SIZE; i ++) 
    {
        if( channelTable->items[i] != NULL )
        {
            if( channelTable->items[i]->adnaFileNum <= startPoint ) s = NULL;
            else {
                pos = 0;
                s = channelTable->items[i]->adnaFilePath;
                for(; (s != NULL) && (pos < startPoint); pos ++ ) s = s->next;
                if( pos != startPoint ) s = NULL;
            }
        }
        else s = NULL;
        paths[i] = s;
    }
}
