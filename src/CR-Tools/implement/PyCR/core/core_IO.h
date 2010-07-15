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

#ifndef PYCR_IO_H
#define PYCR_IO_H

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
#include "IO/DataReader.h"

namespace bpl = boost::python;

namespace CR { // Namespace CR -- begin
  namespace PYCR { // Namespace PYCR -- begin
    
    /*!
      \ingroup CR
      \ingroup core
      
      \brief Python bindings for the Datareader classes
      
      \author Andreas Horneffer
      
      \date 2010/06/13
      
      <h3>Prerequisite</h3>
      
      <ul type="square">
      <li>[start filling in your text here]
      </ul>
      
      <h3>Synopsis</h3>
      
      <h3>Example(s)</h3>
      
    */  
    
    // === "Public" functions used in the bindings =========================================
    /*!
      \brief boost-python style export of the DataReader

      Call this function from a main boost-python "export block" to add the
      wrappers for the DataReader to the libraray.
    */
    void export_DataReader();
    
    // === "Private" functions only used internally ========================================
    /*!
      \brief Open a file and return the corresponding DataReader object
      
      \param Filename -- name (path) of the file to be opened
      
      \return the newly created DataReader object

      This version tries to guess the filetype from the filename.
    */
    DataReader & CRFileOpen(std::string Filename); 

    /*!
      \brief Open a file and return the corresponding DataReader object
      
      \param Filename -- name (path) of the file to be opened
      \param filetype -- the type of the file, currently supported are "LOPESEvent" and "LOFAR_TBB"
      
      \return the newly created DataReader object
    */
    DataReader & CRFileOpenType(std::string Filename, std::string filetype);

    /*!
      \brief Read data from a file and return it in the given numpy array
      
      \param dr       -- an existing DataReader Object
      \param Datatype -- name of the field that is to be read out
      \param pydata   -- the array in which the data is returned
      
      \return True on success
    */
    bool CRFileRead(DataReader &dr, std::string Datatype, bpl::numeric::array &pydata );
    
    /*!
      \brief Read a parameter from a DataReader object
      
      \param dr  -- an existing DataReader Object
      \param key -- name of the field that is to be read out
      
      \return a newly created object with the data
    */
    bpl::object CRFileGetParameter(DataReader &dr, std::string key);
    
    /*!
      \brief Set a parameter in a DataReader object
      
      \param dr   -- an existing DataReader Object
      \param key  -- name of the field that is to be set
      \param pyob -- Python object with the data
      
      \return True on success
    */
    bool CRFileSetParameter(DataReader &dr, std::string key, bpl::object &pyob);
   
  } // Namespace PYCR -- end
  
} // Namespace CR -- end

#endif /* PYCR_IO_H */
  
