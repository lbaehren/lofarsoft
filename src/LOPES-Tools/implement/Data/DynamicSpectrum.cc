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

/* $Id: template-class.cc,v 1.9 2006/04/14 11:34:17 bahren Exp $*/

#include <Data/DynamicSpectrum.h>
#include <fitsio.h>

namespace LOPES {
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  DynamicSpectrum::DynamicSpectrum ()
  {  
    init (defaultShape());
  }
  
  DynamicSpectrum::DynamicSpectrum (blitz::Array<int,1> const &shape)
  {
    init(shape);
  }
  
  DynamicSpectrum::DynamicSpectrum (blitz::Array<int,1> const &shape,
				    blitz::Array<double,1> const &crpix,
				    blitz::Array<double,1> const &crval,
				    blitz::Array<double,1> const &cdelt)
  {
    setShape (shape);
    setRefPixel (crpix);
    setRefValue (crval);
    setIncrement (cdelt);
  }
  
  DynamicSpectrum::DynamicSpectrum (DynamicSpectrum const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  DynamicSpectrum::~DynamicSpectrum ()
  {
    destroy();
  }
  
  void DynamicSpectrum::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  DynamicSpectrum& DynamicSpectrum::operator= (DynamicSpectrum const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void DynamicSpectrum::copy (DynamicSpectrum const &other)
  {
    pixels_p = other.pixels_p;
    shape_p  = other.shape_p;
    crpix_p  = other.crpix_p;
    crval_p  = other.crval_p;
    cdelt_p  = other.cdelt_p;
  }
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void DynamicSpectrum::setShape (blitz::Array<int,1> const &shape)
  {
    shape_p = shape;
  }
  
  void DynamicSpectrum::setRefPixel (blitz::Array<double,1> const &crpix)
  {
    if (crpix.numElements() == nofAxes) {
      crpix_p = crpix;
    } else {
      std::cerr << "[DynamicSpectrum::setRefPixel] Invalid number of axes!"
		<< std::endl;
    }
  }
  
  void DynamicSpectrum::setRefValue (blitz::Array<double,1> const &crval)
  {
    if (crval.numElements() == nofAxes) {
      crval_p = crval;
    } else {
      std::cerr << "[DynamicSpectrum::setRefPixel] Invalid number of axes!"
		<< std::endl;
    }
  }
  
  void DynamicSpectrum::setIncrement (blitz::Array<double,1> const &cdelt)
  {}
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  // ----------------------------------------------------------------------- init

  void DynamicSpectrum::init (blitz::Array<int,1> const &shape)
  {
    uint nelem (shape.numElements());
    
    /* Input array must be of rank 1, containing two entries */
    if (nelem == nofAxes) {
      // copy the pixel map shape
      shape_p = shape;
      // Coordinate axes information
      blitz::Array<double,1> tmp (nofAxes);
      tmp = 1.0,1.0;
      crpix_p = tmp;
      crval_p = tmp;
      cdelt_p = tmp;
    } else {
      std::cerr << "[DynamicSpectrum::init] Invalid number of axes" << std::endl;
      std::cerr << " - Expected : " << nofAxes << std::endl;
      std::cerr << " - Detected : " << nelem   << std::endl;
    }
  }
  
  // ---------------------------------------------------------------- setSpectrum

  void DynamicSpectrum::setSpectrum (blitz::Array<std::complex<double>,2> const &spectra,
				     int const &timestep)
  {
    if (timestep>-1 && timestep<shape_p(1)) {
      // check the shape of the input data
//       blitz::TinyVector<int,spectrum.rank()> shape (spectrum.shape());
    } else {
      std::cerr << "[DynamicSpectrum::setSpectrum]" << std::endl;
    }
  }

  // ---------------------------------------------------------- collapseFrequency
  
  blitz::Array<double,1> DynamicSpectrum::collapseFrequency ()
  {
    blitz::Array<double,1> timeseries (shape_p(1));
    int timestep (0);
    int channel (0);
    
    for (timestep=0; timestep<shape_p(1); timestep++) {
      timeseries(timestep) = 0.0;
      for (channel=0; channel<shape_p(0); channel++) {
	timeseries(timestep) += pixels_p(channel,timestep);
      }
    }
    
    return timeseries;
  }

  // ------------------------------------------------------------------- spectrum

  blitz::Array<double,1> DynamicSpectrum::spectrum (bool const &normalize)
  {
    blitz::Array<double,1> out (shape_p(0));
    int timestep (0);
    int channel (0);
    
    for (channel=0; channel<shape_p(0); channel++) {
      out(channel) = 0.0;
      for (timestep=0; timestep<shape_p(1); timestep++) {
	out(channel) += pixels_p(channel,timestep);
      }
    }
    
    /* 
       Normalize the spectrum? If yes, then divide by the number of timesteps
    */
    if (normalize) {
      out /= shape_p(1);
    }
    
    return out;
  }
  
  // ----------------------------------------------------------------- write2fits

  bool DynamicSpectrum::write2fits (std::string const &filename,
				    bool const &overwrite)
  {
    std::cout << "[DynamicSpectrum::write2fits]" << std::endl;
    
    bool ok (true);
    int status (0);
    long naxis (2);
    long naxes[2] = {long(shape_p(0)),
		     long(shape_p(1))};
    fitsfile *fptr;

    std::cout << " - Creating new FITS file ... " << std::endl;
    fits_create_file (&fptr,
		      filename.c_str(),
		      &status);

    if (status) {
      std::cerr << "write2fits : Error creating output file "
		<< filename
		<< std::endl;
      ok = false;
    }
    
    std::cout << " - Creating primary image array ... " << std::endl;
    fits_create_img (fptr,
		     DOUBLE_IMG,
		     naxis,
		     naxes,
		     &status);
    
    return ok;
  }
}

