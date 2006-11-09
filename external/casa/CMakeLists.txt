PROJECT (CASA)

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
## Check for platform specific header files

SET (INCLUDE_DIRS "/usr/include /usr/local/include /sw/include")

INCLUDE (CheckIncludeFiles)

CHECK_INCLUDE_FILES (stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILES (malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILES (cstring HAVE_CSTRING)

IF (NOT HAVE_CSTRING)
  FIND_PATH (HAVE_CSTRING
    cstring
    PATHS ${INCLUDE_DIRS}
    PATH_SUFFIXES c++ c++/4.0.0 g++
  )
ENDIF (NOT HAVE_CSTRING)

MESSAGE (STATUS "HAVE_STDLIB_H = ${HAVE_STDLIB_H}")
MESSAGE (STATUS "HAVE_MALLOC_H = ${HAVE_MALLOC_H}")
MESSAGE (STATUS "HAVE_CSTRING  = ${HAVE_CSTRING}")

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
## Optimized compile command is:
##
##  $(C++) $(CPPOPT) $(AIPSINCL) -I. $(C++OPT) -c ???.cc
##  

IF (APPLE)
   SET (CASA_CPPSTD "-Wno-deprecated -DAIPS_DARWIN -DAIPS_BIG_ENDIAN -DAIPS_STDLIB -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED -DAIPS_AUTO_STL")
   SET (CASA_CXXSTD "-pipe -Wall -Wno-non-template-friend -Woverloaded-virtual -Wcast-align -Wno-comment -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE")
ELSE (APPLE)
     ADD_DEFINITIONS (-DAIPS_LINUX -DAIPS_LITTLE_ENDIAN -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED -DAIPS_NO_TEMPLATE_SRC -DAIPS_NO_TEMPLATE_SRC)
     SET (CASA_CXX_FLAGS "-Wno-deprecated -DAIPS_LINUX -DAIPS_LITTLE_ENDIAN -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED   -DAIPS_NO_TEMPLATE_SRC -DAIPS_NO_TEMPLATE_SRC -O3 -fPIC  -pipe -Wall -Wno-non-template-friend -Woverloaded-virtual -Wmissing-prototypes -Wno-comment -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -c")
     SET (CASA_CPPSTD "-Wno-deprecated  -DAIPS_LINUX -DAIPS_LITTLE_ENDIAN -DAIPS_STDLIB -DAIPS_AUTO_STL -DAIPS_NO_LEA_MALLOC -D_GLIBCPP_DEPRECATED   -DAIPS_NO_TEMPLATE_SRC -DAIPS_NO_TEMPLATE_SRC")
     SET (CASA_CXXSTD "-O3 -fPIC  -pipe -Wall -Wno-non-template-friend -Woverloaded-virtual -Wmissing-prototypes -Wno-comment -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -c")
     SET (CASA_LDOPT "-s -Xlinker -rpath -Xlinker /dop71_0/aips++/weekly/linux_gnu/lib:/usr/lib:/usr/X11R6/lib:/usr/X11R6/lib
")
ENDIF (APPLE)

SET (CMAKE_C_FLAGS "${CASA_CPPSTD} ${CMAKE_C_FLAGS}")
SET (CMAKE_CXX_FLAGS "${CASA_CXXSTD} ${CMAKE_CXX_FLAGS}")
SET (CMAKE_MODULE_LINKER_FLAGS "${CASA_LDOPT} ${CMAKE_MODULE_LINKER_FLAGS}")

MESSAGE( STATUS "CMAKE_BUILD_TYPE ........ = ${CMAKE_BUILD_TYPE}" )
MESSAGE (STATUS "CMAKE_C_FLAGS ........... = ${CMAKE_C_FLAGS}")
MESSAGE (STATUS "CMAKE_CXX_FLAGS ......... = ${CMAKE_CXX_FLAGS}")
MESSAGE (STATUS "CMAKE_MODULE_LINKER_FLAGS = ${CMAKE_MODULE_LINKER_FLAGS}")

## -----------------------------------------------------------------------------
## List of sub-directories

SUBDIRS (implement)
