
PROJECT (CASA_scimath)

INCLUDE (${CASA_SOURCE_DIR}/../../devel_common/cmake/FindLAPACK.cmake)

subdirs (
  fortran
  implement
  )
