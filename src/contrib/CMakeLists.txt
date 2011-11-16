
## Name of the project
project (contrib)

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.5)

## ------------------------------------------------------------------------------
## Options

option (contrib_VERBOSE_CONFIGURE "Verbose output during configuration?"     NO )
option (contrib_ENABLE_TESTING    "Build and enable running test programs?"  YES )

## ------------------------------------------------------------------------------
## Set CMAKE_MODULE_PATH to load custom CMake modules

if (NOT LUS_CMAKE_MODULES)
  find_path (LUS_CMAKE_MODULES template_FindXX.cmake LUS_CMakeSettings.cmake
    PATHS 
    ${PROJECT_SOURCE_DIR}/..
    ${PROJECT_SOURCE_DIR}/../..
    ${PROJECT_SOURCE_DIR}/../../..
    $ENV{LOFARSOFT}
    PATH_SUFFIXES
    cmake
    devel_common/cmake
    )
endif (NOT LUS_CMAKE_MODULES)

if (LUS_CMAKE_MODULES)
  ## Augment CMAKE_MODULE_PATH variable
  set (CMAKE_MODULE_PATH ${LUS_CMAKE_MODULES} CACHE PATH
    "CMake module path"
    FORCE)
  ## Load custom CMake settings
  include (LUS_CMakeSettings)
endif (LUS_CMAKE_MODULES)

## ------------------------------------------------------------------------------
## Locate components of required external packages

if (LUS_CMAKE_MODULES)

if (NOT contrib_VERBOSE_CONFIGURE)
  set (BLITZ_FIND_QUIETLY      YES )
  set (BOOST_FIND_QUIETLY      YES )
  set (CASA_FIND_QUIETLY       YES )
  set (CASACORE_FIND_QUIETLY   YES )
  set (CFITSIO_FIND_QUIETLY    YES )
  set (DAL_FIND_QUIETLY        YES )
  set (FFTW3_FIND_QUIETLY      YES )
  set (GFORTRAN_FIND_QUIETLY   YES )
  set (GSL_FIND_QUIETLY        YES )
  set (HDF5_FIND_QUIETLY       YES )
  set (LAPACK_FIND_QUIETLY     YES )
  set (PGPLOT_FIND_QUIETLY     YES )
  set (PLPLOT_FIND_QUIETLY     YES )
  set (PNG_FIND_QUIETLY        YES )
  set (POPT_FIND_QUIETLY       YES )
  set (PYTHON_FIND_QUIETLY     YES )
  set (Python_FIND_QUIETLY     YES )
  set (PostgreSQL_FIND_QUIETLY YES )
  set (QT4_FIND_QUIETLY        YES )
  set (ROOT_FIND_QUIETLY       YES )
  set (STARTOOLS_FIND_QUIETLY  YES )
  set (WCSLIB_FIND_QUIETLY     YES )
  set (X11_FIND_QUIETLY        YES )
  set (ZLIB_FIND_QUIETLY       YES )
endif (NOT contrib_VERBOSE_CONFIGURE)

## Standard CMake modules

if (contrib_ENABLE_TESTING)
  include (CTest)
  enable_testing()
endif (contrib_ENABLE_TESTING)

find_package (X11)
find_package (ZLIB)

## USG CMake modules 

foreach (varSystemPackage
    Blitz
    Boost
    CASACORE
    CFITSIO
    GFortran
    HDF5
    LAPACK
    PLplot
    WCSLIB
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
  
  if (${varPackage}_INCLUDES)
    include_directories (${${varPackage}_INCLUDES})
  endif (${varPackage}_INCLUDES)
  
endforeach (varSystemPackage)

else (LUS_CMAKE_MODULES)

  message (WARNING "[contrib] Unable to find LUS CMake modules directory!")

endif (LUS_CMAKE_MODULES)


## -----------------------------------------------------------------------------
## Compiler flags

if (HAVE_BLITZ)
   add_definitions (-DHAVE_BLITZ)
endif (HAVE_BLITZ)

if (HAVE_GSL)
   add_definitions (-DHAVE_GSL)
endif (HAVE_GSL)

if (HAVE_HDF5)
   add_definitions (-DHAVE_HDF5)
endif (HAVE_HDF5)

## -----------------------------------------------------------------------------
## Subdirectories

add_subdirectory (data_inspection)
add_subdirectory (apps)

if (CASA_LIBRARIES AND WCSLIB_LIBRARIES)
  add_subdirectory (smosaic)
endif (CASA_LIBRARIES AND WCSLIB_LIBRARIES)

## -----------------------------------------------------------------------------
## Configuration summary

if (contrib_VERBOSE_CONFIGURE)
  message (STATUS "[config] Configuration summary.")
  message (STATUS "Have Blitz++  = ${HAVE_BLITZ}")
  message (STATUS "Have BOOST    = ${HAVE_BOOST}")
  message (STATUS "Have casacore = ${HAVE_CASACORE}")
  message (STATUS "Have CFITSIO  = ${HAVE_CFITSIO}")
  message (STATUS "Have HDF5     = ${HAVE_HDF5}")
  message (STATUS "Have PLPLOT   = ${HAVE_PLPLOT}")
  message (STATUS "Have WCS      = ${HAVE_WCSLIB}")
  message (STATUS "Have ZLIB     = ${HAVE_ZLIB}")
endif (contrib_VERBOSE_CONFIGURE)
