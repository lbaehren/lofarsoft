
#-------------------------------------------------------------------------------
# $Id: beamshape.g,v 1.2 2006/07/05 16:17:28 bahren Exp $
#-------------------------------------------------------------------------------
# The <a href="http://aips2.nrao.edu/docs/user/SynthesisRef/node143.html">simulator
# tool</a> is used in order to create a measurement set.
#  <pre>
#  shell('rm -rf beamresult.*')
#  createms(data)
#  correlator(data)
#  makeimage(simulate=F)
#  im.done()
#  </pre>
# The created measurement set can be inspected via:
#  <pre>
#  include 'ms.g'
#  include 'msplot.g'
#  msplot()
#  include 'tablebrowser.g'
#  tb:=tablebrowser()
#  </pre>
#-------------------------------------------------------------------------------

include "simulator.g" 
include 'imager.g'


#-------------------------------------------------------------------------------
#Name: clFromSource
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Create componentlist from individual sources
#Par:  complist   - Componentlist used as skymodel
#Par:  sourcename - Name of the source, towards which we want to want to
#                   observe; if left empty, the zenith is taken as default
#                   direction
#Example: clFromSource ();
#-------------------------------------------------------------------------------

clFromSource := function (complist='beamresult.cl', 
                          sourcename='')
{

  #-------------------------------------------------------------------
  # Set the direction towards the selected source

  dir  := dm.measure(dm.direction(sourcename),'J2000')
  azel := dm.measure(dm.direction(sourcename),'AZEL')
  dir0 := dm.measure(dm.direction('AZEL',  '0deg', '90deg'),'J2000')
  if ((sourcename == '') || (sourcename == 'Zenith')) { 
    dir := dir0 
    azel := dir0
  }

  #-------------------------------------------------------------------
  # Convert the direction coordinates of the source

  ra   := dm.getvalue(dir)[1]
  dec  := dm.getvalue(dir)[2]
  sra  := dq.form.long(ra)
  sdec := dq.form.lat(dec)
  saz  := paste(dm.getvalue(azel)[1].value/degree,'deg')
  sel  := paste(dm.getvalue(azel)[2].value/degree,'deg')

  #-------------------------------------------------------------------
  # Create a new componentlist and add the selected source

  cl := emptycomponentlist();

  cl.simulate(1);
  cl.setflux(1, [1e6, 0.0, 0, 0], 'Jy', 'Stokes');
  cl.setrefdirframe(1, 'J2000');
  cl.setrefdir(1, sra, 'time', sdec, 'angle');
  cl.setshape(1, 'Point');

  cl.rename(complist);
  cl.close();
  cl.done();

}


#-------------------------------------------------------------------------------
#Name: clFromFile
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Create a componentslist for a skymodel:
#      The shape properties of a component describe the variation of brightness
#      as a function of the position on the sky. The shape properties are split
#      into ones describing the functional variation and ones describing the
#      position of a reference point on the sky. Components can have one of
#      three shapes; point, Gaussian or disk. For all these shapes the reference
#      point, which is a direction measure, defines the direction of the
#      'centre' of the component.
#Par:  outputfile  - Name of the output file storing the generated CL.
#Par:  inputfile   - ASCII table holding the catalog list
#Created: 2004/12/07 (Demerese Salter)
#-------------------------------------------------------------------------------

clFromFile := function (outputfile='beamresult.cl',inputfile) 
{

  #-------------------------------------------------------------------
  # Read the contents of Fits file into a table

  if (inputfile == '') { inputfile := '/home/demerese/lofar/3C/asu.fit' };
  tbl := tablefromfits('inputable.tbl',inputfile)
  tbl.browse();

  #-------------------------------------------------------------------
  # Input coordinate columns into vectors

  ra := tbl.getcol("_RAJ2000");
  dec := tbl.getcol("_DEJ2000");
  # Input flux values into a vector
  flux :=tbl.getcol("S178MHz");
  # Input gaussian size into a vector
  diam := tbl.getcol("Diam");
  diam_units := tbl.getcol("x_Diam");
  # Put diam measurements in the same unit: degrees
  for (i in 1:tbl.nrows()) {
    if (diam_units[i] == "\'") {
      diam[i] := diam[i] / 60 } else
    if (diam_units[i] == "\"") {
      diam[i] := diam[i] / 3600 }
  }

  #-------------------------------------------------------------------
  # Default constructor for a new componentlist

  cl := emptycomponentlist();
  cl.simulate(tbl.nrows());

  #-------------------------------------------------------------------
  # cl: Fill list from previously read in table

  for (i in 1:tbl.nrows()) {
    sra[i] := dq.form.long(paste(split(ra[i]),sep=":"));
    sdec[i] := dq.form.lat(paste(split(dec[i]),sep="."));
      #TODO: fix the dec seconds conversion

      cl.setflux(i, [flux[i], 0, 0, 0], 'Jy','Stokes');
      cl.setrefdir(i, sra[i], 'time', sdec[i], 'deg');
      cl.setrefdirframe(i, 'J2000');
      # Assume circular gaussian shape for every source
      size := dq.quantity(diam[i],'deg');
      cl.setshape(i, 'Gaussian', size, size);
  }

  cl.rename('beamresult.cl');
  cl.close();
  cl.done();

}


#-------------------------------------------------------------------------------
#Name: createms
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Create measurement set for ITS station with freq range 10 - 35MHz
#Par:  rec            - Reference the data structure
#Par:  nchan  = 10    - Number of frequency channels
#Par:  freq1  = 10e6  - lower limit of the frequency range
#Par:  freq2  = 35e6  - upper limit of the frequency range
#Par:  name   = 'ITS' - Observatory name
#Par:  refant = 1     - Array index of the reference
#Par:  unit   = 1     - Unit for distanaces, [m].
#Par:  complist       - Componentlist used as skymodel
#Par:  sourcename     - Name of the source, towards which we want to want to
#                       observe; if left empty, the zenith is taken as default
#                       direction
#Example: createms(data)
#-------------------------------------------------------------------------------

createms := function(ref rec,
                     nchan=10,
                     freq1=10e6,
                     freq2=35e6,
                     name='ITS',
                     refant=1,
                     unit=1,
                     complist='beamresult.cl',
                     sourcename='')
{
  nant      := rec.len; 
  bandwidth := (freq2-freq1)/nchan;

  #-------------------------------------------------------------------
  # Create a reference frame for the observation; this requires the
  # observatory position and the epoch of the observation
 
  print 'Create a reference frame for the observation ...'

  posobs:=dm.observatory(rec.head('Observatory',refant));
  timobs:=dm.epoch('utc',qnt.quantity(rec.head('Date',refant)));
  dm.doframe(timobs);  
  dm.doframe(posobs);

  # integration time

  dtim:='6.7s'

  #-------------------------------------------------------------------
  # define directions in both AZEL and J2000 coordinates, tied to the
  # previously defined reference frame

  print 'Setting up source direction ...'

  dir0 := dm.measure(dm.direction('AZEL',  '0deg', '90deg'),'J2000')

  if ((sourcename == '') || (sourcename == 'Zenith')) { 
    dir  := dir0 ;
    azel := dir0 ;
  } else {
    dir  := dm.measure(dm.direction(sourcename),'J2000')
    azel := dm.measure(dm.direction(sourcename),'AZEL')
  }

  #-------------------------------------------------------------------
  # Get the 3-dim positions of the array elements (aka antennae); since
  # 'CalcAntPos' returns them inside a record structure, we need to
  # extract them from there into vectors/arrays used later on

  print 'Setting up array element positions ...'

  antpos  := CalcAntPos(rec,rec.ant(),refant);
  x       := array(0,nant);
  y       := array(0,nant);
  z       := array(0,nant);
  antname := array(name,nant);

  dishdiameter := rep(lightspeed/freq2/2,nant)

  for (i in seq(nant)) {
    x[i]:=antpos[i].position[2]*unit
    y[i]:=antpos[i].position[1]*unit
    z[i]:=antpos[i].position[3]*unit
    antname[i]:=spaste(name,as_string(i))
  }

  #-------------------------------------------------------------------
  # Create a simulator tool and configure it using the characteristics
  # of the antenna array; this includes
  #  (a) antenna positions,
  #  (b) feed type,
  #  (c) observatory position,
  #  (d) frequency bands & bandwidth,
  #  (e) integration & observation time,
  #  (f) field of view.
  # Finally the simulator tool is used to create a measurement set,
  # which in input to the imaging routines.

  print 'Creating simulator tool ...'

  sim := simulator();

  print ' - Setting configuration ...'

  sim.setconfig(name,
                x,
                y,
                z,
                dishdiameter,
                mount=rep('ALT-AZ',nant),
                antname=antname,
                coordsystem='local',
                referencelocation=posobs);

  print ' - Setting feed information ...'

  sim.setfeed('perfect X Y');
  
  print ' - Setting spectra window ...'

  sim.setspwindow(row=1, 
                  spwname='ITSBAND', 
                  freq=spaste(as_string(freq1/1e6),'MHz'),
                  deltafreq=spaste(as_string(bandwidth/1e6),'MHz'), 
                  freqresolution=spaste(as_string(bandwidth/1e6),'MHz'),
                  nchannels=nchan, 
                  stokes='XX');

  print ' - Setting times ...'

  sim.settimes(integrationtime=dtim,
               gaptime='0s',
               starttime='0s',
               stoptime=dtim, 
               referencetime=timobs);

  print ' - Setting field ...'

  sim.setfield(row=1,
               sourcename='azel',
               sourcedirection=dir0,
               integrations=1,
               xmospointings=1,
               ymospointings=1,
               mosspacing=0.5);

  print ' - Creating simulator ...'

  sim.create(newms='beamresult.ms',
             elevationlimit='-90deg')

  sim.predict(complist=complist)
  sim.close()
  sim.done()
}


#-------------------------------------------------------------------------------
#Name: correlator
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Create a visibility dataset from raw digitized time-series data.
#Par:  rec            - Reference to the data structure.
#Par:  nchan  = 10    - Number of frequency channels
#Par:  freq1  = 45e6  - Lower limit of frequency band. 
#Par:  freq2  = 75e6  - Upper limit of frequency band. 
#Par:  refant = 1     - Reference antenna.
#Par:  flag   = T     - 
#-------------------------------------------------------------------------------

correlator := function (ref rec,
                        nchan=10,
                        freq1=45e6,
                        freq2=75e6,
                        refant=1,
                        flag=T)
{
  #global ants,nants,nbl,blflag,baseline,ary,cary,visib,freqs,freqsel
  global visib;

  ants      := seq(rec.len);
  freqs     := rec.get('Frequency',ants[refant]);
  l         := len(freqs);
  nants     := len(ants);
  nbl       := nants*(nants-1)/2;
  blflag    := array(T,nbl,l);
  baseline  := array(0,nbl,2);
  ary       := array(0,nants,l);
  cary      := array(0,nbl,l);
  visib     := array(0,nbl,nchan);
  freqsel   := array(T,nchan,l);
  bandwidth := (freq2-freq1)/nchan;

  #-------------------------------------------------------------------
  # Definition of frequency channels

  print 'Define channels.' 
  for (i in seq(nchan)) {
    freqsel[i,]:=(freqs >= (freq1+(i-1)*bandwidth)) & (freqs < (freq1+i*bandwidth))
  }
  print 'Done.' 

  #-------------------------------------------------------------------
  # Get the FFT of the data from the individual antennas

  print 'Reading antenna data';
  for (i in ind(ants)) {
    ary[i,]:=rec.get('FFT',i);
  }
  print 'Done.'

  #-------------------------------------------------------------------
  # Correlation of the data

  n:=0;
  for (i in seq(nants-1)) {
    for (j in seq(i+1,nants)) {
      n+:=1;
      baseline[n,1]:=i; 
      baseline[n,2]:=j;

      if (flag) {
        blflag[n,] := ((data.getflag('FFT',i)*data.getflag('FFT',j))>0.9);
      }

      cary[n,]:=ary[i,]*conj(ary[j,])
      for (k in seq(nchan)) {
        if (flag) {
          visib[n,k] := mean(cary[n,blflag[n,] && freqsel[k,]])
        }
        else { 
          visib[n,k] := mean(cary[n,freqsel[k,]]);
        }
      }
      print n,'/',nbl,' [',i,',',j,']:', round(abs(visib[n,])),round(phase(visib[n,]))
    }
  }

}


#imgr.setimage(nx=nx,ny=ny,cellx=celly,celly=celly,stokes='I',doshift=T,phasecenter=dir)
#imgr.setimage(nx=nx,ny=ny,cellx=celly,celly=celly,stokes='I',mode='channel',nchan=nchan)
#imgr.makeimage('psf','beamresult.psf')


#-------------------------------------------------------------------------------
#Name: ccData2Visibilities
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  
#Par:  datadir = '' - 
#Ret:  status  = 'F'  - Error status; returns 'F' as default.
#-------------------------------------------------------------------------------

ccData2Visibilities := function (datadir='') 
{
  global visib;
  global baselines;

  # Provide location of the CLI binary

  CLI := client(spaste(globalconst.LOPESBIND,'/readdat'));

  # Set up parameter record required by the CLI

  recCLI := [=];
  recCLI.metafile    := spaste(datadir,'/experiment.meta');
  recCLI.antennafile := spaste(globalconst.progDir,'/antpos-its.dat');

  # Run the CLI to read in the cc-data from disk and return them as
  # visibilities

  dataCLI := CLI->getVisibilities (recCLI);

  # Extract visibilities from the return data record

  if (has_field(dataCLI,'visibilityValues')) {
    visib := dataCLI.visibilityValues;
  }
  else {
    print "[beamshape::ccData2Visibilities] No record field visibilityValues!";
    fail;
  }

  if (has_field(dataCLI,'baselines')) {
    baselines := dataCLI.baselines;
  }
  else {
    print "[beamshape::ccData2Visibilities] No record field baselines!";
    fail;
  }

}


#-------------------------------------------------------------------------------
#Name: makeimage
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Create an image from a previously generated visibility dataset.
#Par:  file     - 
#Par:  nchan    - Number of frequency channels
#Par:  nx       - Number of image pixels along the x-axis
#Par:  ny       - Number of image pixels align the y-axis
#Par:  cellx    - Angular resolution along the x-axis
#Par:  celly    - Angular resolution along the y-axis
#Par:  simulate - Run simulator?
#Par:  doshift  - Use the specified phase center?
#Par:  doclean  - Clean the created skymap, using the array beam-pattern?
#Par:  bmaj     - Major/Minor axis of beam
#-------------------------------------------------------------------------------

makeimage := function(file='beamresult',
                      nchan=10, 
                      nx=360, 
                      ny=180, 
                      cellx='1deg', 
                      celly='1deg',
                      simulate=F,
                      doshift=F,
                      doclean=F, 
                      bmaj='3deg')
{
  global im,im2

  #Set the position of the phase-center
  dir:=dm.measure(dm.direction('sun'),'J2000')

  #-------------------------------------------------------------------

  if (!simulate) {
   tab := table(spaste(file,'.ms'),readonly=F)
    x  := array(0,1,nchan); 
    for (s in tab.colnames()[22:24]) {
      for (i in seq(tab.nrows())) {
        x[1,]:=visib[i:i,];
        tab.putcell(s,i,x)
      }
    }
    tab.close()
  }

  #-------------------------------------------------------------------
  # Create and set up an imager tool

  imgr := imager(spaste(file,'.ms'))

  imgr.setimage(nx=nx,
                ny=ny,
                cellx=celly,
                celly=celly,
                stokes='I',
                mode='mfs',
                nchan=nchan,
                phasecenter=dir,
                doshift=doshift)

  imgr.makeimage('psf',spaste(file,'.psf'))

  imgr.makeimage('observed',spaste(file,'.obs'))

  #-------------------------------------------------------------------
  # CLEANing of the image

  if (doclean) {
    imgr.setbeam(bmin=bmaj,
                 bmaj=bmaj);

    imgr.clean(niter=100,
               gain=0.05,
               displayprogress=T,
               image=spaste(file,'.cln.img'),
               model=spaste(file,'.cln.mod'),
               residual=spaste(file,'.cln.res'));

    imgr.restore(image=spaste(file,'.cln.rst'),
                 model=spaste(file,'.cln.mod'),
                 residual=spaste(file,'.cln.res'));

    #complist=spaste(file,'.cl'),
  }

  print spaste(file,'.cln.rst')
  imgr.done()
  im:=image(spaste(file,'.cln.rst'))
  im.view()
  return

  ary:=im.getchunk(dropdeg=T)
  im2:=imagefromarray(outfile=spaste(file,'.nocs'),pixels=ary)
  im2.view()
}

