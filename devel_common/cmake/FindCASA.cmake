
## -----------------------------------------------------------------------------
## - Check for the AIPS++/CASA library
##
## Variables assigned:
##  HAVE_CASA
##  HAVE_AIPS_H    = aips.h header file located
##  HAVE_GLISH_H   = glish.h header file located
##  CASA_INCLUDES  = Path to the CASA header files
##  CASA_LIBRARIES = libraries of the CASA modules
##

## -----------------------------------------------------------------------------
## Required external packages

find_library (libg2c g2c
  PATHS /usr/local/lib /usr/lib /lib /sw/lib
  )

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
  PATH_SUFFIXES
  code/include/casa
  current/code/include/casa
  weekly/code/include/casa
  stable/code/include/casa
  )

if (CASA_INCLUDES)
  set (HAVE_AIPS_H true)
  string (REGEX REPLACE include/casa include CASA_INCLUDES ${CASA_INCLUDES})
  string (REGEX REPLACE /code/include "" AIPSROOT ${CASA_INCLUDES})
endif (CASA_INCLUDES)

## Find "glish.h", which is pointing towards the location of the Glish source code

if (HAVE_AIPS_H)
  ## locate the Glish header file
  find_path (GLISH_INCLUDE_DIR glish.h
    ${CASA_INCLUDES}
    PATH_SUFFIXES
    Glish
    )
  ## adjust the include path for the Glish header files
  if (GLISH_INCLUDE_DIR)
    set (HAVE_GLISH_H true)
    STRING (REGEX REPLACE include/Glish include GLISH_INCLUDE_DIR ${GLISH_INCLUDE_DIR})
  endif (GLISH_INCLUDE_DIR)
endif (HAVE_AIPS_H)

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

find_path (AIPSLIBD version.o
  PATHS ${AIPSROOT}
  PATH_SUFFIXES darwin/lib linux/lib linux_gnu/lib
  )

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
##   LINKcalibration := calibration LINKmsvis
##   LINKionosphere  := ionosphere LINKmeasures
##   LINKflagging    := flagging msvis ms lattices LINKmeasures
##   LINKdish        := dish ms fits coordinates lattices LINKmeasures
##   LINKsimulators  := simulators LINKms
##   LINKsynthesis   := synthesis calibration msvis ms images TMPcomponents
##   LINKgraphics    := graphics LINKcasa
##   LINKtasking     := tasking graphics msvis ms images TMPcomponents
##

## string together the correct order of the libraries as passed to the linker

set (LINKcasa
  ${CASA_libcasa}
  CACHE STRING "LINKcasa")

set (LINKscimath
  ${CASA_libscimath} ${CASA_libscimath_f} ${LINKcasa} ${libg2c}
  CACHE STRING "LINKscimath")

set (LINKtables
  ${CASA_libtables} ${LINKcasa}
  CACHE STRING "LINKtables")

set (LINKmeasures
  ${CASA_libmeasures} ${CASA_libtables} ${LINKscimath}
  CACHE STRING "LINKmeasures")

set (LINKfits
  ${CASA_libfits} ${LINKmeasures}
  CACHE STRING "LINKfits")

set (LINKlattices
  ${CASA_liblattices} ${CASA_libtables} ${LINKscimath}
  CACHE STRING "LINKlattices")

set (LINKcoordinates
  ${CASA_libcoordinates} ${LINKfits}
  CACHE STRING "LINKcoordinates")

set (LINKcomponents
  ${CASA_libcomponents} ${LINKcoordinates}
  CACHE STRING "LINKcomponents")

set (TMPcomponents 
  ${CASA_libcomponents} ${CASA_libcoordinates} ${CASA_liblattices} ${LINKfits}
  CACHE STRING "TMPcomponents")

set (LINKimages
  ${CASA_libimages} ${TMPcomponents}
  CACHE STRING "LINKimages")

set (LINKms
  ${CASA_libms} ${LINKmeasures}
  CACHE STRING "LINKms")

set (LINKmsfits
  ${CASA_libmsfits} ${CASA_libms} ${LINKfits}
  CACHE STRING "LINKmsfits")

set (LINKmsvis
  ${CASA_libmsvis} ${LINKms}
  CACHE STRING "LINKmsvis")

set (LINKcalibration
  ${CASA_libcalibration} ${LINKmsvis}
  CACHE STRING "LINKcalibration")

set (LINKionosphere
  ${CASA_libionosphere} ${LINKmeasures}
  CACHE STRING "LINKionosphere")

set (LINKflagging
  ${CASA_libflagging} ${CASA_libmsvis} ${CASA_libms} ${CASA_liblattices}
  ${LINKmeasures}
  CACHE STRING "LINKflagging")

set (LINKdish
  "-ldish -lms -lfits -lcoordinates -llattices ${LINKmeasures}"
  CACHE STRING "LINKdish")

set (LINKsimulators
  ${CASA_libsimulators} ${LINKms}
  CACHE STRING "LINKsimulators")

set (LINKsynthesis
  "-lsynthesis -lcalibration -lmsvis -lms -limages ${TMPcomponents}"
  CACHE STRING "LINKsynthesis")

set (LINKgraphics
  ${CASA_libgraphics} ${LINKcasa}
  CACHE STRING "LINKgraphics")

set (LINKtasking
  "-ltasking -lgraphics -lmsvis -lms -limages ${TMPcomponents}"
  CACHE STRING "LINKtasking")

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
    -DAIPS_${AIPS_ENDIAN}_ENDIAN
    -DNATIVE_EXCP
    -DAIPS_STDLIB
    -DAIPS_AUTO_STL
    -DAIPS_NO_LEA_MALLOC
    -D_GLIBCPP_DEPRECATED
    -DSIGNBIT 
    -DAIPS_NO_TEMPLATE_SRC
    -I${CASA_INCLUDES}
    -DAIPS_${AIPS_ARCH})
  set (CASA_CXX_FLAGS "-fPIC -pipe -Wall -Wno-non-template-friend -Woverloaded-virtual -Wno-comment -fexceptions -Wcast-align")
  set (CASA_CXX_LFLAGS "-L${CASA_LIBRARIES_DIR} ${CASA_LIBRARIES_DIR}/version.o ${CASA_LIBRARIES}")
  if (NOT CASA_FIND_QUIETLY)
    message (STATUS "Found components for CASA.")
    message (STATUS "AIPSARCH ........... : ${AIPS_ARCH}")
    message (STATUS "AIPSROOT ........... : ${AIPSROOT}")
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
## Mark as advanced ...

mark_as_advanced (
  HAVE_CASA
  CASA_INCLUDES
  CASA_LIBRARIES
  )
