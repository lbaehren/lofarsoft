
##_______________________________________________________________________________
## Find components of required libraries

if (NOT HAVE_BOOST)
  include (FindBoost)
endif (NOT HAVE_BOOST)

if (NOT HAVE_PYTHON)
  include (FindPython)
endif (NOT HAVE_PYTHON)

##_______________________________________________________________________________
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
  
##_______________________________________________________________________________
## Try to compile the test program

if (HAVE_PYTHON)
  
  try_compile (ENABLE_PYTHON_BINDINGS
    ${PROJECT_BINARY_DIR}/CMakeTmp
    ${PROJECT_BINARY_DIR}/CMakeTmp
    TestForPythonBindings
    )
  
else (HAVE_PYTHON)
  
  message (STATUS "Unable to generate Python bindings - missing Python!")

endif (HAVE_PYTHON)
