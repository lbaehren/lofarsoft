# - Check for the AIPS++/CASA library
#

## -----------------------------------------------------------------------------
## Preparation: get additional information about the system

SET (CASA_DARWIN_BASE /sw/share/casa)

IF (UNIX)
	SET (CASA_ARCH "LINUX")
	MESSAGE (STATUS "Unix system detected")
	IF (APPLE)
		MESSAGE (STATUS "Max OS system detected")
		SET (CASA_ARCH "DARWIN")
	ENDIF (APPLE)
ENDIF (UNIX)

## -----------------------------------------------------------------------------
## Check for the library

FIND_PATH (CASA_INCLUDE_DIR
	 aips.h
	 PATHS ${CASA_DARWIN_BASE}/code/include/casa /opt/casa/include/casa
)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_LIBRARY(CASA_LIBRARIES
	NAMES casa
	PATHS ${CASA_DARWIN_BASE}/darwin/lib /opt/casa/lib
)

## -----------------------------------------------------------------------------
## If detection successful, register package as found

IF (CASA_INCLUDE_DIR AND CASA_LIBRARIES)
   SET(CASA_FOUND TRUE)
   STRING (REGEX REPLACE include/casa include CASA_INCLUDE_DIR ${CASA_INCLUDE_DIR})
   STRING (REGEX REPLACE lib/libcasa.a lib CASA_LIBRARIES_DIR ${CASA_LIBRARIES})
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
#	SET (CASA_CXX_FLAGS "-DAIPS_${CASA_ARCH} -DAIPS_LITTLE_ENDIAN -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED -DSIGNBIT  -DAIPS_NO_TEMPLATE_SRC -DAIPS_NO_TEMPLATE_SRC -I${CASA_INCLUDE_DIR} -DAIPS_${CASA_ARCH} -Wno-deprecated -fno-implicit-templates")
#	SET (CASA_CXX_LFLAGS "-L${CASA_LIBRARIES_DIR} ${CASA_LIBRARIES_DIR}/version.o -lmeasures -ltables -lscimath -lcasa")
	IF (NOT CASA_FIND_QUIETLY)
		MESSAGE (STATUS "Found CASA.")
		MESSAGE (STATUS "CASA architecture .. : ${CASA_ARCH}")
		MESSAGE (STATUS "CASA library dir ... : ${CASA_LIBRARIES_DIR}")
		MESSAGE (STATUS "CASA library files . : ${CASA_LIBRARIES}")
		MESSAGE (STATUS "CASA header files .. : ${CASA_INCLUDE_DIR}")
		MESSAGE (STATUS "CASA compile command : ${CASA_CXX_FLAGS}")
		MESSAGE (STATUS "CASA linker command  : ${CASA_CXX_LFLAGS}")
   ENDIF (NOT CASA_FIND_QUIETLY)
ELSE (CASA_FOUND)
   IF (CASA_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find CASA")
   ENDIF (CASA_FIND_REQUIRED)
ENDIF (CASA_FOUND)
