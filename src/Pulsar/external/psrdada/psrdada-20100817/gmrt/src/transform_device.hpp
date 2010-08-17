
#pragma once

#include <stdexcept>
#include <string>

#include "kernel_defs.hpp"
#include "maths_functions.hpp"

namespace detail {

template<gpu_size_t BLOCK_SIZE,
         typename InType, typename OutType, class UnaryFunc>
__global__
void transform_kernel(const InType d_in1,
                      OutType      d_out,
                      UnaryFunc    op,
                      gpu_size_t   count)
{
	// Simple 1D thread decomposition
	gpu_size_t thread_idx = threadIdx.x + blockIdx.x*BLOCK_SIZE;
	gpu_size_t thread_count = BLOCK_SIZE * gridDim.x;
	
    // Iterate over values in the input
	for( gpu_size_t idx=thread_idx; idx<count; idx+=thread_count ) {
		// Apply the unary transform operation
		d_out[idx] = (OutType)op(d_in[idx]);
	}
}

template<gpu_size_t BLOCK_SIZE,
         typename InType1, typename InType2,
         typename OutType, class BinaryFunc>
__global__
void transform_kernel(const InType1 d_in1,
                      const InType2 d_in2,
                      OutType       d_out,
                      BinaryFunc    op,
                      gpu_size_t    count)
{
	// Simple 1D thread decomposition
	gpu_size_t thread_idx = threadIdx.x + blockIdx.x*BLOCK_SIZE;
	gpu_size_t thread_count = BLOCK_SIZE * gridDim.x;
	
    // Iterate over values in the input
	for( gpu_size_t idx=thread_idx; idx<count; idx+=thread_count ) {
		// Apply the binary transform operation
		d_out[idx] = (OutType)op(d_in1[idx], d_in2[idx]);
	}
}

inline void get_1D_thread_decomp(size_t block, size_t& grid, size_t count) {
	size_t grid = math::min(math::div_round_up(count,
	                                           (gpu_size_t)block),
	                        (gpu_size_t)CUDA_MAX_GRID_SIZE);
}

inline void kernel_debug(std::string name, cudaStream_t stream) {
#ifdef _DEBUG
	cudaStreamSynchronize(stream);
	cudaError_t error = cudaGetLastError();
	if( error != cudaSuccess ) {
		throw std::runtime_error("In " + name + ": " +
		                         cudaGetErrorString(error));
	}
#endif
}

} // namespace detail

/*
template<typename Real, typename Complex>
struct phase_shifter {
	Complex phase;
	phase_shifter(Real delay) {
		const float two_pi = 2.f*3.141592f;
		// TODO: CHECK THIS!
		phase.x = cos(two_pi * delay);
		phase.y = -sin(two_pi * delay);
	}
	inline __host__ __device__
	Complex operator()(const Complex& a, gpu_size_t i) const {
		Complex result;
		result.x = a.x*phase.x - a.y*phase.y;
		result.y = a.y*phase.x + a.x*phase.y;
		result.x *= (Real)i;
		result.y *= (Real)i;
		return result;
	}
};
*/


// TODO: This is getting complicated. USE THRUST!
template<typename T>
struct counting_iterator {
	
	__inline__ __host__ __device__
	T operator[]() {
		return threadIdx.x + blockIdx.x*blockDim.x;
	}
};

template<typename InType, typename OutType, class UnaryFunc>
__global__
void transform_device(const InType d_first,
                      const InType d_last,
                      OutType      d_out,
                      UnaryFunc    op,
                      cudaStream_t stream = 0)
{
	enum { BLOCK_SIZE = 256 };
	
	size_t count = d_last - d_first;
	
	// Calculate thread decomposition
	size_t block = BLOCK_SIZE, grid;
	detail::get_1D_thread_decomp(block, grid, count);
	
	// Execute the device kernel
	detail::transform_kernel<BLOCK_SIZE><<<grid, block, 0, stream>>>
		(d_first, d_out, op, count);
	detail::kernel_debug("unary transform_device", stream);
}

template<typename InType1, typename InType2,
         typename OutType, class BinaryFunc>
__global__
void transform_device(const InType1* d_first1,
                      const InType1* d_last1,
                      const InType2* d_first2,
                      OutType*       d_out,
                      BinaryFunc     op,
                      cudaStream_t   stream = 0)
{
	enum { BLOCK_SIZE = 256 };
	
	size_t count = d_last - d_first;
	
	// Calculate thread decomposition
	size_t block = BLOCK_SIZE, grid;
	detail::get_1D_thread_decomp(block, grid, count);
	
	// Execute the device kernel
	detail::transform_kernel<BLOCK_SIZE><<<grid, block, 0, stream>>>
		(d_first1, d_first2, d_out, op, count);
	
	detail::kernel_debug("binary transform_device", stream);
}
