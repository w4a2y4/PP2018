#include <stdio.h>
#include <cuda.h>

#define b 4

__global__ void hello(int A[Size][Size], int B[Size][Size], int C[Size][Size])
{
	/*Your kernel*/
}

void metric_mul(int A[Size][Size], int B[Size][Size], int C[Size][Size])
{
	/*Your initail*/
    int *device_A, *device_B, *device_C;
    dim3 blocks(b, b);
    dim3 grids(Size/b,Size/b);
    int size = sizeof(int ) * Size * Size;
    cudaMalloc((void **)&device_A, size);
    cudaMalloc((void **)&device_B, size);
    cudaMalloc((void **)&device_C, size);

    cudaMemcpy(device_A, (int *)A, sizeof(int) * Size * Size, cudaMemcpyHostToDevice);
    cudaMemcpy(device_B, (int *)B, sizeof(int) * Size * Size, cudaMemcpyHostToDevice);
    hello <<< grids, blocks>>> ((int (*)[Size])device_A, (int (*)[Size])device_B, (int (*)[Size])device_C);

    cudaMemcpy((int *)C, device_C, size, cudaMemcpyDeviceToHost);
    cudaFree(device_A);
    cudaFree(device_B);
    cudaFree(device_C);
    return ;
}
