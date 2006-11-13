//#  config.h: Global configuration for the Sinfoni/PSF software.
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
//#  $Id: Config.h,v 1.6 2006/05/19 16:03:23 loose Exp $

#ifndef SINFONI_CONFIG_H
#define SINFONI_CONFIG_H

#if 1

//# This include is specific for the LOFAR build environment.
#include <lofar_config.h>
#if defined(LOFAR_DEBUG)
#define SINFONI_DEBUG 1
#define SINFONI_TRACE 1
#else
#define SINFONI_DEBUG 0
#define SINFONI_TRACE 1
#endif

#else

//# Use this one when building outside the LOFAR build environment.
#include <config.h> 
#if defined(NDEBUG)
#define SINFONI_DEBUG 0
#define SINFONI_TRACE 1
#else
#define SINFONI_DEBUG 1
#define SINFONI_TRACE 1
#endif

#endif


#if (SINFONI_DEBUG == 1)
#define BZ_DEBUG
#else
#undef BZ_DEBUG
#endif

//# Define if calculations need to be done in single precision. This will
//# roughly half the amount of memory used. It may also reduce processing
//# time, if you have an older CPU.
/* #define USE_SINGLE_PRECISION */


#endif
