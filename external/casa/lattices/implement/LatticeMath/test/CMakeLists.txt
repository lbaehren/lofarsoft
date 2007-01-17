
PROJECT (CASA_lattices_tLatticeMath)

## ------------------------------------------------------------------------------
## Set up the list with test program sources

SET (tests
  tFit2D
  tLatticeFit
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} lattices)
ENDFOREACH (src)
