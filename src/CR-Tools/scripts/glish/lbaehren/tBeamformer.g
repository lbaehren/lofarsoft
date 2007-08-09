
#-------------------------------------------------------------------------------
# 
# A collection of test routines for the Beamformer.For further description
# consult the documentation for
# <a href="http://www.astron.nl/~bahren/research/coding/tBeamformer_8cc.html">tBeamformer.cc</a>
# 
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: testBeamformer
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Driving routines to go through all individual tests.
#      <ol>
#        <li><a href="#testFrequencies">testFrequencies</a>
#        <li><a href="#testAntennaPositions">testAntennaPositions</a>
#        <li><a href="#testFFT">testFFT</a>
#        <li><a href="#testGeometricalDelay">testGeometricalDelay</a>
#        <li><a href="#testPhaseDelay">testPhaseDelay</a>
#        <li><a href="#testDistance">testDistance</a>
#      </ol>
#Par:  rec       - Record where the data is stored
#Par:  blocksize - Size of a datablock, in samples.
#Par:  dotest    - Selection of tests to perform. See list of subroutines above
#                  for effect of the parameters.
#Ref:  rec.setblock,plotgui.replot,dirtoazel,rec.get,getAntennaPositions,
#      testFrequencies,testAntennaPositions,testFFT,testGeometricalDelay,
#      testPhaseDelay,testDistance
#Example: testBeamformer(data, dotest=[T,T,T,T,T,T])
#-------------------------------------------------------------------------------

testBeamformer := function (ref rec, blocksize=4096, dotest=[T,T,T,T,T,T]) {

  global CLI := client(spaste(globalconst.cprogDir,'/tBeamformer'));

  ### adjust the blocksize

  rec.setblock(blocksize=blocksize);
  plotgui.replot();

  #-------------------------------------------------------------------
  # Set up a list of sources

  directions := [=];
  directions['Zenith']  := [0,90];
  directions['Halfzenith']  := [0,45];
  directions['Sun']     := dirtoazel (rec,planet='Sun');
  directions['Jupiter'] := dirtoazel (rec,planet='Jupiter');

  #-------------------------------------------------------------------
  # Get the list of antenna positions

  refant := rec.get('RefAnt');
  antennaPositions := getAntennaPositions (rec,refant=refant);

  #-------------------------------------------------------------------
  # Run the tests

  if (dotest[1]) testFrequencies (rec);

  if (dotest[2]) testAntennaPositions (rec);

  if (dotest[3]) testFFT (rec);

  if (dotest[4]) testGeometricalDelay (rec,directions);  

  if (dotest[5]) testPhaseDelay (rec,directions,antennaPositions);

  if (dotest[6]) testDistance (rec);

  CLI := F;

}

#-------------------------------------------------------------------------------
#Name: extractSourcePositions
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Given a record with sources and their respective positions, extract an
#      array with the source positions only.
#Par:  sources   - Record with source names and source positions
#Ret:  positions - Array with the source positions
#Created: 2004/10/29 (Lars Baehren)
#-------------------------------------------------------------------------------

extractSourcePositions := function (ref sources)
{
  nofSources     := len(sources);
  nofCoordinates := len(sources[1]);
  
  positions := array(0,nofSources,nofCoordinates);

  for (i in 1:nofSources) positions[i,] := sources[i];

  return positions;
}

#-------------------------------------------------------------------------------
#Name: testAntennaPositions
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Compare extraction of antenna positions
#Par:  rec    - Reference to the data structure.
#-------------------------------------------------------------------------------

testAntennaPositions := function (ref rec) 
{
  print "[tBeamformer::testAntennaPositions]";

  index  := [1:rec.len][rec.select()];

  #-------------------------------------------------------------------
  # [1] Default settings

  print '+++ Test 1: Default settings +++'

  antennaPositions := getAntennaPositions (rec);

  antpos := CalcAntPos(rec)

  for (ant in 1:len(index)) {
    print ant,antennaPositions[ant,],antpos[ant].position;
  }  

  #-------------------------------------------------------------------
  # [2] Explicit selection of the reference antenna

  print '+++ Test 2 : Explicit selection of the reference antenna +++';

  refant := 3;

  antennaPositions := getAntennaPositions (rec, refant=refant);

  antpos := CalcAntPos(rec, refant=refant)

  for (ant in 1:len(index)) {
    print ant,antennaPositions[ant,],antpos[ant].position;
  }  

  #-------------------------------------------------------------------
  # [3] Shift phase-center to shower core

  print '+++ Test 3 : Shift phase-center to shower core +++';

  refpos := [rec.get('XY')[2],rec.get('XY')[1],0];

  antennaPositions := getAntennaPositions (rec, xy=T);

  antpos := CalcAntPos(rec, refpos=refpos)

  for (ant in 1:len(index)) {
    print ant,antennaPositions[ant,],antpos[ant].position;
  }  

  #-------------------------------------------------------------------
  # [4] Shift phase-center to shower core and neglect z-component of 
  #     antenna position.

  print '+++ Test 4 : Shift phase-center to shower core and no z-coordinate +++';

  refpos := [rec.get('XY')[2],rec.get('XY')[1],0];

  antennaPositions := getAntennaPositions (rec, xy=T, doz=F);

  antpos := CalcAntPos(rec, refpos=refpos);

  for (ant in 1:len(index)) {
    print ant,antennaPositions[ant,],antpos[ant].position;
  }  

}

#-------------------------------------------------------------------------------
#Name: testFrequencies
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Test for the frequency values as obtained by various methods
#Created: 2004/10/28 (Lars Baehren)
#Status:  production
#-------------------------------------------------------------------------------

testFrequencies := function (ref rec, makePlots=F)
{
  CLI := client(spaste(globalconst.cprogDir,'/tBeamformer'));

  #-------------------------------------------------------------------
  # Use Glish internals for frequency values

  FFTlen         := as_integer(rec.getmeta('FFTlen'));
  FrequencyLow   := as_double(rec.getmeta('FrequencyLow'));
  FrequencyHigh  := as_double(rec.getmeta('FrequencyHigh'));
  Bandwidth      := as_double(FrequencyHigh-FrequencyLow);

  #-------------------------------------------------------------------
  # Test retrival of frequency values for multiple frequency bands

  nofBands := 5;
  
  for (n in seq(nofBands)-1) {

    FrequencyBand  := [FrequencyLow+n*0.05*Bandwidth,FrequencyHigh-n*0.05*Bandwidth];

    print "";
    print "[",n,"] Frequency band =",FrequencyBand;

    #-------------------------------------------------------------------
    # Get the frequency values through the different Glish routines

    freqGet     := rec.get('Frequency');
    freqPhasing := calcFrequencyValues(fftlen=FFTlen,flowlim=FrequencyLow,
				       fbandw=Bandwidth);

    freqMask := freqGet>=FrequencyBand[1] & freqGet<=FrequencyBand[2];
    freqBand := freqGet[freqMask];

    #-------------------------------------------------------------------
    # Get frequency values from CLI

    recCLI                := [=];

    DataToRecord (rec,recCLI,'Frequency');
    recCLI.FrequencyBand  := FrequencyBand;

    freqCLI := CLI->testFrequencies(recCLI);

    freqAllCLI  := freqCLI.all;
    freqBandCLI := freqCLI.band;
    freqMaskCLI := freqCLI.mask;

    #-------------------------------------------------------------------
    # Compare the individual results

    print "Full frequency range ...";
    print " - freqAllCLI .. :",shape(freqAllCLI),
			       min(freqAllCLI),max(freqAllCLI),
			       freqAllCLI[2]-freqAllCLI[1];
    print " - freqGet ..... :",shape(freqGet),
			       min(freqGet),max(freqGet),
			       freqGet[2]-freqGet[1];
    print " - freqPhasing . :",shape(freqPhasing),
			       min(freqPhasing),max(freqPhasing),
			       freqPhasing[2]-freqPhasing[1];
    print "";

    print "Selected frequency band ...";
    print " - freqBandCLI . :",shape(freqBandCLI),
			       min(freqBandCLI),max(freqBandCLI),
			       freqBandCLI[2]-freqBandCLI[1];
    print " - freqBand .... :",shape(freqBand),
			       min(freqBand),max(freqBand),
			       freqBand[2]-freqBand[1];
    print "";

    print "Masking array for frequency selection ...";
    print " - freqMaskCLI . :",shape(freqMaskCLI),sum(freqMaskCLI)
    print " - freqMask .... :",shape(freqMask),sum(freqMask)

    if (makePlots) {
      plot ([seq(1:len(freqGet))],freqGet,lines=T,col=2)
      plot ([seq(1:len(freqPhasing))],freqPhasing,lines=T,col=3)
      plot ([seq(1:len(freqAllCLI))],freqAllCLI,lines=T,col=4)
    }

  }

}

#-------------------------------------------------------------------------------
#Name: testGeometricalDelay
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Compute the geometrical light travel-time delay for a set of antennas
#      and source positions.
#Par:  rec         - Record where the data is stored
#Par:  directions  - List of directions towards which the the delays are computed
#Par:  plotResults - Plot the computed geometrical delays?
#Ref:  CalcAntPos,calcdelay
#Created: 2004/10/26 (Lars Baehren)
#Status:  production
#-------------------------------------------------------------------------------

testGeometricalDelay := function (ref rec,sourcelist,plotResults=F) {

  index  := [1:rec.len][rec.select()];
  nant   := len(index);
  antpos := CalcAntPos(rec)
  antennaPositions := getAntennaPositions (rec);

  direction   := array(0,2);
  delaysGlish := array(0,nant);

  for (source in field_names(sourcelist)) {
    direction := sourcelist[source];

    #-----------------------------------------------------------------
    # Run the CLI to get the results from the C++ routines

    recCLI := [=];
    recCLI.direction        := direction;
    recCLI.antennaPositions := antennaPositions;
    resultCLI := CLI->testGeometricalDelay(recCLI);

    delays := resultCLI.delays;

    #-----------------------------------------------------------------
    # Delay computation using Glish code

    for (ant in 1:nant) {
      delaysGlish[ant] := calcdelay(azel=direction,antpos=antpos[ant],doz=F);
    }
    #-----------------------------------------------------------------
    # Show computation results

    print "Source name ........... :",source;
    print "Source direction (AZEL) :",direction;
    print "Source position (x,y,z) :",resultCLI.xyz;
    print "";
    for (ant in 1:nant) {
      print "ant=",ant,
            ", delays(CLI)=",delays[ant],
            ", delays(Glish)=",delaysGlish[ant],
            ", diff=",delays[ant]-delaysGlish[ant];
    }
    plotTestresults (delaysGlish,delays,source,'Antenna','Delay');
  } 
  
}

#-------------------------------------------------------------------------------
#Name: testDistance
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Test handling of the distance information in the beamforming process. The
#      distance information becomes vital for sources located within the
#      near-field of the antenna array, where incoming electromagnetic radiation
#      no longer can be treated a plane waves.
#Par:  rec            - Reference to the data structure.
#Par:  steps    = 100 - Number of distance steps for which tests are performed.
#Par:  stepsize = 200 - Stepwidth [m] between two subsequent distances for which
#                       computation is performed.
#Par:  ant      = 2   - Antenna array element for which the delay computations
#                       are displayed/logged.
#Par:  dotest         - Selection of tests to perform.
#                       <ol>
#                         <li>Light time travel delay for an individual antenna
#                         <li>Light time travel delay for a set of antennae
#                         <li>Weighting factors for the beamforming
#                       </ol>
#Ref:  dirtoazel,getAntennaPositions,CalcAntPos,calcdelay,plotTestresults
#Created: 2004/12/02 (Lars Baehren)
#Status: production
#-------------------------------------------------------------------------------

testDistance := function (ref rec, steps=100, stepsize=200, ant=2, dotest=[T,T,T],
                          plotResults=F, logResults=T)
{

  #-------------------------------------------------------------------
  # Setup of beam direction and distance steps; to the CLI we pass
  # an array with all three coordinates for given direction and
  # distance

  azel      := dirtoazel (rec,planet='Sun');
  distSteps := [1:steps]*stepsize;

  directions := array(0,steps,3);
  for (i in 1:steps) directions[i,] := [azel[1],azel[2],distSteps[i]];

  delay1 := array(0,steps);
  delay2 := array(0,steps);

  #-------------------------------------------------------------------
  # get the 3-dim antenna positions

  index            := [1:rec.len][rec.select()];
  refant           := rec.get('RefAnt');
  antennaPositions := getAntennaPositions (rec,index=index,refant=refant);
  antpos           := CalcAntPos(rec,index=index,refant=refant)

  antposArray := array(0.0,len(antpos),3);
  for (ant in 1:len(antpos)) {
    antposArray[ant,] := antpos[ant].position;
  }

  #-------------------------------------------------------------------
  # Settings for the CLI

  CLI := client(spaste(globalconst.cprogDir,'/tBeamformer'));

  recCLI := [=];
  DataToRecord (rec,recCLI,'Frequency');
  recCLI.directions       := directions;
  recCLI.antennaPositions := antennaPositions;

  #-------------------------------------------------------------------
  # Log setup for tests

  if (logResults) {
    logfile := open(spaste('>',environ.LOPESCODEDIR,'/tests/testDistance.log'));
    write(logfile,shell('date'));
    write(logfile,' ');
    write(logfile,paste('Beam direction ......... :',azel));
    write(logfile,paste('Number of distance steps :',steps));
    write(logfile,paste('Distance step size . [m] :',stepsize));
    write(logfile,paste('Antenna selection ...... :',index));
    write(logfile,paste('Reference antenna ...... :',refant));
    write(logfile,paste('Antenna positions ...... :',antennaPositions));
    write(logfile,paste('Antenna positions ...... :',antposArray));
    write(logfile,' ');
  }

  #-------------------------------------------------------------------
  # [1] Light time travel delay for an individual antenna

  if (dotest[1]) {

    # { C++ } ----------------------------------------------

    resultsCLI := CLI->testDistance(recCLI);

    # extract results from returned record

    delayCLI       := as_double(resultsCLI.delays);
    delayCLINoDist := as_double(resultsCLI.delaysNoDist);

    # { GLish } --------------------------------------------

    for (i in 1:steps) {
      dist := distSteps[i];

      delay1[i] := calcdelay (azel=azel,antpos=antpos[ant],doz=T,distance=-1);
      delay2[i] := calcdelay (azel=azel,antpos=antpos[ant],doz=T,distance=dist);

      if (logResults) {
        write(logfile,paste(dist,'|',delay1[i],delay2[i],delay2[i]-delay1[i],
                   '|',delayCLINoDist[i,ant],delayCLI[i,ant],
                       delayCLI[i,ant]-delayCLINoDist[i,ant],
                   '|',delayCLINoDist[i,ant]-delay1[i],delayCLI[i,ant]-delay2[i]));
      }
                    
    }
   
    if (plotResults) {
      plotTestresults(delay1,delay2,'Comparison of Glish routines',
                      'Distance','Delay');
      plotTestresults(delayCLINoDist[,ant],delayCLI[,ant],'Comparison of C++ routines',
                      'Distance','Delay');
      plotTestresults(delay1,delayCLI[,ant],'Comparison Glish vs. C++',
                      'Distance','Delay');    
      plotTestresults(delay2-delay1,delayCLI[,ant]-delay1,
                      'Comparison Glish vs. C++','Distance','del(Delay)');    
    }

  }

  #-------------------------------------------------------------------
  # [2] Light time travel delay for a set of antennae

  if (dotest[2]) {
    # C++
    resultsCLI := CLI->testDistance(recCLI);
    delayCLI := resultsCLI.delays;
    # Glish
    ant := 2;
    for (i in 1:steps) {
      dist   := distSteps[i];
      delay1[i] := calcdelay (azel=azel,antpos=antpos[ant],doz=T,distance=dist);
      delay2[i] := calcdelay (azel=azel,antpos=antpos[ant+1],doz=T,distance=dist);
    }
    # plot results
    plotTestresults(delay1,delay2,'Glish: comparison for two antennae',
                    'Distance','Delay');
    plotTestresults(delayCLI[,ant],delayCLI[,ant+1],'C++: comparison for two antennae',
                    'Distance','Delay');
    plotTestresults(delay1,delayCLI[,ant],'Comparison Glish vs. C++',
                    'Distance','Delay');
    plotTestresults(delay2,delayCLI[,ant+1],'Comparison Glish vs. C++',
                    'Distance','Delay');
  } 

  #-------------------------------------------------------------------
  # [3] Weighting factors for the beamforming

  if (dotest[3]) {

    # { C++ } ----------------------------------------------

    delaysCLI := CLI->testPhaseDelay(recCLI);

    # { GLish } -------------------------------------------- 

    bandwidth  := recCLI.FrequencyHigh-recCLI.FrequencyLow;
    nfreq      := len(rec.get('Frequency'));
    nant       := len(index);

    phiPhasing := array(1+0i,nfreq,nant);
    phiTIM40   := array(1+0i,nfreq,nant);

    for (i in 1:steps) {
      dist   := distSteps[i];
  
      for (ant in index) {
        # compute the delay
        delay := calcdelay(azel=azel,antpos=antpos[ant],distance=dist);
        # compute the phase
        phiPhasing[,ant] := calcphasegrad (delay=delay,
                                           fftlen=recCLI.FFTlen,
                                           flowlim=recCLI.FrequencyLow,
                                           fbandw=bandwidth);
        phiTIM40[,ant] := TIM40convFFT2PhaseGrad([],rec,ant);
      
      }

      # compute the phase gradient
      phasegradPhasing := exp(-1i*phiPhasing*degree);
      phasegradTIM40   := exp(-1i*phiTIM40*degree);

      # comparison of the results

      print ' - Distance step :',dist,'m';

      for (ant in index) {
        print ' - Antenna',ant,':';
        print ' [calcphasegrad] ........ : ',phasegradPhasing[1:4,ant];
        print ' [TIM40convFFT2PhaseGrad] : ',phasegradTIM40[1:4,ant];
        print ' [phaseGradient] ........ : ',delaysCLI.gradientsSingleValue[1:4,i,ant];
      }
    }
  }

  CLI := F;

}


#-------------------------------------------------------------------------------
#Name: testFFT
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Test the equivalence of the 'FFT2Fx' functions implemented in 'phasing.g'
#      'Phasing.cc'
#Par:  rec    - Reference to the data structure.
#Par:  dotest - Selection of tests to perform.
#Created: 2004/11/16 (Lars Baehren)
#Status: production
#-------------------------------------------------------------------------------

testFFT := function (ref rec,dotest=[T,T,T]) {

  rec.setblock(n=1,blocksize=512);
  plotgui.replot();

  CLI := client(spaste(globalconst.cprogDir,'/tBeamformer'));

  recCLI := [=];
  DataToRecord (rec,recCLI,'Frequency');

  #-------------------------------------------------------------------
  # Some feedback

  print "[tBeamformer::testFFT]";
  print " - FFT input size .. :",recCLI.FFTSize;
  print " - FFT output length :",recCLI.FFTlen;

  #-------------------------------------------------------------------
  # (1) Run test on an input array where only the first element is
  #     non-zero.

  if (dotest[1]) {
    dataOrig := array(0.0,rec.getmeta('FFTSize'));
    dataOrig[1] := 1.0;

    dataFreq := fftvec (dataOrig);
    dataTime := as_float(FFTtoFx (from=dataFreq,fftsize=rec.getmeta('FFTSize')));

    recCLI.dataOrig := dataOrig;
    recCLI.dataFreq := dataFreq;
    recCLI.dataTime := dataTime;

    resultCLI := CLI->testFFT(recCLI);

    # (a) compare the Glish results
    plotTestresults(dataOrig,dataTime,'test1: Glish');

    # (b) compare Glish and CLI results
    plotTestresults(dataOrig,resultCLI.FFTtoFxTimeRC,'test1: C++ using Real-Complex');

    # (c) compare Glish and CLI results
    plotTestresults(dataOrig,resultCLI.FFTtoFxTimeCC,
                    'test1: C++ using Complex-Complex');
  }

  #-------------------------------------------------------------------
  # (2) Run test on an input array where only the first elements 
  #     non-zero.

  if (dotest[2]) {
    dataOrig := array(0.0,rec.getmeta('FFTSize'));
    dataOrig[1] := 1.0;
    dataOrig[2] := 2.0;

    dataFreq := fftvec (dataOrig);
    dataTime := as_float(FFTtoFx (from=dataFreq,fftsize=rec.getmeta('FFTSize')));

    recCLI.dataOrig := dataOrig;
    recCLI.dataFreq := dataFreq;
    recCLI.dataTime := dataTime;

    resultCLI := CLI->testFFT(recCLI);

    # (a) compare the Glish results
    plotTestresults(dataOrig,dataTime,'test2: Glish');

    # (b) compare Glish and CLI results
    plotTestresults(dataTime,resultCLI.FFTtoFxTimeRC,'test2: C++ using Real-Complex');

    # (c) compare Glish and CLI results
    plotTestresults(dataTime,resultCLI.FFTtoFxTimeCC,
                    'test2: C++ using Complex-Complex');
  }

  #-------------------------------------------------------------------
  # (3) Run test of one block of data

  # (a) Extract one block of data and do a forward->backward FFT

  if (dotest[3]) {
    dataVolt := rec.get('Voltage');
    dataFreq := rec.get('CalFFT');
    dataTime := as_float(FFTtoFx (from=dataFreq,fftsize=rec.getmeta('FFTSize')));

    recCLI.dataOrig := dataVolt;
    recCLI.dataFreq := dataFreq;
    recCLI.dataTime := dataTime;

    resultCLI := CLI->testFFT(recCLI);

    # (b) compare Glish and CLI results
    plotTestresults(dataTime,resultCLI.FFTtoFxTimeRC,'C++ using Real-Complex');

    # (c) compare Glish and CLI results
    plotTestresults(dataTime,resultCLI.FFTtoFxTimeCC,'C++ using Complex-Complex');
  }

  #-------------------------------------------------------------------

  CLI := F;

}

#-------------------------------------------------------------------------------
#Name: testPhaseDelay
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Compare the values of the phase delay factors as returned by (a) the
#      Glish data kernel routines and (b) the C++ code implementation.
#Par:  rec        - Record where the data is stored
#Par:  sourcelist - List of directions towards which the the delays are computed
#Par:  plotGradients - plot the residuals of the computed phase gradients?
#Created: 2004/10/27 (Lars Baehren)
#Status:  experimental
#-------------------------------------------------------------------------------

testPhaseDelay := function (ref rec,sourcelist,plotGradients=F)
{
  global phasegradResidual;
  PI := 3.1415926535897932384626433832795;
  todegree := 180.0/PI;

  index  := [1:rec.len][rec.select()];
  refant := rec.get('RefAnt');

  #-------------------------------------------------------------------
  # Run the CLI to get the results from the C++ routines

  recCLI := [=];

  DataToRecord (rec,recCLI,'Frequency');

  recCLI.directions       := extractSourcePositions(sourcelist);
  recCLI.antennaPositions := getAntennaPositions(rec,index=index,refant=refant);

  delays := CLI->testPhaseDelay(recCLI);

  #-------------------------------------------------------------------
  # Preparation for running the Glish routines

  bandwidth  := recCLI.FrequencyHigh-recCLI.FrequencyLow;
  nfreq      := len(rec.get('Frequency'));
  nant       := len(index);

  antpos     := CalcAntPos(rec,index=index,refant=refant);
  phiPhasing := array(1+0i,nfreq,nant);
  phiTIM40   := array(1+0i,nfreq,nant);

  #-------------------------------------------------------------------
  # compute phase gradients for each source in the list

  numSource := 0;
  for (source in field_names(sourcelist)) {

    numSource +:= 1;

    direction := sourcelist[source];
 
    #-----------------------------------------------------------------
    # Compute the phase gradients using the functions in 'phasing.g' 
    # 'data-TIM40.g'; keep in mind that the frequency values in the
    # first case are computed internally, not using the data.get
    # mechanism

    for (ant in index) {
      # compute the delay
      delay := calcdelay(azel=direction,antpos=antpos[ant]);
      # compute the phase
      phiPhasing[,ant] := calcphasegrad (delay=delay,
                                  fftlen=recCLI.FFTlen,
                                  flowlim=recCLI.FrequencyLow,
                                  fbandw=bandwidth);
      phiTIM40[,ant] := TIM40convFFT2PhaseGrad([],rec,ant);
      
    }
    # compute the phase gradient
    phasegradPhasing := exp(-1i*phiPhasing*degree);
    phasegradTIM40   := exp(-1i*phiTIM40*degree);

    #-----------------------------------------------------------------
    # compare the results

    print 'Phase gradients for',source,'@',direction;

    for (ant in index) {

    print ' - Antenna',ant,':';
    print ' [phasing::calcphasegrad] ........... : ',phasegradPhasing[1:4,ant];
    print ' [data-TIM40::TIM40convFFT2PhaseGrad] : ',phasegradTIM40[1:4,ant];
    print ' [Phasing::phaseGradient] ........... : ',delays.gradientsSingleValue[1:4,numSource,ant];
    }

    #-----------------------------------------------------------------
    # display results

#    plotPhaseGradient1d (phasegrad);

    if (plotGradients) {
#      plotPhaseGradient2d (phasegradResidual);
#      plotPhaseGradient2d (phasegrad);
#      plotPhaseGradient2d (phasegradCLI);
    }
  }

}

#-------------------------------------------------------------------------------
#Name: plotPhaseGradient2d
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par: ary - Data array (passed by reference)
#Created: 2004/10/28 (Lars Baehren)
#Status:  experimental
#-------------------------------------------------------------------------------

plotPhaseGradient2d := function (ref ary)
{
  #-------------------------------------------------------------------
  # create images

  cs := coordsys(spectral=1,linear=1);
  imgReal := imagefromarray(pixels=real(ary),csys=cs)
  imgImag := imagefromarray(pixels=imag(ary),csys=cs)
  imgAbs  := imagefromarray(pixels=abs(ary),csys=cs)
  imgArg  := imagefromarray(pixels=arg(ary),csys=cs)

  #-------------------------------------------------------------------
  # create viewer for images

  vdp := dv.newdisplaypanel(nx=2,ny=2);
  mdd1 := dv.loaddata(imgReal,'raster');
  mdd2 := dv.loaddata(imgImag,'raster');
  mdd3 := dv.loaddata(imgAbs,'raster');
  mdd4 := dv.loaddata(imgArg,'raster');

  #-------------------------------------------------------------------
  # set the display options

  mdd1.setoptions([axislabelswitch=T,titletext='real',colormap=['Hot Metal 1']]);
  mdd2.setoptions([axislabelswitch=T,titletext='imag',colormap=['Hot Metal 1']]);
  mdd3.setoptions([axislabelswitch=T,titletext='abs',colormap=['Hot Metal 1']]);
  mdd4.setoptions([axislabelswitch=T,titletext='phase',colormap=['Hot Metal 1']]);

  #-------------------------------------------------------------------
  # register the images to the viewer

  dv.hold();
  vdp.register(mdd1);  
  vdp.register(mdd2);  
  vdp.register(mdd3);  
  vdp.register(mdd4);  
  dv.release();
}

#-------------------------------------------------------------------------------
#Name: plotPhaseGradient1d
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  -/-
#Status: experimental
#-------------------------------------------------------------------------------

plotPhaseGradient1d := function (ref ary)
{
  aryshape := shape(ary);   # [nfreq,nant]
  x := [seq(1:aryshape[1])]; 

  pg := pgplotter()
  pg.subp(aryshape[2],4)

  for (ant in 1:aryshape[2]) {

    print 'Plotting antenna',ant;

    # get the data for this antenna

    imgReal := real(ary[,ant]);
    imgImag := imag(ary[,ant]);
    imgAbs  := abs(ary[,ant]);
    imgArg  := arg(ary[,ant]);

    print min(x),max(x),min(imgReal),max(imgReal);
    pg.env(min(x),max(x),min(imgReal),max(imgReal),1,0);
    pg.line(x,imgReal);

    print min(x),max(x),min(imgImag),max(imgImag);
    pg.env(min(x),max(x),min(imgImag),max(imgImag),1,0);
    pg.line(x,imgImag);

    print min(x),max(x),min(imgAbs),max(imgAbs);
    pg.env(min(x),max(x),min(imgAbs),max(imgAbs),1,0);
    pg.line(x,imgAbs);

    print min(x),max(x),min(imgArg),max(imgArg);
    pg.env(min(x),max(x),min(imgArg),max(imgArg),1,0);
    pg.line(x,imgArg);

   }
}

#-------------------------------------------------------------------------------
#Name: calcFrequencyValues
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  This is the code segement from 'calcphasegrad' which is responsible for
#      the computation of the frequency values.
#Ret:  freqs - Vector with the frequency values
#Status: production
#-------------------------------------------------------------------------------

calcFrequencyValues := function(fftlen,flowlim=40e6,fbandw=40e6)
{
  fsteps:=fbandw/(fftlen-1);
  freqs := flowlim+[0:(fftlen-1)]*fsteps;
  return freqs;
}

#-------------------------------------------------------------------------------
#Name: plotTestresults
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par:  dataOrig - Vector containing the original data, which were either used
#                 as starting point for the computation, or represent the 
#                 reference to be reproduced in the computation.
#Par:  dataComp - 
#Par:  titleSup - Title suplement.
#par:  xlabel   - Label for the x-axis.
#par:  ylabel   - Label for the y-axis.
#Status: production
#-------------------------------------------------------------------------------

plotTestresults := function (ref dataOrig, ref dataComp, titleSup='',
                             xlabel='', ylabel='')
{
  pg := pgplotter(size=[750,750]);
  pg.subp(1,3);

  pg.setcolor(1);
  pg.env(1,len(dataOrig),min(dataOrig),max(dataOrig),0,0);
  pg.lab(xlabel,'power',spaste('Original data [',titleSup,']'));
  pg.setcolor(2);
  pg.line([1:len(dataOrig)],dataOrig);

  pg.setcolor(1);
  pg.env(1,len(dataComp),min(dataComp),max(dataComp),0,0);
  pg.lab(xlabel,'power',spaste('Computed data [',titleSup,']'));
  pg.setcolor(3);
  pg.line([1:len(dataComp)],dataComp);

  diff := dataOrig - dataComp;
  if (min(diff) == max(diff)) print 'Single value for residual:',diff[1];
  else {
    pg.setcolor(1);
    pg.env(1,len(diff),min(diff),max(diff),0,0);
    pg.lab(xlabel,'power',spaste('Residual [',titleSup,']'));
    pg.setcolor(4);
    pg.line([1:len(diff)],diff);
  }
}