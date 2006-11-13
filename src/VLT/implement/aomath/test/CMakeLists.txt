PROJECT (VLT)

INCLUDE_DIRECTORIES (. .. ${VLT_SOURCE_DIR}/implement)

ADD_EXECUTABLE (matvecTest matvecTest.cc)
TARGET_LINK_LIBRARIES (matvecTest aomath)

ADD_EXECUTABLE (svdTest svdTest.cc)
TARGET_LINK_LIBRARIES (svdTest aomath)

ADD_EXECUTABLE (testMath testMath.cc)
TARGET_LINK_LIBRARIES (testMath aomath)

ADD_EXECUTABLE (tMAFilter tMAFilter.cc)
TARGET_LINK_LIBRARIES (tMAFilter aomath)
