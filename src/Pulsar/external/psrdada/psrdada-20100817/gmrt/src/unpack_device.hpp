
#pragma once

#include <string>
#include <stdexcept>

#include "kernel_defs.hpp"
#include "maths_functions.hpp"

// TODO: This transpose is only efficient when
//         BLOCK_SIZE / ant_count >= half warp size
//       E.g., BLOCK_SIZE=256 => ant_count <= 16
//       To process larger ant_count, do the unpack first,
//       then call a proper transpose kernel.
// Note: BLOCK_SIZE must be a power of 2
// ALGORITHM: transform/for_each (kernel uses shared mem)
template<unsigned int BLOCK_SIZE>
__global__
void unpack_to_real_ibob_kernel(const unsigned int* d_in,
                                float4* d_out,
                                gpu_size_t ant_count,
                                gpu_size_t log2_ant_count,
                                gpu_size_t out_stride)
{
	__shared__ float s_cache[4][BLOCK_SIZE];
	
	gpu_size_t blk = blockIdx.x * BLOCK_SIZE;
	gpu_size_t idx = blk + threadIdx.x;
	
	// Read a 32-bit word in (containing 4 8-bit samples from one antenna)
	unsigned int w = d_in[idx];
	
	// Unpack and convert to floats
	float4 result;
	result.x = (float)((char*)&w)[0];
	result.y = (float)((char*)&w)[1];
	result.z = (float)((char*)&w)[2];
	result.w = (float)((char*)&w)[3];
	
	// Note: Components are separated to avoid bank conflicts
	s_cache[0][threadIdx.x] = result.x;
	s_cache[1][threadIdx.x] = result.y;
	s_cache[2][threadIdx.x] = result.z;
	s_cache[3][threadIdx.x] = result.w;
	__syncthreads();
	
	// Write the unpacked samples to the correct antenna
	// Note: Shared mem is used to perform the transposed gather,
	//       which maintains coalesced writes to device mem.
	gpu_size_t i = threadIdx.x;
	gpu_size_t a = i & (ant_count-1); //i % ant_count;
	gpu_size_t t = i >> log2_ant_count; //i / ant_count;
	gpu_size_t time_per_block = BLOCK_SIZE >> log2_ant_count; // / ant_count;
	gpu_size_t log2_time_per_block = math::static_log2<BLOCK_SIZE>::value - log2_ant_count;
	
	// TODO: Check for bank conflicts
	result.x = s_cache[0][ (a<<log2_time_per_block) + t ];
	result.y = s_cache[1][ (a<<log2_time_per_block) + t ];
	result.z = s_cache[2][ (a<<log2_time_per_block) + t ];
	result.w = s_cache[3][ (a<<log2_time_per_block) + t ];
	// TODO: CHECK THAT THIS IS ALL CORRECT!
	d_out[(blk >> log2_ant_count) + // blk / ant_count +
	      i & (time_per_block-1) + // i % time_per_block +
	      (i >> log2_time_per_block) * out_stride] = result;
	
	// Non-coalesced way
	//gpu_size_t time = idx / ant_count;
	//gpu_size_t ant  = idx % ant_count;
	//d_out[time + ant*out_stride] = result;
}

void unpack_to_real_ibob_device(const unsigned int* d_in,
                                float4* d_out,
                                gpu_size_t in_count,
                                gpu_size_t ant_count,
                                gpu_size_t log2_ant_count,
                                gpu_size_t out_stride,
                                cudaStream_t stream = 0)
{
	enum { BLOCK_SIZE = 256 };
	dim3 block(BLOCK_SIZE);
	// TODO: Watch division
	dim3 grid(in_count / BLOCK_SIZE);
	
	// Dispatch on antenna count
	if( ant_count <= BLOCK_SIZE / (CUDA_WARP_SIZE/2) ) {
		// This version does a small transpose within the unpack kernel
		// TODO: Make this kernel robust against grid size
		unpack_to_real_ibob_kernel<BLOCK_SIZE><<<grid, block, 0, stream>>>
			(d_in, d_out, ant_count, log2_ant_count, out_stride);
	}
	else {
		// This version unpacks and transposes separately
		// TODO: Implement this!
		throw std::runtime_error("Sorry, unpack_to_real cannot yet handle this many antennas");
	}
	
#ifdef _DEBUG
	cudaStreamSynchronize(stream);
	cudaError_t cuda_error = cudaGetLastError();
	if( cuda_error != cudaSuccess ) {
		throw std::runtime_error(
			std::string("In unpack_to_real_ibob_device: ") +
			cudaGetErrorString(cuda_error));
	}
#endif
}

// Unpacks two 4-bit values into a single Complex value
template<typename Complex>
__global__
void unpack_to_complex_kernel(const unsigned char* d_in,
                              Complex* d_out,
                              gpu_size_t word_count)
{
	enum { BITS_PER_SAMPLE  = 4,
	       BIT_MASK         = (1 << BITS_PER_SAMPLE) - 1 };
	
	gpu_size_t thread_idx = threadIdx.x + blockIdx.x*blockDim.x;
	gpu_size_t thread_count = blockDim.x*gridDim.x;
	
	// Iterate over words in the input
	for( gpu_size_t idx=thread_idx; idx<word_count; idx+=thread_count ) {
		unsigned char w = d_in[idx];
		
		Complex sample;
		sample.x = (float)((w >> BITS_PER_SAMPLE) & BIT_MASK);
		sample.y = (float)((w                   ) & BIT_MASK);
		
		d_out[idx] = sample;
	}
}

// Unpacks two 8-bit values into a single Complex value
template<typename Complex>
__global__
void unpack_to_complex_kernel(const unsigned short* d_in,
                              Complex* d_out,
                              gpu_size_t word_count)
{
	enum { BITS_PER_SAMPLE  = 8,
	       BIT_MASK         = (1 << BITS_PER_SAMPLE) - 1 };
	
	gpu_size_t thread_idx = threadIdx.x + blockIdx.x*blockDim.x;
	gpu_size_t thread_count = blockDim.x*gridDim.x;
	
	// Iterate over words in the input
	for( gpu_size_t idx=thread_idx; idx<word_count; idx+=thread_count ) {
		unsigned short w = d_in[idx];
		
		Complex sample;
		sample.x = (float)((unsigned char*)&w)[0];
		sample.y = (float)((unsigned char*)&w)[1];
		
		d_out[idx] = sample;
	}
}

template<typename Word, typename Complex>
void unpack_to_complex_device(const Word* d_in, Complex* d_out,
                              gpu_size_t word_count, cudaStream_t stream=0)
{
	enum { BLOCK_SIZE = 256 };
	dim3 block(BLOCK_SIZE);
	dim3 grid(math::min(math::div_round_up(word_count,
	                                       (gpu_size_t)BLOCK_SIZE),
	                    (gpu_size_t)CUDA_MAX_GRID_SIZE));
	//cout << "unpack grid: " << grid.x << endl;
	//cout << "      block: " << block.x << endl;
	unpack_to_complex_kernel<<<grid, block, 0, stream>>>(d_in, d_out, word_count);
#ifdef _DEBUG
	cudaStreamSynchronize(stream);
	cudaError_t error = cudaGetLastError();
	if( error != cudaSuccess ) {
		throw std::runtime_error(std::string("In unpack_to_complex_device: ") +
		                         cudaGetErrorString(error));
	}
#endif
}

// Unpacks two 4-bit values into two real values
__global__
void unpack_to_real_kernel(const unsigned char* d_in,
                           float2* d_out,
                           gpu_size_t word_count)
{
	enum { BITS_PER_SAMPLE  = 4,
	       BIT_MASK         = (1 << BITS_PER_SAMPLE) - 1 };
	
	gpu_size_t thread_idx = threadIdx.x + blockIdx.x*blockDim.x;
	gpu_size_t thread_count = blockDim.x*gridDim.x;
	
	// Iterate over words in the input
	for( gpu_size_t idx=thread_idx; idx<word_count; idx+=thread_count ) {
		unsigned char w = d_in[idx];
		
		float2 result;
		// TODO: Check the ordering here
		result.x = (float)((w >> BITS_PER_SAMPLE) & BIT_MASK);
		result.y = (float)((w                   ) & BIT_MASK);
		
		d_out[idx] = result;
	}
}

// Unpacks one 8-bit value into one float value
__global__
void unpack_8bit_to_real_kernel(const unsigned char* d_in,
                                float*               d_out,
                                gpu_size_t           word_count)
{
	gpu_size_t thread_idx = threadIdx.x + blockIdx.x*blockDim.x;
	gpu_size_t thread_count = blockDim.x*gridDim.x;
	
	// Iterate over words in the input
	for( gpu_size_t idx=thread_idx; idx<word_count; idx+=thread_count ) {
		// Convert to float
		d_out[idx] = (float)d_in[idx];
	}
}

template<typename Word>
void unpack_to_real_device(const Word* d_in, float* d_out,
                           gpu_size_t word_count, gpu_size_t nbits,
                           cudaStream_t stream=0)
{
	enum { BLOCK_SIZE = 256 };
	dim3 block(BLOCK_SIZE);
	dim3 grid(math::min(math::div_round_up(word_count,
	                                       (gpu_size_t)BLOCK_SIZE),
	                    (gpu_size_t)CUDA_MAX_GRID_SIZE));
	//cout << "unpack grid: " << grid.x << endl;
	//cout << "      block: " << block.x << endl;
	if( nbits == 4 ) {
		//cout << "Unpacking 4-bit data..." << endl;
		unpack_to_real_kernel<<<grid, block, 0, stream>>>(d_in,
		                                                  (float2*)d_out,
		                                                  word_count);
	}
	else if( nbits == 8 ) {
		unpack_8bit_to_real_kernel<<<grid, block, 0, stream>>>(d_in,
		                                                       (float*)d_out,
		                                                       word_count);
	}
	else {
		throw std::runtime_error(std::string("Cannot unpack this data format"));
	}
	
#ifdef _DEBUG
	cudaStreamSynchronize(stream);
	cudaError_t error = cudaGetLastError();
	if( error != cudaSuccess ) {
		throw std::runtime_error(std::string("In unpack_to_real_device: ") +
		                         cudaGetErrorString(error));
	}
#endif
}
