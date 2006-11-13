//#  ControlMatrix.h: Class representing the so-called Control Matrix.
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
//#  $Id: ControlMatrix.h,v 1.11 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_CONTROLMATRIX_H
#define SINFONI_PSF_CONTROLMATRIX_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      // The control matrix, denoted by \f$\mathbf{D}^+\f$, is also known as
      // the generalized inverse of the modal interaction matrix.
      //
      // \note This class acts like a Singleton class, but it is implemented a
      // bit different from the original design pattern. We use the global
      // function theControlMatrix() to return the contents of the control
      // matrix. This function stores a static instance of ControlMatrix,
      // hence it will be constructed when theControlMatrix() is called for
      // the first time.
      class ControlMatrix
      {
      private:
        // The friend function is used to return the contents of the control
        // matrix.
        friend const MatrixReal& theControlMatrix();

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        ControlMatrix();
        ControlMatrix(const ControlMatrix&);
        ControlMatrix& operator=(const ControlMatrix&);
        //@}
        
        // This is the only implemented constructor. It creates a square
        // matrix of size \a size. The data are read from the file
        // DataFile::controlMatrix, and subsequently transformed from \e
        // voltage-basis to \e Zernike-basis.
        //
        // \note In the future, the data may be stored in a file format that
        // has a header which documents the number of rows and columns,
        // etc. Then, we won't need the \a size argument anymore as it can be
        // read from the data file.
        ControlMatrix(Integer size);

        // The data in the control matrix is stored on the so-called
        // voltage-basis. However, we need to have the data on the
        // Zernike-basis. This method takes care of the conversion. It will be
        // called by the constructor of this class, after the data have been
        // loaded into memory.
        void voltageToZernike();

        // The contents of the control matrix.
        MatrixReal itsMatrix;
      };


      // Get the control matrix. The control matrix is precalculated and will
      // be read from file. In the current design, this file contains the
      // response of the deformable mirros to the applied voltages on the
      // so-called potential or voltage basis. However, we need to have this
      // response on the Zernike basis. Hence, we will need to transform the
      // control matrix from voltage basis to Zernike basis. This transform is
      // also precalculated and will be read from file.
      //
      // \return The control matrix on the Zernike basis, also known as the
      // generalized inverse of the modal interaction matrix
      // \f$\mathbf{D}^+\f$.
      const MatrixReal& theControlMatrix();

    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
