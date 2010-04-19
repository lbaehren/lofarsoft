
##_______________________________________________________________________________
##                                                           List of source files

file (GLOB rm_tests *.cc *.cpp)

if (NOT HAVE_ITPP OR NOT RM_ENABLE_ITPP)
  list (REMOVE_ITEM rm_tests ${RM_SOURCE_DIR}/test/trmnoise.cpp)
  list (REMOVE_ITEM rm_tests ${RM_SOURCE_DIR}/test/tWienerFilter.cpp)
  list (REMOVE_ITEM rm_tests ${RM_SOURCE_DIR}/test/tWienerFilter-FourierTransform.cpp)
  list (REMOVE_ITEM rm_tests ${RM_SOURCE_DIR}/test/tWienerFilter-SimpleSignal.cpp)
endif (NOT HAVE_ITPP OR NOT RM_ENABLE_ITPP)

if (NOT HAVE_FFTW3)
  list (REMOVE_ITEM rm_tests ${CMAKE_CURRENT_SOURCE_DIR}/tPreshift.cpp)
endif (NOT HAVE_FFTW3)

##_______________________________________________________________________________
##                                                     Build/Install instructions

foreach (src ${rm_tests})
  ## get the name of the file without the extension
  get_filename_component (src ${src} NAME_WE)
  get_filename_component (extension ${src} EXT)
  ## compiler instructions
  add_executable(${src} ${src}${extension})
  ## linker instructions
  target_link_libraries(${src} rm ${rm_link_libraries})
  ## installation instructions
  install (TARGETS ${src}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    )
endforeach (src)

##_______________________________________________________________________________
##                                                           Testing instructions

## Locate test input files

find_file (trmClean_gaussian gaussian.dat ${RM_SOURCE_DIR}/data)

## Run the tests

add_test (tRMSim tRMSim)

if (HAVE_ITPP AND RM_ENABLE_ITPP)
  add_test (trmnoise trmnoise)
  add_test (tWienerFilter tWienerFilter)
endif (HAVE_ITPP AND RM_ENABLE_ITPP)

if (trmClean_gaussian)
  add_test (trmClean trmClean ${trmClean_gaussian})
else (trmClean_gaussian)
  add_test (trmClean trmClean)
endif (trmClean_gaussian)

if (HAVE_FFTW3)
  add_test (tPreshift tPreshift)
endif (HAVE_FFTW3)

