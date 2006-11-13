//#  PSFConfig.h: PSF specific configuration file
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
//#  $Id: PSFConfig.h,v 1.10 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_PSFCONFIG_H
#define SINFONI_PSF_PSFCONFIG_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>
#include <string>
#include <cstdio>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      // The number of sub-apertures in the Sinfoni instrument.
      const Integer theNrOfSubApertures = 60;

      // The number of samples used sampling the pupil.
      const Integer theNrOfPupilSamples = 128;

      // The number of mirror modes used to compute \f$D/r0\f$. This number
      // should correspond to a good fitting of the calculated variance of the
      // mode coefficients in the turbulent phase (\c var_a in
      // ApertureToFriedRatio::estimate()). In the future an automatic
      // selection of the modes should be developed.
      const Integer theNrOfUsedMirrorModes = 45;

      // The "reference" wave length of the VLT instrument (2200 nm).
      const Real theVLTWaveLength = 2200e-9;

      // Namespace containing the names of the data files that are used. The
      // sole purpose of this namespace is to bundle these filenames in one
      // place.
      namespace DataFile
      {
        // The environement variable \c PSF_DATA can be used to specifiy where
        // to search for the data files. If \c PSF_DATA undefined, the current
        // directory is used to search for the data files.
        inline std::string dataDir()
        {
          static std::string dir;
          if (dir.empty()) {
            char* c = getenv("PSF_DATA");
            if (c) dir = c; else dir = ".";
            dir += "/";
          }
          return dir;
        }

        // Data file containing the control matrix.
        const std::string 
        controlMatrix            = dataDir() + "params_contmat.dat";

        // Data file containing the transformed Noll matrix.
        const std::string 
        transformedNollMatrix    = dataDir() + "Caa_300_128.dat";

        // Data file containing the covariance of the remaining (or aliasing)
        // error.
        const std::string 
        remainingErrorCovariance = dataDir() + "Crr_g6_f25.dat";

        // Data file containing the Voltage-to-Zernike transformation matrix.
        const std::string 
        voltageToZernike         = dataDir() + "v2z_3136.dat";

        // Data file containing the structure function for the high order
        // component of the residual phase.
        const std::string 
        structFuncHighOrderPhase = dataDir() + "Dphi_20s_data_128_1Delta.dat";

        // Data file name of the function of the mirror modes, the so-called
        // U-functions, for the indices \a i and \a j.
        //
        // \note The file naming convention dictates that index numbering
        // starts at 1 (Matlab convention). However, in C++ index numbering
        // starts at 0. Hence we must add 1 to \a i and \a j.
        inline std::string uFunction(int i, int j)
        {
          char s[10];
          sprintf(s, "u%02d_%02d.dat", i+1, j+1);
          return dataDir() + s;
        }

      } // namespace DataFile

    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
