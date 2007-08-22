/*
------------------------------------------------------------
 COURSE OF ACTION
 1) read FITS from FITS-file with filename OK
    + build complex array
 2) extract metainfo & spectra from FITS OK
 3) apply hamming window on spectra OK
 4) apply invFFT OK
 5) extract usable part for timebeam OK
 6) assemble timeslices OK
 7) write to binary file OK
 Y) read & write blockwise OK
-1) extract blocks overlapping with "maxbaseline"
 X) read multiple files in memory and beamform them
 Z) acor & xcor
------------------------------------------------------------
 QUESTIONS
 1) progress bar
 2) number handling and rounding difference in c and glish
 3) changed: data-kern.g, data-BEAM.g
*/

//------------------------------------------------------------
// INCLUDE
#include <fits2wf.h>

//------------------------------------------------------------
// DEFINE
#define pi 3.141592653 // http://en.wikipedia.org/wiki/Pi 3.141592653589793238462643383279502884197169399375105820974944592307816406286 (75digits)
#define lightspeed 299792458 // http://physics.nist.gov/cgi-bin/cuu/Value?c

//------------------------------------------------------------
// MAIN
int main (int argc, char **argv)
{  
  GlishSysEventSource glishStream(argc, argv);
  glishStream.addTarget(readFITS, "readFITS");
  glishStream.loop();
}

//------------------------------------------------------------
// LOCAL FUNCTIONS
/*
float readFITSdata (char& filename, long& npixels) {
  float buffer[npixels];
  long fpixel(1)
  int anynull;
  float nullval(0);
  fitsfile *fptr;
  int status(0);
    if ( fits_open_file(&fptr, filename, READONLY, &status) )
      fits_report_error(stderr, status); // print error report
    if ( fits_read_img(fptr, TFLOAT, fpixel, npixels, &nullval, buffer, &anynull, &status) )
      fits_report_error(stderr, status); // print error report
    // reading first buffer[1:10]:=fits[1:10,1]...
    if ( fits_close_file(fptr, &status) )
      fits_report_error(stderr, status); // print error report
    return buffer;
}
*/

//------------------------------------------------------------
// HEADER FUNCTIONS
Bool readFITS (GlishSysEvent &event, void *)
{

  //------------------------------------------------------------
  // GLISH BUS VARIABLES

  GlishSysEventSource *glishBus = event.glishSource();
  GlishRecord glishRec = event.val();
  GlishArray gtmp;
  GlishRecord record; // record for return
  Bool glish(True);

  String FITSfilename;
  Int maxdel;
try {
    if (glishRec.exists("FITSfilename")) {
      gtmp = glishRec.get("FITSfilename");
      gtmp.get(FITSfilename);
    } else {
      cerr << "[fits2wf@glishbus] Missing field: FITSfilename" << endl;
      glish = False;
    }
    if (glishRec.exists("maxdel")) {
      gtmp = glishRec.get("maxdel");
      gtmp.get(maxdel);
    } else {
      cerr << "[fits2wf@glishbus] Missing field: maxdel" << endl;
      glish = False;
    }
} catch (AipsError x) {
  cerr << "[fits2wf@glishbus] " << x.getMesg() << "\n" << endl;
  glish = False;
}
if (glish) {

  cout << "\n[fits2wf] FITS file name: " << FITSfilename << endl;
  cout << "[fits2wf] maxium antenna delay: " << maxdel << "\n" << endl;

  //------------------------------------------------------------
  // FITS HEADER

cout << "[fits2wf] reading FITS header..." << endl;

  // open file
  fitsfile *hfptr; // pointer to the FITS file, defined in fitsio.h
  String ending=".fits";
  char *filename = LOPES::string2char(FITSfilename+ending); // name of existing FITS file
  int status(0);

try {

    if ( fits_open_file(&hfptr, filename, READONLY, &status) )
      fits_report_error(stderr, status); // print error report

} catch (AipsError x) {
  cerr << "[fits2wf@openfits] " << x.getMesg() << "\n" << endl;
  if (glishBus->replyPending()) {glishBus->reply(GlishArray(False));};
}

  // read header
  long naxis[2];
  int nfound; 
  float crpix[2]; // Reference pixel Frequency                   
  float crval[2]; // Reference value Frequency                      
  float cdelt[2]; // Coordinate increment Frequency                 
  char *comment = NULL;

try {

    if ( fits_read_keys_lng(hfptr, "NAXIS", 1, 2, naxis, &nfound, &status) )
      fits_report_error(stderr, status); // print error report
    if ( fits_read_key(hfptr, TFLOAT, "CRPIX1", &crpix[0], comment, &status) )
      fits_report_error(stderr, status); // print error report
    if ( fits_read_key(hfptr, TFLOAT, "CRPIX2", &crpix[1], comment, &status) )
      fits_report_error(stderr, status); // print error report
    if ( fits_read_key(hfptr, TFLOAT, "CRVAL1", &crval[0], comment, &status) )
      fits_report_error(stderr, status); // print error report
    if ( fits_read_key(hfptr, TFLOAT, "CRVAL2", &crval[1], comment, &status) )
      fits_report_error(stderr, status); // print error report
    if ( fits_read_key(hfptr, TFLOAT, "CDELT1", &cdelt[0], comment, &status) )
      fits_report_error(stderr, status); // print error report
    if ( fits_read_key(hfptr, TFLOAT, "CDELT2", &cdelt[1], comment, &status) )
      fits_report_error(stderr, status); // print error report

} catch (AipsError x) {
  cerr << "[fits2wf@fitsreadheader] " << x.getMesg() << "\n" << endl;
  crpix[0] = crval[0] = cdelt[0] = 0;
  crpix[1] = crval[1] = cdelt[1] = 0;
  naxis[0] = naxis[1] = 0;
}

  // close file
  if ( fits_close_file(hfptr, &status) )
    fits_report_error(stderr, status); // print error report

  //------------------------------------------------------------
  // AXIS VALUES
  float fvals[naxis[1]], tvals[naxis[0]];
  for ( int i=0; i<naxis[0]; i++ ) {tvals[i]=i*cdelt[0]+crval[0];}
  for ( int i=0; i<naxis[1]; i++ ) {fvals[i]=i*cdelt[1]+crval[1];}
//??? for ( int i=0; i<3; i++ ) { cout << fvals[i] << " "; }; cout << endl; // (40e6/2048*(0:2047)+40e6)[1:3]
  
  //------------------------------------------------------------
  // DISPLAY
  cout << "[fits2wf] matrix dimensions [naxis] (freq, time): " << naxis[1] << ", " << naxis[0] << endl;
  cout << "[fits2wf] reference pixel [crpix1] (freq, time): " << crpix[1] << ", " << crpix[0] << endl;
  cout << "[fits2wf] reference value [crval1] (freq, time): " << crval[1] << "Hz, " << crval[0] << "s" << endl;
  cout << "[fits2wf] coordinate increment [cdelt] (freq, time): " << cdelt[1] << "Hz, " << cdelt[0] << "s" << "\n" << endl;

  //------------------------------------------------------------
  //------------------------------------------------------------
  //------------------------------------------------------------
  // BLOCK LOOP

  // extra type definition for glishbus return
  IPosition cvecshape(1,naxis[1]);
  Vector<DComplex> freqblock(cvecshape);
  long timelen = (naxis[1]-1)*2;
  IPosition tvecshape(1,timelen);
  Vector<Double> timeblock(tvecshape);
  long blocklen = tvecshape(0)-2*long(maxdel);
    tvecshape = blocklen;
  Vector<Double> timebeamblock(tvecshape);
  
cout << "[fits2wf] looping over blocks... " << "\n" << endl;
  
  // type definition
  long fpixel[2], lpixel[2], inc[2]; inc[0] = inc[1] = 1;
  int anynull;
  float nullval(0);
  float *absval = new float[naxis[1]];
  float *argval = new float[naxis[1]];

  // opens binary output file for timebeam
    ending=".bin";
  filename = LOPES::string2char(FITSfilename+ending); // name of existing FITS file
  
cout << "[fits2wf] opens & creates binary file: " << filename << endl;

  FILE *fp = fopen(filename,"a"); // a = appending (file neednot exist)
if (fp == NULL) { // error handling
  cout << "file can not be opened: " << filename << endl;
  if (glishBus->replyPending()) {glishBus->reply(GlishArray(False));};
};

  // opens fitsfiles for reading of data
  fitsfile *absfptr, *argfptr; // pointer to the FITS file, defined in fitsio.h

try {
    
    ending="-abs.fits";
    filename = LOPES::string2char(FITSfilename+ending); // name of existing FITS file
    
cout << "[fits2wf] opens fits file with absolute values: " << filename << endl;

    if ( fits_open_file(&absfptr, filename, READONLY, &status) )
      fits_report_error(stderr, status); // print error report
    ending="-arg.fits";
    filename = LOPES::string2char(FITSfilename+ending); // name of existing FITS file
    
cout << "[fits2wf] opens fits file with phase values: " << filename << "\n" << endl;

    if ( fits_open_file(&argfptr, filename, READONLY, &status) )
      fits_report_error(stderr, status); // print error report

} catch (AipsError x) {
  cerr << "[fits2wf@fitsdata] " << x.getMesg() << "\n" << endl;
  if (glishBus->replyPending()) {glishBus->reply(GlishArray(False));};  
}
        
  // loop over blocks
  for ( long block=1; block<=naxis[0]; block++ ) {
//  for ( long block=1; block<=1; block++ ) {
    
cout << "[fits2wf] block: " << block << endl;

    // subset coordinates  
    fpixel[0] = lpixel[0] = block;
    fpixel[1] = 1;
    lpixel[1] = naxis[1];

    //------------------------------------------------------------
    // FITS DATA

//! cout << "[fits2wf] reading FITS data [(r1,r2):(c1,c2)]: (" << fpixel[0] << "," << lpixel[0] << "):(" << fpixel[1] << "," << lpixel[1] << ")" << "\n" << endl;
  
try {

    // read absolute values
//    if ( fits_read_img(absfptr, TFLOAT, fpixel, npixels, &nullval, absval, &anynull, &status) )
    if ( fits_read_subset(absfptr, TFLOAT, fpixel, lpixel, inc, &nullval, absval, &anynull, &status) )
      fits_report_error(stderr, status); // print error report

    // read phase values
//    if ( fits_read_img(argfptr, TFLOAT, fpixel, npixels, &nullval, argval, &anynull, &status) )
    if ( fits_read_subset(argfptr, TFLOAT, fpixel, lpixel, inc, &nullval, argval, &anynull, &status) )
      fits_report_error(stderr, status); // print error report
    
} catch (AipsError x) {
  cerr << "[fits2wf@fitsdata] " << x.getMesg() << "\n" << endl;
  absval = argval = 0;
}

    //------------------------------------------------------------
    // COMPLEX VALUES

//! cout << "[fits2wf] building complex matrix..." << endl;

/* !!!
for ( int i=0; i<13; i++ ) { cout << absval[i] << " "; }; cout << endl;
int fpos = 1, cout << absval[fpos] << " " << argval[fpos] << " " << cos(argval[fpos]) << " " << absval[fpos]*cos(argval[fpos]) << " " << absval[fpos]*sin(argval[fpos]) << "\n" << endl;
*/

//#    IPosition cvecshape(1,naxis[1]);
//#    Vector<Complex> freqblock(cvecshape);
  
      for ( int fpos=0; fpos<naxis[1]; fpos++ ) {
          freqblock(fpos)=Complex(absval[fpos]*cos(argval[fpos]),absval[fpos]*sin(argval[fpos]));
      } // end freq

//!!! for ( int i=0; i<3; i++ ) { cout << freqblock[i] << " "; }; cout << endl;
      
/* !!!
absval:=imagefromfits(infile='gtest-abs.fits').getchunk(); argval:=imagefromfits(infile='gtest-arg.fits').getchunk(); fitsary:=array(0.,2,shape(absval)[1],shape(absval)[2]); fitsary[1,,]:=absval*cos(argval); fitsary[2,,]:=absval*sin(argval); cfitsary:=array(0.i,shape(absval)[1],shape(absval)[2]); cfitsary:=complex(absval*cos(argval),absval*sin(argval))
*/

//! cout << "[fits2wf] shape of complex array: " << freqblock.shape() << "\n" << endl;

    //------------------------------------------------------------
    // HAMMING WINDOW

//! cout << "[fits2wf] applying hamming window... " << endl;

    float alpha=0.54;
      for ( int fpos=0; fpos<naxis[1]; fpos++ ) {
        freqblock(fpos)=freqblock(fpos)*(alpha-(1-alpha)*cos(2*pi*float(fpos)/(float(naxis[1])-1)));
      } // end freq

//!!! for ( int i=0; i<3; i++ ) { cout << freqblock[i] << " "; }; cout << endl;

/* ???
    freqblock = HanningFilter::applyFilter(freqblock);
*/

/* !!!
hlp:=array(0.,shape(cfitsary)[1],shape(cfitsary)[2]); for (i in 1:shape(cfitsary)[1]) {hlp[i,]:=cfitsary[i,]*hanning(shape(cfitsary)[2],alpha=.54)}; hlp[1,1:3]; res.freqblock[1:3]
*/

    //------------------------------------------------------------
    // INV FFT

//! cout << "[fits2wf] applying inverse FFT... " << endl;
      
//#    long timelen = (naxis[1]-1)*2;
//#    IPosition tvecshape(1,timelen);
//#    Vector<Float> timeblock(tvecshape);
      
      try {
	timeblock = LOPES::FFT2Fx<Double,DComplex>(freqblock,timelen,True,False);
      } catch (AipsError x) {
	cerr << "[fits2wf@invFFT] " << x.getMesg() << "\n" << endl;
	timeblock = 0;
      }
      
    //------------------------------------------------------------
    // BLOCK EXTRACTION

//! cout << "[fits2wf] extracting usable time block with maxdel: " << maxdel << endl;

//#    long blocklen = tvecshape(0)-2*long(maxdel);
//#      tvecshape = blocklen;
//#    Vector<Float> timebeamblock(tvecshape);

try {

/* !!!
cout << timeblock.shape() << " - 2*" << maxdel << " = " << tvecshape(0)-2*maxdel << endl;
*/
  timebeamblock = timeblock(Slice(maxdel,blocklen));

} catch (AipsError x) {
  cerr << "[fits2wf@timebeam] " << x.getMesg() << "\n" << endl;
  timebeamblock = 0;
}

    //------------------------------------------------------------
    // WRITE TO BINARY FILE (float32)
    // od -t fF -N 12 gtest.bin
    // readdat('gtest/gtest.bin',offset=0,blocksize=2^16,type='float32le')[1:3]

//! cout << "[fits2wf] writing (float32) to binary file - blocksize: " << blocklen << "\n" << endl;

try {

    Bool deleteit;
    unsigned int len = blocklen;
    double *tmp32point; // for i386 linux
      tmp32point = timebeamblock.getStorage(deleteit);
      fwrite(tmp32point, sizeof(*tmp32point), len, fp);

} catch (AipsError x) {
  cerr << "[fits2wf@writebinaryfile] " << x.getMesg() << "\n" << endl;
}
  
  //------------------------------------------------------------
  // END BLOCK LOOP
  } // end block loop
      
  // close binary file
  fclose(fp);
  
  // close fits files
  if ( fits_close_file(absfptr, &status) )
    fits_report_error(stderr, status); // print error report
  if ( fits_close_file(argfptr, &status) )
    fits_report_error(stderr, status); // print error report
    
  // free memory
  delete [] absval;
  delete [] argval;
  
  //------------------------------------------------------------
  // GLISH BUS RETURN

cout << "\n[fits2wf] Returning last timebeamblock with size: " << timebeamblock.shape() << endl;
  
  IPosition fshape(1), tshape(1);
  fshape=naxis[1];
  tshape=naxis[0];
  Vector<Float> fvec(fshape,fvals);
  Vector<Float> tvec(tshape,tvals);

  record.add("freqblock",freqblock);
  record.add("timeblock",timeblock);
  record.add("timebeamblock",timebeamblock);
  record.add("fvec",fvec);
  record.add("tvec",tvec);

} else {record.add("glish",glish);} // end glishbus error

  if (glishBus->replyPending()) {
    glishBus->reply(record);
  } else {
    cerr << "[fits2wf]" << endl;
    cerr << "\tUnable to send back results; no pending reply." << endl;
    glish = False;
  }

  //------------------------------------------------------------
  // RETURN
  return glish;

} // end glish function
//------------------------------------------------------------


/*
###############
# FITS HEADER #
  ############### (od -s -N 1683 test.fits)

1777777777777777777777
SIMPLE  =                    T / file does conform to FITS standard             BITPIX  =                  -32 / number of bits per data pixel                  
NAXIS   =                    2 / number of data axes                           
NAXIS1  =                   10 / length of data axis 1                         
NAXIS2  =                31130 / length of data axis 2                         
EXTEND  =                    T / FITS dataset may contain extensions            
COMMENT   FITS (Flexible Image Transport System) format is defined in 'AstronomyCOMMENT and Astrophysics', volume 376, page 359; bibcode: 2001A&A...376..359H
TELESCOP= 'LOFAR-ITS'              
INSTRUME= 'LOFAR-ITS'                    
OBSERVER= 'Andreas Nigl'                       
CTYPE1  = 'Time    '           / Frequency axis      
CTYPE2  = 'FREQ    '           / Time axis                 
CUNIT1  = 's       '           / Frequency axis units            
CUNIT2  = 'Hz      '           / Time axis units                       
CRPIX1  =                   1. / Reference pixel Frequency                   
CRPIX2  =                   1. / Reference pixel Time                           
CRVAL1  =         0.0004096063 / Reference value Frequency                      
CRVAL2  =                   0. / Reference value Time                           
CDELT1  =            0.0008192 / Coordinate increment Frequency                 
CDELT2  =             1220.703 / Coordinate increment Time
END
*/

/*
################
# TEST PROGRAM #
################

# FFT #
input_event(data,'example.event')
specnum:=16
data.unflag('Time')
timebeamo:=data.get('Voltage',1)
timelen:=len(timebeamo)/specnum
fftlen:=timelen/2+1 # even timeary len !!!
fftdbeam:=array(0.i,specnum,fftlen)
for (nspec in 1:specnum) {
  hlp:=timebeamo[(1+(nspec-1)*timelen):(nspec*timelen)]
  fftserver().complexfft(hlp,1)
  fftdbeam[nspec,]:=hlp[1:fftlen] # 1st nyquist zone for aips++ fftserver (2nd: hlp[fftlen:1]) !!!
}

# IMGs #
  if (1!=1) {
cs:=coordsys(linear=0,tabular=T,spectral=T);
#  cs.setnames(value=['Frequency','Blocks'])
#  cs.setepoch(dm.epoch('utc',qnt.quantity(data.head('Date'))));
#  cs.settelescope(data.head('Observatory'));
#  cs.setobserver(username());
  cs.reorder([2,1]);
  cs.setreferencepixel(1,'spectral');
  freqbin:=40e6/(fftlen-1)
  cs.setreferencevalue(dq.quantity(spaste(40e6,'Hz')),'spectral'); # freqbin/2 ???
  cs.setincrement(dq.quantity(spaste((freqbin),'Hz')),'spectral');
#  cs.setunits('s',type='tabular',overwrite=T);
#  cs.setincrement(1,'tabular');
pwrimg:=imagefromarray(pixels=abs(fftdbeam)^2,csys=cs);
absimg:=imagefromarray(pixels=abs(fftdbeam),csys=cs);
argimg:=imagefromarray(pixels=arg(fftdbeam),csys=cs);
cs.done()
pwrimg.tofits(outfile='gtest.fits')
absimg.tofits(outfile='gtest-abs.fits')
argimg.tofits(outfile='gtest-arg.fits')
  }

# HAMMING WINDOW #
for (nspec in 1:specnum) {fftdbeam[nspec,]:=fftdbeam[nspec,]*hanning(fftlen,alpha=.54)};

# invFFT #
for (nspec in 1:specnum) {
  hlp:=array(0+0i,timelen)
  hlp[1:fftlen]:=fftdbeam[nspec,]
  halflen:=as_integer(timelen/2)
  hlp[(timelen-halflen+1):timelen]:=conj(fftdbeam[nspec,(halflen+1):2]) # even timeary len !!!
  fftserver().complexfft(hlp,-1)
  timebeam[(1+(nspec-1)*timelen):(nspec*timelen)]:=real(hlp)
}
#plotdim(timebeamo); plotdim(timebeam,add=T) # the same without hamming window

# TEST #
include 'fits2wf_file.g'; res:=fits2wf('gtest',maxdel=0)
timebeam[1:3]; res.timebeamblock[1:3]
input_project(data,'gtest/',blocksize=2^16); timebeam[1:3]; data.get('Voltage')[1:3]
plotdim(timebeam); plotdim(data.get('Voltage'),add=T)
*/

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
