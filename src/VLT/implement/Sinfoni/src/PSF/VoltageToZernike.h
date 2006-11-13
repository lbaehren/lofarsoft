//#  VoltageToZernike.h: Transformation matrix from voltage to Zernike basis.
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
//#  $Id: VoltageToZernike.h,v 1.4 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_VOLTAGETOZERNIKE_H
#define SINFONI_PSF_VOLTAGETOZERNIKE_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      /*! 
	   This class represents the transformation matrix used to transform
       datafrom the so-called potential or voltage basis to the Zernike
       basis. We need this transformation matrix, because the precalculated
       control matrix as well as the control data of the deformable mirror
       are on the so-called potential or voltage basis. However, we need to
       have these data on the Zernike basis. This transformation matrix is
       precalculated and will be read from file.
      
       \note This class acts like a Singleton class, but it is implemented a
       bit different from the original design pattern. We use the global
       function theVoltageToZernike() to return an instance of the class
       VoltageToZernike. This function stores a static instance of
       VoltageToZernike, hence it will be constructed when
       theVoltageToZernike() is called for the first time.
      
       \see ControlMatrix, InputData::mirrorCovariance.
	  */
      class VoltageToZernike
      {
        // The friend function is used to return the contents of the control
        // matrix.
        friend const MatrixReal& theVoltageToZernike();

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        VoltageToZernike();
        VoltageToZernike(const VoltageToZernike&);
        VoltageToZernike& operator=(const VoltageToZernike&);
        //@}
        
        // This is the only implemented constructor. It creates a square
        // matrix of size \a size. The data are read from the file
        // DataFile::voltageToZernike.
        VoltageToZernike(Integer size);

        // The contents of the control matrix.
        MatrixReal itsMatrix;
      };

      // Return the matrix that transforms data from the potential or voltage
      // basis to the Zernike basis.
      const MatrixReal& theVoltageToZernike();
      
    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
