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

#include <Data/tbbctlIn.h>
       
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  tbbctlIn::tbbctlIn()
    : DataReader (1){
    init();
  }    
  
  
  void tbbctlIn::init(){
    DataReader::setNyquistZone(1);
    DataReader::setSampleFrequency(1.);
    NumAntennas_p = 0;
    filename_p = "";
    attached_p = False;
    iterator_p = NULL;
    headerpoint_p = (tbbctl_head*)malloc(sizeof(tbbctl_head));
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  tbbctlIn::~tbbctlIn ()
  {
    destroy();
  }
  
  void tbbctlIn::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void tbbctlIn::summary (std::ostream &os)
  {;}
  
  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool tbbctlIn::attachFile(String filename){
    try {
      int i;
      uint ui,start,noSamples,numchannels=1;
      uint block,numblocks,maxblock,startdate,stopdate,startsample,stopsample;
      short *tmppoint;
      int AntID;
      Double sampleFreq,tmpdouble;
      struct stat filestat;
     
      attached_p = False;
      stat(filename.c_str(), &filestat);
      numblocks = filestat.st_size / TBBCTL_BLOCK_SIZE;
      if (numblocks < 1) {
	cerr << "tbbctlIn:attachFile: File " << filename << "too small (smaller than one blocksize)." <<endl;
	return False;
      };
      FILE *fd = fopen(filename.c_str(),"r");
      if (fd == NULL) {
	cerr << "tbbctlIn:attachFile: Can't open file: " << filename << endl;
	return False;
      };
      ui = fread(headerpoint_p, 1, sizeof(tbbctl_head), fd);
      if ( (ui != sizeof(tbbctl_head) )  ) {
	cerr << "tbbctlIn:attachFile: Inconsitent file: " << filename << endl;
	fclose(fd);
	  return False;
      };
      startdate = headerpoint_p->Date;
      startsample = headerpoint_p->SampleNr;
      stopdate = headerpoint_p->Date;
      stopsample = headerpoint_p->SampleNr + headerpoint_p->NoSamples;
      maxblock = headerpoint_p->NoSamples;
      sampleFreq = headerpoint_p->sampleFreq*1e6;
      AntID = (headerpoint_p->station*256+headerpoint_p->RSP)*256+headerpoint_p->RCU;
      for (block=1; block<numblocks; block++){
	fseek(fd, (block*(long)TBBCTL_BLOCK_SIZE), SEEK_SET); 
	ui = fread(headerpoint_p, 1, sizeof(tbbctl_head), fd);
	if ( (ui != sizeof(tbbctl_head) )  ) {
	  cerr << "tbbctlIn:attachFile: Inconsitent file: " << filename << endl;
	  fclose(fd);
	  return False;
	};
	if (headerpoint_p->Date < startdate) {
	  startdate = headerpoint_p->Date;
	  startsample = headerpoint_p->SampleNr;
	} else if ((headerpoint_p->Date==startdate) && (headerpoint_p->SampleNr<startsample)){
	  startsample = headerpoint_p->SampleNr;
	};
	if (headerpoint_p->Date > stopdate) {
	  stopdate = headerpoint_p->Date;
	  stopsample = headerpoint_p->SampleNr + headerpoint_p->NoSamples;
	} else if ((headerpoint_p->Date==startdate) &&
		   ( (headerpoint_p->SampleNr+ headerpoint_p->NoSamples) > stopsample)){
	  stopsample = headerpoint_p->SampleNr + headerpoint_p->NoSamples;
	};
	if (headerpoint_p->NoSamples > maxblock) { maxblock = headerpoint_p->NoSamples; };
      };
      tmpdouble = (stopdate-startdate)*sampleFreq+stopsample-startsample;
      noSamples = (uint)tmpdouble;
      cout << "tbbctlIn:attachFile: " << "File " << filename << " spans a range of " 
	   << noSamples << " samples" << endl;
      //Allocate memory
      channeldata_p.resize(noSamples,numchannels);
      channeldata_p=0;
      tmppoint = (short*)malloc(maxblock*sizeof(short));
      if (tmppoint == NULL) {
 	cerr << "tbbctlIn:attachFile: Error while allocating temporary memory " << endl;
 	fclose(fd);
 	return False;	
      };
      for (block=0; block<numblocks; block++){
	fseek(fd, (block*(long)TBBCTL_BLOCK_SIZE), SEEK_SET); 
	fread(headerpoint_p, 1, sizeof(tbbctl_head), fd);
	start = (uint)((headerpoint_p->Date-startdate)*sampleFreq + 
		       headerpoint_p->SampleNr-startsample);
	fread(tmppoint, sizeof(short),headerpoint_p->NoSamples, fd); 
	channeldata_p.column(0)(Slice(start,headerpoint_p->NoSamples)) = Vector<short>(IPosition(1,headerpoint_p->NoSamples),tmppoint,SHARE);
      };
      // free the memory
      free(tmppoint);
 
//       setStreams();
//       generateHeaderRecord();
    } catch (AipsError x) {
      cerr << "tbbctlIn:attachFile: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }
  
  Matrix<Double> tbbctlIn::fx () {
    uint antenna;
    uint startsample;
    uint nofSelectedAntennas(DataReader::nofSelectedAntennas());
    Matrix<Double> fx(DataReader::blocksize(),nofSelectedAntennas);
    Matrix<Short> fx_short(DataReader::blocksize(),nofSelectedAntennas);
    
    for (antenna=0; antenna<nofSelectedAntennas; antenna++) {
      startsample = iterator_p[selectedAntennas_p(antenna)].position();
      fx_short.column(antenna) = channeldata_p.column(selectedAntennas_p(antenna))(Slice(startsample,DataReader::blocksize()));
    };
    convertArray(fx,fx_short);
    
    return fx;
  }

} // Namespace CR -- end
