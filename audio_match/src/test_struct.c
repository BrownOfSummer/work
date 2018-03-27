/*************************************************************************
    > File Name: test_struct.c
    > Author: li_pengju
    > Mail: li_pengju@vobile.cn
    > Copyright (c) Vobile Inc. All Rights Reserved
    > Created Time: 2018-03-21 13:37:13
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "afpWrap.h"
#include "utils.h"
#include "genafp.h"

int main()
{
    fprintf(stdout,"int   = %lu bytes.\n", sizeof(int));
    fprintf(stdout,"int*  = %lu bytes.\n", sizeof(int*));
    fprintf(stdout,"float = %lu bytes.\n", sizeof(float));
    fprintf(stdout,"float* = %lu bytes.\n", sizeof(float*));
    fprintf(stdout,"(ListNode) = %lu bytes.\n", sizeof(ListNode));
    fprintf(stdout,"(ListNode*) = %lu bytes.\n", sizeof(ListNode*));
    fprintf(stdout,"(ListNode**) = %lu bytes.\n", sizeof(ListNode**));
    return 0;
}
