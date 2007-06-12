/***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Lars Baehren (<mail>)                                                     *
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

/* $Id: template-class.h,v 1.19 2007/05/29 08:31:52 bahren Exp $*/

#ifndef ANTENNA_H
#define ANTENNA_H

// Standard library header files
#include <string>
#include <vector>

#include <Imaging/Feed.h>

using std::vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class Antenna

    \ingroup CR
    \ingroup Imaging
    
    \brief Brief description for class Antenna
    
    \author Lars B&auml;hren

    \date 2007/06/12

    \test tAntenna.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class Antenna {

    //! Identifier of the antenna
    int ID_p;
    //! Position w.r.t. the array reference position, [m]
    vector<double> position_p;
    //! Orientation w.r.t. the array referene frame, []
    vector<double> orientation_p;
    //! Feeds of the antenna
    vector<Feed> feeds_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    Antenna ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another Antenna object from which to create this new
      one.
    */
    Antenna (Antenna const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~Antenna ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another Antenna object from which to make a copy.
    */
    Antenna& operator= (Antenna const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Antenna.
    */
    std::string className () const {
      return "Antenna";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (Antenna const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* ANTENNA_H */
  
