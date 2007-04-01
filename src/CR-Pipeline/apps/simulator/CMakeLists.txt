
project (simulation)


## -----------------------------------------------------------------------------
## external dependencies

if (NOT HAVE_ROOT)
  include (../../../../devel_common/cmake/FindROOT.cmake)
endif (NOT HAVE_ROOT)

include_directories (
  .
  ${ROOT_INCLUDES}
)

## -----------------------------------------------------------------------------
## library

add_library (simulator
  mathclasses.cc
  )

## -----------------------------------------------------------------------------
## executables

add_executable (sim_main main.cc)

target_link_libraries (sim_main simulator)
