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
#include <vector>

//! The number of image axes
const unsigned int nofAxes = 2;

namespace lopestools {
  
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
    std::complex<double> *pixels_p;
    
    //! Shape of the pixel array, [freq,time]
    std::vector<int> shape_p;
    
    //! FITS keyword: CRPIX
    std::vector<double> crpix_p;
    //! FITS keyword: CRVAL
    std::vector<double> crval_p;
    //! FITS keyword: CDELT
    std::vector<double> cdelt_p;
    
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
    DynamicSpectrum (std::vector<int> const &shape);
    
    /*!
      \brief Argumented constructor
      
      \param shape -- Shape of the pixel array, [freq,time]
      \param crpix -- Reference pixel; FITS WCS key word
      \param crval -- Reference value; FITS WCS key word
      \param cdelt -- Coordinate increment; FITS WCS key word
    */
    DynamicSpectrum (std::vector<int> const &shape,
		     std::vector<double> const &crpix,
		     std::vector<double> const &crval,
		     std::vector<double> const &cdelt);
    
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
      \brief Get the shape of the pixel map

      \return shape -- Shape of the pixel array, [freq,time]
    */
    std::vector<int> shape() {
      return shape_p;
    }

    /*!
      \brief Set the shape of the pixel map

      \param shape -- Shape of the pixel array, [freq,time]
    */
    void setShape (std::vector<int> const &shape);
    
    /*!
      \brief Reference pixel

      \return crpix -- Reference pixel; FITS WCS key word
    */
    std::vector<double> crpix () {
      return crpix_p;
    }
    
    /*!
      \brief Reference value

      \return crval -- 
    */
    std::vector<double> crval () {
      return crval_p;
    }
    
    /*!
      \brief Coordinate increment

      \return cdelt -- Coordinate increment
    */
    std::vector<double> cdelt () {
      return cdelt_p;
    }
    
    // ------------------------------------------------------------------ Methods
    
    
    
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
    void init (std::vector<int> const &shape);
    
    std::vector<int> defaultShape () {
      std::vector<int> shape;
      shape.resize (nofAxes,1);
      return shape;
    }
    
  };

}
  
#endif /* LOPESTOOLS_DYNAMICSPECTRUM_H */
  
