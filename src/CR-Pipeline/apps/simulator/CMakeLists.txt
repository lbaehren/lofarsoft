
project (simulation)


## -----------------------------------------------------------------------------
## external dependencies

include (../../../../devel_common/cmake/FindROOT.cmake)

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
