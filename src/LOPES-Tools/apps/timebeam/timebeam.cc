/*
## PROGRAM STRUCTURE #########################################################
 Calculation of Beamformed Waveform for parallel Antennas
 A1) blocking of antenna timeseries with blocksize (2^20 samples for time efficency)
     and a stride of twice the maximum beamforming delay
     (since after beamforming = shifting & adding the first and last part of the waveform is not usable / physical)
 A2) beamforming (by multiplication with phase gradient and summation in frequency domain)
 A3) application of hamming window on spectra
 A4) application of invFFT
 A5) extraction of usable / physical part for timebeam
 A6) assemgly of timeslices
 A7) storage to binary file
 A8) read binary float32 file in lopestools OK + creat header file []
 YY) Get FileSize from DataReader
## (NO) STRIDE TEST ##########################################################
#include 'timebeam_file.g'; timebeam(name='nostridetest',loc='/data/ITS/jup6/',ants=1,blockinfo=[1,-1,2^16],offset=0,maxdel=0,azel=[163.765127,44.3489562],bandlim=[22,30]*1e6,ret=F) # cp nostridetest.timebeam gtest/ & vi test.hdr
#include 'dstb_file.g'; dstb(name='nostridetest',loc='gtest/',ants=1,blockinfo=[1,-1,2^16],offset=0,bandlim=[22,32]*1e6,sav=F,sosdir='jsos/',bright=2,disp=T,ps=T,ret=F)
#timebeam.cc#for ( unsigned int ant=0; ant<dr.nofFiles(); ant++ ) { for ( unsigned int i=0; i<freqlen; i++ ) { calfft(i,ant)=0 }; calfft(ant,0)=Complex(1,1); };
#include 'timebeam_file.g'; timebeam(name='stridetest',loc='/data/ITS/jup6/',ants=1,blockinfo=[1,-1,80e5],offset=0,maxdel=0,azel=[163.765127,44.3489562],bandlim=[22,30]*1e6,ret=F) #1#bs=80e5 #2#ants=[1,2] #3#maxdel=100 #4#azel=[0,90]
#include 'dstb_file.g'; dstb(name='stridetest',loc='gtest/',ants=1,blockinfo=[1,-1,2^16],offset=0,bandlim=[22,32]*1e6,sav=F,sosdir='jsos/',bright=2,disp=T,ps=T,ret=F)
## QUESTIONS #################################################################
 1) progress bar (bar.showTime(false);)
 2) ClientData *clientData = new ClientData ();
 3) number handling and rounding difference in c and glish
------------------------------------------------------------
*/

//------------------------------------------------------------
// INCLUDE
#include <timebeam.h>

//------------------------------------------------------------
// DEFINE
#define pi 3.141592653 // http://en.wikipedia.org/wiki/Pi 3.141592653589793238462643383279502884197169399375105820974944592307816406286 (75digits)
#define lightspeed 299792458 // http://physics.nist.gov/cgi-bin/cuu/Value?c

//------------------------------------------------------------
// DATA STRUCTURE
ClientData *clientData = new ClientData ();

//------------------------------------------------------------
// MAIN
int main (int argc, char **argv)
{
  GlishSysEventSource glishStream(argc, argv);
  glishStream.addTarget(timebeam,"timebeam");
  glishStream.loop();
}

//------------------------------------------------------------
// LOCAL FUNCTIONS

// ----------------------------------------------------------- applyHammingWindow

void applyHammingWindow (Vector<DComplex>& freqblock)
{
  float alpha=0.54;
  int freqlen (freqblock.nelements());
  
  for (int fpos=0; fpos<freqlen; fpos++ ) {
    freqblock(fpos) *= (alpha-(1-alpha)*cos(2*pi*float(fpos)/(float(freqlen)-1)));
  }
}

// ----------------------------------------------------------- applyHammingWindow

void applyHanningWindow (Vector<DComplex>& freqblock,
			 double& alpha)
{
  int freqlen (freqblock.nelements());
  
  for (int fpos=0; fpos<freqlen; fpos++ ) {
    freqblock(fpos) *= (alpha-(1-alpha)*cos(2*pi*double(fpos)/(double(freqlen)-1)));
  }
}

// ----------------------------------------------------------------------- FFT2Fx

Vector<Double> FFT2Fx (Vector<DComplex>& fft,
		       Int const &fftsize)
{
  Vector<Double> fx (fftsize);
  
  FFTServer<Double,DComplex> server(IPosition(1,fftsize),
				    FFTEnums::REALTOCOMPLEX);
  
  server.fft(fx,fft);
  
  return fx;
}

//------------------------------------------------------------
// HEADER FUNCTIONS

Bool timebeam (GlishSysEvent &event, void *)
{
  
  //------------------------------------------------------------
  // GLISH BUS
  
  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;
  GlishRecord record; // record for return
  Bool glish(True);
  
  // glish record types
  Vector<String> files;
  String filename;
  Int blocksize;
  Int nblocks;
  Int offset;
  Vector<Double> freq,azel(2);
  Matrix<Double> antpos;
  Vector<Bool> fsel;
  Int maxdel;
  
  try {
    
    // Files
    if (glishRec.exists("files")) {
      gtmp = glishRec.get("files");
      gtmp.get(files);
    } else {
      cerr << "[timebeam@glishbus] Missing field: Files" << endl;
      glish = False;
    }
    // filename
    if (glishRec.exists("filename")) {
      gtmp = glishRec.get("filename");
      gtmp.get(filename);
    } else {
      cerr << "[timebeam@glishbus] Missing field: filename" << endl;
      glish = False;
    }
    // antpos
    if (glishRec.exists("antpos")) {
      gtmp = glishRec.get("antpos");
      gtmp.get(antpos);
    } else {
      cerr << "[timebeam@glishbus] Missing field: antpos" << endl;
      glish = False;
    }
    // blocksize
    if (glishRec.exists("blocksize")) {
      gtmp = glishRec.get("blocksize");
      gtmp.get(blocksize);
    } else {
      cerr << "[timebeam@glishbus] Missing field: blocksize" << endl;
      glish = False;
    }
    // offset
    if (glishRec.exists("offset")) {
      gtmp = glishRec.get("offset");
      gtmp.get(offset);
    } else {
      cerr << "[timebeam@glishbus] Missing field: offset" << endl;
      glish = False;
    }
    // nblocks
    if (glishRec.exists("nblocks")) {
      gtmp = glishRec.get("nblocks");
      gtmp.get(nblocks);
    } else {
      cerr << "[timebeam@glishbus] Missing field: nblocks" << endl;
      glish = False;
    }
    // freq
    if (glishRec.exists("freq")) {
      gtmp = glishRec.get("freq");
      gtmp.get(freq);
    } else {
      cerr << "[timebeam@glishbus] Missing field: freq" << endl;
      glish = False;
    }
    // azel
    if (glishRec.exists("azel")) {
      gtmp = glishRec.get("azel");
      gtmp.get(azel);
    } else {
      cerr << "[timebeam@glishbus] Missing field: azel" << endl;
      glish = False;
    }
    // fsel
    if (glishRec.exists("fsel")) {
      gtmp = glishRec.get("fsel");
      gtmp.get(fsel);
    } else {
      cerr << "[timebeam@glishbus] Missing field: fsel" << endl;
      glish = False;
    }
    // maxdel
    if (glishRec.exists("maxdel")) {
      gtmp = glishRec.get("maxdel");
      gtmp.get(maxdel);
    } else {
      cerr << "[timebeam@glishbus] Missing field: maxdel" << endl;
      glish = False;
    }
  } catch (AipsError x) {
    cerr << "[timebeam@glishbus] " << x.getMesg() << "\n" << endl;
    glish = False;
  }

    //------------------------------------------------------------
    // PROGRAMM CORE
 
  if (glish) { // if glish bus worked well
    cout << endl;
    cout << "[timebeam] filename   : " << filename << endl;
    cout << "[timebeam] file names : " << files.shape() << endl;
    cout << "[timebeam] antpos     : " << antpos.shape() << endl;
    cout << "[timebeam] blocksize  : " << blocksize << endl;
    cout << "[timebeam] offset     : " << offset << endl;
    cout << "[timebeam] nblocks    : " << nblocks << endl;
    cout << "[timebeam] freq       : " << freq.shape() << endl;
    cout << "[timebeam] azel       : " << azel.shape() << endl;
    cout << "[timebeam] fsel       : " << fsel.shape() << endl;
    cout << "[timebeam] maxium antenna delay: " << maxdel << "\n" << endl;
    
    //------------------------------------------------------------
    // DATA READER (read timeblock from raw data antennas)
    
cout << "[timebeam] reading raw antenna data..." << endl;
    
    DataReader *dr;
    dr = DataReaderTools::assignFromRecord (glishRec,true);
    
    try {
      
      dr->setStride(-2*maxdel);
      dr->setSelectedChannels(fsel);
      
    } catch (AipsError x) {
      cerr << "[timebeam@datareader] " << x.getMesg() << "\n" << endl;
      if (glishBus->replyPending()) {glishBus->reply(GlishArray(False));};
    }

    //------------------------------------------------------------
    // BLOCK LOOP
    
    // loop parameter
    unsigned int freqlen = dr->nofSelectedChannels();
    
    // extra type definition for glishbus return
    //unsigned int blocklen = blocksize-2*maxdel;
    //Vector<Float> timebeamblock(blocklen);
    unsigned int flen_ne = 0, flen_bl = 0, bytes = 0;
    
    // opens binary output file for timebeam
    String ending=".timebeam";
    char *binfilename = LOPES::string2char(filename+ending);
    
    cout << "[timebeam] opens & creates binary file: " << binfilename << endl;
    
    FILE *fp = fopen(binfilename,"w"); // w = open only for write, create if not existing
    // a = appending (file neednot exist)
    if (fp == NULL) { // error handling
      cout << "file can not be opened: " << binfilename << endl;
      if (glishBus->replyPending()) {glishBus->reply(GlishArray(False));};
    };
    
cout << "[timebeam] looping over blocks... " << "\n" << endl;

    // progress bar
    LOPES::ProgressBar bar (nblocks,70); // widht = 70 pixels
    bar.showTime(false);
        
    // loop over blocks
    for ( unsigned int block=1; block<=(unsigned int)nblocks; block++ ) {
    
     // type definition
      Matrix<DComplex> calfft(freqlen,dr->nofSelectedAntennas());
      Matrix<DComplex> beam;
      Vector<DComplex> freqblock(freqlen);
      
      // reads data from disk
      try {
	calfft = dr->calfft();
	
/*      
for ( int ant=0; ant<dr.nofFiles(); ant++ ) {
  for ( unsigned int i=0; i<freqlen; i++ ) { calfft(i,ant)=0; };
  calfft(0,ant)=Complex(1,1);
};
*/

/*
Matrix<Float> fx = dr.fx();
Matrix<Float> voltage = dr.voltage();
Matrix<Complex> fft = dr.fft();
cout << "[timebeam] fx: " << dr.fx().shape() << endl;
for ( int i=0; i<5; i++ ) { cout << fx(i,0) << " "; }; cout << endl;
cout << "[timebeam] voltage: " << dr.voltage().shape() << endl;
for ( int i=0; i<5; i++ ) { cout << voltage(i,0) << " "; }; cout << endl;
cout << "[timebeam] fft: " << dr.fft().shape() << endl;
for ( int i=0; i<5; i++ ) { cout << fft(i,0) << " "; }; cout << endl;
cout << "[timebeam] calfft: " << dr.calfft().shape() << endl;
for ( int i=0; i<5; i++ ) { cout << calfft(i,0) << " "; }; cout << endl;
*/
	
      } catch (AipsError x) {
	cerr << "[timebeam@datareaderextraction] " << x.getMesg() << endl;
	cerr << " calfft  : " << dr->calfft().shape()  << endl;
	cerr << " voltage : " << dr->voltage().shape() << endl;
	cerr << " fx      : " << dr->fx().shape()      << endl;
      }
      // computes the beamformed spectrum
      try {
	clientData->bf.setPhaseGradients(freq,azel,antpos);
	clientData->bf.addAntennas(beam,calfft);
      } catch (AipsError x) {
	cerr << "[timebeam@beamforming] " << x.getMesg() << endl;
      }
      // extract beam spectrum
      try {
	freqblock = beam.column(0);
      } catch (AipsError x) {
	cerr << "[timebeam@blockextraction] " << x.getMesg() << endl;
      }
      // jump to next block in data from data reader
      try {  
	dr->nextBlock();
      } catch (AipsError x) {
	cerr << "[timebeam@nextdatareaderblock] " << x.getMesg() << endl;
      }
      
      //------------------------------------------------------------
      // HAMMING WINDOW
      
      //applyHammingWindow(freqblock);
      double alpha = 0.54;
      applyHanningWindow(freqblock,alpha);
      
      //------------------------------------------------------------
      // INV FFT
      
      Int timelen = (freqlen-1)*2;
      IPosition tvecshape(1,timelen);
      Vector<Double> timeblock(tvecshape);
      
        try {
      //timeblock = LOPES::FFT2Fx(freqblock,timelen,False,False); // F/T=freqinv/no, F/T=FFT/inv
	  timeblock = FFT2Fx (freqblock,timelen);
        } catch (AipsError x) {
          cerr << "[timebeam@invFFT] " << x.getMesg() << "\n" << endl;
          timeblock = 0;
        }

    //------------------------------------------------------------
    // BLOCK EXTRACTION

    unsigned int blocklen = tvecshape(0)-2*maxdel;
    Vector<Double> timebeamblock(blocklen);

        try {
      timebeamblock = timeblock(Slice(maxdel,blocklen));
        } catch (AipsError x) {
          cerr << "[timebeam@timebeam] " << x.getMesg() << "\n" << endl;
          timebeamblock = 0;
        }
	
	//------------------------------------------------------------
	// WRITE TO BINARY FILE (float32)
	// od -t fF -N 12 gtest.bin
	// readdat('gtest/gtest.bin',offset=0,blocksize=2^16,type='float32le')[1:3]
	
	try {
	  
	  Bool deleteit;
	  double *tmp32point; // for i386 linux
	  
	  tmp32point = timebeamblock.getStorage(deleteit);
	  fwrite(tmp32point, sizeof(*tmp32point), blocklen, fp);
	  
flen_ne += timebeamblock.nelements();
flen_bl += blocklen;
bytes += sizeof(*tmp32point);
//cout << "[timebeam] timebeamblock: " << timebeamblock.nelements() << endl;
//cout << "[timebeam] blocklen: " << blocklen << " " << len << endl;
//cout << "[timebeam] filelen: " << filelen << endl;
      
      } catch (AipsError x) {
        cerr << "[timebeam@writebinaryfile] " << x.getMesg() << "\n" << endl;
      }
  
    //------------------------------------------------------------
    // PROGRESS BAR UPDATE
    bar.update(block+1);

  //------------------------------------------------------------
  // END BLOCK LOOP
  } // end block loop
      
  // close binary file
  fclose(fp);
   
  //------------------------------------------------------------
  // GLISH BUS RETURN

  cout << "\n[timebeam] returning to glish..." << endl;
  
record.add("flen_ne",(int)flen_ne);
record.add("flen_bl",(int)flen_bl);
record.add("bytes",(int)bytes);
//record.add("timebeamblock",timebeamblock);

  } else {record.add("glish",glish);} // end glishbus error

  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[timebeam]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
    glish = False;
  }

  //------------------------------------------------------------
  // RETURN
  return glish;

} // end glish function
//------------------------------------------------------------

/*
#######
# ADD #
#######
# BINFILE HEADER FILE #
#capturesize: S16_512M
#antennas: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59
#Date: 2005/11/17/20:00:00.00
#Files: gtest.bin
*/
