/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

/* $Id: StoredInputObject.cc,v 1.2 2005/07/27 15:33:06 horneff Exp $*/

#include <LopesBase/StoredInputObject.h>

using std::cerr;
using std::cout;
using std::endl;

using casa::AipsError;
using casa::Complex;
using casa::DComplex;

namespace LOPES {  // Namespace LOPES -- begin
  
  // --- Construction ------------------------------------------------------------
  
  template<class T> StoredInputObject<T>::StoredInputObject ()
    : StoredObject<T>() {
    className_p = "StoredInputObject";
    nParent_p = 0;
  }
  
  // --- Destruction -------------------------------------------------------------
  
  template<class T> StoredInputObject<T>::~StoredInputObject ()
  {;}
  
  // --- Parameters --------------------------------------------------------------
  template<class T> bool StoredInputObject<T>::put(Vector<T> *ref) {
    try {
      data_p.resize();
      data_p = *ref;
      invalidate();
      valid_p = true;
    } catch (AipsError x) {
      cerr << x.getMesg() << endl;
      return false;
    };
    return true;
  };
  
  template<class T> bool StoredInputObject<T>::put(Vector<T> *ref, bool copy) {
    try {
      if (copy) {
	data_p.resize();
	data_p = *ref;
      } else {
	data_p.reference(*ref);
	invalidate();
	valid_p = true;
      };
    } catch (AipsError x) {
      cerr << x.getMesg() << endl;
      return false;
    };
    return true;
  };
    
  // ============================================================================
  //
  //  Template instantiation
  //
  // ============================================================================
  
  template StoredInputObject<float>;
  template StoredInputObject<double>;
  template StoredInputObject<Complex>;
  template StoredInputObject<DComplex>;
  
}  // Namespace LOPES -- end
