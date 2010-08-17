
#pragma once

#include <stdexcept>

#include "kernel_defs.hpp"
#include "maths_functions.hpp"

template<typename T>
__global__
void fill_device_kernel(T* first, T* last, T val)
{
	gpu_size_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	gpu_size_t thread_count = blockDim.x * gridDim.x;
	for( ; idx<last-first; idx+=thread_count ) {
		first[idx] = val;
	}
}

template<typename T>
void fill_device(T* first, T* last, T val, cudaStream_t stream=0)
{
	enum { BLOCK_SIZE = 256 };
	
	dim3 block(BLOCK_SIZE);
	dim3 grid(math::min(math::div_round_up(last-first, (long)BLOCK_SIZE),
	                    (long)CUDA_MAX_GRID_SIZE));
	
	fill_device_kernel<<<grid, block, 0, stream>>>(first, last, val);
	
#ifdef _DEBUG
	cudaStreamSynchronize(stream);
	cudaError_t error = cudaGetLastError();
	if( error != cudaSuccess ) {
		throw std::runtime_error(cudaGetErrorString(error));
	}
#endif
}
