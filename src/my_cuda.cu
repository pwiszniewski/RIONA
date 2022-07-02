#include <cuda_runtime.h>
#include "cublas_v2.h"

extern "C" {
#include "my_cuda.h"
#include <cstdio>
#include <omp.h>
}

#define NUM_THREADS 6

cublasHandle_t handles[NUM_THREADS];

__global__ void saxpy_kernel(int n, float a, float * x, float * y){
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i < n) y[i] = a*x[i] + y[i];
}

__global__ void manhattan_kernel(int num_values2, float * values2, 
  float value1, float min, float max, float *dissim){
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  float temp;
  if (i < num_values2) 
  {
        temp = values2[i] - value1;
        temp = temp > 0 ? temp : -temp;
        dissim[i] += temp / (max - min);
  }
}

void my_cuda_init(int id)
{
  cublasStatus_t cbstatus = cublasCreate(&handles[id]);

  if (cbstatus != CUBLAS_STATUS_SUCCESS) {
    fprintf(stderr, "!!!! CUBLAS initialization error\n");
  }
}

void my_cuda_destroy(int id)
{
  cublasStatus_t cbstatus = cublasDestroy(handles[id]);
  if (cbstatus != CUBLAS_STATUS_SUCCESS) {
    fprintf(stderr, "!!!! CUBLAS destroy error\n");
  }
}



float *d_train, *d_test;
float *d_temp[NUM_THREADS];
int num_train, num_test, num_attr;

void copy_train_test_cuda(int num_train, int num_test, int num_attr, float *train,
    float *test)
{
  int n_bytes = num_train*num_attr*sizeof(train[0]);
  cudaMalloc(&d_train, n_bytes);
  cudaMemcpy(d_train, train, n_bytes, cudaMemcpyHostToDevice);

  n_bytes = num_train * sizeof(float);
    for (int i = 0; i < NUM_THREADS; i++) 
    {
      cudaMalloc(&d_temp[i], n_bytes);
    }

}

void free_test_train_cuda()
{
  cudaFree(d_train);

  for (int i = 0; i < NUM_THREADS; i++) 
  {
    cudaFree(&d_temp[i]);
  }
}

extern "C"
{


void calculate_manhattan_cuda(int id, float value1, int num_values2, int idx_value2, float *values2, 
int num_attr,float min, float max, float *dissim)
{
     size_t n_bytes = num_values2 * sizeof(float);

    cudaMemcpy(d_temp[id], dissim, n_bytes, cudaMemcpyHostToDevice);

    manhattan_kernel<<<4096,32>>>(num_values2, &d_train[idx_value2*num_values2], 
      value1, min, max, d_temp[id]);


      cudaMemcpy(dissim, d_temp[id], n_bytes, cudaMemcpyDeviceToHost);
}
}