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

#ifndef _PEAKLIST_H_
#define _PEAKLIST_H_

#include <sstream>
#include <fstream>
#include <Detection/Peak.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class PeakList
    
    \ingroup Analysis
    
    \brief Class for peaks in a data stream
    
    \author Sven Lafebre

  */
  class PeakList {

    //! Length of the peak list
    uint length_;
    // 
    uint space_;
    //! Array of Peak objects as which the individual events are stored
    Peak* peak_;
    
  public:
    /*!
      \brief Default constructor
    */
    PeakList();

    /*!
      \brief Argumented constructor

      \param length -- Length of the peak list
    */
    PeakList(uint length);
    
    /*!
      \brief Destructor
    */
    ~PeakList();
    
    /*!
      \brief Get the number of elements in the list

      \return length -- The number of elements in the list
    */
    uint length() {
      return length_;
    }

    /*!
      \brief Retrieve the data for the i-th peak

      \param i -- The number of the peak on the list to return
      
      \return peak -- The requested peak
    */
    Peak* peak(uint i);

    /*!
      \brief Add a peak to the existing list

      \param p -- The peak to be added to the list
    */
    Peak* addPeak(Peak p);

    /*!
      \brief Add a peak to the existing list
    */
    Peak* addPeak();

    /*!
      \brief Go to the next peak in the list
    */
    Peak* nextPeak();

    /*!
      \brief ??
    */
    Peak* operator[] (uint i) {
      return peak(i);
    }
  };
  
} // Namespace CR -- end

#endif /* _PEAKLIST_H_ */
