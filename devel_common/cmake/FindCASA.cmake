##------------------------------------------------------------------------------
## $Id::                                                                       $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## - Check for the AIPS++/CASA library
##
## Variables assigned:
##  HAVE_CASA
##  CASA_BASEDIR   = Base directory of the CASA installation (aka AIPSROOT)
##  HAVE_AIPS_H    = aips.h header file located
##  HAVE_GLISH_H   = glish.h header file located
##  CASA_INCLUDES  = Path to the CASA header files
##  CASA_LIBRARIES = libraries of the CASA modules
##

## -----------------------------------------------------------------------------
## Required external packages

## check where to find the CMake scripts

find_path (cr_cmake FindCASA.cmake FindGlish.cmake
  PATHS 
  ${CR_SOURCE_DIR}
  ${CR_SOURCE_DIR}/..
  ${CR_SOURCE_DIR}/../..
  ${CR_SOURCE_DIR}/../../..
  PATH_SUFFIXES
  devel_common/cmake
  )

## [1] libg2c

find_library (libg2c g2c
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

if (cr_cmake)
  ## Glish
  if (NOT HAVE_GLISH)
    include (${cr_cmake}/FindGlish.cmake)
  endif (NOT HAVE_GLISH)
else (cr_cmake)
  message (SEND_ERROR "Unable to locate additional CMake find scripts!")
endif (cr_cmake)

## -----------------------------------------------------------------------------
## Check for the header files first, as from this we can derive a number of 
## fundamental CASA variables (such as e.g. the root directory of the
## installation). The main point to keep in mind here is, that parts of the
## final installation will reside in architecture-dependent directories - the
## setup of the code directory however is generic, so once we have located it
## we can go from there.

## Find "aips.h", which is included by all other files

find_path (CASA_INCLUDES aips.h
  PATHS
  /casa
  /opt/casa
  /sw/share/casa
  /app/aips++/Stable
  PATH_SUFFIXES
  code/include/casa
  current/code/include/casa
  weekly/code/include/casa
  stable/code/include/casa
  )

if (CASA_INCLUDES)
  set (HAVE_AIPS_H true)
  string (REGEX REPLACE include/casa include CASA_INCLUDES ${CASA_INCLUDES})
  string (REGEX REPLACE /code/include "" CASA_BASEDIR ${CASA_INCLUDES})
endif (CASA_INCLUDES)

## Once we have detected the header files - and therewith the path to the root
## of the CASA installation, we need to locate the library files to get the 
## library directory

find_path (AIPSLIBD version.o casa
  PATHS ${CASA_BASEDIR}
  PATH_SUFFIXES darwin/lib linux/lib linux_gnu/lib
  )

## -----------------------------------------------------------------------------
## The is a number of packages, which are distrubuted along with CASA, so once
## we have been able to discover the location of the CASA base directory, we
## can set the paths to these additional packages

## [1] Glish

if (HAVE_GLISH)
  list (APPEND CASA_INCLUDES ${GLISH_INCLUDES})
endif (HAVE_GLISH)

## [2] WCSLIB -- library for the dealing with world coordinate systems

find_library (libwcs
  wcs
  PATHS ${CASA_BASEDIR}
  PATH_SUFFIXES
  darwin/lib
  linux_gnu/lib
  )

find_path (wcslib_includes
  wcs.h
  PATHS ${CASA_BASEDIR}/code/casa
  PATH_SUFFIXES wcslib
  )

if (wcslib_includes)
  string (REGEX REPLACE wcslib "" wcslib_includes ${wcslib_includes})
  list (APPEND CASA_INCLUDES ${wcslib_includes})
endif (wcslib_includes)

## -----------------------------------------------------------------------------
## Preparation: get additional information about the system

if (UNIX)
  set (AIPS_ARCH "LINUX")
  set (AIPS_ENDIAN "LITTLE")
  IF (APPLE)
    ## definition passed to the compiler
    SET (AIPS_DARWIN 1)
    ## AIPS architecture -- as used for the compiler
    SET (AIPS_ARCH "DARWIN")
    ## AIPS architecture -- AIPSARCH system variable
    SET (AIPSARCH "darwin")
    ## check the processor type
    IF (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
      SET (AIPS_ENDIAN "BIG")
    ENDIF (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
  endif (APPLE)
endif (UNIX)

## -----------------------------------------------------------------------------
## Check for the library

string (REGEX REPLACE /lib "" AIPSARCH ${AIPSLIBD})

## which libraries do we have to find?

set (casa_libs
  calibration
  casa
  components
  coordinates
  fits
  graphics
  images
  lattices
  measures
  ms
  msfits
  msvis
  scimath
  scimath_f
  tables
  tasking
  )

foreach (casa_lib ${casa_libs})
  find_library (CASA_lib${casa_lib} ${casa_lib} ${AIPSLIBD})
endforeach (casa_lib)

## -----------------------------------------------------------------------------
## Due to fact, that the individual modules of the CASA libraries are
## interdependent, we need to take this into account when setting up the linker
## instructions. The reference for the dependences listed here is the
## corresponding table listed in the 'makedefs'
##
##  LINKcasa        := casa
##  LINKscimath     := scimath casa
##  LINKtables      := tables casa
##  LINKmeasures    := measures tables scimath casa
##  LINKfits        := fits measures tables scimath casa
##  LINKlattices    := lattices tables scimath casa
##  LINKcoordinates := coordinates fits measures tables scimath casa
##  LINKcomponents  := components coordinates fits measures tables scimath casa
##  LINKimages      := images components coordinates lattices fits measures
##                      tables scimath casa
##  LINKms          := ms measures tables scimath casa
##  LINKmsfits      := msfits ms fits measures tables scimath casa
##  LINKmsvis       := msvis ms measures tables scimath casa
##  LINKcalibration := calibration msvis ms measures tables scimath casa
##  LINKionosphere  := ionosphere measures tables scimath casa
##  LINKflagging    := flagging msvis ms lattices measures tables scimath casa
##  LINKdish        := dish ms fits coordinates lattices measures tables scimath
##                      casa
##  LINKsimulators  := simulators ms measures tables scimath casa
##  LINKsynthesis   := synthesis calibration msvis ms images components
##                      coordinates lattices fits measures tables scimath casa
##  LINKgraphics    := graphics casa
##  LINKtasking     := tasking graphics msvis ms images components coordinates
##                      lattices fits measures tables scimath casa
##
##  Starting off with the original table by subsequent substitution we get
##
##   LINKcasa        := casa
##   LINKscimath     := scimath LINKcasa
##   LINKtables      := tables LINKcasa
##   LINKmeasures    := measures tables LINKscimath
##   LINKfits        := fits LINKmeasures
##   LINKlattices    := lattices tables LINKscimath
##   LINKcoordinates := coordinates LINKfits
##   LINKcomponents  := components LINKcoordinates
##   TMPcomponents   := components coordinates lattices LINKfits
##   LINKimages      := images TMPcomponents
##   LINKms          := ms LINKmeasures
##   LINKmsfits      := msfits ms LINKfits
##   LINKmsvis       := msvis LINKms
##   TMPmsvis        := msvis ms images
##   LINKcalibration := calibration LINKmsvis
##   LINKionosphere  := ionosphere LINKmeasures
##   LINKflagging    := flagging msvis ms lattices LINKmeasures
##   LINKdish        := dish ms fits coordinates lattices LINKmeasures
##   LINKsimulators  := simulators LINKms
##   LINKsynthesis   := synthesis calibration TMPmsvis TMPcomponents
##   LINKgraphics    := graphics LINKcasa
##   LINKtasking     := tasking graphics TMPmsvis TMPcomponents
##

## string together the correct order of the libraries as passed to the linker

## -- libcasa
if (CASA_libcasa)
  set (LINKcasa
    ${CASA_libcasa}
    CACHE STRING "LINKcasa")
  ## -- libtables
  if (CASA_libtables)
    set (LINKtables
      ${CASA_libtables} ${LINKcasa}
      CACHE STRING "LINKtables")
  endif (CASA_libtables)
  ## -- libscimath
  if (libg2c AND CASA_libscimath AND CASA_libscimath_f)
    set (LINKscimath
      ${CASA_libscimath} ${CASA_libscimath_f} ${LINKcasa} ${libg2c}
      CACHE STRING "LINKscimath")
  else (libg2c AND CASA_libscimath AND CASA_libscimath_f)
    message (ERROR "Missing g2c library required for libscimath!")
  endif (libg2c AND CASA_libscimath AND CASA_libscimath_f)
  ## -- libmeasures
  if (CASA_libmeasures AND CASA_libtables AND LINKscimath)
    set (LINKmeasures
      ${CASA_libmeasures} ${CASA_libtables} ${LINKscimath}
      CACHE STRING "LINKmeasures")
  endif (CASA_libmeasures AND CASA_libtables AND LINKscimath)
  ## -- libfits
  if (CASA_libfits AND LINKmeasures AND libwcs)
    set (LINKfits
      ${CASA_libfits} ${LINKmeasures} ${libwcs}
      CACHE STRING "LINKfits")
  else (CASA_libfits AND LINKmeasures AND libwcs)
    message (ERROR "Missing WCS library required for libfits!")
  endif (CASA_libfits AND LINKmeasures AND libwcs)
  ## -- liblattices
  if (CASA_liblattices AND CASA_libtables AND LINKscimath)
    set (LINKlattices
      ${CASA_liblattices} ${CASA_libtables} ${LINKscimath}
      CACHE STRING "LINKlattices")
  endif (CASA_liblattices AND CASA_libtables AND LINKscimath)
  ## -- libcoordinates
  if (CASA_libcoordinates AND LINKfits)
    set (LINKcoordinates
      ${CASA_libcoordinates} ${LINKfits}
      CACHE STRING "LINKcoordinates")
  endif (CASA_libcoordinates AND LINKfits)
  ## -- libcomponents
  if (CASA_libcomponents AND LINKcoordinates)
    set (LINKcomponents
      ${CASA_libcomponents} ${LINKcoordinates}
      CACHE STRING "LINKcomponents")
    if (CASA_liblattices AND LINKfits)
      set (TMPcomponents 
	${CASA_libcomponents} ${CASA_libcoordinates} ${CASA_liblattices} ${LINKfits}
	CACHE STRING "TMPcomponents")
    endif (CASA_liblattices AND LINKfits)
  endif (CASA_libcomponents AND LINKcoordinates)
  ## -- libimages
  if (CASA_libimages AND TMPcomponents)
    set (LINKimages
      ${CASA_libimages} ${TMPcomponents}
      CACHE STRING "LINKimages")
  endif (CASA_libimages AND TMPcomponents)
  ## -- libms
  if (CASA_libms AND LINKmeasures)
    set (LINKms
      ${CASA_libms} ${LINKmeasures}
      CACHE STRING "LINKms")
  endif (CASA_libms AND LINKmeasures)
  ## -- libmsfits
  if (CASA_libmsfits AND CASA_libms AND LINKfits)
    set (LINKmsfits
      ${CASA_libmsfits} ${CASA_libms} ${LINKfits}
      CACHE STRING "LINKmsfits")
  endif (CASA_libmsfits AND CASA_libms AND LINKfits)
  ## -- libmsvis
  if (CASA_libmsvis AND LINKms)
    set (LINKmsvis
      ${CASA_libmsvis} ${LINKms}
      CACHE STRING "LINKmsvis")
    if (LINKimages)
      set (TMPmsvis
	${CASA_libmsvis} ${CASA_libms} ${CASA_libimages}
	CACHE STRING "TMPmsvis")
    endif (LINKimages)
  endif (CASA_libmsvis AND LINKms)
  ## -- libcalibration
  if (CASA_libcalibration AND LINKmsvis)
    set (LINKcalibration
      ${CASA_libcalibration} ${LINKmsvis}
      CACHE STRING "LINKcalibration")
  endif (CASA_libcalibration AND LINKmsvis)
  ## -- libionosphere
  if (CASA_libionosphere AND LINKmeasures)
    set (LINKionosphere
      ${CASA_libionosphere} ${LINKmeasures}
      CACHE STRING "LINKionosphere")
  endif (CASA_libionosphere AND LINKmeasures)
  ## -- libflagging
  if (CASA_libflagging AND CASA_libmsvis AND CASA_libms AND CASA_liblattices)
    set (LINKflagging
      ${CASA_libflagging} ${CASA_libmsvis} ${CASA_libms} ${CASA_liblattices}
      ${LINKmeasures}
      CACHE STRING "LINKflagging")
  endif (CASA_libflagging AND CASA_libmsvis AND CASA_libms AND CASA_liblattices)
  ## -- libdish
  set (LINKdish
    ${CASA_libdish} ${CASA_libms} ${CASA_libfits} ${CASA_libcoordinates} ${CASA_liblattices}
    ${LINKmeasures}
    CACHE STRING "LINKdish")
  ## -- libsimulators
  if (CASA_libsimulators AND LINKms)
    set (LINKsimulators
      ${CASA_libsimulators} ${LINKms}
      CACHE STRING "LINKsimulators")
  endif (CASA_libsimulators AND LINKms)
  ## -- libsynthesis
  if (CASA_libsynthesis AND CASA_libcalibration AND TMPmsvis AND TMPcomponents)
    set (LINKsynthesis
      ${CASA_libsynthesis} ${CASA_libcalibration} ${TMPmsvis} ${TMPcomponents}
      CACHE STRING "LINKsynthesis")
  endif (CASA_libsynthesis AND CASA_libcalibration AND TMPmsvis AND TMPcomponents)
  ## -- libgraphics
  if (CASA_libgraphics AND LINKcasa)
    set (LINKgraphics
      ${CASA_libgraphics} ${LINKcasa}
      CACHE STRING "LINKgraphics")
  endif (CASA_libgraphics AND LINKcasa)
  ## -- libtasking
  if (CASA_libtasking AND CASA_libgraphics AND TMPmsvis)
    set (LINKtasking
      ${CASA_libtasking} ${CASA_libgraphics} ${CASA_libmsvis} ${CASA_libms} ${CASA_libimages}
      ${TMPcomponents} 
      CACHE STRING "LINKtasking")
  endif (CASA_libtasking AND CASA_libgraphics AND TMPmsvis)
endif (CASA_libcasa)


## -----------------------------------------------------------------------------
## Depending on the set of located package libraries the instruction towards the
## linker is put together

if (CASA_libcasa)
  set (CASA_LIBRARIES ${LINKcasa})
  if (CASA_libscimath AND CASA_libscimath_f)
    set (CASA_LIBRARIES ${LINKscimath})
  endif (CASA_libscimath AND CASA_libscimath_f)
  if (CASA_libtables)
    set (CASA_LIBRARIES ${LINKtables})
  endif (CASA_libtables)
  if (CASA_libgraphics)
    set (CASA_LIBRARIES ${LINKgraphics})
  endif (CASA_libgraphics)
  if (CASA_libscimath AND CASA_libtables)
    if (CASA_libmeasures)
      set (CASA_LIBRARIES ${LINKmeasures})
      if (MS_LIBRARY)
	set (CASA_LIBRARIES ${LINKms})
      endif (MS_LIBRARY)
      if (CASA_libfits)
	set (CASA_LIBRARIES ${LINKfits})
	if (CASA_libcoordinates)
	  set (CASA_LIBRARIES ${LINKcoordinates})
	  if (CASA_libcomponents)
	    set (CASA_LIBRARIES ${LINKcomponents})
	    if (CASA_libimages)
	      set (CASA_LIBRARIES ${LINKimages})
	      ##
	      if (CASA_libtasking)
		set (CASA_LIBRARIES ${LINKtasking} ${GLISH_LIBRARIES})
	      endif (CASA_libtasking)
	      ##
	    endif (CASA_libimages)
	  endif (CASA_libcomponents)
	endif (CASA_libcoordinates)
      endif (CASA_libfits)
    endif (CASA_libmeasures)
  endif (CASA_libscimath AND CASA_libtables)
endif (CASA_libcasa)

## -----------------------------------------------------------------------------
## If detection successful, register package as found

if (CASA_INCLUDES AND CASA_LIBRARIES)
  set (HAVE_CASA TRUE)
  string (REGEX REPLACE lib/libcasa.a lib CASA_LIBRARIES_DIR ${CASA_libcasa})
else (CASA_INCLUDES AND CASA_LIBRARIES)
  if (NOT CASA_FIND_QUIETLY)
    if (NOT CASA_INCLUDES)
      message (STATUS "Unable to find CASA header files!")
    endif (NOT CASA_INCLUDES)
    if (NOT CASA_LIBRARIES)
      message (STATUS "Unable to find CASA library files!")
    endif (NOT CASA_LIBRARIES)
  endif (NOT CASA_FIND_QUIETLY)
endif (CASA_INCLUDES AND CASA_LIBRARIES)

## ------------------------------------------------------------------------------
## Final assembly of the provided variables and flags; once this is done, we
## provide some extended feedback.

if (HAVE_CASA)
  add_definitions (
    -DAIPS_${AIPS_ARCH}
    -DNATIVE_EXCP
    -DAIPS_STDLIB
    -DAIPS_AUTO_STL
    -DAIPS_NO_LEA_MALLOC
    -D_GLIBCPP_DEPRECATED
    -DSIGNBIT 
    -DAIPS_NO_TEMPLATE_SRC
  )
  ##
  ## Platform test Big/Little Endian ------------------------------------
  ##
  if (CMAKE_SYSTEM_PROCESSOR MATCHES "powerpc")
    message (STATUS "System is big endian.")
    add_definitions (-DAIPS_BIG_ENDIAN)
    set (CMAKE_SYSTEM_BIG_ENDIAN 1)
  elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "powerpc")
    message (STATUS "System is little endian.")
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
  ##
  ## Flags for C++ compiler and Linker --------------------
  ## 
  set (CASA_CXX_FLAGS "-fPIC -pipe -Wall -Wno-non-template-friend -Woverloaded-virtual -Wno-comment -fexceptions -Wcast-align")
  set (CASA_CXX_LFLAGS "${CASA_LIBRARIES_DIR}/version.o ${CASA_LIBRARIES}")
  if (NOT CASA_FIND_QUIETLY)
    message (STATUS "Found components for CASA.")
    message (STATUS "AIPSARCH ........... : ${AIPS_ARCH}")
    message (STATUS "CASA_BASEDIR ....... : ${CASA_BASEDIR}")
    message (STATUS "CASA_LIBRARIES_DIR . : ${CASA_LIBRARIES_DIR}")
    message (STATUS "CASA_LIBRARIES ..... : ${CASA_LIBRARIES}")
    message (STATUS "CASA header files .. : ${CASA_INCLUDES}")
    message (STATUS "CASA compile command : ${CASA_CXX_FLAGS}")
    message (STATUS "CASA linker command  : ${CASA_CXX_LFLAGS}")
  endif (NOT CASA_FIND_QUIETLY)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CASA_CXX_FLAGS}")
else (HAVE_CASA)
  if (CASA_FIND_REQUIRED)
    message (FATAL_ERROR "Could not find CASA")
  endif (CASA_FIND_REQUIRED)
endif (HAVE_CASA)

## ------------------------------------------------------------------------------
## Mark as advanced: clean up not to include all the temporary variables into
## configuration settings

mark_as_advanced (
  CASA_libcasa
  CASA_libcoordinates
  CASA_libimages
  CASA_libmeasures
  CASA_libscimath
  CASA_libscimath_f
  CASA_libtables
  CASA_libcalibration
  CASA_libcomponents
  CASA_libfits
  CASA_libgraphics
  CASA_liblattices
  CASA_libms
  CASA_libmsfits
  CASA_libmsvis
  CASA_libtasking
  LINKcasa
  LINKscimath
  LINKgraphics
  LINKmeasures
  LINKms
  LINKfits
  LINKcalibration
  LINKcomponents
  LINKcoordinates
  LINKdish
  LINKimages
  LINKlattices
  LINKmsfits
  LINKmsvis
  LINKtables
  LINKtasking
  )
