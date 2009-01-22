/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id$*/

#ifndef _DATABLOCKSTATISTICS_H_
#define _DATABLOCKSTATISTICS_H_

#include <crtools.h>

#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>

#include <casa/namespace.h>

/*!
  \class DataBlockStatistics

  \ingroup Analysis
  
  \brief Generate some statistics for a single block of data

  \author Lars B&auml;hren

  \date 2005/07/19

  \test tDataBlockStatistics.cc
*/

template <class T> class DataBlockStatistics {

  //! Shape of the data array, [samples,nofAntennas]
  IPosition shape_p;

  //! Statistics per antenne for the current data data block
  Vector<T> dataMin_p;
  Vector<T> dataMax_p;
  Vector<T> dataSum_p;
  Vector<T> dataMean_p;
  Vector<T> dataVariance_p;
  
  //! Joint statistics for all antennas for the current data block
  T blockMin_p;
  T blockMax_p;
  T blockSum_p;
  T blockMean_p;
  T blockVariance_p;

  T blockMeanMin_p;
  T blockMeanMax_p;
  T blockMeanSum_p;

 public:

  // --- Construction ----------------------------------------------------------

  /*!
    \brief Constructor
  */
  DataBlockStatistics ();

  /*!
    \brief Argumented constructor
    
    \param shape -- Shape of the data array provided at a later point.
  */
  DataBlockStatistics (const IPosition& shape);

  /*!
    \brief Constructor

    \param blocksize -- Size of an individual block of data, [samples].
    \param nofFiles  -- Number of data files.
  */
  DataBlockStatistics (const Int& blocksize,
		       const Int& nofFiles);

  // --- Destruction -----------------------------------------------------------

  /*!
    \brief Destructor
  */
  ~DataBlockStatistics ();

  // --- Statistics ------------------------------------------------------------

  /*!
    \brief Create statistics for a block of data.

    \param data -- Block of data,  [samples,nofAntennas].
  */
  void createStatistics (const Matrix<T>& data);

  /*!
   \param min      -- 
   \param blockMin -- 
  */
  void dataMin (Vector<T>& min,
		T& blockMin);
  
  /*!
   \param max      -- 
   \param blockMax -- 
  */
  void dataMax (Vector<T>& max,
		T& blockMax);
  
  /*!
   \param mean      -- 
   \param blockMean -- 
  */
  void dataMean (Vector<T>& mean,
		T& blockMean);
  
  /*!
   \param variance      -- 
   \param blockVariance -- 
  */
  void dataVariance (Vector<T>& variance,
		T& blockVariance);
  
  /*!
    \brief Retrieve statistics per antenne for the current data data block.

    \param min      -- 
    \param max      -- 
    \param sum      -- 
    \param mean     -- 
    \param variance -- 
  */
  void antennaStatistics (Vector<T>& min,
			  Vector<T>& max,
			  Vector<T>& sum,
			  Vector<T>& mean,
			  Vector<T>& variance);
  
  /*!
    \brief Retrieve absolute collective statistics for the current data block

    \param min  -- 
    \param max  -- 
    \param sum  -- 
    \param mean -- 
  */
  void blockStatistics (T& min,
			T& max,
			T& sum,
			T& mean,
			T& variance);

  /*!
    \brief Retrieve normalized collective statistics for the current data block

    \param min  -- 
    \param max  -- 
    \param sum  -- 
    \param mean -- 
  */
  void blockMeanStatistics (T& min,
			    T& max,
			    T& sum,
			    T& mean);
  
 private:

  /*!
    \brief Store the shape of the data array which is analyzed.

    \param shape -- Shape of the data array.
  */
  void setShape (const IPosition& shape);

  /*!
    \brief Bring all internal arrays to the correct shape.
  */
  void setArrays ();

};

#endif
