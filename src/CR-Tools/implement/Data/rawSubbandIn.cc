/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
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

#include <Data/rawSubbandIn.h>




namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  rawSubbandIn::rawSubbandIn ()
  {;}
  
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  rawSubbandIn::~rawSubbandIn ()
  {
    destroy();
  }
  
  void rawSubbandIn::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  
  void rawSubbandIn::summary (std::ostream &os)
  {;}
  
  double rawSubbandIn::ntohd(double in){
    double out;
    uint8_t *inp,*outp;
    uint i;
    
    inp = (uint8_t *)(&in);
    outp = (uint8_t *)(&out);
    for(i=0;i<sizeof(double);i++){
      *(outp + i) = *(inp + (sizeof(double)-i-1));
    };

    Int64 *a,*b;
    a =(Int64 *)(&in);
    b =(Int64 *)(&out);

    //cout << "ntohd: "<<hex<<*a<<" " <<*b<<endl;

    return out;
  };

  Int64 rawSubbandIn::ntohll(Int64 in){
    Int64 out;
    uint8_t *inp,*outp;
    uint i;
    
    inp = (uint8_t *)(&in);
    outp = (uint8_t *)(&out);
    for(i=0;i<sizeof(Int64);i++){
      *(outp + i) = *(inp + (sizeof(Int64)-i-1));
    };

    return out;
  };
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool rawSubbandIn::attachFile(String Filename){
    try {
      int numblocks=0,veclen=0;
      size_t datalen,datasize;
      uint ui;
      Bool ok;
      struct BlockHeader BHead;

      FILE *fd = fopen(Filename.c_str(),"r");
      if (fd == NULL) {
	cerr << "rawSubbandIn:attachFile: Can't open file: " << Filename << endl;
	return False;
      };
      ok = readFileHeader(fd, FHead);
      if (!ok) {
	cerr << "rawSubbandIn:attachFile: Error while reading file-header of: " << Filename << endl;
	fclose(fd);
	return False;
      };
      datalen = 2*2*FHead.nrPolarizations*FHead.nrBeamlets*FHead.nrSamplesPerBeamlet;
      if (datalen%8 !=0) {
	datasize = datalen + (8-datalen%8);
      } else {
	datasize = datalen;
      };
      cout << "rawSubbandIn:attachFile: datalen: " << dec << datalen << " datasize: " << datasize <<endl;
      
      veclen = 1000;
      blockdates.resize(veclen);
      while (!feof(fd)){
	ok = readBlockHeader(fd, BHead); 
	if (ok) {
	  numblocks++;
	  blockdates(numblocks-1) = BHead.time[0]/FHead.sampleRate;
	} else {
	  cout << "rawSubbandIn:attachFile: Failed to read block-header " << dec << numblocks+1 
	       << " of file " << Filename << " (end of file?)." << endl;
	  break;
	};
	if (numblocks >= veclen){
	  veclen += 1000;
	  blockdates.resize(veclen,True);
	  cout << "rawSubbandIn:attachFile Increased blockdates-vector size at " << numblocks 
	       << " blocks." << endl;
	};
      };
      
    } catch (AipsError x) {
      cerr << "rawSubbandIn:attachFile: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  Bool rawSubbandIn::readFileHeader(FILE *fd, FileHeader &FHead){
    int ui,i;
    FileHeader tmphead;
    
    ui = fread(&tmphead, 1, sizeof(FileHeader), fd);
    if ( (ui != sizeof(FileHeader) )  ) {
      cerr << "rawSubbandIn::readFileHeader: Inconsitent file: too small. "  << endl;
      return False;
    };
    FHead.magic = ntohl(tmphead.magic);
    FHead.bitsPerSample = tmphead.bitsPerSample;
    FHead.nrPolarizations = tmphead.nrPolarizations;
    FHead.nrBeamlets = ntohs(tmphead.nrBeamlets);
    FHead.nrSamplesPerBeamlet = ntohl(tmphead.nrSamplesPerBeamlet);
    for (i=0;i<16;i++){
      FHead.station[i] = tmphead.station[i];
    };
    FHead.sampleRate = ntohd(tmphead.sampleRate);
    for (i=0; i<54; i++){
      FHead.subbandFrequencies[i] = ntohd(tmphead.subbandFrequencies[i]);
    };
    for (i=0; i<8; i++){
      FHead.beamDirections[i][0] = ntohd(tmphead.beamDirections[i][0]);
      FHead.beamDirections[i][1] = ntohd(tmphead.beamDirections[i][1]);
    };
    for (i=0; i<54; i++){
      FHead.beamlet2beams[i] = ntohs(tmphead.beamlet2beams[i]);
    };
    if (FHead.magic == 0x3F8304EC) {
      return True;
    } else {
      cerr << "rawSubbandIn::readFileHeader: Inconsitent file: bad magic!" << endl;
      return False;
    };
  };

  Bool rawSubbandIn::readBlockHeader(FILE *fd, BlockHeader &BHead){
    int ui,i,j;
    BlockHeader tmphead;
    
    ui = fread(&tmphead, 1, sizeof(BlockHeader), fd);
    if ( (ui != sizeof(BlockHeader) )  ) {
      cerr << "rawSubbandIn::readBlockHeader: Inconsitent file: too small. "  << endl;
      return False;
    };
    BHead.magic = ntohl(tmphead.magic);
    for (i=0;i<8;i++) {
      BHead.coarseDelayApplied[i] = ntohl(tmphead.coarseDelayApplied[i]);
      BHead.fineDelayRemainingAtBegin[i] = ntohd(tmphead.fineDelayRemainingAtBegin[i]);
      BHead.fineDelayRemainingAfterEnd[i] = ntohd(tmphead.fineDelayRemainingAfterEnd[i]);
      BHead.time[i] = ntohll(tmphead.time[i]);
      BHead.nrFlagsRanges[i] = ntohl(tmphead.nrFlagsRanges[i]);
      for (j=0;j<16;j++){
	BHead.flagsRanges[i][j].begin = ntohl(tmphead.flagsRanges[i][j].begin);
	BHead.flagsRanges[i][j].end = ntohl(tmphead.flagsRanges[i][j].end);
      };
    };
    if (BHead.magic == 0x2913D852) {
      return True;
    } else {
      cerr << "rawSubbandIn::readBlockHeader: Inconsitent file: bad magic!" << endl;
      return False;
    };
  };
  

} // Namespace CR -- end
