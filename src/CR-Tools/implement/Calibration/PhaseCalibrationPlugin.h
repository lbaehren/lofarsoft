/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                 *
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

/* $Id: PhaseCalibrationPlugin.h,v 1.8 2007/03/26 13:15:30 bahren Exp $*/

#ifndef PHASECALIBRATIONPLUGIN_H
#define PHASECALIBRATIONPLUGIN_H

#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/BasicSL/Complex.h>
#include <casa/Exceptions/Error.h>
#include <casa/Containers/Record.h>
#include <casa/Containers/RecordInterface.h>

#include <Calibration/PluginBase.h>
#include <Calibration/PluginBase.h>
#include <Calibration/PhaseCalibration.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class PhaseCalibrationPlugin
    
    \ingroup Calibration
    
    \brief Brief description for class PhaseCalibrationPlugin
    
    \author Kalpana Singh
    
    \date 2006/11/02
    
    \test tPhaseCalibrationPlugin.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
    <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    This class is a wrapper for class PhaseCalibration developed for Phase Calibration with 
    respect to the TV transmitter.
    
    <h3>Parameter</h3>
    
    \param spectra		--	Two- dimensional complex array of FFT (gaincurve) in which number 
                                        of rows indicates the number of frequency channels however, number
					of columns indicate the number of antennas to be scanned.
    
    \param frequencyRanges	--	Two dimensional matrix of double  which provides the minimum and maximum
                                        value of frequency ranges to define frequency ranges in which power peaks of 
					the signal(gaincurve) lies (we set these frequency ranges greater than 
					three, i.e., picture and audio subcarriers of TV transmission), number of rows
					define the number of peaks, and number of columns are always 2 corresponding to 
					the minimum and maximum value of the frequency range.

    \param expectedPhases	--	Expected Phase delays for various antennas due to the fact that different
    					antennas are pointing towards different directions and due to the time delay 
					of electronic circuit as well.
   
   \param frequencyValues	--	Vector of double that gives the exact frequency values correspoding to 
                                        the frequency indices (or row index) of the given data array (i.e., spectra
					or FFT Pattern) of complex values.
	
    \param sampleJumps	   	--	Vector of integers with multi-sample jumps to correct the time delays
                                        because of the electronic circuits.
    
    \param referenceAntenna	--	An integer which signify that phase calibration is done with respect to 
                                        this particular antenna, this integer number must be less than the number of 
					columns in the given array of FFT (spectra), default value is 0, i.e., first 
					antenna of the selected group.  
    
    \param sampleRate		--	Rate with which sampling of the data is performed, default value is 80 MHz.
    
    \param badnessWeight	--	Parameter to calculate and define criteria for the flagness of antenna,
                                        default value is 0.5 .

    \param badnessThreshold	--	Maximum acceptable "badness" before antenna is flagged. (default 0.15 sample times)
 
    
    <h3>Example(s)</h3>
    

  */  
  class PhaseCalibrationPlugin : public PluginBase<DComplex> 
  {
    
    Vector<Bool> antennaReturn_p;
    
    PhaseCalibration phcl_p ;
    
     
    public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    PhaseCalibrationPlugin ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another PhaseCalibrationPlugin object from which to create
                      this new one.
    */
    PhaseCalibrationPlugin (PhaseCalibrationPlugin const &other);
    
       
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~PhaseCalibrationPlugin ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another PhaseCalibrationPlugin object from which to make a copy.
    */
    PhaseCalibrationPlugin& operator= (PhaseCalibrationPlugin const &other); 
    
    // --------------------------------------------------------------- Parameters
    
     Bool setFrequencyRanges( Matrix<Double> NewFrequencyRanges )   ; 
  
    Bool setExpectedPhases( Matrix<Double> NewExpectedPhases ) ;
    
    Bool setFrequencyValues( Vector<Double> NewFrequencyvalues ) ;
    
    Bool setSampleJumps( Vector<Double> NewSampleJumps ) ; 
    
    Bool setReferenceAntenna( Int NewReferenceAntenna ) ;
    
    Bool setSampleRate( Double NewSampleRate ) ;
    
    Bool setBadnessWeight( Double NewBadnessWeight ) ;

    /*!
      \brief Return the weights to be multiplied to the spectra

      \retval

      \return 
    */
    Bool calcWeights( Matrix<DComplex>const &spectra ) ;
  
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, PhaseCalibrationPlugin.
    */
    
    std::string className () const {
      return "PhaseCalibrationPlugin";
    }
    
    // ------------------------------------------------------------------ Methods
    
   			      
			          
  private:
    
    
      
    /*!
      \brief Unconditional copying
    */
    void copy (PhaseCalibrationPlugin const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* PHASECALIBRATIONPLUGIN_H */
  
