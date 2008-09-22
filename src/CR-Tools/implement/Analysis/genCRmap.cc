/*-------------------------------------------------------------------------*
 | $Id:: Imaging.h 1097 2007-11-28 10:51:59Z baehren                     $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
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

#include <Analysis/genCRmap.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  genCRmap::genCRmap(){

    // Reference position for the antenna setup
    ReferencePos = Vector<Double>(3,0.);
    
    SkyCoord_p.setBeamType(CR::TIME_X);
    SkyCoord_p.setMapOrientation(SkymapCoordinates::NORTH_WEST);
    //SkyCoord_p.setDirectionShape(51,51);
    
  }
    
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  genCRmap::~genCRmap ()
  {
    destroy();
  }
  
  void genCRmap::destroy ()
  {;}
    
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool genCRmap::GenerateMap(const String &filename, const Matrix<DComplex> &data){
    Bool success=True;
    try {
      Skymapper skymapper(SkyCoord_p);
      skymapper.setFilename(filename);
      //      if (!skymapper.isOperational()){
      //	cerr << "genCRmap::GenerateMap: " << "Failed to initialize the skymapper!" << endl;
      //	return False;
      //      };
      cout << "*** Printing skymapper summary before processing" << endl;
      skymapper.summary();
      success = success && skymapper.processData(data);
      cout << "*** Printing skymapper summary after processing" << endl;
      skymapper.summary();
    } catch (AipsError x) {
      cerr << "genCRmap::GenerateMap: " << x.getMesg() << endl;
      return False;
    }; 
    return success;
  }




  Bool genCRmap::setMapHeader(DataReader &dr, CalTableReader *CTRead){
    Bool success=True;
    try {
      Vector<Int> AntennaIDs;
      uInt i,date;
 //     dr.headerRecord().get("Date",date);
 //     dr.headerRecord().get("AntennaIDs",AntennaIDs);
      Vector<Double> tmpvec;

      // Set TimeFreq values from the data reader.
      success = success && SkyCoord_p.setTimeFreq(dr);
      //success = success && SkyCoord_p.setTimeFreq(dr.blocksize(),dr.sampleFrequency(),
      //					  dr.nyquistZone(), dr.referenceTime() );
      
      // Set Observation data values
      ObservationData obsData=SkyCoord_p.observationData();
 //     obsData.setObservatory(dr.headerRecord().asString("Observatory"));
      obsData.setEpoch(LOPES2Epoch(date));
      obsData.setObserver("CR-User");
      Matrix<Double> tmpAntPos(3,AntennaIDs.nelements());
      AntPositions_p.resize(3,AntennaIDs.nelements());
      for (i=0;i<AntennaIDs.nelements();i++){
	CTRead->GetData(date, AntennaIDs(i), "Position", &tmpvec);
	AntPositions_p.column(i) = tmpvec;
	tmpAntPos.column(i) = tmpvec-ReferencePos;
      };
      obsData.setAntennaPositions(tmpAntPos);

      success = success && SkyCoord_p.setObservationData(obsData);

      cout << "*** Printing obsData summary " << endl;
      obsData.summary();
      cout << "*** Printing SkyCoord summary " << endl;
      SkyCoord_p.summary();
    } catch (AipsError x) {
      cerr << "genCRmap::setMapHeader: " << x.getMesg() << endl;
      return False;
    }; 
    return success;
  }


  MEpoch genCRmap::LOPES2Epoch(uInt JDR,int TL){
    return MEpoch(MVEpoch(Quantity( ((Double)JDR+((Double)TL/5e6)+3506716800.), "s")),
		  MEpoch::Ref(MEpoch::UTC));
  };


} // Namespace CR -- end
