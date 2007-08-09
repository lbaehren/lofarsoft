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

/* $Id: PhaseCalibrationPlugin.cc,v 1.9 2007/03/26 14:23:29 singh Exp $*/

#include <Calibration/PhaseCalibrationPlugin.h>

#define pi 3.1415926536
/*!
  \class PhaseCalibrationPlugin
*/

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  PhaseCalibrationPlugin::PhaseCalibrationPlugin ()
                        : PluginBase<DComplex> ()
  
  { 
    //Array<DComplex> tmpArrayDComplex;
  Array<Double> tmpArrayDouble;
  Vector<Double> tmpVectorDouble ;
  
  Int tmpreferenceAntenna = 0 ;
  Double tmpsampleRate = 80e6 ;
  Double tmpbadnessWeight = 0.5 ;
  Double tmpbadnessThreshold = 0.15 ;
  
  Vector<Bool> tmpantennaReturn ;
    
  parameters_p.define("frequencyRanges", tmpArrayDouble);
  parameters_p.define("expectedPhases", tmpArrayDouble);
  parameters_p.define("phaseGradient", tmpArrayDouble);
  parameters_p.define("frequencyValues", tmpVectorDouble);
  parameters_p.define("sampleJumps", tmpVectorDouble );
  parameters_p.define("referenceAntenna", tmpreferenceAntenna );
  parameters_p.define("sampleRate", tmpsampleRate );
  parameters_p.define("badnessWeight", tmpbadnessWeight );
  parameters_p.define("badnessThreshold", tmpbadnessThreshold );
  parameters_p.define("antennaReturn", tmpantennaReturn );
  }
  
  PhaseCalibrationPlugin::PhaseCalibrationPlugin (PhaseCalibrationPlugin const &other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  PhaseCalibrationPlugin::~PhaseCalibrationPlugin ()
  {
    destroy();
  }
  
  void PhaseCalibrationPlugin::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  PhaseCalibrationPlugin& PhaseCalibrationPlugin::operator= (PhaseCalibrationPlugin const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  void PhaseCalibrationPlugin::copy (PhaseCalibrationPlugin const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
 
  
 Bool PhaseCalibrationPlugin::setFrequencyRanges( Matrix<Double> NewFrequencyRanges ) 
  {
  	try{
	parameters_p.define( "frequencyRanges", NewFrequencyRanges ) ;
	}
	catch( AipsError x ){
	cerr<< "PhaseCalibrationPlugin::setFrequencyRanges " << x.getMesg () << endl ;
	return False ;
	}
	return True;
  }
  
  
  Bool PhaseCalibrationPlugin::setExpectedPhases( Matrix<Double> NewExpectedPhases ) 
  {
  	try{
	parameters_p.define( "expectedPhases", NewExpectedPhases ) ;
	}
	catch( AipsError x ){
	cerr<< "PhaseCalibrationPlugin::setExpectedPhases " << x.getMesg () << endl ;
	return False ;
	}
	return True;
  }
  
   Bool PhaseCalibrationPlugin::setFrequencyValues( Vector<Double> NewFrequencyValues ) 
  {
  	try{
	parameters_p.define( "frequencyValues", NewFrequencyValues ) ;
	}
	catch( AipsError x ){
	cerr<< "PhaseCalibrationPlugin::setFrequencyValues " << x.getMesg () << endl ;
	return False ;   
	}
	return True;
  }
  
   Bool PhaseCalibrationPlugin::setSampleJumps( Vector<Double> NewSampleJumps ) 
  {
  	try{
	parameters_p.define( "sampleJumps", NewSampleJumps ) ;
	}
	catch( AipsError x ){
	cerr<< "PhaseCalibrationPlugin::setSampleJumps " << x.getMesg () << endl ;
	return False ;   
	}
	return True;
  }
  
   Bool PhaseCalibrationPlugin::setReferenceAntenna( Int NewReferenceAntenna ) 
  {
  	try{
	parameters_p.define( "referenceAntenna", NewReferenceAntenna ) ;
	}
	catch( AipsError x ){
	cerr<< "PhaseCalibrationPlugin::setReferenceAntenna " << x.getMesg () << endl ;
	return False ;   
	}
	return True;
  }
  
   Bool PhaseCalibrationPlugin::setSampleRate( Double NewSampleRate ) 
  {
  	try{
	parameters_p.define( "sampleRate", NewSampleRate ) ;
	}
	catch( AipsError x ){
	cerr<< "PhaseCalibrationPlugin::setSampleRate " << x.getMesg () << endl ;
	return False ;  
	}
	return True;
  }
  
  Bool PhaseCalibrationPlugin::setBadnessWeight( Double NewBadnessWeight ) 
  {
  try{
	parameters_p.define( "badnessWeight", NewBadnessWeight ) ;
	}
	catch( AipsError x ){
	cerr<< "PhaseCalibrationPlugin::setBadnessWeight " << x.getMesg () << endl ;
	return False ;   
	}
	return True;	
  }
      
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================


  Bool PhaseCalibrationPlugin:: calcWeights( Matrix<DComplex>const &spectra ) 
  {
    
    try{
      Vector<Double> CorrDelays;
      Matrix<Double> frequencyRanges = parameters_p.asArrayDouble("frequencyRanges");
      Matrix<Double> expectedPhases  = parameters_p.asArrayDouble("expectedPhases");
      Matrix<Double> phaseGrad       = parameters_p.asArrayDouble("phaseGradient");
      Matrix<Double> frequencyValues = parameters_p.asArrayDouble("frequencyValues");
      Vector<Double> sampleJumps     = parameters_p.asArrayDouble("sampleJumps"); 

      //Vector<Bool> antennaReturn_p;
      antennaReturn_p = phcl_p.getAntennaReturn (spectra,
						 frequencyRanges,
						 expectedPhases,
						 phaseGrad,
						 frequencyValues,
						 sampleJumps,
						 parameters_p.asInt("referenceAntenna"),
						 parameters_p.asDouble("sampleRate"),
						 parameters_p.asDouble("badnessWeight"), 
						 parameters_p.asDouble("badnessThreshold"), 
						 &CorrDelays );
      
      parameters_p.define("antennaReturn", antennaReturn_p) ;

      cout << "PhaseCalibrationPlugin::calcWeights: Calculated CorrDelays: " << CorrDelays << endl;
      DComplex tmpcomp(0.,1.);
      uInt i;
      Vector<DComplex> cFreqVals;
      cFreqVals =  parameters_p.toArrayDComplex("frequencyValues");
      weights_p.resize(spectra.shape());
      tmpcomp *= 2*pi;
      for (i=0; i<spectra.ncolumn(); i++){
	weights_p.column(i) = exp(tmpcomp*CorrDelays(i)*cFreqVals);
      };
    }
    catch( AipsError x ){
    cerr<< "PhaseCalibrationPlugin::calcWeights "<< x.getMesg() << endl ;
    return False;
    }
    return True;
  }  
 
 } // Namespace CR -- end
