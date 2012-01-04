
## ==============================================================================
##
##  Locate required components
##
## ==============================================================================

find_path (PRESTO_H
  NAMES presto.h
  PATHS ${PULSAR_SOURCE_DIR}
  PATH_SUFFIXES apps/presto/include apps/presto/python/wrappers
  )

if (PRESTO_H)
  include_directories (${PRESTO_H})
endif (PRESTO_H)

if (PYTHON_INCLUDES)
  include_directories (${PYTHON_INCLUDES})
endif (PYTHON_INCLUDES)

## ==============================================================================
##
##  Wrapper generation
##
## ==============================================================================

set_source_files_properties (presto.i PROPERTIES SWIG_FLAGS "-shadow")
set_source_files_properties (presto.i PROPERTIES SWIG_FLAGS "-make_default")

swig_add_module (presto python presto.i presto_wrap.c)

swig_link_libraries (presto ${PYTHON_LIBRARIES})

## ==============================================================================
##
##  Installation
##
## ==============================================================================

# cp presto.py ../presto_src/prestoswig.py
# cp presto_wrap.c ../presto_src/
