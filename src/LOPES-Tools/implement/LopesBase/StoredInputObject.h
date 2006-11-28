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

/* $Id: StoredInputObject.h,v 1.3 2006/08/01 15:03:25 bahren Exp $*/

#ifndef STOREDINPUTOBJECT_H
#define STOREDINPUTOBJECT_H

#include <LopesBase/StoredObject.h>

using casa::Vector;

namespace LOPES {  // Namespace LOPES -- begin
  
  /*!
    \class StoredInputObject
    
    \ingroup LopesBase
    
    \brief Brief description for class StoredInputObject
    
    \author Andreas Horneffer
    
    \date 2005/07/22
    
    \test tStoredInputObject.cc
    
    <h3>Prerequisite</h3>
    
    <h3>Synopsis</h3>
    
    
  */
  
  template<class T> class StoredInputObject : public StoredObject<T>{
    
  public:
    
    // --- Construction ----------------------------------------------------------
    
    /*!
      \brief Constructor
    */
    StoredInputObject ();
    
    // --- Destruction -----------------------------------------------------------
    
    /*!
      \brief Destructor
    */
    ~StoredInputObject ();
    
    // --- Parameters ------------------------------------------------------------
    
    /*!
      \brief Put the data into the storage (a copy is made)
    */
    virtual bool put(Vector<T> *ref);
    
    virtual bool put(Vector<T> *ref,
		     bool copy);
    
  };
  
}  // Namespace LOPES -- end

#endif
