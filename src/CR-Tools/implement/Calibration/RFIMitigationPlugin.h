/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Kalpana Singh (k.singh@astro.ru.nl)                                       *
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

#ifndef RFIMITIGATIONPLUGIN_H
#define RFIMITIGATIONPLUGIN_H

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Containers/Record.h>
#include <casa/Containers/RecordInterface.h>

#include <Calibration/PluginBase.h>
#include <Calibration/RFIMitigation.h>

#include <casa/namespace.h>

namespace CR { //namespace CR -- begin

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
  
   This class is a wrapper for class RFIMitigation developed for for the rejection
   of noise signals.
    
    <h3>Parameter</h3>
    
    
  \param spectra	--	two dimensional complex array (FFT) of data in which number of rows depends 
  				on number of frequency channels selected and number of columns depends on the
				selected number of antennas.
 
  \param nOfSegments    --     An unsigned integer that signifies no of segments in which the gainvalue 
  			       array has to be divided, for the interpolation of gaincurve.
			       The default value is set on 25, if it is greater than 25 then we will trace the
			       bumps in the gaincurve which will be difficult to remove, moreover, if it is less 
			       than 25, we will compromise while interpolating the exact shape of the gain curve
			       excluding RFIs.
  
  \param dataBlockSize  --	Dimension of the array of gaincurves selected for the RFI removal.
    
   
  <h3>Example(s)</h3>

*/

  class RFIMitigationPlugin : public PluginBase<DComplex> {

  /*
  //Matrix<DComplex> spectra_p ;
  //! Size of a single block of data, [samples]
  uint dataBlockSize_p;

  //! Number of segments into which the spectrum is about to be divided
  uint nOfSegments_p;*/

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
  
  
   //Bool setSpectra( Record const &param ) ;
    
   Bool setDataBlockSize( Int NewDataBlockSize ) ;
    
  Bool setNOfSegments( Int NewNOfSegments ) ;
    
    /*!
    \brief Set the internal parameters controlling the behaviour of the plugin
  */
   
   Bool calcWeights(const Matrix<DComplex> &spectra ); 
 
 /*! 
 \brief Get the name of the class
 
 \return className -- The name of the class, RFIMitigationPlugin.
 
 */
 
 std::string className () const {
  return "RFIMitigationPlugin" ;
 };

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

} // Namespace CR -- end

#endif /* RFIMITIGATIONPLUGIN_H */
