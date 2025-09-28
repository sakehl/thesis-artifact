#include <iostream>
#include <cuda_runtime.h>

// CUDA kernel for vector addition
__global__ void vectorAddKernel(int *a, int *b, int *c, int n) { //~|\label{line:vector-add-kernel-1}|
 int i = blockIdx.x * blockDim.x + threadIdx.x;  //~|\label{line:vector-add-tid}|
 if (i < n)
  c[i] = a[i] + b[i]; //~|\label{line:vector-add-add}|
} //~|\label{line:vector-add-kernel-2}|

void vectorAdd(int *a, int *b, int *c, int n) { //~|\label{line:vector-add-host-1}|
  // Allocate device memory
 int *d_a, *d_b, *d_c;
 int size = n * sizeof(int);
 cudaMalloc((void**)&d_a, size);
 cudaMalloc((void**)&d_b, size);
 cudaMalloc((void**)&d_c, size);
 // Copy input data from host to device
 cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice); //~|\label{line:vector-add-memcopy-1}|
 cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice); //~|\label{line:vector-add-memcopy-2}|
 // Launch kernel
 int threadsPerBlock = 256;
 int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
 vectorAddKernel<<<blocksPerGrid, threadsPerBlock>>>(d_a, d_b, d_c, n); //~|\label{line:vector-add-kernel-call}|
 // Copy result back to host
 cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost); //~|\label{line:vector-add-memcopy-3}|
 cudaFree(d_a); cudaFree(d_b); cudaFree(d_c);
} //~|\label{line:vector-add-host-2}|

int main() {
 int n = 1 << 20; // 1M elements 
 // Allocate host memory
 int *a = new int[n];
 int *b = new int[n];
 int *c = new int[n];
 // Initialize input vectors
 for (int i = 0; i < n; ++i) {
  a[i] = i;
  b[i] = i * 2;
 }
 // Perform vector addition
 vectorAdd(a, b, c, n);
 // Verify result
 bool success = true;
 for (int i = 0; i < n; ++i) {
  if (a[i] + b[i] != c[i]) {
    std::cout << "Mismatch at index " << i << ": " << a[i] << " + " << b[i] << " != " << c[i] << std::endl;
    success = false;
    break;
  }
 }
 
 std::cout << (success ? "Result correct!" : "Result incorrect!") << std::endl;
 
 // Cleanup
 delete[] a;
 delete[] b;
 delete[] c;
 
 return 0;
}