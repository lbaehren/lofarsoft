
## ------------------------------------------------------------------------------
## Find external packages

find_path (POPT_H popt.h
  PATH /usr/include /usr/local/include /sw/include
  )

if (POPT_H)
  message (STATUS "popt.h = ${POPT_H}")
else (POPT_H)
  message (STATUS "Unable to find popt.h")
endif (POPT_H)

## ------------------------------------------------------------------------------
## Include directories

include_directories (
  .
  ..
  ../../implement
  ${POPT_H}
  ${CASA_INCLUDE_DIR}
  )

## ------------------------------------------------------------------------------
## create executable

add_executable (trigrun trigrun.cc)
target_link_libraries (trigrun lopes ${CASA_LIBRARY})