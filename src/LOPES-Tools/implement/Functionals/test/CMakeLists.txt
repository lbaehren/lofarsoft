PROJECT (LOPES-Tools)

include_directories (. .. ../..)

add_executable (tHermite1D tHermite1D.cc)
target_link_libraries (tHermite1D lopestools)

add_executable (tShapelets1D tShapelets1D.cc)
target_link_libraries (tShapelets1D lopestools)

add_executable (tShapelets2D tShapelets2D.cc)
target_link_libraries (tShapelets2D lopestools)

add_executable (tStatisticsFilter tStatisticsFilter.cc)
target_link_libraries (tStatisticsFilter
  lopestools
  ${CASA_LIBRARY}
  ${SCIMATH_LIBRARY}
  )
