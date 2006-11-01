/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars Baehren (bahren@astron.nl)                                       *
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

/* $Id: template-class.h,v 1.15 2006/06/20 09:42:16 bahren Exp $*/

#ifndef LOPESTOOLS_DYNAMICSPECTRUM_H
#define LOPESTOOLS_DYNAMICSPECTRUM_H

#include <iostream>
#include <cmath>
#include <complex>

#include <blitz/array.h>

//! The number of image axes
const unsigned int nofAxes = 2;

namespace LOPES {
  
  /*!
    \class DynamicSpectrum
    
    \ingroup Data
    
    \brief Brief description for class DynamicSpectrum
    
    \author Lars B&auml;hren
    
    \date 2006/10/25
    
    \test tDynamicSpectrum.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */
  class DynamicSpectrum {
    
    //! Pixel values
    blitz::Array<double,2> pixels_p;
    
    //! Shape of the pixel array, [freq,time]
    blitz::Array<int,1> shape_p;
    
    //! FITS keyword: CRPIX
    blitz::Array<double,1> crpix_p;
    //! FITS keyword: CRVAL
    blitz::Array<double,1> crval_p;
    //! FITS keyword: CDELT
    blitz::Array<double,1> cdelt_p;
    //! FITS keyword: CUNIT
    blitz::Array<std::string,1> cunit_p;
    
  public:

    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor

      All FITS WCS parameters will be set to 1, the pixel map array will be
      set to shape (1,1).
    */
    DynamicSpectrum ();
    
    /*!
      \brief Argumented constructor
      
      \param shape -- Shape of the pixel array, [freq,time]
    */
    DynamicSpectrum (blitz::Array<int,1> const &shape);
    
    /*!
      \brief Argumented constructor
      
      \param shape -- Shape of the pixel array, [freq,time]
      \param crpix -- Reference pixel; FITS WCS key word
      \param crval -- Reference value; FITS WCS key word
      \param cdelt -- Coordinate increment; FITS WCS key word
    */
    DynamicSpectrum (blitz::Array<int,1> const &shape,
		     blitz::Array<double,1> const &crpix,
		     blitz::Array<double,1> const &crval,
		     blitz::Array<double,1> const &cdelt);
    
    /*!
      \brief Copy constructor
      
      \param other -- Another DynamicSpectrum object from which to create this new
      one.
    */
    DynamicSpectrum (DynamicSpectrum const &other);
    
    // -------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~DynamicSpectrum ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another DynamicSpectrum object from which to make a copy.
    */
    DynamicSpectrum& operator= (DynamicSpectrum const &other); 
    
    // --------------------------------------------------------------- Parameters

    /*!
      \brief Get the array containing the pixel map of the dynamic spectrum
      
      \return pixels -- Pixel map array of the dynamic spectrum
    */
    blitz::Array<double,2> pixels () const {
      return pixels_p;
    }
    
    /*!
      \brief Get the shape of the pixel map

      \return shape -- Shape of the pixel array, [freq,time]
    */
    blitz::Array<int,1> shape() const {
      return shape_p;
    }

    /*!
      \brief Set the shape of the pixel map

      \param shape -- Shape of the pixel array, [freq,time]
    */
    void setShape (blitz::Array<int,1> const &shape);
    
    /*!
      \brief Get the reference pixel

      \return crpix -- Reference pixel; FITS WCS key word
    */
    blitz::Array<double,1> refPixel () const {
      return crpix_p;
    }
    
    /*!
      \brief Set the reference pixel

      \param crpix -- Reference pixel; FITS WCS key word
    */
    void setRefPixel (blitz::Array<double,1> const &crpix);
    
    /*!
      \brief Get the reference value

      \return crval -- 
    */
    blitz::Array<double,1> refValue () const {
      return crval_p;
    }
    
    /*!
      \brief Set the reference value

      \param crval -- 
    */
    void setRefValue (blitz::Array<double,1> const &crval);
    
    /*!
      \brief Get the coordinate increment

      \return cdelt -- Coordinate increment
    */
    blitz::Array<double,1> increment () const {
      return cdelt_p;
    }
    
    /*!
      \brief Set the coordinate increment

      \param cdelt -- Coordinate increment
    */
    void setIncrement (blitz::Array<double,1> const &cdelt);

    /*!
      \brief Get the physical units to the image axes

      \return units -- Physical units associated with the image axes
    */
    blitz::Array<std::string,1> axisUnits () const {
      return cunit_p;
    }
    
    /*!
      \brief Set the physical units to the image axes

      \param units -- Physical units associated with the image axes
    */
    void setAxisUnits (blitz::Array<std::string,1> const &cunit);
    
    // ------------------------------------------------------------------ Methods
    
    /*!
      \brief Set a dynamic spectrum slice from antenna spectra
      
      \param spectra  -- [freq,antennae]
      \param timestep -- Timestep at which to insert the spectrum
    */
    void setSpectrum (blitz::Array<std::complex<double>,2> const &spectra,
		      int const &timestep=0);
    
    /*!
      \brief Collapse the dynamic spectrum along the frequency axis

      Collapsing the dynamic spetrum \f$ \widetilde P [k,m] \f$ along the
      frequency axis yields the total power as function of time:
      \f[
      \widetilde P [m] = \frac{1}{N_{\rm Channels}} \sum_{k} \widetilde S [k,m]
      \overline{\widetilde S [k,m]}
      \f]
      where the index \f$ m \f$ runs over the number of datablocks of length
      \f$ N_{\rm Blocksize} \f$.

      \return timeseries -- 
    */
    blitz::Array<double,1> collapseFrequency ();
    
    /*!
      \brief Collapse the dynamic spectrum along the time axis

      Collapsing the dynamic spetrum \f$ \widetilde S [k,m] \f$ along the
      time axis yields the average power spectrum:
      \f[
      \widetilde P [k] = \frac{1}{N_{\rm Blocks}} \sum_{m} \widetilde S [k,m]
      \overline{\widetilde S [k,m]}
      \f]
      where the sum extends over all time-steps (or blocks).

      \return spectrum -- 
    */
    blitz::Array<double,1> spectrum (bool const &normalize=true);

    /*!
      \brief Export the pixel map to a FITS file

      \param filename  -- Name of the output file
      \param overwrite -- Overwrite file, in case it already exists? By default
                          we do not overwrite an already existing file of the 
			  same name.

      \return ok -- Returns <i>true</i> if everything went fine; returns
                    <i>false</i> in case an error was encountered.
    */
    bool write2fits (std::string const &filename,
		     bool const &overwrite=false);
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (DynamicSpectrum const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
    /*!
      \brief Initialize object based on the shape
      
      \param shape -- Shape of the pixel array
    */
    void init (blitz::Array<int,1> const &shape);
    
    blitz::Array<int,1> defaultShape () {
      blitz::Array<int,1> shape (nofAxes);
      shape = 1,1;
      return shape;
    }
    
  };

}
  
#endif /* LOPESTOOLS_DYNAMICSPECTRUM_H */
  
