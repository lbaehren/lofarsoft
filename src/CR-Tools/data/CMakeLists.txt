
## ==============================================================================
##
##  Test datasets & calibration data
##
## ==============================================================================

##____________________________________________________________________
## LOPES Cal-Table

find_path (caltable_lopes
  NAMES
  table.dat table.info
  PATHS
  LOPES-CalTable
  )

##____________________________________________________________________
## Test datasets

find_file (dataset_tbb
  NAMES
  lightning_16_48.h5 rw_20090417_181700.h5
  PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

if (dataset_tbb)
  set (CR_DATASET_TBB 1)
  set (CR_DATASET_TBB_STATION "Station023")
  set (CR_DATASET_TBB_CHANNEL "023000000")
  set (CR_DATASET_TBB_DATASET "Station023/023000000")
endif (dataset_tbb)

## Reference values for test programs

find_file (dataset_lopes_example
  NAMES
  example.event
  PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

find_file (data_ppf_coefficients Coeffs16384Kaiser-quant.dat
  PATHS
  ${CR_SOURCE_DIR}/data
  ${CR_SOURCE_DIR}/../data
  ${CR_SOURCE_DIR}/../../data
  PATH_SUFFIXES
  lopes
  )

find_file (data_ppf_inversion ppf_inv.dat
  PATHS
  ${CR_SOURCE_DIR}/data
  ${CR_SOURCE_DIR}/../data
  ${CR_SOURCE_DIR}/../../data
  PATH_SUFFIXES
  lopes
  )

find_file (data_modelcoeff1 modelcoeff1.dat
  PATHS
  ${CR_SOURCE_DIR}/data
  ${CR_SOURCE_DIR}/../data
  ${CR_SOURCE_DIR}/../../data
  PATH_SUFFIXES
  lopes
  )

find_file (data_modelcoeff2 modelcoeff2.dat
  PATHS
  ${CR_SOURCE_DIR}/data
  ${CR_SOURCE_DIR}/../data
  ${CR_SOURCE_DIR}/../../data
  PATH_SUFFIXES
  lopes
  )

find_file (positions_lopes08 positions_lopes08.dat
  PATHS
  ${CR_SOURCE_DIR}/data
  ${CR_SOURCE_DIR}/../data
  ${CR_SOURCE_DIR}/../../data
  PATH_SUFFIXES
  lopes
  )

##
## ==============================================================================
##

## include guard
if (NOT FIND_TESTDATASETS_CMAKE)

  set (FIND_TESTDATASETS_CMAKE TRUE)

  ## === FITS ===================================================================
  
  if (NOT dataset_fits)
    find_file (dataset_fits WN65341H.fits
      PATHS
      ${LUS_ROOT}/data
      ${LUS_ROOT}/data/test
      )
  endif (NOT dataset_fits)
  
  ## === Measurement Set ========================================================
  
  if (NOT dataset_ms)
    find_path (dataset_ms table.dat table.info
      PATHS
      ${LUS_ROOT}/data
      ${LUS_ROOT}/data/test
      PATH_SUFFIXES
      L2007_01810_SB18-20_OB20.MS
      lofar/cs1/L2007_01810_SB18-20_OB20.MS
      10602381_S0_T20.MS
      wsrt/lffe/10602381_S0_T20.MS
      )
  endif (NOT dataset_ms)
  
  ## === TBB time-series data ===================================================
  
  if (NOT dataset_tbb_raw)
    find_file (dataset_tbb_raw
      NAMES
      triggered-pulse-2010-02-11-TBB1.h5
      rw_20071024_090656_0101.dat
      rw_20071024_090656_0102.dat
      PATHS
      ${LUS_ROOT}/data
      ${LUS_ROOT}/data/test
      ${LUS_ROOT}/data/lofar/trigger-2010-02-11
      )
  endif (NOT dataset_tbb_raw)
  
  ## === Beam-formed data =======================================================
  
  if (NOT dataset_bf_raw)
    find_file (dataset_bf_raw
      NAMES
      bf_20080604_121337.cor
      PATHS
      ${LUS_ROOT}/data
      ${LUS_ROOT}/data/test
      )
  endif (NOT dataset_bf_raw)
  
  if (NOT dataset_beamformed)
    find_file (dataset_beamformed
      NAMES
      bf_20080604_121337.cor.h5 bf_20080604_121337.h5 TBB1.cor.h5
      PATHS
      ${LUS_ROOT}/data
      ${LUS_ROOT}/data/test
      )
  endif (NOT dataset_beamformed)
  
  ## === LOPES event ============================================================
  
  if (NOT dataset_lopes)
    find_file (dataset_lopes
      NAMES
      2007.01.31.23:59:33.960.event
      PATHS
      ${LUS_ROOT}/data
      ${LUS_ROOT}/data/test
      PATH_SUFFIXES
      lopes
      )
  endif (NOT dataset_lopes)
  
endif (NOT FIND_TESTDATASETS_CMAKE)
