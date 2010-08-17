
#pragma once

#include <stdexcept>
#include <string>

#include "kernel_defs.hpp"
#include "maths_functions.hpp"

template<gpu_size_t BLOCK_SIZE, typename Complex>
__global__
void complex_multiply_kernel(const Complex* d_in,
                             Complex* d_out,
                             gpu_size_t count)
{
	gpu_size_t thread_idx = threadIdx.x + blockIdx.x*BLOCK_SIZE;
	gpu_size_t thread_count = BLOCK_SIZE * gridDim.x;
	
    // Iterate over values in the input
	for( gpu_size_t idx=thread_idx; idx<count; idx+=thread_count ) {
		Complex a = d_in[idx];
		
		Complex result;
		result.x = a.x*b.x - a.y*b.y;
		result.y = a.y*b.x + a.x*b.y;
		d_out[idx] = result;
	}
}

template<typename Complex>
__global__
void complex_multiply_device(const Complex* d_in,
                             Complex* d_out,
                             gpu_size_t count,
                             cudaStream_t stream = 0)
{
	enum { BLOCK_SIZE = 256 };
	dim3 block(BLOCK_SIZE);
	dim3 grid(math::min(math::div_round_up(word_count,
	                                       (gpu_size_t)BLOCK_SIZE),
	                    (gpu_size_t)CUDA_MAX_GRID_SIZE));
	complex_multiply_kernel<<<grid, block, 0, stream>>>(d_in1,
	                                                    d_in2,
	                                                    d_out,
	                                                    count);
#ifdef _DEBUG
	cudaStreamSynchronize(stream);
	cudaError_t error = cudaGetLastError();
	if( error != cudaSuccess ) {
		throw std::runtime_error(std::string("In complex_multiply_kernel: ") +
		                         cudaGetErrorString(error));
	}
#endif
}
