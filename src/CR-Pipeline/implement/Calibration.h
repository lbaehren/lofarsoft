/*-------------------------------------------------------------------------*
 | $Id:: Imaging.h 392 2007-06-13 10:38:12Z baehren                      $ |
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
  \file Calibration.h

  \defgroup Calibration CR-Pipeline: Calibration module
  \ingroup CR

  \brief Calibration module for the CR-pipeline
  
  \author Lars B&auml;hren
  
  \date 2007/01/16

  <h3>Usage</h3>

  Usage this file in order to import all the prototypes for functions and classes
  in the <i>Calibration</i> module.
*/

#include <Calibration/AntennaGain.h>
#include <Calibration/AverageGain.h>
#include <Calibration/BaselineGain.h>
#include <Calibration/CalTableCreator.h>
#include <Calibration/CalTableInterpolater.h>
#include <Calibration/CalTableReader.h>
#include <Calibration/CalTableWriter.h>
#include <Calibration/PhaseCalibration.h>
#include <Calibration/PhaseCalibrationPlugin.h>
#include <Calibration/PluginBase.h>
#include <Calibration/RFIMitigation.h>
#include <Calibration/RFIMitigationPlugin.h>
#include <Calibration/TVCalibrationPlugin.h>
