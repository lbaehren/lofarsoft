
## ------------------------------------------------------------------------------
## Test programs for images/Images

SET (tests
  dImageHistograms
  dImageInterface
  dImageMoments
  dImageProfileFit
  dImageStatistics
  dImageSummary
  dPagedImage
  fits2image
  image2fits
  tComponentImager
  tExtendImage
  tFITSImage
  tImageConcat
  tImageConvolver
  tImageDecomposer
  tImageExpr2Gram
  tImageExpr3Gram
  tImageExprGram
  tImageFFT
  tImageFit1D
  tImageInfo
  tImagePolarimetry
  tImageRegrid
  tImageSourceFinder
  tImageUtilities
  tLELSpectralIndex
  tMIRIADImage
  tPagedImage
  tPagedImage2
  tRebinImage
  tRegionHandler
  tSepImConvolver
  tSubImage
  tTempImage
  tWCBox
  tWCExtension
  tWCLELMask
  tWCUnion
)

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} images components coordinates fits measures tables scimath casa mir)
  ## testing 
  add_test (${src} ${src})
ENDFOREACH (src)

## ------------------------------------------------------------------------------
## Installation

install (TARGETS ${tests}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
