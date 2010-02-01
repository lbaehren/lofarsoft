##------------------------------------------------------------------------------
## $Id:: CMakeLists.txt 1865 2008-08-08 14:51:40Z baehren                      $
##------------------------------------------------------------------------------

## -----------------------------------------------------------------------------
## Build the library

enable_language(Fortran)

## collect the source files in the modules

file (GLOB MODULE_SCIMATH_F_SOURCES *.f *.c)

## compiler instructions

if (CASACORE_ENABLE_SHARED)
  add_library (casa_scimath_f SHARED ${MODULE_SCIMATH_F_SOURCES})
else (CASACORE_ENABLE_SHARED)
  add_library (casa_scimath_f STATIC ${MODULE_SCIMATH_F_SOURCES})
endif (CASACORE_ENABLE_SHARED)

## linker instructions

if (APPLE)
  if (CMAKE_SYSTEM_KERNEL MATCHES x86_x64)
    set_target_properties (casa_scimath_f
      PROPERTIES
      COMPILE_FLAGS "-fPIC -O2 -fPIC -m64"
      LINKER_LANGUAGE Fortran
      )
  else (CMAKE_SYSTEM_KERNEL MATCHES x86_x64)
    if (CMAKE_SYSTEM_KERNEL MATCHES i386)
      set_target_properties (casa_scimath_f
	PROPERTIES
	COMPILE_FLAGS "-fPIC -O2 -fPIC -m32"
	LINKER_LANGUAGE Fortran
	)
    endif (CMAKE_SYSTEM_KERNEL MATCHES i386)
  endif (CMAKE_SYSTEM_KERNEL MATCHES x86_x64)
else (APPLE)
  set_target_properties (casa_scimath_f
    PROPERTIES
    COMPILE_FLAGS "-fPIC -O2 -fPIC"
    LINKER_LANGUAGE Fortran
    )
endif (APPLE)

## -----------------------------------------------------------------------------
## Installation

## library

install (TARGETS casa_scimath_f
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )
