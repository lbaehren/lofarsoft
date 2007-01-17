
PROJECT (CASA_lattices_tLattices)

## ------------------------------------------------------------------------------
## Set up the list with test program sources

SET (tests
  dLattice
  dPagedArray
  tArrayLattice
  tCurvedLattice2D
  tExtendLattice
  tLCComplement
  tLCConcatenation
  tLCDifference
  tLCExtension
  tLCIntersection
  tLCLELMask
  tLCMask
  tLCPagedMask
  tLCPixelSet
  tLCPolygon
  tLCRegion
  tLCSlicer
  tLCStretch
  tLCUnion
  tLEL
  tLELAttribute
  tLELMedian
  tLatticeAddNoise
  tLatticeApply
  tLatticeApply2
  tLatticeCache
  tLatticeConcat
  tLatticeConvolver
  tLatticeExpr
  tLatticeExpr2
  tLatticeExpr2Node
  tLatticeExpr3
  tLatticeExpr3Node
  tLatticeExprNode
  tLatticeFFT
  tLatticeFractile
  tLatticeHistograms
  tLatticeIndexer
  tLatticeIterator
  tLatticeLocker
  tLatticeRegion
  tLatticeSlice1D
  tLatticeStatistics
  tLatticeStepper
  tLatticeTwoPtCorr
  tLatticeUtilities
  tPagedArray
  tPixelCurve1D
  tRebinLattice
  tSubLattice
  tTempLattice
  tTileStepper
  tTiledLineStepper
  tTiledShape
  )

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} lattices)
ENDFOREACH (src)
