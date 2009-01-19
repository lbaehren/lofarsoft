/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2009                                                    *
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

#ifndef SKYMAPORIENTATION_H
#define SKYMAPORIENTATION_H

// Standard library header files
#include <iostream>
#include <string>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class SkymapOrientation
    
    \ingroup CR_Imaging
    
    \brief Handling of the orientation settings of a skymap
    
    \author Lars B&auml;hren

    \date 2009/01/13

    \test tSkymapOrientation.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>

    <table border=0>
      <tr>
        <td class="indexkey">Descriptor</td>
        <td class="indexkey">Orientation of the skymap</td>
      </tr>
      <tr>
        <td>NORTH_EAST</td>
        <td></td>
      </tr>
      <tr>
        <td>NORTH_WEST</td>
        <td></td>
      </tr>
      <tr>
        <td>SOUTH_EAST</td>
        <td></td>
      </tr>
      <tr>
        <td>SOUTH_WEST</td>
        <td></td>
      </tr>
    </table>
    
    <h3>Example(s)</h3>
    
  */  
  class SkymapOrientation {

    //! Clockwise sequence of image world axes
    typedef enum {
      //! North is up, East is right
      NORTH_EAST,
      //! North is up, West is right
      NORTH_WEST,
      //! South is up, East is right
      SOUTH_EAST,
      //! South is up, West is right
      SOUTH_WEST
    } Orientation;

    bool northIsUp_p;
    bool eastIsRight_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    //! Default constructor
    SkymapOrientation () {
      northIsUp_p   = true;
      eastIsRight_p = true;
    }
    
    //! Default constructor
    SkymapOrientation (bool const &northIsUp,
		       bool const &eastIsRight) {
      northIsUp_p   = northIsUp;
      eastIsRight_p = eastIsRight;
    }
    
    /*!
      \brief Copy constructor
      
      \param other -- Another SkymapOrientation object from which to create this new
             one.
    */
    SkymapOrientation (SkymapOrientation const &other);
    
    // -------------------------------------------------------------- Destruction

    //! Destructor
    ~SkymapOrientation ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another SkymapOrientation object from which to make a copy.
    */
    SkymapOrientation& operator= (SkymapOrientation const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Is the upward direction of the map pointing towards North?

      \return northIsUp -- Returns <tt>true</tt> in case the upward direction of
              the map poiting towards North
    */
    inline bool northIsUp () const {
      return northIsUp_p;
    }

    /*!
      \brief Is the upward direction of the map to be pointing towards North?

      \param northIsUp -- Set <tt>true</tt> in case the upward direction of
             the map is to be poiting towards North
    */
    inline void northIsUp (bool const &northIsUp) {
      northIsUp_p = northIsUp;
    }
    
    /*!
      \brief Is the rightward direction of the map pointing towards East?

      \return eastIsRight -- Returns <tt>true</tt> in case the rightward direction
              of the map poiting towards East.
    */
    inline bool eastIsRight () const {
      return eastIsRight_p;
    }

    /*!
      \brief Is the rightward direction of the map to be pointing towards East?

      \param eastIsRight -- Set <tt>true</tt> in case the rightward direction
             of the map is to be poiting towards East.
    */
    inline void eastIsRight (bool const &eastIsRight) {
      eastIsRight_p = eastIsRight;
    }
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, SkymapOrientation.
    */
    inline std::string className () const {
      return "SkymapOrientation";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status

      \param os -- Output stream to which the summary is written.
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods

    /*!
      \brief Get the descriptor for the orientation of the skymap

      \return orientation -- Descriptor for the orientation of the skymap, e.g.
              \e NORTH_EAST
    */
    std::string orientation ();
    
  private:
    
    //! Unconditional copying
    void copy (SkymapOrientation const &other);
    
    //! Unconditional deletion 
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* SKYMAPORIENTATION_H */
  
