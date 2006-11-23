
#include <lopes/Analysis/Masking.h>

Array<Bool> Masking::mergeMasks (const Array<Bool>& mask1,
				 const Array<Bool>& mask2,
				 const String logic)
{
  IPosition shape1 = mask1.shape();
  IPosition shape2 = mask2.shape();
  IPosition shapeFallback (1);
  Array<Bool> mask;
  
  // Check if the shape of the two arrays is consistent
  if (shape1 != shape2) {
    cerr << "[Masking::mergeMasks] Shape of the two are is inconsistent!" << endl;
    cerr << "\tArray 1 : " << shape1 << endl;
    cerr << "\tArray 2 : " << shape2 << endl;
    mask.resize(shapeFallback);
    mask = False;
  }
  else {
    if (logic == "AND") {
      cout << "[Masking::mergeMasks] AND" << endl;
      mask = (mask1 && mask2);
    }
    else if (logic == "NAND") {
      cout << "[Masking::mergeMasks] NAND" << endl;
      mask = !(mask1 && mask2);
    }
    else if (logic == "OR") {
      cout << "[Masking::mergeMasks] OR" << endl;
      mask = (mask1 || mask2);
    }
    else if (logic == "NOR") {
      cout << "[Masking::mergeMasks] NOR" << endl;
      mask = !(mask1 || mask2);
    }
    else {
      cerr << "[Masking::mergeMasks] Unsupported merging option "
	   << logic << endl;
      mask.resize(shapeFallback);
      mask = False;
    }
  }
  return mask;
}

void Masking::setSubmask (Vector<Bool>& outMask, 
			  const Vector<Bool>& inMask,
			  const Vector<Int>& inIndex)
{
  Int nOutMask;
  Int nInMask;
  Int nInIndex;
  
  nOutMask = outMask.nelements();
  nInMask = inMask.nelements();
  nInIndex = inIndex.nelements();
  
  // 
  if (nInIndex == nOutMask) {
    for (Int i=0; i<nOutMask; i++) {
      if (inMask(inIndex(i))) {
	outMask(i) = True;
      } else {
	outMask(i) = False;
      }
    }
  } else {
    outMask.resize(nOutMask);
    outMask = True;
  }
  
}

Vector<Bool> Masking::maskFromRange (const Vector<Double>& range,
				     const Vector<Double>& values)
{
  IPosition shapeRange = range.shape();
  uInt nofValues = values.nelements();
  Vector<Bool> mask(nofValues,False);

  /*
    Step through all alement of the data vector and check if they lie within
    the specified range.
  */
  if (range.nelements() == 2) {
    Double rangeMin = min(range);
    Double rangeMax = max(range);
    for (uInt k=0; k<nofValues; k++) {
      if (values(k) >= rangeMin && values(k) <= rangeMax) {
	mask(k) = True;
      }
    }
  } else {
    cerr << "[Masking::maskFromRange] Invalid number of element in range vector"
	 << endl;
  }

  return mask;
}

uInt Masking::validElements (const Vector<Bool>& mask) 
{
  uInt nofElements = mask.nelements();
  uInt validElements;

  validElements = 0;
  for (uInt i=0; i<nofElements; i++) {
    validElements += uInt(mask(i));
  }

  return validElements;
}

uInt Masking::invalidElements (const Vector<Bool>& mask) 
{
  uInt nofElements = mask.nelements();
  uInt invalidElements;

  invalidElements = nofElements;
  for (uInt i=0; i<nofElements; i++) {
    invalidElements -= uInt(mask(i));
  }

  return invalidElements;
}
