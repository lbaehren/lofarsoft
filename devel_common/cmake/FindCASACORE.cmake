##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 489 2007-08-01 11:40:38Z baehren                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Check for the casacore distribution
##
## Variables assigned:
##  HAVE_CASACORE      = Do we have both headers and libraries of casacore?
##  CASACORE_INCLUDES  = Path to the casacore header files
##  CASACORE_LIBRARIES = Libraries of the casacore modules
##  CASACORE_COMPILE_FLAGS = 
##
## __TODO__
##
##  - If both CASA and casacore are present on a system, this scripts tends to
##    find the CASA libraries instead of the casacore libraries; we need to come
##    up with a method to separate both installations from each other.
##

set (include_locations
  ./../casacore
  ./../external/casacore
  ./../../casacore
  ./../../external/casacore
  /usr/include/casacore
  /usr/local/include/casacore
  /sw/share/casacore
  /sw/share/casacore/stage/include
  /sw/include/casacore
)

set (lib_locations
  ./../lib
  ./../release/lib
  ./../../lib
  ./../../release/lib
  /usr/lib
  /usr/local/lib
  /opt/lib
  /opt/casacore/lib
  /sw/lib
)

set (casacore_modules
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
## Check for system header files

if (NOT CASACORE_FIND_QUIETLY)
  message (STATUS "[FindCASACORE] Check for system header files ...")
endif (NOT CASACORE_FIND_QUIETLY)

include (CheckIncludeFiles)

check_include_files (assert.h HAVE_ASSERT_H)
check_include_files (ctype.h HAVE_CTYPE_H)
check_include_files (fcntl.h HAVE_FCNTL_H)
check_include_files (stdlib.h HAVE_STDLIB_H)
check_include_files (stdio.h HAVE_STDIO_H)
check_include_files (string.h HAVE_STRING_H)
check_include_files (unistd.h HAVE_UNISTD_H)

## -----------------------------------------------------------------------------
## Required external packages

find_library (libcfitsio cfitsio PATHS ${lib_locations})
find_library (libm m PATHS ${lib_locations})
find_library (libg2c g2c f2c PATHS ${lib_locations})
find_library (libwcs wcs PATHS ${lib_locations})

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

if (NOT CASACORE_FIND_QUIETLY)
  message (STATUS "[FindCASACORE] Check for the header files ...")
endif (NOT CASACORE_FIND_QUIETLY)

set (CASACORE_INCLUDES "")

## [1] <casa/Arrays.h>

find_path (CASACORE_casa Arrays.h
  PATHS ${include_locations}
  PATH_SUFFIXES casa/casa
)

if (CASACORE_casa)
  string (REGEX REPLACE "casa/casa" "casa" tmp ${CASACORE_casa})
  get_filename_component (tmp ${tmp} ABSOLUTE)
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_casa)

## [2] <tables/Tables.h>

find_path (CASACORE_tables Tables.h
  PATHS ${include_locations}
  PATH_SUFFIXES tables/tables
)

if (CASACORE_tables)
  string (REGEX REPLACE tables/tables tables tmp ${CASACORE_tables})
  get_filename_component (tmp ${tmp} ABSOLUTE)
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
  get_filename_component (tmp ${tmp} ABSOLUTE)
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_scimath)

## [5] <measures/Measures.h>

find_path (CASACORE_measures Measures.h
  PATHS ${include_locations}
  PATH_SUFFIXES measures/measures
)

if (CASACORE_measures)
  string (REGEX REPLACE measures/measures measures tmp ${CASACORE_measures})
  get_filename_component (tmp ${tmp} ABSOLUTE)
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_measures)

## [6] <fits/FITS.h>

find_path (CASACORE_fits FITS.h
  PATHS ${include_locations}
  PATH_SUFFIXES fits/fits
)

if (CASACORE_fits)
  string (REGEX REPLACE fits/fits fits tmp ${CASACORE_fits})
  get_filename_component (tmp ${tmp} ABSOLUTE)
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_fits)

## [7] <coordinates/Coordinates.h>

find_path (CASACORE_coordinates Coordinates.h
  PATHS ${include_locations}
  PATH_SUFFIXES coordinates/coordinates
)

if (CASACORE_coordinates)
  string (REGEX REPLACE coordinates/coordinates coordinates tmp ${CASACORE_coordinates})
  get_filename_component (tmp ${tmp} ABSOLUTE)
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_coordinates)

## [8] <components/ComponentModels.h>

find_path (CASACORE_components ComponentModels.h
  PATHS ${include_locations}
  PATH_SUFFIXES components/components
)

if (CASACORE_components)
  string (REGEX REPLACE components/components components tmp ${CASACORE_components})
  get_filename_component (tmp ${tmp} ABSOLUTE)
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_components)

## [9] <lattices/Lattices.h>

find_path (CASACORE_lattices Lattices.h
  PATHS ${include_locations}
  PATH_SUFFIXES lattices/lattices
)

if (CASACORE_lattices)
  string (REGEX REPLACE lattices/lattices lattices tmp ${CASACORE_lattices})
  get_filename_component (tmp ${tmp} ABSOLUTE)
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_lattices)

## [10] <ms/MeasurementSets.h>

find_path (CASACORE_ms MeasurementSets.h
  PATHS ${include_locations}
  PATH_SUFFIXES ms/ms
)

if (CASACORE_ms)
  string (REGEX REPLACE ms/ms ms tmp ${CASACORE_ms})
  get_filename_component (tmp ${tmp} ABSOLUTE)
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_ms)

## [11] <images/Images.h>

find_path (CASACORE_images Images.h
  PATHS ${include_locations}
  PATH_SUFFIXES images/images
)

if (CASACORE_images)
  string (REGEX REPLACE images/images images tmp ${CASACORE_images})
  get_filename_component (tmp ${tmp} ABSOLUTE)
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_images)

## [12] <msfits/MSFits.h>

find_path (CASACORE_msfits MSFits.h
  PATHS ${include_locations}
  PATH_SUFFIXES msfits/msfits
)

if (CASACORE_msfits)
  string (REGEX REPLACE msfits/msfits msfits tmp ${CASACORE_msfits})
  get_filename_component (tmp ${tmp} ABSOLUTE)
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_msfits)

## [13] <msvis/MSVis.h>

find_path (CASACORE_msvis MSVis.h
  PATHS ${include_locations}
  PATH_SUFFIXES msvis/msvis
)

if (CASACORE_msvis)
  string (REGEX REPLACE msvis/msvis msvis tmp ${CASACORE_msvis})
  get_filename_component (tmp ${tmp} ABSOLUTE)
  list (APPEND CASACORE_INCLUDES ${tmp})
endif (CASACORE_msvis)

## -----------------------------------------------------------------------------
## Check for the library

if (NOT CASACORE_FIND_QUIETLY)
  message (STATUS "[FindCASACORE] Check for the library ...")
endif (NOT CASACORE_FIND_QUIETLY)

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

## Locate the libraries themselves; depending on whether they have been build
## using SCons or CMake, the names of the library files will be slightly 
## different.

foreach (casacore_lib ${casacore_modules})
  ## search for the library
  find_library (CASACORE_lib${casacore_lib} casa_${casacore_lib} ${casacore_lib}
    PATHS ${lib_locations}
    )
  ## if we have found the library, add it to the list
  if (CASACORE_lib${casacore_lib})
    list (APPEND CASACORE_LIBRARIES ${CASACORE_lib${casacore_lib}})
  endif (CASACORE_lib${casacore_lib})
endforeach (casacore_lib)

## -----------------------------------------------------------------------------
## If detection successful, register package as found

if (CASACORE_INCLUDES AND CASACORE_LIBRARIES)
  set (HAVE_CASACORE TRUE)
else (CASACORE_INCLUDES AND CASACORE_LIBRARIES)
  if (NOT CASACORE_FIND_QUIETLY)
    if (NOT CASACORE_INCLUDES)
      message (STATUS "Unable to find CASACORE header files!")
    endif (NOT CASACORE_INCLUDES)
    if (NOT CASACORE_LIBRARIES)
      message (STATUS "Unable to find CASACORE library files!")
    endif (NOT CASACORE_LIBRARIES)
  endif (NOT CASACORE_FIND_QUIETLY)
endif (CASACORE_INCLUDES AND CASACORE_LIBRARIES)

## ------------------------------------------------------------------------------
## Final assembly of the provided variables and flags; once this is done, we
## provide some extended feedback.

## --- Compiler settings -------------------------

IF (UNIX)
  add_definitions (
    -DAIPS_STDLIB
    -DAIPS_AUTO_STL
    -DAIPS_NO_LEA_MALLOC
    )
  set (CASACORE_COMPILE_FLAGS "-DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_NO_LEA_MALLOC")
  IF (APPLE)
    add_definitions (-DAIPS_DARWIN)
    set (CASACORE_COMPILE_FLAGS "${CASACORE_COMPILE_FLAGS} -DAIPS_DARWIN")
  else (APPLE)
    add_definitions (-DAIPS_LINUX)
    set (CASACORE_COMPILE_FLAGS "${CASACORE_COMPILE_FLAGS} -DAIPS_LINUX")
  endif (APPLE)
  ##
  ## Platform test Big/Little Endian ------------------------------------
  ##
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "powerpc")
    add_definitions (-DAIPS_BIG_ENDIAN)
    set (CMAKE_SYSTEM_BIG_ENDIAN 1)
    set (CASACORE_COMPILE_FLAGS "${CASACORE_COMPILE_FLAGS} -DAIPS_BIG_ENDIAN")
  else (CMAKE_SYSTEM_PROCESSOR MATCHES "powerpc")
    add_definitions (-DAIPS_LITTLE_ENDIAN)
    set (CMAKE_SYSTEM_BIG_ENDIAN 0)
  endif (CMAKE_SYSTEM_PROCESSOR MATCHES "powerpc")
  #  TEST_BIG_ENDIAN (CMAKE_SYSTEM_BIG_ENDIAN)
  #  if (CMAKE_SYSTEM_BIG_ENDIAN)
  #    message (STATUS "System is big endian.")
  #    add_definitions (-DAIPS_BIG_ENDIAN)
  #  elseif (CMAKE_SYSTEM_BIG_ENDIAN)
  #    message (STATUS "System is little endian.")
  #    add_definitions (-DAIPS_LITTLE_ENDIAN)
  #  endif (CMAKE_SYSTEM_BIG_ENDIAN)
  ##
  ## Platform test 32/64 bit ------------------------------
  ##
  set (CMAKE_SYSTEM_64BIT 0)
  if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i386)
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i686)
      if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
        set (CMAKE_SYSTEM_64BIT 1)
        add_definitions (-DAIPS_64B)
      endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
    endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i686)
  endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES i386)
  if (APPLE)
    if (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
      set (CMAKE_SYSTEM_64BIT 1)
      add_definitions (-DAIPS_64B)
    endif (NOT CMAKE_SYSTEM_PROCESSOR MATCHES powerpc)
  endif (APPLE)
ENDIF (UNIX)

## ------------------------------------------------------------------------------
## Compatibility settings (should be removed at some point)

set (CASA_INCLUDES ${CASACORE_INCLUDES})
set (CASA_LIBRARIES ${CASACORE_LIBRARIES})
