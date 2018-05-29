/* header */
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 
 
#include <stdio.h>
#include <assert.h>
#include <CL/cl.h>
#include <stdint.h>
#include <omp.h>
#include <inttypes.h>
 
#define MAXK 4096
#define CHUNK 1024
/* main */
int main(int argc, char *argv[])
{
 
    cl_int status;
    cl_platform_id platform_id;
    cl_uint platform_id_got;
    status = clGetPlatformIDs(1, &platform_id, &platform_id_got);
    assert(status == CL_SUCCESS && platform_id_got == 1);
 
    cl_device_id GPU;
    status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &GPU, NULL);
    assert(status == CL_SUCCESS);
 
    /* getcontext */
    cl_context context = clCreateContext(NULL, 1, &GPU, NULL, NULL, &status);
    assert(status == CL_SUCCESS);
 
    /* commandqueue */
    cl_command_queue commandQueue = clCreateCommandQueue(context, GPU, 0, &status);
    assert(status == CL_SUCCESS);
 
    /* kernelsource */
    FILE *kernelfp = fopen("vecdot.cl", "r");
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
    // assert(status == CL_SUCCESS);
    char program_log[MAXK];
    status = clGetProgramBuildInfo (program, GPU, 
        CL_PROGRAM_BUILD_LOG, MAXK, program_log, NULL);
    assert(status == CL_SUCCESS);
    // printf("%s", program_log);
 
    /* createkernel */
    cl_kernel kernel = clCreateKernel(program, "add", &status);
    assert(status == CL_SUCCESS);
 
    unsigned int N, key1, key2;
    /* createbuffer */
    cl_uint* C = (cl_uint*)malloc(67108864 * sizeof(cl_uint));
    assert(C != NULL);
    cl_mem bufferC = 
      clCreateBuffer(context, 
           CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
           67108864 * sizeof(cl_uint), C, &status);
    assert(status == CL_SUCCESS);

    while( scanf("%u%u%u", &N, &key1, &key2) == 3) {
 
        int gl = N - N%CHUNK + CHUNK;

        /* setarg */
        status = clSetKernelArg(kernel, 0, sizeof(cl_uint), (void *)&key1);
        assert(status == CL_SUCCESS);
        status = clSetKernelArg(kernel, 1, sizeof(cl_uint), (void *)&key2);
        assert(status == CL_SUCCESS);
        status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&bufferC);
        assert(status == CL_SUCCESS);
        status = clSetKernelArg(kernel, 3, sizeof(cl_uint), (void *)&N);
        assert(status == CL_SUCCESS);
        // printf("Set kernel arguments completes\n");
 
        /* setshape */
        size_t globalThreads[] = {(size_t)( gl )};
        size_t localThreads[] = {CHUNK};
 
        status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, 
                   globalThreads, localThreads, 0, NULL, NULL);
        assert(status == CL_SUCCESS);
        // printf("Specify the shape of the domain completes.\n");
 
        /* getcvector */
        status = clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 
                  0, gl * sizeof(cl_uint), C, 
                  0, NULL, NULL);
        assert(status == CL_SUCCESS);
        // printf("Kernel execution completes.\n");
 
        // print
        uint32_t sum = 0;
        // #pragma omp parallel for reduction(+: sum)
        for (int i = 0; i < gl/CHUNK; i++) {
            sum += C[i];
        }
 
        printf("%" PRIu32 "\n", sum);
    }
 
    clReleaseContext(context);    /* context etcmake */
    clReleaseCommandQueue(commandQueue);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseMemObject(bufferC);
    free(C);
    return 0;
}
/* end */