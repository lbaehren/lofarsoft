/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) <year>                                                  *
 *   <author> (<mail>)                                                     *
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

#ifndef <NEWCLASS>_H
#define <NEWCLASS>_H

// Standard library header files
#include <iostream>
#include <string>

namespace <newNamespace> { // Namespace <newNamespace> -- begin
  
  /*!
    \class <newClass>
    
    \ingroup <newModule>
    
    \brief Brief description for class <newClass>
    
    \author <author>

    \date <date>

    \test t<newClass>.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class <newClass> {
    
  public:
    
    // === Construction =========================================================
    
    //! Default constructor
    <newClass> ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another <newClass> object from which to create this new
             one.
    */
    <newClass> (<newClass> const &other);
    
    // === Destruction ==========================================================

    //! Destructor
    ~<newClass> ();
    
    // === Operators ============================================================
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another <newClass> object from which to make a copy.
    */
    <newClass>& operator= (<newClass> const &other); 
    
    // === Parameter access =====================================================
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, <newClass>.
    */
    inline std::string className () const {
      return "<newClass>";
    }

    //! Provide a summary of the object's internal parameters and status
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // === Methods ==============================================================
    
    
    
  private:
    
    //! Unconditional copying
    void copy (<newClass> const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  }; // Class <newClass> -- end
  
} // Namespace <newNamespace> -- end

#endif /* <NEWCLASS>_H */
  
