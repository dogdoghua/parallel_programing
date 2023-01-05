
#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"

// handle device & host communication
void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage, cl_device_id *device,
            cl_context *context, cl_program *program)
{
    cl_int status;
    int filterSize = filterWidth * filterWidth;
    int imageSize = imageHeight * imageWidth;
    
    char *source = readSource("kernel.cl");
    cl_command_queue commandQueue = clCreateCommandQueue(*context, *device, 0, &status);
    CHECK(status, "clCreateCommandQueue");
    
    // create memory buffers on the device
    cl_mem inputImgMem = clCreateBuffer(*context, CL_MEM_READ_ONLY, imageSize*sizeof(float), NULL, NULL);
    cl_mem filterMem = clCreateBuffer(*context, CL_MEM_READ_ONLY, filterSize*sizeof(float), NULL, NULL);
    cl_mem outputImgMem = clCreateBuffer(*context, CL_MEM_WRITE_ONLY, imageSize*sizeof(float), NULL, NULL);
    status = !inputImgMem || !filterMem || !outputImgMem;
    CHECK(status, "clCreateBuffer"); 
    
    // write to memory buffer from host memory
    status |= clEnqueueWriteBuffer(commandQueue, inputImgMem, CL_TRUE, 0, imageSize*sizeof(float), inputImage, 0, NULL, NULL);
    status |= clEnqueueWriteBuffer(commandQueue, filterMem, CL_TRUE, 0, filterSize*sizeof(float), filter, 0, NULL, NULL);
    CHECK(status, "clEnqueueWriteBuffer");
    
    // create kernel structure
    cl_kernel kernel = clCreateKernel(*program, "convolution", &status);
    CHECK(status, "clCreateKernel");
    
    // set the arguments of the kernel
    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputImgMem);
    clSetKernelArg(kernel, 1, sizeof(cl_int), (void *)&imageWidth);
    clSetKernelArg(kernel, 2, sizeof(cl_int), (void *)&imageHeight);
    
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&filterMem);
    clSetKernelArg(kernel, 4, sizeof(cl_int), (void *)&filterWidth);
    
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&outputImgMem);
    
    // work sizes=> globalSize % localSize==0
    size_t globalSize=imageSize;
    size_t localSize=64;
    
    // enqueues a command to execute a kernel on a devices
    status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &globalSize, &localSize, 0, NULL, NULL);
    CHECK(status, "clEnqueueNDRangeKernel");
    
    // copy program result from device to host
    status = clEnqueueReadBuffer(commandQueue, outputImgMem, CL_TRUE, 0, globalSize*sizeof(float), outputImage, 0, NULL, NULL);
    CHECK(status, "clEnqueueReadBuffer");
    
    clReleaseKernel(kernel);
    clReleaseMemObject(inputImgMem);
    // clReleaseMemObject(filterMem);
    clReleaseMemObject(outputImgMem);
    clReleaseCommandQueue(commandQueue);
}