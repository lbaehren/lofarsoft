/***************************************************************************
 *   Copyright (C) 2005 by Sven Lafebre                                    *
 *   s.lafebre@astro.ru.nl                                                 *
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

/* $Id: Data.h,v 1.7 2006/05/04 11:46:29 bahren Exp $ */

#ifndef _DATA_H_
#define _DATA_H_

#include <sstream>
#include <fstream>
#include <string>

namespace CR {
  
  /*!
    \class Data
    
    \ingroup CR_Data
    
    \brief Class for dealing with data
    
    This class was meant as a quick-and-easy solution for a project of mine. It
    is incomplete and somewhat buggy. 
    
    \author Sven Lafebre  
    
    \test tData.cc
  */
  class Data {
    //! The length of the data record in bytes
    uint length_;
    
    //! An identifier for the data
    uint id_;
    
  public:
    
    //! Pointer to array of data samples
    short* data_;
    
    // Constructors
    
    /*!
      \brief Empty constructor
      
      This constructor returns an empty data set, e.g a data set with length 0.
    */
    Data();
    
  /*!
    \brief Argumented constructor
    
    \param length    - Number of elements in the data set
    
    This constructor returns a zero-initialised data set, e.g. a data set of
    <em>length</em> points with value 0.
  */
  Data(uint length);
  
  /*!
    \brief Argumented constructor
    
    \param length    - Number of elements in the data set
    \param id        - ID of the data set (e.g. antenna number)
    
    This constructor returns a zero-initialised data set, e.g. a data set of
    <b>length</b> points with value 0. Additionally, it sets the ID of the data
    set (which could be an antenna number, for instance) to the supplied
    <b>id</b>.
  */
  Data(uint length, uint id);
  
  // Destructor
  
  /*!
    \brief Destructor
  */
  ~Data();
  
  /*!
    \brief Data initialisation
    
    This function clears the data array, setting all elements to zero.
  */
  void clear();
  
  /*!
    \brief Array initialization
    
    \param length    - Number of elements in the data set
    \param id        - ID of the data set (e.g. antenna number)
    
    Allocates memory for a zero-initialised data set, e.g. a data set of
    <b>length</b> points with value 0. Additionally, it sets the ID of the
    data set (which could be an antenna number, for instance) to the supplied
    <b>id</b>.
  */
  void init(uint length, uint id);
  
  /*!
    \brief Get or set the ID tag of the data set

    This function gets the ID tag of the data set, or sets this tag when it is
    called as an l-value.
  */
  uint& id()            {return id_;}
  
  /*!
    \brief Set and get the ID tag of the data set

    \param id    - New ID tag of the data set

    This function sets the ID tag of the data set, and returns the value it has
    been set to.
  */
  uint& id(uint id)     {id_ = id; return id_;}
  
  /*!
    \brief Get the length of the data set

    This function gets the length, in samples, of the data set. It cannot be
    called as an l-value.
  */
  uint  length()        {return length_;}
  
  /*!
    \brief Get or set a sample from the data set

    \param idx   - Sample number to get or set

    This function gets a sample from the data set, or sets this sample when it
    is called as an l-value.
  */
  short& data(uint idx) {return data_[idx];}

  /*!
    \brief Get the memory size of the data in the data set

    This function returns the amount of memory used to store the data samples
    in the data set, in bytes.
  */
  uint  size()          {return length_*sizeof(short);}

  /*!
    \brief Get the memory size of the object

    This function returns the amount of memory used to store the entire Data
    object, in bytes.
  */
  uint  objectSize()    {return size()+2*sizeof(uint);}
  
  /*!
    \brief Get or set a sample from the data set
    
    \param i   - Sample number to get or set
    
    The bracket operator gets a sample from the data set, or sets this sample
    when it is called as an l-value.
  */
  short& operator[]     (uint i)  {return data(i);}
  
  /* Experimental stuff.
     Data&  operator+      (Data&);
     Data   operator=      (Data&);
  */
  };
  
}

#endif /* _DATA_H_ */
