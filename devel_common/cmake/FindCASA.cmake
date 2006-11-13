# - Check for the AIPS++/CASA library
#

## -----------------------------------------------------------------------------
## Preparation: get additional information about the system
## AIPS++/CASA uses a number of compiler flags describing the platform, which 
## need to be detected and assigned.

SET (AIPSROOT "/sw/share/casa")
SET (AIPSCODE "${AIPSROOT}/code")

IF (UNIX)
  SET (AIPSARCH "${AIPSROOT}/linux")
  SET (AIPS_ARCH "LINUX")
  SET (AIPS_ENDIAN "LITTLE")
  IF (APPLE)
    SET (AIPSARCH "${AIPSROOT}/darwin")
    SET (AIPS_ARCH "DARWIN")
    IF (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
      SET (AIPS_ENDIAN "BIG")
    ENDIF (${CMAKE_OSX_ARCHITECTURES} MATCHES "ppc")
  ENDIF (APPLE)
ENDIF (UNIX)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_PATH (CASA_INCLUDE_DIR aips.h
  PATHS ${AIPSCODE} /opt/casa /casa /dop71_0/aips++/weekly/code
  PATH_SUFFIXES /include/casa
  )

IF (CASA_INCLUDE_DIR)
  STRING (REGEX REPLACE include/casa include CASA_INCLUDE_DIR ${CASA_INCLUDE_DIR})
ENDIF (CASA_INCLUDE_DIR)

## -----------------------------------------------------------------------------
## Check for the library

SET (AIPSLIBD ${AIPSARCH}/lib /opt/casa/lib)

FIND_LIBRARY (CALIBRATION_LIBRARY calibration ${AIPSLIBD})
FIND_LIBRARY (CASA_LIBRARY casa ${AIPSLIBD})
FIND_LIBRARY (COMPONENTS_LIBRARY components ${AIPSLIBD})
FIND_LIBRARY (COORDINATES_LIBRARY coordinates ${AIPSLIBD})
FIND_LIBRARY (FITS_LIBRARY fits ${AIPSLIBD})
FIND_LIBRARY (GRAPHICS_LIBRARY graphics ${AIPSLIBD})
FIND_LIBRARY (IMAGES_LIBRARY images ${AIPSLIBD})
FIND_LIBRARY (LATTICES_LIBRARY lattices ${AIPSLIBD})
FIND_LIBRARY (MEASURES_LIBRARY measures ${AIPSLIBD})
FIND_LIBRARY (MS_LIBRARY ms ${AIPSLIBD})
FIND_LIBRARY (MSFITS_LIBRARY msfits ${AIPSLIBD})
FIND_LIBRARY (MSVIS_LIBRARY msvis ${AIPSLIBD})
FIND_LIBRARY (SCIMATH_LIBRARY scimath ${AIPSLIBD})
FIND_LIBRARY (TABLES_LIBRARY tables ${AIPSLIBD})
FIND_LIBRARY (TASKING_LIBRARY tasking ${AIPSLIBD})

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

IF (CASA_LIBRARY)
  SET (CASA_LIBRARIES "-lcasa")
  IF (SCIMATH_LIBRARY)
    SET (CASA_LIBRARIES "-lscimath -lcasa")
  ENDIF (SCIMATH_LIBRARY)
  IF (TABLES_LIBRARY)
    SET (CASA_LIBRARIES "-ltables -lcasa")
  ENDIF (TABLES_LIBRARY)
  IF (GRAPHICS_LIBRARY)
    SET (CASA_LIBRARIES "-lgraphics -lcasa")
  ENDIF (GRAPHICS_LIBRARY)
  IF (SCIMATH_LIBRARY AND TABLES_LIBRARY)
    IF (MEASURES_LIBRARY)
      SET (CASA_LIBRARIES "-lmeasures -ltables -lscimath -lcasa")
      IF (MS_LIBRARY)
	SET (CASA_LIBRARIES "-lms -lmeasures -ltables -lscimath -lcasa")
      ENDIF (MS_LIBRARY)
      IF (FITS_LIBRARY)
	SET (CASA_LIBRARIES "-lfits -lmeasures -ltables -lscimath -lcasa")
	IF (COORDINATES_LIBRARY)
	  SET (CASA_LIBRARIES "-lcoordinates -lfits -lmeasures -ltables -lscimath -lcasa")
	  IF (COMPONENTS_LIBRARY)
	    SET (CASA_LIBRARIES "-lcomponents -lcoordinates -lfits -lmeasures -ltables -lscimath -lcasa")
	  ENDIF (COMPONENTS_LIBRARY)
	ENDIF (COORDINATES_LIBRARY)
      ENDIF (FITS_LIBRARY)
    ENDIF (MEASURES_LIBRARY)
  ENDIF (SCIMATH_LIBRARY AND TABLES_LIBRARY)
ENDIF (CASA_LIBRARY)

## -----------------------------------------------------------------------------
## If detection successful, register package as found

IF (CASA_INCLUDE_DIR AND CASA_LIBRARIES)
  SET(CASA_FOUND TRUE)
  STRING (REGEX REPLACE lib/libcasa.a lib CASA_LIBRARIES_DIR ${CASA_LIBRARY})
ELSE (CASA_INCLUDE_DIR AND CASA_LIBRARIES)
  IF (NOT CASA_FIND_QUIETLY)
    IF (NOT CASA_INCLUDE_DIR)
      MESSAGE (STATUS "Unable to find CASA header files!")
    ENDIF (NOT CASA_INCLUDE_DIR)
    IF (NOT CASA_LIBRARIES)
      MESSAGE (STATUS "Unable to find CASA library files!")
    ENDIF (NOT CASA_LIBRARIES)
  ENDIF (NOT CASA_FIND_QUIETLY)
ENDIF (CASA_INCLUDE_DIR AND CASA_LIBRARIES)

## -----------------------------------------------------------------------------

IF (CASA_FOUND)
  SET (CASA_CXX_FLAGS "-DAIPS_${AIPS_ARCH} -DAIPS_${AIPS_ENDIAN}_ENDIAN -DNATIVE_EXCP -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED -DSIGNBIT  -DAIPS_NO_TEMPLATE_SRC -DAIPS_NO_TEMPLATE_SRC -I${CASA_INCLUDE_DIR} -DAIPS_${AIPS_ARCH} -Wno-deprecated -fno-implicit-templates")
  SET (CASA_CXX_LFLAGS "-L${CASA_LIBRARIES_DIR} ${CASA_LIBRARIES_DIR}/version.o ${CASA_LIBRARIES}")
  IF (NOT CASA_FIND_QUIETLY)
    MESSAGE (STATUS "Found components for CASA.")
    MESSAGE (STATUS "CASA architecture .. : ${AIPS_ARCH}")
    MESSAGE (STATUS "CASA library dir ... : ${CASA_LIBRARIES_DIR}")
    MESSAGE (STATUS "CASA library files . : ${CASA_LIBRARIES}")
    MESSAGE (STATUS "CASA header files .. : ${CASA_INCLUDE_DIR}")
    MESSAGE (STATUS "CASA compile command : ${CASA_CXX_FLAGS}")
    MESSAGE (STATUS "CASA linker command  : ${CASA_CXX_LFLAGS}")
  ENDIF (NOT CASA_FIND_QUIETLY)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CASA_CXX_FLAGS}")
ELSE (CASA_FOUND)
  IF (CASA_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find CASA")
  ENDIF (CASA_FIND_REQUIRED)
ENDIF (CASA_FOUND)
