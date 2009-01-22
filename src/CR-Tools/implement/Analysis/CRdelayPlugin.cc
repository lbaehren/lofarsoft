/***************************************************************************
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

/* $Id$*/

#include <Analysis/CRdelayPlugin.h>

#define DEG2RAD (PI/180.)

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  CRdelayPlugin::CRdelayPlugin () {
    init();
  }

  void CRdelayPlugin::init(){
    parameters_p.define("Az",(Double)0.);
    parameters_p.define("El",(Double)90.);
    parameters_p.define("frequencyValues",  Vector<Double>());
    parameters_p.define("ReferencePos",Vector<Double>(3,0.));
    parameters_p.define("ExtraDelay",(Double)0.);
    parameters_p.define("TotalDelay",(Double)0.);

    Double kascadeX[16] = {-78.,-26., 26., 78.,-78.,-26., 26., 78.,-78.,-26.,26.,78.,-78.,-26.,26.,78.};
    Double kascadeY[16] = {-78.,-78.,-78.,-78.,-26.,-26.,-26.,-26., 26., 26.,26.,26., 78., 78.,78.,78.};
    Vector<Double> kasX(IPosition(1,16),kascadeX,SHARE),kasY(IPosition(1,16),kascadeY,SHARE);
    Matrix<Double> ClusterPos(16,3);
    ClusterPos.column(0) = kasX* 0.965925826 + kasY*0.258819045;
    ClusterPos.column(1) = kasX*-0.258819045 + kasY*0.965925826;
    ClusterPos.column(2) = 0.;
    parameters_p.define("ClusterPos",ClusterPos);
  }
    
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  CRdelayPlugin::~CRdelayPlugin ()
  {
    destroy();
  }
  
  void CRdelayPlugin::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void CRdelayPlugin::summary ()
  {;}
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool CRdelayPlugin::calcWeights(const Matrix<DComplex> &data){
    try {
      Matrix<Double> ClPos, RelClPos, RotClPos;
      Vector<Double> RefPos;
      Double triggerZpos,triggerDelay,totalDelay;
      ClPos = parameters_p.asArrayDouble("ClusterPos");
      RefPos = parameters_p.asArrayDouble("ReferencePos");
      RelClPos.resize(ClPos.shape());
      Int i,ncl=ClPos.nrow();
      for (i=0; i<ncl; i++){
	RelClPos.row(i) = ClPos.row(i)-RefPos;
      };
      RotClPos = toShower(RelClPos);
      triggerZpos = fractile(RotClPos.column(2),10.1/16.);
      triggerDelay = -triggerZpos/LIGHTSPEED;

      totalDelay = triggerDelay + parameters_p.asDouble("ExtraDelay");

      parameters_p.define("TotalDelay",totalDelay);

      //calculate the phase gradients (i.e. the new wheigths) from the correction delays
      Int antInd,nAntennas=data.ncolumn();
      DComplex tmpcomp(0.,1.);
      Vector<DComplex> cFreqVals;
      cFreqVals =  parameters_p.toArrayDComplex("frequencyValues");
      weights_p.resize(data.shape());
      tmpcomp *= -2.*PI*totalDelay;
      weights_p.column(0) = exp(tmpcomp*cFreqVals);
      for (antInd=1; antInd<nAntennas; antInd++){
	weights_p.column(antInd) = weights_p.column(0);
      };

    } catch (AipsError x) {
      cerr << "CRdelayPlugin:calcWeights: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  
  Matrix<Double> CRdelayPlugin::toShower(Matrix<Double> & pos){
    Matrix<Double> erg;
    try {
      if (pos.ncolumn() != 3) {
	cerr << "CRdelayPlugin:toShower: " << "invalis parameters: pos.ncolumn() != 3 " << endl;
	return Matrix<Double>();
      };
      erg.resize(pos.shape());

      Double azrad,elrad;
      azrad = parameters_p.asDouble("Az")*DEG2RAD;
      elrad = parameters_p.asDouble("El")*DEG2RAD;

      Double l1, l2, l3, m1, m2, m3, n1, n2, n3;
      
      l1 = -sin(azrad);
      l2 = cos(azrad);
      l3 = 0.;

      m1 = sin(elrad)*cos(azrad);
      m2 = sin(elrad)*sin(azrad);
      m3 = -cos(elrad);

      n1 = cos(elrad)*cos(azrad);
      n2 = cos(elrad)*sin(azrad);
      n3 = sin(elrad);
      
      erg.column(0) = pos.column(0)*l1 + pos.column(1)*l2;
      erg.column(1) = pos.column(0)*m1 + pos.column(1)*m2 + pos.column(2)*m3;
      erg.column(2) = pos.column(0)*n1 + pos.column(1)*n2 + pos.column(2)*n3;

    } catch (AipsError x) {
      cerr << "CRdelayPlugin:toShower: " << x.getMesg() << endl;
      return Matrix<Double>();
    }; 
    return erg;
  }


} // Namespace CR -- end
