/*************************************************************************
    > File Name: print_adna_info.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-16 14:14:53
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "match_utils.h"

int main(int argc, char *argv[])
{
    FILE *fl = fopen(argv[1], "rb");
    if( !fl ) {
        fprintf(stderr, "Cannot open %s\n", argv[1]);
        return -1;
    }

    fseek(fl, 0, SEEK_END);
    unsigned int size = ftell(fl);
    fseek(fl, 0, SEEK_SET);
    unsigned int numl = size / ADNA_SIZE;

    LANDMARK *Lm1 = (LANDMARK *)malloc( numl * sizeof( LANDMARK ) );

    unsigned int nL1 = AfpReadFeatures((unsigned char *)Lm1, numl, fl);
    fclose(fl);
    double ss = Lm1[0].t1 * 0.032;
    double ee = Lm1[nL1 -1].t1 * 0.032;
    fprintf(stdout, "Starts: %.3lfs, Ends: %.3lfs, time_len: %.3lf, %u landmarks.\n", ss, ee, ee - ss, nL1);
    free(Lm1);
    return 0;
}
