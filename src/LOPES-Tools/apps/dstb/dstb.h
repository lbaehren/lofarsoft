/***************************************************************************
 *   Copyright (C) 07.02.2006                                              *
 *   Andreas Nigl (anigl@astro.ru.nl)                                      *
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

/*!
  \file dstb.h

  \ingroup Applications
  
  \brief Generation of dynamic spectrum on timebeam
  
  \author Andreas Nigl
 */

// casa wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/stdio.h>
#include <casa/iostream.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Containers/Map.h>
#include <casa/Containers/OrderedMap.h>
#include <casa/OS/Time.h>
#include <casa/Quanta.h>

#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasTable.h>

#include <scimath/Mathematics.h>
#include <tasking/Glish.h>
#include <tasking/Glish/GlishRecord.h>
#include <wcslib/prj.h>

// custom header files
#include <lopes/Math/MathFFT.h>
#include <lopes/Math/MathGeometry.h>
#include <lopes/Math/MathStatistics.h>
#include <lopes/Observation/ObservationData.h>
#include <lopes/Beamforming/Beamformer.h>
#include <lopes/Utilities/StringTools.h>

// casa namespace
#include <casa/namespace.h>

// ----------------------------------------------------- Event handling functions

/*!
  \brief Generation of matrix for dynamic spectrum on timebeam-file
*/
Bool dstb (GlishSysEvent &event, void *);

// -------------------------------------------------------- Computation functions
