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

#define CHANNEL_TABLE_SIZE 200

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

int main(int argc, char *argv[])
{
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
    fprintf(stdout, "%d channels, maxAdnaFileNum = %d\n", channelCnt, maxAdnaFileNum);
    //traversalOncePrint(channelTable, maxAdnaFileNum - 1);
    const stringList **paths = ( const stringList **)malloc( CHANNEL_TABLE_SIZE * sizeof(stringList*) );
    for(int j = 0; j < maxAdnaFileNum; j ++) {
        int cnt = 0;
        traversalOnce(channelTable, j, paths);
        for(int k = 0; k < CHANNEL_TABLE_SIZE; k ++) {
            if(paths[k] != NULL) 
            {
                fprintf(stdout,"\t%s\n",paths[k]->element);
                cnt ++;
            }
        }
        fprintf(stdout,"%dth traversal, %d adnaFilePath.\n\n", j, cnt);
    }
    //print_channelTable(channelTable);
    del_channelTable(channelTable);
    free(channelTable);
    free(paths);
    return 0;
}
