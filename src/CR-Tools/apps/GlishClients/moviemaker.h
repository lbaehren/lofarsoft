/***************************************************************************
 *   Copyright (C) 2004,2005                                               *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

// include C++ standard header files
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

// include the AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Quanta/QC.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Cube.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>

#include <ApplicationSupport/Glish.h>
#include <ApplicationSupport/GlishRecord.h>

#include <casa/namespace.h>

/*!
  \file MovieMaker.h
  
  ingroup CR_Applications

  \brief Additional functionality for the moviefromimage tool.

  \author Lars B&auml;hren

  \date 2004/02/04
*/

/*!
  \fn Bool interpolateFrames (GlishSysEvent &event, void *)

  \brief Compute interpolation frames.

  Introduce a number of interpolation frames into a given sequence of images.
  This routine mainly is thought for the production of a movie from an AIPS++
  image tool.

  \param pixelsIN         = Data cube with the original image frames.
  \param shape            = Shape of the input data cube.
  \param additionalFrames = The number of additional frames introduced to
                            smoothen the transition between the original frames.

  \retval pixelsOUT      = Data cube with the new extended set of image frames.

*/
Bool interpolateFrames (GlishSysEvent &event, void *);
