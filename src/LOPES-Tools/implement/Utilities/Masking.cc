/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <Utilities/Masking.h>

namespace LOPES {  // namespace LOPES -- begin
  
  Array<bool> mergeMasks (const Array<bool>& mask1,
			  const Array<bool>& mask2,
			  const String logic)
  {
    IPosition shape1 = mask1.shape();
    IPosition shape2 = mask2.shape();
    IPosition shapeFallback (1);
    Array<bool> mask;
    
    // Check if the shape of the two arrays is consistent
    if (shape1 != shape2) {
      cerr << "[mergeMasks] Shape of the two are is inconsistent!" << endl;
      cerr << "\tArray 1 : " << shape1 << endl;
      cerr << "\tArray 2 : " << shape2 << endl;
      mask.resize(shapeFallback);
      mask = false;
    }
    else {
      if (logic == "AND") {
	cout << "[mergeMasks] AND" << endl;
	mask = (mask1 && mask2);
      }
      else if (logic == "NAND") {
	cout << "[mergeMasks] NAND" << endl;
	mask = !(mask1 && mask2);
      }
      else if (logic == "OR") {
	cout << "[mergeMasks] OR" << endl;
	mask = (mask1 || mask2);
      }
      else if (logic == "NOR") {
	cout << "[mergeMasks] NOR" << endl;
	mask = !(mask1 || mask2);
      }
      else {
	cerr << "[mergeMasks] Unsupported merging option "
	     << logic << endl;
	mask.resize(shapeFallback);
	mask = false;
      }
    }
    return mask;
  }
  
  void setSubmask (Vector<bool>& outMask, 
		   const Vector<bool>& inMask,
		   const Vector<int>& inIndex)
  {
    int nOutMask;
    int nInMask;
    int nInIndex;
    
    nOutMask = outMask.nelements();
    nInMask = inMask.nelements();
    nInIndex = inIndex.nelements();
    
    // 
    if (nInIndex == nOutMask) {
      for (int i=0; i<nOutMask; i++) {
	if (inMask(inIndex(i))) {
	  outMask(i) = true;
	} else {
	  outMask(i) = false;
	}
      }
    } else {
      outMask.resize(nOutMask);
      outMask = true;
    }
    
  }
  
  Vector<bool> maskFromRange (const Vector<double>& range,
			      const Vector<double>& values)
  {
    IPosition shapeRange = range.shape();
    uint nofValues = values.nelements();
    Vector<bool> mask(nofValues,false);
    
    /*
      Step through all alement of the data vector and check if they lie within
      the specified range.
    */
    if (range.nelements() == 2) {
      double rangeMin = min(range);
      double rangeMax = max(range);
      for (uint k=0; k<nofValues; k++) {
	if (values(k) >= rangeMin && values(k) <= rangeMax) {
	  mask(k) = true;
	}
      }
    } else {
      cerr << "[maskFromRange] Invalid number of element in range vector"
	   << endl;
    }
    
    return mask;
  }
  
  uint validElements (const Vector<bool>& mask) 
  {
    uint nofElements = mask.nelements();
    uint validElements;
    
    validElements = 0;
    for (uint i=0; i<nofElements; i++) {
      validElements += uint(mask(i));
    }
    
    return validElements;
  }
  
  uint invalidElements (const Vector<bool>& mask) 
  {
    uint nofElements = mask.nelements();
    uint invalidElements;
    
    invalidElements = nofElements;
    for (uint i=0; i<nofElements; i++) {
      invalidElements -= uint(mask(i));
    }
    
    return invalidElements;
  }
  
}  // namespace LOPES -- end
