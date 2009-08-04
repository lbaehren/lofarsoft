
## Create temporary CMakeLists.txt for the test program

  file (WRITE ${PROJECT_BINARY_DIR}/CMakeTmp/CMakeLists.txt
    "project (TestForPythonBindings)
     cmake_minimum_required(VERSION 2.5)
     include_directories (${PYTHON_INCLUDES})
     include_directories (${BOOST_INCLUDES})
     include_directories (${NUMPY_INCLUDES})
     include_directories (${NUM_UTIL_INCLUDES} ${NUM_UTIL_INCLUDES}/num_util)
     ADD_EXECUTABLE(TestForPythonBindings ${CMAKE_MODULE_PATH}/TestForPythonBindings.cc)
     TARGET_LINK_LIBRARIES(TestForPythonBindings ${PYTHON_LIBRARIES} ${NUM_UTIL_LIBRARIES})"
    )
  
## Try to compile the test program

try_compile (ENABLE_PYTHON_BINDINGS
  ${PROJECT_BINARY_DIR}/CMakeTmp
  ${PROJECT_BINARY_DIR}/CMakeTmp
  TestForPythonBindings
  )
