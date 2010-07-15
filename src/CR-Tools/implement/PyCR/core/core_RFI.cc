/*-------------------------------------------------------------------------*
 | $Id::    NewClass.cc 4824 2010-05-06 09:07:23Z baehren                $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                    *
 *   Andreas Horneffer (A.Horneffer@astro.ru.nl)                           *
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

#include <PyCR/core/core_RFI.h>

#include <PyCR/core/core_numpycasa.h>
#include <Calibration/RFIMitigationPlugin.h>

namespace CR { // Namespace CR -- begin
  namespace PYCR { // Namespace PYCR -- begin
  
    // ------------------------------------------------------- applyRFIMitigation    
    bool applyRFIMitigation(bpl::numeric::array &pyFFTdata){
      casa::Array<casa::DComplex> data;
      casaFromNumpy(pyFFTdata, data);
      if (data.ndim() > 2) {
	cerr << "applyRFIMitigation: Illegal parameter: number of dimensions > 2" << endl;
	return false;
      };
      RFIMitigationPlugin rfiM;
      casa::Matrix<casa::DComplex> datamatrix(data); // this references the original storage
      rfiM.apply(datamatrix);
      rfiM.apply(datamatrix);
      return true;
    }; 


    // -------------------------------------------------------- export_RFIMitigation
    void export_RFIMitigation() { 
      def("applyRFIMitigation", &CR::PYCR::applyRFIMitigation);
    }
    
  } // Namespace PYCR -- end
  
} // Namespace CR -- end
