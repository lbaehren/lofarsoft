## CASA/measures/implement

project (libmeasures)

## -----------------------------------------------------------------------------
## Directories containing the test programs

subdirs (
  Measures
  )

## -----------------------------------------------------------------------------
## Library build

add_library (measures 
  Measures/Aberration.cc
  Measures/EarthField.cc
  Measures/EarthMagneticMachine.cc
  Measures/MBaseline.cc
  Measures/MCBase.cc
  Measures/MCBaseline.cc
  Measures/MCDirection.cc
  Measures/MCDoppler.cc
  Measures/MCEarthMagnetic.cc
  Measures/MCEpoch.cc
  Measures/MCFrame.cc
  Measures/MCFrequency.cc
  Measures/MConvertBase.cc
  Measures/MCPosition.cc
  Measures/MCRadialVelocity.cc
  Measures/MCuvw.cc
  Measures/MDirection.cc
  Measures/MDoppler.cc
  Measures/MEarthMagnetic.cc
  Measures/MeasBase.cc
  Measures/MeasComet.cc
  Measures/MeasConvert.cc
  Measures/MeasData.cc
  Measures/MeasFrame.cc
  Measures/MeasIERS.cc
  Measures/MeasJPL.cc
  Measures/MeasMath.cc
  Measures/MeasRef.cc
  Measures/MeasTable.cc
  Measures/Measure.cc
  Measures/MeasureHolder.cc
  Measures/MeasuresProxy.cc
  Measures/MEpoch.cc
  Measures/MFrequency.cc
  Measures/MPosition.cc
  Measures/MRadialVelocity.cc
  Measures/MRBase.cc
  Measures/Muvw.cc
  Measures/Nutation.cc
  Measures/ParAngleMachine.cc
  Measures/Precession.cc
  Measures/SofaTest.cc
  Measures/SolarPos.cc
  Measures/Stokes.cc
  Measures/UVWMachine.cc
  Measures/VelocityMachine.cc
  TableMeasures/ArrayMeasColumn.cc
  TableMeasures/ArrayQuantColumn.cc
  TableMeasures/ScalarMeasColumn.cc
  TableMeasures/ScalarQuantColumn.cc
  TableMeasures/TableMeasColumn.cc
  TableMeasures/TableMeasDescBase.cc
  TableMeasures/TableMeasDesc.cc
  TableMeasures/TableMeasOffsetDesc.cc
  TableMeasures/TableMeasRefDesc.cc
  TableMeasures/TableMeasType.cc
  TableMeasures/TableMeasValueDesc.cc
  TableMeasures/TableQuantumDesc.cc
  ## -- Template instantiation----------------------
  templates.cc
  Measures/templates.cc
  TableMeasures/templates.cc
  )
