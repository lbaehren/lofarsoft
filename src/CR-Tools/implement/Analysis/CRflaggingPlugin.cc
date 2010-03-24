/*-------------------------------------------------------------------------*
 | $Id$ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

#include <Analysis/CRflaggingPlugin.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  CRflaggingPlugin::CRflaggingPlugin (){
    init();
  }
  
  void CRflaggingPlugin::init(){
    parameters_p.define("DoSpikyFlagging",False);
    parameters_p.define("SpikyFaktor",(Double)10.);
    parameters_p.define("SpikyStart",(Int)0);
    parameters_p.define("SpikyLen",(Int)25000);
    parameters_p.define("DoHighPowerFlagging",True);
    parameters_p.define("HighPowerFaktor",(Double)10.);
    parameters_p.define("DoLowPowerFlagging",True);
    parameters_p.define("LowPowerFaktor",(Double)0.1);
    parameters_p.define("AntennaMask", Vector<Bool>() );
    parameters_p.define("Verbose", True );
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  CRflaggingPlugin::~CRflaggingPlugin ()
  {
    destroy();
  }
  
  void CRflaggingPlugin::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void CRflaggingPlugin::summary (std::ostream &os)
  {
    os << "[CRflaggingPlugin] Summary of object properties" << std::endl;
  }
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool CRflaggingPlugin::calcWeights(const Matrix<Double> &data){
    try {
      int nants = data.ncolumn();
      Vector<Bool> flags(nants,True);
      Vector<Double> means(nants),devs(nants),tmpvec;
      Double cmpval,cmpfakt;
      Int i;
      for (i=0; i<nants; i++) {
	means(i) = mean(data.column(i));
	devs(i) = stddev(data.column(i));
      };
      if (parameters_p.asBool("DoSpikyFlagging")) {
	cmpfakt = parameters_p.asDouble("SpikyFaktor");
	for (i=0; i<nants; i++) {
	  tmpvec = data.column(i);;
	  if (max(abs(tmpvec(Slice(parameters_p.asInt("SpikyStart"),
				       parameters_p.asInt("SpikyLen"))))) >
	      means(i)+cmpfakt*devs(i)) {
	    flags(i) = False;
	    if (parameters_p.asBool("Verbose")){
	      cout << "CRflaggingPlugin:calcWeights: Flagged Antenna #" << i+1 
		   << " due to excess Spikyness." <<endl;
	    };
	  };
	};
      };
      cmpval = median(devs);
      if (parameters_p.asBool("DoHighPowerFlagging")) {
	cmpfakt = parameters_p.asDouble("HighPowerFaktor");
	for (i=0; i<nants; i++) {
	  if (devs(i) > cmpfakt*cmpval) {
	    flags(i) = False;
	    if (parameters_p.asBool("Verbose")){
	      cout << "CRflaggingPlugin:calcWeights: Flagged Antenna #" << i+1 
		   << " due to excess signal." <<endl;
	    };
	  };
	};
      };
      if (parameters_p.asBool("DoLowPowerFlagging")) {
	cmpfakt = parameters_p.asDouble("LowPowerFaktor");
	for (i=0; i<nants; i++) {
	  if (devs(i) < cmpfakt*cmpval) {
	    flags(i) = False;
	    if (parameters_p.asBool("Verbose")){
	      cout << "CRflaggingPlugin:calcWeights: Flagged Antenna #" << i+1 
		   << " due to lack of signal." <<endl;
	    };
	  };
	};
      };
      parameters_p.define("AntennaMask", flags );
      weights_p.resize(data.shape());
      weights_p = 1.;
    } catch (AipsError x) {
      cerr << "CRflaggingPlugin:calcWeights: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  

} // Namespace CR -- end
