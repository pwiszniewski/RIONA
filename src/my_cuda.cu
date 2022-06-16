#include <cuda_runtime.h>
#include "cublas_v2.h"
extern "C" {
#include "my_cuda.h"
#include <cstdio>
#include <omp.h>
}

// __global__
// void saxpy(int n, float a, float * restrict x, float * restrict y)
// {
//   int i = blockIdx.x*blockDim.x + threadIdx.x;
//   if (i < n) y[i] = a*x[i] + y[i];
// }



extern "C"
{
void calculate_manhattan_cuda(float value1, int num_values2, int idx_value2, float *value2, 
int num_attr,float min, float max, float *dissim)
{

//   int dev = findCudaDevice(argc, (const char **)argv);

//   if (dev == -1) {
//     printf("No CUDA device found");
//   }
cudaError_t status;
status = cudaSetDevice(0);
if (status != cudaSuccess)
{
   fprintf(stderr, "!!!! CUDA initialization error\n"); 
}

// int N = num_values2*num_attr;
// #pragma omp single
// #pragma omp criticall
{
    // printf("kernel\n");
cublasHandle_t handle;
cublasStatus_t cbstatus = cublasCreate(&handle);

  if (cbstatus != CUBLAS_STATUS_SUCCESS) {
    fprintf(stderr, "!!!! CUBLAS initialization error\n");
  }

    float temp[num_values2];
    for (int i = 0; i < num_values2; i++) 
    {
        temp[i] = value1;
    }
float *x, *y, alpha=-1.0;
cudaMalloc(&x, num_values2 * sizeof(float));
cudaMalloc(&y, num_values2 * sizeof(float));
cublasSaxpy(handle, num_values2, &alpha, x, 1, y, 1);
// cublasDestroy(handle);
cudaFree(x);
cudaFree(y);
}
//     cblas_scopy(num_values2, &value1, 0, temp, 1);
//     cblas_saxpy(num_values2, -1, &value2[idx_value2], num_attr, temp, 1);

// // Perform SAXPY on 1M elements
// saxpy<<<4096,256>>>(N, 2.0, d_x, d_y);

// cudaMemcpy(y, d_y, N, cudaMemcpyDeviceToHost);





    //     float temp ;
    // for (int i = 0; i < num_values2; i++) 
    // {
        
    //     temp = value2[idx_value2*num_values2 + i] - value1;
    //     temp = temp > 0 ? temp : -temp;
    //     dissim[i] += temp / (max - min);
    // }



    // printf("kernel\n");
    // float temp[num_values2];
    // float *temp = malloc(sizeof(*temp) * num_values2);
    // float temp ;
    // for (int i = 0; i < num_values2; i++) 
    // {
    //     // printf("%d\n", i);
    //     temp = value1 - value2[idx_value2 + num_attr*i];
    //     temp = temp > 0 ? temp : -temp;
    //     temp = 1 / (max - min);
    //     dissim[i] += temp;
    // }

    // free(temp);
}


}

// void calculate_manhattan_vect(float value1, int num_values2, int idx_value2, float *value2, 
// int num_attr,float min, 
// float max, float *dissim)
// {

    // float temp[num_values2];
    // for (int i = 0; i < num_values2; i++) 
    // {
    //     temp[i] = value1 - value2[idx_value2 + num_attr*i];
    //     temp[i] = temp[i] > 0 ? temp[i] : -temp[i];
    //     temp[i] = 1 / (max - min);
    //     dissim[i] += temp[i];
    // }
// ///////////////////////////////

//     float temp[num_values2];
//     // for (int i = 0; i < num_values2; i++) 
//     // {
//     //     temp[i] = value1;
//     // }
//     cblas_scopy(num_values2, &value1, 0, temp, 1);
//     cblas_saxpy(num_values2, -1, &value2[idx_value2], num_attr, temp, 1);

//     for (int i = 0; i < num_values2; i++) 
//     {
//         temp[i] = temp[i] > 0 ? temp[i] : -temp[i];
//     }
//     // cblas_scabs1(num_values2, 1, temp, 1, dissim, 1);
//     cblas_sscal(num_values2, 1 / (max - min), temp, 1);
//     // cblas_saxpy(num_values2, 1, temp, 1, dissim, 1);
//     for (int i = 0; i < num_values2; i++) 
//     {
//         dissim[i] += temp[i];
//     }
// }