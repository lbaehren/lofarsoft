########################################################################
#
# Contains a number of routines to deal with phase gradients, delays, 
# and phase calibration.
#
# <h3>Contents:</h3>
# <ol>
#  <li>Calculation of Phases and Delays
#  <li>Various Schemes to calibrate the phases of the station
# </ol>
########################################################################


#######################################################################
#----------------------------------------------------------------------
#Name: phasecmp  
#----------------------------------------------------------------------
#Type: Function
#Doc: calculates delay corrections from phase differences (e.g. of the 
#     TV signal)
#Par: data 	= 	- data structure
#     freqrange = 	- lower and upper values of frequency ranges
#               (default: TV-picture carrier, 1st and 2nd sound carrier)
#      ants 	= 	- antenna selection, ants=-1 -> all antennas
#      refant 	= 	- reference antenna
#      disp 	= T 	- display summary of the phases and changes
#      pipeline	= F 	- function was called by the CR event pipeline
#			  (i.e. return some additional information)
#      reset    = T     - reset corrdelays to default values before running
#Ref: pdiff2delay, delay2pdiff
#----------------------------------------------------------------------
phasecmp:=function(ref rec, 
           freqrange=[62165865.6,62223239.5,67670000,67712000,67915000,67947000],
           ants=-1, refant=1, disp=T, pipeline=F, reset=T){
  global globalpar;
  # globals #
  globalpar.corrected_ant  := [=];
  samplerate:=rec.headf('Samplerate')*rec.headf('SamplerateUnit');

  # parameters #
  if (ants==-1) {ants:=1:rec.len};
  nants:=len(ants);
  nrange := floor(len(freqrange)/2)

  # reset to default value #
  if (reset) { FixDelays(rec,ants); }

  # load #
  corrdelays:=rec.getary('Corrdelay')[ants]*samplerate;

# zenith stare...
  olddistance:=rec.get('Distance',refant);
  oldxy:=rec.get('XY',refant);
  oldazel:=rec.get('AZEL',refant);
  oldcenter:=rec.get('Center',refant);

  rec.put(0,'Distance',-1);
  rec.put([0,0],'XY',-1);
  rec.put([0,90],'AZEL',-1);
  rec.put([0,0,0],'Center',-1);

  rec.setphase(); # look towards zenith

  rec.put(refant,'RefAnt',-1);
  freq:=rec.get('Frequency',1);
  power := rec.get('Power',refant);
  peakpos := array(0.,nrange);
  peakfreq := array(0.,nrange);
  for (i in seq(nrange)){
    sequence := (freq>=freqrange[(2*i-1)]) & (freq<=freqrange[(2*i)]);
    if (sum(sequence)==0) {print "phasecmp: Incorrect frequency !!!"; return};
    peakpos[i] := maxpos(power*sequence);
  };
  peakfreq := freq[peakpos];

  # variables #
  dphi:=array(0.,nrange,nants);
  delays:=array(0.,nrange,nants);
  newdelays:=array(0.,nrange,nants);
  reldelays:=array(0.,nrange,nants);
  relphases:=array(0.,nrange,nants);
  reldelmean := array(0.,nants);
  flag := array(T,nants);
  # ants #
  for (combi in ants[ants!=refant]) {
    #take the phase difference at the peak position
    dphi[,combi] := rec.get('PhaseDiff',combi)[peakpos];

    # delays #
    delays[,combi]:=pdiff2delay(dphi[,combi],peakfreq);
  }; # end ant combis


#print 'Peak Positions:',peakpos,'Peak Frequencies:',peakfreq
#rawphaseeins := phase(rec.get('RawFFT',1)[peakpos])
#for (i in [1:nants]){
#print 'Ant:',i,'RawPhase:',phase(rec.get('RawFFT',i)[peakpos]),'RawPhaseDiff:',phase(rec.get('RawFFT',i)[peakpos])-rawphaseeins;
#}
#posdifeins := phase(rec.get('RawFFT',1)[peakpos]) - rec.get('Phase',1)[peakpos];
#for (i in [1:nants]){
#posdif := phase(rec.get('RawFFT',i)[peakpos]) - rec.get('Phase',i)[peakpos]-posdifeins;
#newphases := globalpar.tvphases[,i]+posdif
#newphases := newphases-(360*(newphases>180));
#newphases := newphases+(360*(newphases<-180));
#print 'Ant:',i,' posdif:',posdif,' TVPhase:',globalpar.tvphases[,i],' NewTVPhase:',newphases
#}


  # change #
  aseq := seq(nants);
  for (i in seq(nrange)) {
    relphases[i,] := dphi[i,]-globalpar.tvphases[i,aseq];
    relphases[i,] := relphases[i,]-(360*(relphases[i,]>180));
    relphases[i,] := relphases[i,]+(360*(relphases[i,]<-180));
    reldelays[i,] := pdiff2delay(relphases[i,],peakfreq[i])*samplerate;
#print i, ':',relphases[i,]
#print i, ':',reldelays[i,] 
  };
  maxdeldiff := array(0.,30)
  for (i in [1:nants]) { 
    maxdeldiff[i] := max(reldelays[,i])-min(reldelays[,i]);
  };
##------------------------
#print spaste('maxdeldiff [',sprintf('%6.3f,',maxdeldiff[1:10]),'');
#print spaste('            ',sprintf('%6.3f,',maxdeldiff[11:20]),'');
#print spaste('            ',sprintf('%6.3f,',maxdeldiff[21:30]),'] samples');
##------------------------

  #try to detect and correct multi sample jumps
  badwgh := 0.5
  if (nants > 10) { #a try to distinguish between LOPES10 and LOPES30
    diffarray := [-2,2,-1,1,-3,3];
  } else {
    diffarray := [-2,2,-16,16,-14,14,-18,18,-32,32];
  };
  for (ant in ants) {
    badness := (max(reldelays[,ant])-min(reldelays[,ant])) * (1-badwgh) +
                     mean(abs(reldelays[,ant])) * badwgh
    if (badness  >= 0.1) { 
      minbadness := badness;
      minrelphases:= relphases[,ant];
      minrelpos := 0;
      for (diff in diffarray){
        jphase := delay2pdiff(diff/samplerate,peakfreq);
        nrelphases := relphases[,ant]-jphase;
        nrelphases := nrelphases-360*as_integer(nrelphases/360);
        nrelphases := nrelphases-(360*(nrelphases>180));
        nrelphases := nrelphases+(360*(nrelphases<-180));
        nreldel := pdiff2delay(nrelphases,peakfreq)*samplerate;
        badness := (max(nreldel)-min(nreldel)) * (1-badwgh) +
                     mean(abs(nreldel)) * badwgh
        if (minbadness>badness+0.01) { 
secondbad := minbadness;
secondpos := minrelpos;
          minbadness := badness
          minrelphases:= nrelphases;
          minrelpos := diff;
        };
      };
      relphases[,ant] :=minrelphases;     
      reldelays[,ant] := pdiff2delay(relphases[,ant],peakfreq)*samplerate+minrelpos;
      if (minbadness >= 0.15) {
        flag[ant] := F;
        print 'Not trusting Ant:',ant,'with residual badness:',minbadness,'phases:',minrelphases;
      } else {
        globalpar.corrected_ant[len(globalpar.corrected_ant)+1]  := ant;
        if (disp) print 'Corrected Ant:',ant,':',minrelpos,minbadness,minrelphases,reldelays[,ant],secondbad,secondpos;
      };
    };
  };

  # revised change #
  for (i in ants) {
    if (flag[i]) reldelmean[i] := mean(reldelays[,i]);
  };

  rec.put(olddistance,'Distance',-1);  
  rec.put(oldxy,'XY',-1);  
  rec.put(oldazel,'AZEL',-1);  
  rec.put(oldcenter,'Center',-1);  

  newcorrdelays:=corrdelays-reldelmean;     # reldelays[1,];
  FixDelays(rec,ants,delays=newcorrdelays); # set hardware offset delays to new correction delays (Corrdelays)
  rec.setphase();                           # apply and also reset AZEL value to proper value

  # print #
  if (disp) {
    print spaste('tvphase1  [',sprintf('%6.1f,',dphi[1,]),'] degrees');
    print spaste('changeph1 [',sprintf('%6.1f,',relphases[1,]),'] degrees');
    print spaste('change1   [',sprintf('%6.2f,',reldelays[1,]),'] samples');
    print spaste('tvphase2  [',sprintf('%6.1f,',dphi[2,]),'] degrees');
    print spaste('changeph2 [',sprintf('%6.1f,',relphases[2,]),'] degrees');
    print spaste('change2   [',sprintf('%6.2f,',reldelays[2,]),'] samples');
    print spaste('tvphase3  [',sprintf('%6.1f,',dphi[3,]),'] degrees');
    print spaste('changeph3 [',sprintf('%6.1f,',relphases[3,]),'] degrees');
    print spaste('change3   [',sprintf('%6.2f,',reldelays[3,]),'] samples');
    print spaste('-----------------------------------------------------------------------------------');
    print spaste('meanphase [',sprintf('%6.1f,',(dphi[1,]+dphi[2,]+dphi[3,])/3),'] degrees');
    print spaste('meanchange[',sprintf('%6.2f,',reldelmean),'] samples');

    print spaste('act corr  [',sprintf('%6.2f,',corrdelays),'] samples');
    print spaste('new corr  [',sprintf('%6.2f,',newcorrdelays),'] samples');
  };

##------------------------
#maxdeldiff := array(0.,30)
#for (i in [1:nants]) { maxdeldiff[i] := max(reldelays[,i])-min(reldelays[,i]);};
#print spaste('maxdeldiff [',sprintf('%6.3f,',maxdeldiff[1:10]),'');
#print spaste('            ',sprintf('%6.3f,',maxdeldiff[11:20]),'');
#print spaste('            ',sprintf('%6.3f,',maxdeldiff[21:30]),'] samples');
##------------------------

  # return #
  if (pipeline) {
    erg := [=];
    erg.flag := flag;
    erg.dphi := dphi;
    return erg;
  };
  return flag;

} # end phasecmp
#######################################################################

### SUBFUNCTIONS ###
pdiff2delay:=function(pdiff,freq)
	{
		delays:=pdiff/180*pi/(2*pi*freq);
		return delays;
	}
delay2pdiff:=function(delay,freq)
	{
		pdiff:=2*pi*freq*delay*180/pi;
		return pdiff;
	}
### FUNCTION ###


getrephases := function(ref rec, transdir = [155.1,0],freqrange=[62165865.6,62223239.5,67673778,67712097,67925426,67946548],ants=-1, refant=1){
  global globalpar;
  # globals #
  globalpar.corrected_ant  := [=];
  samplerate:=rec.headf('Samplerate')*rec.headf('SamplerateUnit');

  # parameters #
  if (ants==-1) {ants:=1:rec.len};
  nants:=len(ants);
  nrange := floor(len(freqrange)/2)

  olddistance:=rec.get('Distance',refant);
  oldxy:=rec.get('XY',refant);
  oldazel:=rec.get('AZEL',refant);
  oldcenter:=rec.get('Center',refant);
#### look towards the tv-ransmitter
  rec.put(0,'Distance',-1);
  rec.put([0,0],'XY',-1);
  rec.put(transdir,'AZEL',-1);
  rec.put([0,0,0],'Center',-1);
  rec.setphase(); 

  rec.put(refant,'RefAnt',-1);
  freq:=rec.get('Frequency',1);
  power := rec.get('Power',refant);
  peakpos := array(0.,nrange);
  peakfreq := array(0.,nrange);
  for (i in seq(nrange)){
    sequence := (freq>=freqrange[(2*i-1)]) & (freq<=freqrange[(2*i)]);
    if (sum(sequence)==0) {print "phasecmp: Incorrect frequency !!!"; return};
    peakpos[i] := maxpos(power*sequence);
  };
  peakfreq := freq[peakpos];

  # variables #
  global dphi:=array(0.,nrange,nants);
  global delays:=array(0.,nrange,nants);
  # ants #
  for (combi in ants[ants!=refant]) {
    #take the phase difference at the peak position
    dphi[,combi] := rec.get('PhaseDiff',combi)[peakpos];

    # delays #
    delays[,combi]:=pdiff2delay(dphi[,combi],peakfreq);
  }; # end ant combis
}

phasecmp2:=function(ref rec, transdir = [155.1,0],
           freqrange=[62165865.6,62223239.5,67673778,67712097,67925426,67946548],
           ants=-1, refant=1, disp=T, pipeline=F, reset=T){
  global globalpar;
  # globals #
  globalpar.corrected_ant  := [=];
  samplerate:=rec.headf('Samplerate')*rec.headf('SamplerateUnit');

  # parameters #
  if (ants==-1) {ants:=1:rec.len};
  nants:=len(ants);
  nrange := floor(len(freqrange)/2)

  # reset to default value #
  if (reset) { FixDelays(rec,ants); };

  # load #
  corrdelays:=rec.getary('Corrdelay')[ants]*samplerate;

# zenith stare...
  olddistance:=rec.get('Distance',refant);
  oldxy:=rec.get('XY',refant);
  oldazel:=rec.get('AZEL',refant);
  oldcenter:=rec.get('Center',refant);

#### look towards the tv-ransmitter
  rec.put(0,'Distance',-1);
  rec.put([0,0],'XY',-1);
  rec.put(transdir,'AZEL',-1);
  rec.put([0,0,0],'Center',-1);

  rec.setphase(); 

  rec.put(refant,'RefAnt',-1);
  freq:=rec.get('Frequency',1);
  power := rec.get('Power',refant);
  peakpos := array(0.,nrange);
  peakfreq := array(0.,nrange);
  for (i in seq(nrange)){
    sequence := (freq>=freqrange[(2*i-1)]) & (freq<=freqrange[(2*i)]);
    if (sum(sequence)==0) {print "phasecmp: Incorrect frequency !!!"; return};
    peakpos[i] := maxpos(power*sequence);
  };
  peakfreq := freq[peakpos];

  # variables #
  dphi:=array(0.,nrange,nants);
  delays:=array(0.,nrange,nants);
  newdelays:=array(0.,nrange,nants);
  reldelays:=array(0.,nrange,nants);
  relphases:=array(0.,nrange,nants);
  reldelmean := array(0.,nants);
  flag := array(T,nants);
  # ants #
  for (combi in ants[ants!=refant]) {
    #take the phase difference at the peak position
    dphi[,combi] := rec.get('PhaseDiff',combi)[peakpos];

    # delays #
    delays[,combi]:=pdiff2delay(dphi[,combi],peakfreq);
  }; # end ant combis
  # change #
  aseq := seq(nants);
  for (i in seq(nrange)) {
    relphases[i,] := dphi[i,];
    relphases[i,] := relphases[i,]-(360*(relphases[i,]>180));
    relphases[i,] := relphases[i,]+(360*(relphases[i,]<-180));
    reldelays[i,] := pdiff2delay(relphases[i,],peakfreq[i])*samplerate;
  };

  #try to detect and correct multi sample jumps
  for (ant in ants) {
    if (sum(abs(relphases[,ant]))>150) { ###give +/- 50 deg grace per frequency
      minrelsum := sum(abs(relphases[,ant]),(max(relphases[,ant])-min(relphases[,ant])))
      minrelphases:= relphases[,ant];
      minrelpos := 0;
#      for (diff in [-2,2,-16,16,-14,14,-18,18,-32,32]){
      for (diff in [-32:32]){
       jphase := delay2pdiff(diff/samplerate,peakfreq);
        nrelphases := relphases[,ant]-jphase;
        nrelphases := nrelphases-360*as_integer(nrelphases/360);
        nrelphases := nrelphases-(360*(nrelphases>180));
        nrelphases := nrelphases+(360*(nrelphases<-180));
        abssum := sum(abs(nrelphases),(max(nrelphases)-min(nrelphases))); 
        if (minrelsum>abssum) { 
          minrelsum := abssum;
          minrelphases:= nrelphases;
          minrelpos := diff;
        };
      };
      relphases[,ant] :=minrelphases;     
      reldelays[,ant] := pdiff2delay(relphases[,ant],peakfreq)*samplerate+minrelpos;
      if (minrelsum>100) {
        flag[ant] := F;
        print 'Not trusting Ant:',ant,'with residual phases:',minrelphases;
      } else {
        globalpar.corrected_ant[len(globalpar.corrected_ant)+1]  := ant;
        if (disp) print 'Corrected Ant:',ant,':',minrelpos,minrelphases,minrelsum,reldelays[,ant];
      };
    };
  };

  # revised change #
  for (i in ants) {
    if (flag[i]) reldelmean[i] := mean(reldelays[,i]);
  };

  rec.put(olddistance,'Distance',-1);  
  rec.put(oldxy,'XY',-1);  
  rec.put(oldazel,'AZEL',-1);  
  rec.put(oldcenter,'Center',-1);  

  newcorrdelays:=corrdelays-reldelmean;     # reldelays[1,];
  FixDelays(rec,ants,delays=newcorrdelays); # set hardware offset delays to new correction delays (Corrdelays)
  rec.setphase();                           # apply and also reset AZEL value to proper value

  # print #
  if (disp) {
    print spaste('tvphase1  [',sprintf('%6.1f,',dphi[1,]),'] degrees');
    print spaste('changeph1 [',sprintf('%6.1f,',relphases[1,]),'] degrees');
    print spaste('change1   [',sprintf('%6.2f,',reldelays[1,]),'] samples');
    print spaste('tvphase2  [',sprintf('%6.1f,',dphi[2,]),'] degrees');
    print spaste('changeph2 [',sprintf('%6.1f,',relphases[2,]),'] degrees');
    print spaste('change2   [',sprintf('%6.2f,',reldelays[2,]),'] samples');
    print spaste('tvphase3  [',sprintf('%6.1f,',dphi[3,]),'] degrees');
    print spaste('changeph3 [',sprintf('%6.1f,',relphases[3,]),'] degrees');
    print spaste('change3   [',sprintf('%6.2f,',reldelays[3,]),'] samples');
    print spaste('-----------------------------------------------------------------------------------');
    print spaste('meanphase [',sprintf('%6.1f,',(dphi[1,]+dphi[2,]+dphi[3,])/3),'] degrees');
    print spaste('meanchange[',sprintf('%6.2f,',reldelmean),'] samples');
    print spaste('act corr  [',sprintf('%6.2f,',corrdelays),'] samples');
    print spaste('new corr  [',sprintf('%6.2f,',newcorrdelays),'] samples');
  };


  # return #
  if (pipeline) {
    erg := [=];
    erg.flag := flag;
    erg.dphi := dphi;
    return erg;
  };
  return flag;

} # end phasecmp2



#----------------------------------------------------------------------
#Name: dirtoazel
#-----------------------------------------------------------------------
#Type: Function
#Doc: Convertes a direction for a source, planet, or direction from one
#     epoch to another using the frame set in a data record.
#
#Ret: Vector with direction infomation (long,lat or Az,El) in degrees.
#
#Par: rec =	  		    - data record
#Par: index =	  		- index of antenna/data record
#Par: source = F        - if set, calculate direction for source name 
#                         from default catalog (e.g., source="3C273")
#Par: planet = F        - if set, calculate direction for planet name 
#                         from default catalog (e.g., source="Sun")
#Par: coord = F        - if set, calculate direction for direction 
#                         from default catalog (e.g., coord=["0deg","90deg"])
#Par: epoch ="J2000"    - input epoch
#Par: toepoch ="AZEL"   - output epoch
#Ref: meastoazel
#-----------------------------------------------------------------------
dirtoazel:=function(ref rec, index=1,source=F,planet=F,coord=F,epoch='J2000',toepoch='AZEL',prettystring=F){
  dm.doframe(dm.epoch('utc',qnt.quantity(rec.head('Date',index))))
  dm.doframe(dm.observatory(rec.head('Observatory')));
  if (coord) {sdir:=dm.measure(dm.direction(epoch,coord[1],coord[2]),toepoch)}
  else if (source) {sdir:=dm.measure(dm.source(source),toepoch)}
  else if (planet) {sdir:=dm.measure(dm.direction(planet),toepoch)}
  else return [0,90]
  if (prettystring) {
    return [dq.time(sdir.m0),dq.angle(sdir.m1)]
  } else {
   return [sdir.m0.value,sdir.m1.value]/pi*180
  }
}


#----------------------------------------------------------------------
#Name: meastoazel
#-----------------------------------------------------------------------
#Type: Function
#Doc: Convertes a direction for a source given in a measure record
#     using the frame set in a data record.
#
#Ret: Vector with direction infomation (long,lat or Az,El) in degrees.
#
#Par: rec 	  		    - data record
#Par: index 	  		- index of antenna/data record
#Par: dir               - measure 
#Ref: dirtoazel
#-----------------------------------------------------------------------
meastoazel:=function(ref rec, dir ,index=1){
  dm.doframe(dm.epoch('utc',qnt.quantity(rec.head('Date',index))))
  dm.doframe(dm.observatory(rec.head('Observatory')));
  sdir:=dm.measure(dir,'AZEL')
  return [sdir.m0.value,sdir.m1.value]/pi*180
}


########################################################################
#Section: Calculation of Phases and Delays
########################################################################

#----------------------------------------------------------------------
#Name: calcphasegradrec
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Calculates the phase gradient for a specific record to be
#      mutliplied later to the FFT data.
#Par:  rec            - Reference to the data structure.
#Par:  field = 'FFT'  - field for which to calculate the phasegradient
#Par:  index =        - index of antenna/data record
#Ret:  Array with phases phi, for shifting multiply with exp(i*phi)
#-----------------------------------------------------------------------

calcphasegradrec := function(ref rec, index=1, tozenith=F)
{
  # get variables from data structure
  refant   := rec.get('RefAnt',index);
  doz      := rec.get('DoZ',index);
  distance := rec.get('Distance',index);
  xy       := rec.get('XY',index);

  # get 3-dim antenna positions
  if (xy==[0,0]) antpos := CalcAntPos(rec,index,refant)[1]
  else antpos:=CalcAntPos(rec,index,refant,refpos=[xy[2],xy[1],0],phasecenter=T)[1]

  # Frequency information
  fbandw  := rec.headf('Bandwidth',index)*rec.headf('FrequencyUnit',index);
  flowlim := rec.getmeta('FrequencyLow',index);
  fftlen  := rec.getmeta('FFTlen',index)

  # pointing direction for which the delays are computed
  if (tozenith) {azel:=[0,90]} else {azel:=rec.get('AZEL',index)};

  delay:=calcdelay(azel,antpos,doz,distance=distance)+rec.get('Corrdelay',index)+rec.get('Caldelay',index); 
#  print '[',index,'] tau=',delay*1e9/12.5,'Samples, tau=',delay*1e9,'ns',azel,tozenith
#  print 'AZEL=',azel,'Offset=',offset,'Distance=',distance,' DoZ=',doz
#  print 'Antpos=',antpos
#  print 'geodelay=',calcdelay(azel,antpos,doz,distance=distance,offset=offset)*1e9/12.5,' Corrdelay=',rec.get('Corrdelay',index)*1e9/12.5,'Caldelay=',rec.get('Caldelay',index)*1e9/12.5,' (unit=12.5 ns)'
  rec.put(delay,'Delay',index)
  return calcphasegrad(delay,fftlen,flowlim=flowlim,fbandw=fbandw)
}


#-----------------------------------------------------------------------
#Name: calcphasegrad
#-----------------------------------------------------------------------
#Type: Function
#Doc: Calculates the phase gradient for certain parameters to be mutliplied
#     later to the FFT data to shift the data. For shifting an array of
#     FFT data, [fft], use
#     <ul><tt>[fft] -> [shifted] = exp(-1i*[phi]*degree)</tt></ul>
#     For an example see 'TIM40revgetFFT'
#Ret: array with phases phi
#    
#
#Par: delay =           - light travel time delay between antennas
#Par: fftlen =          - Length of FFT vector
#Par: flowlim =         - Start of Frequency range
#Par: fbandw =          - Frequency bandwidth (to determine range)
#Todo: Use data,get() mechanism to obtain the frequency values instead
#      of local computation.
#-----------------------------------------------------------------------
calcphasegrad:=function(delay,fftlen,flowlim=40e6,fbandw=40e6)
{
  #Note, there is a discrepancy with Andreas here!!!!!!
  #He did not use the "-1" for fftlen !
  #Strangely, rec.get('Frequency') starts with 40 MHz, which is not quite right.

  fsteps:=fbandw/(fftlen-1);
  phase:= 2*pi/degree*delay*(flowlim+[0:(fftlen-1)]*fsteps);

  return phase
}

#-----------------------------------------------------------------------
#Name: toShower
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Calculates the position of an antenna in shower-coordinates. The
#      calculation is done like in Bronstein chapter 3.5.3.1 3. 2.-4.
#      (pages 208f in Tims book), where
#      <ul>
#      <li>(x',y',z') are lab coordinates (x'=north, y'=east, z'=up),
#      <li>(x,y,z) are shower coordinates (x is in the x',y'-plane, z=air
#          shower axis, y=undefined),
#      <li>Euler angles are: theta= 90-el, phi=90-az, psi= undefined, set
#      to psi=0
#      </ul>
#
#Ret:  3-dim position of the antenna in shower-coordinates
#
#Par:  azel   =       - Direction in which to look [az,el] in degree
#Par:  antpos =	      - Antenna position relative to the shower center
#Par:  doz    =       - Use the z-coordinates of the antenna
#Par:  verboseON = F  - Be verbose during computation; this can be used e.g 
#                       for debugging purposes
#-----------------------------------------------------------------------

toShower := function(azel,antpos,doz=T,verboseON=F)
{
  az:=azel[1]*degree;
  el:=azel[2]*degree;

  erg := [=];

  l1 := sin(az);
  l2 := -cos(az);
  l3 := 0.;
  erg.x := -(antpos.position[1]*l1+antpos.position[2]*l2+antpos.position[3]*l3);

  m1 := sin(el)*cos(az);
  m2 := sin(el)*sin(az);
  if (doz) {m3:=-cos(el)} else {m3:=0.}
  erg.y := (antpos.position[1]*m1+antpos.position[2]*m2+antpos.position[3]*m3);

  n1 := cos(el)*cos(az);
  n2 := cos(el)*sin(az);
  if (doz) {n3:=sin(el)} else {n3:=0.}
  erg.z := (antpos.position[1]*n1+antpos.position[2]*n2+antpos.position[3]*n3);

  if (verboseON) {
    print "[phasing::toShower]"
    print " - Direction (az,el) .. :",azel;
    print " - Direction (x,y,z) .. :",az,el;
    print " - Antenna position ... :",antpos;
    print " - use z-coordinate ... :",doz;
    print " - (l1,l2,l3) ......... :",l1,l2,l3;
    print " - (m1,m2,m3) ......... :",m1,m2,m3;
    print " - (n1,n2,n3) ......... :",n1,n2,n3;
    print " - Result ............. :",erg;
  }

  return erg;
};

#-----------------------------------------------------------------------
#Name: calcdelay
#-----------------------------------------------------------------------
#Type: Function
#Doc: Calculates the delay between two antennas for a given direction
#
#Ret: light travel time delay in seconds
#
#Par: azel     =    - direction in which to look [az,el] in degree
#Par: antpos   =    - antenna positions relative to a reference antenna
#Par: doz      =    - use the z-coordinates of the antenna
#Par: distance =-1  - distance to the source in meters (used if >0)
#Ref: toShower
#-----------------------------------------------------------------------

calcdelay := function(azel,antpos,doz=T,distance=-1)
{
  c:=lightspeed;
  deld:=0.

  spos := toShower(azel,antpos,doz);

  if (distance>0) {
    deld:=sqrt((distance-spos.z)^2+spos.x^2+spos.y^2)-distance+spos.z
  }
  return (deld-spos.z)/c;
}

#-------------------------------------------------------------------------------
#Name: LightTravelTimeDelay
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Compute the time travel delay for an antenna position relative to the 
#      array phase center and a given source direction.
#Ret:  Light time travel delay, [sec].
#Par:  azel =	  	- direction in which to look [az,el] in degree
#Par:  antpos =	  	- antenna positions relative to a reference antenna
#Par:  doz =	  	- use the z-coordinates of the antenna
#Par:  distance =-1     - distance to the source in meters (used if >0)
#-------------------------------------------------------------------------------

LightTravelTimeDelay := function (azel,antpos,doz=T,distance=-1)
{
  c := lightspeed;
  d := 0;

  # Conversion from (az,el) to cartesian coordinates
  rho := azelToCartesian (azel,distance);

  if (distance>0) {
    d := L2Norm(rho-antpos) - L2Norm(rho);
  } else {
    d := sum(rho*antpos);
  }

  return d/c;

}

#-------------------------------------------------------------------------------
#Name: azelToCartesian
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Conversion of spherical coordinates, (az,el), to cartesian coordinates,
#      (x,y,z).
#Par:  azel =	  	- direction in which to look [az,el] in degree
#Par:  distance =-1     - distance to the source in meters (used if >0)
#-------------------------------------------------------------------------------

azelToCartesian := function (azel,distance=-1) {

  azel *:= degree;
  r := distance;

  if (!distance) r := 1;

  rho[1] := r*cos(azel[2])*cos(azel[1]);
  rho[2] := r*cos(azel[2])*sin(azel[1]);
  rho[3] := r*sin(azel[2]);

  return rho;
} 

#-------------------------------------------------------------------------------
#Name: L2Norm
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Computation of the L2-Norm (Euclidean norm) of a vector.
#-------------------------------------------------------------------------------

L2Norm := function (ary) { return sqrt(sum(ary*ary)); }


########################################################################
#Section: Various Schemes to calibrate the phases of the station
########################################################################

#-------------------------------------------------------------------------------
#Name: calibratedelays
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Finds the correct delays by maximizing the crosscorrelation at 0
#      This is appropriate if the noise is dominated by a single source,
#      i.e. during a solar burst.
#
#Ret:  array with phases phi, for shifting multiply with exp(i*phi*degree)
#
#-------------------------------------------------------------------------------

calibratedelays := function(ref rec,field='f(x)',index=-1,substeps=20,zeropos=-1)
{
  if (index<0) {idx:=seq(rec.len)} else {idx:=index};
  if (zeropos<0) {zeropos:=len(rec.get(field,rec.get('RefAnt',1)))/2+1}
  step:=1./substeps
  samplerateunit:=rec.headf('SamplerateUnit',1);
  tunit:=1/(rec.headf('Samplerate',1)*samplerateunit);
  for (i in idx) {
  maxima:=array(0.,substeps*2)
  minima:=array(0.,substeps*2)
  pos:=array(0.,substeps*2)
  posmin:=array(0.,substeps*2)
  p:=0; n:=0
  if (i != rec.get('RefAnt',i)) {
    for (j in seq(0,1-step,1./substeps)) { # 0:substeps:1
      n+:=1;
      rec.put(j*tunit,'Caldelay',i)
      rec.setphase(index=i);

      if (rec.ref == F ) {
        maxima[n]:=max_with_location(rec.aux.get('CrossCorr',i),p)
        pos[n]:=p-j;
        minima[n]:=min_with_location(rec.aux.get('CrossCorr',i),p)
      } else {
        maxima[n]:=max_with_location(rec.get('CrossCorr',i),p)
        pos[n]:=p-j;
        minima[n]:=min_with_location(rec.get('CrossCorr',i),p)
     }
    }
  maxima2:=maxima+minima
  mx:=max_with_location(maxima2,nmax)
#  print i,'Maxstep',nmax,'Max=',mx
#  print 'pos=',pos-zeropos,' max=',maxima2
  delay[i]:=-(pos[nmax]-zeropos)*tunit
  } else {delay[i] := 0}
  print spaste('[',i,'] ',delay[i]/tunit,' samples  ',delay[i]*1e9,' ns');
  rec.put(delay[i],'Caldelay',i)
  #rec.setphase(index=i)
  }
	delay[abs(delay/tunit)>35]:=0;
	sdelays:=sundelays(rec,unit=1);
  print spaste('thedelay   [',sprintf('%5.2f,',sdelays/tunit),'] samples')
	print spaste('caldelay   [',sprintf('%5.2f,',delay/tunit),'] samples')
  print spaste('difference [',sprintf('%5.2f,',(delay-sdelays)/tunit),'] samples')
}

#-----------------------------------------------------------------------
#Name: calibratedelayscr
#-----------------------------------------------------------------------
#Type: Function
#Doc: Finds the correct delays by maximizing the crosscorrelation at 0
#     for a sub-range of the time data. This is appropriate for a
#     cosmic ray event
#
#Ret: array with phases phi, for shifting multiply with exp(i*phi*degree)
#
#-----------------------------------------------------------------------
calibratedelayscr:=function(field='Voltage',index=-1,x1=1,x2=100,substeps=5,refant=1){
  global data
  if (index<0) {idx:=seq(rec.len)} else {idx:=index};
  if (abs(x1-x2)%2==1) {x2b:=x2+1} else {x2b:=x2}

  print 'Reference Antenna:',refant
  print 'Calibrating on Pulse:',idx
  print 'Field=',field,'x1=',x1,'x2=',x2
  step:=1./substeps
  caldelays:=data.getary('Caldelay');
  for (i in idx) {
    maxima:=array(0.,substeps*2)
    minima:=array(0.,substeps*2)
    pos:=array(0.,substeps*2)
    posmin:=array(0.,substeps*2)
    p:=0; n:=0
    tunit:=1/(data.headf('Samplerate',i)*data.headf('SamplerateUnit',i))
    if (i != refant) {
    for (j in seq(-1,1-step,1./substeps)) {
      n+:=1;
      data.put(caldelays[i]+j*tunit,'Caldelay',i)
      data.setphase(index=i);
      ary1:=data.aux.get(field,refant,start=x1,end=x2b)
      ary2:=data.aux.get(field,i,start=x1,end=x2b)
      cc:=global_fftserver.crosscorr(ary1,ary2)
      maxima[n]:=max_with_location(cc,p)
      pos[n]:=p-j;            
      minima[n]:=min_with_location(cc,p)
#      print n,maxima[n],pos[n],minima[n]
    }
  maxima2:=maxima+minima
  mx:=max_with_location(maxima2,nmax)
  delay:=-(pos[nmax]+(x1-x2b)/2-1)*tunit
  } else {delay := 0}
  print '[',i,']','Caldelay=',delay/tunit,'Samples',' Caldelay=',delay*1e9,'ns'
  data.put(caldelays[i]+delay,'Caldelay',i)
  data.setphase(index=i)
  }
  print 'Delays:',(data.getary('Caldelay')-caldelays)/12.5e-9
}

########################################################################
#Section: LOPES-specfic calibration on the TV-transmitter
########################################################################


#-----------------------------------------------------------------------
#Name: gettvphasearray
#-----------------------------------------------------------------------
#Type: Function
#Doc: generate an array with the relative phases of the TV-transmitter
#Ret: record with:
#       GT	- (1-dim array) Globaltime of the event
#       dphi	- (3-dim array) relative phases: 
#		  1st-dim: events
#		  2nd-dim: frequencies (currently 3)
#		  3rd-dim: antennas
#
#Par: rec       =	- Data record to identify dataset
#Par: files     =	- pathnames to the event-files
#Par: freqrange =	- array with lower and upper frequencies of the
#			  ranges to look at
#Par: ants      = -1	- number of antennas (-1: determine from first event)
#Par: refant    = 1	- index of the reference antenna
#-----------------------------------------------------------------------
gettvphasearray := function(ref rec, files,
           freqrange=[62165865.6,62223239.5,67673778,67712097,67925426,67946548],
           ants=-1, refant=1){
  global globalpar;
  input_event(rec,files[1]);
  # globals #
  samplerate:=rec.headf('Samplerate',refant)*rec.headf('SamplerateUnit',refant);

  # parameters #
  if (ants==-1) {ants:=1:rec.len};
  nants:=len(ants);
  nrange := floor(len(freqrange)/2);
  nofiles := len(files);

  # variables #
  erg := [=];
  erg.GT := array(0,nofiles);
  erg.dphi:=array(0.,nofiles,nrange,nants);
  peakpos := array(0.,nrange);
  peakfreq := array(0.,nrange);

  for (fno in seq(nofiles)){
    # load #
    input_event(rec,files[fno]);
print files[fno]
    erg.GT[fno] := rec.head('JDR',1);

    #get data#
    freq:=rec.get('Frequency',1);
    power := rec.get('Power',refant);
    for (i in seq(nrange)){
      sequence := (freq>=freqrange[(2*i-1)]) & (freq<=freqrange[(2*i)]);
      if (sum(sequence)==0) {print "phasecmp: Incorrect frequency !!!"; return};
      peakpos[i] := maxpos(power*sequence);
    };
    peakfreq := freq[peakpos];

    ## important ## 
    rec.put(0,'Distance',-1);  
    rec.put([0,0],'XY',-1);  
    rec.put([0,90],'AZEL',-1);  
    rec.put([0,0,0],'Center',-1);  
    rec.put(refant,'RefAnt',-1);
    rec.setphase(); # now looks towards zenith

    # ants #
    for (combi in ants[ants!=refant]) {
      #take the phase difference at the peak position
      erg.dphi[fno,,combi] := rec.get('PhaseDiff',combi)[peakpos];
    }; # end ant combis
    if ((fno%50)==0) {print 'done with',fno,'out of',nofiles,'Files',jdr2string();};
  };# end "for (fno in seq(nofiles))"

  # return #
  return erg;
}

#-----------------------------------------------------------------------
#Name: mergetvphasearrays
#-----------------------------------------------------------------------
#Type: Function
#Doc: merge several relative-phases-arrays (see function gettvphasearray)
#     and sort the events acording to their time
#Ret: record with merged and sorted data
#
#Par: files     =	- pathnames to the data-files
#-----------------------------------------------------------------------
mergetvphasearrays := function(files){
  dump := [=];
  ndat := 0;
  for (fname in files){
    dump[fname] := read_value(fname);
    ndat := ndat + len(dump[fname].GT);
  };
  allgt := array(0,ndat);
  alldphi := array(0.,ndat,3,30);
  pos := 1;
  for (fname in files){
    endpos := pos+len(dump[fname].GT)-1;
    allgt[pos:endpos] := dump[fname].GT;
    alldphi[[pos:endpos],,] := dump[fname].dphi;
    pos := pos+len(dump[fname].GT);
  };
  indi := order(allgt);
  erg.GT := allgt[indi];
  erg.dphi := alldphi[indi,,];
  return erg;
};

#-----------------------------------------------------------------------
#Name: tvphase2delayarray
#-----------------------------------------------------------------------
#Type: Function
#Doc: convert relative phases to delay offsets
#Ret: old record with added fields:
#      days 	- (fractional) days since 2004/01/01/00:00:00.000
#      delays 	- phasedifference to the reference values converted 
#		  to delays in samples
#
#Par: inrec =		- input record (from gettvphasearray)
#Par: freq =		- frequencies at which the phases were taken
#-----------------------------------------------------------------------
tvphase2delayarray := function(inrec,freq=[62193900,67692300,67936100],debug=F){
  noent := shape(inrec.dphi)[1];
  nofreq := shape(inrec.dphi)[2];
  noants := shape(inrec.dphi)[3];
  inrec.days := (inrec.GT-1.0729152e+09)/3600/24;#days sinc 1.1.2004
  inrec.relphases := array(0.,noent,nofreq,noants);
  inrec.meandelays := array(0.,noent,noants);
  inrec.flag := array(T,noent,noants);
  if (debug){
    inrec.delays := array(0.,noent,nofreq,noants);
    inrec.minrelsum := array(0.,noent,noants);
  };
  for (i in seq(noent)){
#    inrec.relphases[i,,] := inrec.dphi[i,,]-globalpar.tvphases;
    inrec.relphases[i,,] := inrec.dphi[i,,];
  };
  inrec.relphases := inrec.relphases+360*(inrec.relphases<-180);
  inrec.relphases := inrec.relphases-360*(inrec.relphases>+180);
  samplerate := 80e6; #######samplerate hardcoded, not nice, but I'm lazy!
  #try to detect and correct multi sample jumps
  for (i in seq(noent)){
    for (ant in seq(noants)) {
      if (sum(abs(inrec.relphases[i,,ant]))>150) { ###give +/- 50 deg grace per frequency
        minrelsum := sum(abs(inrec.relphases[i,,ant]),(max(inrec.relphases[i,,ant])-min(inrec.relphases[i,,ant])));
        minrelphases:= inrec.relphases[i,,ant];
        minrelpos := 0;
#        for (diff in [-2,2,-16,16,-14,14,-18,18,-32,32]){
        for (diff in [-32:32]){
          jphase := delay2pdiff(diff/samplerate,freq);
          nrelphases := inrec.relphases[i,,ant]-jphase;
          nrelphases := nrelphases-360*as_integer(nrelphases/360);
          nrelphases := nrelphases-(360*(nrelphases>180));
          nrelphases := nrelphases+(360*(nrelphases<-180));
          abssum := sum(abs(nrelphases),(max(nrelphases)-min(nrelphases))); 
          if (minrelsum>abssum) { 
            minrelsum := abssum;
            minrelphases:= nrelphases;
            minrelpos := diff;
          };
        };
        inrec.relphases[i,,ant] :=minrelphases;     
        delays:=pdiff2delay(minrelphases,freq)*samplerate+minrelpos;
        inrec.meandelays[i,ant] := mean(delays);
        if (minrelsum>100) {
          inrec.flag[i,ant] := F;
          if (debug) print 'Flagged Ant:',ant,'at day:',inrec.days[i],minrelpos,minrelphases,minrelsum,inrec.delays[i,,ant];
        };
        if (debug){ 
          inrec.delays[i,,ant] := delays;
          inrec.minrelsum[i,ant] := minrelsum;
        };
      } else {
        inrec.meandelays[i,ant] := mean(pdiff2delay(inrec.relphases[i,,ant],freq))*samplerate;
        if (debug) inrec.delays[i,,ant] := pdiff2delay(inrec.relphases[i,,ant],freq)*samplerate;
      };
    };
  };

  return inrec;
};

########################################################################
#Section: Calibration on point sources, using images as starting point
########################################################################


#-----------------------------------------------------------------------
#Name: calibrate_phases_on_pointsource
#-----------------------------------------------------------------------
#Type: Function
#Doc: This function assumes that the power in a certain frequency interval
#     is dominated by a single point source in a given direction. It computes
#     the average residual phase difference with respect to a reference antenna.
#     The position of the point source is assumed to be in data.get('AZEL') and 
#     the phases are set to point a beam in this direction. The "Caldelay" 
#     values will be set with the correction values if setdelays is T.
#
#Par: rec                   - data set to use
#Par: index=1               - antenna number to use (-1:all)
#Par: frequency=[50e6,60e6] - Frequency range to use in MHz
#Par: setdelays=T           - If T the "Caldelay" (data.get("Caldelay"))
#     values will be set with the correction values.
# 
#Ret: array with delays in units of Second
#Ref: find_peak_in_image, selfcal_on_image, calibrate_on_brightest_transmitters
#-----------------------------------------------------------------------
calibrate_phases_on_pointsource:=function(ref rec,index=-1,frequency=[50e6,60e6], setdelays=T){
  if (index<0) {idx:=seq(rec.len)[rec.select()]} else {idx:=index};
  samplerateunit:=rec.headf('SamplerateUnit',1);
  tunit:=1/(rec.headf('Samplerate',1)*samplerateunit);
  phases:=array(0,rec.len); delays:=phases;
  n1:=rec.getxn(frequency[1],'Frequency');
  n2:=rec.getxn(frequency[2],'Frequency');
  mfreq:=mean(rec.get('Frequency',rec.get('RefAnt',1))[n1:n2]);
  print 'Frequency range for Self-Cal:',frequency/1e6,'MHz (avg:',mfreq/1e6,'MHz)'
  for (i in idx) {
    if (setdelays) {rec.put(0,'Caldelay',i)};
    rec.setphase(index=i);
#    phases phi, for shifting multiply with exp(i*phi*degree)
    phases[i]:=-mean(rec.get('PhaseDiff',i)[n1:n2])
    delays[i]:=phases[i]*degree/(2*pi*mfreq)
    printf ('%s%2.2i%s%6.2f%s%5.2f%s%5.1f%s\n','Offset [',i,']: ',phases[i],'°, ',delays[i]/tunit,' samples, ',delays[i]*1e9,' ns');
    if (setdelays) {rec.put(delays[i],'Caldelay',i)};
    }
    return delays
}

#-----------------------------------------------------------------------
#Name: find_peak_in_image
#-----------------------------------------------------------------------
#Type: Function
#Doc: This function finds the maximum point in an image, and fits the
#     position of the source. The position will be stored in 'AZEL' and
#     and phases will be set to steer antennas into that direction.
#
#Par: rec               - data record to set frame and put direction
#Par: img               - imagetool to search peak in
#Par: azelmin=[-360,0] - restrict the coordinates of the points source to be
#                      above azelmin in AZEL. The first value is azimuth in 
#                      degrees, the second value elevation in degrees. If
#                      a position below this limit is found the respective values
#                      are set to the respective value in azelmin.
#Par: azelmax=[360,90] - restrict the coordinates of the points source to be
#                      below azelmax in AZEL. The first value is azimuth in 
#                      degrees, the second value elevation in degrees. If
#                      a position above this limit is found the respective values
#                      are set to the respective value in azelmax.
#
#Ret: 
#-----------------------------------------------------------------------
find_peak_in_image:=function(ref rec, ref img,azelmin=[-360,0],azelmax=[360,90]) {
  cl:=img.maxfit(point=T,list=T);
  rec.doframe();
  dir:=dm.measure(cl.getrefdir(1),'AZEL');
  azel:=[max(min(dq.convert(dm.getvalue(dir)[1],'deg').value,azelmax[1]),azelmin[1]),max(min(dq.convert(dm.getvalue(dir)[2],'deg').value,azelmax[2]),azelmin[2])];
#  print '"weird WCS rotation" fix for Heino\'s PC! Added 180 degrees in find_peak_in_imag!'
#  azel[1]:=(azel[1]+180)%360;
  if (azel[1]<0) {azel[1]:=360+azel[1]};
  print 'Found source at AZEL = ',azel
  rec.put(azel,'AZEL',-1);
  return azel
}

#-----------------------------------------------------------------------
#Name: selfcal_on_image
#-----------------------------------------------------------------------
#Type: Function
#Doc: This function calibrates the phase by picking the brightest source in 
#      an image, assuming that the power in a certain frequency interval
#     is dominated by this single point source. It computes
#     the average residual phase difference with respect to a reference antenna.
#
#Par: rec                   - data record to set frame and put direction
#Par: img                   - imagetool to search peak in
#Par: frequency=[50e6,60e6] - Frequency range to use in MHz
#Par: setdelays=T           - If true the "Caldelay" (data.get("Caldelay"))
#                             values will be set using the calculated 
#                             correction values.
# 
#Ref: find_peak_in_image, calibrate_phases_on_pointsource
#-----------------------------------------------------------------------
selfcal_on_image:=function(ref rec, ref img,frequency=[50e6,60e6],setdelays=T) {
  find_peak_in_image(rec,img);
  return calibrate_phases_on_pointsource(rec,index=-1,frequency=frequency,setdelays=setdelays);
}

#-----------------------------------------------------------------------
#Name: find_brightest_transmitters
#-----------------------------------------------------------------------
#Type: Function
#
#Doc: This function splits the frequency in "nband" logarithmically spaced 
#     frequency intervals (subbands). Within each subband the brightest spike
#     (asumeded to be an RFI transmitter) is located, provided the spike is a
#     factor "threshold" above the mean power in this band.
#
#Par: rec - data set to use
#Par: index=1 - antenna number to use
#Par: nbands=10 - the number of logarithmically spaced frequency intervals/bands
#Par: freqlow=10 - Frequency in "FrequencyUnit" untis (typically MHz) where to start
#                  the subbands.
#Par: freqhigh=32 - Frequency in "FrequencyUnit" untis (typically MHz) where to end
#                  the subbands.
#Par: threshold=100 - determines the factor by which the spike has to exceed the noise
#     in order to be considered and returned
#
#Ret: peaks - vector of peak frequencies
#Ref: calibrate_on_brightest_transmitters
#-----------------------------------------------------------------------
find_brightest_transmitters:=function(ref rec,index=1,nbands=10,
                                      freqlow=10,freqhigh=33,threshold=100){
  freqUnit := rec.headf("FrequencyUnit",index);
  bands:=freqlow*freqUnit*10^(log(freqhigh/freqlow)/nbands*seq(0,nbands));
  peaks:=array(-1,nbands);
  nf:=0;
  n10:=rec.getxn(freqlow*freqUnit,"Frequency");
  n20:=rec.getxn(freqhigh*freqUnit,"Frequency");
  mn:=median(rec.get('Power',index)[as_integer(n10+(n20-n10)*0.5):n20]);
  print 'Cutoff Level =',threshold*mn,threshold
  for (i in seq(nbands)) {  
    f1:=bands[i];  
    f2:=bands[i+1];
    n1:=rec.getxn(f1,"Frequency");
    n2:=rec.getxn(f2,"Frequency");
    mx:=max_with_location(rec.get('Power',index)[n1:n2],pkn);
    pkn:=n1+pkn-1;
    if (mx>threshold*mn) {
      nf+:=1;
      peaks[nf]:=rec.get('Frequency',index)[pkn];
      }
    }
  print "Frequency Bands [MHz]:",roundn(bands/1e6,2)
  print "Peaks           [MHz]:",roundn(peaks/1e6,4)
  return peaks[1:nf]
}

#-----------------------------------------------------------------------
#Name: cal_on_brightest_transmitters
#-----------------------------------------------------------------------
#Type: Function
#
#Doc: This function calibrates station phases across the frequencyband by
#imaging of and self-calibrating on the brightest interference sources at the
#horizon. It splits the band in frequency intervals (subbands). Within each
#subband the brightest spike (asumeded to be an RFI transmitter) is located,
#provided the spike is a factor "threshold" above the mean power in this band.
#The basic assumption is that the source at this frequency is the dominating
#pointsource in the map. The map is made of "nchannel" adjacent channels around 
#the central channel. To narrow done the search you should make a smaller map
#with the expected position as the center.
#
#
#Example: c:=cal_on_brightest_transmitters(data,1,nbands=2,freqlow=50,freqhigh=70)
# c:=cal_on_brightest_transmitters(data,1,nbands=100,nchannels=1,threshold=0.005,freqhigh=30)
#c:=cal_on_brightest_transmitters(data,1,nbands=1,freqlow=60,freqhigh=70,nchannels=3,center=[153,0],pixels=30,pixelsize=1)
#Par: rec - data set to use
#Par: index=1 - antenna number to use
#Par: nbands=10 - the number of logarithmically spaced frequency intervals/bands
#Par: freqlow=10 - Frequency in "FrequencyUnit" untis (typically MHz) where to start
#                  the subbands.
#Par: freqhigh=33 - Frequency in "FrequencyUnit" untis (typically MHz) where to end
#                  the subbands.
#Par: threshold=100 - determines the factor by which the spike has to exceed the noise
#     in order to be considered and returned
#Par: nchannels=3   - Number of adjacent channels around the central to use (should be
#                     an odd number).
#Par: center=[0,90] - Determines the center of the map [Az,El] in degrees (around which 
#                     to search for the trasmitter).
#Par: pixelsize=2   - Determines the pixelsize at the Zenith in each dimension (in degrees).
#Par: pixels=120    - Determines the number of pixels in each dimension around the center.
#
#Ret: vector of peak frequencies
#Ref:  find_brightest_transmitters
#-----------------------------------------------------------------------
cal_on_brightest_transmitters:=function(ref rec,index=1,nbands=10,freqlow=10,freqhigh=33,threshold=100,nchannels=3,center=[0,90],pixelsize=2,pixels=120){
  
  global imageTool;
  global globalpar;

  filename:="transmitters.log";
  file:=open(spaste(">",filename));

  global f,nfreqs,cals,n0,n1,n2,f1,f2,freqs

  freqs:=find_brightest_transmitters(rec,index=index,nbands=nbands,freqlow=freqlow,freqhigh=freqhigh,threshold=threshold);

#print 'real freqs=',freqs;
#  freqs:=freqs[28:len(freqs)];
#print 'cut freqs=',freqs;

  line:=spaste(as_string(roundn(freqs/1e6,4)),sep=' ') ~ s/ /, /g ~ s/[, ]+$//;
  write(file,'DelaysAnt := [=]');
  write(file,'Delays := array(0,',len(freqs),', ', rec.len,')',sep=''); write(file);
  write(file,'AZEL := array(0,',len(freqs),', 2)',sep=''); write(file);
  write(file,'Frequencies := [',line,']',sep=''); write(file);

  nfreqs:=0;
  cals:=array(0,len(freqs),rec.len);
  for (f in freqs) {
    print 'Starting with block 1!'
    rec.setblock(1);
    nfreqs+:=1;
    n0:=rec.getxn(f,"Frequency",index);
    n1:=n0-floor(nchannels/2); n2:=n0+floor(nchannels/2);
    f1:=rec.getn("Frequency",index,n1);
    f2:=rec.getn("Frequency",index,n2);
    print "FREQUENCY RANGE:",f1/1e6,f2/1e6,n1,n2
    rec.put([0,90],'AZEL',-1);
    rec.setphase();
    skymapper(rec,band=[f1/rec.headf("FrequencyUnit"),f2/rec.headf("FrequencyUnit")],
                  mapCoordinates='AZEL',
                  mapTitle=spaste('Freq=',as_string(f)),
                  mapProjection='STG',
                  mapCenter=center,
                  minCSC=[0,0],
                  maxCSC=[360,90],
                  incrCSC=[pixelsize,pixelsize],
                  rangeElevation=[-10,90],
                  mapPixels=[pixels,pixels],
                  blocks=1,
                  channels=1,
                  SignalQuantity='power',
                  SignalDomain='freq',
                  exportFITS=False,
                  backupsNumber=0,
                  selfcal=False,
                  viewmap=F
)
    await globalpar.skymapwatch->done;
    pos:=find_peak_in_image(rec,imageTool);
    cals[nfreqs,]:=calibrate_phases_on_pointsource(rec,frequency=[f1,f2],setdelays=F);

    write(file,'#-----------------------------------------------------------------------');
    write(file,'Source := ',nfreqs,sep=''); write(file);
    write(file,'Frequency[',nfreqs,'] := ',roundn(freqs[nfreqs]/1.e6,4),sep=''); write(file);
    write(file,'AZEL[',nfreqs,',] := [',roundn(pos[1],1),', ',roundn(pos[2],1),']',sep=''); write(file);
    line:=spaste(as_string(roundn(cals[nfreqs,]*rec.headf("SamplerateUnit")*rec.headf("Samplerate"),2)),sep=' ')  ~ s/ /, /g ~ s/[, ]+$//;
    write(file,'Delays[',nfreqs,',] := [',line,']',sep=''); write(file);
    print 'Frequency=',roundn(f/1e6,2),'MHz, Delays=',roundn(cals[nfreqs,]*rec.headf("SamplerateUnit")*rec.headf("Samplerate"),2);
    }

    write(file,'#=======================================================================');
    for (ant in seq(len(cals[1,]))) {
       write(file,'DelaysAnt[',ant,'] := [',spaste(as_string(roundn(cals[,ant]*rec.headf("SamplerateUnit")*rec.headf("Samplerate"),2)),sep=' ')  ~ s/ /, /g ~ s/[, ]+$//,']',sep=''); write(file);
    };
file:=F;
return cals; 
}
