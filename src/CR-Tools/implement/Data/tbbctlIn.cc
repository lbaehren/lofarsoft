/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
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

#include <Data/tbbctlIn.h>
#include <Data/CRCcheck.h>       
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#define TBBCTLIN_MAXSAMPLES 0x4000000

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

  tbbctlIn::tbbctlIn(Vector<String> const &filenames)
    : DataReader (1){
    init();
    attachFile(filenames);
  }    
  
  
  void tbbctlIn::init(){
    DataReader::setNyquistZone(1);
    DataReader::setSampleFrequency(1.);
    NumAntennas_p = 0;
    filenames_p.resize(0);
    attached_p = False;
    iterator_p = NULL;
    headerpoint_p = (tbbctl_head*)malloc(sizeof(tbbctl_head));
		// test CRC32-speed
		short * testbuffer = new short[2048];
		for (uint i=0; i<2048; i++)
		{
			testbuffer[i]=rand();
		}
		CRCcheck * checker = new CRCcheck;
		
		clock_t clocks = clock();
		uint nofblocks = 10000;
		for(uint i=0; i<nofblocks; i++)
		{
			checker->CRC32_tbbdata(testbuffer, 1024); // output uint32_t CRC is discarded
		}
		clocks = clock() - clocks;
		cout << "CRC speed: " << nofblocks * 2 / 1024 / ( (double) clocks / CLOCKS_PER_SEC) << " MB/s" << endl;
		// MB = nofblocks * 2 / 1024
		delete checker;
		delete testbuffer;									
		
		
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
  
  void tbbctlIn::destroy() {
    free(headerpoint_p);
  }
  
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
	
	
  Bool tbbctlIn::attachFile(Vector<String> filenames){
    try {
      int numchannels,fnum;
      uint ui,start,noSamples;
      uint block,maxblock=0,startdate=0,stopdate=0,startsample=0,stopsample=0;
      short *tmppoint;
      Vector<int> AntIDs;
      Double sampleFreq=0.,tmpdouble;
      Vector<uint> numblocks;
      String filename;
      struct stat filestat;
     
      attached_p = False;
      CRCcheck * CRCchecker = new CRCcheck;
			
      numchannels = filenames.nelements();
      if (numchannels < 1) {
        cerr << "tbbctlIn:attachFile: No filenames given!" <<endl;
        return False;
            };
            numblocks.resize(numchannels); AntIDs.resize(numchannels);
            for (fnum=0;fnum<numchannels;fnum++){
        filename = filenames(fnum);
        stat(filename.c_str(), &filestat);
        numblocks(fnum) = filestat.st_size / TBBCTL_BLOCK_SIZE;
        if (numblocks(fnum) < 1) {
          cerr << "tbbctlIn:attachFile: File " << filename << "too small (smaller than one blocksize)." <<endl;
          return False;
        };
        //	cout << " working on file: " << filename << endl;
        FILE *fd = fopen(filename.c_str(),"r");
        if (fd == NULL) {
          cerr << "tbbctlIn:attachFile: Can't open file: " << filename << endl;
          return False;
        };
        ui = fread(headerpoint_p, 1, sizeof(tbbctl_head), fd);
        if ( (ui != sizeof(tbbctl_head) )  ) {
          cerr << "tbbctlIn:attachFile: Inconsistent file: " << filename << endl;
          fclose(fd);
          return False;
        };
        // Header CRC check (CRC16)
              // uint16_t CRC16 = this->CRC16((uint16_t *)headerpoint_p, 44);
        uint16_t CRC16 = CRCchecker->headerCRC(headerpoint_p);
			
				//std::cout << "Header CRC = " << CRC16 << std::endl;      
				if (CRC16 != 0)
				{
					cerr << "tbbctlIn:attachFile: header CRC failure: " << filename << endl;
					fclose(fd);
					return False;
				}      
              
        if (fnum==0){
          startdate = headerpoint_p->Date;
          startsample = headerpoint_p->SampleNr;
          stopdate = headerpoint_p->Date;
          stopsample = headerpoint_p->SampleNr + headerpoint_p->NoSamples;
          maxblock = headerpoint_p->NoSamples;
          sampleFreq = headerpoint_p->sampleFreq*1e6;
        };
        //AntIDs(fnum) = (headerpoint_p->station*256+headerpoint_p->RSP)*256+headerpoint_p->RCU;
        AntIDs(fnum) = (1*256+headerpoint_p->RSP)*256+headerpoint_p->RCU;
        for (block=0; block<numblocks(fnum); block++){
          fseek(fd, (block*(long)TBBCTL_BLOCK_SIZE), SEEK_SET); 
          ui = fread(headerpoint_p, 1, sizeof(tbbctl_head), fd);
          if ( (ui != sizeof(tbbctl_head) )  ) {
            cerr << "tbbctlIn:attachFile: Inconsistent file: " << filename << endl;
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
          //cout << "file:" << filename <<" Date:" << headerpoint_p->Date << " start:" << headerpoint_p->SampleNr << endl;
        };
        fclose(fd);
            }; // END: for (fnum=0;fnum<numchannels;fnum++)
            tmpdouble = (stopdate-startdate)*sampleFreq+stopsample-startsample;
            noSamples = (uint)tmpdouble;
      #ifdef DEBUGGING_MESSAGES
            cout << "tbbctlIn:attachFile: " << "Files " << filenames << " spans a range of " 
                 << noSamples << " samples" << endl;
      #endif
            if ((noSamples*numchannels) > TBBCTLIN_MAXSAMPLES){
        cout << "tbbctlIn:attachFile: " << "Files " << filenames 
             << " contain more than "<< TBBCTLIN_MAXSAMPLES <<" samples! Truncating to " << TBBCTLIN_MAXSAMPLES/numchannels 
             << " samples/channel!" << endl;
        noSamples = TBBCTLIN_MAXSAMPLES/numchannels;
            }
            //Allocate memory
            channeldata_p.resize(noSamples,numchannels);
            channeldata_p=0;
            tmppoint = (short*)malloc((maxblock+2)*sizeof(short));
            if (tmppoint == NULL) {
        cerr << "tbbctlIn:attachFile: Error while allocating temporary memory " << endl;
        return False;	
            };
            for (fnum=0;fnum<numchannels;fnum++){
        filename = filenames(fnum);
        FILE *fd = fopen(filename.c_str(),"r");
        for (block=0; block<numblocks(fnum); block++){
          fseek(fd, (block*(long)TBBCTL_BLOCK_SIZE), SEEK_SET); 
          fread(headerpoint_p, 1, sizeof(tbbctl_head), fd);
          start = (uint)((headerpoint_p->Date-startdate)*sampleFreq + 
             headerpoint_p->SampleNr-startsample);
          if ( (start+headerpoint_p->NoSamples) <= noSamples) {
						uint itemsToRead = headerpoint_p->NoSamples + sizeof(uint32_t) / 2; // also read payload CRC which is 2 shorts
            fread(tmppoint, sizeof(short), itemsToRead, fd); 

						// frame is read in, now do CRC32
						uint32_t CRC32 = CRCchecker->CRC32_tbbdata(tmppoint, itemsToRead);
						//std::cout << "Frame no " << block << ": Payload CRC = " << CRC32 << std::endl;      
						if (CRC32 != 0)
						{
							cerr << "tbbctlIn:attachFile: CRC failure: " << filename << " block: " << block << endl;
							fclose(fd);
							return False;
						}
						
            channeldata_p.column(fnum)(Slice(start,headerpoint_p->NoSamples)) = Vector<short>(IPosition(1,headerpoint_p->NoSamples),tmppoint,casa::SHARE);
          };
        };
				
        fclose(fd);
				delete CRCchecker;
            };
            // Save the general data
            headerpoint_p->Date = startdate;
            headerpoint_p->SampleNr = startsample;
            filenames_p = filenames;
            NumAntennas_p = numchannels;
            AntennaIDs_p = AntIDs;
            datasize_p = noSamples;


            // free the memory
            free(tmppoint);

            setStreams();
            setHeaderRecord();
          } catch (AipsError x) {
            cerr << "tbbctlIn:attachFile: " << x.getMesg() << endl;
            return False;
          }; 
				
          return True;
        }


  Bool tbbctlIn::setStreams(){
    try{
      int antenna;
      

      DataReader::setSampleFrequency(headerpoint_p->sampleFreq*1e6);

      if (iterator_p != NULL) { delete [] iterator_p; iterator_p=NULL; };
      iterator_p = new DataIterator[NumAntennas_p];
      
      for (antenna=0; antenna<NumAntennas_p; antenna++){
 	iterator_p[antenna].setDataStart(0);
 	iterator_p[antenna].setStride(0);
 	iterator_p[antenna].setShift(0);
 	//Set the width of a step to 1, so we count in samples:
 	iterator_p[antenna].setStepWidthToSample();
 	iterator_p[antenna].setBlocksize(datasize_p);
      }
      
      /*
	Setup of the conversion arrays
      */
      Vector<Double> adc2voltage(NumAntennas_p,1.);
      
      uint fftLength(datasize_p/2+1);      
      Matrix<DComplex> fft2calfft(fftLength,NumAntennas_p,1.0);
      
      DataReader::init(datasize_p,
		       adc2voltage,
		       fft2calfft);      
    } catch (AipsError x) {
      cerr << "tbbctlIn:setStreams: " << x.getMesg() << endl;
      return False;
    }; 
    return True;
    
  };

  // ------------------------------------------------------------ setHeaderRecord

  Bool tbbctlIn::setHeaderRecord(){
    try {
      // Mandatory header fields
      header_p.define("Date",headerpoint_p->Date);
      header_p.define("AntennaIDs",AntennaIDs_p);
      header_p.define("Observatory","LOFAR");
      header_p.define("Filesize",datasize_p);
      header_p.define("dDate",(Double)headerpoint_p->Date + 
		      (Double)headerpoint_p->SampleNr/(Double)headerpoint_p->sampleFreq/1e6);
      // special tbb-header fields
      header_p.define("SampleFreq",headerpoint_p->sampleFreq);
      header_p.define("StartSample",headerpoint_p->SampleNr);
    } catch (AipsError x) {
      cerr << "[tbbctlIn::setHeaderRecord] " << x.getMesg() << endl;
      return False;
    }; 
    return True;
  };

  //_______________________________________________________________________________
  //                                                                             fx

  /*!
    \return Matrix with the data, [sample,antenna]
  */
  Matrix<Double> tbbctlIn::fx ()
  {
    Matrix<Double> data (DataReader::blocksize(),
			 DataReader::nofSelectedAntennas());
    
    tbbctlIn::fx (data);
    
    return data;
  }
  
  //_______________________________________________________________________________
  //                                                                             fx
  
  /*!
    \retval Matrix with the data, [sample,antenna]
  */
  void tbbctlIn::fx (Matrix<Double> &data)
  {
    uint antenna;
    uint startsample;
    casa::IPosition shape (2,
			   DataReader::blocksize(),
			   DataReader::nofSelectedAntennas());
    uint nofSelectedAntennas (DataReader::nofSelectedAntennas());
    Matrix<Short> fx_short(shape);
    
    /* Check the shape of the array returning the data */
    if (data.shape() != shape) {
      data.resize(shape);
    }
    
    /* Retrieve the data */
    for (antenna=0; antenna<nofSelectedAntennas; antenna++) {
      startsample = iterator_p[selectedAntennas_p(antenna)].position();
      fx_short.column(antenna) = channeldata_p.column(selectedAntennas_p(antenna))(Slice(startsample,DataReader::blocksize()));
    };

    /* Convert the type of the array values */
    convertArray(data,fx_short);
  }
  
} // Namespace CR -- end
