# +-----------------------------------------------------------------------------+
# |   Copyright (C) 2007                                                        |
# |   Lars B"ahren (bahren@astron.nl)                                           |
# |                                                                             |
# |   This program is free software; you can redistribute it and/or modify      |
# |   it under the terms of the GNU General Public License as published by      |
# |   the Free Software Foundation; either version 2 of the License, or         |
# |   (at your option) any later version.                                       |
# |                                                                             |
# |   This program is distributed in the hope that it will be useful,           |
# |   but WITHOUT ANY WARRANTY; without even the implied warranty of            |
# |   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
# |   GNU General Public License for more details.                              |
# |                                                                             |
# |   You should have received a copy of the GNU General Public License         |
# |   along with this program; if not, write to the                             |
# |   Free Software Foundation, Inc.,                                           |
# |   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 |
# +-----------------------------------------------------------------------------+

if (NOT CASACORE_FOUND)

## -----------------------------------------------------------------------------
## Check for the casacore distribution
##
## Variables assigned:
##  HAVE_CASACORE          = Do we have both headers and libraries of casacore?
##  CASACORE_INCLUDES      = Path to the casacore header files
##  CASACORE_LIBRARIES     = Libraries of the casacore modules
##  CASACORE_COMPILE_FLAGS = 
##
## -----------------------------------------------------------------------------

## ==============================================================================
##
##  Organization of casacore and its components
##
## ==============================================================================

# Dependency tree of the casacore modules:
#   casa         :  --
#   scimath_f    :  casa
#   scimath      :  scimath_f
#   tables       :  casa
#   measures     :  scimath, tables
#   measures_f   :  scimath, tables
#   lattices     ->  scimath, tables
#   fits         ->  measures
#   ms           ->  measures
#   coordinates  ->  fits
#   msfits       ->  fits, ms
#   components   ->  coordinates
#   images       ->  components, mirlib, lattices

##__________________________________________________________
## List of casacore modules

set (casacore_modules
  casa
  tables
  mirlib
  scimath_f
  scimath
  measures
  fits
  coordinates
  components
  lattices
  ms
  images
  msfits
  )

##__________________________________________________________
## casacore library dependencies

set (casacore_dependencies_components   coordinates                )
set (casacore_dependencies_coordinates  fits                       )
set (casacore_dependencies_fits         measures                   )
set (casacore_dependencies_images       components lattices mirlib )
set (casacore_dependencies_lattices     scimath tables             )
set (casacore_dependencies_measures     scimath tables             )
set (casacore_dependencies_measures_f   scimath tables             )
set (casacore_dependencies_ms           measures                   )
set (casacore_dependencies_msfits       fits ms                    )
set (casacore_dependencies_scimath      scimath_f                  )
set (casacore_dependencies_scimath_f    casa                       )
set (casacore_dependencies_tables       casa                       )

set (casacore_header_casa      casa/Arrays.h        )
set (casacore_header_tables    tables/Tables.h      )
set (casacore_header_scimath   scimath/Fitting.h    )
set (casacore_header_measures  measures/Measures.h  )
set (casacore_header_fits      fits/FITS.h          )
set (casacore_header_ms        ms/MeasurementSets.h )

set (casacore_headers
  casa/Arrays.h
  tables/Tables.h
  mirlib/miriad.h
  scimath/Fitting.h
  measures/Measures.h
  fits/FITS.h
  coordinates/Coordinates.h
  components/ComponentModels.h
  lattices/Lattices.h
  ms/MeasurementSets.h
  images/Images.h
  msfits/MSFits.h
  )

## ==============================================================================
##
##  Search for installation of casacore
##
## ==============================================================================

list (APPEND CMAKE_INCLUDE_PATH ${CASACORE_ROOT_DIR}/include/casacore )
list (APPEND CMAKE_INCLUDE_PATH /sw/share/casacore                    )
list (APPEND CMAKE_INCLUDE_PATH /app/usg/release/include              )

list (APPEND CMAKE_LIBRARY_PATH ${CASACORE_ROOT_DIR}/lib )
list (APPEND CMAKE_LIBRARY_PATH /opt/casacore/lib        )
list (APPEND CMAKE_LIBRARY_PATH /app/usg/release/lib     )

## -----------------------------------------------------------------------------
## Check for system header files

if (NOT CASACORE_FIND_QUIETLY)
  message (STATUS "[FindCASACORE] Check for system header files ...")
endif (NOT CASACORE_FIND_QUIETLY)

find_path (HAVE_ASSERT_H  assert.h )
find_path (HAVE_CTYPE_H   ctype.h  )
find_path (HAVE_FCNTL_H   fcntl.h  )
find_path (HAVE_STDLIB_H  stdlib.h )
find_path (HAVE_STDIO_H   stdio.h  )
find_path (HAVE_STRING_H  string.h )
find_path (HAVE_UNISTD_H  unistd.h )

## -----------------------------------------------------------------------------
## Required external packages

find_library (HAVE_LIBM         m        )
find_library (HAVE_LIBG2C       g2c      )
find_library (HAVE_LIBF2C       f2c      )
find_library (HAVE_LIBGFORTRAN  gfortran )

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

foreach (CASACORE_HEADER ${casacore_headers})
  ## search for the header file of a given module
  find_path (header_path ${CASACORE_HEADER}
    PATH_SUFFIXES casacore include/casacore
    )
  ## if the header file was found, add its path to the include path
  if (header_path)
    get_filename_component (tmp ${header_path} ABSOLUTE)
    list (APPEND CASACORE_INCLUDES ${tmp})
  else (header_path)
    message (MESSAGE "[casacore] Unable to locate ${CASACORE_HEADER}")
  endif (header_path)
endforeach (CASACORE_HEADER)

## Remove duplicates from the list of include directories

list (REMOVE_DUPLICATES CASACORE_INCLUDES)

## -----------------------------------------------------------------------------
## Check for the library

if (NOT CASACORE_FIND_QUIETLY)
  message (STATUS "[FindCASACORE] Check for the library ...")
endif (NOT CASACORE_FIND_QUIETLY)

## Locate the libraries themselves; keep in mind that the libraries follow
## the naming convention libcasa_<module>, e.g. libcasa_images.a

foreach (casacore_lib ${casacore_modules})
  ## search for the library
  if (${casacore_lib} MATCHES "mirlib")
    find_library (CASACORE_lib${casacore_lib} mir casa_mirlib)
  else (${casacore_lib} MATCHES "mirlib")
    find_library (CASACORE_lib${casacore_lib} casa_${casacore_lib})
  endif (${casacore_lib} MATCHES "mirlib")
  ## if we have found the library, add it to the list
  if (CASACORE_lib${casacore_lib})
    list (APPEND CASACORE_LIBRARIES ${CASACORE_lib${casacore_lib}})
  endif (CASACORE_lib${casacore_lib})
endforeach (casacore_lib)

## Once we are done, rearrange the order of the libraries in the list

if (CASACORE_LIBRARIES)
  list (REVERSE CASACORE_LIBRARIES)
endif (CASACORE_LIBRARIES)

## -----------------------------------------------------------------------------
## If detection successful, register package as found

if (CASACORE_INCLUDES AND CASACORE_LIBRARIES)
  set ( HAVE_CASACORE  TRUE )
  set ( CASACORE_FOUND TRUE )
else (CASACORE_INCLUDES AND CASACORE_LIBRARIES)
  set ( HAVE_CASACORE  FALSE )
  set ( CASACORE_FOUND FALSE )
  if (NOT CASACORE_FIND_QUIETLY)
    if (NOT CASACORE_INCLUDES)
      message (STATUS "Unable to find CASACORE header files!")
    endif (NOT CASACORE_INCLUDES)
    if (NOT CASACORE_LIBRARIES)
      message (STATUS "Unable to find CASACORE library files!")
    endif (NOT CASACORE_LIBRARIES)
  endif (NOT CASACORE_FIND_QUIETLY)
endif (CASACORE_INCLUDES AND CASACORE_LIBRARIES)

IF (HAVE_CASACORE)
  IF (NOT CASACORE_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for CASACORE")
    MESSAGE (STATUS "CASACORE_LIBRARIES = ${CASACORE_LIBRARIES}")
    MESSAGE (STATUS "CASACORE_INCLUDES  = ${CASACORE_INCLUDES}")
  ENDIF (NOT CASACORE_FIND_QUIETLY)
ENDIF (HAVE_CASACORE)

## ------------------------------------------------------------------------------
## Final assembly of the provided variables and flags; once this is done, we
## provide some extended feedback.

## --- Compiler settings -------------------------

IF (UNIX)
  add_definitions (
    -DAIPS_STDLIB
    -DAIPS_AUTO_STL
    )
  set (CASACORE_COMPILE_FLAGS "-DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_NO_LEA_MALLOC")
  IF (APPLE)
    set (CASACORE_COMPILE_FLAGS "${CASACORE_COMPILE_FLAGS} -DAIPS_DARWIN")
  else (APPLE)
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

set (CASA_INCLUDES "")
set (CASA_INCLUDES ${CASACORE_INCLUDES})

set (CASA_LIBRARIES "")
set (CASA_LIBRARIES ${CASACORE_LIBRARIES})

## ------------------------------------------------------------------------------
## Variables only to be displayed in advanced mode

mark_as_advanced (
  casacore_modules
  casacore_headers
  )

endif (NOT CASACORE_FOUND)
