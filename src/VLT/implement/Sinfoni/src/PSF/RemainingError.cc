//#  RemainingError.cc: Covariance matrix of the remaining (or aliasing) error.
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
//#  $Id: RemainingError.cc,v 1.6 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/RemainingError.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/AsciiReader.h>
#include <Sinfoni/Exceptions.h>
#include <Sinfoni/Debug.h>
#include <cerrno>

using namespace blitz;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      RemainingError::RemainingError(Integer size) : 
        itsCovariance(size, size)
      {
        //# Load the covariance matrix of the remaining (or aliasing) error
        //# for \f$D/r_0=1\f$ from file.
        DBG_TRACE("Reading scaled covariance matrix of the remaining "
                  "(or aliasing) error ...");
        AsciiReader reader(DataFile::remainingErrorCovariance);
        reader.read(itsCovariance);

        //# Dump the covariance matrix to file if debugging is enabled.
        DBG_DUMP("RemainingError.out", itsCovariance);
      }


      MatrixReal RemainingError::covariance(Real d_r0) const
      {
        MatrixReal cov(itsCovariance.shape());
        cov = itsCovariance * pow(d_r0, Real(5.0/3.0));
        if (errno) THROW (MathException, strerror(errno));
        return cov;
      }


      const RemainingError& theRemainingError()
      {
        static RemainingError re(theNrOfSubApertures);
        return re;
      }


    } //# namespace PSF

  } //# namespace SINFONI
  
} //# namespace ASTRON
