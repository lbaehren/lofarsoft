PROJECT (CASA)

INCLUDE (CPack)

## -----------------------------------------------------------------------------
## Check for the platform

IF (UNIX)
  IF (APPLE)
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

INCLUDE (CheckIncludeFiles)

#CHECK_INCLUDE_FILES (stdlib.h HAVE_STDLIB_H)

#FIND_PATH (HAVE_MALLOC_H malloc.h
#  PATHS ${INCLUDE_DIRS}
#  PATH_SUFFIXES sys
#  )

find_path (HAVE_STRING_H
  string.h
  PATHS /usr/include /usr/local/include /sw/include
#  PATH_SUFFIXES c++ c++/4.0.0 g++
  )

## -----------------------------------------------------------------------------
## Required external libraries

find_package (X11)

## -----------------------------------------------------------------------------
## Compiler settings
##
## The following command is used when building the CASA library from source,
## using the internal build environment:
##
## (a) Darwin - Mac OS X
## 
##  c++  -DAIPS_STDLIB -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DNATIVE_EXCP
##       -DAIPS_NO_TEMPLATE_SRC -DAIPS_NO_TEMPLATE_SRC 
##       -I/sw/share/casa/code/include -I. -O2 -fPIC  -fexceptions -pipe -Wall
##       -Wno-non-template-friend -Woverloaded-virtual -Wcast-align -Wno-comment
##       -pipe -fno-implicit-templates -Wreturn-type -Wimplicit -fPIC -c ???.cc;
##
## (b) Debian GNU/Linux
## 
##  g++ -Wno-deprecated -DAIPS_LINUX -DAIPS_LITTLE_ENDIAN -DAIPS_STDLIB
##      -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED -DAIPS_AUTO_STL
##      -DAIPS_NO_TEMPLATE_SRC -DAIPS_NO_TEMPLATE_SRC
##      -I/opt/casa/stable/code/include -I. -O2 -fPIC -pipe -Wall
##      -Wno-non-template-friend -Woverloaded-virtual -Wmissing-prototypes
##      -Wcast-align -Wno-comment -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
##      -c ???.cc;
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

## Start with the generic information common to all the platforms

include_directories (
  .
  ${CASA_SOURCE_DIR}
  ${CASA_SOURCE_DIR}/include
  )

set (CASA_OPTLEVEL "-O3")

## Platform-dependent compiler/linker settings

if (UNIX)
  ## Compiler flags
  add_definitions (
    -DAIPS_STDLIB
    -DAIPS_NO_TEMPLATE_SRC
    -DAIPS_AUTO_STL
    -DAIPS_NO_LEA_MALLOC
    )
  ## Linker flags
  set (CASA_LDSTD "-Xlinker -rpath -Xlinker")
  if (APPLE)
    add_definitions (
      -DAIPS_DARWIN
      -DAIPS_BIG_ENDIAN
      -DNATIVE_EXCP
      )
    set (CASA_CXXSTD "${CASA_CXXSTD} -fno-implicit-templates -Wreturn-type -Wimplicit -fPIC")
  else (APPLE)
    add_definitions (
      -DAIPS_LINUX
      -DAIPS_LITTLE_ENDIAN
      -D_LARGEFILE_SOURCE
      )
  endif (APPLE)
  add_definitions (
    ${CASA_OPTLEVEL}
    -fPIC
    -pipe
    -Wall
    -Wno-comment
    -fexceptions
    -Wcast-align
    -Wno-non-template-friend
    -Woverloaded-virtual
    )
endif (UNIX)

## Now that all the platform-specific settings have been done, we can assign
## the variables internally used by CMake

set (CMAKE_VERBOSE_MAKEFILE 0)

set (CMAKE_C_FLAGS "${CASA_OPTLEVEL} ${CASA_CXXSTD}")
SET (CMAKE_MODULE_LINKER_FLAGS "${CASA_LDSTD}")

message (STATUS "CASA_SOURCE_DIR ......... = ${CASA_SOURCE_DIR}")
message (STATUS "CMAKE_SYSTEM_NAME ....... = ${CMAKE_SYSTEM_NAME}")
message (STATUS "CMAKE_SYSTEM_PROCESSOR .. = ${CMAKE_SYSTEM_PROCESSOR}")
message (STATUS "CMAKE_C_FLAGS ........... = ${CMAKE_C_FLAGS}")
message (STATUS "CMAKE_CXX_FLAGS ......... = ${CMAKE_CXX_FLAGS}")
message (STATUS "CMAKE_MODULE_LINKER_FLAGS = ${CMAKE_MODULE_LINKER_FLAGS}")

## -----------------------------------------------------------------------------
## List of sub-directories

subdirs (
  casa
  scimath
  tables
  measures
  ms
  )
