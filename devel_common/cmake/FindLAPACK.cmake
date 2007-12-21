##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Check for the presence of LAPACK
##
## The following variables are set when LAPACK is found:
##  HAVE_LAPACK     - Set to true, if all components of LAPACK have been found.
##  LAPACK_LIBRARIES - Link these to use LAPACK
##  LAPACK_INCLUDES  - Location of the LAPACK header files

set (lib_locations
 /usr/lib
 /usr/local/lib
 /sw/lib
 /Developer/SDKs/MacOSX10.4u.sdk/usr/lib
)

set (include_locations
  /usr/include
  /usr/local/include
  /sw/include
  /Developer/SDKs/MacOSX10.4u.sdk/usr/include
  /Developer/SDKs/MacOSX10.4u.sdk/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/Headers
)


## -----------------------------------------------------------------------------
## Check for the header files

find_path (LAPACK_INCLUDES clapack.h
  PATHS ${include_locations}
)

## -----------------------------------------------------------------------------
## Check for the library files (-llapack -lblas -lcblas -latlas)

set (libs
  lapack
  cblas
  blas
  atlas
  )

set (LAPACK_LIBRARIES "")

foreach (lib ${libs})
  ## try to locate the library
  find_library (LAPACK_${lib} ${lib} ${lib}_LINUX
    PATHS ${lib_locations}
    PATH_SUFFIXES lapack
    NO_DEFAULT_PATH
    )
  ## check if location was successful
  if (LAPACK_${lib})
    list (APPEND LAPACK_LIBRARIES ${LAPACK_${lib}})
  endif (LAPACK_${lib})
endforeach (lib)

## -----------------------------------------------------------------------------
## Actions taken when all components have been found

IF (LAPACK_LIBRARIES)
  SET (HAVE_LAPACK TRUE)
ELSE (LAPACK_LIBRARIES)
  IF (NOT LAPACK_FIND_QUIETLY)
    IF (NOT LAPACK_LIBRARIES)
      MESSAGE (STATUS "Unable to find LAPACK library files!")
    ENDIF (NOT LAPACK_LIBRARIES)
  ENDIF (NOT LAPACK_FIND_QUIETLY)
ENDIF (LAPACK_LIBRARIES)

IF (HAVE_LAPACK)
  IF (NOT LAPACK_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for LAPACK")
    MESSAGE (STATUS "LAPACK_INCLUDES  = ${LAPACK_INCLUDES}")
    MESSAGE (STATUS "LAPACK_LIBRARIES = ${LAPACK_LIBRARIES}")
  ENDIF (NOT LAPACK_FIND_QUIETLY)
ELSE (HAVE_LAPACK)
  IF (LAPACK_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could not find LAPACK!")
  ENDIF (LAPACK_FIND_REQUIRED)
ENDIF (HAVE_LAPACK)

## -----------------------------------------------------------------------------
## Mark as advanced...

MARK_AS_ADVANCED (
  LAPACK_atlas
  LAPACK_blas
  LAPACK_cblas
  LAPACK_lapack
  )
