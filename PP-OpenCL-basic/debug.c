/* header */
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 
 
#include <stdio.h>
#include <assert.h>
#include <CL/cl.h>
 
#define MAXGPU 10
#define MAXK 1024
 
/* main */
int main(int argc, char *argv[])
{
 
    cl_int status;
    cl_platform_id platform_id;
    cl_uint platform_id_got;
    status = clGetPlatformIDs(1, &platform_id, &platform_id_got);
    assert(status == CL_SUCCESS && platform_id_got == 1);
    // printf("%d platform found\n", platform_id_got);
 
    cl_device_id GPU;
    status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &GPU, NULL);
    assert(status == CL_SUCCESS);
    // printf("There are %d GPU devices\n", GPU_id_got); 
 
    /* getcontext */
    cl_context context = 
        clCreateContext(NULL, 1, &GPU, NULL, NULL, &status);
    assert(status == CL_SUCCESS);
 
    /* kernelsource */
    char fname[30];
    scanf("%s", fname);
    FILE *kernelfp = fopen(fname, "r");
    assert(kernelfp != NULL);
 
    char kernelBuffer[MAXK];
    const char *constKernelSource = kernelBuffer;
    size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);
    // printf("The size of kernel source is %zu\n", kernelLength);
    cl_program program =
      clCreateProgramWithSource(context, 1, &constKernelSource, &kernelLength, &status);
    assert(status == CL_SUCCESS);
 
    /* buildprogram */
    status = clBuildProgram(program, 1, &GPU, NULL, NULL, NULL);
 
    char program_log[10240];
    status = clGetProgramBuildInfo (program, GPU, 
        CL_PROGRAM_BUILD_LOG, 10240, program_log, NULL);
 
    assert(status == CL_SUCCESS);
 
    printf("%s", program_log);
 
    clReleaseContext(context);    /* context etcmake */
    clReleaseProgram(program);
    return 0;
}
/* end */