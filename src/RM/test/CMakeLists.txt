
##_______________________________________________________________________________
##                                                           List of source files

set (rm_tests
  tRMSim.cpp
  trmClean.cpp
  trmParallel.cpp
  )

if (HAVE_ITPP AND RM_WITH_ITPP)
  list (APPEND rm_tests trmnoise.cpp)
  list (APPEND rm_tests tWienerFilter.cpp)
  list (APPEND rm_tests tWienerFilter-FourierTransform.cpp)
  list (APPEND rm_tests tWienerFilter-SimpleSignal.cpp)
endif (HAVE_ITPP AND RM_WITH_ITPP)

if (HAVE_FFTW3)
  list (APPEND rm_tests tPreshift.cpp)
endif (HAVE_FFTW3)

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
add_test (trmParallel trmParallel)

if (HAVE_ITPP AND RM_WITH_ITPP)
  add_test (trmnoise trmnoise)
  add_test (tWienerFilter tWienerFilter)
endif (HAVE_ITPP AND RM_WITH_ITPP)

if (trmClean_gaussian)
  add_test (trmClean trmClean ${trmClean_gaussian})
else (trmClean_gaussian)
  add_test (trmClean trmClean)
endif (trmClean_gaussian)

if (HAVE_FFTW3)
  add_test (tPreshift tPreshift)
endif (HAVE_FFTW3)

