/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: AxisIterator.h,v 1.1 2006/07/05 16:01:08 bahren Exp $*/

#ifndef AXISITERATOR_H
#define AXISITERATOR_H

#include <Coordinates/AxisCoordinates.h>

using casa::Vector;

namespace CR {  // Namespace CR -- begin

/*!
  \class AxisIterator

  \ingroup Coordinates

  \brief Navigation along the main data axis.

  \author Lars B&auml;hren

  \date 2006/03/15

  \test tAxisIterator.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[CR-Tools] DataIterator 
  </ul>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

*/

class AxisIterator : public AxisCoordinates {

  //! Number of the current data block (of size "blocksize")
  int block_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  AxisIterator ();

  /*!
    \brief Default constructor

    \param block -- Number of the current data block
  */
  AxisIterator (const int& block);

  /*!
    \brief Argumented constructor

    \param blocksize       -- Size of a single data block, [samples]
    \param offset          -- Offset from the start of the data segment, [samples]
    \param presync         -- Synchronization between sample and time axis
    \param sampleFrequency -- Sample frequency, [Hz]
    \param frequencyRange  -- Frequency range, [Hz]
  */
  AxisIterator (const int& blocksize,
		const int& offset,
		const int& presync,
		const double& sampleFrequency,
		const Vector<double>& frequencyRange);

  /*!
    \brief Argumented constructor

    \param block -- Number of the current data block
    \param axis  -- AxisCoordinates object to be embedded inside this AxisIterator
                    object.
  */
  AxisIterator (const int& block,
		AxisCoordinates const& axis);

  /*!
    \brief Copy constructor

    \param other -- Another AxisIterator object from which to create this new
                    one.
  */
  AxisIterator (AxisIterator const& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~AxisIterator ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another AxisIterator object from which to make a copy.
  */
  AxisIterator& operator= (AxisIterator const &other); 

  // ----------------------------------------------------------------- Parameters

  /*!
    \brief Get the number of the current data block

    \return block -- Number of the current data block
  */
  int block () {
    return block_p;
  }

  /*!
    \brief Set the number of the current data block

    After updating the internal value of <i>block</i> the <i>offset</i> stored
    in AxisCoordinates is adjusted via
    \f[ N_{\rm Offset} = \left( N_{\rm Block} - 1 \right) \cdot N_{\rm Blocksize} \f]

    \param block -- Number of the current data block
  */
  void setBlock (const int& block);

  /*!
    \brief Advance the block number by 1

    Simplest iteration operation -- just advance the position by one block
  */
  void nextBlock () {
    setBlock (block_p+1);
  }

  // -------------------------------------------------------------------- Methods



 private:

  /*!
    \brief Unconditional copying
  */
  void copy (AxisIterator const& other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

};

}  // Namespace CR -- end

#endif /* _AXISITERATOR_H_ */
