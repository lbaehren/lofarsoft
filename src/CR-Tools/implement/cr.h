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
    <li><a href="#Synopsis">Synopsis</a>
    <li><a href="#Optional build parameters">Optional build parameters</a>
  </ul>
  
  <h3><a name="Synopsis">Synopsis</a></h3>

  Actively developed and supported successor to the older LOPES-Tools package.

  <h3><a name="Optional build parameters">Optional build parameters</a></h3>
  
  The behavior during configuration and the components included in the build
  can be controlled from a number of optional parameters:

  \verbatim
  option (CR_BUILD_TESTS          "Build the test programs?"                   YES )
  option (CR_BUILD_EXTERNAL_TESTS "Build test programs for external packages?" YES )
  option (CR_COMPILER_WARNINGS    "Enable standard set of compiler warnings?"  YES )
  option (CR_DEBUGGING_MESSAGES   "Print debugging information?"               NO  )
  option (CR_VERBOSE_CONFIGURE    "Verbose output during configuration?"       NO  )
  option (CR_WITH_GLISH           "Support for Glish applications?"            NO  )
  option (CR_WITH_PYTHON          "Support for Python applications?"           NO  )
  option (CR_WITH_PLOTTING        "Support for generation of plots?"           YES )
  option (CR_WITH_GUI             "Build the graphical interface?"             NO  )
  option (CR_WITH_STARTOOLS       "Enable using routines from LOPES-Star?"     YES )
  \endverbatim
  
  <ul>
    <li>\b CR_BUILD_TESTS <br>
    Build the test programs? If enabled all the test programs in
    ''implement/<Module>/test'' will be build.
    <li>\b CR_BUILD_EXTERNAL_TESTS <br> 
    Build the test programs to test working with the various external packages
    and libraries? If enabled the test programs in ''test'' will be build:
    <ul>
      <li>\e tcasa_casa -- Test working with the classes in the ''casa''
      module of casacore.
      <li>\e tcasa_tables -- Test working with the classes in the ''tables''
      module of casacore.
      <li>\e tcasa_coordinates -- Test working with the classes in the
      ''coordinates'' module of casacore.
      <li>etc.
    </ul>
  </ul>
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

  <ul>
    <li><a href="#Synopsis">Synopsis</a>
    <li><a href="#Reference frames">Reference frames</a>
    <li><a href="#References">References</a>
  </ul>
  
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
    thus \f$\tau_{ij} > 0\f$ if the signal arrives at antenna <i>i</i> before
    arriving at antenna <i>j</i>.
  </ol>

  <h3><a name="Reference frames">Reference frames</a></h3>

  A Terrestrial Reference System (TRS) is a spatial reference system co-rotating
  with the Earth in its diurnal motion in space. In such a system, positions of
  points anchored on the Earth solid surface have coordinates which undergo only
  small variations with time, due to geophysical effects (tectonic or tidal
  deformations). A Terrestrial Reference Frame (TRF) is a set of physical points
  with precisely determined coordinates in a specific coordinate system (cartesian,
  geographic, mapping...) attached to a Terrestrial Reference System. Such a TRF
  is said to be a realization of the TRS.

  <ol>
    <li><i>Ideal Terrestrial Reference Systems.</i> <br>
    An ideal Terrestrial Reference System (TRS) is defined as a tri-dimensional
    reference frame (in the mathematical sense) close to the Earth and co-rotating
    with it. In the newtonian background, the geometry of the physical space
    considered as an euclidian affine space of dimension 3 provides a standard
    and rigorous model of such a system through the selection of an affine frame
    (O,E). O is a point of the space named origin. E is a vector base of the
    associated vector space.The currently adopted restrictions to E are to be
    orthogonal with same length for the base vectors. Moreover, one adopts a direct
    orientation. The common length of these vectors will express the scale of the
    TRS and the set of unit vectors collinear to the base its orientation:
    \f[ \lambda = || \vec{E}_i ||_{i=1,2,3} \f]
    In the context of IERS, we consider the geocentric systems where the origin
    is close to the geocenter and the orientation is equatorial (Z axis is the
    direction of the pole). In this case, cartesian coordinates, geographical
    coordinates or plane (map) coordinates are currently used.<br>
    Under these hypothesis, the general transformation of the cartesian
    coordinates of any point close to the Earth from a TRS (1) to a TRS (2) will
    be given by a tri-dimensional similarity (\f$ T_{1,2} \f$ is a translation
    vector, \f$ \lambda_{1,2} \f$ a scale factor and \f$ R_{1,2} \f$ a rotation
    matrix):
    \f[ X^{(2)} = T_{1,2} + \lambda_{1,2} \cdot R_{1,2} X^{(1)} \f]
    <li><i>Conventional Terrestrial Reference System (CTRS).</i><br>
    Such a system designates the set of all conventions, algorithms and constants
    which determine the estimation of coordinates of points in a specific ideal TRS.
    <li><i>Conventional Terrestrial Reference Frame (CTRF).</i><br>
    A Conventional Terrestrial Reference Frame is defined as a set of physical
    points with precisely determined coordinates in a specific coordinate system
    as a realization of an ideal Terrestrial Reference System. Two types of
    frames are currently distinguished (this is also valid for celestial
    references), namely dynamical or kinematical ones, depending whether a
    dynamical model is applied in the determination process of these coordinates,
    or not.
  </ol>

  The standard relation of transformation between two reference systems is an
  Euclidian similarity of seven parameters: three translation components, one
  scale factor, and three rotation angles, designated respectively, T1, T2, T3,
  D, R1, R2, R3, and their first times derivations : \f$ \dot{T1} \f$, 
  \f$ \dot{T2} \f$, \f$ \dot{T3} \f$, \f$ \dot{D} \f$, \f$ \dot{R1} \f$,
  \f$ \dot{R2} \f$, \f$ \dot{R3} \f$. The transformation of coordinate vector
  \f$ X_1 \f$, expressed in a reference system (1), into a coordinate vector
  \f$ X_2 \f$, expressed in a reference system (2), is given by the following
  equation:
  \f[ X_2 = X_1 + T + D X_1 + R X_1 \f]
  with :
  \f[
  T = \left( \begin{array}{c} T1 \\ T2 \\ T3 \end{array} \right)
  \quad \hbox{and} \qquad
  R = \left( \begin{array}{ccc} 0 & -R3 & R2 \\ R3 & 0 & -R1 \\ -R2 & R1 & 0
  \end{array} \right)
  \f]
  It is assumed that first equation is linear for sets of station coordinates
  provided by space geodetic technique (origin difference is about a few hundred
  meters, and differences in scale and orientation are of \f$ 10^{-5} \f$ level).
  Generally, X1, X2, T, D, R are function of time.

  <h3><a name="References">References</a></h3>
  
  <ul>
    <li>casacore <a href="http://www.astron.nl/casacore/doc/html/classcasa_1_1Coordinate.html">Coordinate</a> class
    <li><a href="http://itrf.ensg.ign.fr">itrf.ensg.ign.fr</a>
    <li><a href="http://www.iers.org">International Earth Rotation and Reference
    System Service</a>
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
