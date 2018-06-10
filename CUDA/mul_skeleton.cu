#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>
#include <time.h>
#ifndef Size
#define Size 1000

#endif
#define b 4

void metric_mul_gold(int A[Size][Size], int B[Size][Size], int C[Size][Size])
{

    int i,j,k;
	for(i=0; i<Size; i++)
		for(j=0; j<Size; j++)
			for(k=0; k<Size; k++)
				C[i][j] += A[i][k]*B[k][j];
    return;
}


void metric_mul(int A[Size][Size], int B[Size][Size], int C[Size][Size]);

int main(void)
{
    int i, j, k;
    int size = sizeof(int) * Size * Size;
    int *aptr, *bptr, *cptr;

    int *host_A, *host_B, *host_C;

    srand(time(NULL));

    host_A = (int *)malloc(size);
    host_B = (int *)malloc(size);
    host_C = (int *)malloc(size);

    aptr = host_A;
    bptr = host_B;
	cptr = host_C;
    for (i = 0; i < Size; i++)
        for (j = 0; j < Size; j++) {
            *aptr++ = rand() % 10;
            *bptr++ = rand() % 10;
			*cptr++ = 0;
        }
		
	int *gold_C;
    gold_C = (int *)malloc(size);
    metric_mul_gold((int (*)[Size])host_A, (int (*)[Size])host_B, (int (*)[Size])gold_C);
	
    cudaEvent_t start_time, stop_time;
    float exectime;
    cudaEventCreate(&start_time);
    cudaEventCreate(&stop_time);
    cudaEventRecord(start_time, 0);
    metric_mul((int (*)[Size])host_A, (int (*)[Size])host_B, (int (*)[Size])host_C);
    cudaEventRecord(stop_time, 0);
    cudaEventSynchronize(stop_time);
    cudaEventElapsedTime(&exectime, start_time, stop_time);
    
    printf("real %f ms\n ", exectime);
    cudaEventDestroy(start_time);
    cudaEventDestroy(stop_time);

    //check result
    if (!memcmp(host_C, gold_C, size))
    	printf("AC!\n");
    else
    	printf("Failed!\n");
    	
    /*k = 0;
    for (i = 0; i < Size; i++)
        for (j = 0; j < Size; j++)
            printf("host_C[%d][%d] = %d\n", i, j, host_C[k++]);*/

    free(host_A);
    free(host_B);
    free(host_C);
    free(gold_C);
    return 0;
}
