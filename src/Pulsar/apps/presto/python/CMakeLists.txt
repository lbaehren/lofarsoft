## ==============================================================================
##
##  Handles the install of the presto pgplot bindings and python wrapper scripts.
##
## ==============================================================================

##____________________________________________________________________
## Generate setup.py script to configure package and adjust its
## permissions; by default the output of 'configure_file()' is
## read-only, but since modifications are required later, the file
## needs to be writable as well.

configure_file (
    ${PULSAR_SOURCE_DIR}/apps/presto/python/setup.py.in
    ${PULSAR_BINARY_DIR}/apps/presto/python/setup.py
)

file (COPY ${PULSAR_BINARY_DIR}/apps/presto/python/setup.py
  DESTINATION ${PULSAR_BINARY_DIR}/tmp/presto/python
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

##____________________________________________________________________
## Find the previously generated setup script; the path is made
## available through the PRESTO_SETUP_PY variable, which in turn can
## be used with all of the subsequent steps.

find_file (PRESTO_SETUP_PY
  NAMES setup.py
  PATHS ${PULSAR_BINARY_DIR}
  PATH_SUFFIXES tmp/presto/python
  )

if (NOT PRESTO_SETUP_PY)
  message (WARNING "[Presto] Unable to configure and build ppgplot - missing setup.py")
endif (NOT PRESTO_SETUP_PY)

##____________________________________________________________________
## Generate the Python bindings

add_custom_target (ppgplot_bindings_build
  COMMAND rm -rf ${CMAKE_BINARY_DIR}/apps/presto/python/*
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/ppgplot_src ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/presto_src ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/wrappers ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND export PYTHONPATH=${PULSAR_PYTHONPATH}
  COMMAND ${PYTHON_EXECUTABLE} ${PRESTO_SETUP_PY} build --build-base=${CMAKE_BINARY_DIR}/apps/presto/python
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMENT "Running build using ${PYTHON_EXECUTABLE} ${PRESTO_SETUP_PY} build --build-base=${CMAKE_BINARY_DIR}/apps/presto/python"
  )

add_custom_target (ppgplot_bindings_install
  COMMAND rm -rf ${CMAKE_BINARY_DIR}/apps/presto/python/*
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/ppgplot_src ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/presto_src ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/wrappers ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND export PYTHONPATH=${PULSAR_PYTHONPATH}
  COMMAND ${PYTHON_EXECUTABLE} ${PRESTO_SETUP_PY} install --home=${CMAKE_INSTALL_PREFIX}
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/presto/python
  COMMENT "Installing project presto python bingings for ${PULSAR_BINARY_DIR}/apps/presto/python ..."
  )
add_dependencies (ppgplot_bindings_install presto_pyscripts_install presto)

add_custom_target (presto_fftfit
  COMMAND rm -rf ${CMAKE_BINARY_DIR}/apps/presto/python/fftfit_src
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/fftfit_src ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND export PYTHONPATH=${PULSAR_PYTHONPATH}
  COMMAND ${F2PY_EXECUTABLE} -c fftfit_src/fftfit.pyf fftfit_src/*.f 
  COMMAND cp fftfit.so ${CMAKE_INSTALL_PREFIX}/lib/python
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMENT "Executing ${F2PY_EXECUTABLE} -c ${CMAKE_BINARY_DIR}/apps/presto/python/fftfit_src/fftfit.pyf ${CMAKE_BINARY_DIR}/apps/presto/python/fftfit_src/*.f ..."
  COMMAND echo "Installed fftfit.so to ${CMAKE_INSTALL_PREFIX}/lib/python"
  )
#add_dependencies (presto_fftfit ppgplot_bindings_build)
