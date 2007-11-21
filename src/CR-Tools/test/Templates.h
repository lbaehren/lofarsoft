/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Lars B"ahren (<mail>)                                                 *
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

#ifndef TEMPLATES_H
#define TEMPLATES_H

// Standard library header files
#include <iostream>
#include <string>
#include <vector>

using std::vector;

/*!
  \file Templates.h
  
  \ingroup CR_test
  
  \brief Brief description for class Templates
  
  \author Lars B&auml;hren
  
  \date 2007/08/21
  
  \test tTemplates.cc
  
  <h3>Prerequisite</h3>
  
  <ul type="square">
  <li>[start filling in your text here]
  </ul>
  
  <h3>Synopsis</h3>
  
  <h3>Example(s)</h3>
  
*/  

// ==============================================================================
//
//  Class VectorList
//
// ==============================================================================

// ------------------------------------------------------------ Class declaration

/*!
	\class VectorList
*/
template<class T> class VectorList {
  
  vector<T> elements_p;
  
 public:
  
  //! Argumented constructor
  VectorList (T const &elem);
  
  //! Argumented constructor
  VectorList (vector<T> const &vect);
  
  //! Destructor
  ~VectorList() {};
  
  /*!
    \brief Append an element to the list

    \param elem -- The element to be appended to the list

    \return status -- Status of the operation; returns <tt>false</tt> in case
                      an error was encountered.
  */
  bool appendElement (T const &elem);

  /*!
    \brief Retrieve an individual element from the list

    \retval elem -- 
    \param index -- 

    \return status -- Status of the operation; returns <tt>false</tt> in case
                      an error was encountered.
  */
  inline bool getElement (T & elem,
			  uint const &index)
  {
    if (index < elements_p.size()) {
      elem = elements_p[index];
      return true;
    } else {
      return false;
    }
  }

  /*!
    \brief Retrieve the complete list of elements 

    \return elements --  The complete list of elements 
  */
  inline vector<T> getElements () const {
    return elements_p;
  }

};

#endif /* TEMPLATES_H */

