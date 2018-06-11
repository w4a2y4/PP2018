#include <stdio.h>
#include <assert.h>
#include <omp.h>
#include <inttypes.h>
#include <CL/cl.h>
#include <assert.h>
#include "utils.h"
#define MAXK 2048
#define MAXN 67108864
#define CHUNK 1024
#define LOCALSIZE 256
int main(){
    int N;
    uint32_t key1, key2;
    /* platform */
    cl_int status;
    cl_platform_id platform_id;
    status = clGetPlatformIDs(1, &platform_id, NULL);
    assert(status == CL_SUCCESS);
 
    /* device */
    cl_device_id GPU;
    cl_uint GPU_num;
    status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &GPU, &GPU_num);
    assert(status == CL_SUCCESS);
 
    /* context */
    cl_context context = clCreateContext(NULL, 1, &GPU, NULL, NULL, &status);
    assert(status == CL_SUCCESS);
 
    /* command queue */
    cl_command_queue cq = clCreateCommandQueue(context, GPU, 0, &status);
    assert(status == CL_SUCCESS);
 
    /* kernelsource */
    FILE *fp = fopen("vecdot.cl", "r");
    char buf[MAXK];
    const char *const_buffer = buf;
    size_t len = fread(buf, 1, MAXK, fp);
    cl_program program = clCreateProgramWithSource(context, 1, &const_buffer, &len, &status);
 
    /* program */
    status = clBuildProgram(program, 1, &GPU, NULL, NULL, NULL);
 
    /* kernel */
    cl_kernel kernel = clCreateKernel(program, "VectorDot", &status);
 
    /* vector & buffer */
    int size = ((MAXN+CHUNK-1)/CHUNK+LOCALSIZE-1)/LOCALSIZE;
    cl_uint *C = (cl_uint*)malloc(size * sizeof(cl_uint));
    cl_mem bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size * sizeof(cl_uint), C, &status);
 
    /* read N, key1, key2 */
    while (scanf("%d %" PRIu32 " %" PRIu32, &N, &key1, &key2) == 3){
        /* set argument */
        status = clSetKernelArg(kernel, 0, sizeof(cl_uint), (void*)&key1);
        status = clSetKernelArg(kernel, 1, sizeof(cl_uint), (void*)&key2);
        status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&bufC);
        status = clSetKernelArg(kernel, 3, sizeof(int), (void*)&N);
 
        /* setshape*/
        int n_global = (N+CHUNK-1) / CHUNK;
        int n_groups = (n_global+LOCALSIZE-1) / LOCALSIZE;
        size_t global_threads[] = {(size_t)n_groups*LOCALSIZE};
        size_t local_threads[] = {(size_t)LOCALSIZE};
        status = clEnqueueNDRangeKernel(cq, kernel, 1, NULL, global_threads, local_threads, 0, NULL, NULL);
 
        /* getcvector*/
        status = clEnqueueReadBuffer(cq, bufC, CL_TRUE, 0, n_groups * sizeof(cl_uint), C, 0, NULL, NULL);
 
        uint32_t sum = 0;
        for (int i = 0; i < n_groups; i++)
            sum += C[i];
        printf("%" PRIu32 "\n", sum);
    }
 
    /* free*/
    free(C);
    clReleaseContext(context);
    clReleaseCommandQueue(cq);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseMemObject(bufC);
 
    return 0;
}