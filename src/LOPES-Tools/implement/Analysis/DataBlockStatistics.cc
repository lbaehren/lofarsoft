/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Bahren (bahren@astron.nl)                                        *
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

/* $Id: DataBlockStatistics.cc,v 1.5 2006/11/02 12:38:31 horneff Exp $*/

#include <Analysis/DataBlockStatistics.h>

namespace LOPES {  // Namespace LOPES -- begin

// --- Construction ------------------------------------------------------------

template <class T>
DataBlockStatistics<T>::DataBlockStatistics ()
{
  IPosition shape (2);
  shape = 1;
  //
  DataBlockStatistics::setShape (shape);
}

template <class T>
DataBlockStatistics<T>::DataBlockStatistics (const IPosition& shape)
{
  DataBlockStatistics::setShape (shape);
}

template <class T>
DataBlockStatistics<T>::DataBlockStatistics (const int& blocksize,
					     const int& nofFiles)
{
  IPosition shape (2);
  //
  shape(0) = blocksize;
  shape(1) = nofFiles;
  //
  DataBlockStatistics::setShape (shape);
}

// --- Destruction -------------------------------------------------------------

template <class T>
DataBlockStatistics<T>::~DataBlockStatistics ()
{;}

// --- Parameters --------------------------------------------------------------

template <class T>
void DataBlockStatistics<T>::setShape (const IPosition& shape)
{
  if (shape.nelements() == 2) {
    shape_p = shape;
    //
    DataBlockStatistics::setArrays ();
  } else {
    cerr << "[DataBlockStatistics::setShape] Wrong number of elements!" << endl;
  }
}

template <class T>
void DataBlockStatistics<T>::setArrays ()
{
  dataMin_p.resize(shape_p(1),0.0);
  dataMax_p.resize(shape_p(1),0.0);
  dataSum_p.resize(shape_p(1),0.0);
  dataMean_p.resize(shape_p(1),0.0);
  dataVariance_p.resize(shape_p(1),0.0);
}

// --- Statistics --------------------------------------------------------------

template <class T>
void DataBlockStatistics<T>::createStatistics (const Matrix<T>& data)
{
  // Store the shape of the data array [samples,files]
  DataBlockStatistics::setShape (data.shape());
  
  // 
  for (int ant(0); ant<shape_p(1); ant++) {
    dataMin_p(ant)  = min(data.column(ant));
    dataMax_p(ant)  = max(data.column(ant));
    dataSum_p(ant)  = sum(data.column(ant));
    dataMean_p(ant) = dataSum_p(ant)/shape_p(0);
    dataVariance_p(ant) = variance(data.column(ant));
  }

  // normalized collective statistics for the current data block
  blockMeanMin_p = mean(dataMin_p);
  blockMeanMax_p = mean(dataMax_p);
  blockMeanSum_p = mean(dataSum_p);

  // absolute collective statistics for the current data block
  blockMin_p = min(dataMin_p);
  blockMax_p = max(dataMax_p);
  blockSum_p = sum(dataSum_p);
  blockMean_p = mean(dataMean_p);
  blockVariance_p = variance(data);
}

template <class T>
void DataBlockStatistics<T>::dataMin (Vector<T>& min,
				      T& blockMin)
{
  min.resize(shape_p(1));
  min  = dataMin_p;
  //
  blockMin = blockMin_p;
}

template <class T>
void DataBlockStatistics<T>::dataMax (Vector<T>& max,
				      T& blockMax)
{
  max.resize(shape_p(1));
  max  = dataMax_p;
  //
  blockMax = blockMax_p;
}

template <class T>
void DataBlockStatistics<T>::dataMean (Vector<T>& mean,
				       T& blockMean)
{
  mean.resize(shape_p(1));
  mean  = dataMean_p;
  //
  blockMean = blockMean_p;
}

template <class T>
void DataBlockStatistics<T>::dataVariance (Vector<T>& variance,
					   T& blockVariance)
{
  variance.resize(shape_p(1));
  variance  = dataVariance_p;
  //
  blockVariance = blockVariance_p;
}

template <class T>
void DataBlockStatistics<T>::antennaStatistics (Vector<T>& min,
						Vector<T>& max,
						Vector<T>& sum,
						Vector<T>& mean,
						Vector<T>& variance)
{
  // Resize the arrays
  try {
    min.resize(shape_p(1));
    max.resize(shape_p(1));
    sum.resize(shape_p(1));
    mean.resize(shape_p(1));
    variance.resize(shape_p(1));
  } catch (AipsError x) {
    cerr << "[DataBlockStatistics::antennaStatistics] " << x.getMesg() << endl;
  }
  // Copy the data
  min  = dataMin_p;
  max  = dataMax_p;
  sum  = dataSum_p;
  mean = dataMean_p;
  variance = dataVariance_p;
}

template <class T>
void DataBlockStatistics<T>::blockStatistics (T& min,
					      T& max,
					      T& sum,
					      T& mean,
					      T& variance)
{
  min      = blockMin_p;
  max      = blockMax_p;
  sum      = blockSum_p;
  mean     = blockMean_p;
  variance = blockVariance_p;
}

template <class T>
void DataBlockStatistics<T>::blockMeanStatistics (T& min,
						  T& max,
						  T& sum,
						  T& mean)
{
  min = blockMeanMin_p;
  max = blockMeanMax_p;
  sum = blockMeanSum_p;
  mean = blockMean_p;
}

// ==============================================================================
//
//  Template instantiations
//
// ==============================================================================

template class DataBlockStatistics<float>;
template class DataBlockStatistics<double>;

}  // Namespace LOPES -- end
