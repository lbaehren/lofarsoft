/*
## PROGRAM STRUCTURE #########################################################
 Production of Dynamic Spectrum on Beamformed Waveform
 01) DataReader
 02) FFT
 03) ...
 XX) Save to FITS (optional with Glish return)
 YY) Get FileSize from DataReader
 ZZ) For multiple antennas in parallel
------------------------------------------------------------
cp /home/anigl/data/lopesgui/lopescasa/linux_gnu/bin/dstb /home/anigl/data/lopesgui/lopescasa/linux_gnu/bin/dstbfloat
cp /home/anigl/data/lopesgui/lopescasa/linux_gnu/bin/dstb /home/anigl/data/lopesgui/lopescasa/linux_gnu/bin/dstbshort
*/

//------------------------------------------------------------
// INCLUDE
#include <dstb.h>
#include <lopes/IO/DataReader.h>
#include <lopes/IO/DataReaderTools.h>

//------------------------------------------------------------
// MAIN
int main (int argc, char **argv)
{
  GlishSysEventSource glishStream(argc, argv);
  glishStream.addTarget(dstb,"dstb");
  glishStream.loop();
}

//------------------------------------------------------------
// LOCAL FUNCTIONS

//------------------------------------------------------------
// HEADER FUNCTIONS
Bool dstb (GlishSysEvent &event, void *)
{

  //------------------------------------------------------------
  // GLISH BUS

  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;
  GlishRecord record; // record for return
  Bool glish(True);

  // glish record types
  Vector<String> filenames;
  Int blocksize, nblocks, startblock, offset;
  Vector<Bool> fsel;
  Bool nods(False);
  
  try {
    
    // nods
    if (glishRec.exists("nods")) {
      gtmp = glishRec.get("nods");
      gtmp.get(nods);
    } else {
      cerr << "[dstb@glishbus] Missing field: nods" << endl;
      glish = False;
    }
    // filenames
    if (glishRec.exists("filenames")) {
      gtmp = glishRec.get("filenames");
      gtmp.get(filenames);
    } else {
      cerr << "[dstb@glishbus] Missing field: filenames" << endl;
      glish = False;
    }
    // blocksize
    if (glishRec.exists("blocksize")) {
      gtmp = glishRec.get("blocksize");
      gtmp.get(blocksize);
    } else {
      cerr << "[dstb@glishbus] Missing field: blocksize" << endl;
      glish = False;
    }
    // nblocks
    if (glishRec.exists("nblocks")) {
      gtmp = glishRec.get("nblocks");
      gtmp.get(nblocks);
    } else {
      cerr << "[dstb@glishbus] Missing field: nblocks" << endl;
      glish = False;
    }
    // startblock
    if (glishRec.exists("startblock")) {
      gtmp = glishRec.get("startblock");
      gtmp.get(startblock);
    } else {
      cerr << "[dstb@glishbus] Missing field: startblock" << endl;
      glish = False;
    }
    // offset
    if (glishRec.exists("offset")) {
      gtmp = glishRec.get("offset");
      gtmp.get(offset);
    } else {
      cerr << "[dstb@glishbus] Missing field: offset" << endl;
      glish = False;
    }
    // fsel
    if (glishRec.exists("fsel")) {
      gtmp = glishRec.get("fsel");
      gtmp.get(fsel);
    } else {
      cerr << "[dstb@glishbus] Missing field: fsel" << endl;
      glish = False;
    }
  } catch (AipsError x) {
    cerr << "[dstb@glishbus] " << x.getMesg() << "\n" << endl;
    glish = False;
  }

    //------------------------------------------------------------
    // PROGRAMM CORE
 
  if (glish) { // if glish bus worked well
    cout << "\n[dstb] nods: " << nods << endl;
    cout << "[dstb] filenames: " << filenames << endl;
    cout << "[dstb] blocksize: " << blocksize << endl;
    cout << "[dstb] nblocks: " << nblocks << endl;
    cout << "[dstb] startblock: " << startblock << endl;
    cout << "[dstb] offset: " << offset << endl;
    cout << "[dstb] fsel: " << fsel.shape() << "\n" << endl;
    
    //------------------------------------------------------------
    // DATA READER (read timeblock from raw data antennas)
    
cout << "[dstb] reading raw waveform data..." << endl;
    
    DataReader *dr = new DataReader;
    float alpha = 0.54;

    try {
      // EVENTFILES
      // DataReader<short> reader(EventFiles,files,blocksize);
      // reader.block(startblock);
      // reader.start(offset);
      
      // PROJECTFILES
      dr = DataReaderTools::assignFromRecord (glishRec,true);
      dr->setSelectedChannels(fsel);
      dr->setHanningFilter(alpha);
      
// cout << "[dstb] number of files loaded: " << dr.nofFiles() << endl;
// cout << "[dstb] number of valid channels: " << dr.nofValidChannels() << endl;
      
    } catch (AipsError x) {
      cerr << "[dstb@datareader] " << x.getMesg() << "\n" << endl;
      if (glishBus->replyPending()) {glishBus->reply(GlishArray(False));};
    }

//Matrix<Double> fx = dr.fx();
//Matrix<DComplex> calfft = dr.calfft();
//cout << "[dstb] fx: " << dr.fx().shape() << endl;
//for ( int i=0; i<5; i++ ) { cout << fx(i,0) << " "; }; cout << endl;
//cout << "[dstb] calfft: " << dr.calfft().shape() << endl;
//for ( int i=0; i<5; i++ ) { cout << calfft(i,0) << " "; }; cout << endl;
        
    //------------------------------------------------------------
    // BLOCK LOOP
    
    // loop parameter
    unsigned int freqlen = dr->nofSelectedChannels();

    // extra type definition for glishbus return
    Matrix<DComplex> ds(1,1,0);
    Vector<Double> avgspec(freqlen); avgspec = 0;

cout << "[dstb] looping over blocks... " << "\n" << endl;

    // progress bar
    LOPES::ProgressBar bar (nblocks,70); // widht = 70 pixels
    bar.showTime(false);
        
    // loop over blocks
    for ( unsigned int block=0; block<(unsigned int)nblocks; block++ ) {
      
      // type definition
      Matrix<DComplex> calfft(freqlen,
			      dr->nofSelectedAntennas());
      
      // reads data from disk
        try {
      calfft = dr->calfft();

/*
cout << "[dstb] calfft: " << calfft.shape() << endl;
for ( int i=0; i<5; i++ ) { cout << calfft(i,0) << " "; }; cout << endl;
cout << "[dstb] ds: " << ds.shape() << endl;
for ( int i=0; i<5; i++ ) { cout << ds(i,0) << " "; }; cout << endl;
*/
      // assemly of dynamic spectrum
      if (!nods) {
        ds.resize(nblocks,freqlen);
        ds.row(block) = calfft.column(0);
      } else {};
      // addition of spectra
      for ( unsigned int i=0; i<freqlen; i++ ) { avgspec(i) += abs(calfft(i,0)); }
      //avgspec += abs(calfft.column(0));

/*
cout << "[dstb] calfft: " << calfft.shape() << endl;
for ( int i=0; i<5; i++ ) { cout << calfft(i,0) << " "; }; cout << endl;
cout << "[dstb] avgspec: " << avgspec.nelements() << endl;
for ( int i=0; i<5; i++ ) { cout << avgspec(i) << " "; }; cout << endl;
*/
            
        } catch (AipsError x) {
          cerr << "[dstb@datareaderextraction] " << x.getMesg()
	       << " fx: " << dr->fx().shape()
               << " voltage: " << dr->voltage().shape()
	       << " calfft: " << dr->calfft().shape()
	       << endl;
        }
      // jump to next block in data from data reader
        try {  
      dr->nextBlock();
        } catch (AipsError x) {
          cerr << "[dstb@nextdatareaderblock] " << x.getMesg() << endl;
        }
      
    //------------------------------------------------------------
    // PROGRESS BAR UPDATE
    bar.update(block+1);

  //------------------------------------------------------------
  // END BLOCK LOOP
  } // end block loop

  // average spectra (division by number of spectra)
  for ( unsigned int i=0; i<freqlen; i++ ) { avgspec(i) /= nblocks; }

//cout << "[dstb] avgspec: " << avgspec.nelements() << endl;
//for ( int i=0; i<5; i++ ) { cout << avgspec(i) << " "; }; cout << endl;
  
  //------------------------------------------------------------
  // GLISH BUS RETURN
  
  cout << "\n[dstb] returning to glish..." << endl;
  
  record.add("absds",abs(ds)); // to avoid complex to double conversion by abs() in glish
  record.add("ds",ds);
  record.add("avgspec",avgspec);

  } else {record.add("glish",glish);} // end glishbus error

  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[dstb]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
    glish = False;
  }

  //------------------------------------------------------------
  // RETURN
  return glish;

} // end glish function
//------------------------------------------------------------
