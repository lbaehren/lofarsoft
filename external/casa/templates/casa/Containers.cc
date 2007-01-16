
/*!
  \file templatesContainers.cc

  \brief Template instantiation for classes in casa/implement/Containers

  \author Lars B&auml;hren
  
  \date 2006/12/14
*/

// include of header files

#include <casa/Arrays/Array.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <casa/Containers/Block.h>
#include <casa/Inputs/Param.h>
#include <casa/Logging/LogMessage.h>
#include <casa/OS/SysEvent.h>
#include <casa/Quanta/Quantum.h>
#include <casa/Utilities/CountedPtr.h>
#include <casa/Utilities/DataType.h>
#include <casa/Utilities/Notice.h>

// include of implementation files

#include <casa/Containers/BlockIO.cc>
#include <casa/Containers/Link.cc>
#include <casa/Containers/List.cc>
#include <casa/Containers/ListIO.cc>
#include <casa/Containers/ListMap.cc>
#include <casa/Containers/ListMapIO.cc>
#include <casa/Containers/Map.cc>
#include <casa/Containers/MapIO.cc>
#include <casa/Containers/OrderedMap.cc>
#include <casa/Containers/OrderedPair.cc>
#include <casa/Containers/Queue.cc>
#include <casa/Containers/RecordField.cc>
#include <casa/Containers/RecordFieldWriter.cc>
#include <casa/Containers/SimOrdMap.cc>
#include <casa/Containers/SimOrdMapIO.cc>
#include <casa/Containers/Stack.cc>

#include <casa/Arrays/ArrayLogical.cc>
#include <casa/Containers/Dlink.cc>
#include <casa/Containers/Dlist.cc>
#include <casa/Containers/HashMap.cc>
#include <casa/Containers/HashMapIO.cc>
#include <casa/Containers/HashMapIter.cc>
#include <casa/Containers/ObjectPool.cc>
#include <casa/Containers/PoolStack.cc>

namespace casa {

  // ============================================================================
  //
  //  ReposFiller/templates
  //
  // ============================================================================

  template class Block<Int>;
  template class Block<uInt>;
  template class Block<Float>;
  template class Block<Double>;
  template class Block<String>;
  template class Block<IPosition>;
  template class Block<LogMessage>;

  // --------------------------------------------------------------------- AipsIO

  template AipsIO & operator>> (AipsIO &, Block<Int> &);
  template AipsIO & operator>> (AipsIO &, Block<uInt> &);
  template AipsIO & operator>> (AipsIO &, Block<String> &);

  template AipsIO & operator<< (AipsIO &, Block<Int> const &);
  template AipsIO & operator<< (AipsIO &, Block<uInt> const &);
  template AipsIO & operator<< (AipsIO &, Block<String> const &);

  template ostream & operator<< (ostream &, Block<String> const &);

  template void getBlock(AipsIO &, Block<String> &);
  template void getBlock(AipsIO &, Block<Int> &);
  template void getBlock(AipsIO &, Block<uInt> &);
  template void putBlock(AipsIO &, Block<uInt> const &, Int);
  template void showBlock(ostream &, Block<uInt> const &, Int);
  template void showBlock(ostream &, Block<String> const &, Int);

  // ----------------------------------------------------------------------- Link

  template class Link<void *>;
  template class Link<Int>;
  template class Link<uInt>;
  template class Link<uLong>;
  template class Link<Param>;
  template class Link<Vector<Int> *>;
  template class Link<Vector<uInt> *>;
  template class Link<Vector<Float> *>;
  template class Link<Vector<Double> *>;
  template class Link<CountedPtr<SysEventTargetInfo> >;
  template class Link<NoticeTarget *>;

  // ---------------------------------------------------------------- OrderedPair

  template class OrderedPair<Int, Bool>;
  template class OrderedPair<Int, Int>;
  template class OrderedPair<Int, uInt>;
  template class OrderedPair<Int, Float>;
  template class OrderedPair<Int, Double>;
  template class OrderedPair<Int, String>;
  template class OrderedPair<Int, DataType>;
  template class OrderedPair<Int, Array<Float> >;
  template class OrderedPair<Int, Vector<Float> >;
  template class OrderedPair<Int, Vector<Int> >;
  template class OrderedPair<Int, Vector<uInt> >;
  template class OrderedPair<Int, Vector<Double> >;
  template class OrderedPair<Int, Quantum<Double> >;
  template class OrderedPair<Int, Quantum<Array<Double> > >;
  template class OrderedPair<Int, Quantum<Vector<Double> > >;
  //
  template class OrderedPair<uInt, Int>;
  template class OrderedPair<uInt, uInt>;
  //
  template class OrderedPair<String, Bool>;
  template class OrderedPair<String, Int>;
  template class OrderedPair<String, uInt>;
  template class OrderedPair<String, String>;
  template class OrderedPair<String, void *>;
  template class OrderedPair<String, Block<IPosition> >;
  template class OrderedPair<String, Record *>;
  template <class T> class AutoDiffRep;
  template class OrderedPair<uInt, PoolStack<AutoDiffRep<Complex>, uInt> *>;
  template class OrderedPair<uInt, PoolStack<AutoDiffRep<DComplex>, uInt> *>;
  template class OrderedPair<uInt, PoolStack<AutoDiffRep<Double>, uInt> *>;
  template class OrderedPair<uInt, PoolStack<AutoDiffRep<Float>, uInt> *>;
  template class OrderedPair<Double, uInt>;
  template class OrderedPair<Double, Int>;
  template class OrderedPair<void *, void *>;

  // -------------------------------------------------------------- ConstListIter
  // ------------------------------------------------------------------- ListIter
  // ----------------------------------------------------------------------- List

  template class ConstListIter<void *>;
  template class ConstListIter<Int>;
  template class ConstListIter<uInt>;
  template class ConstListIter<Short>;
  template class ConstListIter<uShort>;
  template class ConstListIter<Long>;
  template class ConstListIter<uLong>;
  template class ConstListIter<Float>;
  template class ConstListIter<Double>;
  template class ConstListIter<Param>;
  template class ConstListIter<CountedPtr<SysEventTargetInfo> >;
  template class ConstListIter<OrderedPair<String, OrderedPair<String, uInt> > >;
  template class ConstListIter<OrderedPair<String, int> >;
  template class ConstListIter<OrderedPair<int, int> >;

  template class ListIter<void *>;
  template class ListIter<Int>;
  template class ListIter<uInt>;
  template class ListIter<Short>;
  template class ListIter<uShort>;
  template class ListIter<Long>;
  template class ListIter<uLong>;
  template class ListIter<Float>;
  template class ListIter<Double>;
  template class ListIter<Param>;
  template class ListIter<CountedPtr<SysEventTargetInfo> >;

  template class List<void *>;
  template class List<Int>;
  template class List<uInt>;
  template class List<Short>;
  template class List<uShort>;
  template class List<Long>;
  template class List<uLong>;
  template class List<Float>;
  template class List<Double>;
  template class List<Param>;
  template class List<Block<IPosition> >;
  template class List<OrderedPair<String, uInt> >;
  template class List<CountedPtr<SysEventTargetInfo> >;

  template class ListNotice<void *>;
  template class ListNotice<Int>;
  template class ListNotice<uInt>;
  template class ListNotice<Short>;
  template class ListNotice<uShort>;
  template class ListNotice<Long>;
  template class ListNotice<uLong>;
  template class ListNotice<Float>;
  template class ListNotice<Double>;
  template class ListNotice<Param>;
  template class ListNotice<Block<IPosition> >;
  template class ListNotice<OrderedPair<String, uInt> >;
  template class ListNotice<CountedPtr<SysEventTargetInfo> >;

  template class ListMap <Int, Int>;
  template class ListMap <Int, Array<Float> >;
  template class ListMap <Int, Vector<Float> >;
  template class ListMap <String, Int>;
  template class ListMap <String, Double>;
  template class ListMap <String, Block<IPosition> >;
  template class ListMap <String, OrderedPair<String, uInt> >;

  template class ListMapRep <Int, Int>;
  template class ListMapRep <Int, Array<Float> >;
  template class ListMapRep <Int, Vector<Float> >;
  template class ListMapRep <String, Int>;
  template class ListMapRep <String, Double>;
  template class ListMapRep <String, Block<IPosition> >;
  template class ListMapRep <String, OrderedPair<String, uInt> >;

  template class ListMapIterRep <Int, Int>;
  template class ListMapIterRep <Int, Array<Float> >;
  template class ListMapIterRep <Int, Vector<Float> >;
  template class ListMapIterRep <String, Int>;
  template class ListMapIterRep <String, Double>;
  template class ListMapIterRep <String, Block<IPosition> >;
  template class ListMapIterRep <String, OrderedPair<String, uInt> >;

  template ostream &operator<< (ostream &, const List<Int> &);
  template ostream &operator<< (ostream &, const List<uInt> &);
  template ostream &operator<< (ostream &, const List<Short> &);
  template ostream &operator<< (ostream &, const List<uShort> &);
  template ostream &operator<< (ostream &, const List<Long> &);
  template ostream &operator<< (ostream &, const List<uLong> &);
  template ostream &operator<< (ostream &, const List<Float> &);
  template ostream &operator<< (ostream &, const List<Double> &);
  template ostream &operator<< (ostream &, const List<Param> &);
  template ostream &operator<< (ostream &, const List<Block<IPosition> > &);
//   template ostream &operator<< (ostream &, const List<OrderedPair<String, uInt> > &);

  // ----------------------------------------------------------------- OrderedMap

  template class OrderedMapRep<Int, Int>;
  template class OrderedMapRep<Int, uInt>;
  template class OrderedMapRep<Int, Array<Float> >;
  template class OrderedMapRep<Int, Vector<Float> >;
  template class OrderedMapRep<String, Int>;
  template class OrderedMapRep<String, uInt>;
  template class OrderedMapRep<String, Block<IPosition> >;
  template class OrderedMapRep<String, OrderedPair<String, uInt> >;

  template class OrderedMap<Int, Int>;
  template class OrderedMap<Int, uInt>;
  template class OrderedMap<Int, Array<Float> >;
  template class OrderedMap<Int, Vector<Float> >;
  template class OrderedMap<String, Int>;
  template class OrderedMap<String, uInt>;
  template class OrderedMap<String, Block<IPosition> >;
  template class OrderedMap<String, OrderedPair<String, uInt> >;

  template class OrderedMapIterRep<Int, Int>;
  template class OrderedMapIterRep<Int, uInt>;
  template class OrderedMapIterRep<String, Block<IPosition> >;
  template class OrderedMapIterRep<String, Int>;
  template class OrderedMapIterRep<Int, Array<Float> >;
  template class OrderedMapIterRep<Int, Vector<Float> >;
  template class OrderedMapIterRep<String, OrderedPair<String, uInt> >;

  template class OrderedMapNotice<Int, Int>;
  template class OrderedMapNotice<Int, uInt>;
  template class OrderedMapNotice<Int, Array<Float> >;
  template class OrderedMapNotice<Int, Vector<Float> >;
  template class OrderedMapNotice<String, Int>;
  template class OrderedMapNotice<String, uInt>;
  template class OrderedMapNotice<String, Block<IPosition> >;
  template class OrderedMapNotice<String, OrderedPair<String, uInt> >;

  // --------------------------------------------------------------- ConstMapIter
  // -------------------------------------------------------------------- MapIter
  // ------------------------------------------------------------------------ Map
  // ----------------------------------------------------------------- MapIterRep

  template class ConstMapIter <Int, Int>;
  template class ConstMapIter <Int, Double>;
  template class ConstMapIter <Int, Array<Float> >;
  template class ConstMapIter <Int, Vector<Float> >;
  template class ConstMapIter <String, Int>;
  template class ConstMapIter <String, Double>;
  template class ConstMapIter <String, Block<IPosition> >;
  template class ConstMapIter <String, OrderedPair<String, uInt> >;

  template class MapIter <Int, Int>;
  template class MapIter <Int, Double>;
  template class MapIter <Int, Array<Float> >;
  template class MapIter <Int, Vector<Float> >;
  template class MapIter <String, Int>;
  template class MapIter <String, Double>;
  template class MapIter <String, Block<IPosition> >;
  template class MapIter <String, OrderedPair<String, uInt> >;

  template class Map <Int, Int>;
  template class Map <Int, Double>;
  template class Map <Int, Array<Float> >;
  template class Map <Int, Vector<Float> >;
  template class Map <String, Int>;
  template class Map <String, Double>;
  template class Map <String, Block<IPosition> >;
  template class Map <String, OrderedPair<String, uInt> >;

  template class MapIterRep <Int, Int>;
  template class MapIterRep <Int, Double>;
  template class MapIterRep <Int, Array<Float> >;
  template class MapIterRep <Int, Vector<Float> >;
  template class MapIterRep <String, Int>;
  template class MapIterRep <String, Double>;
  template class MapIterRep <String, Block<IPosition> >;
  template class MapIterRep <String, OrderedPair<String, uInt> >;

  template class MapRep <Int, Int>;
  template class MapRep <Int, Double>;
  template class MapRep <Int, Array<Float> >;
  template class MapRep <Int, Vector<Float> >;
  template class MapRep <String, Int>;
  template class MapRep <String, Double>;
  template class MapRep <String, Block<IPosition> >;
  template class MapRep <String, OrderedPair<String, uInt> >;

  template ostream &operator<< (ostream &, const Map<Int, Int>&);
  template ostream &operator<< (ostream &, const Map<Int, Double>&);
  template ostream &operator<< (ostream &, const Map<Int, Array<Float> >&);
  template ostream &operator<< (ostream &, const Map<Int, Vector<Float> >&);
  template ostream &operator<< (ostream &, const Map<String, Int>&);
  template ostream &operator<< (ostream &, const Map<String, Double>&);
  template ostream &operator<< (ostream &, const Map<String, Block<IPosition> >&);
//   template ostream &operator<< (ostream &, const Map<String, OrderedPair<String, uInt> >&);

  // ----------------------------------------------------------- SimpleOrderedMap

  template class SimpleOrderedMap<void *, void *>;
  template class OrderedPair<void*, Int>;
  //
  template class SimpleOrderedMap<Int, Bool>;
  template class SimpleOrderedMap<Int, Int>;
  template class SimpleOrderedMap<Int, uInt>;
  template class SimpleOrderedMap<Int, Double>;
  template class SimpleOrderedMap<Int, String>;
  template class SimpleOrderedMap<Int, DataType>;
  template class SimpleOrderedMap<Int, Vector<Int> >;
  template class SimpleOrderedMap<Int, Vector<uInt> >;
  template class SimpleOrderedMap<Int, Vector<Double> >;
  template class SimpleOrderedMap<Int, Quantum<Double> >;
  template class SimpleOrderedMap<Int, Quantum<Array<Double> > >;
  template class SimpleOrderedMap<Int, Quantum<Vector<Double> > >;
  //
  template class SimpleOrderedMap<String, Record *>;
  template class SimpleOrderedMap<String, Bool>;
  template class SimpleOrderedMap<String, Int>;
  template class SimpleOrderedMap<String, String>;
  template class SimpleOrderedMap<String, void *>;
  template class SimpleOrderedMap<void*, Int>;
  template <class T> class AutoDiffRep;
  template class SimpleOrderedMap<uInt, PoolStack<AutoDiffRep<Complex>, uInt>*>;
  template class SimpleOrderedMap<uInt, PoolStack<AutoDiffRep<DComplex>, uInt>*>;
  template class SimpleOrderedMap<uInt, PoolStack<AutoDiffRep<Double>, uInt>*>;
  template class SimpleOrderedMap<uInt, PoolStack<AutoDiffRep<Float>, uInt>*>;
  template class SimpleOrderedMap<uInt, Int>;
  template class SimpleOrderedMap<uInt, uInt>;
  template class SimpleOrderedMap<Double, uInt>;
  template class SimpleOrderedMap<Double, Int>;

  template ostream &operator<< (ostream &, const SimpleOrderedMap<Int, Bool>&);
  template ostream &operator<< (ostream &, const SimpleOrderedMap<Int, Int>&);
  template ostream &operator<< (ostream &, const SimpleOrderedMap<Int, uInt>&);
  template ostream &operator<< (ostream &, const SimpleOrderedMap<String, Bool>&);
  template ostream &operator<< (ostream &, const SimpleOrderedMap<String, Int>&);
  template ostream &operator<< (ostream &, const SimpleOrderedMap<String, String>&);

  template AipsIO & operator<< (AipsIO &, SimpleOrderedMap<Int, Bool> const &);
  template AipsIO & operator<< (AipsIO &, SimpleOrderedMap<Int, Int> const &);
  template AipsIO & operator<< (AipsIO &, SimpleOrderedMap<Int, uInt> const &);
  template AipsIO & operator<< (AipsIO &, SimpleOrderedMap<String, Bool> const &);
  template AipsIO & operator<< (AipsIO &, SimpleOrderedMap<String, Int> const &);
  template AipsIO & operator<< (AipsIO &, SimpleOrderedMap<String, uInt> const &);

  template AipsIO & operator>> (AipsIO &, SimpleOrderedMap<Int, Bool> &);
  template AipsIO & operator>> (AipsIO &, SimpleOrderedMap<Int, Int> &);
  template AipsIO & operator>> (AipsIO &, SimpleOrderedMap<Int, uInt> &);
  template AipsIO & operator>> (AipsIO &, SimpleOrderedMap<String, uInt> &);
  template AipsIO & operator>> (AipsIO &, SimpleOrderedMap<String, Int> &);
  template AipsIO & operator>> (AipsIO &, SimpleOrderedMap<String, String> &);

  // ---------------------------------------------------------------------- Queue

  template class Queue<Int>;
  template class Queue<uInt>;
  template class Queue<Float>;
  template class Queue<Double>;
  template class Queue<String>;
  template class Queue<IPosition>;
  template class Queue<LogMessage>;

  // ----------------------------------------------------------- RORecordFieldPtr
  // ------------------------------------------------------------- RecordFieldPtr

  template class RORecordFieldPtr<Complex>;
  template class RORecordFieldPtr<DComplex>;
  template class RORecordFieldPtr<Bool>;
  template class RORecordFieldPtr<Double>;
  template class RORecordFieldPtr<Float>;
  template class RORecordFieldPtr<Int>;
  template class RORecordFieldPtr<Record>;
  template class RORecordFieldPtr<Short>;
  template class RORecordFieldPtr<String>;
  template class RORecordFieldPtr<uChar>;
  template class RORecordFieldPtr<uInt>;
  template class RORecordFieldPtr<Array<Complex> >;
  template class RORecordFieldPtr<Array<DComplex> >;
  template class RORecordFieldPtr<Array<Bool> >;
  template class RORecordFieldPtr<Array<Char> >;
  template class RORecordFieldPtr<Array<Double> >;
  template class RORecordFieldPtr<Array<Float> >;
  template class RORecordFieldPtr<Array<Int> >;
  template class RORecordFieldPtr<Array<Short> >;
  template class RORecordFieldPtr<Array<String> >;
  template class RORecordFieldPtr<Array<uChar> >;
  template class RORecordFieldPtr<Array<uInt> >;
  //
  template class RecordFieldPtr<Bool>;
  template class RecordFieldPtr<Complex>;
  template class RecordFieldPtr<DComplex>;
  template class RecordFieldPtr<Double>;
  template class RecordFieldPtr<Float>;
  template class RecordFieldPtr<Int>;
  template class RecordFieldPtr<Record>;
  template class RecordFieldPtr<Short>;
  template class RecordFieldPtr<String>;
  template class RecordFieldPtr<uChar>;
  template class RecordFieldPtr<uInt>;
  template class RecordFieldPtr<Array<Bool> >;
  template class RecordFieldPtr<Array<Char> >;
  template class RecordFieldPtr<Array<Complex> >;
  template class RecordFieldPtr<Array<DComplex> >;
  template class RecordFieldPtr<Array<Double> >;
  template class RecordFieldPtr<Array<Float> >;
  template class RecordFieldPtr<Array<Int> >;
  template class RecordFieldPtr<Array<Short> >;
  template class RecordFieldPtr<Array<String> >;
  template class RecordFieldPtr<Array<uChar> >;
  template class RecordFieldPtr<Array<uInt> >;

  template class RecordFieldCopier<Array<Bool>, Array<Bool> >;
  template class RecordFieldCopier<Array<Complex>, Array<Complex> >;
  template class RecordFieldCopier<Array<DComplex>, Array<DComplex> >;
  template class RecordFieldCopier<Array<Double>, Array<Double> >;
  template class RecordFieldCopier<Array<Float>, Array<Float> >;
  template class RecordFieldCopier<Array<Int>, Array<Int> >;
  template class RecordFieldCopier<Array<Short>, Array<Short> >;
  template class RecordFieldCopier<Array<String>, Array<String> >;
  template class RecordFieldCopier<Array<uChar>, Array<uChar> >;
  template class RecordFieldCopier<Bool, Bool>;
  template class RecordFieldCopier<Complex, Complex>;
  template class RecordFieldCopier<DComplex, DComplex>;
  template class RecordFieldCopier<Double, Double>;
  template class RecordFieldCopier<Float, Float>;
  template class RecordFieldCopier<Float, Int>;
  template class RecordFieldCopier<Int, Int>;
  template class RecordFieldCopier<Short, Short>;
  template class RecordFieldCopier<String, String>;
  template class RecordFieldCopier<uChar, uChar>;
  template class RecordFieldCopier<Double, Float>;
  template class RecordFieldCopier<Int, Short>;

  template class Stack<Int>;
  template class Stack<uInt>;
  template class Stack<Short>;
  template class Stack<uShort>;
  template class Stack<Long>;
  template class Stack<uLong>;
  template class Stack<Float>;
  template class Stack<Double>;
  template class Stack<Complex>;
  template class Stack<DComplex>;
  template class Stack<Vector<Double> *>;
  template class Stack<Vector<Float> *>;
  template class Stack<void *>;
  
  template ostream &operator<< (ostream &, const ConstListIter<Int> &);

  // ============================================================================
  //
  //  Containers/test/templates
  //
  // ============================================================================

  template class Dlink<Int>;
  template class Dlink<uInt>;
  template class Dlink<Float>;
  template class Dlink<Double>;

  template class Dlist<Int>;
  template class Dlist<uInt>;

  template class ConstDlistIter<Int>;
  template class DlistIter<Int>;

  template class HashClass<String>;
  template class HashMap<String, Int>;

  template ostream &operator<<(ostream &, HashMap<String, Int> const &);

  template class ConstHashMapIter<String, Int>;
  template class HashMapIter<String, Int>;

  template class Link<OrderedPair<Int, Int> >;
  template class Link<OrderedPair<Int, uInt> >;
  template class Link<OrderedPair<String, Int> >;
  template class Link<OrderedPair<String, uInt> >;
  template class Link<OrderedPair<String, OrderedPair<String, uInt> > >;

  // tObjectPool
  template class ObjectPool <Vector<Double>, uInt>;

  // tPoolStack
  template class PoolStack<Vector<Double>, uInt>;

}
