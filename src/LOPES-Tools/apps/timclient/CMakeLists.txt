PROJECT (LOPES-Tools)

FIND_FILE (TYPES_H types.h PATHS /usr/include /usr/local/include PATH_SUFFIXES machine)

IF (TYPES_H)
	MESSAGE (STATUS "Found types.h")
	STRING (REGEX REPLACE /types.h "" TYPES_H ${TYPES_H})
ELSE (TYPES_H)
	MESSAGE (STATUS "Unable to locate types.h")
ENDIF (TYPES_H)

INCLUDE_DIRECTORIES (. .. ${TYPES_H})

#ADD_LIBRARY (timu timu.cc)

#ADD_EXECUTABLE (timclient timclient.cc timu.cc)
