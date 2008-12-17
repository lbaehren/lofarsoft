/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
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

// ------------------------------------------------------------------------------

/*!
  \defgroup CR Cosmic Rays (CR) analysis pipeline
  
  This documentation contains the description of the C/C++ components of the
  software used for data inspection and reduction with the
  <a href="http://www.astro.ru.nl/lopes">LOPES</a> and LOFAR/CR experiments.

  <ul>
    <li>\ref crtools_build
    <li>\ref crtools_source_tree
    <li>\ref crtools_changes
    <li>\ref crtools_imaging
    <li>\ref tasks
  </ul>

  <h3>Prerequisite</h3>

  <ul>
    <li><a href="group__DAL.html">Data Access Library</a> (DAL)
    <li><a href="http://www.astron.nl/casacore/doc/html">casacore API
    documentation</a>
  </ul>
  
  <h3><a name="Synopsis">Synopsis</a></h3>

  Actively developed and supported successor to the older LOPES-Tools package.

*/

//_________________________________________________________________ CR-Tools/apps

/*!
  \defgroup CR_Applications CR-Tools :: Applications collection
  \ingroup CR
*/

//_______________________________________________________________________________
//                                                    CR-Tools/implement/Analysis

/*!
  \defgroup Analysis CR-Tools :: Analysis module
  \ingroup CR

  \brief Analysis module for the CR-pipeline
  
  <h3>Synopsis</h3>
  
*/

//_______________________________________________________________________________
//                                          CR-Tools/implement/ApplicationSupport

/*!
  \defgroup ApplicationSupport CR-Tools :: ApplicationSupport module
  \ingroup CR

  \brief ApplicationSupport module for the CR-pipeline
  
  <h3>Synopsis</h3>
  
*/

//_______________________________________________________________________________
//                                                 CR-Tools/implement/Calibration

/*!
  \defgroup Calibration CR-Tools :: Calibration module
  \ingroup CR

  \brief Calibration module for the CR-pipeline
  
  <h3>Synopsis</h3>
  
*/

//_______________________________________________________________________________
//                                                        CR-Tools/implement/Data

/*!
  \defgroup CR_Data CR-Tools :: Data module
  \ingroup CR

  \brief Data module for the CR-pipeline
  
  <h3>Synopsis</h3>

  This module collects various classes and functions used for the interaction
  with datasets - be they from the LOPES experiment or the LOFAR telescope.

  <ul>
    <li>The structure of a dataset containing a dump from the LOFAR transient 
    buffer boards (TBBs) is mapped onto a hierarchy of C++ classes:
    <ul>
      <li>DAL::LOFAR_Timeseries -- Top-level structure of a time-series dataset
      <li>DAL::LOFAR_StationGroup -- Group collecting the data for an individual
      LOFAR station.
      <li>DAL::LOFAR_DipoleDataset
    </ul>
  </ul>
  
*/

//_______________________________________________________________________________
//                                                     CR-Tools/implement/Display

/*!
  \defgroup CR_Display CR-Tools :: Display module
  \ingroup CR

  \brief Display module for the CR-pipeline
*/

//_______________________________________________________________________________
//                                                         CR-Tools/implement/GUI

/*!
  \defgroup CR_GUI CR-Tools :: Graphical user-interface module
  \ingroup CR

  \brief The basic data get/put mechanism for the Plotting Tool for LOFAR Data

  <b>Note:</b> This module is optional and -- at least for the moment -- not
  included in the default build of the CR-Tools software package; since the code
  within this module is highly experimental and still subject to the design 
  process it has to be enabled explicitelty by adding
  \verbatim
  -DCR_WITH_GUI:BOOL=TRUE
  \endverbatim
  to the list of CMake configuration parameters.
  
  <h3>Usage</h3>

  In order to use the generated shared object library in Python run
  \verbatim
  python 
  from libhfget import *
  \endverbatim
  Once you have done this you can make use of the actual functionaly provided 
  by the module:
  \verbatim
  d=Data("LOFAR")
  d.printStatus(1)
  v=d.newObjects("Header")
  d2=d.ObjectID(v[0])
  v=d.newObjects("Header:X")
  len(v)
  d.printDecendants("")
  DIR.TO+1
  
  v2=IntVec()
  v[:]=[0,1,2,3,4,5,6,7]
  v[3:4]=[8,9]
  \endverbatim
*/

// ==============================================================================
//
//  Module IO
//
// ==============================================================================

/*!
  \defgroup IO CR-Tools :: IO module
  \ingroup CR

  \brief IO module for the CR-pipeline
  
  <h3>Synopsis</h3>
  
*/

// ==============================================================================
//
//  Module LopesLegacy
//
// ==============================================================================

/*!
  \defgroup LopesLegacy CR-Tools :: LopesLegacy module
  \ingroup CR

  \brief LopesLegacy module for the CR-pipeline
  
  <h3>Synopsis</h3>

*/

// ==============================================================================
//
//  Module Math
//
// ==============================================================================

/*!
  \defgroup CR_Math CR-Tools :: Math module
  \ingroup CR

  \brief Math module for the CR-pipeline
  
  <h3>Synopsis</h3>

  Even though we make usage of the extensive functionality of the CASA math
  modules, there sometimes are operations missing which will be required 
  over and over again. The Math module of the CR-Pipeline is a collection of
  such additional functions.
*/

// ==============================================================================
//
//  Module Observation
//
// ==============================================================================

/*!
  \defgroup Observation CR-Tools :: Observation module
  \ingroup CR

  \brief Observation module for the CR-pipeline
  
  <h3>Synopsis</h3>
  
*/

// ==============================================================================
//
//  Module Utilities
//
// ==============================================================================

/*!
  \defgroup CR_Utilities CR-Tools :: Utilities collection
  \ingroup CR

  \brief Utilities collection for the CR-pipeline
  
  <h3>Synopsis</h3>

*/
