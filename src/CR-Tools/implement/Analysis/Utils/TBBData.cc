/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Maaijke Mevius (<mail>)                                            *
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


  TBBData::TBBData():nofAntennas(-1)
  {
  }
  
  TBBData::~TBBData(){
    for(vector<TBB_Timeseries *>::iterator idat=daldata.begin();idat!=daldata.end();idat++)
      delete *idat;
    daldata.clear();
    
  }
  
  void  TBBData::initData(vector<string> filenames,bool odd,bool even){
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
  
  void  TBBData::initOffsets(){
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
    uint time0= (daldata[0]->stationGroup(0)).time()(0);
    for (uint idat =0;idat<nr_files;idat++){
      uint nofGroups = daldata[idat]->nofStationGroups();
       uint dipoles = daldata[idat]->nofDipoleDatasets() ;
   
      for(uint stati=0;stati <nofGroups;stati++)
	{
 
	  TBB_StationGroup stat= daldata[idat]->stationGroup(stati);
	  casa::Vector<uint> samplenr = stat.sample_number();
	  casa::Vector<uint> timenr = stat.time();
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
	  casa::Vector<uint> lths = stat.data_length();
	  
	  casa::Vector<uint> samplenr = stat.sample_number();
	  casa::Vector<uint> timenr   = stat.time();
	  
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

  void TBBData::getTimeSeries(vector<double>& out,uint blocksize,int ant, uint startSample) const{

    out.clear();
    casa::Vector<double> outdat;
    getTimeSeries(outdat,blocksize,ant,startSample);
    
    outdat.tovector(out);
    
  };
  
  void TBBData::getTimeSeries(casa::Vector<double>& out,uint blocksize,int ant, uint startSample) const{

    assert(ant<nofAntennas);
    vector<uint> dipoleSelection(1);
    dipoleSelection[0] = dipnr[ant];
    out= (daldata[filenr[ant]]->stationGroup(0)).fx(offsets[ant] + startSample,
						       blocksize,
						       dipoleSelection);
    
  };
  
}//namespace CR

