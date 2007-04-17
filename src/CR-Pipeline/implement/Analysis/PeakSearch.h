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
 
/* $Id */

#ifndef _PEAKSEARCH_H_
#define _PEAKSEARCH_H_

#include <sstream>
#include <fstream>
#include <Analysis/PeakList.h>
#include <Data/Data.h>

namespace CR { // Namespace CR -- begin

/*!
  \class PeakSearch
  
  \ingroup Analysis
  
  \brief Class for searching peaks in data
  
  \author Sven Lafebre
  
*/
class PeakSearch {

  /*!
    The blocksize (in bits) over which the average and standard deviation are to
    be calculated. A blocksize of \f$k\f$ means \f$2^k\f$ samples are included in
    an average.
  */
  uint blocksize_;
  
  /*!
    The threshold (in sigma above average) over which a peak is reported.
  */
  uint threshold_;
  
  /*!
    The delay (in samples) between the last sample over which the average was
    calculated and a sample being considered.
  */
  int  delay_;
  
  /*!
    The window (in samples) in which a certain number of peaks should be present for a
    correlation.
  */
  uint window_;
  
  /*!
    The minimum number of peaks within a certain time window for reporting a
    correlation.
  */
  uint npeaks_;
  
 public:
  // Constructors
  
  /*!
    \brief Empty constructor
    
    This constructor initializes a PeakSearch object with default parameters.
    These are:
    <ul>
      <li>
      <em>bs</em>
      The blocksize (in bits) over which the average and standard deviation
      are to be calculated. A blocksize of \f$k\f$ means \f$2^k\f$ samples are
      included in an average. This parameter is set to 10.
      <li>
      <em>th</em>
      The threshold (in sigma above average) over which a peak is reported.
      This parameter is set to 5.
      <li>
      <em>dl</em>
      The delay (in samples) between the last sample over which the average
      was calculated and the sample being considered. This parameter is set
      to 10.
    </ul>
  */
  PeakSearch();
  
  /*!
    \brief Argumented constructor
    
    \param th    The threshold (in sigma above average) over which a peak is reported.
    \param dl    The delay (in samples) between the last sample over which the average
                 was calculated and the sample being considered.

    This constructor initializes a PeakSearch object with the following parameters:
    \param bs    The blocksize (in bits) over which the average and standard deviation
    are to be calculated. A blocksize of \f$k\f$ means \f$2^k\f$ samples are
    included in an average.
  */
  PeakSearch(uint bs,
	     uint th,
	     int dl);
  
  /*!
    \brief Get the blocksize
    
    This function gets the blocksize (in bits) of the PeakSearch object. The blocksize
    identifies the interval over which the average and standard deviation are to be
    calculated. A blocksize of \f$k\f$ means \f$2^k\f$ samples are included in an
    average.
  */
  uint& blocksize() {return blocksize_;}
  
  /*!
    \brief Get the threshold
    
    This function gets the threshold (in sigma above average) of the PeakSearch object.
    The threshold identifies the value over which a peak is reported.
  */
  uint& threshold() {return threshold_;}
  
  /*!
    \brief Get the delay
    
    This function gets the delay (in samples) of the PeakSearch object. The delay
    identifies the number of samples between the last sample over which the average
    was calculated and the sample being considered.
  */
  int&  delay()     {return delay_;}
  
  /*!
    \brief Get the correlation window
    
    This function gets the window (in samples) of the PeakSearch object. The window
    identifies the maximum time difference between the first and last peak of a possible
    correlation.
  */
  uint& window()     {return window_;}
  
  /*!
    \brief Get the correlation threshold
    
    This function gets the correlation threshold (in samples) of the PeakSearch
    object. The correlation threshold identifies the number of peaks needed
    within a time of <b><em>window</em></b> samples for a positive detection.
  */
  uint& npeaks()     {return npeaks_;}
  
  /*!
    \brief Set the blocksize

    \param bs    The blocksize (in bits) over which the average and standard
                 deviation are to be calculated. A blocksize of \f$k\f$ means
		 \f$2^k\f$ samples are included in an average.

    This function sets the blocksize of the PeakSearch object to
    <b><em>bs</em></b>. Some `sanity checks' are performed.
  */
  uint& blocksize(uint bs);
  
  /*!
    \brief Set the threshold
    
    This function sets the threshold of the PeakSearch object to <b><em>th</em></b>. Some
    `sanity checks' are performed.
    \param th    The threshold (in sigma above average) over which a peak is reported.
  */
  uint& threshold(uint th);
  
  /*!
    \brief Set the delay
    
    This function sets the delay of the PeakSearch object to <b><em>dl</em></b>.
    \param dl    The delay (in samples) between the last sample over which the average
    was calculated and the sample being considered.
  */
  int&  delay(int dl);
  
  /*!
    \brief Set the correlation window
    
    This function gets the window (in samples) of the PeakSearch object.
    \param wn     The window (in samples) between the first and last peak of a possible
    correlation.
  */
  uint& window(uint wn);
  
  /*!
    \brief Set the correlation threshold
    
    This function gets the delay (in samples) of the PeakSearch object.
    \param np     The number of peaks needed within a time of <b><em>window</em></b>
    samples for a positive detection.
  */
  uint& npeaks(uint np);
  
  /*!
    \brief Perform the peaksearch in a dataset
    
    This function finds the actual peaks in a dataset <b><em>d</em></b> and returns
    an object containing information about the peaks.
    \param *d     Pointer to the dataset to be analysed.
  */
  PeakList findPeaks(Data* d);
  
  /*!
    \brief Perform the peaksearch in a dataset
    
    This function sets parameters according to those supplied to the function and then
    finds the actual peaks in a dataset <b><em>d</em></b> and returns
    an object containing information about the peaks.
    \param *d     Pointer to the dataset to be analysed.
    \param bs     Value of the <b><em>blocksize</em></b> to be used in the analysis.
    \param th     Value of the <b><em>threshold</em></b> to be used in the analysis.
    \param dl     Value of the <b><em>delay</em></b> to be used in the analysis.
  */
  PeakList findPeaks(Data* d,
		     uint bs,
		     uint th,
		     int dl);
  
  /*!
    \brief Set the correlation threshold
    
    This function performs a simple window correlation between different PeakList
    objects.
    
    <em>WARNING: The ouput of this function is (at this stage) limited to printing
    matches to the terminal. There are probably more useful things to do
    with the data...</em>
    \param *pl    Pointer to an array of PeakList objects, each of which is seen as
    the peak analysis of one data stream.
    \param n      The number of PeakList objects in the array.
  */
  void corrPeaks(PeakList* pl,
		 uint n);
  /*!
    \brief Set the correlation threshold
    
    This function sets parameters according to those supplied to the function and
    then performs a simple window correlation between different PeakList
    objects.
    
    <em>WARNING: The ouput of this function is (at this stage) limited to printing
    matches to the terminal. There are probably more useful things to do
    with the data...</em>
    \param *pl    Pointer to an array of PeakList objects, each of which is seen as
    the peak analysis of one data stream.
    \param n      The number of PeakList objects in the array.
    \param wn     The value for the maximum time window for matches.
    \param np     The value for the minimal number of peaks for a detection.
  */
  void corrPeaks(PeakList* pl,
		 uint n,
		 uint wn,
		 uint np);
};

} // Namespace CR -- end

#endif /* _PEAKSEARCH_H_ */
