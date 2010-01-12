/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Maaijke Mevius (<mail>)                                               *
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

#include <Analysis/Utils/TBBData.h>

namespace CR { // Namespace CR -- begin

  //_____________________________________________________________________________
  //                                                                     ~TBBData

  TBBData::~TBBData(){
    for(vector<TBB_Timeseries *>::iterator idat=daldata.begin();idat!=daldata.end();idat++)
      delete *idat;
    daldata.clear();
  }
  
  //_____________________________________________________________________________
  //                                                                  nofAntennas

  TBBData::TBBData():nofAntennas(-1)
  {
  }
  
  //_____________________________________________________________________________
  //                                                                     initData

  /*!
    \e even, \e odd is dipole selection within a file (x an y repsectively) 
  */
  void  TBBData::initData (vector<string> filenames,
			   bool odd,
			   bool even)
  {
    uint nofFiles = filenames.size();
  
    daldata.resize(nofFiles);
    
    for(uint i=0;i<nofFiles;i++){
      TBB_Timeseries *dat = new TBB_Timeseries(filenames[i]);
      daldata[i] = dat;
    }
    if(odd){
      if(even)
	{dipstart=0;dipstep=1;}
      else
	{dipstart=1;dipstep=2;}
    }
    else
      {dipstart=0;dipstep=2;}

    
    initOffsets();
  }
  
  //_____________________________________________________________________________
  //                                                                  initOffsets
  
  void  TBBData::initOffsets()
  {
    //first get maximal offset
    uint nr_files = daldata.size();
    assert(nr_files>0);
    filenr.clear();
    dipnr.clear();
    offsets.clear();
    uint max_samp = 0;
    uint imax=0;
    uint maxdat=0;
    //get reference time;
    std::vector<uint> attrTime;
    (daldata[0]->stationGroup(0)).getAttributes("TIME",attrTime);
    uint time0= attrTime[0];
    //
    for (uint idat =0;idat<nr_files;idat++){
      uint nofGroups = daldata[idat]->nofStationGroups();
      uint dipoles = daldata[idat]->nofDipoleDatasets() ;
      casa::Vector<uint> samplenr;
      casa::Vector<uint> timenr;
      
      for(uint stati=0;stati <nofGroups;stati++)
	{
	  
	  TBB_StationGroup stat= daldata[idat]->stationGroup(stati);
	  stat.getAttributes("TIME",timenr);
	  stat.getAttributes("SAMPLE_NUMBER",samplenr);
	  for(uint idip=dipstart;idip<dipoles;idip+=dipstep){
	    int time_offs = timenr(idip)-time0;
	    if(abs(time_offs)>0) {
	      cout<<"WARNING, time offset!! filenr"<<idat<<" dipole "<<idip<<endl;
	      //time diferences not implemented yet
	      
	    }
	    if((time_offs+samplenr(idip))>max_samp){
	      max_samp=samplenr(idip);
	      imax=idip;
	      maxdat=idat;
	    }
	  }
	}
      
    }//file loop
    
    cout<<"Maximum offset for file:"<<maxdat<<" and dipolenr :"<<imax<<" = "<<max_samp<<endl;
    cout<<"time0 "<<time0<<endl;
    
    minlength=1e9;
    nofAntennas=0;
    //Now fill vector with offsets
    for (uint idat =0;idat<nr_files;idat++){
      uint nofGroups = daldata[idat]->nofStationGroups();
      
      uint dipoles = daldata[idat]->nofDipoleDatasets() ;
      for(uint stati=0;stati <nofGroups;stati++)
	{
	  
	  TBB_StationGroup stat= daldata[idat]->stationGroup(stati);
	  // set sample at which to start reading
	  casa::Vector<uint> lths;
	  casa::Vector<uint> samplenr;
	  casa::Vector<uint> timenr;

	  stat.getAttributes("DATA_LENGTH",lths);
	  stat.getAttributes("SAMPLE_NUMBER",samplenr);
	  stat.getAttributes("TIME",timenr);
	  
	  //read the data per dipole
	  for(uint idip=dipstart;idip<dipoles;idip+=dipstep){
	    uint offset = time0 - timenr(idip) +  max_samp - samplenr(idip);
	    offsets.push_back(offset);
	    uint length = lths(idip) - offset;
	    if(length < minlength) minlength = length;
	    filenr.push_back(idat); //keep track of file belonging to antenna number
	    dipnr.push_back(idip);//keep track of dipole number within file
	    nofAntennas++;
	  }
	}
    }
    
    
    
  }

  //_____________________________________________________________________________
  //                                                                getTimeSeries

  void TBBData::getTimeSeries (vector<double>& out,
			       uint blocksize,
			       int ant,
			       uint startSample) const
  {
    out.clear();
    casa::Vector<double> outdat;
    getTimeSeries (outdat,blocksize,ant,startSample);
    
    outdat.tovector(out);
    
  };
  
  //_____________________________________________________________________________
  //                                                                getTimeSeries

  /*!
    \retval out -- Array/Vector returning the time-series data for dipole \e ant
    \param blocksize -- The number of samples per block of data.
    \param ant -- Index of the dipole within a station (not a global, unique 
           identifier for the entire telescope).
    \param startSample -- Start position within the dataset from which to start
           reading the data.
   */
  void TBBData::getTimeSeries (casa::Vector<double>& out,
			       uint blocksize,
			       int ant,
			       uint startSample) const
  {
    assert(ant<nofAntennas);

    int start;
    casa::Vector<casa::String> dipoleIDs;
    std::set<std::string> dipoleSelection;
    casa::Matrix<double> data;

    // Get the IDs/names of the dipoles attached to the station group
    (daldata[filenr[ant]]->stationGroup(0)).dipoleNames (dipoleIDs);
    // Select dipole
    dipoleSelection.clear();
    dipoleSelection.insert(dipoleIDs[ant]);
    (daldata[filenr[ant]]->stationGroup(0)).setSelectedDipoles(dipoleSelection);
    // Set the position from which to start reading data
    start              = offsets[ant] + startSample;
    // read the data
    (daldata[filenr[ant]]->stationGroup(0)).fx(data,
					       start,
					       blocksize);

    out = data.row(0);
  };
  
} // end -- namespace CR

