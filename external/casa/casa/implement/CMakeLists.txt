## CASA/implement/casa

project (libcasa)

## -----------------------------------------------------------------------------
## Directories containing the test programs

subdirs (
  Arrays/test
  BasicMath/test
  BasicSL/test
  Containers/test
  Exceptions/test
  Inputs/test
  IO/test
  Logging/test
  OS/test
  Quanta/test
  System/test
#  test
  Utilities/test
  )

## -----------------------------------------------------------------------------
## libcasa

add_library (casa
## -- casa/implement/casa
  aips.cc
  version2.cc
## casa/implement/casa/Arrays --------------------
  Arrays/Array.cc
  Arrays/Array2.cc
  Arrays/Array2Math.cc
  Arrays/ArrayBase.cc
  Arrays/ArrayError.cc
  Arrays/ArrayIO.cc
  Arrays/ArrayIter.cc
  Arrays/ArrayLogical.cc
  Arrays/ArrayMath.cc
  Arrays/ArrayPosIter.cc
  Arrays/ArrayUtil.cc
  Arrays/ArrayUtil2.cc
  Arrays/AxesMapping.cc
  Arrays/AxesSpecifier.cc
  Arrays/Cube.cc
  Arrays/ExtendSpecifier.cc
  Arrays/IPosition.cc
  Arrays/IPosition2.cc
  Arrays/MaskArrIO.cc
  Arrays/MaskArrLogi.cc
  Arrays/MaskArrMath.cc
  Arrays/MaskArrMath2.cc
  Arrays/MaskedArray.cc
  Arrays/Matrix.cc
  Arrays/Matrix2Math.cc
  Arrays/MatrixIter.cc
  Arrays/MatrixMath.cc
  Arrays/Slicer.cc
  Arrays/Vector.cc
  Arrays/Vector2.cc
  Arrays/VectorIter.cc
## casa/implement/casa/BasicMath -----------------
  BasicMath/Functional.cc
  BasicMath/Math.cc
  BasicMath/Random.cc
## casa/implement/casa/BasicSL -------------------
  BasicSL/Complex.cc
  BasicSL/Constants.cc
  BasicSL/IComplex.cc
  BasicSL/RegexBase.cc
  BasicSL/String.cc
## casa/implement/casa/Containers ----------------
  Containers/BlockIO.cc
  Containers/Dlink.cc
  Containers/Dlist.cc
  Containers/Dlist2.cc
  Containers/DlistIO.cc
  Containers/HashMap.cc
  Containers/HashMap2.cc
  Containers/HashMapIO.cc
  Containers/HashMapIter.cc
  Containers/IterError.cc
  Containers/Link.cc
  Containers/List.cc
  Containers/List2.cc
  Containers/ListIO.cc
  Containers/ListMap.cc
  Containers/ListMapIO.cc
  Containers/Map.cc
  Containers/Map2.cc
  Containers/MapIO.cc
  Containers/ObjectPool.cc
  Containers/OrdMapIO.cc
  Containers/OrdPairIO.cc
  Containers/OrderedMap.cc
  Containers/OrderedPair.cc
  Containers/PoolStack.cc
  Containers/Queue.cc
  Containers/Record.cc
  Containers/Record2.cc
  Containers/Record2Interface.cc
  Containers/RecordDesc.cc
  Containers/RecordDescRep.cc
  Containers/RecordField.cc
  Containers/RecordField2Writer.cc
  Containers/RecordFieldId.cc
  Containers/RecordFieldWriter.cc
  Containers/RecordInterface.cc
  Containers/RecordRep.cc
  Containers/SimOrdMap.cc
  Containers/SimOrdMapIO.cc
  Containers/Stack.cc
  Containers/Stack2.cc
  Containers/StackError.cc
  Containers/ValueHolder.cc
  Containers/ValueHolderRep.cc
## casa/implement/casa/Exceptions ----------------
  Exceptions/Error.cc
  Exceptions/Error2.cc
## casa/implement/casa/IO ------------------------
  IO/AipsIO.cc
  IO/AipsIOCarray.cc
  IO/BaseSinkSource.cc
  IO/BucketCache.cc
  IO/BucketFile.cc
  IO/ByteIO.cc
  IO/ByteSink.cc
  IO/ByteSinkSource.cc
  IO/ByteSource.cc
  IO/CanonicalIO.cc
  IO/ConversionIO.cc
  IO/FileLocker.cc
  IO/FilebufIO.cc
  IO/FiledesIO.cc
  IO/LECanonicalIO.cc
  IO/LargeFilebufIO.cc
  IO/LargeFiledesIO.cc
  IO/LargeRegularFileIO.cc
  IO/LockFile.cc
  IO/MemoryIO.cc
  IO/RawIO.cc
  IO/RegularFileIO.cc
  IO/StreamIO.cc
  IO/TapeIO.cc
  IO/TypeIO.cc
## casa/implement/casa/Inputs --------------------
  Inputs/Input.cc
  Inputs/Param.cc
## casa/implement/casa/Logging -------------------
  Logging/LogFilter.cc
  Logging/LogFilterInterface.cc
  Logging/LogIO.cc
  Logging/LogMessage.cc
  Logging/LogOrigin.cc
  Logging/LogSink.cc
  Logging/LogSinkInterface.cc
  Logging/MemoryLogSink.cc
  Logging/NullLogSink.cc
  Logging/StreamLogSink.cc
## casa/implement/casa/OS ------------------------
  OS/CanonicalConversion.cc
  OS/CanonicalDataConversion.cc
  OS/Conversion.cc
  OS/DOos.cc
  OS/DataConversion.cc
  OS/Directory.cc
  OS/DirectoryIterator.cc
  OS/EnvVar.cc
  OS/File.cc
  OS/HostInfo.cc
  OS/IBMConversion.cc
  OS/IBMDataConversion.cc
  OS/LECanonicalConversion.cc
  OS/LECanonicalDataConversion.cc
  OS/LittleEndianConversion.cc
  OS/Memory.cc
  OS/ModcompConversion.cc
  OS/ModcompDataConversion.cc
  OS/Path.cc
  OS/PrecTimer.cc
  OS/RawDataConversion.cc
  OS/RegularFile.cc
  OS/SymLink.cc
  OS/SysEvent.cc
  OS/Time.cc
  OS/Timer.cc
  OS/VAXConversion.cc
  OS/VAXDataConversion.cc
  OS/malloc.cc
## casa/implement/casa/Quanta --------------------
  Quanta/Euler.cc
  Quanta/MVAngle.cc
  Quanta/MVBaseline.cc
  Quanta/MVDirection.cc
  Quanta/MVDoppler.cc
  Quanta/MVDouble.cc
  Quanta/MVEarthMagnetic.cc
  Quanta/MVEpoch.cc
  Quanta/MVFrequency.cc
  Quanta/MVPosition.cc
  Quanta/MVRadialVelocity.cc
  Quanta/MVTime.cc
  Quanta/MVuvw.cc
  Quanta/MeasValue.cc
  Quanta/QBase.cc
  Quanta/QC.cc
  Quanta/QLogical.cc
  Quanta/QLogical2.cc
  Quanta/QMath.cc
  Quanta/QMath2.cc
  Quanta/QuantaProxy.cc
  Quanta/Quantum.cc
  Quanta/Quantum2.cc
  Quanta/QuantumHolder.cc
  Quanta/RotMatrix.cc
  Quanta/Unit.cc
  Quanta/UnitDim.cc
  Quanta/UnitMap.cc
  Quanta/UnitMap2.cc
  Quanta/UnitMap3.cc
  Quanta/UnitMap4.cc
  Quanta/UnitMap5.cc
  Quanta/UnitMap6.cc
  Quanta/UnitMap7.cc
  Quanta/UnitName.cc
  Quanta/UnitVal.cc
## casa/implement/casa/System --------------------
  System/Aipsrc.cc
  System/AipsrcBool.cc
  System/AipsrcVBool.cc
  System/AipsrcVString.cc
  System/AipsrcValue.cc
  System/AipsrcValue2.cc
  System/AipsrcVector.cc
  System/AppInfo.cc
  System/Choice.cc
  System/ObjectID.cc
  System/ObjectID2.cc
  System/PGPlotter.cc
  System/PGPlotterInterface.cc
  System/ProgressMeter.cc
## casa/implement/casa/Utilities -----------------
  Utilities/Assert.cc
  Utilities/BinarySearch.cc
  Utilities/BitVector.cc
  Utilities/COWPtr.cc
  Utilities/Compare.cc
  Utilities/CompositeNumber.cc
  Utilities/Copy.cc
  Utilities/Copy2.cc
  Utilities/CountedPtr.cc
  Utilities/CountedPtr2.cc
  Utilities/DataType.cc
  Utilities/DynBuffer.cc
  Utilities/Fallible2.cc
  Utilities/GenSort.cc
  Utilities/LinearSearch.cc
  Utilities/MUString.cc
  Utilities/Notice.cc
  Utilities/PtrHolder.cc
  Utilities/RecordTransformable.cc
  Utilities/RegSequence.cc
  Utilities/Regex.cc
  Utilities/Register.cc
  Utilities/Sequence.cc
  Utilities/Sequence2.cc
  Utilities/Sort.cc
  Utilities/SortError.cc
  Utilities/Template.cc
  Utilities/ValType.cc
  Utilities/cregex.cc
## -- Template instantiation----------------------
#  templatesArrays.cc
  templatesBasicMath.cc
#  templatesBasicSL.cc
  templatesContainers.cc
  templatesExceptions.cc
  templatesQuanta.cc
  templatesSystem.cc
  templatesUtilities.cc
)

