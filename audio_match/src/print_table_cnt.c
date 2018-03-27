/*************************************************************************
    > File Name: print_table_cnt.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-16 15:24:07
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "match_utils.h"

#define MAX_NUM_ADNA 1000
void help()
{
    fprintf(stderr,"Demonstrate the use of print_table_cnt.\n");
    fprintf(stderr,"Load a list of adna and push into table, then print number of elements in each slot.\n");
    fprintf(stderr,"\t./bin adna.list [outputfile.txt]\n");
}
int main(int argc, char *argv[])
{
    if ( (argc != 2) && ( argc != 3 ) ) {
        fprintf(stderr,"Input ERROR.\n");
        help();
        return -1;
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

    fprintf(stdout,"Push Adna to HASHTABLE ...\n");
    
    //  1 Build HASHTABLE.
    HASHTABLE *table = (HASHTABLE *)malloc( sizeof(HASHTABLE) );
    build_hashtable(table);
    unsigned int max_num = 10000;
    LANDMARK *LM = (LANDMARK *)malloc(max_num * sizeof(LANDMARK));
    
    uint64_t total_lms = 0;
    //  2 Read adna and add to table.
    uint64_t ID = 0;
    while(1)
{
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
    
        int resize_num = 0;
        while( num > max_num ) {
            if( resize_num > 2 ) {
                fprintf(stderr,"When deal with %s\n", master[i]);
                fprintf(stderr, "Continuely realloc %d times, maybe wrong, please enlarge initial max_num.\n", resize_num);
                free(LM);
                del_hashtable(table);
                free(table);
                fclose(flm);
                exit(EXIT_FAILURE);
            }
            fprintf(stdout, "WARNING, %d > %d, need to realloc memory !\n", num, max_num);
            max_num *= 2;
            LM = reallocf(LM, max_num * sizeof(LANDMARK));
            resize_num ++;
        }

        unsigned int nL = AfpReadFeatures( (unsigned char *)LM, num, flm );
        if( nL != num ) {
            fprintf(stderr, "Load %s faild !",master[i]);
        }
        fclose(flm);
        add_track(LM, nL, i, table);
        //add_track(LM, nL, ID++, table);
        
        //uint64_t table_cnt = print_table_cnt(table, NULL);
        //fprintf(stdout,"\tPushed %u LANDMARK, ID: %llu, LANDMARK in table: %llu.\n", nL, ID, table_cnt);
        total_lms += nL;
        flm = NULL;
    }
    fprintf(stdout,"Push To HASHTABLE Done.\n");
    
    uint64_t table_cnt = 0;
    if( argc == 3 )
        table_cnt = print_table_cnt(table, argv[2]);
    else
        table_cnt = print_table_cnt(table, NULL);
    //fprintf(stdout,"Load %d Adnas, Pushed %llu LANDMARK, %llu LANDMARK in table.\n",cnt1, total_lms, table_cnt);
    fprintf(stdout,"Load %d Adnas, MAX_ID: %llu, Pushed %llu LANDMARK, %llu LANDMARK in table.\n",cnt1, ID, total_lms, table_cnt);

}
    // load output
    /*
    FILE *fin = fopen("table_cnt.txt","r");
    uint64_t num = 0, total_nums = 0;
    while( fgets(str, 200, fin) != NULL ) {
        sscanf(str, "%llu\n", &num);
        total_nums += num;
        //fprintf(stdout,"%llu\n",num);
    }
    fprintf(stdout,"Totally %llu LMs in table !\n", total_nums);
    fclose(fin);
    */
    free(LM);
    del_hashtable(table);
    free(table);
    return 0;
}
