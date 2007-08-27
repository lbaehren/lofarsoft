
## ------------------------------------------------------------------------------
## Test programs for fits/FITS

SET (tests
  tBinTable
  tFITS
  tFITSDateUtil
  tFITSHistoryUtil
  tFITSKeywordUtil
  tFITSSpectralUtil
  t_priArr_imgExt
  tfits1
  tfits2
  tfits3
  tfits4
  tfits5
  tfits_ascTbl
  tfits_ascTbl2
  tfits_binTbl1
  tfits_binTbl2
  tfits_imgExt2
  tfits_priGrp
  tfitsread_data
  tfitsskip
  tfitsskip_all
  tfitsskip_hdu
)

## ------------------------------------------------------------------------------
## Build the test programs

FOREACH (src ${tests})
  ADD_EXECUTABLE (${src} ${src}.cc)
  TARGET_LINK_LIBRARIES (${src} casa_fits casa_measures casa_tables casa_scimath casa_scimath_f casa_casa)
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
