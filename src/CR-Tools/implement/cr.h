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

// ==============================================================================
//
//  Application collection
//
// ==============================================================================

/*!
  \defgroup CR_Applications CR-Pipeline applications collection
  \ingroup CR
*/

// ==============================================================================
//
//  Module Analysis
//
// ==============================================================================

/*!
  \defgroup Analysis CR-Pipeline: Analysis module
  \ingroup CR

  \brief Analysis module for the CR-pipeline
  
  <h3>Synopsis</h3>
  
*/

// ==============================================================================
//
//  Module ApplicationSupport
//
// ==============================================================================

/*!
  \defgroup ApplicationSupport CR-Pipeline: ApplicationSupport module
  \ingroup CR

  \brief ApplicationSupport module for the CR-pipeline
  
  <h3>Synopsis</h3>
  
*/

// ==============================================================================
//
//  Module Calibration
//
// ==============================================================================

/*!
  \defgroup Calibration CR-Pipeline: Calibration module
  \ingroup CR

  \brief Calibration module for the CR-pipeline
  
  <h3>Synopsis</h3>
  
*/

// ==============================================================================
//
//  Module Coordinates
//
// ==============================================================================

/*!
  \defgroup CR_Coordinates CR-Pipeline: Coordinates module
  \ingroup CR

  \brief Coordinates module for the CR-pipeline
  
  <h3>Synopsis</h3>
  
*/

// ==============================================================================
//
//  Module Data
//
// ==============================================================================

/*!
  \defgroup CR_Data CR-Pipeline: Data module
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

// ==============================================================================
//
//  Module GUI
//
// ==============================================================================

/*!
  \defgroup CR_GUI CR-Pipeline: Graphical user-interface module
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
//  Module Imaging
//
// ==============================================================================

/*!
  \defgroup CR_Imaging CR-Pipeline: Imaging module
  \ingroup CR

  \brief Imaging module for the CR-pipeline

  <h3><a name="Synopsis">Synopsis</a></h3>

  The functionality required for beamforming and subsequent imaging is broken
  down into a number of classes.

  <table border="0">
    <tr>
      <td class="indexkey">Quantity</td>
      <td class="indexkey">implemented in</td>
      <td class="indexkey">Relation</td>
    </tr>
    <tr>
      <td>gemoetrical delay</td>
      <td>CR::GeometricalDelay</td>
      <td>\f$ \tau_j = \frac{1}{c} \left( |\vec \rho - \vec x_j| - |\vec \rho|
      \right) \f$</td>
    </tr>
    <tr>
      <td>geometrical phase</td>
      <td>CR::GeometricalPhase</td>
      <td>\f$ \phi (\vec x_j, \vec \rho, \nu) = 2 \pi \nu \tau_{\rm geom} \f$</td>
    </tr>
    <tr>
      <td>geometrical weight</td>
      <td>CR::GeometricalWeight</td>
      <td>\f$ w (\vec x_j, \vec \rho, \nu) = \exp \Bigl( i\, \phi (\vec x_j,
      \vec \rho, \nu) \Bigr) \f$</td>
    </tr>
  </table>

  - \ref crtools_imaging_geometry
  - \ref crtools_imaging_frames
  - \ref crtools_imaging_references
*/

// ==============================================================================
//
//  Module IO
//
// ==============================================================================

/*!
  \defgroup IO CR-Pipeline: IO module
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
  \defgroup LopesLegacy CR-Pipeline: LopesLegacy module
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
  \defgroup CR_Math CR-Pipeline: Math module
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
  \defgroup Observation CR-Pipeline: Observation module
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
  \defgroup CR_Utilities CR-Pipeline: Utilities collection
  \ingroup CR

  \brief Utilities collection for the CR-pipeline
  
  <h3>Synopsis</h3>

*/
