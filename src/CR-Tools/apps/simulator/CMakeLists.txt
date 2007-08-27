##------------------------------------------------------------------------
## $Id::                                                                 $
##------------------------------------------------------------------------

project (simulation)


## -----------------------------------------------------------------------------
## external dependencies

set (lib_locations
  /usr/lib
  /usr/local/lib
  /sw/lib
  )

if (NOT HAVE_ROOT)
  include (../../../../devel_common/cmake/FindROOT.cmake)
endif (NOT HAVE_ROOT)

find_library (libshift shift ${lib_locations})
find_library (libnsl nsl ${lib_locations})
find_library (libcrypt crypt ${lib_locations})
find_library (libdl dl ${lib_locations})
find_library (libm m ${lib_locations})

include_directories (
  .
  ${ROOT_INCLUDES}
)

## -----------------------------------------------------------------------------
## library

add_library (simulator
  mathclasses.cc
  )

target_link_libraries (simulator
  ${libdl}
  ${libm}
  ${ROOT_LIBRARIES}
  )

## -----------------------------------------------------------------------------
## executables

add_executable (sim_main main.cc)

target_link_libraries (sim_main
  ${libdl}
  ${libm}
  ${ROOT_LIBRARIES}
  simulator
  )

