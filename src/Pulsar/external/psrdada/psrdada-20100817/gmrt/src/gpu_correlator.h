/*****************************************************************************
 *                                                                           * 
 *  GPU Correlator for GMRT                                                  *  
 *                                                                           *
 *****************************************************************************/

#include "kernel_defs.hpp"
#include "maths_functions.hpp"
#include "device_interface.hpp"
#include "fill_device.hpp"
#include "unpack_device.hpp"
#include "fft_device.hpp"
#include "mac_device.hpp"
#include "stopwatch.hpp"

typedef cudaStream_t   Queue;
typedef float          Real;
typedef float2         Complex;

