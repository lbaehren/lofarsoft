PROJECT (LopesTools)

INCLUDE_DIRECTORIES (. .. ../..)

ADD_EXECUTABLE (tHermite1D tHermite1D.cc)
TARGET_LINK_LIBRARIES (tHermite1D lopestools)

ADD_EXECUTABLE (tShapelets1D tShapelets1D.cc)
TARGET_LINK_LIBRARIES (tShapelets1D lopestools)

ADD_EXECUTABLE (tShapelets2D tShapelets2D.cc)
TARGET_LINK_LIBRARIES (tShapelets2D lopestools)
