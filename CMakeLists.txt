
## ==============================================================================
##
##  CMake project settings
##
## ==============================================================================

## Name of the project handled by CMake
project (LUS)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.8)

## Enforced CMake policy
cmake_policy (VERSION 2.8)

## Project release version (major.minor.patch)
set (LUS_VERSION_MAJOR 0 )
set (LUS_VERSION_MINOR 0 )
set (LUS_VERSION_PATCH 0 )
set (LUS_VERSION "${LUS_VERSION_MAJOR}.${LUS_VERSION_MINOR}.${LUS_VERSION_PATCH}")

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

##____________________________________________________________________
##                                                       CMake modules

find_path (LUS_CMAKE_MODULES
  NAMES         template_FindXX.cmake LUS_CMakeSettings.cmake
  PATHS         ${PROJECT_SOURCE_DIR}
  PATH_SUFFIXES Modules cmake devel_common/cmake
 )

if (LUS_CMAKE_MODULES)
  ## Augment CMAKE_MODULE_PATH variable
  set (CMAKE_MODULE_PATH ${LUS_CMAKE_MODULES} CACHE PATH
    "CMake module path"
    FORCE)
  ## Load custom CMake settings
  include (CMakeSettings)
  include (LUS_CMakeSettings)
endif (LUS_CMAKE_MODULES)

## ==============================================================================
##
##  Build options
##
## ==============================================================================

option (LUS_VERBOSE_CONFIGURE  "Be verbose during configuration?"             NO  )
option (LUS_ENABLE_TESTING     "Enable testing and experimental builds?"      YES )
option (POST_BUILD_CLEAN       "Clean up the build directory after a build?"  NO  )
option (MULTITHREADED_BUILD    "Enable multithreading during build?"          NO  )

## Options affecting the build of individual packages

if (APPLE)
  option (WCSLIB_PYTHON_WRAPPER   "Generate Python wrapper for WCSLIB?"      NO )
else (APPLE)
  option (WCSLIB_PYTHON_WRAPPER   "Generate Python wrapper for WCSLIB?"      YES )
endif (APPLE)

option (PYCRTOOLS_ENABLE_TESTING         "Enable testing for the PyCRTools?"                         YES )
option (PYCRTOOLS_WITH_COMPILER_WARNINGS "Compile PyCRTools with compiler warnings enabled?"         YES )
option (PYCRTOOLS_WITH_CPU_OPTIMIZATIONS "Compile PyCRTools with optimization for CPU architecture?" NO )
option (PYCRTOOLS_WITH_DEBUG_SYMBOLS     "Compile PyCRTools with debug symbols?"                     YES )
option (PYCRTOOLS_WITH_OPTIMIZATIONS     "Compile PyCRTools with optimization flags turned on?"      YES )
option (PYCRTOOLS_WITH_RUNTIME_WARNINGS  "Compile PyCRTools with runtime warnings enabled?"          YES )
option (PYCRTOOLS_WITH_AERA              "Compile PyCRTools with AERA data support?"                 NO )
option (PYCRTOOLS_WITH_CASACORE          "Compile PyCRTools with CASACore support?"                  YES )
option (PYCRTOOLS_WITH_DAL1               "Compile PyCRTools with DAL1 support?"                       YES )
option (PYCRTOOLS_WITH_FFTW              "Compile PyCRTools with FFTW support?"                      YES )
option (PYCRTOOLS_WITH_GSL               "Compile PyCRTools with GSL support?"                       YES )
option (PYCRTOOLS_WITH_NUMPY             "Compile PyCRTools with Numpy support?"                     YES )

## Handle options

if (LUS_ENABLE_TESTING)
  include (CTest)
  enable_testing()
endif (LUS_ENABLE_TESTING)

## Create common options file

configure_file (
  ${PROJECT_SOURCE_DIR}/CMakeOptions.cmake
  ${PROJECT_BINARY_DIR}/CMakeOptions.cmake
  )

## ==============================================================================
##
##  Basic system inspection:
##
##  - Look for essential system tools, which are required; some of the targets
##    only will be activated if the necessary tool is available.
##
## ==============================================================================

find_package (UnixCommands)

if (HOSTNAME_EXECUTABLE)
  exec_program (${HOSTNAME_EXECUTABLE} ARGS OUTPUT_VARIABLE CMAKE_SYSTEM_HOSTNAME)
endif (HOSTNAME_EXECUTABLE)

## Got through the basic list of system tools and libraries; for each package
## take into account the ENABLE_VERBOSE_CONFIGURE option and ensure a standard
## version of the status variable is being define (<PACKAGE>_FOUND)

foreach (varSystemPackage
    Armadillo
    BISON
    Boost
    CASACORE
    CFITSIO
    CUDA
    CppUnit
    DAL1
    DAL
    Doxygen
    FFTW3
    FLEX
    GSL
    Git
    HDF5
    IPython
    ITPP
    LAPACK
    NumPy
    NumUtil
    OpenGL
    OpenMP
    PIL
    PLplot
    Perl
    Python
    PyFITS
    ROOT
    Sphinx
    Subversion
    SIP
    SWIG
    SZIP
    WCSLIB
    Wget
    )

  lusFindModules (${varSystemPackage} ${LUS_VERBOSE_CONFIGURE})
  
endforeach (varSystemPackage)

## ==============================================================================
##
##  Project sub-directories
##
## ==============================================================================

add_subdirectory (devel_common)
add_subdirectory (doc)
add_subdirectory (external)
add_subdirectory (src)

##____________________________________________________________________
## Enable testing

if (LUS_ENABLE_TESTING)
  include (LUS_TestTargets)
endif (LUS_ENABLE_TESTING)

##____________________________________________________________________
## Prepare release directory (CMAKE_INSTALL_PREFIX)

if (IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib64)
  if (LUS_VERBOSE_CONFIGURE)
    message (STATUS "[LUS] Found installation prefix directory.")
  endif (LUS_VERBOSE_CONFIGURE)
else (IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib64)
  if (UNIX)
    execute_process (
      COMMAND ${CMAKE_COMMAND} -E make_directory lib
      COMMAND ${CMAKE_COMMAND} -E create_symlink lib lib64
      WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
      )
  endif (UNIX)
endif (IS_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib64)

## ==============================================================================
##
##  Configuration summary
##
## ==============================================================================

message (STATUS "------------------------------------------------------------")
message (STATUS "[LUS] Configuration summary.")
message (STATUS "------------------------------------------------------------")
message (STATUS " System configuration:"                                      )
message (STATUS " .. Hostname ................... = ${CMAKE_SYSTEM_HOSTNAME}" )
message (STATUS " .. Processor type ............. = ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS " .. CMake executable ........... = ${CMAKE_COMMAND}"         )
message (STATUS " .. CMake version .............. = ${CMAKE_VERSION}"         )
message (STATUS " .. System name ................ = ${CMAKE_SYSTEM}"          )
message (STATUS " .. C++ compiler ............... = ${CMAKE_CXX_COMPILER}"    )
message (STATUS " .. C compiler ................. = ${CMAKE_C_COMPILER}"      )
message (STATUS " .. size(void*) ................ = ${CMAKE_SIZEOF_VOID_P}"   )
message (STATUS " Required external packages:"                                )
message (STATUS " .. Bison ...................... = ${BISON_FOUND}"           )
message (STATUS " .. Boost++ .................... = ${BOOST_FOUND}"           )
message (STATUS " .. CFITSIO .................... = ${CFITSIO_FOUND}"         )
message (STATUS " .. CUDA ....................... = ${CUDA_FOUND}"            )
message (STATUS " .. casacore ................... = ${CASACORE_FOUND}"        )
message (STATUS " .. Doxygen .................... = ${DOXYGEN_FOUND}"         )
message (STATUS " .. Flex ....................... = ${FLEX_FOUND}"            )
message (STATUS " .. GNU Scientific Library (GSL) = ${GSL_FOUND}"             )
message (STATUS " .. Git ........................ = ${GIT_FOUND}"             )
message (STATUS " .. FFTW3 ...................... = ${FFTW3_FOUND}"           )
message (STATUS " .. HDF5 Library ............... = ${HDF5_FOUND}"            )
message (STATUS " .. IPython .................... = ${IPYTHON_FOUND}"         )
message (STATUS " .. IT++ Library ............... = ${ITPP_FOUND}"            )
message (STATUS " .. OpenGL ..................... = ${OPENGL_FOUND}"          )
message (STATUS " .. OpenMP ..................... = ${OPENMP_FOUND}"          )
message (STATUS " .. Perl ....................... = ${PERL_FOUND}"            )
message (STATUS " .. PyFITS...................... = ${PYFITS_FOUND}"          )
message (STATUS " .. ROOT ....................... = ${ROOT_FOUND}"            )
message (STATUS " .. SIP ........................ = ${SIP_FOUND}"             )
message (STATUS " .. SWIG ....................... = ${SWIG_FOUND}"            )
message (STATUS " .. Subversion ................. = ${SUBVERSION_FOUND}"      )
message (STATUS " .. SZIP compression library ... = ${SZIP_FOUND}"            )
message (STATUS " .. WCSLIB ..................... = ${WCSLIB_FOUND}"          )
message (STATUS " .. Wget ....................... = ${WGET_FOUND}"            )
message (STATUS " LOFAR User software configuration:"                         )
message (STATUS " .. LUS_SOURCE_DIR ............. = ${LUS_SOURCE_DIR}"        )
message (STATUS " .. LUS_BINARY_DIR ............. = ${LUS_BINARY_DIR}"        )
message (STATUS " .. CMAKE_INSTALL_PREFIX ....... = ${CMAKE_INSTALL_PREFIX}"  )
message (STATUS " .. LUS_CMAKE_MODULES .......... = ${LUS_CMAKE_MODULES}"     )
message (STATUS " .. LUS_PYTHONPATH ............. = ${USG_PYTHONPATH}"        )
message (STATUS " .. LUS_DATA_DIR ............... = ${LUS_DATA_DIR}"          )
message (STATUS "------------------------------------------------------------")
