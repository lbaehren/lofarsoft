/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
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

#include <IO/genTVSpec.h>

#define PI 3.1415926536

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  genTVSpec::genTVSpec (){
    CTRead = NULL;
    // set default frequency axis; default blocksize: 131072
    frequency_p.resize(65537);
    for (int i=0; i<65537; i++){
      frequency_p(i) = i*80e6/65536;
    };
  };
    
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  genTVSpec::~genTVSpec ()
  {
    destroy();
  };
  
  void genTVSpec::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void genTVSpec::summary (std::ostream &os)
  {;}
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Matrix<DComplex> genTVSpec::GetTVSpectrum(uInt date, Double Amplitude){
    Matrix<DComplex> output;
    try {
      int i,chan,nAnts,nFreqs,nSignals;
      Matrix<Double> tmpmat;
      Vector<Double> tmpvec;
      Vector<Int> SigPos;
 
      nAnts = AntIDs_p.nelements();
      nFreqs = frequency_p.nelements();
      if ( CTRead == NULL ){
	cerr << "genTVSpec::GetTVSpectrum: " << "CalTableReader not set!" << endl;
	return Matrix<DComplex>();
      };
      if ( nAnts==0 || nFreqs==0) {
	cerr << "genTVSpec::GetTVSpectrum: " << "Antennas or frequencies not specified!" << endl;
	return Matrix<DComplex>();
      };
      output.resize(nFreqs,nAnts);
      output = 0.;
      CTRead->GetData(date, AntIDs_p(0), "PhaseRefFreqs", &tmpmat);
      nSignals = tmpmat.nrow();
      SigPos.resize(nSignals);
      for (i=0; i<nSignals; i++){
	SigPos(i) = ntrue( frequency_p< (mean(tmpmat.row(i))) );
      };
      for (chan=0; chan<nAnts; chan++) {
	CTRead->GetData(date,AntIDs_p(chan) , "PhaseRefPhases", &tmpvec);
	for (i=0; i<nSignals; i++){
	  output(SigPos(i),chan) = polar(Amplitude,(tmpvec(i)/180.*PI));
	};
      };
    } catch (AipsError x) {
      cerr << "genTVSpec::GetTVSpectrum: " << x.getMesg() << endl;
      return Matrix<DComplex>();
    }; 
    return output; 
  };


  

} // Namespace CR -- end
