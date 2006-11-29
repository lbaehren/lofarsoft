/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

/* $Id: SkymapQuantity.h,v 1.2 2006/09/20 10:21:59 bahren Exp $*/

#ifndef SKYMAPQUANTITY_H
#define SKYMAPQUANTITY_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/string.h>
#include <casa/Exceptions/Error.h>

using casa::String;

namespace LOPES {  // namespace LOPES -- begin
  
  /*!
    \class SkymapQuantity
    
    \ingroup Skymap
    
    \brief Brief description for class SkymapQuantity
    
    \author Lars B&auml;hren
    
    \date 2006/09/18
    
    \test tSkymapQuantity.cc
    
    <h3>Prerequisite</h3>
    
    --/--
    
    <h3>Synopsis</h3>
    
    In order to provide a unified scheme by which to propagate information on the
    quantity to be stored in the sky map, this class holds a list of the various 
    quantities.
    
    <h3>Example(s)</h3>
    
    The code example below show a typical usage scenario within a <tt>switch</tt>
    statement (taken from the Skymap class):
    \code
    SkymapQuantity qnt = SkymapQuantity (SkymapQuantity::TIME_FIELD);
    
    switch (qnt) {
    case SkymapQuantity::FREQ_FIELD:
    domain   = "Frequency";
    quantity = "Field";
    pFunction = &Skymap::addAntennaSignals;
    break;
    case SkymapQuantity::FREQ_POWER:
    domain   = "Frequency";
    quantity = "power";
    pFunction = &Skymap::addAntennaSignals;
    break;
    }
    \endcode
  */
  
  class SkymapQuantity {
    
  public:
    
    /*!
      \brief Electrical quantities, for which the sky map is generated
    */
    typedef enum {
      //! Electric field strength as function of frequency
      FREQ_FIELD,
      //! Power of the electric field as function of frequency
      FREQ_POWER,
      //! Electric field strength as function of time (sample resolution)
      TIME_FIELD,
      //! Power of the electric field as function of time
      TIME_POWER,
      //! Cross-correlation beam (cc-beam)
      TIME_CC,
      //! Excess-beam
      TIME_X
    } Type ;
    
  private:
    
    //! Electrical quantity
    SkymapQuantity::Type quantity_p;
    
  public:
    
    // --------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    SkymapQuantity ();
    
    /*!
      \brief Argumented constructor
    */
    SkymapQuantity (SkymapQuantity::Type const &quantity);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another SkymapQuantity object from which to create this new
      one.
    */
    SkymapQuantity (SkymapQuantity const &other);
    
    // ---------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~SkymapQuantity ();
    
    // ------------------------------------------------------------------ Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another SkymapQuantity object from which to make a copy.
    */
    SkymapQuantity& operator= (SkymapQuantity const &other); 
    
    // ----------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return class -- The name of the class, SkymapQuantity.
    */
    std::string className () const {
      return "SkymapQuantity";
    }
    
    /*!
      \brief Get the SkymapQuantity type
      
      \return quantity -- Type of the SkymapQuantity
    */
    SkymapQuantity::Type skymapQuantity () const {
      return quantity_p;
    }
    
    /*!
      \brief Set the SkymapQuantity type
      
      \param quantity -- Type of the SkymapQuantity
    */
    void setSkymapQuantity (SkymapQuantity::Type const &quantity) {
      quantity_p = quantity;
    }
    
    /*!
      \brief Get the domain in which the quantity is defined
      
      \return domain -- Domain of the electrical quantity: <i>Time</i>,
      <i>Frequency</i>
    */
    String domain ();
    
    /*!
      \brief Get the type of electrical quantity
      
      \return quantity -- Electrical quantity: <i>Field</i>, <i>Power</i>
    */
    String quantity ();
    
    // -------------------------------------------------------------------- Methods
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (SkymapQuantity const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };  // Class SkymapQuantity -- end
  
}  // Namespace LOPES -- end

#endif /* SKYMAPQUANTITY_H */
  
