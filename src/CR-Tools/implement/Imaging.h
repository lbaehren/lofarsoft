/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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
  \file Imaging.h

  \defgroup CR_Imaging CR-Pipeline: Imaging module
  \ingroup CR

  \brief Imaging module for the CR-pipeline
  
  \author Lars B&auml;hren
  
  \date 2007/01/16

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

  <h3>Usage</h3>

  Use this file in order to import all the prototypes for functions and classes
  in the <i>Imaging</i> module.
*/

#include <Imaging/Antenna.h>
#include <Imaging/AntennaGain.h>
#include <Imaging/Beam.h>
#include <Imaging/Beamformer.h>
#include <Imaging/Beamshape.h>
#include <Imaging/CoordinateConversion.h>
#include <Imaging/Feed.h>
#include <Imaging/GeometricalDelay.h>
#include <Imaging/GeometricalPhase.h>
#include <Imaging/GeometricalWeight.h>
#include <Imaging/HorizonScan.h>
#include <Imaging/SkymapCoordinates.h>
#include <Imaging/SkymapGrid.h>
#include <Imaging/Skymapper.h>
#include <Imaging/ccBeam.h>
#include <Imaging/xBeam.h>
