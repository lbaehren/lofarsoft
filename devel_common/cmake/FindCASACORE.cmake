##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Check for the casacore distribution
##
## Variables assigned:
##  CASACORE_INCLUDES = Path to the casacore header files
##  CASA_LIBRARIES    = Libraries of the casacore modules
##  HAVE_CASACORE     = Do we have both headers and libraries of casacore?
##

set (include_locations
  /casacore
  /opt/casacore
  /opt/casacore/include
  /sw/share/casacore
  ./../casacore
  ./../external/casacore
  ./../../casacore
  ./../../external/casacore
)

set (lib_locations
  /usr/lib
  /usr/local/lib
  /opt/lib
  /opt/casacore/lib
  /sw/lib
  ./../lib
  ./../release/lib
  ./../../lib
  ./../../release/lib
)

set (casacore_libs
  casa
  tables
  mirlib
  scimath
  measures
  fits
  coordinates
  components
  lattices
  ms
  images
  msfits
  msvis
  )

## -----------------------------------------------------------------------------
## Required external packages

find_library (libg2c g2c f2c PATHS ${lib_locations})
find_library (libwcs wcs PATHS ${lib_locations})
find_library (libcfitsio cfitsio PATHS ${lib_locations})

if (NOT libwcs)
  message (SEND_ERROR "Missing WCSLIB required for casacore!")
endif (NOT libwcs)

if (NOT libcfitsio)
  message (SEND_ERROR "Missing CFITSIO required for casacore!")
endif (NOT libcfitsio)

## -----------------------------------------------------------------------------
## Check for the header files.
##
## Depending on whether the header files are part of a complete casacore
## installation or have remained at their original location in the source
## directories, we need to check for different variants to the include paths.

set (CASACORE_INCLUDES "")

## [1] <casa/Arrays.h>

find_path (CASACORE_casa Arrays.h
  PATHS ${include_locations}
  PATH_SUFFIXES casa/casa
)

if (CASACORE_casa)
  string (REGEX REPLACE casa/casa casa tmp ${CASACORE_casa})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_casa)

## [2] <tables/Tables.h>

find_path (CASACORE_tables Tables.h
  PATHS ${include_locations}
  PATH_SUFFIXES tables/tables
)

if (CASACORE_tables)
  string (REGEX REPLACE tables/tables tables tmp ${CASACORE_tables})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_tables)

## [3] <miriad.h>

find_path (CASACORE_mirlib miriad.h
  PATHS ${include_locations}
  PATH_SUFFIXES mirlib
)

if (CASACORE_mirlib)
  list (APPEND CASACORE_INCLUDES ${CASACORE_mirlib})
endif (CASACORE_mirlib)

## [4] <scimath/Fitting.h>

find_path (CASACORE_scimath Fitting.h
  PATHS ${include_locations}
  PATH_SUFFIXES scimath/scimath
)

if (CASACORE_scimath)
  string (REGEX REPLACE scimath/scimath scimath tmp ${CASACORE_scimath})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_scimath)

## [5] <measures/Measures.h>

find_path (CASACORE_measures Measures.h
  PATHS ${include_locations}
  PATH_SUFFIXES measures/measures
)

if (CASACORE_measures)
  string (REGEX REPLACE measures/measures measures tmp ${CASACORE_measures})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_measures)

## [6] <fits/FITS.h>

find_path (CASACORE_fits FITS.h
  PATHS ${include_locations}
  PATH_SUFFIXES fits/fits
)

if (CASACORE_fits)
  string (REGEX REPLACE fits/fits fits tmp ${CASACORE_fits})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_fits)

## [7] <coordinates/Coordinates.h>

find_path (CASACORE_coordinates Coordinates.h
  PATHS ${include_locations}
  PATH_SUFFIXES coordinates/coordinates
)

if (CASACORE_coordinates)
  string (REGEX REPLACE coordinates/coordinates coordinates tmp ${CASACORE_coordinates})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_coordinates)

## [8] <components/ComponentModels.h>

find_path (CASACORE_components ComponentModels.h
  PATHS ${include_locations}
  PATH_SUFFIXES components/components
)

if (CASACORE_components)
  string (REGEX REPLACE components/components components tmp ${CASACORE_components})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_components)

## [9] <lattices/Lattices.h>

find_path (CASACORE_lattices Lattices.h
  PATHS ${include_locations}
  PATH_SUFFIXES lattices/lattices
)

if (CASACORE_lattices)
  string (REGEX REPLACE lattices/lattices lattices tmp ${CASACORE_lattices})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_lattices)

## [10] <ms/MeasurementSets.h>

find_path (CASACORE_ms MeasurementSets.h
  PATHS ${include_locations}
  PATH_SUFFIXES ms/ms
)

if (CASACORE_ms)
  string (REGEX REPLACE ms/ms ms tmp ${CASACORE_ms})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_ms)

## [11] <images/Images.h>

find_path (CASACORE_images Images.h
  PATHS ${include_locations}
  PATH_SUFFIXES images/images
)

if (CASACORE_images)
  string (REGEX REPLACE images/images images tmp ${CASACORE_images})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_images)

## [12] <msfits/MSFits.h>

find_path (CASACORE_msfits MSFits.h
  PATHS ${include_locations}
  PATH_SUFFIXES msfits/msfits
)

if (CASACORE_msfits)
  string (REGEX REPLACE msfits/msfits msfits tmp ${CASACORE_msfits})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_msfits)

## [13] <msvis/MSVis.h>

find_path (CASACORE_msvis MSVis.h
  PATHS ${include_locations}
  PATH_SUFFIXES msvis/msvis
)

if (CASACORE_msvis)
  string (REGEX REPLACE msvis/msvis msvis tmp ${CASACORE_msvis})
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_msvis)

## -----------------------------------------------------------------------------
## Check for the library

## Locate the libraries themselves; depending on whether they have been build
## using SCons or CMake, the names of the library files will be slightly 
## different.

foreach (casacore_lib ${casacore_libs})
  find_library (CASACORE_lib${casacore_lib} ${casacore_lib} casa_${casacore_lib}
    PATHS ${lib_locations}
  )
endforeach (casacore_lib)

## Dependency of the packages in casacore

# casa        : --
# tables      : casa
# mirlib      : casa
# scimath     : casa
# measures    : tables scimath
# fits        : measures
# coordinates : fits
# components  : coordinates
# lattices    : tables scimath
# ms          : measures
# images      : components mirlib
# msfits      : ms fits
# msvis       : ms

if (CASACORE_libcasa)
  list (APPEND CASACORE_LIBRARIES ${CASACORE_libcasa})
endif (CASACORE_libcasa)

if (CASACORE_libtables)
  list (APPEND CASACORE_LIBRARIES ${CASACORE_libtables})
endif (CASACORE_libtables)

if (CASACORE_libmir)
  list (APPEND CASACORE_LIBRARIES ${CASACORE_libmir})
endif (CASACORE_libmir)

if (CASACORE_libscimath)
  list (APPEND CASACORE_LIBRARIES ${CASACORE_libscimath})
endif (CASACORE_libscimath)

if (CASACORE_libmeasures)
  list (APPEND CASACORE_LIBRARIES ${CASACORE_libmeasures})
endif (CASACORE_libmeasures)

if (CASACORE_libfits)
  list (APPEND CASACORE_LIBRARIES ${CASACORE_libfits})
endif (CASACORE_libfits)

if (CASACORE_libcoordinates)
  list (APPEND CASACORE_LIBRARIES ${CASACORE_libcoordinates})
endif (CASACORE_libcoordinates)

if (CASACORE_libcomponents)
  list (APPEND CASACORE_LIBRARIES ${CASACORE_libcomponents})
endif (CASACORE_libcomponents)

if (CASACORE_liblattices)
  list (APPEND CASACORE_LIBRARIES ${CASACORE_liblattices})
endif (CASACORE_liblattices)

## -----------------------------------------------------------------------------
## If detection successful, register package as found

if (CASACORE_INCLUDES AND CASA_LIBRARIES)
  set (HAVE_CASACORE TRUE)
  string (REGEX REPLACE lib/libcasa.a lib CASA_LIBRARIES_DIR ${CASA_libcasa})
else (CASACORE_INCLUDES AND CASA_LIBRARIES)
  if (NOT CASA_FIND_QUIETLY)
    if (NOT CASACORE_INCLUDES)
      message (STATUS "Unable to find CASACORE header files!")
    endif (NOT CASACORE_INCLUDES)
    if (NOT CASA_LIBRARIES)
      message (STATUS "Unable to find CASACORE library files!")
    endif (NOT CASA_LIBRARIES)
  endif (NOT CASA_FIND_QUIETLY)
endif (CASACORE_INCLUDES AND CASA_LIBRARIES)

## ------------------------------------------------------------------------------
## Final assembly of the provided variables and flags; once this is done, we
## provide some extended feedback.

if (HAVE_CASACORE)
  add_definitions (
    -DAIPS_${AIPS_ARCH}
    -DAIPS_${AIPS_ENDIAN}_ENDIAN
    -DNATIVE_EXCP
    -DAIPS_STDLIB
    -DAIPS_AUTO_STL
    -DAIPS_NO_LEA_MALLOC
    -D_GLIBCPP_DEPRECATED
    -DSIGNBIT 
    -DAIPS_NO_TEMPLATE_SRC
    -DAIPS_${AIPS_ARCH})
  set (CASA_CXX_FLAGS "-fPIC -pipe -Wall -Wno-non-template-friend -Woverloaded-virtual -Wno-comment -fexceptions -Wcast-align")
  set (CASA_CXX_LFLAGS "${CASA_LIBRARIES_DIR}/version.o ${CASA_LIBRARIES}")
  if (NOT CASA_FIND_QUIETLY)
    message (STATUS "Found components for CASACORE.")
    message (STATUS "CASACORE_LIBRARIES_DIR . : ${CASA_LIBRARIES_DIR}")
    message (STATUS "CASACORE_LIBRARIES ..... : ${CASA_LIBRARIES}")
    message (STATUS "CASACORE header files .. : ${CASACORE_INCLUDES}")
    message (STATUS "CASACORE compile command : ${CASA_CXX_FLAGS}")
    message (STATUS "CASACORE linker command  : ${CASA_CXX_LFLAGS}")
  endif (NOT CASA_FIND_QUIETLY)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CASA_CXX_FLAGS}")
else (HAVE_CASACORE)
  if (CASA_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find CASA")
  endif (CASA_FIND_REQUIRED)
endif (HAVE_CASACORE)

## ------------------------------------------------------------------------------
## Mark as advanced: clean up not to include all the temporary variables into
## configuration settings

mark_as_advanced (
  CASACORE_libcasa
  CASACORE_libcoordinates
  CASACORE_libimages
  CASACORE_libmeasures
  CASACORE_libscimath
  CASACORE_libscimath_f
  CASACORE_libtables
  CASACORE_libcalibration
  CASACORE_libcomponents
  CASACORE_libfits
  CASACORE_libgraphics
  CASACORE_liblattices
  CASACORE_libms
  CASACORE_libmsfits
  CASACORE_libmsvis
  )
