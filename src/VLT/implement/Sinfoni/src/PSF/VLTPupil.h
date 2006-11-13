//#  VLTPupil.h: Class representing the pupil of the VLT telescope
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
//#  $Id: VLTPupil.h,v 1.5 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_VLTPUPIL_H
#define SINFONI_PSF_VLTPUPIL_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      // This class represent the pupil of the VLT telescope.
      class VLTPupil
      {
      public:
        // Return the mask that corresponds to the pupil.
        const MatrixInteger& mask() const { return itsMatrix; }

        // Return the diameter of the pupil.
        static Real diameter() { return theirPupilDiameter; }

      private:
        // The friend function is used to return the pupil of the VLT
        // telescope.
        friend const VLTPupil& theVLTPupil();

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        VLTPupil();
        VLTPupil(const VLTPupil&);
        VLTPupil& operator=(const VLTPupil&);
        //@}

        // This is the only implemented constructor. It creates a pupil on a
        // square grid of size \a size. The pupil will be centered exactly in
        // the middle of the grid, i.e. at 0.5 * (\a size - 1).
        //
        // If \a hasCentralObstruction is \c true, the central obstruction of
        // the mount will be taken into account. If \a hasSpiderMount is \c
        // true, the spider mount will be taken into account.
        VLTPupil(int size, 
                 bool hasCentralObstruction = true, 
                 bool hasSpiderMount = false);

        // Create the primary mask, which corresponds to the unobstructed
        // pupil.
        void createPrimaryMask();

        // Add the secondary mask, which represents the central obstruction.
        void addSecondaryMask();

        // Add the spider mount of the central obstruction. The exact shape of
        // the spider mount depends on the presence or absence of the
        // secondary mask (the central obstruction).
        void addSpiderMount(bool hasSecondaryMask);

        // Pupil diameter in meters.
        static const Real theirPupilDiameter;

        // Diameter of the central obstruction in meters.
        static const Real theirSecondaryDiameter;

        // Angle of the legs of the spider mount in degrees.
        static const Real theirSpiderAngle;

        // Width of the legs of the spider mount in meters.
        static const Real theirSpiderWidth;

        // Distance between two grid lines, expressed in meters.
        Real itsDelta;

        // Matrix containing the mask that corresponds to the pupil.
        MatrixInteger itsMatrix;
      };

      // Return the pupil of the VLT telescope.
      const VLTPupil& theVLTPupil();
      
    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
