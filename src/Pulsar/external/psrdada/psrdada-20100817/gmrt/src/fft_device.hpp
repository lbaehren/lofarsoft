
#pragma once

#include <cufft.h>
#include <stdexcept>

#include "maths_functions.hpp"
#include "kernel_defs.hpp"

template<typename Real, typename Complex>
void fft_r2c_device(const Real* in, Complex* out,
                    size_t length, size_t count,
                    cudaStream_t stream = 0)
{
	cufftResult error;
	cufftHandle plan;
	error = cufftPlan1d(&plan, length, CUFFT_R2C, count);
	if( error != CUFFT_SUCCESS ) {
		throw std::runtime_error("Failed to create cufft 1d plan");
	}
	error = cufftSetStream(plan, stream);
	if( error != CUFFT_SUCCESS ) {
		throw std::runtime_error("Failed to set cufft stream");
	}
	error = cufftExecR2C(plan, (cufftReal*)in, (cufftComplex*)out);
	if( error != CUFFT_SUCCESS ) {
		throw std::runtime_error("Failed to execute cufft plan");
	}
	cufftDestroy(plan);
}

template<typename Complex>
void fft_c2c_device(const Complex* in, Complex* out, int direction,
                    size_t length, size_t count,
                    cudaStream_t stream = 0)
{
	cufftResult error;
	cufftHandle plan;
	error = cufftPlan1d(&plan, length, CUFFT_C2C, count);
	if( error != CUFFT_SUCCESS ) {
		throw std::runtime_error("Failed to create cufft 1d plan");
	}
	error = cufftSetStream(plan, stream);
	if( error != CUFFT_SUCCESS ) {
		throw std::runtime_error("Failed to set cufft stream");
	}
	error = cufftExecC2C(plan, (cufftComplex*)in, (cufftComplex*)out, direction);
	if( error != CUFFT_SUCCESS ) {
		throw std::runtime_error("Failed to execute cufft plan");
	}
	cufftDestroy(plan);
}

/* TODO: This is not used at the moment
// Note: coef should be 2 * pi / length
// ALGORITHM: transform + counting_iterator
template<typename Complex>
__global__
void compute_twiddles_kernel(Complex* d_twiddles, float coef)
{
	gpu_size_t idx = blockIdx.x*blockDim.x + threadIdx.x;
	
	float theta = coef * idx;
	Complex result;
	__sincosf(theta, &result.y, &result.x);
	d_twiddles[idx] = result;
}
*/

// This is the "realtr" trick, which performs the final radix-2 butterfly to
//   complete an R2C FFT performed using a C2C FFT of half the length.
// Note: This version produces exactly N/2 complex outputs by packing the
//       Nyquist value into the imaginary component of the 0th element.
// Note: coef should be 2 * pi / length
// Note: The computation is performed in-place
// Note: length is the number of Real samples
template<typename Complex>
__global__
void finish_real_fft_kernel(Complex* d_fft, gpu_size_t length_on_4, float coef)
{
	gpu_size_t i = blockIdx.x*blockDim.x + threadIdx.x;
	gpu_size_t batch_idx = blockIdx.y;
	gpu_size_t length_on_2 = length_on_4 * 2;
	
	// Avoid excess threads
	// TODO: Replace this with a loop over the grid to handle
	//       insufficient or excess threads.
	// Note: We need exactly length/4+1 threads
	if( i > length_on_4 )
		return;
	
	Complex result;
	
	// Special case for the 0th element, which will hold the DC and Nyquist vals
	if( 0 == i ) {
		Complex d_fft0 = d_fft[0 + batch_idx * length_on_2];
		// DC
		result.x = d_fft0.x + d_fft0.y;
		// Nyquist (packed here for convenience)
		result.y = d_fft0.x - d_fft0.y;
		d_fft[0 + batch_idx * length_on_2] = result;
		return;
	}
	
	// Compute twiddle factors
	// TODO: Check if it's faster to pre-compute these and load them from memory
	Complex twiddle;
	__sincosf(coef * i, &twiddle.y, &twiddle.x);
	
	// Grab one element from the start and one from the end
	gpu_size_t k = length_on_2 - i;
	Complex fft_i = d_fft[i + batch_idx * length_on_2];
	Complex fft_k = d_fft[k + batch_idx * length_on_2];
	
	Complex a, b;
	a.x = fft_i.x + fft_k.x;
	a.y = fft_i.y + fft_k.y;
	b.x = fft_k.x - fft_i.x;
	b.y = fft_k.y - fft_i.y;
	
	result.x = twiddle.x*a.y + twiddle.y*b.x;
	result.y = twiddle.y*a.y - twiddle.x*b.x;
	
	d_fft[i + batch_idx * length_on_2].x =  0.5f*(result.x + a.x);// /(length_on_2*2);
	d_fft[i + batch_idx * length_on_2].y = -0.5f*(result.y + b.y);// /(length_on_2*2);
	d_fft[k + batch_idx * length_on_2].x = -0.5f*(result.x - a.x);// /(length_on_2*2);
	d_fft[k + batch_idx * length_on_2].y = -0.5f*(result.y - b.y);// /(length_on_2*2);
}

// TEST
#include <iostream>
#include "stopwatch.hpp"

// TODO: This hack needs to be done properly!
//cufftHandle fft_r2c_short_plan;
void init_fft_r2c_short_device(size_t length, size_t count, cudaStream_t stream=0) {
	/*
	cufftResult error;
	error = cufftPlan1d(&fft_r2c_short_plan, length/2, CUFFT_C2C, count);
	if( error != CUFFT_SUCCESS ) {
		throw std::runtime_error("Failed to create cufft 1d plan");
	}
	*/
}

// Note: This version produces exactly N/2 complex outputs by packing the
//       Nyquist value into the imaginary component of the 0th element.
// TODO: Check if this only works for power of two lengths
template<typename Real, typename Complex>
void fft_r2c_short_device(const Real* in, Complex* out,
                          size_t length, size_t count,
                          cudaStream_t stream = 0)
{
	// First part...
	
	Stopwatch timer;
	timer.start();
	
	cufftResult error;
	
	cufftHandle fft_r2c_short_plan;
	// TODO: Check if this is correct for odd length
	
	error = cufftPlan1d(&fft_r2c_short_plan, length/2, CUFFT_C2C, count);
	if( error != CUFFT_SUCCESS ) {
		throw std::runtime_error("Failed to create cufft 1d plan");
	}
	
	error = cufftSetStream(fft_r2c_short_plan, stream);
	if( error != CUFFT_SUCCESS ) {
		throw std::runtime_error("Failed to set cufft stream");
	}
	
	//Stopwatch inner_timer;
	//inner_timer.start();
	error = cufftExecC2C(fft_r2c_short_plan,
	                     (cufftComplex*)in, (cufftComplex*)out,
	                     CUFFT_FORWARD);
	if( error != CUFFT_SUCCESS ) {
		throw std::runtime_error("Failed to execute cufft plan");
	}
	//cudaStreamSynchronize(stream);
	//inner_timer.stop();
	//std::cout << "***       C2C time: " << inner_timer.getTime() << endl;
	cufftDestroy(fft_r2c_short_plan);
	
	timer.stop();
	//std::cout << "*** C2C time: " << timer.getTime() << endl;
	
	
	// Second part...
	
	//Stopwatch timer2;
	//timer2.start();
	
	enum { BLOCK_SIZE = 256 };
	
	dim3 block(BLOCK_SIZE);
	// TODO: Watch grid dim limits
	dim3 grid(math::div_round_up(length/4+1, (size_t)BLOCK_SIZE),
	          count);
	//cout << "length: " << length << endl;
	//cout << "block: " << block.x << endl;
	//cout << "grid: " << grid.x << endl;
	
	// Complete the R2C FFT using a final radix-2 butterfly computation
	finish_real_fft_kernel<<<grid, block, 0, stream>>>
		(out, length/4, 2.0 * 3.14159265358979 / length);
	
#ifdef _DEBUG
	cudaStreamSynchronize(stream);
	cudaError_t cuda_error = cudaGetLastError();
	if( cuda_error != cudaSuccess ) {
		throw std::runtime_error(std::string("In fft_r2c_short_device: ") +
		                         cudaGetErrorString(cuda_error));
	}
#endif
	
	//timer2.stop();
	//  std::cout << "*** realtr time: " << timer2.getTime() << endl;
}
