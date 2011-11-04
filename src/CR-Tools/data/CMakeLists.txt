
## ==============================================================================
##
##  Test datasets & calibration data
##
## ==============================================================================

##____________________________________________________________________
## LOPES-08 antenna positions

find_file (positions_lopes08 positions_lopes08.dat
  PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

##____________________________________________________________________
## LOPES Cal-Table

find_path (caltable_lopes
  NAMES
  table.dat table.info
  PATHS
  LOPES-CalTable
  )

##____________________________________________________________________
## LOPES example datasets

find_file (dataset_lopes
  NAMES
  example.event
  2007.01.31.23:59:33.960.event
  PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

##____________________________________________________________________
## Test TBB datasets

find_file (dataset_tbb_raw
  NAMES
  triggered-pulse-2010-02-11-TBB1.h5
  rw_20071024_090656_0101.dat
  rw_20071024_090656_0102.dat
  PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

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

##____________________________________________________________________
## PPF coefficients

find_file (data_ppf_coefficients Coeffs16384Kaiser-quant.dat
  PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

find_file (data_ppf_inversion ppf_inv.dat
  PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

find_file (data_modelcoeff1 modelcoeff1.dat
  PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

find_file (data_modelcoeff2 modelcoeff2.dat
  PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}
  )
