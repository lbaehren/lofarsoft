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
      int veclen=0;
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

      numblocks=0;
      veclen = 1000;
      blockdates.resize(veclen);
      firstdate = 1e99; lastdate =0;
      while (!feof(fd)){
	ok = readBlockHeader(fd, BHead); 
	if (ok) {
	  numblocks++;
	  blockdates(numblocks-1) = BHead.time[0]/FHead.sampleRate;
	  fseek(fd, datasize, SEEK_CUR);
	  if (blockdates(numblocks-1) < firstdate) {
	    firstdate = blockdates(numblocks-1);
	  };
	  
	  cout << "block: " << numblocks  << " position: " << ftell(fd) 
	       << " date: " << blockdates(numblocks-1) - firstdate <<endl;
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
      blockdates.resize(numblocks,True);
      lastdate = max(blockdates)+FHead.nrSamplesPerBeamlet/FHead.sampleRate;
      OpenedFile = Filename;
      fclose(fd);
    } catch (AipsError x) {
      cerr << "rawSubbandIn:attachFile: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  Matrix<DComplex> rawSubbandIn::getData(Double startdate, int nSamples, int pol){
    Matrix<DComplex> data(nSamples,FHead.nrBeamlets,0.);
    try {
      int bnum,datalen,sample,beamlet,sindex,ui;
      Double stopdate;
      struct BlockHeader BHead;
      
      stopdate = startdate + nSamples/FHead.sampleRate;
      if (stopdate >= lastdate){
	cerr << "rawSubbandIn:getData: " << "Too many samples requested! (stopdate >= lastdate)" << endl;
	return Matrix<DComplex>();
      }
      
      FILE *fd = fopen(OpenedFile.c_str(),"r");
      if (fd == NULL) {
	cerr << "rawSubbandIn:getData: Can't open file: " << OpenedFile << endl;
	return Matrix<DComplex>();
      };
      fseek(fd, sizeof(struct FileHeader), SEEK_CUR);

      datalen = 2*2*FHead.nrPolarizations*FHead.nrBeamlets*FHead.nrSamplesPerBeamlet;
      if (datalen%8 !=0) {
	datalen = datalen + (8-datalen%8);
      };
      short *datap;
      datap = (short int *)malloc(datalen);
      if (datap == NULL){
	cerr << "rawSubbandIn:getData: " << "Unable to allocate temporary memory" << endl;
	fclose(fd);
	return Matrix<DComplex>();
      }

      for (bnum=0; bnum<numblocks; bnum++){
	if (blockdates(bnum+1) < startdate){
	  //current block is before the requested part
	  fseek(fd, sizeof(struct BlockHeader), SEEK_CUR);
	  fseek(fd, datalen, SEEK_CUR);
	} else if (blockdates(bnum) >stopdate) {
	  //current block is after the requested part
	  break;
	} else {
	  fseek(fd, sizeof(struct BlockHeader), SEEK_CUR);
	  if (blockdates(bnum) != BHead.time[0]/FHead.sampleRate) {
	    cerr << "rawSubbandIn:getData: " << "Inconsistent state: blockdates != BHead.time" << endl;
	  };
	  ui = fread(datap, 1, datalen, fd);
	  if ( ui != datalen ) {
	    cerr << "rawSubbandIn:getData: Failed to read in full datablock. "  << endl;
	  };
	  for (sample=0; sample<FHead.nrSamplesPerBeamlet; sample++){
	    sindex = (int)ceil((blockdates(bnum)-startdate)*FHead.sampleRate)+sample;
	    if (sindex>0 && sindex<nSamples){
	      for (beamlet=0; beamlet<FHead.nrBeamlets; beamlet++){
		data(sindex,beamlet) = DComplex(ntohs(datap[0]),ntohs(datap[1]));
	      };
	    };
	  };
	};
      };
      free(datap);
      fclose(fd);
    } catch (AipsError x) {
      cerr << "rawSubbandIn:getData: " << x.getMesg() << endl;
      return Matrix<DComplex>();
    }; 
    return data;
  }

  Vector<Int> rawSubbandIn::getSubbandIndices(){
    Vector<Int> out(36);
    out(0) = 256; out(1) = 259; out(2) = 262; out(3) = 265; out(4) = 268;
    out(5) = 271; out(6) = 274; out(7) = 277; out(8) = 280; out(9) = 283;
    out(10) = 286; out(11) = 289; out(12) = 292; out(13) = 295; out(14) = 298;
    out(15) = 301; out(16) = 304; out(17) = 307; out(18) = 310; out(19) = 313; 
    out(20) = 316; out(21) = 319; out(22) = 322; out(23) = 325; out(24) = 328; 
    out(25) = 331; out(26) = 334; out(27) = 337; out(28) = 340; out(29) = 343; 
    out(30) = 346; out(31) = 349; out(32) = 352; out(33) = 355; out(34) = 358; 
    out(35) = 361;

    return out;
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
