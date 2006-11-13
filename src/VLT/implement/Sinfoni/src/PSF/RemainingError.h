//#  RemainingError.h: Covariance matrix of the remaining (or aliasing) error.
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
//#  $Id: RemainingError.h,v 1.4 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_REMAININGERROR_H
#define SINFONI_PSF_REMAININGERROR_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      // This class represents the <em>remaining (or aliasing) error</em>. 
      class RemainingError
      {
      public:
        // Return the covariance of the remaining (or aliasing) error. Note
        // that the matrix must be scaled using the formula:
        //\f[
        //  \mathbf{C}_{rr} = (\mathbf{C}_{rr})_{D/r_0=1} (\frac{D}{r_0})^{5/3}
        //\f]
        // where \f$D/r_0\f$ is passed in as \a d_r0.
        MatrixReal covariance(Real d_r0) const;

      private:
        // The friend function is used to return an instance of the class
        // RemainingError.
        friend const RemainingError& theRemainingError();

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        RemainingError();
        RemainingError(const RemainingError&);
        RemainingError& operator=(const RemainingError&);
        //@}
        
        // This is the only implemented constructor. It creates a square
        // matrix of size \a size. The data are read from the file
        // DataFile::remainingErrorCovariance and cached in the data member \c
        // itsCovariance.
        RemainingError(Integer size);

        // The covariance matrix of the remaining (or aliasing) error for
        // \f$D/r_0=1\f$.
        MatrixReal itsCovariance;
      };
      
    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
