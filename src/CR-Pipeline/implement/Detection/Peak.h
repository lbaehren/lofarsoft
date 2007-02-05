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

#ifndef _PEAK_H_
#define _PEAK_H_

#include <sstream>
#include <fstream>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class Peak
    
    \ingroup Analysis
    
    \brief Class for peaks in a data stream
    
    \author Sven Lafebre
    
    This class contains the framework for storing information about peaks 
    -- short, high signals -- in a data stream.
  */
  class Peak {

    //! Time at which the peak occured
    uint p_time;
    //! Duration of the peak
    int  p_duration;
    //! Height of the peak
    int  p_height;
    //! Significance of the peak
    uint p_significance;
    
  public:
    /*!
      \brief Default constructor
    
      The Peak constructor. This function allocates memory for the peak data, and
      inititalizes it with zeroes.
    */
    Peak();

    /*!
      \brief Argumented constructor
      
      The Peak constructor. This function allocates memory for the peak data, and
      inititalizes it using the supplied parameters, where <b><em>time</em></b> maps
      to the newly created <b>Peak</b>'s time stamp, <b><em>height</em></b> to its
      absolute height, and <b><em>sigma</em></b> to its sigificance (in sigma above
      average).
    */
    Peak (uint time,
	  int height,
	  uint sigma);
    
    /*!
      \brief Argumented constructor
    
      The Peak constructor. This function allocates memory for the peak data,
      and inititalizes it using the supplied char string. At least 12 bytes are
      assumed to be available from the pointer, and they are read as the newly
      created <b>Peak</b>'s <tt>p_time</tt> (8 bytes), <tt>p_height</tt> (4
      bytes), and <tt>p_sigificance</tt> (4 bytes). Please note that these
      conversions are machine-dependent, so this conversion should only be
      applied to char strings created on the same machine.
    */
    Peak(char* str);
    
    /*!
      Returns the timestamp of occurence of the peak.
    */
    uint& time() {
      return p_time;
    }
    
    /*!
      Returns the timestamp of occurence of the peak.
    */
    int&  duration() {
      return p_duration;
    }
    
    /*!
      Returns the absolute height of the peak.
    */
    int&  height() {
      return p_height;
    }
    
    /*!
      Returns the significance of the peak, i.e. the sigma level above average.
    */
    uint& significance() {
      return p_significance;
    }
    
    /*!
      Prints a textual representation of the peak information to standard output.
    */
    void print();
    
    /*!
      Merges the peak info in an array <b><em>peaks</em></b> of
      <b><em>num</em></b> Peaks by averaging their values. The resulting peak is
      returned.
    */
    Peak* mergePeaks (Peak* peaks,
		      uint num);
  };
  
} // Namespace CR -- end

#endif /* _PEAK_H_ */
