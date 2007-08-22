//------------------------------------------------------------
// TOOLS
// ...lopescasa/
// cvs update -dP
// make build

//------------------------------------------------------------
// TOOLS
// cout << "Press enter please!" << endl; readln();

//------------------------------------------------------------
// INCLUDE
#include "dstb.h"

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

DataReader* drfeed (
  String &datatype,
  String &basefile,
  uint &blocksize //,
  //Matrix<Double> &adc2voltage,
  //Matrix<DComplex> &fft2calfft
)
{
  DataReader *dr = new DataReader();
  uint nyquistZone (1);
  Double samplingRate (80e06);

    if (datatype == "LopesEvent") {
      try {
        LopesEvent *le = new LopesEvent (
        //LopesEventIn *le = new LopesEventIn (
          basefile,
          blocksize); //,
          //adc2voltage,
          //fft2calfft);
        dr = le;
        nyquistZone = 2; //1=ITS&2=LOPES
      } catch (AipsError x) {
        cerr << "[DataReaderTools::assignFromRecord] LopesEvent: "
             << x.getMesg() << endl;
      }
    }
    else if (datatype == "ITSCapture") {
      try {
        ITSCapture *capture = new ITSCapture (
          basefile,
          blocksize); //,
          //adc2voltage,
          //fft2calfft);
        dr = capture;
      } catch (AipsError x) {
        cerr << "[DataReaderTools::assignFromRecord] ITSCapture: "
             << x.getMesg() << endl;
      }
    }
    else if (datatype == "ITSBeam") {
      try {
        ITSBeam *beam = new ITSBeam (
          basefile,
          blocksize); //,
          //adc2voltage,
          //fft2calfft);
        dr = beam;
      } catch (AipsError x) {
        cerr << "[DataReaderTools::assignFromRecord] ITSBeam: "
             << x.getMesg() << endl;
      }
    }
    else if (datatype == "NDABeam") {
      try {
        NDABeam *beam = new NDABeam (
          basefile,
          blocksize); //,
          //adc2voltage,
          //fft2calfft);
        dr = beam;
      } catch (AipsError x) {
        cerr << "[DataReaderTools::assignFromRecord] NDABeam: "
             << x.getMesg() << endl;
      }
    }
    else {
      cerr << "[DataReaderTools::assignFromRecord] Undefined data type!" << endl;
      return NULL;
    }
    dr->setNyquistZone(nyquistZone);
    dr->setSampleFrequency (samplingRate);

  return dr;
} // end function drfeed

// READLN();
void readln ()
{ 
  //cin.ignore(1000,'\n'); // clear the stream
  while(true)
  { //cout << "Press enter key";
    if(cin.peek() == '\n') // looks to see if the next character in the standard input stream is enter
    {
      cin.ignore(1000,'\n'); // clear the stream
      break; // break the loop
    } 
    else
    {
      cin.ignore(1000, '\n');
      continue;
    }
  } // end while(true) // cout << "while broken";
} // end readln() function
        
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
  Matrix<DComplex> fft2calfft;
  Matrix<Double> antpos, adc2voltage;
  Vector<String> filenames;
  Vector<Double> freq, azel;
  Vector<uint> antennas, antsel; //!!Int
  Vector<bool> freqsel;
  String datatype;
  Double alpha(1.0); //0.54;
  uint nfiles, nblocks(1), startblock(0), blocksize(65536), offset(0), intblockn(1); //!!Int
  bool nods(False);
  short method(1);

  // initialize
  freqsel.resize(1); freqsel = True;
  adc2voltage.resize(1,1); adc2voltage = 1.0; // reshape to [blocksize,antnum]
  fft2calfft.resize(1,1); fft2calfft = 1.0;

  // extract variables from glishrecord    
  try {
    
    // nods
    if (glishRec.exists("nods")) {
      gtmp = glishRec.get("nods");
      gtmp.get(nods);
    } else {
      cerr << "[nods@glishbus] Missing field: nods" << endl;
    }
    // datatype
    if (glishRec.exists("datatype")) {
      gtmp = glishRec.get("datatype");
      gtmp.get(datatype);
    } else {
      cerr << "[timebeam@glishbus] Missing field: datatype" << endl;
      glish = False;
    }
    // method
    if (glishRec.exists("method")) {
      gtmp = glishRec.get("method");
      gtmp.get(method);
    } else {
      cerr << "[timebeam@glishbus] Missing field: method" << endl;
    }
    // filenames
    if (glishRec.exists("filenames")) {
      gtmp = glishRec.get("filenames");
      gtmp.get(filenames);
    } else {
      cerr << "[filenames@glishbus] Missing field: filenames" << endl;
      glish = False;
    }
    // antennas
    {
      Vector<int> tmp;
      if (glishRec.exists("antennas")) {
        gtmp = glishRec.get("antennas");
        gtmp.get(tmp);
      } else {
        cerr << "[timebeam@glishbus] Missing field: antennas -> set to antenna 1" << endl;
        tmp.resize(1);
        tmp = 0;
      }
      uint nelem (tmp.nelements());
      antennas.resize (nelem);
      for (uint n(0); n<nelem; n++) { antennas(n) = tmp(n); }
    }
    // antsel
    {
      Vector<int> tmp;
      if (glishRec.exists("antsel")) {
        gtmp = glishRec.get("antsel");
        gtmp.get(tmp);
      } else {
        cerr << "[timebeam@glishbus] Missing field: antsel -> set to antenna 1" << endl;
        tmp.resize(1);
        tmp = 0;
      }
      uint nelem (tmp.nelements());
      antsel.resize (nelem);
      for (uint n(0); n<nelem; n++) { antsel(n) = (uint)tmp(n); }
    }
    // antpos
    if (glishRec.exists("antpos")) {
      gtmp = glishRec.get("antpos");
      gtmp.get(antpos);
    } else {
      cerr << "[antpos@glishbus] Missing field: antpos" << endl;
      glish = False;
    }
    // blocksize
    {
      int tmp = (int)blocksize;
      if (glishRec.exists("blocksize")) {
        gtmp = glishRec.get("blocksize");
        gtmp.get(tmp);
      } else {
        cerr << "[blocksize@glishbus] Missing field: blocksize -> set to 65536" << endl;
      }
      blocksize = (uint)tmp;
    }
    // nblocks
    {
      int tmp = (int)nblocks;
      if (glishRec.exists("nblocks")) {
        gtmp = glishRec.get("nblocks");
        gtmp.get(tmp);
      } else {
        cerr << "[nblocks@glishbus] Missing field: nblocks -> set to 1" << endl;
      }
      nblocks = (uint)tmp;
    }
    // startblock
    {
      int tmp = (int)startblock;
      if (glishRec.exists("startblock")) {
        gtmp = glishRec.get("startblock");
        gtmp.get(tmp);
      } else {
        cerr << "[startblock@glishbus] Missing field: startblock -> set to first block (0)" << endl;
      }
      startblock = (uint)tmp;
    }
    // offset
    {
      int tmp = (int)offset;
      if (glishRec.exists("offset")) {
        gtmp = glishRec.get("offset");
        gtmp.get(tmp);
      } else {
        cerr << "[offset@glishbus] Missing field: offset -> set to 0" << endl;
      }
      offset = (uint)tmp;
    }
    // intblockn
    {
      int tmp = (int)intblockn;
      if (glishRec.exists("intblockn")) {
        gtmp = glishRec.get("intblockn");
        gtmp.get(tmp);
      } else {
        cerr << "[intblockn@glishbus] Missing field: intblockn -> set to 1" << endl;
      }
      intblockn = (uint)tmp;
    }
    // freq
    if (glishRec.exists("freq")) {
      gtmp = glishRec.get("freq");
      gtmp.get(freq);
    } else {
      cerr << "[freq@glishbus] Missing field: freq" << endl;
      glish = False;
    }
    // freqsel
    if (glishRec.exists("freqsel")) {
      gtmp = glishRec.get("freqsel");
      gtmp.get(freqsel);
    } else {
      cerr << "[freqsel@glishbus] Missing field: freqsel -> set to all frequency channels" << endl;
    }
    // azel
    if (glishRec.exists("azel")) {
      gtmp = glishRec.get("azel");
      gtmp.get(azel);
    } else {
      cerr << "[azel@glishbus] Missing field: azel" << endl;
      glish = False;
    }
    // adc2voltage
    if (glishRec.exists("adc2voltage")) {
      gtmp = glishRec.get("adc2voltage");
      gtmp.get(adc2voltage);
    } else {
      cerr << "[adc2voltage@glishbus] Missing field: adc2voltage -> set to 1.0" << endl;
    }
    // fft2calfft
    if (glishRec.exists("fft2calfft")) {
      gtmp = glishRec.get("fft2calfft");
      gtmp.get(fft2calfft);
    } else {
      cerr << "[fft2calfft@glishbus] Missing field: fft2calfft -> set to 1.0" << endl;
    }
    // alpha
    if (glishRec.exists("alpha")) {
      gtmp = glishRec.get("alpha");
      gtmp.get(alpha);
    } else {
      cerr << "[alpha@glishbus] Missing field: alpha -> set to 1.0 (no change to data)" << endl;
    }
  } catch (AipsError x) {
    cerr << "[dstb@glishbus] " << x.getMesg() << "\n" << endl;
    glish = False;
  }

  //------------------------------------------------------------
  // PROGRAMM CORE
 
  if (glish) { // if glish bus works
    nfiles = filenames.nelements();
    uint freqlen = freq.nelements();
    uint nants = antsel.nelements();
    cout << endl;
    cout << "[dstb] nods: " << nods << endl;
    cout << "[dstb] datatype: " << datatype << endl;
    cout << "[dstb] method: " << method << endl;
    cout << "[dstb] filenames: [" << nfiles << "] " << filenames(0) << endl;
    cout << "[dstb] antennas: [" << antennas.nelements() << "] " << antennas(0) << endl;
    cout << "[dstb] antsel: [" << nants << "] " << antsel(0) << endl;
    cout << "[dstb] antpos: " << antpos.shape() << " " << antpos.row(0)  << endl; // first matrix index
    cout << "[dstb] blockinfo: [" << startblock << "," << nblocks << "," << blocksize << "]" << endl;
    cout << "[dstb] offset: " << offset << endl;
    cout << "[dstb] intblockn: " << intblockn << endl;
    cout << "[dstb] freqlen: " << freqlen << endl;
    cout << "[dstb] freqsel: [" << freqsel.nelements() << "] " << freqsel(0) << endl;
    cout << "[dstb] azel: " << azel.shape() << " " << azel << endl; //!! UPDATE FOR EACH FILE/BLOCK
    cout << "[dstb] adc2voltage: " << adc2voltage.shape() << " " << adc2voltage(0,0) << endl;
    cout << "[dstb] fft2calfft: " << fft2calfft.shape() << " " << fft2calfft(0,0) << endl;
    cout << "[dstb] alpha: " << alpha << endl;
    cout << endl;

    //------------------------------------------------------------
    // maximum memory check
    const short doublesize(8); //byte
    const double mem = nfiles*nblocks/intblockn*freqlen*doublesize/1e6;
    cout << "[dstb] memory: " << mem << " MB" << endl << endl;
    if (mem>1000) {return(0);} // maximum 2(4) GB per process

    //------------------------------------------------------------
    // extra type definition (for glishbus return)
    bfData *bfo = new bfData ();                                   //beamformerobject
    Vector<Double> avgspec(freqlen); avgspec = 0;                  //return
    Vector<Double> avgpwrc(nfiles*nblocks/intblockn); avgpwrc = 0; //return
    Matrix<Double> ds(1,1,0);                                      //return
      if (!nods) { ds.resize(nfiles*nblocks/intblockn,freqlen); }

cout << "[dstb] ds shape: " << ds.shape() << endl << endl;
    
    //------------------------------------------------------------
    // FILE LOOP

cout << "[dstb] looping over files... " << endl;
      
    // file progress bar (only one progress bar -> block bar)
    //CR::ProgressBar filebar (nfiles,70); // width = 70 pixels
    //filebar.showTime(false);
    
    for ( uint filen=0; filen<nfiles; filen++ ) {
      
cout << endl << "[dstb] file (number & name): [" << filen << "] " << filenames(filen) << endl;

      //------------------------------------------------------------
      // DATA READER (read timeblock from raw data antennas)
      DataReader *dr = new DataReader(); //datareaderobject
      
      try {
      
        dr = drfeed(datatype,filenames(filen),blocksize); //,adc2voltage,fft2calfft);
      
      } catch (AipsError x) {
        cerr << "[dstb@drfeed@datareader] " << x.getMesg() << "\n" << endl;
        if (glishBus->replyPending()) {glishBus->reply(GlishArray(False));}
      }
      
      try {

        dr->setBlocksize(blocksize);
        dr->setADC2Voltage(adc2voltage);
        dr->setFFT2calFFT(fft2calfft);
        dr->setStartBlock(startblock);
        dr->setStride(0);
        dr->setShift(offset);
        dr->setSelectedChannels(freqsel);
        dr->setHanningFilter(alpha);

/*
cout << "[dstb] nyquistZone: " << dr->nyquistZone() << endl;
cout << "[dstb] blocksize: " << dr->blocksize() << endl;
cout << "[dstb] adc2voltage: " << adc2voltage.shape() << " " << adc2voltage(0,0) << endl;
cout << "[dstb] fft2calfft: " << fft2calfft.shape() << " " << fft2calfft(0,0) << endl;
cout << "[dstb] number of selected antennas: " << dr->nofSelectedAntennas() << endl;
cout << "[dstb] selected antennas: " << dr->selectedAntennas() << endl;
cout << "[dstb] number of selected channels: " << dr->nofSelectedChannels() << endl;
*/
    
      } catch (AipsError x) {
        cerr << "[dstb@datareadersettings] " << x.getMesg() << "\n" << endl;
        if (glishBus->replyPending()) {glishBus->reply(GlishArray(False));}
      }

      try {
      //------------------------------------------------------------
      // AUTO-FLAGGING
      Matrix<Double> fx(blocksize,dr->nofSelectedAntennas());
      Matrix<DComplex> Cfx(blocksize,dr->nofSelectedAntennas());
      Matrix<DComplex> calfft(freqlen,dr->nofSelectedAntennas());
      Matrix<DComplex> Cflags(freqlen,dr->nofSelectedAntennas());
      RFIMitigation rfim;
      Int segments = 25;

      // time
      fx = dr->fx();
      convertArray(Cfx,fx);
 
/*   
cout << "realtocomplex: " << endl;
for ( uint i=0; i<5; i++ ) { cout << Cfx(i,0); }; cout << endl;
cout << "timeautoflags: " << endl;
Matrix<Double> Dflags(blocksize,dr->nofSelectedAntennas());
Dflags=rfim.getOptimizedSpectra(Cfx,blocksize,segments);
cout << "Dflags: " << Dflags.shape() << " " << Dflags(0,0) << endl;
for ( uint i=0; i<5; i++ ) { cout << Dflags(i,0); }; cout << endl;
adc2voltage *= Dflags
cout << "[dstb] fx: " << fx.shape() << " " << fx(0,0) << endl;
*/

      adc2voltage *= rfim.getOptimizedSpectra(Cfx,blocksize,segments);
      dr->setADC2Voltage(adc2voltage);
      
      // freq
      calfft = dr->calfft();
      convertArray(Cflags,rfim.getOptimizedSpectra(calfft,freqlen,segments));
      fft2calfft *= Cflags;
      dr->setFFT2calFFT(fft2calfft);

      //------------------------------------------------------------
      // ANTENNA SELECTION
      dr->setAntennas(antennas,antsel);
      
//cout << "[dstb] number of selected antennas: " << dr->nofSelectedAntennas() << endl;
//cout << "[dstb] selected antennas: " << dr->selectedAntennas() << endl;

      //------------------------------------------------------------
      } catch (AipsError x) {
        cerr << "[dstb@autoflagging] " << x.getMesg() << "\n" << endl;
        if (glishBus->replyPending()) {glishBus->reply(GlishArray(False));}
      }
      
      //------------------------------------------------------------
      // BLOCK LOOP
 
      // type definition
      Matrix<DComplex> calfft(freqlen,dr->nofSelectedAntennas());
      Matrix<DComplex> cbeam(freqlen,1); // 1 = only one direction
      uint blockc(0), intblockc(0);
  
      // block progress bar
      ProgressBar blockbar (nblocks,70); // width = 70 pixels
      blockbar.showTime(false);
      
      // loop over blocks
      for ( uint blockn=0; blockn<nblocks; blockn++ ) {

//cout << "filen: " << filen << " blockn: " << blockn << " blockc: " << blockc << " intblockc: " << intblockc;
//cout << " filen*nblocks/intblockn+blockc: " << filen*nblocks/intblockn+blockc << endl;
          
          try {
        // initialization
        cbeam = 0;
        
        // reads data from disk
        calfft = dr->calfft();

//dr->summary();
//cout << "calfft: "; for ( uint i=0; i<5; i++ ) { cout << calfft(i,0) << " "; }; cout << endl;
        
        // computes the beamformed spectrum
          try {
            
//cout << "bf.setPhaseGradients: " << freq.nelements() << azel.shape() << antpos.shape() << endl;

            bfo->bf.setPhaseGradients(freq,azel,antpos); //freq = dr->frequencies

/*
cout << "phasegradients: ";
for ( uint j=0; j<nants; j++ ) {
  for ( uint i=0; i<5; i++ ) { cout << bfo->bf.getPhaseGradients(i,0,j); }; cout << endl;
}
cout << endl;
cout << "antennapositions: " << bfo->bf.antennaPositions() << endl;
cout << "frequencyvalues: " << bfo->bf.frequencyValues().shape() << endl; //" " << bfo->bf.frequencyValues(0) << endl;
bfo->bf.summary();
*/
            
            bfo->bf.addAntennas(cbeam,calfft);

//cout << "cbeam: "; for ( uint i=0; i<5; i++ ) { cout << cbeam(i,0) << " "; }; cout << endl;
            
          } catch (AipsError x) {
            cerr << "[timebeam@beamforming] " << x.getMesg() << endl;
          }

        // beam-forming
        for ( uint i=0; i<freqlen; i++ ) {
        
          // assemly of dynamic spectrum
          if (!nods) {
            if (method==1) { // 1 - mean power & 2 - beamforming
              for (uint j=0;j<nants;j++) { ds(filen*nblocks/intblockn+blockc,i) += abs(calfft(i,j)); }
            } else {
              for (uint j=0;j<nants;j++) { ds(filen*nblocks/intblockn+blockc,i) += abs(cbeam(i,j)); } 
            } // endif method
          } // endif ds is wanted
          
          // addition of power spectra (average power spectrum)
          avgspec(i) += abs(cbeam(i,0));
          
          // addition of power per fileblock (lightcurve/powercurve)
          avgpwrc(filen*nblocks/intblockn+blockc) += abs(cbeam(i,0));
          //!!!??? average power cureve (division by number of frequency channels) ???PHYSICAL???
        
        } // end for i=0..freqlen

        if (intblockc==(intblockn-1)) {
          
          // averaging
          avgpwrc(filen*nblocks/intblockn+blockc) /= freqlen*intblockn;
            //if (!nods) { ds.row(filen*nblocks/intblockn+blockc) /= intblockn; }
          
//cout << "blockc++" << endl;

          // block counter increment
          blockc++; intblockc = 0;
        } else { intblockc++; } //cout << "intblockc++" << endl;

              
          } catch (AipsError x) {
            cerr << "[dstb@datareaderextraction] '" << x.getMesg()
                << "' fx: " << dr->fx().shape()
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
      // BLOCK PROGRESS BAR UPDATE
      blockbar.update(blockn+1);
  
    //------------------------------------------------------------
    // END BLOCK LOOP
    } // end block loop

    //------------------------------------------------------------
    // FILE PROGRESS BAR UPDATE
    //filebar.update(filen+1);
   
    //------------------------------------------------------------
    // DECONSTRUCTOR (close file streams)
    delete dr;
  
  //------------------------------------------------------------
  // END FILE LOOP
  } // end file loop
  
  //------------------------------------------------------------
  // DECONSTRUCTOR (close file streams)
  delete bfo;

  // dynamic spectrum (division by number of summed blocks)  
  //if (!nods) { ds /= intblockn; } //!!! IMPLEMENT AVERAGING OF NUMBER OF SUMMED BLOCKS !!!
  
  // average spectra (division by number of spectra)
  for ( uint i=0; i<freqlen; i++ ) { avgspec(i) /= nblocks; }

//cout << "[dstb] avgspec: " << avgspec.nelements() << endl;
//for ( uint i=0; i<5; i++ ) { cout << avgspec(i) << " "; }; cout << endl;
  
  //------------------------------------------------------------
  // GLISH BUS RETURN
  
  cout << endl << "[dstb] returning to glish..." << endl;

    try {
  cout << "[dstb] ds.shape(): " << ds.shape() << endl;
  record.add("absds",abs(ds)); // to avoid complex to double conversion by abs() in glish
  record.add("avgspec",avgspec);
  record.add("avgpwrc",abs(avgpwrc));
    } catch (AipsError x) {
      cerr << "[dstb@returnglishrecord] " << x.getMesg() << endl;
    }


  } else {record.add("glish",glish);} // end glishbus error

    try {  
  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[dstb] Unable to send back results; no pending reply." << endl;
    glish = False;
  }
    } catch (AipsError x) {
      cerr << "[dstb@answerpendingglishbus] " << x.getMesg() << endl;
    }

  //------------------------------------------------------------
  // RETURN
  return glish;

} // end glish function
//------------------------------------------------------------
