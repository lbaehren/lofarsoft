/***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

/* $Id$*/

#ifndef TVCALIBRATIONPLUGIN_H
#define TVCALIBRATIONPLUGIN_H

// Standard library header files
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <crtools.h>
#include <Calibration/DelayCorrectionPlugin.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class TVCalibrationPlugin
    
    \ingroup Calibration
    
    \brief Do the "LOPES-style" phase calibration on a TV transmitter. 
    
    \author Andreas Horneffer

    \date 2007/04/16

    \test tTVCalibrationPlugin.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>PluginBase<DComplex> 
      <li>DelayCorrectionPlugin
    </ul>
    
    <h3>Synopsis</h3>

    This class implements the phase calibration on narrow band (RFI) transmitters
    as it was originally implemented into the Glish-based lopestools.

    <h3>Parameter</h3>
    
    The following is a list of entries in the parameters record that are used by this class.
    those without a default value have to be set before calcDelays() is called!
    
    \param frequencyRanges   --  List of the frequency ranges in which to look for the transmitter 
                                 peaks. Two dimensional matrix of double with the minimum and 
                                 maximum frequencies in each row. The number of rows defines the 
                                 number of peaks, and the number of columns has to be 2.
                                 If the begin and the end of the frequency range is exactly the same,
                                 the nearest frequency bin which is nearest to this frequency will
                                 be used.

    \param referencePhases   --  Reference phase values for comparison to the measured phase differences.
                                 The phase differences are the phase (at a peaks frequency) of the antenna
				 minus the phase of the reference antenna.
   
    \param frequencyValues   --  The frequency axis of the input data. Vector of double that gives the 
                                 exact frequency values correspoding tothe frequency indices. 
	
    \param sampleJumps       --  Vector of integers with multi-sample jumps to try out.
    
    \param referenceAntenna  --  Number of the reference antenna. Default value is 0, i.e. the first 
				 antenna of the selected group.  
    
    \param sampleRate        --  Rate with which sampling of the data is performed, default value is 80 MHz.
    
    \param badnessWeight     --  Parameter to calculate and define criteria for the "badness" of an antenna.
                                 Default value is 0.5.

    \param badnessThreshold  --  Maximum acceptable "badness" before antenna is flagged, in sample times. 
                                 Default value is 0.15 sample times.
    \param SampleShiftsOnly  --  Corrections are done only for sample shifts:
                                 If the sample unit is e.g. 12.5 ns, phase differences corresponding to 
                                 delays of < 6.25 ns will have no effect. Default is False.

    The following entries in the parameters record are set by this class:

    \return delays       --  Vector with the delays in seconds (e.g. as used by the 
                             DelayCorrectionPlugin class).
    \return AntennaMask  --  Vector of Bool that lists the antennas for which the algorithm failed.
                             True for good antennas, False for antennas that failed.

     \Note
     <b> Currently (April 2007) the reference values, that are stored in the CalTabels, 
     are defined in such a way that the values from "fft()" (and not "calfft()") of
     the DataReader have to be used. \b>

    <h3>Example</h3>
    \code
    DataReader *dr = ...;
    TVCalibrationPlugin pCal_p;
    Matrx<DComplex> data;

    pCal_p.parameters().define("frequencyValues",dr->frequencyValues());
    ...

    pCal_p.calcDelays(dr->fft());
    data = dr->calfft();
    pCal_p.apply(data);
    \endcode
        
  */  
  class TVCalibrationPlugin : public DelayCorrectionPlugin {
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    TVCalibrationPlugin ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~TVCalibrationPlugin ();
    
    // ---------------------------------------------------------------- Operators
        
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, TVCalibrationPlugin.
    */
    virtual std::string className () const {
      return "TVCalibrationPlugin";
    }

    // ------------------------------------------------------------------ Methods
    

    /*!
      \brief Calculate the delays from the data

      This function essentially does all tha magic. It reads out all the parameters 
      from the parameters record, so they have to be set correctly before. Then it
      calculates the corrections delays and sets them in the parameters record (from
      which the <tt>DelayCorrectionPlugin</tt> calculates the phase gradients.
      It also sets the entry in the parameters record "AntennaMask" which marks all 
      the channels for which the routine failed.

      \param data           -- Data for which the calibration is to be done. 
                               (Typically the <tt>fft()</tt> values from the DataReader.)

      \return ok            -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    virtual Bool calcDelays(const Matrix<DComplex> &data);    

    
    
  }; //Class TVCalibrationPlugin  -- end
} // Namespace CR -- end

#endif /* TVCALIBRATIONPLUGIN_H */
  
