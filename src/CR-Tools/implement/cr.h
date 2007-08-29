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

/*!
  \defgroup CR Cosmic Rays (CR) analysis pipeline

  <h3>Synopsis</h3>

  

  <h3>Pending issues</h3>

  <ol>
    <li>In order to be able to contain dependency on Glish, all classes in the
    CR library depending on Glish should be confined within the
    <i>ApplicationSupport</i> module; by this measure setting the
    <tt>CR_GLISH_SUPPORT</tt> parameter to <i>false</i> enables building the
    core functionality using the <i>casacore</i> libraries only.
    \verbatim
    Analysis/CRinvFFT.h:#include <ApplicationSupport/Beamformer.h>
    \endverbatim
    \code
      // Container for the phase gradients
      Matrix<DComplex> phaseGradients;
      /* ---- old version (needs to go) ---- */
      // calculate the phase gradients
      if (verbose) { bf_p.showProgress(True); };
      bf_p.setPhaseGradients(dr->frequencyValues(), tmpvec, tmpAntPos);
      if (verbose) { bf_p.showProgress(False); };
      // retrieve the phase gradients
      phaseGradients = bf_p.phaseGradients().nonDegenerate();
      /* ---- new version ---- */
      try {
	CR::GeometricalPhase geomPhase (tmpAntPos,tmpvec,dr->frequencyValues());
	phaseGradients.phases().nonDegenerate();
      } catch (std::string message) {
	std::cerr << "[CRinvFFT::GetShiftedFFT]"
		  << " Error while retrieving phase gradients!"
		  << message
		  << std::endl;
      }
    \endcode
  </ol>
*/
