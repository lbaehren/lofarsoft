/*-------------------------------------------------------------------------*
 | $Id:: IO.h 393 2007-06-13 10:49:08Z baehren                           $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
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

/*!
  \file Analysis.h

  \defgroup Analysis CR-Pipeline: Analysis module
  \ingroup CR

  \brief Analysis module for the CR-pipeline
  
  \author Lars B&auml;hren
  
  \date 2007/01/16

  <h3>Synopsis</h3>
  
  <h3>Usage</h3>

  Use this file in order to import all the prototypes for functions and classes
  in the <i>IO</i> module.
*/

#include <Analysis/ClippingFraction.h>
#include <Analysis/DataBlockStatistics.h>
#include <Analysis/DynamicSpectrum.h>
#include <Analysis/FirstStagePipeline.h>
#include <Analysis/inverseFFT.h>
#include <Analysis/Masking.h>
#include <Analysis/Peak.h>
#include <Analysis/PeakList.h>
#include <Analysis/PeakSearch.h>
#include <Analysis/SecondStagePipeline.h>
#include <Analysis/SourceFit.h>
