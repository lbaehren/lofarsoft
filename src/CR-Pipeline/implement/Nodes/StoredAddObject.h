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

/* $Id: StoredAddObject.h,v 1.2 2006/08/01 15:03:25 bahren Exp $*/

#ifndef STOREDMULTIPLYOBJECT_H
#define STOREDMULTIPLYOBJECT_H

#include <Nodes/StoredObject.h>

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class StoredAddObject
    
    \ingroup Nodes
    
    \brief Class for objectes that add data from two (or more) parents and stores them
    
    \author Andreas Horneffer
    
    \date 2005/07/21
    
    \test tStoredAddObject.cc
    
    <h3>Prerequisite</h3>
    
    <tt>StoredObject</tt>
    
    <h3>Synopsis</h3>
    
    
  */
  
  template <class T> class StoredAddObject : public StoredObject<T>{
    
    bool updateStorage();
    
  public:
    
    // --- Construction ----------------------------------------------------------
    
    /*!
      \brief Constructor
    */
    StoredAddObject (uint parents);
    
    // --- Destruction -----------------------------------------------------------
    
    /*!
      \brief Destructor
    */
    ~StoredAddObject ();
    
    // --- Parameters ------------------------------------------------------------
    
    // --- Children & Parents ----------------------------------------------------
    
    /*!
      \brief Set a reference to a parent object.
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    virtual bool setParent(uint num,
			   BasicObject* obj);
    
  };
  
}  // Namespace CR -- endl;

#endif
