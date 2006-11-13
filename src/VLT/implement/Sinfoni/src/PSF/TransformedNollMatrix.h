//#  TransformedNollMatrix.h: Class representing the transformed Noll matrix.
//#
//#  Copyright (C) 2002-2006
//#  ASTRON (Netherlands Foundation for Research in Astronomy)
//#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
//#
//#  This program is free software; you can redistribute it and/or modify
//#  it under the terms of the GNU General Public License as published by
//#  the Free Software Foundation; either version 2 of the License, or
//#  (at your option) any later version.
//#
//#  This program is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#  GNU General Public License for more details.
//#
//#  You should have received a copy of the GNU General Public License
//#  along with this program; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//#
//#  $Id: TransformedNollMatrix.h,v 1.4 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_TRANSFORMEDNOLLMATRIX_H
#define SINFONI_PSF_TRANSFORMEDNOLLMATRIX_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      // This class represents the transformed Noll matrix, denoted by
      // \f$\mathbf{K}'\f$, which is in fact the covariance matrix of the
      // theorical mirror modes. It can be calculated by transforming the
      // so-called Noll matrix from Zernike basis to mirror mode basis.
      // \f$\mathbf{K}'\f$ relates the covariance matrix of the actual mirror
      // mode coefficients \f$\mathbf{C}_{aa}\f$ to the ratio of the telescope
      // aperture and the Fried parameters \f$D/r_0\f$.
      //
      // \note This class acts like a Singleton class, but it is implemented a
      // bit different from the original design pattern. We use the global
      // function theTransformedNollMatrix() to return the contents of the
      // TransformedNoll matrix. This function stores a static instance of
      // TransformedNollMatrix, hence it will be constructed when
      // theTransformedNollMatrix() is called for the first time.
      class TransformedNollMatrix
      {
      private:
        // The friend function is used to return the contents of the
        // transformed Noll matrix.
        friend const MatrixReal& theTransformedNollMatrix();

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        TransformedNollMatrix();
        TransformedNollMatrix(const TransformedNollMatrix&);
        TransformedNollMatrix& operator=(const TransformedNollMatrix&);
        //@}
        
        // This is the only implemented constructor. It creates a square
        // matrix of size \a size. The precalculated data are read from the
        // file DataFile::transformedNollMatrix and cached in the data member
        // \c itsMatrix.
        TransformedNollMatrix(Integer size);

        // The contents of the transformed Noll matrix
        MatrixReal itsMatrix;
      };

      // Return the so-called transformed Noll matrix. 
      const MatrixReal& theTransformedNollMatrix();

    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
