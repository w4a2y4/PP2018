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
#define SIZE 1024
#define DEVICENUM 2

static inline uint32_t rotate_left(uint32_t x, uint32_t n) {
    return  (x << n) | (x >> (32-n));
}
static inline uint32_t encrypt(uint32_t m, uint32_t key) {
    return (rotate_left(m, key&31) + key)^key;
}

/* main */
int main(void)
{

    cl_int status;
    cl_platform_id platform_id;
    status = clGetPlatformIDs(1, &platform_id, NULL);
    assert(status == CL_SUCCESS);
 
    cl_device_id GPU[DEVICENUM];
    status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, DEVICENUM, GPU, NULL);
    assert(status == CL_SUCCESS);
 
    /* getcontext */
    cl_context context = clCreateContext(NULL, DEVICENUM, GPU, NULL, NULL, &status);
    assert(status == CL_SUCCESS);
 
    /* commandqueue */
    cl_command_queue commandQueue[DEVICENUM];
    for ( int i = 0; i < DEVICENUM; i++ ) {
        commandQueue[i] = clCreateCommandQueue(context, GPU[i], 0, &status);
        assert(status == CL_SUCCESS);
    }
 
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
    status = clBuildProgram(program, DEVICENUM, GPU, NULL, NULL, NULL);
    if (status != CL_SUCCESS) {
        char program_log[MAXK];
        for (int device = 0; device < DEVICENUM; device++) {
            clGetProgramBuildInfo(program, GPU[device], 
                CL_PROGRAM_BUILD_LOG, MAXK, program_log, NULL);
            puts(program_log);
        }
        exit(-1);
    }
    // printf("Build program succeed\n");
 
    /* createkernel */
    cl_kernel kernel = clCreateKernel(program, "add", &status);
    assert(status == CL_SUCCESS);
    // printf("Create kernel succeed\n");
 
    unsigned int N, key1, key2;

    /* createbuffer */
    cl_mem bufferC[DEVICENUM];
    cl_uint* C = (cl_uint*)malloc(8388608 * sizeof(cl_uint));
    assert(C != NULL);
    for( int i = 0; i < DEVICENUM; i++ ) {
        // C[i] = (cl_uint*)malloc(2097152 * sizeof(cl_uint));
        // assert(C[i] != NULL);
        bufferC[i] = clCreateBuffer(context, 
            CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
            4194304 * sizeof(cl_uint),
            C + i * 4194304, &status);
        assert(status == CL_SUCCESS);
    }
    // printf("Create buffer succeed\n");

    while( scanf("%u%u%u", &N, &key1, &key2) == 3) {
 
        int newN = N/DEVICENUM;
        int gl = (newN/SIZE) - (newN/SIZE)%CHUNK + CHUNK;
        uint32_t sum = 0;

        for( int d = 0; d < DEVICENUM; d++ ) {

            int base = d * newN;

            /* setarg */
            status = clSetKernelArg(kernel, 0, sizeof(cl_uint), (void *)&key1);
            assert(status == CL_SUCCESS);
            status = clSetKernelArg(kernel, 1, sizeof(cl_uint), (void *)&key2);
            assert(status == CL_SUCCESS);
            status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)(&bufferC[d]));
            assert(status == CL_SUCCESS);
            status = clSetKernelArg(kernel, 3, sizeof(cl_uint), (void *)&newN);
            assert(status == CL_SUCCESS);
            status = clSetKernelArg(kernel, 4, sizeof(cl_uint), (void *)&base);
            assert(status == CL_SUCCESS);
            // printf("Set arg succeed\n");

            /* setshape */
            size_t globalThreads[] = {(size_t)( gl )};
            size_t localThreads[] = {CHUNK};
     
            status = clEnqueueNDRangeKernel(commandQueue[d], kernel, 1, NULL, 
                       globalThreads, localThreads, 0, NULL, NULL);
            assert(status == CL_SUCCESS);
            // printf("clEnqueueNDRangeKernel\n");
        }

        for( int d = 0; d < DEVICENUM; d++ ) {
            /* getcvector */
            status = clEnqueueReadBuffer(commandQueue[d], bufferC[d], CL_TRUE, 
                      0, gl * sizeof(cl_uint), C + d * 4194304, 
                      0, NULL, NULL);
            assert(status == CL_SUCCESS);
            // printf("clEnqueueReadBuffer\n");
     
            // print
            for (int i = 0; i < gl/CHUNK; i++) {
                sum += *(C + d * 4194304 + i);
            }
        }

        for ( int i = newN * DEVICENUM; i < N; i++ )
            sum += encrypt(i, key1) * encrypt(i, key2);
 
        printf("%" PRIu32 "\n", sum);
    }
 
    clReleaseContext(context);    /* context etcmake */
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    for ( int d = 0; d < DEVICENUM; d++ ) {
        clReleaseMemObject(bufferC[d]);
        clReleaseCommandQueue(commandQueue[d]);
        // free(C[d]);
    }
    free(C);

    return 0;
}
/* end */