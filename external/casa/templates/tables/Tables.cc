
// include of header files

#include <casa/BasicSL/Complex.h>
#include <casa/BasicSL/String.h>
#include <tables/Tables/TableRecord.h>
#include <tables/Tables/TableVector.h>
#include <tables/Tables/TVec.h>

// include of implementation files

#include <tables/Tables/ArrColData.cc>
#include <tables/Tables/ArrColDesc.cc>
#include <tables/Tables/ArrayColumn.cc>
#include <tables/Tables/BaseMappedArrayEngine.cc>
#include <tables/Tables/MappedArrayEngine.cc>
#include <tables/Tables/ScaColData.cc>
#include <tables/Tables/ScaColDesc.cc>
#include <tables/Tables/ScalarColumn.cc>
#include <tables/Tables/TVec.cc>
#include <tables/Tables/TVecMath.cc>
#include <tables/Tables/TVecScaCol.cc>
#include <tables/Tables/TVecTemp.cc>
#include <tables/Tables/TabVecMath.cc>
#include <tables/Tables/TableVector.cc>
#include <tables/Tables/VirtArrCol.cc>
#include <tables/Tables/VirtScaCol.cc>

namespace casa {

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

}
