PROJECT (LopesTools)

INCLUDE_DIRECTORIES (. .. ../..)

ADD_EXECUTABLE (tProgressBar tProgressBar.cc)
TARGET_LINK_LIBRARIES (tProgressBar lopestools)

ADD_EXECUTABLE (tTimestamp tTimestamp.cc)
TARGET_LINK_LIBRARIES (tTimestamp lopestools)
