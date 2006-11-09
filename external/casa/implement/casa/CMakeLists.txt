## CASA/implement/casa

PROJECT (CASA)

## -----------------------------------------------------------------------------

INCLUDE_DIRECTORIES (. .. ../.. ${CASA_SOURCE_DIR}/implement)

SUBDIRS (
	Arrays
	BasicSL
	Containers
	Exceptions
	IO
	Inputs
	Logging
	OS
	Quanta
	System
	Utilities
	test
)

## -----------------------------------------------------------------------------
## libcasa

ADD_LIBRARY (casa
# local versions of the standard header files
	complex.h
	fstream.h
	typeinfo.h
	vector.h
# basic AIPS++/CASA header file (include by all others)
	aips.cc
	aipstype.h
	aipsxtype.h
	version2.cc
)
