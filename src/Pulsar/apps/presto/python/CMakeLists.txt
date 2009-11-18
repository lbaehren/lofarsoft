## ==============================================================================
##
##  Handles the install of the presto pgplot bindings and python wrapper scripts.
##
## ==============================================================================

## Minimum required version of CMake to configure the project
cmake_minimum_required (VERSION 2.6)

configure_file (
    ${PULSAR_SOURCE_DIR}/apps/presto/python/setup.py.in
    ${PULSAR_SOURCE_DIR}/apps/presto/python/setup.py
)

add_custom_target (ppgplot_bindings_build
  COMMAND rm -rf ${CMAKE_BINARY_DIR}/apps/presto/python/*
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/ppgplot_src ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/presto_src ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/wrappers ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND cp ${CMAKE_SOURCE_DIR}/apps/presto/python/setup.py ${CMAKE_BINARY_DIR}/apps/presto/python/
  COMMAND ${PYTHON_EXECUTABLE} setup.py build --build-base=${CMAKE_BINARY_DIR}/apps/presto/python
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMENT "Running build using ${PYTHON_EXECUTABLE} setup.py build --build-base=${CMAKE_BINARY_DIR}/apps/presto/python"
  )

add_custom_target (ppgplot_bindings_install
  COMMAND rm -rf ${CMAKE_BINARY_DIR}/apps/presto/python/*
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/ppgplot_src ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/presto_src ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/wrappers ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND cp ${CMAKE_SOURCE_DIR}/apps/presto/python/setup.py ${CMAKE_BINARY_DIR}/apps/presto/python/
  COMMAND ${PYTHON_EXECUTABLE} setup.py install --home=${CMAKE_INSTALL_PREFIX}/lib/python
  WORKING_DIRECTORY ${PULSAR_BINARY_DIR}/apps/presto/python
  COMMENT "Installing project presto python bingings for ${PULSAR_BINARY_DIR}/apps/presto/python ..."
  )
add_dependencies (ppgplot_bindings_install presto_pyscripts_install presto)

add_custom_target (presto_fftfit
  COMMAND rm -rf ${CMAKE_BINARY_DIR}/apps/presto/python/fftfit_src
  COMMAND cp -r ${CMAKE_SOURCE_DIR}/apps/presto/python/fftfit_src ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMAND ${F2PY_EXECUTABLE} -c fftfit_src/fftfit.pyf fftfit_src/*.f 
  COMMAND cp fftfit.so ${CMAKE_INSTALL_PREFIX}/lib/python
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/apps/presto/python
  COMMENT "Executing ${F2PY_EXECUTABLE} -c ${CMAKE_BINARY_DIR}/apps/presto/python/fftfit_src/fftfit.pyf ${CMAKE_BINARY_DIR}/apps/presto/python/fftfit_src/*.f ..."
  COMMAND echo "Installed fftfit.so to ${CMAKE_INSTALL_PREFIX}/lib/python"
  )
#add_dependencies (presto_fftfit ppgplot_bindings_build)