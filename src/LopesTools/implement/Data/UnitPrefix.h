/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

/* $Id: template-class.h,v 1.16 2006/10/31 19:19:57 bahren Exp $*/

#ifndef UNITPREFIX_H
#define UNITPREFIX_H

#include <blitz/array.h>


namespace LOPES { // Namespace LOPES -- begin
  
  /*!
    \class UnitPrefix
    
    \ingroup Data
    
    \brief Brief description for class UnitPrefix
    
    \author Lars Baehren

    \date 2006/11/01

    \test tUnitPrefix.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
	
	\verbatim
	globalconst.UnitPrefixName:=['none','nano','micro','milli','Kilo','Mega','Giga','Tera'];
	globalconst.UnitPrefixSymbol:=['','n','µ','m','K','M','G','T'];
	globalconst.UnitPrefixFactor:=[1,1e-9,1e-6,1e-3,1e3,1e6,1e9,1e12];
	\endverbatim
    
    <h3>Example(s)</h3>
    
  */  
  class UnitPrefix {

    //! Number of unit prefixes
    uint nofPrefixes_p;
    //! Units names
    blitz::Array<std::string,1> names_p;
    //! Units symbols
    blitz::Array<std::string,1> symbols_p;
    
  public:
    
    /*!
      \brief Names for the unit prefixes
    */
    typedef enum {
      //! [1] = [1]
      none,
      //! [n] = \f$ 10^{-9} \f$
      nano,
      //! [mu] = \f$ 10^{-6} \f$
      micro,
      //! [m] = \f$ 10^{-3} \f$
      milli,
      //! [k] = \f$ 10^{3} \f$
      Kilo,
      //! [M] = \f$ 10^{6} \f$
      Mega,
      //! [G] = \f$ 10^{9} \f$
      Giga,
      //! [T] = \f$ 10^{12} \f$
      Tera
    } NAME;

    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    UnitPrefix ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another UnitPrefix object from which to create this new
      one.
    */
    UnitPrefix (UnitPrefix const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~UnitPrefix ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another UnitPrefix object from which to make a copy.
    */
    UnitPrefix& operator= (UnitPrefix const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, UnitPrefix.
    */
    std::string className () const {
      return "UnitPrefix";
    }

    /*!
      \brief Get the name of the unit prefix

      \param name -- 

      \return unitName -- The full name of the unit prefix, as string.
     */
    std::string unitName (UnitPrefix::NAME const &name);

    /*!
      \brief The symbol of the unit prefix

      \param  name -- 

      \return unitSymbol -- 
    */
    std::string unitSymbol (UnitPrefix::NAME const &name);
    
    // ------------------------------------------------------------------ Methods
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (UnitPrefix const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);

    /*!
      \brief Initialize the internal parameters
    */
    void init ();
    
  };
  
} // Namespace LOPES -- end

#endif /* UNITPREFIX_H */
  
