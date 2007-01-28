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

/* $Id: nuMoon.h,v 1.1 2006/07/05 16:22:59 bahren Exp $*/

#ifndef NUMOON_H
#define NUMOON_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <IO/DataReader.h>

namespace LOPES {  // namespace LOPES -- begin
  
  /*!
    \class nuMoon
    
    \ingroup Data
    
    \brief Brief description for class nuMoon
    
    \author Lars B&auml;hren
    
    \date 2006/05/31
    
    \test tnuMoon.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */
  
  class nuMoon : public DataReader {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    nuMoon ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another nuMoon object from which to create this new
      one.
    */
    nuMoon (nuMoon const &other);
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~nuMoon ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another nuMoon object from which to make a copy.
    */
    nuMoon& operator= (nuMoon const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    
    
    // ------------------------------------------------------------------ Methods
    
    
  protected:
    
    bool setStreams ();
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (nuMoon const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };

}  //  namespace LOPES -- end

#endif /* NUMOON_H */
