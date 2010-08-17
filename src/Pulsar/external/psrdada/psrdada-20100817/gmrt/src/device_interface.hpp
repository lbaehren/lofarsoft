
#pragma once

#include <stdexcept>
#include <string>

// Internal functions
namespace detail {
void checkCudaError(cudaError_t error) {
	if( error != cudaSuccess ) {
		throw std::runtime_error(std::string("CUDA mem error: ") +
		                         cudaGetErrorString(error));
	}
}
void checkAsyncCudaError(cudaError_t error, cudaStream_t stream=0) {
#ifdef _DEBUG
	cudaStreamSynchronize(stream);
	if( error != cudaSuccess ) {
		throw std::runtime_error(std::string("Async CUDA mem error: ") +
		                         cudaGetErrorString(error));
	}
#endif
}
} // namespace detail

// Public functions
void setDevice(int d) {
	cudaError_t error = cudaSetDevice(d);
	detail::checkCudaError(error);
}
template<typename T> void createQueue(T& queue) {
	cudaError_t error = cudaStreamCreate(&queue);
	detail::checkCudaError(error);
}
template<typename T> void destroyQueue(const T& queue) {
	cudaStreamDestroy(queue);
}
template<typename T> void mallocHost(T*& h, size_t n) {
	//cout << "Allocating " << n/1048576. << " MB on host..." << endl;
	cudaError_t error = cudaMallocHost((void**)&h, n);
	detail::checkCudaError(error);
	// TEST for checking stream overlapping
	/*h = (T*)malloc(n);
	if( h == 0 ) {
		throw std::runtime_error("malloc failed");
	}
	*/
}
template<typename T> void mallocDevice(T*& d, size_t n) {
	//cout << "Allocating " << n/1048576. << " MB on device..." << endl;
	cudaError_t error = cudaMalloc((void**)&d, n);
	detail::checkCudaError(error);
	// Blank out the memory to aid in debugging
	error = cudaMemset(d, 0, n);
	detail::checkCudaError(error);
}
template<typename T> void freeHost(T* h) { cudaFreeHost(h); }
template<typename T> void freeDevice(T* d) { cudaFree(d); }
template<typename T> void copyHostToDevice(T* d, const T* h, size_t n, cudaStream_t stream=0) {
	cudaError_t error = cudaMemcpyAsync(d, h, n, cudaMemcpyHostToDevice, stream);
	//cudaError_t error = cudaMemcpyAsync(d, h, n, cudaMemcpyHostToDevice, 0);
	detail::checkAsyncCudaError(error, stream);
}
template<typename T> void copyDeviceToHost(T* h, const T* d, size_t n, cudaStream_t stream=0) {
	cudaError_t error = cudaMemcpyAsync(h, d, n, cudaMemcpyDeviceToHost, stream);
	detail::checkAsyncCudaError(error, stream);
}
template<typename T> void synchronize(const T& queue) {
	cudaStreamSynchronize(queue);
}
void synchronize() {
	cudaThreadSynchronize();
}
