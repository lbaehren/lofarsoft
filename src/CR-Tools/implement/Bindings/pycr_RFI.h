/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 4824 2010-05-06 09:07:23Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                  *
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

#ifndef PYCR_RFI_H
#define PYCR_RFI_H

// Standard library header files
#include <iostream>
#include <string>

#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/numeric.hpp>
#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>

#include <crtools.h>

namespace bpl = boost::python;

namespace CR { // Namespace CR -- begin
  namespace PYCR { // Namespace PYCR -- begin
    
    /*!
      \ingroup CR
      \ingroup pycr
      
      \brief Python bindings for the RFI-mitigation class
      
      \author Andreas Horneffer
      
      \date 2010/06/21
      
      <h3>Prerequisite</h3>
      
      <ul type="square">
      <li>[start filling in your text here]
      </ul>
      
      <h3>Synopsis</h3>
      
      <h3>Example(s)</h3>
      
    */  
    
    // === "Public" functions used in the bindings =========================================
    /*!
      \brief export for the RFIMitigation class

      Call this function from a main boost-python "export block" to add the
      wrappers for the RFIMitigation class to the libraray.
    */
    void export_RFIMitigation();
    
    // === "Private" functions only used internally ========================================
    /*!
      \brief Apply the RFI-mitigation to a block of data
      
      \param pyFFTdata -- numpy array with frequency-domain data, gets modified
      
      \return True on success
    */
    bool applyRFIMitigation(bpl::numeric::array &pyFFTdata); 
   
  } // Namespace PYCR -- end
  
} // Namespace CR -- end

#endif /* PYCR_RFI_H */
  
