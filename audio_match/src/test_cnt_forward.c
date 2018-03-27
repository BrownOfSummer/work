#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include "match_utils.h"
void help()
{
    fprintf(stderr, "Demonstrate the use of cnt_forward.\n");
    fprintf(stderr, "cnt_forward is designed to find the end index that adna[endIndex] - adna[startIndex] >= time_step.\n");
    fprintf(stderr, "\t./bin file.adna");
}
int main(int argc, char *argv[])
{
    if( argc != 2 ) {
        help();
        return -1;
    }
    FILE *f = fopen(argv[1], "rb");
    if (f == NULL) {
        fprintf(stdout, "Can not open file\n");
        return -1;
    }
    fseek(f, 0, SEEK_END);
    unsigned int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    int num = size / ADNA_SIZE;
    LANDMARK *LM = (LANDMARK *)malloc(num * sizeof(LANDMARK));
    unsigned int nL = AfpReadFeatures((unsigned char *)LM, num, f);
    unsigned int start_Index = 0, flag = 1;
    //unsigned int time_len = LM[nL - 1].t1 - LM[0].t1;
    unsigned int time_len = LM[nL - 1].t1;
    fprintf(stdout, "lmcnt = %d; lm_time_len = %.3lfs.\n", nL, time_len * 0.032);
    while(flag)
    {
        unsigned int steps_forward = cnt_forward(LM, nL, start_Index, 32);
        if( steps_forward < 1 )
            break;
        else{
            double t = LM[start_Index].t1 * 0.032;
            double t_end = LM[start_Index + steps_forward].t1 * 0.032;
            fprintf(stdout, "Index = %6d %6.3lfs -- %6.3lfs\t diff=%6.3fs\n", start_Index, t, t_end, t_end - t);
            start_Index += steps_forward;
        }
    }

    free(LM);
    return 0;
}

