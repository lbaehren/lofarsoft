/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2010                                                  *
 *   maaijke mevius (<mail>)                                                     *
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

#include <Calibration/RFIMitigationPlugin.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  RFIMitigationPlugin::RFIMitigationPlugin ()  : PluginBase<DComplex> ()
  {  

    Int tmpNOfsegments = 25 ;
    Double tmprmsThresholdValue=1.e20;
    Double tmpflagThresholdValue=5;
    Int tmpMethod=RFIMitigation::linear;
    parameters_p.define("nofSamples",tmpNOfsegments ) ; 
    parameters_p.define("rmsThresholdValue",tmprmsThresholdValue ) ; 
    parameters_p.define("flagThresholdValue",tmpflagThresholdValue ) ; 
    parameters_p.define("method",tmpMethod ) ; 
  }

  
  RFIMitigationPlugin::RFIMitigationPlugin (RFIMitigationPlugin const &other) : PluginBase<DComplex> ()
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  RFIMitigationPlugin::~RFIMitigationPlugin ()
  {
    destroy();
  }
  
  void RFIMitigationPlugin::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                    operator=
  
  RFIMitigationPlugin& RFIMitigationPlugin::operator= (RFIMitigationPlugin const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  //_____________________________________________________________________________
  //                                                                         copy
  
  void RFIMitigationPlugin::copy (RFIMitigationPlugin const &other)
  {;}

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  //_____________________________________________________________________________
  //                                                                      summary
  
  void RFIMitigationPlugin::summary (std::ostream &os)
  {
    os << "[RFIMitigationPlugin] Summary of internal parameters." << std::endl;
  }

  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool RFIMitigationPlugin :: setNOfSegments( Int NewNOfSegments )
  {
    
    try {
      parameters_p.define("nofSamples",NewNOfSegments ) ;
    }
    catch( AipsError x){
      cerr << "RFIMitigationPlugin:: setNOfSegments" << x.getMesg () << endl ;
      return False ;
    }
    return True ;
    
  }

  Bool RFIMitigationPlugin :: setMethod( Int NewMethod )
  {
    
    try {
      parameters_p.define("method",NewMethod ) ;
    }
    catch( AipsError x){
      cerr << "RFIMitigationPlugin:: setNOfSegments" << x.getMesg () << endl ;
      return False ;
    }
    return True ;
    
  }

  Bool RFIMitigationPlugin :: setRmsThreshold( Double NewRmsThreshold )
  {
    
    try {
      parameters_p.define("rmsThresholdValue",NewRmsThreshold ) ;
    }
    catch( AipsError x){
      cerr << "RFIMitigationPlugin:: setrmsThreshold" << x.getMesg () << endl ;
      return False ;
    }
    return True ;
    
  }

  Bool RFIMitigationPlugin :: setFlagThreshold( Double NewFlagThreshold )
  {
    
    try {
      parameters_p.define("flagThresholdValue",NewFlagThreshold ) ;
    }
    catch( AipsError x){
      cerr << "RFIMitigationPlugin:: setflagThreshold" << x.getMesg () << endl ;
      return False ;
    }
    return True ;
    
  }

 Bool RFIMitigationPlugin::apply (Matrix<DComplex>& data,
				   const Bool& computeWeights){
    try{
      uint nr_chan = data.nrow();
      uint nr_ant  = data.ncolumn();
      for(uint iant=0;iant<nr_ant;iant++){
	Vector<Double> indat =  amplitude(data.column(iant));
	Vector<Double> phasedat =  phase(data.column(iant));
	Vector<Double> outdat(nr_chan);
	RFIMitigation::doRFIMitigation(indat,
				       parameters_p.asInt("nofSamples"),
				       parameters_p.asDouble("rmsThresholdValue"),
				       parameters_p.asDouble("flagThresholdValue"),
				       outdat,
				       parameters_p.asInt("method"));
	//just copy the data
	for(uint ichan=0;ichan<nr_chan;ichan++) 
	  data(ichan,iant) = DComplex(outdat(ichan)*cos(phasedat(ichan)),outdat(ichan)*sin(phasedat(ichan)));
						      
      }
    }
    catch(AipsError x){
      cerr << "RFIMitigation :: calcWeights" << x.getMesg() << endl ;
      return False ;
    }
    return True ;
  
  }

 
  
  Bool RFIMitigationPlugin::apply (Matrix<DComplex> &outputData,
				   const Matrix<DComplex>& inputData,
				   const Bool& computeWeights){
    try{
      uint nr_chan = inputData.nrow();
      uint nr_ant  = inputData.ncolumn();
      outputData.resize(nr_chan,nr_ant);

      for(uint iant=0;iant<nr_ant;iant++){
	Vector<Double> indat =  amplitude(inputData.column(iant));
	Vector<Double> phasedat =  phase(inputData.column(iant));
	Vector<Double> outdat(nr_chan);
	RFIMitigation::doRFIMitigation(indat,
				       parameters_p.asInt("nofSamples"),
				       parameters_p.asDouble("rmsThresholdValue"),
				       parameters_p.asDouble("flagThresholdValue"),
				       outdat,
				       parameters_p.asInt("method"));
	//just copy the data
	for(uint ichan=0;ichan<nr_chan;ichan++) 
	  outputData(ichan,iant) = DComplex(outdat(ichan)*cos(phasedat(ichan)),outdat(ichan)*sin(phasedat(ichan)));
	  
      }
    }
    catch(AipsError x){
      cerr << "RFIMitigation :: calcWeights" << x.getMesg() << endl ;
      return False ;
    }
    return True ;
  
  

  }
  

} // Namespace CR -- end
