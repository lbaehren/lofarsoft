
// include of header files

#include <casa/BasicSL/String.h>

// include of implementation files

#include <casa/Containers/BlockIO.cc>


namespace casa {

  template AipsIO & operator>>(AipsIO &, Block<String> &);
  template ostream & operator<<(ostream &, Block<String> const &);
  template void showBlock(ostream &, Block<String> const &, Int);
  template void getBlock(AipsIO &, Block<String> &);
  template AipsIO & operator<<(AipsIO &, Block<uInt> const &);
  template AipsIO & operator>>(AipsIO &, Block<Int> &);
  template AipsIO & operator>>(AipsIO &, Block<uInt> &);
  template void getBlock(AipsIO &, Block<Int> &);
  template void getBlock(AipsIO &, Block<uInt> &);
  template void putBlock(AipsIO &, Block<uInt> const &, Int);
  template void showBlock(ostream &, Block<uInt> const &, Int);
  
}

// 1000 casa/Containers/Link.cc casa/Arrays/Vector.h 
//      template class Link<Vector<Double> *> 
// 1010 casa/Containers/Link.cc casa/Arrays/Vector.h 
//      template class Link<Vector<Float> *> 
// 1020 casa/Containers/Link.cc casa/Inputs/Param.h 
//      template class Link<Param> 
// 1030 casa/Containers/Link.cc casa/Utilities/CountedPtr.h 
//      = casa/OS/SysEvent.h 
//      template class Link<CountedPtr<SysEventTargetInfo> > 
// 1040 casa/Containers/Link.cc casa/Utilities/Notice.h 
//      template class Link<NoticeTarget *> 
// 1050 casa/Containers/Link.cc 
//      template class Link<uInt> 
// 1060 casa/Containers/Link.cc 
//      template class Link<uLong> 
// 1070 casa/Containers/Link.cc 
//      template class Link<void *> 
// 1000 casa/Containers/List.cc casa/Inputs/Param.h 
//      template class ConstListIter<Param> 
// 1010 casa/Containers/List.cc casa/Inputs/Param.h 
//      template class List<Param> 
// 1020 casa/Containers/List.cc casa/Inputs/Param.h 
//      template class ListIter<Param> 
// 1030 casa/Containers/List.cc casa/Inputs/Param.h 
//      template class ListNotice<Param> 
// 1040 casa/Containers/List.cc casa/Utilities/CountedPtr.h 
//      = casa/OS/SysEvent.h 
//      template class ConstListIter<CountedPtr<SysEventTargetInfo> > 
// 1050 casa/Containers/List.cc casa/Utilities/CountedPtr.h 
//      = casa/OS/SysEvent.h 
//      template class List<CountedPtr<SysEventTargetInfo> > 
// 1060 casa/Containers/List.cc casa/Utilities/CountedPtr.h 
//      = casa/OS/SysEvent.h 
//      template class ListIter<CountedPtr<SysEventTargetInfo> > 
// 1070 casa/Containers/List.cc casa/Utilities/CountedPtr.h 
//      = casa/OS/SysEvent.h 
//      template class ListNotice<CountedPtr<SysEventTargetInfo> > 
// 1080 casa/Containers/List.cc 
//      template class ConstListIter<uInt> 
//      template class List<uInt> 
//      template class ListIter<uInt> 
//      template class ListNotice<uInt> 
// 1090 casa/Containers/List.cc 
//      template class ConstListIter<uLong> 
//      template class List<uLong> 
//      template class ListIter<uLong> 
//      template class ListNotice<uLong> 
// 1100 casa/Containers/List.cc 
//      template class ConstListIter<void *> 
//      template class List<void *> 
//      template class ListIter<void *> 
//      template class ListNotice<void *> 
// 1000 casa/Containers/Map.cc 
//      = casa/BasicSL/String.h 
//      = casa/Containers/Block.h 
//      = casa/Arrays/IPosition.h 
//      template class ConstMapIter<String, Block<IPosition> > 
//      template class MapIter<String, Block<IPosition> > 
//      template class MapIterRep<String, Block<IPosition> > 
//      template class MapRep<String, Block<IPosition> > 
//      template class Map<String, Block<IPosition> > 
// 1010 casa/Containers/Map.cc 
//      = casa/BasicSL/String.h 
//      template class ConstMapIter<String, Int> 
//      template class MapIter<String, Int> 
//      template class MapIterRep<String, Int> 
//      template class MapRep<String, Int> 
//      template class Map<String, Int> 
// 1020 casa/Containers/Map.cc casa/Arrays/Array.h 
//      template class ConstMapIter<Int, Array<Float> > 
// 1030 casa/Containers/Map.cc casa/Arrays/Array.h 
//      template class Map<Int, Array<Float> > 
// 1040 casa/Containers/Map.cc casa/Arrays/Array.h 
//      template class MapIter<Int, Array<Float> > 
// 1050 casa/Containers/Map.cc casa/Arrays/Array.h 
//      template class MapIterRep<Int, Array<Float> > 
// 1060 casa/Containers/Map.cc casa/Arrays/Array.h 
//      template class MapRep<Int, Array<Float> > 
// 1070 casa/Containers/Map.cc casa/Arrays/Vector.h 
//      template class ConstMapIter<Int, Vector<Float> > 
// 1080 casa/Containers/Map.cc casa/Arrays/Vector.h 
//      template class Map<Int, Vector<Float> > 
// 1090 casa/Containers/Map.cc casa/Arrays/Vector.h 
//      template class MapIterRep<Int, Vector<Float> > 
// 1100 casa/Containers/Map.cc casa/Arrays/Vector.h 
//      template class MapRep<Int, Vector<Float> > 
// 1110 casa/Containers/Map.cc 
//      template class ConstMapIter<Int, Int> 
// 1120 casa/Containers/Map.cc 
//      template class ConstMapIter<String, Double> 
// 1130 casa/Containers/Map.cc 
//      template class Map<Int, Int> 
// 1140 casa/Containers/Map.cc 
//      template class Map<String, Double> 
// 1150 casa/Containers/Map.cc 
//      template class MapIterRep<Int, Int> 
// 1160 casa/Containers/Map.cc 
//      template class MapIterRep<String, Double> 
// 1170 casa/Containers/Map.cc 
//      template class MapRep<Int, Int> 
// 1180 casa/Containers/Map.cc 
//      template class MapRep<String, Double> 
// 1000 casa/Containers/OrderedMap.cc 
//      = casa/Arrays/IPosition.h 
//      = casa/BasicSL/String.h 
//      = casa/Containers/Block.h 
//      template class OrderedMapRep<String, Block<IPosition> > 
//      template class OrderedMapIterRep<String, Block<IPosition> > 
//      template class OrderedMap<String, Block<IPosition> > 
//      template class OrderedMapNotice<String, Block<IPosition> > 
// 1010 casa/Containers/OrderedMap.cc 
//      = casa/BasicSL/String.h 
//      template class OrderedMapRep<String, Int> 
//      template class OrderedMapIterRep<String, Int> 
//      template class OrderedMap<String, Int> 
//      template class OrderedMapNotice<String, Int> 
// 1020 casa/Containers/OrderedMap.cc casa/Arrays/Array.h 
//      template class OrderedMap<Int, Array<Float> > 
// 1030 casa/Containers/OrderedMap.cc casa/Arrays/Array.h 
//      template class OrderedMapIterRep<Int, Array<Float> > 
// 1040 casa/Containers/OrderedMap.cc casa/Arrays/Array.h 
//      template class OrderedMapNotice<Int, Array<Float> > 
// 1050 casa/Containers/OrderedMap.cc casa/Arrays/Array.h 
//      template class OrderedMapRep<Int, Array<Float> > 
// 1060 casa/Containers/OrderedMap.cc casa/Arrays/Vector.h 
//      template class OrderedMap<Int, Vector<Float> > 
// 1070 casa/Containers/OrderedMap.cc casa/Arrays/Vector.h 
//      template class OrderedMapIterRep<Int, Vector<Float> > 
// 1080 casa/Containers/OrderedMap.cc casa/Arrays/Vector.h 
//      template class OrderedMapNotice<Int, Vector<Float> > 
// 1090 casa/Containers/OrderedMap.cc casa/Arrays/Vector.h 
//      template class OrderedMapRep<Int, Vector<Float> > 
// 1100 casa/Containers/OrderedMap.cc 
//      template class OrderedMap<Int, Int> 
// 1110 casa/Containers/OrderedMap.cc 
//      template class OrderedMapIterRep<Int, Int> 
// 1120 casa/Containers/OrderedMap.cc 
//      template class OrderedMapNotice<Int, Int> 
// 1130 casa/Containers/OrderedMap.cc 
//      template class OrderedMapRep<Int, Int> 
// 1000 casa/Containers/OrderedPair.cc 
//      = casa/Arrays/IPosition.h 
//      = casa/BasicSL/String.h 
//      = casa/Containers/Block.h 
//      template class OrderedPair<String, Block<IPosition> > 
// 1010 casa/Containers/OrderedPair.cc 
//      = casa/Containers/Record.h 
//      = casa/BasicSL/String.h 
//      template class OrderedPair<String, Record *> 
// 1020 casa/Containers/OrderedPair.cc 
//      = casa/Utilities/DataType.h 
//      template class OrderedPair<Int, DataType> 
// 1030 casa/Containers/OrderedPair.cc casa/Arrays/Array.h 
//      template class OrderedPair<Int, Array<Float> > 
// 1040 casa/Containers/OrderedPair.cc casa/Arrays/Vector.h 
//      template class OrderedPair<Int, Vector<Float> > 
// 1050 casa/Containers/OrderedPair.cc casa/Arrays/Vector.h 
//      template class OrderedPair<Int, Vector<Int> > 
//      template class OrderedPair<Int, Vector<uInt> > 
//      template class OrderedPair<Int, Vector<Double> > 
// 1060 casa/Containers/OrderedPair.cc casa/BasicSL/String.h 
//      template class OrderedPair<Int, String> 
// 1070 casa/Containers/OrderedPair.cc casa/BasicSL/String.h 
//      template class OrderedPair<String, Bool> 
// 1080 casa/Containers/OrderedPair.cc casa/BasicSL/String.h 
//      template class OrderedPair<String, Int> 
// 1090 casa/Containers/OrderedPair.cc casa/BasicSL/String.h 
//      template class OrderedPair<String, String> 
// 1100 casa/Containers/OrderedPair.cc casa/BasicSL/String.h 
//      template class OrderedPair<String, void *> 
// 1110 casa/Containers/OrderedPair.cc casa/Containers/PoolStack.h 
//      = casa/BasicSL/Complex.h 
//      template <class T> class AutoDiffRep 
//      template class OrderedPair<uInt, PoolStack<AutoDiffRep<Complex>, uInt> *> 
// 1120 casa/Containers/OrderedPair.cc casa/Containers/PoolStack.h 
//      = casa/BasicSL/Complex.h 
//      template <class T> class AutoDiffRep 
//      template class OrderedPair<uInt, PoolStack<AutoDiffRep<DComplex>, uInt> *> 
// 1130 casa/Containers/OrderedPair.cc casa/Containers/PoolStack.h 
//      template <class T> class AutoDiffRep 
//      template class OrderedPair<uInt, PoolStack<AutoDiffRep<Double>, uInt> *> 
// 1140 casa/Containers/OrderedPair.cc casa/Containers/PoolStack.h 
//      template <class T> class AutoDiffRep 
//      template class OrderedPair<uInt, PoolStack<AutoDiffRep<Float>, uInt> *> 
// 1150 casa/Containers/OrderedPair.cc casa/Quanta/Quantum.h 
//      = casa/Arrays/Array.h 
//      template class OrderedPair<Int, Quantum<Array<Double> > > 
// 1160 casa/Containers/OrderedPair.cc casa/Quanta/Quantum.h 
//      = casa/Arrays/Vector.h 
//      template class OrderedPair<Int, Quantum<Vector<Double> > > 
// 1170 casa/Containers/OrderedPair.cc casa/Quanta/Quantum.h 
//      template class OrderedPair<Int, Quantum<Double> > 
// 1180 casa/Containers/OrderedPair.cc 
//      template class OrderedPair<Int, Bool> 
// 1190 casa/Containers/OrderedPair.cc 
//      template class OrderedPair<Int, Double> 
//      template class OrderedPair<Int, uInt> 
//      template class OrderedPair<Double, uInt> 
//      template class OrderedPair<Double, Int> 
// 1200 casa/Containers/OrderedPair.cc 
//      template class OrderedPair<Int, Int> 
// 1210 casa/Containers/OrderedPair.cc 
//      template class OrderedPair<uInt, Int> 
// 1220 casa/Containers/OrderedPair.cc 
//      template class OrderedPair<uInt, uInt> 
// 1230 casa/Containers/OrderedPair.cc 
//      template class OrderedPair<void *, void *> 
// 1000 casa/Containers/Queue.cc 
//      = casa/Logging/LogMessage.h 
//      template class Queue<LogMessage> 
// 1000 casa/Containers/RecordField.cc casa/Arrays/Array.h 
//      = casa/BasicSL/Complex.h 
//      template class RORecordFieldPtr<Array<Complex> > 
//      template class RORecordFieldPtr<Array<DComplex> > 
//      template class RORecordFieldPtr<Array<Bool> > 
//      template class RORecordFieldPtr<Array<Char> > 
//      template class RORecordFieldPtr<Array<Double> > 
//      template class RORecordFieldPtr<Array<Float> > 
//      template class RORecordFieldPtr<Array<Int> > 
//      template class RORecordFieldPtr<Array<Short> > 
//      template class RORecordFieldPtr<Array<String> > 
//      template class RORecordFieldPtr<Array<uChar> > 
//      template class RORecordFieldPtr<Array<uInt> > 
// 1010 casa/Containers/RecordField.cc casa/Arrays/Array.h 
//      = casa/BasicSL/Complex.h 
//      template class RecordFieldPtr<Array<Bool> > 
//      template class RecordFieldPtr<Array<Char> > 
//      template class RecordFieldPtr<Array<Complex> > 
//      template class RecordFieldPtr<Array<DComplex> > 
//      template class RecordFieldPtr<Array<Double> > 
//      template class RecordFieldPtr<Array<Float> > 
//      template class RecordFieldPtr<Array<Int> > 
//      template class RecordFieldPtr<Array<Short> > 
//      template class RecordFieldPtr<Array<String> > 
//      template class RecordFieldPtr<Array<uChar> > 
//      template class RecordFieldPtr<Array<uInt> > 
// 1020 casa/Containers/RecordField.cc casa/BasicSL/Complex.h 
//      template class RORecordFieldPtr<Complex> 
//      template class RORecordFieldPtr<DComplex> 
//      template class RORecordFieldPtr<Bool> 
//      template class RORecordFieldPtr<Double> 
//      template class RORecordFieldPtr<Float> 
//      template class RORecordFieldPtr<Int> 
//      template class RORecordFieldPtr<Record> 
//      template class RORecordFieldPtr<Short> 
//      template class RORecordFieldPtr<String> 
//      template class RORecordFieldPtr<uChar> 
//      template class RORecordFieldPtr<uInt> 
// 1030 casa/Containers/RecordField.cc casa/BasicSL/Complex.h 
//      template class RecordFieldPtr<Bool> 
//      template class RecordFieldPtr<Complex> 
//      template class RecordFieldPtr<DComplex> 
//      template class RecordFieldPtr<Double> 
//      template class RecordFieldPtr<Float> 
//      template class RecordFieldPtr<Int> 
//      template class RecordFieldPtr<Record> 
//      template class RecordFieldPtr<Short> 
//      template class RecordFieldPtr<String> 
//      template class RecordFieldPtr<uChar> 
//      template class RecordFieldPtr<uInt> 
// 1000 casa/Containers/RecordFieldWriter.cc 
//      = casa/Arrays/Array.h 
//      template class RecordFieldCopier<Array<Bool>, Array<Bool> > 
//      template class RecordFieldCopier<Array<Complex>, Array<Complex> > 
//      template class RecordFieldCopier<Array<DComplex>, Array<DComplex> > 
//      template class RecordFieldCopier<Array<Double>, Array<Double> > 
//      template class RecordFieldCopier<Array<Float>, Array<Float> > 
//      template class RecordFieldCopier<Array<Int>, Array<Int> > 
//      template class RecordFieldCopier<Array<Short>, Array<Short> > 
//      template class RecordFieldCopier<Array<String>, Array<String> > 
//      template class RecordFieldCopier<Array<uChar>, Array<uChar> > 
// 1010 casa/Containers/RecordFieldWriter.cc 
//      template class RecordFieldCopier<Bool, Bool> 
//      template class RecordFieldCopier<Complex, Complex> 
//      template class RecordFieldCopier<DComplex, DComplex> 
//      template class RecordFieldCopier<Double, Double> 
//      template class RecordFieldCopier<Float, Float> 
//      template class RecordFieldCopier<Float, Int> 
//      template class RecordFieldCopier<Int, Int> 
//      template class RecordFieldCopier<Short, Short> 
//      template class RecordFieldCopier<String, String> 
//      template class RecordFieldCopier<uChar, uChar> 
//      template class RecordFieldCopier<Double, Float> 
//      template class RecordFieldCopier<Int, Short> 
// 1000 casa/Containers/SimOrdMap.cc 
//      = casa/BasicSL/String.h 
//      = casa/Containers/Record.h 
//      template class SimpleOrderedMap<String, Record *> 
// 1010 casa/Containers/SimOrdMap.cc 
//      = casa/Utilities/DataType.h 
//      template class SimpleOrderedMap<Int, DataType> 
// 1020 casa/Containers/SimOrdMap.cc casa/Arrays/Vector.h 
//      template class SimpleOrderedMap<Int, Vector<uInt> > 
//      template class SimpleOrderedMap<Int, Vector<Int> > 
//      template class SimpleOrderedMap<Int, Vector<Double> > 
// 1030 casa/Containers/SimOrdMap.cc casa/BasicSL/String.h 
//      template class SimpleOrderedMap<Int, String> 
// 1040 casa/Containers/SimOrdMap.cc casa/BasicSL/String.h 
//      template class SimpleOrderedMap<String, Bool> 
// 1050 casa/Containers/SimOrdMap.cc casa/BasicSL/String.h 
//      template class SimpleOrderedMap<String, Int> 
// 1060 casa/Containers/SimOrdMap.cc casa/BasicSL/String.h 
//      template class SimpleOrderedMap<String, String> 
// 1070 casa/Containers/SimOrdMap.cc casa/BasicSL/String.h 
//      template class SimpleOrderedMap<String, void *> 
// 1080 casa/Containers/SimOrdMap.cc casa/Containers/OrderedPair.cc 
//      template class SimpleOrderedMap<void*, Int> 
//      template class OrderedPair<void*, Int> 
// 1090 casa/Containers/SimOrdMap.cc casa/Containers/PoolStack.h 
//      = casa/BasicSL/Complex.h 
//      template <class T> class AutoDiffRep 
//      template class SimpleOrderedMap<uInt, PoolStack<AutoDiffRep<Complex>, uInt>*> 
// 1100 casa/Containers/SimOrdMap.cc casa/Containers/PoolStack.h 
//      = casa/BasicSL/Complex.h 
//      template <class T> class AutoDiffRep 
//      template class SimpleOrderedMap<uInt, PoolStack<AutoDiffRep<DComplex>, uInt>*> 
// 1110 casa/Containers/SimOrdMap.cc casa/Containers/PoolStack.h 
//      template <class T> class AutoDiffRep 
//      template class SimpleOrderedMap<uInt, PoolStack<AutoDiffRep<Double>, uInt>*> 
// 1120 casa/Containers/SimOrdMap.cc casa/Containers/PoolStack.h 
//      template <class T> class AutoDiffRep 
//      template class SimpleOrderedMap<uInt, PoolStack<AutoDiffRep<Float>, uInt>*> 
// 1130 casa/Containers/SimOrdMap.cc casa/Quanta/Quantum.h 
//      = casa/Arrays/Array.h 
//      template class SimpleOrderedMap<Int, Quantum<Array<Double> > > 
// 1140 casa/Containers/SimOrdMap.cc casa/Quanta/Quantum.h 
//      = casa/Arrays/Vector.h 
//      template class SimpleOrderedMap<Int, Quantum<Vector<Double> > > 
// 1150 casa/Containers/SimOrdMap.cc casa/Quanta/Quantum.h 
//      template class SimpleOrderedMap<Int, Quantum<Double> > 
// 1160 casa/Containers/SimOrdMap.cc 
//      template class SimpleOrderedMap<Int, Bool> 
// 1170 casa/Containers/SimOrdMap.cc 
//      template class SimpleOrderedMap<Int, Double> 
// 1180 casa/Containers/SimOrdMap.cc 
//      template class SimpleOrderedMap<Int, Int> 
// 1190 casa/Containers/SimOrdMap.cc 
//      template class SimpleOrderedMap<Int, uInt> 
//      template class SimpleOrderedMap<uInt, Int> 
//      template class SimpleOrderedMap<uInt, uInt> 
//      template class SimpleOrderedMap<Double, uInt> 
//      template class SimpleOrderedMap<Double, Int> 
// 1200 casa/Containers/SimOrdMap.cc 
//      template class SimpleOrderedMap<void *, void *> 
// 1000 casa/Containers/SimOrdMapIO.cc casa/BasicSL/String.h 
//      template AipsIO & operator>>(AipsIO &, SimpleOrderedMap<String, String> &) 
//      template AipsIO & operator<<(AipsIO &, SimpleOrderedMap<String, String> const &) 
// 1010 casa/Containers/SimOrdMapIO.cc 
//      template AipsIO & operator>>(AipsIO &, SimpleOrderedMap<Int, Int> &) 
//      template AipsIO & operator<<(AipsIO &, SimpleOrderedMap<Int, Int> const &) 
// 1000 casa/Containers/Stack.cc casa/Arrays/Vector.h 
//      template class Stack<Vector<Double> *> 
// 1010 casa/Containers/Stack.cc casa/Arrays/Vector.h 
//      template class Stack<Vector<Float> *> 
// 1020 casa/Containers/Stack.cc 
//      template class Stack<void *> 
