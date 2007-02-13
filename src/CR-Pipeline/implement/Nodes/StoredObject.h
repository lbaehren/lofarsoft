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

/* $Id: StoredObject.h,v 1.9 2006/08/01 15:03:25 bahren Exp $*/

#ifndef STOREDOBJECT_H
#define STOREDOBJECT_H

// AIPS++/CASA includes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>

// CR-Pipeline includes
#include <Nodes/BasicHeader.h>
#include <Nodes/BasicObject.h>

using casa::AipsError;
using casa::Complex;
using casa::DComplex;
using casa::String;
using casa::Vector;

namespace CR {  // Namespace CR -- begin

/*!
  \class StoredObject
  
  \ingroup Nodes

  \brief Mother class for onjects that store their data localy

  \author Andreas Horneffer

  \date 2005/07/20

  \test tStoredObject.cc
*/

template <class T> class StoredObject : public BasicObject {
 
  protected:
    
    /*!
      \brief Update the internal storage area
      
      \return status -- Status of the operation
    */
    virtual bool updateStorage() {
      return false;
    };
    
    /*!
      \brief The data is stored here
    */
    Vector<T> data_p;

  public:
    
    // --- Construction ----------------------------------------------------------
    
    /*!
      \brief Constructor
    */
    StoredObject ();
    
    StoredObject (uint format);
    
    // --- Destruction -----------------------------------------------------------
    
    /*!
      \brief Destructor
    */
    ~StoredObject ();
    
    // --- Parameters ------------------------------------------------------------
    /*!
      \brief Get the data from this object
      
      \param result -- Pointer to an empty vector of the desired class
      
      \return ok    -- Was operation successful? Returns <tt>true</tt> if yes.
      
      <b> Warning </b>
      
      The returned vector is a reference to the data stored in the parent object.
      Modifying it can alter the data inside the parent object. Before writing
      to the used vector you have to break the reference, e.g. by calling 
      <tt>unique()</tt> (makes a copy), referencing it to another vector or
      destroying it.
      
    */
    virtual bool get(void *ref);
    
  };
  
}  // Namespace CR -- end

#endif
