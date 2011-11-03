
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
set (LUS_VERSION_MAJOR 0   )
set (LUS_VERSION_MINOR 0   )
set (LUS_VERSION_PATCH 0   )
set (LUS_VERSION "${LUS_VERSION_MAJOR}.${LUS_VERSION_MINOR}.${LUS_VERSION_PATCH}")

## ==============================================================================
##
##  CMake configuration settings
##
## ==============================================================================

find_path (LUS_ROOT devel_common/cmake/CMakeSettings.cmake
  PATHS
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/..
  $ENV{LOFARSOFT}
  )

if (LUS_ROOT)
  ## load custom modules
  include (${LUS_ROOT}/devel_common/cmake/CMakeSettings.cmake)
else (LUS_ROOT)
  set (LUS_ROOT ${PROJECT_SOURCE_DIR})
endif (LUS_ROOT)

## ==============================================================================
##
##  Build options
##
## ==============================================================================

option (LUS_VERBOSE_CONFIGURE  "Be verbose during configuration?"             NO )
option (LUS_ENABLE_TESTING     "Enable testing and experimental builds?"      NO )
option (POST_BUILD_CLEAN       "Clean up the build directory after a build?"  NO )
option (MULTITHREADED_BUILD    "Enable multithreading during build?"          NO )
option (DOXYGEN_GENERATE_LATEX "Have Doxygen generate LaTeX output?"          NO )

## Options affecting the build of individual packages

option (ANAAMIKA_ENABLE_SCIPY   "Build SciPy as interactive environment?"  NO  )
option (ANAAMIKA_ENABLE_FBDSM   "Build Fortran-based version of BDSM?"     NO  )
option (ANAAMIKA_ENABLE_PYBDSM  "Build Python-enabled version of BDSM?"    YES )

option (CR_WITH_DAL             "Build and use the Data Access Library?"   YES )
option (CR_WITH_GLISH           "Support for Glish applications?"          NO  )
option (CR_WITH_PYCR            "Support for Python applications?"         NO )
option (CR_WITH_PLOTTING        "Support for generation of plots?"         YES )
option (CR_WITH_GUI             "Build the graphical interface?"           NO  )
option (CR_WITH_STARTOOLS       "Enable using routines from LOPES-Star?"   YES )
option (CR_WITH_CASAPY          "Build Pyline library against CASApy?"     NO  )

option (PYCRTOOLS_ENABLE_TESTING         "Enable testing for the PyCRTools?"                         YES )
option (PYCRTOOLS_WITH_COMPILER_WARNINGS "Compile PyCRTools with compiler warnings enabled?"         YES )
option (PYCRTOOLS_WITH_CPU_OPTIMIZATIONS "Compile PyCRTools with optimization for CPU architecture?" NO )
option (PYCRTOOLS_WITH_DEBUG_SYMBOLS     "Compile PyCRTools with debug symbols?"                     YES )
option (PYCRTOOLS_WITH_OPTIMIZATIONS     "Compile PyCRTools with optimization flags turned on?"      NO )
option (PYCRTOOLS_WITH_RUNTIME_WARNINGS  "Compile PyCRTools with runtime warnings enabled?"          YES )
option (PYCRTOOLS_WITH_AERA              "Compile PyCRTools with AERA data support?"                 NO )
option (PYCRTOOLS_WITH_CASACORE          "Compile PyCRTools with CASACore support?"                  YES )
option (PYCRTOOLS_WITH_DAL               "Compile PyCRTools with DAL support?"                       YES )
option (PYCRTOOLS_WITH_FFTW              "Compile PyCRTools with FFTW support?"                      YES )
option (PYCRTOOLS_WITH_GSL               "Compile PyCRTools with GSL support?"                       YES )
option (PYCRTOOLS_WITH_NUMPY             "Compile PyCRTools with Numpy support?"                     YES )
option (PYCRTOOLS_WITH_OPENMP            "Compile PyCRTools with OpenMP support?"                    NO )

option (DOCUMENTATION_ONLY     "Configure for building documentation only?"  NO  )
option (DAL_WITH_DOCUMENTATION "Generate documentation for the DAL?"         YES )
option (DAL_SHARED_LIBRARY     "Build DAL as shared library?"                NO  )
option (DAL_ENABLE_TESTING     "Build and enable test programs?"             YES )
option (DAL_COMPILER_WARNINGS  "Enable standard set of compiler warnings?"   YES )
option (DAL_PYTHON_BINDINGS    "Create python bindings?"                     YES )
option (DAL_DEBUGGING_MESSAGES "Print debugging information?"                NO  )
option (DAL_VERBOSE_CONFIGURE  "Verbose output during configuration?"        NO  )
option (DAL_WITH_MYSQL         "Build with support for MySQL database?"      NO  )
set (DAL_GIT "git://github.com/nextgen-astrodata/DAL.git" CACHE STRING "Git repository for DAL.")

option (WCSLIB_PYTHON_WRAPPER    "Generate Python wrapper for WCSLIB?"  YES )

option (RM_ENABLE_ITPP       "Enable using IT++ library?"                  NO  )
option (RM_ENABLE_ARMADILLO  "Enable using Armadillo library?"             YES )

## Handle options

if (LUS_ENABLE_TESTING)
  include (CTest)
  enable_testing()
endif (LUS_ENABLE_TESTING)

## Create common options file

configure_file (
  ${PROJECT_SOURCE_DIR}/CMakeOptions.cmake
  ${LUS_BINARY_DIR}/CMakeOptions.cmake
  )

## ==============================================================================
##
##  Basic system inspection:
##
##  - Look for essential system tools, which are required; some of the targets
##    only will be activated if the necessary tool is available.
##
## ==============================================================================

find_program (HOSTNAME_CMD NAMES hostname)
if (HOSTNAME_CMD)
  exec_program (${HOSTNAME_CMD} ARGS OUTPUT_VARIABLE CMAKE_SYSTEM_HOSTNAME)
endif (HOSTNAME_CMD)

## Got through the basic list of system tools and libraries; for each package
## take into account the ENABLE_VERBOSE_CONFIGURE option and ensure a standard
## version of the status variable is being define (<PACKAGE>_FOUND)

foreach (varSystemPackage
    BISON
    Boost
    Blitz
    CASACORE
    CFITSIO
    CUDA
    CppUnit
    Doxygen
    FFTW3
    FLEX
    GSL
    Git
    HDF5
    IPython
    ITPP
    OpenGL
    OpenMP
    PIL
    Perl
    Python
    ROOT
    Sphinx
    Subversion
    SWIG
    SZIP
    WCSLIB
    Wget
    )

  ## Get package name in uppercase
  string (TOUPPER ${varSystemPackage} varPackageUpper)

  ## Set verbosity level
  if (LUS_VERBOSE_CONFIGURE)
    set (${varSystemPackage}_FIND_QUIETLY FALSE )
    set (${varPackageUpper}_FIND_QUIETLY  FALSE )
  else (LUS_VERBOSE_CONFIGURE)  
    set (${varSystemPackage}_FIND_QUIETLY TRUE )
    set (${varPackageUpper}_FIND_QUIETLY  TRUE )
  endif (LUS_VERBOSE_CONFIGURE)

  ## Load CMake module if package not yet located
  if (NOT ${varPackageUpper}_FOUND)
    find_package (${varSystemPackage})
  endif (NOT ${varPackageUpper}_FOUND)

  ## Conformation of returned status variable
  if (HAVE_${varSystemPackage})
    set (HAVE_${varPackageUpper} TRUE)
  endif (HAVE_${varSystemPackage})

  if (HAVE_${varPackageUpper})
    set (${varPackageUpper}_FOUND TRUE)
  endif (HAVE_${varPackageUpper})
  
  if (${varPackageUpper}_FOUND)
    set (${varPackageUpper}_FOUND TRUE)
  endif (${varPackageUpper}_FOUND)
  
endforeach (varSystemPackage)

find_program (bash_executable   bash    PATHS ${bin_locations})
find_program (curl_executable   curl    PATHS ${bin_locations})
find_program (tar_executable    tar     PATHS ${bin_locations})

set (VERSION_CONTROL_FIND_QUIETLY YES )

include (BuildPackage)
include (FindVersionControl)

## ==============================================================================
##
##  Cleaning up
##
## ==============================================================================

add_custom_target (clean-build
  COMMAND rm -rf *~
  COMMAND rm -rf contrib devel_common external
  COMMAND rm -rf anaamika cr dal lasa pulsar smosaic
  COMMAND rm -rf cmake_install.cmake
  COMMAND rm -rf CMakeCache.txt
  COMMAND rm -rf CMakeOptions.cmake
  COMMAND rm -rf CMakeFiles
  WORKING_DIRECTORY ${LUS_BUILD_DIR}
  COMMENT "Cleaning up build directory ..."
)

add_custom_target (clean-doc
  COMMAND rm -f *~
  COMMAND rm -f html/formula*
  COMMAND rm -f html/tab*
  COMMAND rm -f html/search.*
  COMMAND rm -f html/*.png
  COMMAND rm -f html/*.map
  COMMAND rm -f html/*.html
  COMMAND rm -f html/*.md5
  WORKING_DIRECTORY ${LUS_ROOT}/doc
  COMMENT "Cleaning up documentation directory ..."
  )

add_custom_target (clean-release
  COMMAND rm -rf bin
  COMMAND rm -rf etc
  COMMAND rm -rf include
  COMMAND rm -rf info
  COMMAND rm -rf lib lib64
  COMMAND rm -rf plugins
  COMMAND rm -rf share
  WORKING_DIRECTORY ${LUS_ROOT}/release
  COMMENT "Cleaning up release directory ..."
)

add_custom_target (clean-src
  COMMAND rm -rf *~ */*~ */*/*~ */*/*/*~
  WORKING_DIRECTORY ${LUS_ROOT}/src
  COMMENT "Cleaning up build directory ..."
)

## ==============================================================================
##
##  External packages
##
## ==============================================================================

## Packages with build handled through shell script

configure_file (
  ${LUS_ROOT}/devel_common/scripts/build_package.sh.in
  ${LUS_BUILD_DIR}/build_package.sh
  )

set (packages_external
  num_util
  wcslib
  wcstools
  )

if (ROOT_FOUND)
  list (APPEND packages_external startools)
endif (ROOT_FOUND)

## Packages handled directly through CMake

add_subdirectory (${LUS_ROOT}/doc)
add_subdirectory (${LUS_ROOT}/external)
add_subdirectory (${LUS_ROOT}/devel_common)

## Build instructions for packages

foreach (package_name ${packages_external})
  build_package (${package_name} external/${package_name})
endforeach (package_name)

##____________________________________________________________________
##                                        Dependencies of the packages

add_dependencies (casacore hdf5 cfitsio wcslib bison flex)
add_dependencies (hdf5 szip)
add_dependencies (mathgl hdf5 swig num_util gsl)
add_dependencies (matplotlib numpy)
add_dependencies (num_util boost numpy)
add_dependencies (numpy cblas)
add_dependencies (pelican git boost cfitsio)
add_dependencies (plplot numpy swig)
add_dependencies (pyqt sip)
add_dependencies (wcslib boost flex)

if (ROOT_FOUND)
  add_dependencies (startools fftw3)
endif (ROOT_FOUND)

## ==============================================================================
##
##  USG packages
##
## ==============================================================================

##____________________________________________________________________
##                                                            Anaamika

build_package (anaamika src/Anaamika)

add_dependencies (anaamika wcslib cfitsio boost num_util pyfits)

if (ANAAMIKA_ENABLE_SCIPY)
  add_dependencies (anaamika scipy)
endif (ANAAMIKA_ENABLE_SCIPY)

##____________________________________________________________________
##                                           Data Access Library (DAL)

add_subdirectory (src/DAL)

add_custom_target (dal_depends DEPENDS casacore git)
add_dependencies (dal casacore git)

if (DAL_PYTHON_BINDINGS)
  if (TEST_DATASETS)
    add_dependencies (dal_depends num_util test-data)
    add_dependencies (dal num_util test-data)
  else(TEST_DATASETS)
    add_dependencies (dal_depends num_util)
    add_dependencies (dal num_util)
  endif (TEST_DATASETS)
else (DAL_PYTHON_BINDINGS)
  if (TEST_DATASETS)
    add_dependencies (dal_depends test-data)
    add_dependencies (dal test-data)
  endif (TEST_DATASETS)
endif (DAL_PYTHON_BINDINGS)

# add_dependencies (dal dal_depends)

##____________________________________________________________________
##                                                            CR-Tools

build_package (cr src/CR-Tools)

add_dependencies (cr plplot fftw3 gsl)

if (CR_WITH_DAL)
  add_dependencies (cr dal)
endif (CR_WITH_DAL)

if (CR_WITH_GUI)
  add_dependencies (cr mathgl)
endif (CR_WITH_GUI)

if (CR_WITH_AERA)
  add_dependencies(cr aera)
endif (CR_WITH_AERA)

if (ROOT_FOUND AND CR_WITH_STARTOOLS)
  add_dependencies (cr startools)
endif (ROOT_FOUND AND CR_WITH_STARTOOLS)

if (CR_WITH_GUI)
  add_dependencies (cr pyqt)
endif (CR_WITH_GUI)

##____________________________________________________________________
##                                                           PyCRTools

build_package (pycrtools src/PyCRTools)

add_dependencies (pycrtools dal casacore fftw3 gsl tmf num_util)
if (PYCRTOOLS_WITH_AERA)
  add_dependencies (pycrtools aera)
endif (PYCRTOOLS_WITH_AERA)

##____________________________________________________________________
##                                       LASA (LOFAR Air-Shower Array)

build_package (lasa src/LASA)

##____________________________________________________________________
##                                                              Pulsar

build_package (pulsar src/Pulsar)

add_dependencies (pulsar cfitsio wcstools swig)

##____________________________________________________________________
##                                                                  RM

build_package (rm src/RM)

add_dependencies (rm dal)

if (RM_ENABLE_ARMADILLO)
  add_dependencies (rm armadillo)
endif (RM_ENABLE_ARMADILLO)

if (RM_ENABLE_ITPP)
  add_dependencies (rm itpp)
endif (RM_ENABLE_ITPP)

##____________________________________________________________________
##                                                             smosaic

build_package (smosaic src/smosaic)

add_dependencies (smosaic casacore cfitsio wcslib)

##____________________________________________________________________
##                                                             contrib

build_package (contrib src/contrib)

add_dependencies (contrib dal)

## ==============================================================================
##
##  Configuration summary
##
## ==============================================================================

message (STATUS "------------------------------------------------------------")
message (STATUS "[LUS] Configuration summary.")
message (STATUS "------------------------------------------------------------")
message (STATUS " System configuration:"                                      )
message (STATUS " .. Hostname               = ${CMAKE_SYSTEM_HOSTNAME}"       )
message (STATUS " .. Processor type         = ${CMAKE_SYSTEM_PROCESSOR}"      )
message (STATUS " .. CMake executable       = ${CMAKE_COMMAND}"               )
message (STATUS " .. CMake version          = ${CMAKE_VERSION}"               )
message (STATUS " .. System name            = ${CMAKE_SYSTEM}"                )
message (STATUS " .. C++ compiler           = ${CMAKE_CXX_COMPILER}"          )
message (STATUS " .. C compiler             = ${CMAKE_C_COMPILER}"            )
message (STATUS " .. size(void*)            = ${CMAKE_SIZEOF_VOID_P}"         )
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
message (STATUS " .. ROOT ....................... = ${ROOT_FOUND}"            )
message (STATUS " .. SWIG ....................... = ${SWIG_FOUND}"            )
message (STATUS " .. Subversion ................. = ${SUBVERSION_FOUND}"      )
message (STATUS " .. SZIP compression library ... = ${SZIP_FOUND}"            )
message (STATUS " .. WCSLIB ..................... = ${WCSLIB_FOUND}"          )
message (STATUS " .. Wget ....................... = ${WGET_FOUND}"            )
message (STATUS " LOFAR User software configuration:"                         )
message (STATUS " .. LOFARSOFT                    = $ENV{LOFARSOFT}"          )
message (STATUS " .. LUS_ROOT                     = ${LUS_ROOT}"              )
message (STATUS " .. LUS_INSTALL_PREFIX           = ${LUS_INSTALL_PREFIX}"    )
message (STATUS " .. LUS_PYTHONPATH               = ${USG_PYTHONPATH}"        )
message (STATUS " .. LUS_DATA_DIR                 = ${LUS_DATA_DIR}"          )
message (STATUS "------------------------------------------------------------")
