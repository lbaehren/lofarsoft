## CASA/implement/lattices

PROJECT (CASA_lattices_liblattices)

## -----------------------------------------------------------------------------
## Directories containing the test programs

subdirs (
  LatticeMath/test
  Lattices/test
  )

## -----------------------------------------------------------------------------
## Files for template instantiation

subdirs (../../templates)

## -----------------------------------------------------------------------------
## liblattices

add_library (lattices
  LatticeMath/Fit2D.cc
  LatticeMath/LatticeFit.cc
  Lattices/ArrayLattice.cc
  Lattices/CLInterpolator2D.cc
  Lattices/CLIPNearest2D.cc
  Lattices/CurvedLattice2D.cc
  Lattices/ExtendLattice.cc
  Lattices/FITSMask.cc
  Lattices/LatticeAddNoise.cc
  Lattices/LatticeApply.cc
  Lattices/LatticeBase.cc
  Lattices/LatticeCache.cc
  Lattices/Lattice.cc
  Lattices/LatticeCleaner.cc
  Lattices/LatticeCleanProgress.cc
  Lattices/LatticeConcat.cc
  Lattices/LatticeConvolver.cc
  Lattices/LatticeExpr.cc
  Lattices/LatticeExprNode.cc
  Lattices/LatticeFFT.cc
  Lattices/LatticeFractile.cc
  Lattices/LatticeHistograms.cc
  Lattices/LatticeHistProgress.cc
  Lattices/LatticeHistSpecialize.cc
  Lattices/LatticeIndexer.cc
  Lattices/LatticeIterator.cc
  Lattices/LatticeIterInterface.cc
  Lattices/LatticeLocker.cc
  Lattices/LatticeNavigator.cc
  Lattices/LatticeProgress.cc
  Lattices/LatticeRegion.cc
  Lattices/LatticeSlice1D.cc
  Lattices/LatticeStatistics.cc
  Lattices/LatticeStatsBase.cc
  Lattices/LatticeStepper.cc
  Lattices/LatticeTwoPtCorr.cc
  Lattices/LatticeUtilities.cc
  Lattices/LattRegionHolder.cc
  Lattices/LattStatsProgress.cc
  Lattices/LattStatsSpecialize.cc
  Lattices/LCBox.cc
  Lattices/LCComplement.cc
  Lattices/LCConcatenation.cc
  Lattices/LCDifference.cc
  Lattices/LCEllipsoid.cc
  Lattices/LCExtension.cc
  Lattices/LCIntersection.cc
  Lattices/LCLELMask.cc
  Lattices/LCMask.cc
  Lattices/LCPagedMask.cc
  Lattices/LCPixelSet.cc
  Lattices/LCPolygon.cc
  Lattices/LCRegion2.cc
  Lattices/LCRegion.cc
  Lattices/LCRegionFixed.cc
  Lattices/LCRegionMulti.cc
  Lattices/LCRegionSingle.cc
  Lattices/LCSlicer.cc
  Lattices/LCStretch.cc
  Lattices/LCUnion.cc
  Lattices/LELArrayBase.cc
  Lattices/LELArray.cc
  Lattices/LELAttribute.cc
  Lattices/LELBinary2.cc
  Lattices/LELBinary.cc
  Lattices/LELCondition.cc
  Lattices/LELConvert.cc
  Lattices/LELCoordinates.cc
  Lattices/LELFunction2.cc
  Lattices/LELFunction.cc
  Lattices/LELInterface.cc
  Lattices/LELLattCoordBase.cc
  Lattices/LELLattCoord.cc
  Lattices/LELLattice.cc
  Lattices/LELRegion.cc
  Lattices/LELScalar.cc
  Lattices/LELSpectralIndex.cc
  Lattices/LELUnary2.cc
  Lattices/LELUnary.cc
  Lattices/LineCollapser.cc
  Lattices/MaskedLattice.cc
  Lattices/MaskedLatticeIterator.cc
  Lattices/PagedArray.cc
  Lattices/PagedArrIter.cc
  Lattices/PixelCurve1D.cc
  Lattices/RebinLattice.cc
  Lattices/SubLattice.cc
  Lattices/TempLattice.cc
  Lattices/TiledCollapser.cc
  Lattices/TiledLineStepper.cc
  Lattices/TiledShape.cc
  Lattices/TileStepper.cc
  )
