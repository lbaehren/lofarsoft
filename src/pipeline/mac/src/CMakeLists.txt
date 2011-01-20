include(PythonInstall)

lofar_add_library(_ep_interface MODULE ep_interface.cc)
set_target_properties(_ep_interface PROPERTIES
  PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY ${PYTHON_BUILD_DIR}/lofar/pipeline)

lofar_add_library(_ep_echo MODULE ep_echo.cc eventwrappers.cc)
set_target_properties(_ep_echo PROPERTIES
  PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY ${PYTHON_BUILD_DIR}/lofar/pipeline)

lofar_add_library(_ep_control MODULE ep_control.cc)
set_target_properties(_ep_control PROPERTIES
  PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY ${PYTHON_BUILD_DIR}/lofar/pipeline)
