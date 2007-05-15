/***************************************************************************
 *   Copyright (C) 2005                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

/* $Id: StoredMultiplyObject.h,v 1.4 2006/08/01 15:03:25 bahren Exp $*/

#ifndef _STOREDMULTIPLYOBJECT_H_
#define _STOREDMULTIPLYOBJECT_H_

#include <LopesBase/StoredObject.h>

using casa::uInt;

namespace CR {  // Namespace CR -- begin
  
/*!
  \class StoredMultiplyObject

  \ingroup LopesBase

  \brief Class for objectes that multiply data from two (or more) parents and stores them

  \author Andreas Horneffer

  \date 2005/07/21

  \test tStoredMultiplyObject.cc

  <h3>Prerequisite</h3>

  <tt>StoredObject</tt>

  <h3>Synopsis</h3>

  Parents: 

  Example:
  
  Keywords: math, stored

*/

template <class T> class StoredMultiplyObject : public StoredObject<T>{

    bool updateStorage();

 public:

  // --- Construction ----------------------------------------------------------

  /*!
    \brief Constructor
  */
  StoredMultiplyObject (uInt parents);

  // --- Destruction -----------------------------------------------------------

  /*!
    \brief Destructor
  */
  ~StoredMultiplyObject ();

  // --- Parameters ------------------------------------------------------------

// --- Children & Parents ----------------------------------------------------

  /*!
    \brief Set a reference to a parent object.

    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
   */
  virtual bool setParent(uInt num,
			 BasicObject* obj);

};

}  // Namespace CR -- end

#endif
