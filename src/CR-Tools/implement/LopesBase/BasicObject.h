/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
 *   Andreas Horneffer (a.horneffer@astro.ru.nl)                           *
 *   Sven Lafebre (s.lafebre@astro.ru.nl)                                  *
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

/* $Id: BasicObject.h,v 1.17 2007/04/17 13:52:47 bahren Exp $*/

#ifndef _BASICOBJECT_H_
#define _BASICOBJECT_H_

#include <casa/aips.h>
#include <casa/Arrays.h>

#include <LopesBase/BasicHeader.h>
#include <LopesBase/ExchangeFormat.h>

using casa::String;
using casa::uInt;
using casa::Vector;

namespace CR {  // Namespace CR -- begin
  
  /*!
    \class BasicObject
    
    \ingroup LopesBase
    
    \brief Base class for objects in a LOPES-Tools processing tree
    
    \author Lars B&auml;hren, Andreas Horneffer, Sven Lafebre
    
    \date 2005/07/13
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>BasicHeader
    </ul>
    
    <h3>Synopsis</h3>
    
  */
  class BasicObject {
    
  protected:
    
    //! Holds the name of the class, e.g. <i>BasicObject</i>
    String className_p;
    
    //! Title of the object in a tree of objects
    String title_p;
    
    //! Is this a valid object?
    bool valid_p;
    
    //! Meta data information
    BasicHeader header_p;
    
    //! Information on the output data format
    ExchangeFormat format_p;
    
    //! A list of pointers the objects' parents
    BasicObject** parent_p;
    //  Vector<BasicObject*> parent_p;
    
    //! A list of pointers to the objects. children
    BasicObject** child_p;
    //  Vector<BasicObject*> child_p;
    
    //! Number of parents
    uInt nParent_p;
    //! Number of children
    uInt nChild_p;
    
    //Not needed here
    //virtual void update();
    
    /*!
      \brief Initialize the object
      
      \return ok -- Was operation successful? Returns \c True if yes.
    */
    virtual bool init ();
    
    /*!
      \brief Set the parent after checking
      
      \return ok -- Was operation successful? Returns \c True if yes.
    */
    virtual bool putParent(uInt num, BasicObject* obj);
    
    /*!
      \brief Initialize the object
      
      \return ok -- Was operation successful? Returns \c True if yes.
    */
    virtual bool cleanup ();
    
  public:
    
    // --- Construction ----------------------------------------------------------
    
    /*!
      \brief Constructor
    */
    BasicObject ();
    
    // --- Destruction -----------------------------------------------------------
    
    /*!
      \brief Destructor
    */
    virtual ~BasicObject ();
    
    // --- Parameters ------------------------------------------------------------
    
    /*!
      \brief Return the class name
      
      \return name -- The object's class name
    */
    String className ()      {return className_p;}
    
    /*!
      \brief Return the title of the object
      
      \return className -- The title of the object
    */
    String title ()          {return title_p;}
    
    /*!
      \brief Provide a short summary of the object's internal data
    */
    void summary ();
    
    /*!
      \brief Return the object's header info
      
      \return header -- The header information for the object
    */
    BasicHeader& header()    {return header_p;}
    
    /*!
      \brief Return info on the output data format of the class
      
      \param f       -- The data format flag to check for, e.g. \c EF_isComplex or
      \c EF_isNumeric.
      \return format -- The state of a flag in the output data format of this class.
    */
    bool format(uInt f) {return format_p.flag(f);}
    
    // --- Functions -------------------------------------------------------------
    
    /*!
      \brief Invalidate the contents of the object
      
      \return ok -- Was operation successful? Returns \c True if yes.
      
      This function sets the object's \b valid_p flag to \c false, so that it will
      be updated next time its contents are queried.
    */
    virtual bool invalidate ();
    
    /*!
      \brief Get the data from this object
      
      \param result -- Pointer to something to put the data into
      
      \return ok    -- Was operation successful? Returns <tt>True</tt> if yes.
      
      Prototype function has to be implemented in the derived classes
      
      <b> Warning </b>
      
      Please look at the documentation of the derived classes for the object that
      <tt>result</tt> has to point to and any requirements what you can or must 
      not do with it afterwards.
    */
    virtual bool get (void *result) {return false;};
    
    /*!
      \brief Add a child class to this class.
      
      \param obj -- Pointer to an object which is derived from BasicObject
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    virtual bool addChild (BasicObject* obj);
    
    /*!
      \brief Set a reference to a parent object after checking types etc.
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    virtual bool setParent(uInt num, BasicObject* obj) { return putParent(num, obj); };
    
    /*!
      \brief Present a list of children of this class
      
      \return children -- The names of the children of this class.
      
      \todo to be implemented
    */
    Vector<String> listChildren();
    
    /*!
      \brief Present a list of parents of this class
      
      \return children -- The names of the parents of this class.
      
      \todo to be implemented
    */
    Vector<String> listParents();
  };

}  // Namespace CR -- end
  
#endif
  
  // Assorted junk below
  
  /*
    \brief Remove a class reference from an object array
    
    \param ar  -- Pointer to a Vector of references to BasicObjects
    
    \param obj -- Pointer to an object which is derived from BasicObject
    
    \param ret -- Should the reverse reference also be deleted?

    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
   */
  //virtual bool removeNode   (Vector <BasicObject*> *ar, BasicObject* obj, bool ret);

  /*
    \brief Remove the reference of a child class to this class.

    \param obj -- Pointer to another object which is derived from BasicObject

    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
   */
  //virtual bool removeChild  (BasicObject* obj)
                            //{ return removeNode(&child_p,  obj, True);}
  /*
    \brief Remove the reference of a child class to this class.

    \param obj -- Pointer to another object which is derived from BasicObject

    \param ret -- Should the reverse reference also be deleted?

    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
   */
  //virtual bool removeChild  (BasicObject* obj, bool ret)
                            //{ return removeNode(&child_p,  obj, ret);}
  /*
    \brief Remove the reference of a parent class to this class.

    \param obj -- Pointer to another object which is derived from BasicObject

    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
   */
  //virtual bool removeParent (BasicObject* obj)
                            //{ return removeNode(&parent_p, obj, True);}
  /*
    \brief Remove the reference of a parent class to this class.

    \param obj -- Pointer to another object which is derived from BasicObject

    \param ret -- Should the reverse reference also be deleted?

    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
   */
  //virtual bool removeParent (BasicObject* obj, bool ret)
                            //{ return removeNode(&parent_p, obj, ret);}

