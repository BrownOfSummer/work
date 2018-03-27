/*************************************************************************
    > File Name: mul_on_mul.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-12 14:05:13
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "utils.h"
#include "genafp.h"
#include "afpWrap.h"

void help() 
{
    fprintf(stdout, "\nDeal with the adna push and match process.\n");
    fprintf(stdout, "Input master_adna.list and sample_adna.list.\n");
    fprintf(stdout, "Push each adna in master, and query each in sample.\n");
    fprintf(stdout, "\tCall ./bin master_adna.list sample_adna.list\n");
}

int main(int argc, char *argv[])
{
    if ( argc != 3 ) {
        help();
        return -1;
    }
    FILE *fm = fopen(argv[1], "r");
    FILE *fs = fopen(argv[2], "r");
    if (fm == NULL) {
        fprintf(stderr, "Open master.list failed !");
        return -1;
    }
    if (fs == NULL) {
        fprintf(stderr, "Open sample.list failed !");
        return -1;
    }
    char str[200];
    char master[1000][200];
    char sample[1000][200];
    int cnt1 = 0, cnt2 = 0;
    while (fgets(str, 200, fm) != NULL && cnt1 < 1000) {
        memset(str + strlen(str)-1, 0, 1);
        //master[cnt1] = str;
        strcpy(master[cnt1], str);
        cnt1 ++;
    }
    while (fgets(str, 200, fs) != NULL && cnt2 < 1000) {
        memset(str + strlen(str) -1, 0, 1);
        //sample[cnt2] = str;
        strcpy(sample[cnt2], str);
        cnt2 ++;
    }
    fclose(fm);
    fclose(fs);
    fprintf(stdout,"Find %d master adna, and %d sample adna !\n", cnt1, cnt2);

    fprintf(stdout,"Push master to HASHTABLE ...\n");
    
    //  1 Build HASHTABLE.
    HASHTABLE *table = (HASHTABLE *)malloc( sizeof(HASHTABLE) );
    build_hashtable(table);
    unsigned int max_num = 5000;
    LANDMARK *LM = (LANDMARK *)malloc(max_num * sizeof(LANDMARK));
    
    //  2 Read adna and add to table.
    for(int i = 0; i < cnt1; i ++) {
        //fprintf(stdout,"%s\n", master[i]);
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
            //realloc(LM, max_num * sizeof(LANDMARK));
            resize_num ++;
        }
        unsigned int nL = AfpReadFeatures( (unsigned char *)LM, num, flm );
        if( nL != num ) {
            fprintf(stderr, "Load %s faild !",master[i]);
        }

        add_track(LM, nL, i, table);
        fclose(flm);
    }
    fprintf(stdout,"Push master to HASHTABLE Done.\n");

    //  3 Do match process.
    MATCH_RESULT *R = (MATCH_RESULT *)calloc(10, sizeof(MATCH_RESULT));
    for(int i=0; i < cnt2; i ++) {
        FILE *flm = fopen(sample[i], "rb");
        if(flm == NULL) {
            fprintf(stderr, "ERROR, Open %s error !\n", sample[i]);
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
                exit (EXIT_FAILURE);
            }
            fprintf(stdout, "WARNING, %d > %d, need to realloc memory !\n", num, max_num);
            max_num *= 2;
            LM = reallocf(LM, max_num * sizeof(LANDMARK));
            resize_num ++;
        }
       
        unsigned nL = AfpReadFeatures( (unsigned char *)LM, num, flm );
        if( nL != num ) {
            fprintf(stderr, "ERROR, Load %s faild !",sample[i]);
        }
        fclose(flm);
        int nR = match_query(LM, nL, R, table);
        if( nR > 0 ) {
            double skew2s = R[0].skew * 0.032;
            fprintf(stdout,"%s -- %s, score = %d, skew = %lld, skew=%.3lfs.\n",sample[i], master[ R[0].ID ], R[0].best, R[0].skew, skew2s);
            for(int i = 1; i < nR; i ++)
                fprintf(stdout,"\t\t\t -- %s, score = %d, skew = %lld, skew=%.3lfs.\n", master[ R[i].ID ], R[i].best, R[i].skew, R[i].skew * 0.032);
        }
        memset(R, 0, 10*sizeof(MATCH_RESULT));
    }
    free(R);
    free(LM);
    del_hashtable(table);
    free(table);
    return 0;
}
