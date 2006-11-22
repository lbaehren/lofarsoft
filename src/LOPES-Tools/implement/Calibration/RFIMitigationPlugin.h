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

/* $Id: RFIMitigationPlugin.h,v 1.3 2006/09/22 14:01:06 bahren Exp $*/

#ifndef RFIMITIGATIONPLUGIN_H
#define RFIMITIGATIONPLUGIN_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <lopes/Calibration/PluginBase.h>
#include <lopes/Calibration/RFIMitigation.h>

#include <casa/namespace.h>

/*!
  \class RFIMitigationPlugin

  \ingroup Calibration

  \brief Plugin for RFI Mitigation

  \author Lars B&auml;hren

  \date 2006/09/22

  \test tRFIMitigationPlugin.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[start filling in your text here]
  </ul>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

*/

class RFIMitigationPlugin : public PluginBase<Complex> {

  //! Size of a single block of data, [samples]
  uint blocksize_p;

  //! Number of segments into which the spectrum is about to be divided
  uint nofSegments_p;

  //! Indices for segmentation of the frequency axis
  Vector<uint> segmentationIndices_p;
  
  RFIMitigation rfi_p;

 public:

  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  RFIMitigationPlugin ();

  /*!
    \brief Copy constructor

    \param other -- Another RFIMitigationPlugin object from which to create this
                    new one.
  */
  RFIMitigationPlugin (RFIMitigationPlugin const &other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~RFIMitigationPlugin ();

  // ------------------------------------------------------------------ Operators

  /*!
    \brief Overloading of the copy operator

    \param other -- Another RFIMitigationPlugin object from which to make a copy.
  */
  RFIMitigationPlugin& operator= (RFIMitigationPlugin const &other); 

  // ----------------------------------------------------------------- Parameters
  
  /*!
    \brief Set the internal parameters controlling the behaviour of the plugin
  */
  virtual Bool setParameters (Record const &param); 

  /*!
    \brief Get the size of a block of data

    \return blocksize -- Size of a block of data
  */
  uint blocksize () const {
    return blocksize_p;
  }
  
  /*!
    \brief Set a new value for the blocksize

    \param blocksize -- The size of a block of data, [samples]
  */
  void setBlocksize (uint const &blocksize);

  /*!
    \brief Get the number of segments

    \return nofSegements -- The number of segments into which the spectrum
                            is sub-divided.
  */
  uint nofSegments () const {
    return nofSegments_p;
  }
  
  /*!
    \brief Set the number of segments of the frequency axis

    \param nofSegments -- 
  */
  void setNofSegments (uint const &nofSegments);

  /*
    \brief Get the indices used for segmentation of the frequency axis

    \return segmentationIndices -- Indices used for segmentation of the
                                   frequency axis
   */
  Vector<uint> segmentationIndices () const {
    return segmentationIndices_p;
  }
  
  /*
    \brief Set the indices used for segmentation of the frequency axis

    \param segmentationIndices -- Indices used for segmentation of the
                                  frequency axis
   */
  void setSegmentationIndices (Vector<uint> const &indices);

  // -------------------------------------------------------------------- Methods



 private:

  /*!
    \brief Unconditional copying
  */
  void copy (RFIMitigationPlugin const &other);

  /*!
    \brief Unconditional deletion 
  */
  void destroy(void);

  void setNofSegments ();

  void setSegmentationIndices ();

};

#endif /* RFIMITIGATIONPLUGIN_H */
