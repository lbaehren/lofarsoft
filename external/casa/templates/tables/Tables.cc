
// include of header files

#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <casa/Containers/Block.h>
#include <casa/Utilities/ValTypeId.h>
#include <tables/Tables/TableRecord.h>
#include <tables/Tables/TableVector.h>
#include <tables/Tables/TVec.h>
#include <tables/Tables/test/tTableDesc.h>
#include <tables/Tables/test/dVSCEngine.h>

// include of implementation files

#include <casa/Arrays/Array.cc>
#include <casa/Arrays/ArrayIter.cc>
#include <casa/Arrays/MaskedArray.cc>
#include <casa/Arrays/Vector.cc>
#include <casa/Utilities/Compare.cc>
#include <casa/Utilities/CountedPtr.cc>
#include <tables/Tables/ArrColData.cc>
#include <tables/Tables/ArrColDesc.cc>
#include <tables/Tables/ArrayColumn.cc>
#include <tables/Tables/BaseMappedArrayEngine.cc>
#include <tables/Tables/MappedArrayEngine.cc>
#include <tables/Tables/ScaColData.cc>
#include <tables/Tables/ScaColDesc.cc>
#include <tables/Tables/ScalarColumn.cc>
#include <tables/Tables/ScaledArrayEngine.cc>
#include <tables/Tables/ScaledComplexData.cc>
#include <tables/Tables/TableVector.cc>
#include <tables/Tables/TabVecLogic.cc>
#include <tables/Tables/TabVecMath.cc>
#include <tables/Tables/TVec.cc>
#include <tables/Tables/TVecLogic.cc>
#include <tables/Tables/TVecMath.cc>
#include <tables/Tables/TVecScaCol.cc>
#include <tables/Tables/TVecTemp.cc>
#include <tables/Tables/VirtArrCol.cc>
#include <tables/Tables/VirtScaCol.cc>
#include <tables/Tables/VSCEngine.cc>

namespace casa {

  // -- tables/Tables/ArrColData.cc
  template class ArrayColumnData<Bool>;
  template class ArrayColumnData<Char>;
  template class ArrayColumnData<uChar>;
  template class ArrayColumnData<Short>;
  template class ArrayColumnData<uShort>;
  template class ArrayColumnData<Int>;
  template class ArrayColumnData<uInt>;
  template class ArrayColumnData<Float>;
  template class ArrayColumnData<Double>;
  template class ArrayColumnData<Complex>;
  template class ArrayColumnData<DComplex>;
  template class ArrayColumnData<String>;

  // -- tables/Tables/ArrColDesc.cc
  template class ArrayColumnDesc<Bool>;
  template class ArrayColumnDesc<Char>;
  template class ArrayColumnDesc<uChar>;
  template class ArrayColumnDesc<Short>;
  template class ArrayColumnDesc<uShort>;
  template class ArrayColumnDesc<Int>;
  template class ArrayColumnDesc<uInt>;
  template class ArrayColumnDesc<Float>;
  template class ArrayColumnDesc<Double>;
  template class ArrayColumnDesc<Complex>;
  template class ArrayColumnDesc<DComplex>;
  template class ArrayColumnDesc<String>;

  template class ArrayColumn<Bool>;
  template class ArrayColumn<uChar>;
  template class ArrayColumn<Short>;
  template class ArrayColumn<uShort>;
  template class ArrayColumn<Int>;
  template class ArrayColumn<uInt>;
  template class ArrayColumn<Float>;
  template class ArrayColumn<Double>;
  template class ArrayColumn<Complex>;
  template class ArrayColumn<DComplex>;
  template class ArrayColumn<String>;
  template class ROArrayColumn<Bool>;
  template class ROArrayColumn<uChar>;
  template class ROArrayColumn<uShort>;
  template class ROArrayColumn<Short>;
  template class ROArrayColumn<Int>;
  template class ROArrayColumn<uInt>;
  template class ROArrayColumn<Float>;
  template class ROArrayColumn<Double>;
  template class ROArrayColumn<Complex>;
  template class ROArrayColumn<DComplex>;
  template class ROArrayColumn<String>;

  template class BaseMappedArrayEngine<Complex, Int>;
  template class BaseMappedArrayEngine<Float, Short>;

  template class MappedArrayEngine<Complex, DComplex>;
  template class BaseMappedArrayEngine<Complex, DComplex>;

  template class ScalarColumnData<Bool>;
  template class ScalarColumnData<Char>;
  template class ScalarColumnData<uChar>;
  template class ScalarColumnData<Short>;
  template class ScalarColumnData<uShort>;
  template class ScalarColumnData<Int>;
  template class ScalarColumnData<uInt>;
  template class ScalarColumnData<Float>;
  template class ScalarColumnData<Double>;
  template class ScalarColumnData<Complex>;
  template class ScalarColumnData<DComplex>;
  template class ScalarColumnData<String>;

  template class ScalarColumnDesc<Bool>;
  template class ScalarColumnDesc<Char>;
  template class ScalarColumnDesc<uChar>;
  template class ScalarColumnDesc<Short>;
  template class ScalarColumnDesc<uShort>;
  template class ScalarColumnDesc<Int>;
  template class ScalarColumnDesc<uInt>;
  template class ScalarColumnDesc<Float>;
  template class ScalarColumnDesc<Double>;
  template class ScalarColumnDesc<Complex>;
  template class ScalarColumnDesc<DComplex>;
  template class ScalarColumnDesc<String>;

  template class ROScalarColumn<Bool>;
  template class ROScalarColumn<uChar>;
  template class ROScalarColumn<Short>;
  template class ROScalarColumn<uShort>;
  template class ROScalarColumn<Int>;
  template class ROScalarColumn<uInt>;
  template class ROScalarColumn<Float>;
  template class ROScalarColumn<Double>;
  template class ROScalarColumn<Complex>;
  template class ROScalarColumn<DComplex>;
  template class ROScalarColumn<String>;
  template class ROScalarColumn<TableRecord>;
  template class ScalarColumn<Bool>;
  template class ScalarColumn<uChar>;
  template class ScalarColumn<Short>;
  template class ScalarColumn<uShort>;
  template class ScalarColumn<Int>;
  template class ScalarColumn<uInt>;
  template class ScalarColumn<Float>;
  template class ScalarColumn<Double>;
  template class ScalarColumn<Complex>;
  template class ScalarColumn<DComplex>;
  template class ScalarColumn<String>;
  template class ScalarColumn<TableRecord>;

  template class TabVecRep<Double>;
  template class TabVecRep<Int>;

  template void tabVecRepminmax(Int &, Int &, TabVecRep<Int> const &);

  template class TabVecScaCol<Double>;
  template class TabVecScaCol<Int>;

  template class TabVecTemp<Double>;
  template class TabVecTemp<Int>;

  template Int max(ROTableVector<Int> const &);

  template class ROTableVector<Int>;
  template class TableVector<Double>;
  template class ROTableVector<Double>;
  template class TableVector<Int>;

  template class VirtualArrayColumn<Float>;
  template class VirtualArrayColumn<Complex>;

  template class VirtualScalarColumn<Float>;

  // ============================================================================

  // -- tables/Tables/BaseMappedArrayEngine.cc
  // -- tables/Tables/ScaledArrayEngine.cc 
  template class BaseMappedArrayEngine<Double, Int>;
  template class BaseMappedArrayEngine<Float, uChar>;
  template class ScaledArrayEngine<Double, Int>;
  template class ScaledArrayEngine<Float, uChar>;

  // -- tables/Tables/MappedArrayEngine.cc 
  // -- tables/Tables/BaseMappedArrayEngine.cc 
  template class BaseMappedArrayEngine<Float, uShort>;
  template class MappedArrayEngine<Float, uShort>;

  // -- tables/Tables/ScaledComplexData.cc 
  // -- tables/Tables/BaseMappedArrayEngine.cc 
  // -- tables/Tables/VirtArrCol.cc 
  // -- casa/BasicSL/Complex.h 
  template class BaseMappedArrayEngine<DComplex, Int>;
  template class BaseMappedArrayEngine<Complex, Short>;
  template class ScaledComplexData<DComplex, Int>;
  template class ScaledComplexData<Complex, Short>;
  template class VirtualArrayColumn<DComplex>;

  // -- tables/Tables/TabVecLogic.cc
  // -- tables/Tables/TabVecMath.cc 
  // -- tables/Tables/TVecLogic.cc
  // -- tables/Tables/TVecMath.cc 
  template Bool allEQ(ROTableVector<Double> const &, ROTableVector<Double> const &);
  template Bool allEQ(ROTableVector<Float> const &, ROTableVector<Float> const &);
  template Bool allEQ(ROTableVector<Int> const &, ROTableVector<Int> const &);
  template Bool allNE(ROTableVector<Int> const &, ROTableVector<Int> const &);
  template Bool anyEQ(ROTableVector<Int> const &, ROTableVector<Int> const &);
  template Bool anyNE(ROTableVector<Double> const &, ROTableVector<Double> const &);
  template Bool anyNE(ROTableVector<Float> const &, ROTableVector<Float> const &);
  template Bool anyNE(ROTableVector<Int> const &, ROTableVector<Int> const &);
  template void operator*=(TableVector<Int> &, Int const &);
  template TableVector<Int> operator+(ROTableVector<Int> const &, Int const &);
  template TableVector<Int> operator+(ROTableVector<Int> const &, ROTableVector<Int> const &);
  template void operator+=(TableVector<Int> &, ROTableVector<Int> const &);
  template Bool tabVecReptvEQ(TabVecRep<Double> const &, TabVecRep<Double> const &);
  template Bool tabVecReptvEQ(TabVecRep<Float> const &, TabVecRep<Float> const &);
  template Bool tabVecReptvEQ(TabVecRep<Int> const &, TabVecRep<Int> const &);
  template Bool tabVecReptvNE(TabVecRep<Int> const &, TabVecRep<Int> const &);
  template TabVecRep<Int> & tabVecReptvadd(TabVecRep<Int> const &, TabVecRep<Int> const &);
  template void tabVecReptvassadd(TabVecRep<Int> &, TabVecRep<Int> const &);
  template void tabVecRepvalasstim(TabVecRep<Int> &, Int const &);
  template TabVecRep<Int> & tabVecRepvalradd(TabVecRep<Int> const &, Int const &);

  // -- tables/Tables/TableVector.cc
  // -- tables/Tables/TVec.cc 
  // -- tables/Tables/TVecScaCol.cc
  // -- tables/Tables/TVecTemp.cc 
  template class ROTableVector<Float>;
  template class TableVector<Float>;
  template class TabVecRep<Float>;
  template class TabVecScaCol<Float>;
  template class TabVecTemp<Float>;

  // -- tables/Tables/VirtArrCol.cc
  // -- tables/Tables/VirtScaCol.cc 
  template class VirtualArrayColumn<Double>;
  template class VirtualScalarColumn<Double>;
  
  // -- casa/Arrays/Array.cc 
  // -- casa/Arrays/MaskedArray.cc 
  // -- casa/Arrays/Vector.cc
  // -- tables/Tables/ScaColData.cc 
  // -- tables/Tables/ScaColDesc.cc
  // -- tables/Tables/ScalarColumn.cc 
  // -- tables/Tables/VSCEngine.cc
  // -- tables/Tables/VirtScaCol.cc 
  // -- casa/Utilities/Compare.cc 
  // -- casa/Utilities/CountedPtr.cc
  template class Array<VSCExample>;
  template class MaskedArray<VSCExample>;
  template class Vector<VSCExample>;
  template class ScalarColumnData<VSCExample>;
  template class ScalarColumnDesc<VSCExample>;
  template class ROScalarColumn<VSCExample>;
  template class ScalarColumn<VSCExample>;
  template class VSCEngine<VSCExample>;
  template class VirtualScalarColumn<VSCExample>;
  template class ObjCompare<VSCExample>;
  template class CountedConstPtr<Block<VSCExample> >;
  template class CountedPtr<Block<VSCExample> >;
  template class PtrRep<Block<VSCExample> >;
  template class SimpleCountedConstPtr<Block<VSCExample> >;
  template class SimpleCountedPtr<Block<VSCExample> >;
  template String valDataTypeId(VSCExample const *);
  
  // -- casa/Arrays/Array.cc 
  // -- casa/Arrays/ArrayIter.cc 
  // -- casa/Arrays/MaskedArray.cc
  // -- casa/Arrays/Vector.cc 
  // -- tables/Tables/ArrColData.cc
  // -- tables/Tables/ArrColDesc.cc 
  // -- tables/Tables/ScaColData.cc
  // -- tables/Tables/ScaColDesc.cc 
  // -- casa/Utilities/Compare.cc 
  // -- casa/Utilities/CountedPtr.cc
  template class Array<ExampleDesc>;
  template class ArrayIterator<ExampleDesc>;
  template class ReadOnlyArrayIterator<ExampleDesc>;
  template class MaskedArray<ExampleDesc>;
  template class Vector<ExampleDesc>;
  template class ArrayColumnData<ExampleDesc>;
  template class ArrayColumnDesc<ExampleDesc>;
  template class ScalarColumnData<ExampleDesc>;
  template class ScalarColumnDesc<ExampleDesc>;
  template class ObjCompare<ExampleDesc>;
  template class CountedConstPtr<Block<ExampleDesc> >;
  template class CountedPtr<Block<ExampleDesc> >;
  template class PtrRep<Block<ExampleDesc> >;
  template class SimpleCountedConstPtr<Block<ExampleDesc> >;
  template class SimpleCountedPtr<Block<ExampleDesc> >;
  template String valDataTypeId(ExampleDesc const *);
  
}
