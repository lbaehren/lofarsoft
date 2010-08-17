
#pragma once

#include <stdexcept>
#include <string>

#include "kernel_defs.hpp"
#include "maths_functions.hpp"

// Note: coef should be e^-2*pi*i*delay
template<gpu_size_t BLOCK_SIZE, typename Complex, typename Real>
__global__
void phase_shift_kernel(const Complex* d_in,
                        Complex*       d_out,
                        Real           coef,
                        gpu_size_t     count)
{
	gpu_size_t thread_idx = threadIdx.x + blockIdx.x*BLOCK_SIZE;
	gpu_size_t thread_count = BLOCK_SIZE * gridDim.x;
	
    // Iterate over values in the input
	for( gpu_size_t idx=thread_idx; idx<count; idx+=thread_count ) {
		Complex a = d_in[idx];
		
		Complex phase;
		// TODO: Check if pre-computing these and loading from mem is faster
		__sincosf(coef*idx, &phase.y, &phase.x);
		
		Complex result;
		result.x = a.x*phase.x + a.y*phase.y;
		result.y = a.y*phase.x - a.x*phase.y;
		d_out[idx] = result;
	}
}

template<typename Complex, typename Real>
void phase_shift_device(const Complex* d_in,
                        Complex*       d_out,
                        gpu_size_t     count,
                        Real           delay,
                        cudaStream_t   stream = 0)
{
	enum { BLOCK_SIZE = 256 };
	dim3 block(BLOCK_SIZE);
	dim3 grid(math::min(math::div_round_up(count,
	                                       (gpu_size_t)BLOCK_SIZE),
	                    (gpu_size_t)CUDA_MAX_GRID_SIZE));
	
	const float two_pi = 2.f*3.141592f;
	/*Complex coef;
	coef.x =  cos(two_pi * delay) / count;
	coef.y = -sin(two_pi * delay) / count;
	*/
	Real coef = two_pi * delay / count;
	phase_shift_kernel<BLOCK_SIZE><<<grid, block, 0, stream>>>(d_in,
	                                                           d_out,
	                                                           coef,
	                                                           count);
#ifdef _DEBUG
	cudaStreamSynchronize(stream);
	cudaError_t error = cudaGetLastError();
	if( error != cudaSuccess ) {
		throw std::runtime_error(std::string("In phase_shift_kernel: ") +
		                         cudaGetErrorString(error));
	}
#endif
}
