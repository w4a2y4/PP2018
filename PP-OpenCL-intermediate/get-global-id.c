/* header */
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 

#include <stdio.h>
#include <assert.h>
#include <CL/cl.h>

#define MAXGPU 10
#define MAXK 1024
#define N 16
/* printid */
void printId(char *title, cl_uint id[N][N])
{
  puts(title);
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) 
      printf("%2d ", id[i][j]);
    printf("\n");
  }
}
/* main */
int main(int argc, char *argv[])
{
  assert(argc == 2);
  printf("Hello, OpenCL\n");
  cl_int status;
  cl_platform_id platform_id;
  cl_uint platform_id_got;
  status = clGetPlatformIDs(1, &platform_id, 
			    &platform_id_got);
  assert(status == CL_SUCCESS && platform_id_got == 1);
  printf("%d platform found\n", platform_id_got);
  cl_device_id GPU[MAXGPU];
  cl_uint GPU_id_got;
  status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 
			  MAXGPU, GPU, &GPU_id_got);
  assert(status == CL_SUCCESS);
  printf("There are %d GPU devices\n", GPU_id_got); 
  /* getcontext */
  cl_context context = 
    clCreateContext(NULL, GPU_id_got, GPU, NULL, NULL, 
		    &status);
  assert(status == CL_SUCCESS);
  /* commandqueue */
  cl_command_queue commandQueue =
    clCreateCommandQueue(context, GPU[0], 0, &status);
  assert(status == CL_SUCCESS);
  /* kernelsource */
  FILE *kernelfp = fopen(argv[1], "r");
  assert(kernelfp != NULL);
  char kernelBuffer[MAXK];
  const char *constKernelSource = kernelBuffer;
  size_t kernelLength = 
    fread(kernelBuffer, 1, MAXK, kernelfp);
  printf("The size of kernel source is %zu\n", kernelLength);
  cl_program program =
    clCreateProgramWithSource(context, 1, &constKernelSource, 
			      &kernelLength, &status);
  assert(status == CL_SUCCESS);
  /* buildprogram */
  status = 
    clBuildProgram(program, GPU_id_got, GPU, NULL, NULL, 
		   NULL);
  printf("status = %d\n", status);
  assert(status == CL_SUCCESS);
  printf("Build program completes\n");
  /* createkernel */
  cl_kernel kernel = clCreateKernel(program, "getGlobalId", &status);
  assert(status == CL_SUCCESS);
  printf("Build kernel completes\n");
  /* vector */
  cl_uint globalId[2][N][N];
  /* createbuffer */
  cl_mem bufferGlobalId = 
    clCreateBuffer(context, 
		   CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
		   2 * N * N * sizeof(cl_uint), globalId, &status);
  assert(status == CL_SUCCESS);
  printf("Build buffers completes\n");
  /* setarg */
  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), 
			  (void*)&bufferGlobalId);
  assert(status == CL_SUCCESS);
  printf("Set kernel arguments completes\n");
  /* setshape */
  size_t globalDim[] = {(size_t)N, (size_t)N};
  size_t localDim[] = {1, 1};
  status = 
    clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, 
			   globalDim, localDim, 
			   0, NULL, NULL);
  assert(status == CL_SUCCESS);
  printf("Specify the shape of the domain completes.\n");
  /* getresult */
  clFinish(commandQueue);
  printf("Kernel execution completes.\n");

  printId("get_global_id(0)", globalId[0]);
  printId("get_global_id(1)", globalId[1]);
  /* printids */
  clReleaseContext(context);	/* context etcmake */
  clReleaseCommandQueue(commandQueue);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseMemObject(bufferGlobalId);	/* buffers */
  return 0;
}
/* end */
