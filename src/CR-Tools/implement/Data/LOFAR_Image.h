/*-------------------------------------------------------------------------*
 | $Id:: NewClass.h 1159 2007-12-21 15:40:14Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Lars B"ahren (lbaehren@gmail.com)                                     *
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

#ifndef LOFAR_IMAGE_H
#define LOFAR_IMAGE_H

// Standard library header files
#include <iostream>
#include <string>

namespace LOFAR { // Namespace LOFAR -- begin
  
  /*!
    \class LOFAR_Image
    
    \ingroup CR_Data
    
    \brief Brief description for class LOFAR_Image
    
    \author Lars B&auml;hren

    \date 2008/01/15

    \test tLOFAR_Image.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class LOFAR_Image {

    std::string filename_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    LOFAR_Image ();
    
    /*!
      \brief Argumented constructor

      \param filename -- Name of the file into which the image is stored.
    */
    LOFAR_Image (std::string const &filename);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another LOFAR_Image object from which to create this new
      one.
    */
    LOFAR_Image (LOFAR_Image const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~LOFAR_Image ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another LOFAR_Image object from which to make a copy.
    */
    LOFAR_Image& operator= (LOFAR_Image const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, LOFAR_Image.
    */
    std::string className () const {
      return "LOFAR_Image";
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
    void copy (LOFAR_Image const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace LOFAR -- end

#endif /* LOFAR_IMAGE_H */
  
