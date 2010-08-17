
#pragma once

/*
	 Total intensity [NOT YET IMPLEMENTED]
	    R0 R1 R2 R3 L0 L1 L2 L3
	  R0 *  *  *  *
	  R1    *  *  *
	  R2       *  *
	  R3          *
	  L0             *  *  *  *
	  L1                *  *  *
	  L2                   *  *
	  L3                      *
*/

/*
	  Full polar [As implemented below]
	    R0 L0 R1 L1 R2 L2 R3 L3
	  R0 *  *  *  *  *  *  *  *
	  L0 *  *  *  *  *  *  *  *
	  R1       *  *  *  *  *  *
	  L1       *  *  *  *  *  *
	  R2             *  *  *  *
	  L2             *  *  *  *
	  R3                   *  *
	  L3                   *  *
*/

// Note: This kernel computes the full-polar correlation where
//         polarisations are adjacent in memory.
template<bool DO_FULL_POLAR, gpu_size_t BD_X, gpu_size_t BD_Y, class T>
__global__
void mac_device_kernel(const T*   d_in,
                       gpu_size_t in_stride1,
                       gpu_size_t in_stride2,
                       gpu_size_t ant_block_count, // Needed for manual grid decomp
                       gpu_size_t accum_count,
                       T*         d_out,
                       gpu_size_t out_stride1,
                       gpu_size_t out_stride2) {
	
	//                         ant   freq
	__shared__ float s_aj_real[BD_Y][BD_X];
	__shared__ float s_aj_imag[BD_Y][BD_X];
	
	// Block decomposition
	// -------------------
	// Note: We manually decompose the 2nd grid dim into two dims
	gpu_size_t blk_ai = blockIdx.y % ant_block_count;
	gpu_size_t blk_aj = blockIdx.y / ant_block_count;
	
	// Apply diagonal reordering to avoid partition camping
	gpu_size_t old_blk_ai = blk_ai;
	blk_ai = blk_aj;
	blk_aj = (blk_aj + old_blk_ai) % ant_block_count;
	
	// Calculate the corresponding sample indices
	gpu_size_t blk_ai_idx = blk_ai * BD_Y;
	gpu_size_t blk_aj_idx = blk_aj * BD_Y;
	// -------------------
	
	// Skip symmetric antenna terms at the block level
	if( blk_ai_idx > blk_aj_idx ) {
		return;
	}
	
	// Thread decomposition
	gpu_size_t f_idx  = blockIdx.x * BD_X + threadIdx.x;
	gpu_size_t ai_idx = blk_ai_idx        + threadIdx.y;
	gpu_size_t aj_idx = blk_aj_idx        + threadIdx.y;
	
	// Initialise the accumulation results to zero
	// TODO: Try putting this in shared mem instead and seeing if can
	//       boost occupancy. Answer: no. 2x slower.
	T cij[BD_Y];
	//__shared__ float s_cij_real[BD_Y][BD_X];
	//__shared__ float s_cij_imag[BD_Y][BD_X];
#pragma unroll
	for( gpu_size_t j=0; j<BD_Y; ++j ) {
		cij[j].x = 0;
		cij[j].y = 0;
		//s_cij_real[j][threadIdx.x] = 0;
		//s_cij_imag[j][threadIdx.x] = 0;
	}
	
	// Iterate over each accumulation
	for( gpu_size_t accum_idx=0; accum_idx<accum_count; ++accum_idx ) {
		// Load global data into registers
		T ai = d_in[f_idx + in_stride1*accum_idx + in_stride2*ai_idx];
		T aj = d_in[f_idx + in_stride1*accum_idx + in_stride2*aj_idx];
		/* TEST to check accuracy of half-float values
		ai.x = __half2float(__float2half_rn(ai.x));
		ai.y = __half2float(__float2half_rn(ai.y));
		aj.x = __half2float(__float2half_rn(aj.x));
		aj.y = __half2float(__float2half_rn(aj.y));
		*/
		// Write complex components to separate parts of shared mem
		__syncthreads(); // Wait for computations using shared mem to complete
		s_aj_real[threadIdx.y][threadIdx.x] = aj.x;
		s_aj_imag[threadIdx.y][threadIdx.x] = aj.y;
		__syncthreads(); // Wait for writes to shared mem to complete
		
		// Iterate over the block of j antennas
#pragma unroll
		for( gpu_size_t j=0; j<BD_Y; ++j ) {
			aj.x = s_aj_real[j][threadIdx.x];
			aj.y = s_aj_imag[j][threadIdx.x];
			// TEST for comparing shared mem to global mem
			//aj = d_in[f_idx + in_stride1*accum_idx +
			//          in_stride2*(blk_aj_idx+j)];
			
			// Conjugate-multiply and accumulate
			// Note: This form produces MAD MAD MAD MUL SUB
			cij[j].x += ai.x*aj.x;
			cij[j].x += ai.y*aj.y;
			cij[j].y += aj.x*ai.y;
			cij[j].y -= ai.x*aj.y;
			/*
			s_cij_real[j][threadIdx.x] += ai.x*aj.x;
			s_cij_real[j][threadIdx.x] += ai.y*aj.y;
			s_cij_imag[j][threadIdx.x] += aj.x*ai.y;
			s_cij_imag[j][threadIdx.x] -= ai.x*aj.y;
			*/
			//cij[j].x += ai.x*aj.x + ai.y*aj.y;
			//cij[j].y += aj.x*ai.y - ai.x*aj.y;
		}
		
	}
	
	// Write the result to global mem
#pragma unroll
	for( gpu_size_t j=0; j<BD_Y; ++j ) {
		aj_idx = blk_aj_idx + j;
		// Avoid symmetric antenna terms
		// Note: This is a little trick to divide by either 1 (single-pol)
		//       or 2 (full-polar) when skipping terms near the diagonal.
		if( ai_idx/(DO_FULL_POLAR+1) <= aj_idx/(DO_FULL_POLAR+1) ) {
			// Note: This is triangular lexicographical indexing, which
			//       excludes the symmetric terms.
			// TODO: The alignment of this indexing may kill coalescence.
			//d_out[ai_idx + aj_idx*(aj_idx+1)/2] = cij;
			// Note: This is standard lexicographical indexing, which includes
			//       unused space for the symmetric terms.
			// Note: We add the result to the existing output data
			T result = d_out[f_idx + ai_idx*out_stride1 + aj_idx*out_stride2];
			result.x += cij[j].x;
			result.y += cij[j].y;
			//result.x += s_cij_real[j][threadIdx.x];
			//result.y += s_cij_imag[j][threadIdx.x];
			d_out[f_idx + ai_idx*out_stride1 + aj_idx*out_stride2] = result;
		}
	}
	
}


// ************************************
// TEST VERSION
// ************************************
// This version uses half_floats to store a complex value in 4 bytes
// Everything should work exactly the same except that the input and output data
//   are only 4 bytes (ushort2).
template<bool DO_FULL_POLAR, gpu_size_t BD_X, gpu_size_t BD_Y>
__global__
void mac_device_kernel_half(const ushort2*   d_in,
                            //const uint* d_in,
                            gpu_size_t in_stride1,
                            gpu_size_t in_stride2,
                            gpu_size_t ant_block_count, // Needed for manual grid decomp
                            gpu_size_t accum_count,
                            ushort2*         d_out,
                            //uint* d_out,
                            gpu_size_t out_stride1,
                            gpu_size_t out_stride2) {
	
	//                         ant   freq
	//__shared__ uint s_aj[BD_Y][BD_X];
	__shared__ float s_aj_real[BD_Y][BD_X];
	__shared__ float s_aj_imag[BD_Y][BD_X];
	
	// Block decomposition
	// -------------------
	// Note: We manually decompose the 2nd grid dim into two dims
	gpu_size_t blk_ai = blockIdx.y % ant_block_count;
	gpu_size_t blk_aj = blockIdx.y / ant_block_count;
	
	// Apply diagonal reordering to avoid partition camping
	gpu_size_t old_blk_ai = blk_ai;
	blk_ai = blk_aj;
	blk_aj = (blk_aj + old_blk_ai) % ant_block_count;
	
	// Calculate the corresponding sample indices
	gpu_size_t blk_ai_idx = blk_ai * BD_Y;
	gpu_size_t blk_aj_idx = blk_aj * BD_Y;
	// -------------------
	
	// Skip symmetric antenna terms at the block level
	if( blk_ai_idx > blk_aj_idx ) {
		return;
	}
	
	// Thread decomposition
	gpu_size_t f_idx  = blockIdx.x * BD_X + threadIdx.x;
	gpu_size_t ai_idx = blk_ai_idx        + threadIdx.y;
	gpu_size_t aj_idx = blk_aj_idx        + threadIdx.y;
	
	// Initialise the accumulation results to zero
	// TODO: Try putting this in shared mem instead and seeing if can boost occupancy
	float2 cij[BD_Y];
#pragma unroll
	for( gpu_size_t j=0; j<BD_Y; ++j ) {
		cij[j].x = 0;
		cij[j].y = 0;
	}
	
	// Iterate over each accumulation
	for( gpu_size_t accum_idx=0; accum_idx<accum_count; ++accum_idx ) {
		// Load global data into registers
		ushort2 ai = d_in[f_idx + in_stride1*accum_idx + in_stride2*ai_idx];
		ushort2 aj = d_in[f_idx + in_stride1*accum_idx + in_stride2*aj_idx];
		/*ai.x = 1;
		ai.y = 2;
		aj.x = 3;
		aj.y = 4;
		*/
		// Unpack the antenna values
		float2 aif, ajf;
		/*aif.x = __half2float( ((ushort*)&ai)[0] ); // (ai.x);
		aif.y = __half2float( ((ushort*)&ai)[1] ); // (ai.y);
		ajf.x = __half2float( ((ushort*)&aj)[0] ); // (ai.x);
		ajf.y = __half2float( ((ushort*)&aj)[1] ); // (ai.y);
		*/
		aif.x = ai.x;
		aif.y = ai.y;
		ajf.x = aj.x;
		ajf.y = aj.y;
		/*aif.x = __half2float(ai.x);
		aif.y = __half2float(ai.y);
		ajf.x = __half2float(aj.x);
		ajf.y = __half2float(aj.y);
		*/
		// Write complex components to separate parts of shared mem
		__syncthreads(); // Wait for computations using shared mem to complete
		//s_aj[threadIdx.y][threadIdx.x] = aj;
		s_aj_real[threadIdx.y][threadIdx.x] = ajf.x;
		s_aj_imag[threadIdx.y][threadIdx.x] = ajf.y;
		__syncthreads(); // Wait for writes to shared mem to complete
		
		// Unpack the i antenna
		//float2 aif;
		//aif.x = __half2float( ((ushort*)&ai)[0] ); // (ai.x);
		//aif.y = __half2float( ((ushort*)&ai)[1] ); // (ai.y);
		
		// Iterate over the block of j antennas
#pragma unroll
		for( gpu_size_t j=0; j<BD_Y; ++j ) {
			//aj = s_aj[j][threadIdx.x];
			ajf.x = s_aj_real[j][threadIdx.x];
			ajf.y = s_aj_imag[j][threadIdx.x];
			/*
			// Unpack the j antenna
			float2 ajf;
			ajf.x = __half2float( ((ushort*)&aj)[0] ); // (ai.x);
			ajf.y = __half2float( ((ushort*)&aj)[1] ); // (ai.y);
			*/
			// Conjugate-multiply and accumulate
			// Note: This form produces MAD MAD MAD MUL SUB
			cij[j].x += aif.x*ajf.x;
			cij[j].x += aif.y*ajf.y;
			cij[j].y += ajf.x*aif.y;
			cij[j].y -= aif.x*ajf.y; // Throughput = 1.6
			/*
			uchar bx0, by0, bx1, by1;
			uchar ax0, ay0, ax1, ay1;
			uint  ix = ax0 | (ax1 << 12);
			uint  iy = ay0 | (ay1 << 12);
			uint  jx = bx0 | (bx1 << 12);
			uint  jy = by0 | (by1 << 12);
			uint x;
			c += __umul24(ax, bx);
			c += __umul24(ay, by);
			c += __umul24(ay, bx);
			c -= __umul24(ax, by); // Throughput = 1.9375
			*/
		}
		
	}
	
	// Write the result to global mem
#pragma unroll
	for( gpu_size_t j=0; j<BD_Y; ++j ) {
		aj_idx = blk_aj_idx + j;
		// Avoid symmetric antenna terms
		// Note: This is a little trick to divide by either 1 (single-pol)
		//       or 2 (full-polar) when skipping terms near the diagonal.
		if( ai_idx/(DO_FULL_POLAR+1) <= aj_idx/(DO_FULL_POLAR+1) ) {
			// Note: This is triangular lexicographical indexing, which
			//       excludes the symmetric terms.
			// TODO: The alignment of this indexing may kill coalescence.
			//d_out[ai_idx + aj_idx*(aj_idx+1)/2] = cij;
			// Note: This is standard lexicographical indexing, which includes
			//       unused space for the symmetric terms.
			// Note: We add the result to the existing output data
			ushort2 r = d_out[f_idx + ai_idx*out_stride1 + aj_idx*out_stride2];
			// Unpack halfs to floats
			//ushort* u = (ushort*)&r;
			float2 rf;
			//rf.x = __half2float( u[0] ); // (r.x);
			//rf.y = __half2float( u[1] ); // (r.y);
			rf.x = __half2float(r.x);
			rf.y = __half2float(r.y);
			// Accumulate
			rf.x += cij[j].x;
			rf.y += cij[j].y;
			// Re-pack halfs to floats
			//u[0] = __float2half_rn(rf.x);
			//u[1] = __float2half_rn(rf.y);
			r.x = __float2half_rn(rf.x);
			r.y = __float2half_rn(rf.y);
			// Write to device mem
			d_out[f_idx + ai_idx*out_stride1 + aj_idx*out_stride2] = r;
		}
	}
	
}
// ************************************

template<class T>
void mac_device(const T* d_in, size_t in_stride1, size_t in_stride2,
                size_t ant_count,
                size_t freq_count,
                size_t accum_count,
                T*     d_out, size_t out_stride1, size_t out_stride2,
                bool   do_full_polar = true,
                cudaStream_t stream=0) {
	// Block dimensions
	// Note: These can be tuned, but BD_X should be a multiple of 16
	enum { BD_X = 128,     // frequency
	       BD_Y = 2 };    // antenna TODO: 8 is best
	
#ifdef _DEBUG
	cudaError_t cuda_error;
#endif
	
	dim3 block(BD_X, BD_Y);
	// TODO: This assumes even divisions!
	if( ant_count % BD_Y != 0 ) cerr << "*** WARNING: antenna count must be a multiple of " << BD_Y << endl;
	gpu_size_t ant_block_count  = ant_count / BD_Y;
	// Note: We flatten a 3D grid decomposition into 2D to fit CUDA
	if( freq_count % BD_X != 0 ) cerr << "*** WARNING: frequency count must be a multiple of " << BD_X << endl;
	dim3 grid(freq_count/BD_X, ant_block_count * ant_block_count);
	
	// TODO: Remove this when done debugging
	//cerr << "block = " << block.x << ", " << block.y << endl;
	//cerr << "grid = " << grid.x << ", " << grid.y << endl;
	
	if( do_full_polar ) {
		mac_device_kernel/*_half*/<true,BD_X,BD_Y><<<grid, block, 0, stream>>>
			(d_in, in_stride1, in_stride2, ant_block_count, accum_count,
			 d_out, out_stride1, out_stride2);
	}
	else {
		mac_device_kernel/*_half*/<false,BD_X,BD_Y><<<grid, block, 0, stream>>>
			(d_in, in_stride1, in_stride2, ant_block_count, accum_count,
			 d_out, out_stride1, out_stride2);
	}

#ifdef _DEBUG
	cudaStreamSynchronize(stream);
	cuda_error = cudaGetLastError();
	if( cuda_error != cudaSuccess ) {
		throw std::runtime_error(
			std::string("In mac_device kernel: ") +
			cudaGetErrorString(cuda_error));
	}
#endif
}
