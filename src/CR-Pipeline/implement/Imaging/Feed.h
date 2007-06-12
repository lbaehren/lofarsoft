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

#ifndef FEED_H
#define FEED_H

// Standard library header files
#include <string>
#include <vector>

using std::vector;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class Feed

    \ingroup CR
    \ingroup Imaging
    
    \brief Properties of an Antenna feed
    
    \author Lars B&auml;hren

    \date 2007/06/12

    \test tFeed.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class Feed {

  public:

    /*!
      \brief Type of antenna feed
     */
    typedef enum {
      X,
      Y,
      XY,
      L,
      R,
      LR
    } FeedType;
    
  protected:

    //! Type of antenna feed
    FeedType feedType_p;
    //! Identifier for the feed
    int ID_p;
    //! Offset w.r.t. to the reference frame of the antenna, [m]
    vector<double> offset_p;
    //! Rotation w.r.t. to the  reference frame fo the antenna, [rad]
    vector<double> orientation_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    Feed ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another Feed object from which to create this new
      one.
    */
    Feed (Feed const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~Feed ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another Feed object from which to make a copy.
    */
    Feed& operator= (Feed const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the ID of the antenna feed

      \return ID -- The ID of the antenna feed
    */
    inline int ID () {
      return ID_p;
    }

    /*!
      \brief Set the ID of the antenna feed

      \param ID -- The ID of the antenna feed
    */
    inline void setID (int const &ID) {
      ID_p = ID;
    }
    
    /*!
      \brief Get the offset of the feed w.r.t. the reference frame of the antenna

      \return offset -- The offset of the feed w.r.t. the reference frame of the
                        antenna to which it belongs, [m]
    */
    inline vector<double> offset () {
      return offset_p
    }

    void setOffset (vector<double> const &offset);
    
    inline vector<double> orientation () {
      return orientation_p
    }

    void setOrientation (vector<double> const &orientation);
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, Feed.
    */
    std::string className () const {
      return "Feed";
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
    void copy (Feed const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* FEED_H */
  
