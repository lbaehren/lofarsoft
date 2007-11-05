/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
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

#include <iostream>

#include <casa/aips.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Inputs/Input.h>
#include <casa/OS/Time.h>
#include <casa/Quanta/MVTime.h>
#include <casa/Quanta/MVPosition.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <measures/Measures.h>
#include <measures/Measures/MDirection.h>
#include <measures/Measures/MEpoch.h>
#include <measures/Measures/MPosition.h>
#include <measures/Measures/MeasConvert.h>
#include <measures/Measures/MeasFrame.h>

#include <Observation/ObservationData.h>
#include <Utilities/MConversions.h>

using std::cout;
using std::endl;

using casa::MDirection;
using casa::MPosition;
using casa::Quantity;

/*!
  \file calcSourcePosition.cc
  
  \ingroup CR_Applications
  
  \brief Compute the position of a source for a certain epoch and observatory
  
  \author Lars B&auml;hren
  
  \date 2007/11/02

  \todo Provide mean to pass observatory position (in a given geographic
        reference system), in case it is not listed in the measures data
	tables.

  <h3>Motivation</h3>

  We need some simple to use means to obtain the position of a celestial source
  (e.g. the Moon) at a given epoch and location within chosen coordinate
  reference frame (e.g. within a local Azimuth-Elevation system, in order to
  determine whether or not the Moon is actually visible within an acceptable area
  on the sky).

  <h3>Prerequisites</h3>

  <ul>
    <li>Some basic understanding of how to use casa::Measures.
  </ul>

  <h3>Example</h3>

  \verbatim
  calMoonPosition telescope=WSRT starttime=19:00:00 timesteps=20
  \endverbatim

*/

// ------------------------------------------------------------------------------

/*!
  \brief Set up the casa::Input object handling the input parameters

  \return inputs -- casa::Input object used to parse and process the command line
          parameters given to this program.
*/
casa::Input set_InputParameters ()
{
  casa::Input inputs(1);

  // general oservation setting

  inputs.create("epoch",
		"0.0",
		"Epoch of the observation (modified Julian day)?",
		"Double");
  inputs.create("telescope",
		"WSRT",
		"Name of the telescope?",
		"String");
  inputs.create("observer",
		"LOFAR-CR",
		"Name of the observer?",
		"String");

  // scan parameters

  inputs.create("source",
		"MOON",
		"Target source of the observation?",
		"String");
  inputs.create("sourcePos",
		"0,90",
		"Position of the observation target?",
		"Block<Double>");
  inputs.create("sourceRef",
		"AZEL",
		"Reference code for the position of the observation target?",
		"String");
  inputs.create("starttime",
		"20:00:00",
		"Start time of the observation?",
		"String");
  inputs.create("timesteps",
		"1",
		"Number of time steps for which to compute?",
		"Int");
  inputs.create("refsys",
		"AZEL",
		"Reference frame in which to compute the positions?",
		"String");

  return inputs;
}

// ------------------------------------------------------------------------------

/*!
  \brief Present a summary of the input parameters

  \param inputs -- casa::Input object containing the list and values of the
         supported input parameters
*/
void show_Input (casa::Input &inputs)
{
  cout << "[calcSourcePosition] Summary of parameters" << endl;
  
  cout << "Epoch of the observation ........................ : "
       << inputs.getDouble("epoch") << endl;
  cout << "Name of the telescope used for the observation .. : "
       << inputs.getString("telescope") << endl;

  cout << "Target source of the observation ................ : "
       << inputs.getString("source") << endl;
  cout << "Position of the observation target .............. : "
       << inputs.getString("sourcePos") << endl;
  cout << "Reference code for the target position .......... : "
       << inputs.getString("sourceRef") << endl;
  cout << "Start time of the observation ................... : "
       << inputs.getString("starttime") << endl;
  cout << "Number of time steps for which to compute ....... : " 
       << inputs.getInt("timesteps") << endl;
  cout << "Reference frame in which to compute the positions : "
       << inputs.getString("refsys") << endl;
}

// ------------------------------------------------------------------------------

/*!
  \brief Set ObsInfo object from set of parameters
  
  \param obsEpoch     -- Epoch of the observation, given as modified Julian
         day; if no no-zero value is provided we use the present time, i.e.
	 the start-up time of the program.
  \param obsTelescope -- Name of the telescope. Note that the telescope needs
         to be locatable on the mesasures data tables.
  \param observer     -- Name of the observer; though this is no really necessary
         parameter, it makes the later output look a lot nicer as having it 
	 left unspecified.

  \return obsInfo -- casa::ObsInfo object encapsulating the basic information
          on the observation location and epoch
*/
casa::ObsInfo set_ObsInfo (casa::Double const &obsEpoch,
			   casa::String const &obsTelescope,
			   casa::String const &observer)
{
  casa::ObsInfo obsInfo;

  if (obsEpoch == 0.0) {
    casa::Time startTime;
    Quantity epoch (startTime.modifiedJulianDay(), "d");
    obsInfo.setObsDate (epoch);
  } else {
    Quantity epoch (obsEpoch, "d");
    obsInfo.setObsDate (epoch);
  }

  obsInfo.setTelescope (obsTelescope);
  obsInfo.setObserver(observer);
  
  return obsInfo;
}

// ------------------------------------------------------------------------------

/*!
  \brief Test routines to convert Measures from one frame to another

  \return status -- Did the test work out fine? If not <tt>false</tt> is returned
*/
bool test_Convert ()
{
  std::cout << "\n[test_Convert]\n" << std::endl;
  
  bool status (true);
  casa::Vector<double> sourcePosition (2);
  
  // Time of the observation
  casa::Time startTime;
  Quantity epoch (startTime.modifiedJulianDay(), "d");
  casa::MEpoch date (epoch);
  
  // Location of the observatory
  MPosition obs (casa::MVPosition (Quantity(10,"m"),
				   Quantity(-6,"deg"),
				   Quantity(50,"deg")),
		 MPosition::Ref(MPosition::WGS84));
  
  // Set up frame with the observatory position
  casa::MeasFrame frame (obs);
  frame.set(date);
  
  // Direction towards the observation target
  MDirection source (Quantity( 20,"deg"),
		     Quantity(-10,"deg"),
		     MDirection::Ref(MDirection::J2000));

  try {
    // temporary variables
    casa::MVDirection MVDirectionFROM;
    casa::Vector<Quantity> QDirectionTO(2);
    // set up conversion engine (J2000 -> AZEL)
    CR::ObservationData obsData (epoch,"WSRT");
    MDirection::Convert conv = obsData.conversionEngine ("AZEL",
							 "J2000");
    MVDirectionFROM = casa::MVDirection (Quantity( 20,"deg"),
					 Quantity(-10,"deg"));
    QDirectionTO = conv(MVDirectionFROM).getValue().getRecordValue();

    sourcePosition(0) = QDirectionTO(0).getValue("deg");
    sourcePosition(1) = QDirectionTO(1).getValue("deg"); 
  } catch (std::string message) {
    std::cerr << message << std::endl;
    status = false;
  }

  // Summary of the conversion
  std::cout << "-- Observation time     : " << epoch  << std::endl;
  std::cout << "                        : " << date   << std::endl;
  std::cout << "-- Observatory position : " << obs    << std::endl;
  std::cout << "-- Source position      : " << source << std::endl;
  std::cout << "                        : " << sourcePosition << std::endl;

  return status;
}

// ------------------------------------------------------------------------------

/*!
  \brief Main function of calcSourcePosition
  
  \return status -- Exit status of the program, returns non-zero if an error was
          encountered.
*/
int main (int argc,
	  char* argv[])
{
  int retval (0);

  /* Get the command line parameters and show a short summary of the settings */

  casa::Input inputs = set_InputParameters();
  inputs.readArguments (argc,argv);
  show_Input (inputs);

  /* Combine observation epoch and telescope name into an ObsInfo object */

  casa::ObsInfo obsInfo = set_ObsInfo (inputs.getDouble("epoch"),
				       inputs.getString("telescope"),
				       inputs.getString("observer"));
  
  if (!test_Convert()) {
    retval++;
  }

  return retval;
}
