/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2006                                                  *
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

#include <Data/LopesEventIn.h>

namespace CR { // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================

  // --------------------------------------------------------------- LopesEventIn
  
  LopesEventIn::LopesEventIn()
    : DataReader (1){
    init();
  }

  // --------------------------------------------------------------- LopesEventIn
  
  LopesEventIn::LopesEventIn(String const &filename)
    : DataReader (1){
    init();
    attachFile(filename);
  }
  
  // --------------------------------------------------------------- LopesEventIn
  
  LopesEventIn::LopesEventIn (String const &filename,
			      uint const &blocksize)
    : DataReader (blocksize){
    init();
    attachFile(filename);
  }
  
  void LopesEventIn::init(){
    DataReader::setNyquistZone(LOPES_NYQUIST_ZONE);
    DataReader::setSampleFrequency(LOPES_SAMPLERATE);
    NumAntennas_p = 0;
    filename_p = "";
    attached_p = False;
    iterator_p = NULL;
    headerpoint_p = (lopesevent_v1*)malloc(LOPESEV_HEADERSIZE);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  LopesEventIn::~LopesEventIn()
  {
    destroy();
  }
  
  void LopesEventIn::destroy()
  {
    free(headerpoint_p);
    if (iterator_p != NULL) { delete [] iterator_p; iterator_p=NULL; };
  }

  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================  
  
  // ============================================================================
  //
  //  Methods
  //
  // ============================================================================
  
  Bool LopesEventIn::attachFile(String filename){
    int i;
    uint ui,tmpchan,tmplen;
    short *tmppoint;
    
    FILE *fd = fopen(filename.c_str(),"r");
    if (fd == NULL) {
      cerr << "LOPESEventIn:attachFile: Can't open file: " << filename << endl;
      return False;
    };
    attached_p = False;
    ui = fread(headerpoint_p, 1, LOPESEV_HEADERSIZE, fd);
    if ( (ui != LOPESEV_HEADERSIZE ) || (headerpoint_p->type != TIM40) ) {
      cerr << "LOPESEventIn:attachFile: Inconsitent file: " << filename << endl;
      fclose(fd);
      return False;
    };
    NumAntennas_p =0;
    fread(&tmpchan, 1, sizeof(uint), fd);
    while (!feof(fd)){
      fread(&tmplen, 1, sizeof(uint), fd); 
      if (headerpoint_p->blocksize==0) {headerpoint_p->blocksize = tmplen;};
      if (headerpoint_p->blocksize != tmplen) {
	cerr << "LOPESEventIn:attachFile: Inconsitent file (different blocksizes): " << filename << endl;
	fclose(fd);
	return False;
      };
      i = fseek(fd,tmplen*sizeof(short),SEEK_CUR);
      if (i != 0) {
	cerr << "LOPESEventIn:attachFile: Inconsitent file (unexpected end): " << filename << " in channel " << 
	  tmpchan << " len:" << tmplen << endl;
	fclose(fd);
	return False;
      };
      NumAntennas_p++;
      fread(&tmpchan, 1, sizeof(uint), fd); //this will trigger eof condition after last channel;
    };
    try {
      AntennaIDs_p.resize(NumAntennas_p);
      channeldata_p.resize(headerpoint_p->blocksize,NumAntennas_p);
      tmppoint = (short*)malloc(headerpoint_p->blocksize*sizeof(short));
      if (tmppoint == NULL) {
	cerr << "LOPESEventIn:attachFile: Error while allocating temporary memory " << endl;
	fclose(fd);
	return False;
	
      };
      //    clearerr(fd);
      fseek(fd,LOPESEV_HEADERSIZE,SEEK_SET); // this should reset the eof condition
      for (i=0;i<NumAntennas_p;i++){
	fread(&tmpchan, 1, sizeof(uint), fd); 
	fread(&tmplen, 1, sizeof(uint), fd); 
	AntennaIDs_p(i) = (int)tmpchan;
	fread(tmppoint, sizeof(short),headerpoint_p->blocksize , fd); 
	channeldata_p.column(i) = Vector<short>(IPosition(1,headerpoint_p->blocksize),tmppoint,SHARE);
      };
      filename_p = filename;
      attached_p = True;
      setStreams();
      generateHeaderRecord();
      free(tmppoint);
    } catch (AipsError x) {
      cerr << "LOPESEventIn:attachFile: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  }

  Bool LopesEventIn::setStreams(){
    try{
      int antenna;
      
      Vector<String> filenames(NumAntennas_p,filename_p);

      if (iterator_p != NULL) { delete [] iterator_p; iterator_p=NULL; };
      iterator_p = new DataIterator[NumAntennas_p];
      
      for (antenna=0; antenna<NumAntennas_p; antenna++){
	iterator_p[antenna].setDataStart(0);
	iterator_p[antenna].setStride(0);
	iterator_p[antenna].setShift(0);
	//Set the width of a step to 1, so we count in samples:
	iterator_p[antenna].setStepWidthToSample();
	iterator_p[antenna].setBlocksize(headerpoint_p->blocksize);
      }
      
      /*
	Setup of the conversion arrays
      */
      Vector<Double> adc2voltage(NumAntennas_p,1./2048.);
      
      uint fftLength(headerpoint_p->blocksize/2+1);      
      Matrix<DComplex> fft2calfft(fftLength,NumAntennas_p,1.0);

      DataReader::init(headerpoint_p->blocksize,
		       adc2voltage,
		       fft2calfft);
      setReferenceTime(-headerpoint_p->presync/LOPES_SAMPLERATE);

    } catch (AipsError x) {
      cerr << "LOPESEventIn:setStreams: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
    
  };

  Bool LopesEventIn::generateHeaderRecord(){
    try {
      header_p.define("Date",headerpoint_p->JDR);
      header_p.define("AntennaIDs",AntennaIDs_p);
      switch (headerpoint_p->observatory) {
      case (LopesEventIn::LOPES):
	header_p.define("Observatory","LOPES");
	break;
      case (LopesEventIn::LORUN):
	header_p.define("Observatory","LORUN");
	break;
      default:
	header_p.define("Observatory","");
      };
      header_p.define("Filesize",(Int)headerpoint_p->blocksize);
      header_p.define("presync",headerpoint_p->presync);
      header_p.define("TL",(Int)headerpoint_p->TL);
      header_p.define("LTL",(Int)headerpoint_p->LTL);
      header_p.define("dDate",(Double)headerpoint_p->JDR+(double)headerpoint_p->TL/5e6);
      header_p.define("EventClass",(Int)headerpoint_p->evclass);
    } catch (AipsError x) {
      cerr << "LOPESEventIn:generateHeaderRecord: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };
  
  
  Matrix<Double> LopesEventIn::fx ()
  {
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
  

  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================
  

} // Namespace CR -- end
