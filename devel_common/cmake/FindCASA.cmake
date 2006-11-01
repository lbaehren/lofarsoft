# - Check for the AIPS++/CASA library
#

## -----------------------------------------------------------------------------
## Preparation: get additional information about the system

SET (CASA_DARWIN_BASE /sw/share/casa)
SET (CASA_LIBRARY_PATHS "${CASA_DARWIN_BASE}/darwin/lib /opt/casa/lib")

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

FIND_PATH (CASA_INCLUDE_DIR aips.h
	 PATHS ${CASA_DARWIN_BASE}/code/include/casa /opt/casa/include/casa
)

## -----------------------------------------------------------------------------
## Check for the header files

FIND_LIBRARY(CASA_casa_LIBRARY casa
	PATHS ${CASA_LIBRARY_PATHS}
)

FIND_LIBRARY(CASA_scimath_LIBRARY scimath
	PATHS ${CASA_LIBRARY_PATHS}
)

FIND_LIBRARY(CASA_tables_LIBRARY tables
	PATHS ${CASA_LIBRARY_PATHS}
)

FIND_LIBRARY(CASA_measures_LIBRARY measures
	PATHS ${CASA_LIBRARY_PATHS}
)

FIND_LIBRARY(CASA_fits_LIBRARY fits
	PATHS ${CASA_LIBRARY_PATHS}
)

FIND_LIBRARY(CASA_lattices_LIBRARY lattices
	PATHS ${CASA_LIBRARY_PATHS}
)

FIND_LIBRARY(CASA_graphics_LIBRARY graphics
	PATHS ${CASA_LIBRARY_PATHS}
)

FIND_LIBRARY(CASA_coordinates_LIBRARY coordinates
	PATHS ${CASA_LIBRARY_PATHS}
)

## -----------------------------------------------------------------------------
## Due to fact, that the individual modules of the CASA libraries are
## interdependent, we need to take this into account when setting up the linker
## instructions

IF (CASA_casa_LIBRARY)
	SET (CASA_LIBRARIES "-lcasa")
	IF (CASA_scimath_LIBRARY)
		SET (CASA_LIBRARIES "-lscimath -lcasa")
	ENDIF (CASA_scimath_LIBRARY)
	IF (CASA_tables_LIBRARY)
		SET (CASA_LIBRARIES "-ltables -lcasa")
	ENDIF (CASA_tables_LIBRARY)
	IF (CASA_graphics_LIBRARY)
		SET (CASA_LIBRARIES "-lgraphics -lcasa")
	ENDIF (CASA_graphics_LIBRARY)
	IF (CASA_scimath_LIBRARY AND CASA_tables_LIBRARY)
		IF (CASA_measures_LIBRARY)
			SET (CASA_LIBRARIES "-lmeasures -ltables -lscimath -lcasa")
			IF (CASA_fits_LIBRARY)
				SET (CASA_LIBRARIES "-lfits -lmeasures -ltables -lscimath -lcasa")
				IF (CASA_coordinates_LIBRARY)
					SET (CASA_LIBRARIES "-lcoordinates -lfits -lmeasures -ltables -lscimath -lcasa")
				ENDIF (CASA_coordinates_LIBRARY)
			ENDIF (CASA_fits_LIBRARY)
		ENDIF (CASA_measures_LIBRARY)
	ENDIF (CASA_scimath_LIBRARY AND CASA_tables_LIBRARY)
ENDIF (CASA_casa_LIBRARY)

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
	SET (CASA_CXX_FLAGS "-DAIPS_${CASA_ARCH} -DAIPS_BIG_ENDIAN -DNATIVE_EXCP -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED -DSIGNBIT  -DAIPS_NO_TEMPLATE_SRC -DAIPS_NO_TEMPLATE_SRC -I${CASA_INCLUDE_DIR} -DAIPS_${CASA_ARCH} -Wno-deprecated -fno-implicit-templates")
	SET (CASA_CXX_LFLAGS "-L${CASA_LIBRARIES_DIR} ${CASA_LIBRARIES_DIR}/version.o -limages -lcomponents -lcoordinates -llattices -lfits -lmeasures -ltables -lscimath -lcasa")
	IF (NOT CASA_FIND_QUIETLY)
		MESSAGE (STATUS "Found CASA.")
		MESSAGE (STATUS "CASA architecture .. : ${CASA_ARCH}")
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
