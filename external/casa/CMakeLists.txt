PROJECT (CASA)

## -----------------------------------------------------------------------------
## Check for the header files first, as from this we can derive a number of 
## fundamental CASA variables (such as e.g. the root directory of the
## installation)

find_path (CASA_INCLUDE_DIR aips.h
  PATHS
  /casa
  /opt/casa
  /sw/share/casa
  PATH_SUFFIXES
  code/include/casa
  stable/code/include/casa
  weekly/code/include/casa
  )

IF (CASA_INCLUDE_DIR)
  SET (HAVE_AIPS_H true)
  STRING (REGEX REPLACE include/casa include CASA_INCLUDE_DIR ${CASA_INCLUDE_DIR})
  STRING (REGEX REPLACE /code/include "" AIPSROOT ${CASA_INCLUDE_DIR})
ENDIF (CASA_INCLUDE_DIR)

## -----------------------------------------------------------------------------
## Check for the platform

IF (UNIX)
  IF (APPLE)
    MESSAGE (STATUS "System platform = APPLE")
    MESSAGE (STATUS "Architecture    = ${CMAKE_OSX_ARCHITECTURES}")
    SET (AIPSARCH "darwin")
    SET (AIPS_DARWIN 1)
  ELSE (APPLE)
    MESSAGE (STATUS "System platform = UNIX")
  ENDIF (APPLE)
ENDIF (UNIX)

## -----------------------------------------------------------------------------
## Check for platform specific header files; in most cases we should be able to
## use the CMake build-in macro, but in a number of cases those will not yield
## proper results - where therefore run an explicit check.

SET (INCLUDE_DIRS "/usr/include /usr/local/include /sw/include")

INCLUDE (CheckIncludeFiles)

CHECK_INCLUDE_FILES (stdlib.h HAVE_STDLIB_H)

FIND_PATH (HAVE_MALLOC_H malloc.h
  PATHS ${INCLUDE_DIRS}
  PATH_SUFFIXES sys
  )

FIND_PATH (HAVE_CSTRING
  cstring
  PATHS ${INCLUDE_DIRS}
  PATH_SUFFIXES c++ c++/4.0.0 g++
  )

MESSAGE (STATUS "HAVE_STDLIB_H = ${HAVE_STDLIB_H}")
MESSAGE (STATUS "HAVE_MALLOC_H = ${HAVE_MALLOC_H}")
MESSAGE (STATUS "HAVE_CSTRING  = ${HAVE_CSTRING}")

## -----------------------------------------------------------------------------
## Required external libraries

FIND_PACKAGE (X11)

## -----------------------------------------------------------------------------
## Compiler settings
##
## The following command is used when building the CASA library from source,
## using the internal build environment:
## 
##  c++  -DAIPS_STDLIB -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP
##       -DAIPS_NO_TEMPLATE_SRC -DAIPS_NO_TEMPLATE_SRC 
##       -I/sw/share/casa/code/include -I. -O2 -fPIC  -fexceptions -pipe -Wall
##       -Wno-non-template-friend -Woverloaded-virtual -Wcast-align -Wno-comment
##       -pipe -fno-implicit-templates -Wreturn-type -Wimplicit -fPIC -c ???.cc;
##
## The following settings have been extracted from a 'makedefs' file used on an
## Intel Mac:
##
##  CPPSTD  += -DAIPS_LINUX -DAIPS_BIG_ENDIAN -DAIPS_STDLIB -DAIPS_NO_LEA_MALLOC
##             -D_GLIBCPP_DEPRECATED -DAIPS_AUTO_STL
##  CPPOPT  += -DAIPS_NO_TEMPLATE_SRC
##  CC      := $(C++ROOT)/bin/gcc
##  CSTD    += -DHAVE_LINUX_GLIBC -Wall
##  COPTLVL := -O2 -fno-strength-reduce
##  C++     := $(C++ROOT)/bin/g++ -Wno-deprecated
##  C++STD  += -pipe -Wall -Wno-non-template-friend -Woverloaded-virtual
##             -Wmissing -prototypes -Wcast-align -Wno-comment
##             -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
##  LDSTD   := -Xlinker -rpath -Xlinker
##             $(AIPSARCH)/lib:$(C++ROOT)/lib:$(MotifLIBD):$(X11LIBD)
##
## Optimized compile command within a package (e.g. casa/implement) is:
##
##  $(C++) $(CPPOPT) $(AIPSINCL) -I. $(C++OPT) -c ???.cc
##
## Thus by substitution of the above listed variables, we get
## 
##   $(C++ROOT)/bin/g++ -Wno-deprecated -DAIPS_NO_TEMPLATE_SRC -I.
##  

IF (UNIX)
  SET (CASA_LDSTD "-Xlinker -rpath -Xlinker")
  SET (CASA_TEMPLATES "diy")
  IF (APPLE)
    # Preprocessor flags for system debug/optimized compilations.
    SET (CASA_CPPSTD "-DAIPS_STDLIB")
    SET (CASA_CPPDBG "-DAIPS_STDLIB -DAIPS_DEBUG -DAIPS_ARRAY_INDEX_CHECK -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP -DAIPS_ARRAY_INDEX_CHECK")
    SET (CASA_CPPOPT "-DAIPS_STDLIB -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP")
    # C++ compiler flags for system debug/optimized compilations.
    SET (CASA_CXXSTD "-dynamic -pipe -fPIC -Wall -Wno-non-template-friend -Woverloaded-virtual -Wcast-align -Wno-comment")
    SET (CASA_CXXDBGLVL "-g")
    SET (CASA_CXXDBG "${CASA_CXXDBGLVL} -pipe -fno-implicit-templates -Wreturn-type -Wimplicit -fPIC")
    SET (CASA_CXXOPTLVL "-O2")
    SET (CASA_CXXOPT "${CASA_CXXOPTLVL} -fPIC  -pipe -fno-implicit-templates -Wreturn-type -Wimplicit -fPIC")
  ELSE (APPLE)
    # Preprocessor flags for system debug/optimized compilations.
    SET (CASA_CPPSTD "")
    SET (CASA_CPPDBG "")
    SET (CASA_CPPOPT "")
    # C++ compiler flags for system debug/optimized compilations.
    SET (CASA_CXXDBGLVL "")
    SET (CASA_CXXDBG "${CASA_CXXDBGLVL}")
    SET (CASA_CXXOPTLVL "")
    SET (CASA_CXXOPT "${CASA_CXXOPTLVL}")
  ENDIF (APPLE)
ENDIF (UNIX)
  
MESSAGE (STATUS "Preprocessor flags for system debug/optimized compilations")
MESSAGE (STATUS " CPPSTD = ${CASA_CPPSTD}")
MESSAGE (STATUS " CPPDBG = ${CASA_CPPDBG}")
MESSAGE (STATUS " CPPOPT = ${CASA_CPPOPT}")
MESSAGE (STATUS "C++ compiler flags for system debug/optimized compilations")
MESSAGE (STATUS " C++DBGLVL = ${CASA_CXXDBGLVL}")
MESSAGE (STATUS " C++DBG    = ${CASA_CXXDBG}")
MESSAGE (STATUS " C++OPTLVL = ${CASA_CXXOPTLVL}")
MESSAGE (STATUS " C++OPT    = ${CASA_CXXOPT}")

## Now that all the platform-specific settings have been done, we can assign
## the variables internally used by CMake

SET (CMAKE_C_FLAGS "${CASA_CPPSTD} ${CMAKE_C_FLAGS}")
SET (CMAKE_CXX_FLAGS "${CASA_CPPOPT} ${CASA_CXXOPT} ${CMAKE_CXX_FLAGS}")
SET (CMAKE_MODULE_LINKER_FLAGS "${CASA_LDSTD} ${CMAKE_MODULE_LINKER_FLAGS}")

MESSAGE (STATUS "CMake-internal compilation and linkage variables")
MESSAGE (STATUS " CMAKE_BUILD_TYPE ........ = ${CMAKE_BUILD_TYPE}" )
MESSAGE (STATUS " CMAKE_C_FLAGS ........... = ${CMAKE_C_FLAGS}")
MESSAGE (STATUS " CMAKE_CXX_FLAGS ......... = ${CMAKE_CXX_FLAGS}")
MESSAGE (STATUS " CMAKE_MODULE_LINKER_FLAGS = ${CMAKE_MODULE_LINKER_FLAGS}")

## -----------------------------------------------------------------------------
## List of sub-directories

SUBDIRS (implement)
