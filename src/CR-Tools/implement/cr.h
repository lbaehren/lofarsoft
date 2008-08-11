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
  
  <h3>Synopsis</h3>
  
  
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
  \defgroup Math CR-Pipeline: Math module
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
//  Module Imaging
//
// ==============================================================================

/*!
  \defgroup CR_Imaging CR-Pipeline: Imaging module
  \ingroup CR

  \brief Imaging module for the CR-pipeline
  
  <h3>Synopsis</h3>

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

  <b>Coordinate systems</b>
  Obviously there is quite a number of coordinate systems and reference frames,
  within which positions and directions can be defined.

  \image html beamforming-geometry.png

  <b>Basic geometry and conventions</b>

  For the beamforming process we adopt the geometry as shown in the figure above,
  from this we derive the following fundamental statements:
  <ol>
    <li>Coordinates are given within a cartesian coordinate system frame 
    tied to the center of the array; its origin is designated by
    \f$\vec O = (0,0,0)\f$.
    <li>\f$ \vec x_i \f$ is the  three-dimensional position vector of the
    i-th array antenna, pointing away from the coordinate system origin.
    <li>\f$ \vec \rho_i \f$ denotes the direction vector from the position of
    the i-th antenna towards the position of the source.
    <li>Baseline vectors originate from the chosen phase center; the baseline
    vector between a pair of antennae <i>i</i> and <i>j</i> as therefore is
    given by
    \f[ \vec B_{ij} = \vec x_{j} - \vec x_{i} \f]
    The coordinates of the default array phase center are \f$\vec O = (0,0,0)\f$,
    hence \f$\vec B_j = \vec x_j\f$.
    <li>The light travel time delay, \f$\tau_{ij}\f$, between two antennae
    <i>i</i> and <i>j</i> is defined following the convention for the baselines,
    \f[ \tau_{ij} = \frac{1}{c}\, \Bigl( |\vec \rho_j| - |\vec \rho_i| \Bigr) \f]
    thus \f$\tau_{ij} > 0\f$ if the signal arrives at antenna $i$ before arriving
    at antenna <i>j</i>.
  </ol>
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
