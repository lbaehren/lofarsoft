#
# - Check for the presence of a CUDA enabled GPU
#
# The following variables are set when a CUDA enabled GPU is found:
#
#  CUDA_HAVE_GPU            = Set to true, if a CUDA enabled GPU is found
#

## ------------------------------------------------------------------------------
## include guard

if (NOT CUDA_HAVE_GPU)

    cmake_minimum_required (VERSION 2.8)
    
    if (NOT CUDA_FOUND)
      find_package (cuda)
    endif (NOT CUDA_FOUND)

    if (CUDA_FOUND)
        try_run (RUN_RESULT_VAR COMPILE_RESULT_VAR
            ${CMAKE_BINARY_DIR} 
            ${CMAKE_MODULE_PATH}/TestCUDAGPU.c
            CMAKE_FLAGS 
                -DINCLUDE_DIRECTORIES:STRING=${CUDA_TOOLKIT_INCLUDE}
                -DLINK_LIBRARIES:STRING=${CUDA_CUDART_LIBRARY}
            COMPILE_OUTPUT_VARIABLE COMPILE_OUTPUT_VAR
            RUN_OUTPUT_VARIABLE RUN_OUTPUT_VAR)
        message ("${RUN_OUTPUT_VAR}") # Display number of GPUs found
        # COMPILE_RESULT_VAR is TRUE when compile succeeds
        # RUN_RESULT_VAR is zero when a GPU is found
        if (COMPILE_RESULT_VAR AND NOT RUN_RESULT_VAR)
            set (CUDA_HAVE_GPU TRUE CACHE BOOL "Whether CUDA-capable GPU is present")
        else (COMPILE_RESULT_VAR AND NOT RUN_RESULT_VAR)
            set (CUDA_HAVE_GPU FALSE CACHE BOOL "Whether CUDA-capable GPU is present")
        endif (COMPILE_RESULT_VAR AND NOT RUN_RESULT_VAR)
    endif (CUDA_FOUND)
    
endif (NOT CUDA_HAVE_GPU)

