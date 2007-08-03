#######################################################################
#Section: TOC #
###############
# <br>blank       standard header
#
# <a href="#data">DATA</a>
# <li>filesel     extracts a selection of files from directory
# <li>dataconv    conversion between data types
# <li>wf2ds       generates dynamic spectrum on input signal
# <li>wf2fft      performs FFT on input signal
# <li>fft2wf      generates a waveform signal on a complex fourier series
# <li>fft         FFT
# <li>invfft      recovers the wave form from its spectrum
#
# <a href="#beamforming">BEAMFORMING</a>
# <li>phaseid     determines delays from phasedifferences
# <li>antdelays   calculates antenna signal delays in seconds
# <li>sundelays   calculates delays for the position of the sun
# <li>sundir      calculates the sun direction for an exact time
# <li>sourcedir   calculates source direction for an exact time
#
# <a href="#filtering">FILTERING</a>
# <li>ants_check  check antenna data for realistic values
# <li>filter      confines fourier data in frequency & amplitude
# <li>rebin       rebins an array
# <li>channels    changes frequency band & produces channels within
# <li>smooth      produces moving average
# <li>intpol2d    calculates an 2d interpolation
#
# <a href="#displaying">DISPLAYING</a>
# <li>dsary       generates dynamic spectrum
# <li>dsplot      plots dynamic spectrum from file
# <li>imgplot     plots a 2D pixel array as an image in the viewer
# <li>plotdim     plots each column of a matrix
# <li>writops     generates psfile from viewer and pgplotter
# <li>timestamps  checks for saved time information for a dynamic spectrum
# <li>fileinfo    extracts time of events 
# <li>plotantpos  plots antenna positions
#
# <a href="#tools">TOOLS</a>
# <li>xcor        calculates cross correlation of two arrays
# <li>xcorwin     calculates cross correlation in a window on two arrays
# <li>xcor2avg    calculates average cross correlation of two matrices
# <li>xcor2win    calculates 2d cross correlation in a window on two matrices
# <li>xcor2ful    calculates full 2d cross correlation of two matrices
# <li>blockint    reduces array length by averaging blocks
# <li>blockavgstd calculates average and standard deviation for arrayblocks
# <li>runmin      returns the running minimum of an ary for a defined block size
# <li>run2sum     returns the running difference between two ary for a defined block size
# <li>str2ary     conversion of string array to float array
# <li>radec2azel  conversion of RA & DEC to AZEL
# <li>res         calculates the resolution of a telescope in degrees
# <li>error       calculates relative (and absolut) error
# <li>sigsel      returns selection of an array for values within a sigma level
#
# <a href="#tricks">TRICKS</a>
# <li>stdev       calculates standard deviation with STDDEV
# <li>tic & toc   functions for time registration
# <li>xflag       flags yvalues for regions on the xaxis to a certain value
# <li>charsel     extraction of specified chars out of a string
# <li>flip2d      flips 2D matrix
# <li>matary2d    substraction of ary from matrix
# <li>mean2d      average of matrix column or row
# <li>stdv2d      standard deviation of matrix column or row
# <li>funclog     creates log file and adds open number of items to it (loging of function parameter)
# <li>maxpos      returns position of maximum in ary
# <li>minpos      returns position of minimum in ary
# <li>valpos      returns position of value in ary
# <li>valext      extracts one value from input array by position
# <li>hist        returns record including array with different numbers and array with their frequency
# <li>int & dif   integrates and differentiates inputarray
# <li>rest        calculates rest for a division
# <li>doc         opens browser with aips++ documentation
# <li>printvar    displays any variable
# <li>sam2dist    displays & returns light travel distance for a given number of samples
# <li>rnd         rounds input array to a number of digitis


#######################################################################
#Section: BLANK #######################################################
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: funcname
#----------------------------------------------------------------------
#Type: function
#Doc:
#Par:
#Ret:
#Ref:
#Example:
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 00.00.00 @ KUN
#----------------------------------------------------------------------
func funcname() {

}
#######################################################################


#######################################################################
#Section: <a name="data">DATA</a> #####################################
#######################################################################
func fexist(file) {return dms.fileexists(file)}          # file check
func dexist(dir) {return dms.fileexists(directory,'-d')} # dir check

#######################################################################
#----------------------------------------------------------------------
#Name: filesel
#----------------------------------------------------------------------
#Type: function
#Doc: extracts a selection of files from directory
#Example: files:=filesel('',ext='*.event',first=1,last=-1,disp=T);
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 03.03.04 @ KUN
#----------------------------------------------------------------------
#files:=filesel('/raid/pe/2004/12/21/',ext='*.event',first=500,last=502,disp=F);
#----------------------------------------------------------------------
filesel:=function(dir,ext='',first=1,last=-1,disp=F)
{
  # fexist function #
  func fexist(file) {return dms.fileexists(file)}

  # directory check #
  if (!fexist(dir))
  {print '[filesel] directory ',dir,' does not exist or is not accessible !!!'; return ''};

  # directory extraction #
  if (dir=='') {dir:=spaste(shell('pwd'),'/')};
  ##files:=shell(spaste('cd ',dir,'; ls *',ext)) # !!! not more than a certain number of files !!!
  files:=shell(spaste('cd ',dir,'; ls'))
  filen:=len(files)
  if (filen==0) {print '[filesel] directory ',dir,' is empty !!!'; return ''};

  # files check #
  filesel:=array(F,filen)
  for (i in 1:filen) {
    filecheck:=split(files[i],'') #2006.12.01.23:59:57.245.event (29)
    filelen:=len(filecheck)
    if (filelen==29&&spaste(filecheck[24:29])==ext) filesel[i]:=T;
  }
  files:=files[filesel]
  filen:=len(files)
#print 'number of files:',len(files)
  if (len(files)==0) {print '[filesel] no files !!!'; return ''}; # no files

  # file selection #
  if (first>filen) {
    print '[filesel] directory:',dir;
    print '- number of files in directory:',filen;
    print '- number of first file:',first;
    print '- number of last file:',last;
    return F;
  };
  if (last==-1|last>filen) {last:=filen};
#print 'number of files:',len(files)
  files:=files[first:last]
  filen:=len(files)
#print 'number of files:',len(files)

  # file check #
  #if (!fexist(spaste(dir,files[1])))
  #{print 'file',files[1],'in',dir,'is not accessible !!!'; return F};

  # filenames #
  for (i in 1:len(files)) {
    if (disp) {print spaste('[',(first:last)[i],'] ',files[i])};
    files[i]:=spaste(dir,files[i]);
  }

  # return #
  return files;
}
#######################################################################
#files:=shell(spaste(['ls ',eventdir,'*.event']))
#files:=files~globalconst.FilenameExtractor

#######################################################################
#----------------------------------------------------------------------
#Name: dataconv
#----------------------------------------------------------------------
#Type: function
#Doc: conversion between data types
#     f(x)->Voltage->FFT->AbsFFT->Power->CalPower->NoiseT
#     Pos->Time->IntermedFreq->Frequency->SkyT
#     MeanPower->MNT
#Par: infield  = - input field name
#Par: inary    = - input data with type of infield
#Par: outfield = - output field name
#Ret: outary   output data with type of outfield
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 06.08.03 @ MPIfR
#----------------------------------------------------------------------
dataconv:=function(ref rec,infield,inary,outfield)
{
  fftlen:=len(inary);
  if (infield=='FFT'|infield=='Power') {arylen:=fftlen*2-1} else {arylen:=fftlen}; # aplies for all series in frequency domain
  newindex:=data.len+1;
  data.copy(1,newindex);
  ci:=data.get('Coordinfo',newindex);
  ci[infield].IsInput:=T; ci[infield].Stored:=T; ci[infield].Ref:='-';
  ci:=data.put(ci,'Coordinfo',newindex);
  data.setregion([1,arylen],'T',newindex);
  data.putmeta(arylen,'FileBlocksize',newindex);
  data.putmeta(arylen,'Blocksize',newindex);
  data.putmeta(0,'FileOffset',newindex);
  data.putmeta(arylen,'FFTSize',newindex);
  data.putmeta(fftlen,'FFTlen',newindex);
  data.put(inary,infield,newindex);
  data.put(F,'Doshift',newindex);
  outary:=data.get(outfield,newindex);
  data.del(newindex);
  return outary;
}
#######################################################################


#######################################################################
#----------------------------------------------------------------------
#Name: wf2ds
#----------------------------------------------------------------------
#Type: function
#Doc:  generates dynamic spectrum on input signal
#Example: fft:=10+5i; freq:=10e6; bs:=2^8; tvals:=((1:bs)-1)/sr; sr:=80e6; wf:=real(fft)*cos(2*pi*freq*tvals)+imag(fft)*sin(2*pi*freq*tvals); retf:=wf2fft(wf=wf,sr=sr); plotdim(log(abs(retf.yf)^2+1),xvals=retf.f/1e6,title='Spectrum',xlab='Frequency (MHz)',ylab='sqr(absFFT)'); sel:=valpos(abs(retf.yf),max(abs(retf.yf))); print retf.f[sel]/1e6,retf.yf[sel]
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 08.02.06 @ RU
#----------------------------------------------------------------------
func wf2ds(ref rec=[=],name='DS',wf=1:10,bs=0,sr=10,alpha=0.5,fshift=0,disp=F) { # hanning: alpha=0.5 & hamming: alpha=0.54
### for constant samplerate ###
  include 'fftserver.g'; fftserv:=fftserver(); ret:=[=]
  if (len(field_names(rec))==0) {ret.bs:=bs; ret.sr:=sr; rec.x:=wf; wf:=[]}
  else {ret.bs:=rec.bs; ret.sr:=rec.sr};
  ret.xlen:=len(rec.x)
  center:=floor(ret.bs/2)
  ## blocking
  ret.bn:=floor(ret.xlen/ret.bs); ret.pix:=array(0.,ret.bn,len(1:(center+1)))
  for (block in 1:ret.bn) {
    hlp:=rec.x[((block-1)*ret.bs+1):(block*ret.bs)]*hanning(ret.bs,alpha=alpha)
    fftserv.complexfft(hlp,1) # 1:forward & -1:reverse
    # glishfft: even timelen: max...|conj(min...) & odd timelen: max...|min|conj(...max)
    # normalfft: even timelen: min...|conj(max...) & odd timelen: min...|max|conj(...min)
    if (ret.bs%2==0) {hlp:=hlp[[(center+1):ret.bs,1:center]]} # even
    else {hlp:=hlp[[(center+1):ret.bs,2:(center+1)]]; hlp[1]:=conj(hlp[1])}; # odd ???
    hlp/:=ret.bs/2 # fft normalization
    hlp:=hlp[1:(center+1)] # spec from 1st nyquist zone ??? for even: conj(last value) ???
    ret.pix[block,]:=hlp; hlp:=[]
    ret.flen:=shape(ret.pix)[2]
    ret.f:=(0:(ret.flen-1))/ret.bs*ret.sr+fshift # f_i = i / N * sr
  } # end for blocks
  if (disp) {
print spaste(name,': time [s] & freq [Hz] axis (len,[min,max],[mindif,maxdif]):')
    ret.t:=(1:rec.bn-1)*ret.bs/ret.sr+ret.bs/ret.sr/2
printvar(ret.t,'-'); printvar(ret.f,'-')
    imgplot(log(abs(ret.pix)^2),name=name,timvals=ret.t,bandlim=range(ret.f),
            axisorder=[2,1],freqdir=1,dmin=0,dmax=1,xax=T,evnums=0,timunit='s',ps=F,chk=F)
  }; # endif disp
  ret.alpha:=alpha
  ret.fshift:=fshift
  return ret
} # endfunc wf2ds
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: wf2fft
#----------------------------------------------------------------------
#Type: function
#Doc:  performs FFT on input signal
#Example: fft:=10+5i; freq:=10e6; bs:=2^8; tvals:=((1:bs)-1)/sr; sr:=80e6; wf:=real(fft)*cos(2*pi*freq*tvals)+imag(fft)*sin(2*pi*freq*tvals); retf:=wf2fft(wf=wf,sr=sr); plotdim(log(abs(retf.yf)^2+1),xvals=retf.f/1e6,title='Spectrum',xlab='Frequency (MHz)',ylab='sqr(absFFT)'); sel:=valpos(abs(retf.yf),max(abs(retf.yf))); print retf.f[sel]/1e6,retf.yf[sel]
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 17.10.05 @ LESIA
#----------------------------------------------------------------------
func wf2fft(ref rec=[=],wf=1:10,sr=10,nyquist=1,alpha=0.5,fshift=0) { # hanning: alpha=0.5 & hamming: alpha=0.54
## for constant samplerate
  include 'fftserver.g'; fftserv:=fftserver()
  if (len(field_names(rec))!=0) {sr:=rec.sr} else {rec.sr:=sr; rec.x:=wf; wf:=[]};
  rec.xlen:=len(rec.x)
  center:=floor(rec.xlen/2)
  rec.xf:=rec.x*hanning(rec.xlen,alpha=alpha)
  fftserv.complexfft(rec.xf,1) # 1:forward & -1:reverse
  # glishfft: even timelen: max...|conj(min...) & odd timelen: max...|min|conj(...max)
  # normalfft: even timelen: min...|conj(max...) & odd timelen: min...|max|conj(...min)
  if (rec.xlen%2==0) {rec.xf:=conj(rec.xf[[(center+1):rec.xlen,1:center]])} # even
  else {rec.xf:=conj(rec.xf[[(center+1):rec.xlen,2:(center+1)]])}; # odd
  rec.xf/:=rec.xlen/2 # fft normalization
  if (nyquist%2==1) {rec.xf:=rec.xf[1:(center+1)]} # 1st nyquist zone
  else {rec.xf:=rec.xf[(center+1):1]}; # 2nd nyquist zone
  rec.flen:=len(rec.xf)
  rec.f:=(0:(rec.flen-1))/rec.xlen*rec.sr+fshift # f_i = i / N * sr
  rec.alpha:=alpha
  rec.fshift:=fshift
  return rec
} # endfunc wf2fft
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: fft2wf
#----------------------------------------------------------------------
#Type: function
#Doc:  generates a waveform signal on a complex fourier series
#Example: rec:=fft2wf(); plotdim(rec.x,xvals=rec.t*1e3,title='Waveform',xlab='Time (ms)',ylab='Amplitude')
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 17.10.05 @ LESIA
#----------------------------------------------------------------------
func fft2wf(ref rec=[=],fft=1+1i,frq=10e6,sr=80e6,ds=2^8,sign=1,progres=T) {
  if (len(field_names(rec))==0) {rec.sr:=sr; rec.ds:=ds};
  if (rec.ds>2^28) {rec.ds:=2^28};
  rec.t:=(0:(rec.ds-1))/rec.sr #rec.t:=((0:(rec.ds-1))+rec.ds/4)/rec.sr 
  fftlen:=len(fft)
  rec.x:=0
  old:=0
  for (k in 1:fftlen) {
    prz:=k/fftlen*100; if (progres&&floor(prz/10)==floor(old/10)+1)
                       {old:=round(prz); print old,'% (',k,'/',fftlen,')'};
    if (sign==1) {rec.x+:=abs(fft[k])*cos(2*pi*frq[k]*rec.t+sign*arg(fft[k]))}
    else {rec.x+:=abs(fft[k])*cos(2*pi*frq[k]*rec.t+sign*atan(imag(fft[k])/real(fft[k])))};
    #arg(fft[k])=atan(imag(fft[k])/real(fft[k]))) ???
  }
  return rec
} # endfunc fft2wf
#######################################################################
## Percentage Display
#if (i%100==0) {print i,'/',n,round(i/n*100),'%'};
#prz:=i/n*100; if (prz%10==0) {print round(prz),'% (',i,'/',n,')'};
#prz:=i/n*100; if (floor(prz/10)==floor(old/10)+1) {old:=round(prz); print old,'% (',i,'/',n,')'};

#######################################################################
#----------------------------------------------------------------------
#Name: fft
#----------------------------------------------------------------------
#Type: function
#Doc:  FFT
#Par:  ary
#Ret:  ary
#Example: input_event(data,'example.event'); bs:=2^8; data.setblock(blocksize=bs); data.unflag('Time'); fft(data.get('Dummy'),dir=1,nyquist=2)[1:5]/2^(12-ln(bs)/ln(2)); data.get('FFT')[1:5]
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 21.04.06 @ RUN
#----------------------------------------------------------------------
func fft(ary,alpha=1,dir=1,nyquist=1,sr=1,fshift=0,ret=F) {
  include 'fftserver.g'; fftserv:=fftserver()
  arylen:=len(ary)
  center:=floor(arylen/2)
  ary*:=hanning(arylen,alpha=alpha)
  fftserv.complexfft(ary,dir) # 1:forward & -1:reverse
  # glishfft: even timelen: max...|conj(min...) & odd timelen: max...|min|conj(...max)
  # normalfft: even timelen: min...|conj(max...) & odd timelen: min...|max|conj(...min)
  if (arylen%2==0) {ary:=conj(ary[[(center+1):arylen,1:center]])} # even
  else {ary:=conj(ary[[(center+1):arylen,2:(center+1)]])}; # odd
  ary/:=arylen/2 # fft normalization
  if (nyquist%2==1) {ary:=ary[1:(center+1)]} # 1st nyquist zone
  else {ary:=ary[(center+1):1]}; # 2nd nyquist zone
  if (ret) {rec:=[=]
    rec.fftlen:=len(ary)
    rec.freq:=(0:(rec.fftlen-1))/arylen*sr+fshift # f_i = i / N * sr
    rec.ary:=ary; rec.timlen:=arylen
    rec.alpha:=alpha; rec.dir:=dir; rec.nyquist:=nyquist; rec.sr:=sr; rec.fshift:=fshift
    return rec
  } else {return ary};
} # end fft func
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: invfft
#----------------------------------------------------------------------
#Type: function
#Doc:  recovers the wave form from its spectrum
#Par:  fft
#Par:  dir -1 inverse FFT & 1 forward FFT
#Ret:  ary
#Example: include 'dynspectools.g'; timx:=invfft(data.get('FFT'),nyquist=2)
#plotdim(data.get('Voltage'),xvals=data.get('Time')*1e3,title='Antenna Time Series',ylab='ADCcounts',xlab='Time [ms]'); plotdim(timx,xvals=data.get('Time')*1e3,add=T)
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 09.07.03 @ MPIfR
#----------------------------------------------------------------------
invfft:=function(fft,timelen=-1,dir=-1,nyquist=1,realvals=T)
{
  fftlen:=len(fft)
  if (timelen==-1) {if (fftlen%2==1) # odd fftlen -> even timelen
    {timelen:=(fftlen-1)*2} else {timelen:=fftlen*2+1}}; # even & odd
  if (nyquist%2==1) {fft:=fft[fftlen:1]}; # 1st nyquist zone
  ary:=array(0+0i,timelen)
  center:=floor(timelen/2)
  ary[1:fftlen]:=fft #exact: ary[1:center]:=fft[1:center]
  if (timelen%2==1) {ary[(center+2):timelen]:=conj(fft[center:1])} # odd timelen
  else {ary[(center+1):timelen]:=conj(fft[(center+1):2])}; # even timelen
  ary:=ary*timelen/2
  # normalfft: even timelen: min...|conj(max...) & odd timelen: min...|max|conj(...min)
  # glishfft: even timelen: max...|conj(min...) & odd timelen: max...|min|conj(...max)
  # event timelen -> odd fftlen & odd timelen -> even fftlen
  include 'fftserver.g'; fftserv:=fftserver()
  fftserv.complexfft(ary,dir)
  if (realvals) {return real(ary)} else {return ary};
}
#######################################################################
# "%x" calculates the rest of the division by "x"

#######################################################################
#Section: <a name="beamforming">BEAMFORMING</a> #######################
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: phaseid
#----------------------------------------------------------------------
#Type: function
#Doc: determines delays by phasedifference
#Par: name     = - project name
#Par: files    = - eventfiles for identification
#Par: sequence = - frequency band with signal for identification (optional)
#                  sequence=0 -> sequence:=freq
#                  falcke: sequence:=[seq(18174,18195,1)];
#                  anigl:  sequence:=[seq(18159,18206,1)];
#Par: freq     = - special frequency for identification (optional)
#                  freq=0 -> middle frequency of sequence is taken
#Par: ants     = - antenna selection, ants=-1 -> all antennas
#Par: nsigma   = - number of sigmas for statistical filter by deviation
#Par: sosdir   = - directory for storage of phasedifferences and delays
#Par: act      = T - calculation of phasedifferences
#Par: disp     = T - combination of sosfiles and displaying
#Ret: delays
#Ref: pdiff2delay, delay2pdiff
#----------------------------------------------------------------------
#Info: tvfreq:=62.194e6; tvsample:=18181.3248; tv360delay:=1.2863;
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 10.03.04 @ KUN
#----------------------------------------------------------------------
#ARGUMENT(+): pos delay -> 2nd signal later -> shift 2nd ary to the left (+delay)
#The delay is positive for a signal arriving at the referenced antenna later than at the reference antenna.
#The delay is negative for a signal arriving at the referenced antenna earlier than at the reference antenna.
#ARGUMENT(-): pos delay -> 2nd signal earlier -> shift 2nd ary to the right (-delay)
#The delay is negative for a signal arriving at the referenced antenna later than at the reference antenna.
#The delay is positive for a signal arriving at the referenced antenna earlier than at the reference antenna.
#----------------------------------------------------------------------
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
#phasedelays:=dsmap(name='phaseid-master-fx',act=F,ants=1:8,method=0,disp=T,sosdir=spaste(dyndir,'sos/test/'))
#----------------------------------------------------------------------
#files:=filesel(spaste(workdir,'lopes-040317-f1001/'),first=241,last=241); sosdir:=sosfiledir;
#files:=filesel(spaste(workdir,'lopes-040112-sel/'),first=70,last=72); sosdir:=sosfiledir;
#files:=filesel(spaste(workdir,'lopes-040330-f1501/'),first=1,last=5); sosdir:=sosfiledir; delays.phi:=phaseid(name='test',files=files,ants=-1,refant=1,nsigma=2,sosdir=sosdir,act=T,disp=T);
#----------------------------------------------------------------------
#delays.phi:=phaseid(name='test',files=filesel(spaste(workdir,'lopes-031028-1046t1327-burst/'),first=1,last=3),ants=-1,refant=1,nsigma=2,sosdir=sosfiledir,act=T,disp=T);
#delays.phi:=phaseid(name='testray',files=filesel(spaste(datadir,'2004/05/10/'),first=1,last=-1),ants=-1,refant=1,nsigma=2,sosdir='',act=T,disp=F);
#----------------------------------------------------------------------
phaseid:=function
(
 name='',
 act=T,
 files='',
 freqrange=[62165865.6,62223239.5,67673778,67712097,67925426,67946548],
 ants=-1,
 refant=1,
 nsigma=2,
 sosdir='',
 disp=T,plots=T,ps=T,
 debug=F
)
{
  # Parameters #
  numfiles:=len(files);
  input_event(data,files[1]);
  samplerateunit:=data.headf('SamplerateUnit',refant);
  samplerate:=data.headf('Samplerate',refant)*samplerateunit;
  if (ants==-1) {ants:=1:data.len};
  nants:=len(ants);

  # print # (title)
    print '';
    print '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%'
    print 'PDDI - Phase Difference Delay Id';
    print '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%'

  # PHASECALCULATION ####################################################

  if (act)
  {
    # Parameters #
    nrange := floor(len(freqrange)/2)

    # Print # (infos)
    print '';
    print 'CALCULATION';
    print 'datafile directory: ',files[1]~globalconst.DirExtractor;
    print 'number of files: ',numfiles;
    print 'antenna selection: ',ants;
    print 'reference antenna: ',refant;
    print 'number of sigmas for statistics: ',nsigma;
    print 'sosfile directory: ',sosdir;
    print '';

    # Globals #
    global globalpar;

    # Variables #
    #filesdphim:=array(0,nants);
    newdelaysm:=array(0,numfiles,nants);

    # Files #
    for (filenum in 1:numfiles)
    {
      # Event #
      input_event(data,files[filenum]);

      # Load #
      #corrdelays:=data.getary('Corrdelay')[ants]*samplerate;
      corrdelays:=globalpar.delays[ants];
      FixDelays(data,index=-1,delays=corrdelays);
      data.setphase(tozenith=T); # look towards zenith
      data.put(refant,'RefAnt',-1);

      # Peak Frequencies #
      freq:=data.get('Frequency',1);
      power := data.get('Power',refant);
      peakpos := array(0.,nrange);
      peakfreq := array(0.,nrange);
      for (i in 1:nrange)
      {
        sequence := (freq>=freqrange[(2*i-1)]) & (freq<=freqrange[(2*i)]);
        if (sum(sequence)==0) {print "phasecmp: Incorrect frequency !!!"; return};
        peakpos[i] := maxpos(power*sequence);
      };
      peakfreq := freq[peakpos];

      # Variables #
      dphi:=array(0.,nrange,nants);
      delays:=array(0.,nrange,nants);
      newdelays:=array(0.,nrange,nants);
      olddelays:=array(0.,nrange,nants);
      reldelays:=array(0.,nrange,nants);
      relphases:=array(0.,nrange,nants);
      reldelmean := array(0.,nants);

      # Ants #
      for (combi in ants[ants!=refant])
      {
        dphi[,combi] := data.get('PhaseDiff',combi)[peakpos]; #take the phase difference at the peak position
        delays[,combi]:=pdiff2delay(dphi[,combi],peakfreq);
      }; # end ant combis

      # Change #!!!
      #aseq := 1:nants;
      #for (i in 1:nrange)
      #{
      #  relphases[i,] := -globalpar.tvphases[i,aseq]+dphi[i,];
      #  relphases[i,] := relphases[i,]-(360*(relphases[i,]>180));
      #  relphases[i,] := relphases[i,]+(360*(relphases[i,]<-180));
      #  reldelays[i,] := pdiff2delay(relphases[i,],peakfreq[i])*samplerate;
      #};
      #for (i in ants) {reldelmean[i] := mean(reldelays[,i])};

      # New Corrdelays #
      for (i in ants) {newdelaysm[filenum,i]:=mean(delays[,i])};
      #for (i in 1:nrange) {olddelays[i,]:=pdiff2delay(globalpar.tvphases[i,],peakfreq[i])};
      #for (i in ants) {olddelaysm[i]:=mean(olddelays[,i])};
      #delayschgm:=newdelaysm[filenum,]-olddelaysm;
      #newcorrdelays:=corrdelays-reldelmean; #-delayschgm!!!
      #FixDelays(data,ants,delays=newcorrdelays); # set hardware offset delays to new correction delays (Corrdelays)
      #data.setphase();                           # apply and also reset AZEL value to proper value

      # Print #
      if (disp)
      {
        #for (i in ants) {newdphim[i] := mean(dphi[,i])};
        #print spaste('tvphase1  [',sprintf('%6.1f,',dphi[1,]),'] degrees');
        #print spaste('changeph1 [',sprintf('%6.1f,',relphases[1,]),'] degrees');
        #print spaste('change1   [',sprintf('%6.2f,',reldelays[1,]),'] samples');
        #print spaste('tvphase2  [',sprintf('%6.1f,',dphi[2,]),'] degrees');
        #print spaste('changeph2 [',sprintf('%6.1f,',relphases[2,]),'] degrees');
        #print spaste('change2   [',sprintf('%6.2f,',reldelays[2,]),'] samples');
        #print spaste('tvphase3  [',sprintf('%6.1f,',dphi[3,]),'] degrees');
        #print spaste('changeph3 [',sprintf('%6.1f,',relphases[3,]),'] degrees');
        #print spaste('change3   [',sprintf('%6.2f,',reldelays[3,]),'] samples');
        #print spaste('-----------------------------------------------------------------------------------');
        #print spaste('newphases [',sprintf('%6.2f,',newdphim),'] degrees');
        #print spaste('newdelays [',sprintf('%6.2f,',newdelaysm[filenum,]*samplerate),'] samples');
        #print spaste('olddelays [',sprintf('%6.2f,',olddelaysm*samplerate),'] samples');
        #print spaste('delayschg [',sprintf('%6.2f,',delayschgm*samplerate),'] samples');
        #print spaste('delays!!! [',sprintf('%6.2f,',reldelmean),'] samples');
        #print spaste('act corr  [',sprintf('%6.2f,',corrdelays),'] samples');
        #print spaste('new corr  [',sprintf('%6.2f,',newcorrdelays),'] samples');
        #print spaste(sprintf('[%3.0f]',filenum),' ',spaste(split(files[filenum]~globalconst.FilenameExtractor,'')[1:23]),' [',sprintf('%4.0f,',newdphim[ants[ants!=refant]]),'] [',sprintf('%5.2f,',newdelaysm[filenum,ants[ants!=refant]]*samplerate),']');
        print spaste(sprintf('[%3.0f]',filenum),
                     ' ',spaste(split(files[filenum]~globalconst.FilenameExtractor,'')[1:23]),
                     ' [',sprintf('%5.2f,',newdelaysm[filenum,ants[ants!=refant]]*samplerate),']');
      }

      # sum #
      #filesdphim+:=newdphim;

    } # end files

    # averaging #
    #filesdphim/:=numfiles;

    # Saving #
    print '';
    print 'SAVING...';
    sosname:=name;
    if (sosdir=='') {sosdir:='sos/'};
    if (!fexist(sosdir)) {shell(spaste('mkdir ',sosdir))};

    # Print #
    print '';
    print 'size of delay array:',len(newdelaysm);
    print 'sosfilename:',sosname;
    print '';

    # write #
    write_value(newdelaysm,spaste(sosdir,sosname,'.delay.sos')); # save delays to sosfile

  }; # end action

  # DISPLAYING ##########################################################
  if (disp)
  {
    # info #
    print '';
    print 'PLOTTING...';
    print 'sosfile directory:',sosdir;

    # load sos-filenames #
    sosfnames:=filesel(sosdir,ext='*.delay.sos');
    if (act) {sosfnames:=spaste(sosdir,sosname,'.delay.sos'); slast:=1}; #just one file
    numfiles:=len(sosfnames);
    print 'sofile number:',numfiles

    # load delays #
    print 'sosfile(s) [number, name, size]:';
    ary:=[=];
    arylen:=[=];
    for (filenum in 1:numfiles)
    {
      ary[filenum]:=read_value(sosfnames[filenum]); # delays from sosfile
      arylen[filenum]:=shape(ary[filenum]);
      print spaste('[',filenum,'] ',sosfnames[filenum],' ',arylen[filenum]);
    }

    # combination of data #
    timelen:=0; for (i in 1:len(arylen)) {timelen+:=arylen[i][1]};
    delays:=array(0.,timelen,nants);
    start:=1;
    for (block in 1:numfiles)
    {
      delays[start:(start+arylen[block][1]-1),]:=ary[block];
      start:=(start+arylen[block][1]);
    };

    # FILTER --------------------------------------------------------------
    # statistics #
    dev:=array(0,timelen,nants);
    sdev:=array(0,nants);

    # antenna average #
    for (ant in 1:nants) {delaysm[ant]:=mean(delays[,ant])}
    plotarym:=delaysm;

    if (numfiles>1)
    {
      # standard deviation #
      for (ant in 1:nants) {sdev[ant]:=stddev(delays[,ant])}

      # deviation # (normalized in sigmas by standard deviation)
      for (ant in 1:nants)                         # for antennas
        {for (fc in 1:numfiles)                    # for files
          {if (abs(delays[fc,ant]-delaysm[ant])!=0) # if not zero
            {dev[fc,ant]:=abs(delays[fc,ant]-delaysm[ant])/sdev[ant]}
          else {dev[fc,ant]:=0}}};

      # print # (flags)
      #print '';
      #print 'flags:',dev<nsigma;
      #print '';
      #print 'dev:',round(dev*1000)/1000;

      # print # (pre flagging)
      print '';
      print 'pre flagging (average & standarddeviations):';
      printf('%7.2f, ',delaysm*samplerate); print '';
      printf('%7.2f, ',sdev*samplerate); print ' ';

      # revised antenna average #
      for (ant in 1:nants) plotarym[ant]:=mean(delays[,ant][dev[,ant]<nsigma]);

      # revised standard deviation #
      for (ant in 1:nants) sdev[ant]:=stddev(delays[,ant][dev[,ant]<nsigma]);

      # print # (post flagging)
      print '';
      print 'post flagging (average & standarddeviations):';
      printf('%7.2f, ',plotarym*samplerate); print '';
      printf('%7.2f, ',sdev*samplerate); print '';

    } # endif statistics

    # filtering #
    plotary:=delays;
    for (ant in 1:nants) {plotary[dev[,ant]<nsigma,ant]:=plotarym[ant]};
    # FILTER --------------------------------------------------------------

    # plot #
    if (plots) {
      print ''
      if (!is_defined('pgp') || (len(pgp)==1)) pgp:=pgplotter(foreground='black',background='white');
      #unfiltered
      pgp.plotxy1(1:len(delays),delays*samplerate);
      pgp.lab(toplbl=name,xlbl='eventnumber (time)',ylbl='delays [12.5ns]');
      #filtered
      pgp.plotxy1(1:len(delays),plotary*samplerate);
      pgp.lab(toplbl=name,xlbl='eventnumber (time)',ylbl='delays [12.5ns]');
      if (ps) {writops(handle=pgp,psdir='',name=spaste(name,'.aio'))};
    }; # endif plots

    # sosfile #
    if ((!act)&(numfiles>1)) # save array to sosfile if generated from a number of sosfiles
    {
      # infos #
      print '';
      print 'SAVING...';
      print 'pixarray:',shape(plotary);
      print spaste('sfile:',name,'.sos');
      print '';

      # saving #
      sfile:=spaste(sosdir,name,'.delay.sos');
      write_value(delays,sfile);
      #write_value(plotary,sfile);
      sfile:=spaste(dyndir,'sos/delaymaster/daily.delay.sos');
      master:=read_value(sfile);
      pos:=len(master)+1;
      master[pos].name:=name;
      master[pos].delaym:=plotarym;
      master[pos].sdev:=sdev;
      write_value(master,sfile);

      # print #
      #master:=read_value('/home/anigl/data/lopesgui/lopesdyn/sos/delaymaster/daily.delay.sos'); for (i in 1:len(master)) {print sprintf('%3.0f',i),master[i].name,spaste('[',sprintf('%5.2f,',master[i].delaym*samplerate),']'),spaste('[',sprintf('%4.0f,',master[i].sdev[2:len(master[i].sdev)]/master[i].delaym[2:len(master[i].sdev)]*100),']')}
    };

    # return #
    return delays; # day delays

  } else {return delays}; # end plotting
} # end phaseid
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: antdelays
#----------------------------------------------------------------------
#Type: function
#Doc:  calculates antenna signal delays for shifting to form a beam
#Par:  azm, elv = - direction of the beam
#Par:  antpos   = - antenna positions
#Ret:  antdels  antenna delays in seconds
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 24.11.03 @ KUN
#----------------------------------------------------------------------
antdelays:=function(azm,elv,antpos)
{
  # parameter #
  c:=lightspeed
  
  #---------------------------------------------------------
  # Source position: convert angles to radians

  azmrad:=azm*pi/180
  elvrad:=elv*pi/180

  #---------------------------------------------------------
  # Convert spherical coordinates, (az,el), to cartesian
  # coordiates, (x,y,z)

  xdir:=cos(elvrad)*cos(azmrad)
  ydir:=cos(elvrad)*sin(azmrad)
  zdir:=sin(elvrad)

  #---------------------------------------------------------
  # Compute the light time travel delay given the source
  # position and the antenna position.

  for (ant in 1:len(antpos))
  {
    # distance # (projection of baseline on source direction by scalar product)
    distance:=(antpos[ant].position[1]*-xdir+  # +North&-South: position[1]=yant & xdir
               antpos[ant].position[2]*-ydir+  # +East&-West:   position[2]=xant & ydir
               antpos[ant].position[3]*-zdir)  # +Up&-Dn:       position[3]=zant & zdir
    # delay #
    antdels[ant]:=(distance/c)
  }

  # return #
  return antdels
}
## C++ ##
## delay
#  const Double c = QC::c.getValue();
#  Vector<Double> rho = Phasing::azel2cartesian (azel);
#  Double delay;
#  if (nearField && azel.nelements() == 3) {
#    Vector<Double> diff = rho-baseline;
#    delay = (MathTools::L2Norm(diff)-MathTools::L2Norm(rho))/c;
#  } else {
#    Vector<Double> product = rho*baseline;
#    delay = -sum(product)/c;
#  }
## phase gradient
#  delay = 2*PI_p*Phasing::geometricalDelay(direction,antpos);
#  phi = frequencies(freq)*delay;
#  gradients(freq,dir) = Complex(cos(phi),sin(phi));
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: sundelays
#----------------------------------------------------------------------
#Type: function
#Doc:  calculates delays for the position of the sun
#Par:  data =   - data structure
#Par:  unit = 1 - means delays is returned as time in seconds
#Ref:  CalcAntPos(), sundir(), antdelays()
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 21.04.04 @ KUN
#----------------------------------------------------------------------
sundelays:=function(ref rec,unit=1)
{
  antpos:=CalcAntPos(data,1:rec.len,rec.get('RefAnt'),1);
  sdir:=sundir(rec=rec,disp=F);
  delays:=antdelays(sdir.azm,sdir.elv,antpos);
  return delays*unit;
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: sundir
#----------------------------------------------------------------------
#Type: function
#Doc: calculates sun direction for an exact time
#Par: rec  = - data structure
#Par: date = - string with date and time of event
#Ret: sdir.xsun, sdir.ysun, sdir.zsun, sdir.azm, sdir.elv  direction of the sun
#Example: sdir:=sundir(date=dq.time(dm.getvalue(dm.epoch('utc','today')),form="ymd local"))
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 18.11.03 @ KUN
#----------------------------------------------------------------------
#files:='lopesdata/2003/10/28/11:05:49.161'; sdir:=sundir(date=files[1]~globalconst.FilenameExtractor);
#sdir:=sundir(date='2003/10/28/11:05:49.161',disp=F);
#----------------------------------------------------------------------
sundir:=function(ref rec=[=], date='', disp=F)
{
  # inclusion #
  include 'measures.g'

  # date #
  sun:=T;
  utclen:=23; #2003/10/28/11:05:49.161
  utc:=split(date,'');
  if (date==''|len(utc)!=utclen) {
    if (len(field_names(rec))!=0) {utc:=GetDataHeader(rec,field='Date',index=1)}
    else {print 'function sundir: no datum, setting AZEL to zenith'; sun:=F};
  }
  else {
    utc[5]:='/'; utc[8]:='/'; utc[11]:='/';
    utc:=spaste(utc[1:utclen]);
  };
  utc:=spaste(utc);

  # direction #
  if (sun) {

    # ref #
    #bref:=5; # direction for reference beam

    # calculation #
    dm.doframe(dm.epoch('utc',utc));
    dm.doframe(dm.observatory(rec.head('Observatory')));
    angles:=dm.measure(dm.direction('sun'),'azel');
    azmrad:=angles.m0.value;
    elvrad:=angles.m1.value;
    #sdir.xsun:=cos(elvrad+bref/180*pi)*cos(azmrad);
    sdir.xsun:=cos(elvrad)*cos(azmrad);
    sdir.ysun:=cos(elvrad)*sin(azmrad);
    sdir.zsun:=sin(elvrad);
    sdir.azm:=azmrad/pi*180;
    #if (sdir.azm>180) {sdir.azm:=azmrad/pi*180-180} else {sdir.azm:=azmrad/pi*180+180};
    sdir.elv:=elvrad/pi*180;
    #sdir.elv:=elvrad/pi*180+bref;

  } else {sdir.azm:=0;sdir.elv:=90;sdir.xsun:=0;sdir.ysun:=0;sdir.zsun:=0};

  # print #
  if (disp&sun) {
    print 'UTC:',utc;
    print 'LT:',dq.time(utc,form="ymd local");
    print 'sundir:',sdir.xsun,sdir.ysun,sdir.zsun;
    print spaste('azm:=',sdir.azm,' elv:=',sdir.elv);
  };

  # print #
  #if (disp) {
  #  if (sun) print 'UTC:',utc;
  #  if (sun) print 'LT:',dq.time(utc,form="ymd local");
  #  if (sun) print 'sundir:',sdir.xsun,sdir.ysun,sdir.zsun;
  #  if (!sun) print 'unknown datum:',utc;
  #  print spaste('azm:=',sdir.azm,' elv:=',sdir.elv);
  #};

  # return #
  return sdir;
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: sourcedir
#----------------------------------------------------------------------
#Type: function
#Doc:  Calculates source direction for an exact time
#Par:  rec       = - data structure
#Par:  obs       = - observatory
#Par:  direction = - source name
#Par:  azmref    = - change in Azimuth for reference beam
#Par:  date      = - string with date and time of event
#Ret:  sdir.name, sdir.xdir, sdir.ydir, sdir.zdir, sdir.azm, sdir.elv
#Example: sdir:=sourcedir(obs='KASCADE',direction='sun',azmref=0,date=dq.time(dm.getvalue(dm.epoch('utc','today')),form="ymd local"),disp=T)
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 18.08.04 @ KUN
#----------------------------------------------------------------------
#Example: files:='lopesdata/2003/10/28/11:05:49.161'; sdir:=sourcedir(obs='KASCADE',direction='sun',date=files[1]~globalconst.FilenameExtractor);
#sdir:=sourcedir(obs='KASCADE',direction='sun',date='2003/10/28/11:05:49.161',disp=F);
#----------------------------------------------------------------------
sourcedir:=function(ref rec=[],obs='',direction='',azmref=0,date='',disp=F)
{
  # check #
  sdir.obs:=''; sdir.name:=''; sdir.azel:=[0,90]
  sdir.xdir:=0; sdir.ydir:=0; sdir.zdir:=0
  source:=T;
  if (is_record(rec)&&obs=='') {obs:=rec.head('Observatory')};
  if (direction=='') {print '[sourcedir] no source specified -> setting AZEL to zenith'; source:=F};

  # inclusion #
  include 'measures.g'

  # date #
  utclen:=23; #2003/10/28/11:05:49.161
  utc:=split(date,'');
  if (date==''|len(utc)!=utclen) {
    if (is_record(rec)) {utc:=rec.head('Date')};
    if (!is_string(utc)) {print '[sourcedir] no datum -> setting AZEL to zenith'; source:=F};
  } else {
    utc[5]:='/'; utc[8]:='/'; utc[11]:='/';
    utc:=spaste(utc[1:utclen]);
  };
  utc:=spaste(utc);

  # direction #
  if (source) {

    # calculation #
    sdir.name:=direction
    sdir.obs:=obs
    dm.doframe(dm.epoch('utc',utc))
    dm.doframe(dm.observatory(sdir.obs))
    dmdirection:=dm.direction(sdir.name)
    if (dmdirection.refer!='J2000') {
      angles:=dm.measure(dmdirection,'azel')
      azmrad:=angles.m0.value
      elvrad:=angles.m1.value
      sdir.xdir:=cos(elvrad)*cos(azmrad)
      sdir.ydir:=cos(elvrad)*sin(azmrad)
      sdir.zdir:=sin(elvrad)
      sdir.azel[2]:=elvrad/pi*180
      sdir.azel[1]:=azmrad/pi*180+azmref
      #if (sdir.azm>180) {sdir.azel[1]:=azmrad/pi*180-180} else {sdir.azel[1]:=azmrad/pi*180+180};
    } else {sdir.azel:=[0,90]};

    # print #
    if (disp) {
      print 'UTC:',utc
      print 'LT:',dq.time(utc,form="ymd local")
      print spaste(sdir.name,' direction from ',sdir.obs,': azm:=',sdir.azel[1],
                   ' elv:=',sdir.azel[2],' (',sdir.xdir,' ',sdir.ydir,' ',sdir.zdir,')')
    }; # end disp

  }; # end source

  # return #
  return sdir;
}
#######################################################################

#######################################################################
#Section: <a name="filtering">FILTERING</a>  ##########################
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: ants_check
#----------------------------------------------------------------------
#Type: function
#Doc:  check antenna data for realistic values
#Par:  data  = - data set
#Ret:  erg  boolvector with a flag for each antenna
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 30.06.03 @ MPIfR
#----------------------------------------------------------------------
# SPECTRUMFILTER (stddev)
# AHORNEFFERFILTER
# f1: ADCMax/MinChann
# f2: +-200
# f3: +-1000
# f4: +2047 to -2048 (ADCMax/Min)
ants_check:=function(ref data) {
  for (ant in 1:data.len) {
    erg[ant]:=(max(data.get('f(x)',ant))<=as_integer(data.head('ADCMaxChann'))&
               min(data.get('f(x)',ant))>=as_integer(data.head('ADCMinChann')));
  };
  return erg;
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: filter
#----------------------------------------------------------------------
#Type: function
#Doc: confines fourier data in frequency & amplitude
#Par: yra   = - input data array
#Par: fmin  = - minimum frequency
#Par: band  = - frequency bandwidth
#Par: dnlim = - new frequency band down limit
#Par: uplim = - new frequency band up limit
#Par: sig   = - faktor for standard deviation (sigma)
#Ret: ary   output data array
#----------------------------------------------------------------------
#Info: 3dB frequency band for LOPES filter: dnlim=43.6,uplim=76.1
#----------------------------------------------------------------------
#Statistics: 3 * standard deviation = 99.7 %
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 11.02.04 @ KUN
#----------------------------------------------------------------------
filter:=function(yra,fmin=40,band=40,dnlim=43.6,uplim=76.1,sig=3)
{
  yralen:=len(yra);
  ary:=yra[ceil(yralen/band*(dnlim-fmin)+1):floor(yralen/band*(uplim-fmin))];
  dev:=stddev(ary);
  trisig:=sig*dev;
  ary[ary>trisig|ary<-trisig]:=0;
  return ary;
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: channels
#----------------------------------------------------------------------
#Type: function
#Doc: changes frequency band and produces channels within
#Par: ary     = - data array
#Par: bandsel = - frequency band selection array
#Par: chans   = - frequency band channels
#Ret: ary
#Example: bandlim:=[43.6,76.1]; freq:=data.get('Frequency'); frequnit:=data.headf('FrequencyUnit'); bandsel:=freq>=bandlim[1]*frequnit&freq<=bandlim[2]*frequnit; chanary:=channels(data.get('FFT'),bandsel=bandsel,chans=2^12)
#----------------------------------------------------------------------
#Info: 3dB frequency band for LOPES filter: dnlim=43.6,uplim=76.1
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 26.07.04 @ KUN
#----------------------------------------------------------------------
channels:=function(ary,bandsel=-1,chans=-1)
{
  # bandsel #
  chani:=T; arylen:=len(ary);
  if (sum(bandsel==T)>0) {
    if (sum(bandsel==T)==arylen) {limary:=ary};
    if (sum(bandsel==T)>arylen) {print 'CHANNELS: bandsel>arylen: T'; chani:=F};
    if (sum(bandsel==T)<arylen) {limary:=ary[bandsel]};
  } else {limary:=ary};
  arylen:=len(limary);
#print sum(bandsel==T),len(ary),arylen;

  # channeling #
  if (chans<=0) {chani:=F};
  if (chani) {
    if (chans==arylen) {chanary:=limary}; # no channeling needed
    if (chans<arylen)
    {
      for (channo in 1:chans)
       #{chanary[channo]:=min(limary[ceil((channo-1)*(arylen/chans)+1):floor(channo*(arylen/chans))])}
        {chanary[channo]:=mean(limary[ceil((channo-1)*(arylen/chans)+1):floor(channo*(arylen/chans))])}
       #{chanary[channo]:=median(limary[ceil((channo-1)*(arylen/chans)+1):floor(channo*(arylen/chans))])}
#print channo,arylen,chans,ceil((channo-1)*(arylen/chans)+1),floor(channo*(arylen/chans)
    }; # endif channeling
    if (chans>arylen) {chani:=F; print 'CHANNELS chans>arylen: chans:',chans,'arylen:',arylen};
  }; # endif chani

  # "failed" #
  if (!chani) {chanary:=array(0.,chans)};

  # return #
  return chanary;
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: rebin
#----------------------------------------------------------------------
#Type: function
#Doc:  rebins an array
#Par:  ary   = - data-array
#Par:  bin   = - binsize for new array
#Ret:  ary   = - rebinned array
#Example: ary:=1:10; ret:=rebin(ary,bin=2/3); print ary,ret
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 07.10.05 @ LESIA
#----------------------------------------------------------------------
func rebin(ary,bin=-1) {
  arylen:=len(ary)
  chans:=dif(range(ary))/bin
  chans:=round(chans)
  hlp:=((1:chans)-1)*bin+min(ary)
  ary:=hlp+(max(ary)-max(hlp))/2
  return ary
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: smooth
#----------------------------------------------------------------------
#Type: function
#Doc:  produces moving average
#Par:  ary   = - data-array
#Par:  smfak = - smooting factor, number of values included for
#                averaging
#Ret:  ary     data-array
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 25.07.03 @ MPIfR
#----------------------------------------------------------------------
smooth:=function(ary,smfak,squared=F)
{
  if (squared) ary:=sign(ary)*ary^2;
  if (is_integer(smfak/2)) {smfak:=smfak-1};
  pnts:=smfak/2;
  values:=(pnts+1):(len(ary)-pnts); # first and last few values are not smoothed
  for (i in values)  {smo[i]:=mean(ary[(i-pnts):(i+pnts)])}
  #smo:=smo[(pnts+1):(len(ary)-pnts)];
  smo[1:pnts]:=0; smo[(len(ary)-pnts+1):len(ary)]:=0;
  if (squared) smo:=sign(smo)*sqrt(abs(smo));
  return smo;
}
#######################################################################
#median(randomnumbers().discreteuniform(-10,10,9))

#######################################################################
#----------------------------------------------------------------------
#Name: intpol2d
#----------------------------------------------------------------------
#Type: function
#Doc:  performs a 2d interpolation
#Par:  grid   = - grid array of axis (dim)
#Par:  matrix = - data matrix
#Par:  mask   = - mask array for axis (dim)
#Par:  type   = - type of interpolation ('linear','nearest','cubic','spline') 
#Par:  dim    = - axis to interpolate (1=columns,2=rows)
#Ret:  matrix = - interpolated matrix
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 05.10.05 @ LESIA
#----------------------------------------------------------------------
func intpol2d(grid,matrix,mask,type='linear',dim=2) {
#grid:=nstep; matrix:=read_value(spaste(file,'.org.int.sos')); mask:=isteps; dim:=2
  include 'interpolate1d.g'; interp:=interpolate1d()
  if (dim==1) {ret:=array(0,shape(matrix)[1],len(mask))}
  else {ret:=array(0,len(mask),shape(matrix)[2])};
print 'INTPOL2D: retarysize:',shape(ret)
  for (line in 1:shape(matrix)[dim]) {
    if (dim==1) {
      interp.initialize(grid,matrix[line,],type)
      ret[line,]:=interp.interpolate(mask)
    } else {
      interp.initialize(grid,matrix[,line],type)
      ret[,line]:=interp.interpolate(mask)
    };
  }
  return ret
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: intpol2dxy
#----------------------------------------------------------------------
#Type: function
#Doc:  performs a 2d interpolation
#Par:  grid   = - grid array of axis (dim)
#Par:  matrix = - data matrix
#Par:  mask   = - mask array for axis (dim)
#Par:  type   = - type of interpolation ('linear','nearest','cubic','spline')
#Par:  dim    = - axis to interpolate (1=columns,2=rows)
#Ret:  matrix = - interpolated matrix
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 05.10.05 @ LESIA
#----------------------------------------------------------------------
func intpol2dxy(xgrid,ygrid,matrix,xmask,ymask,type='linear') {
  include 'interpolate1d.g'; interp:=interpolate1d()
  for (line in 1:shape(matrix)[1]) {
    interp.initialize(ygrid,matrix[line,],type)
    matrix[line,]:=interp.interpolate(ymask)
  }
  for (line in 1:shape(matrix)[2]) { 
    interp.initialize(xgrid,matrix[,line],type)
    matrix[,line]:=interp.interpolate(xmask)
  }
  return matrix
}
#######################################################################

#######################################################################
#Section: <a name="displaying">DISPLAYING</a> #########################
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: dsary
#----------------------------------------------------------------------
#Type: function
#Doc:  generates dynamic spectrum
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 12.10.05 @ LESIA
#----------------------------------------------------------------------
#Example: ds:=dsary(); printvar(ds); imgplot(ds.res,timvals=ds.step,bandlim=range(ds.freq)/1e6)
#----------------------------------------------------------------------
func dsary(loc='example.its/',head='*.hdr',ant=1,first=1,bn=5,bs=2^8,of=0,bandlim=-1,han=0.5,pwr=F) {
#first:=1;han:=0.5;pwr:=T
  if (loc=='') {return F};
  rec:=[=]; input_project(rec,loc,head=head,blocksize=1,ants=ant)
  filesize:=rec.getmeta('Filesize',1)
  if (bn==-1) {bn:=floor(filesize/bs)-1}; # !!!!! -1 !!!!!
  firstsample:=(first-1)*bs+of
  lastsample:=firstsample+bn*bs
print 'FILESIZE:',filesize
print 'BLOCKNUMBER:',as_integer(filesize/bs)
print 'DATARANGE:',as_integer(firstsample),as_integer(lastsample)
  if (startsample>=filesize) {exit};
  if (lastsample>=filesize) {bn:=floor((filesize-firstsample)/bs)};
  rec.setblock(n=first,blocksize=bs,offset=of,index=1)
  #SetBlock(rec,n=first,blocksize=bs,offset=of,index=1)
  rec.setphase()
  if (han!=-1) {rec.putflag(hanning(bs,alpha=han),'Voltage',1)}
  else {rec.unflag('Voltage',1)};
  frequnit:=rec.headf('FrequencyUnit',1)
  samplerate:=rec.headf('Samplerate',1)*frequnit
  ds:=[=]; freq:=rec.get('Frequency',1)
  fftlen:=rec.getmeta('FFTlen',1)
  if (bandlim!=-1) {bandsel:=freq>=bandlim[1]&freq<=bandlim[2]}
  else {bandlim:=range(freq); bandsel:=array(T,fftlen)};
  freq:=freq[bandsel] #(((1:fftlen)-1)/(fftlen-1)*dif(range(bandlim))+bandlim)*frequnit
  lim:=2^27; intp:=F; if (bn*bs>lim) {intp:=T
    freqres:=dif(range(bandlim))/(as_integer(lim/bn)/2+1)
    ds.freq:=rebin(freq,bin=freqres)
  } else {ds.freq:=freq};
#print 'FREQ'; printvar(freq); print 'DS.FREQ'; printvar(ds.freq)
print 'MEM (2^X doubles):',log(len(ds.freq)*bn)/log(2)
  ds.res:=array(0.i,bn,len(ds.freq)); include 'interpolate1d.g'; interp:=interpolate1d()
  for (block in 1:bn) {print block
    hlp:=rec.get('CalFFT',1)[bandsel]
#print hlp[1]
#if (block==1) {plotdim(log(abs(hlp)),xvals=rec.get('Frequency',1)[bandsel])}
#else {plotdim(log(abs(hlp)),xvals=rec.get('Frequency',1)[bandsel],add=T)};
    if (intp) {
      interp.initialize(freq,hlp) #,type='linear'
      if (pwr) {ds.res[block,]:=abs(interp.interpolate(ds.freq))}
      else {ds.res[block,]:=interp.interpolate(ds.freq)};
    } else {
      if (pwr) {ds.res[block,]:=abs(hlp)}
      else {ds.res[block,]:=hlp};
    }; # endif intp
#printvar(ds.res[block,])
#if (block==1) {plotdim(log(abs(ds.res[block,])),xvals=ds.freq)}
#else {plotdim(log(abs(ds.res[block,])),xvals=ds.freq,add=T)};
#plotdim(log(abs(ds.res[block,])),xvals=ds.freq,add=T)
#print len(bandsel),hlp[1:5],ds.res[block,1:5]
#print ds.res[block,1]
    if (block!=bn) {rec.nextblock(n=1,index=1)};
  } # endfor specs
  print 'number of blocks:',bn
  ds.step:=((1:bn)-1)*bs/samplerate+bs/samplerate/2+of/samplerate
  ds.samplerate:=samplerate
  ds.blocksize:=bs
  ds.freqres:=freqres
  ds.bn:=bn
  ds.avgspec:=mean2d(abs(ds.res))
if (max(ds.res[block,1])==0) {print '!!!',max(ds.res[block,1]),'!!!'};
  return ds
} # end func dsary
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: dsplot
#----------------------------------------------------------------------
#Type: function
#Doc:  plots dynamic spectrum from file
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 07.10.05 @ LESIA
#----------------------------------------------------------------------
#timeres:=0.0008192
#rdir:='/raid/pv3/jupsos/'
#rdir:=spaste(rdir,'X/')
#file:=spaste(rdir,'ITS_jup6_beam_220604_X_2p15_22t32')
#filename:=file
#step:=read_value(spaste(filename,'.sos.timesteps'))
#samplerate:=80e6 #40.002560e6
#save(((1:len(step))-1)*2^15/samplerate+2^15/samplerate/2,spaste(file,'.sos.timesteps'))
#----------------------------------------------------------------------
func dsplot(filename,freqres=-1,timeres=-1,bandlim=-1,bandlimx=-1,bright=-1,xrel=F) {
  if (!fexist(spaste(filename,'.sos'))) {return F};
  # time #
  if (fexist(spaste(filename,'.sos.timesteps'))) {
    step:=read_value(spaste(filename,'.sos.timesteps'))
    if (xrel) {step-:=round(min(step))};
    if (timeres!=-1) {tim:=T
      rstep:=rebin(step,bin=timeres)
      write_value(intpol2d(step,read_value(spaste(filename,'.sos')),rstep,dim=2),spaste(filename,'.int.sos'))
      save(rstep,spaste(filename,'.int.sos.timesteps'))
      step:=rstep
    } else {tim:=F}; # endif timres
  } else {step:=0; tim:=F}; # endif exist timefile
  # freq #
  if (fexist(spaste(filename,'.sos.freqaxis'))&&freqres!=-1) {frq:=T
    freq:=read_value(spaste(filename,'.sos.freqaxis'))
    rfreq:=rebin(freq,bin=freqres)
    if (tim) {readfile:=spaste(filename,'.int.sos')
    } else {
      readfile:=spaste(filename,'.sos')
      if (fexist(spaste(filename,'.sos.timesteps'))) {save(step,spaste(filename,'.int.sos.timesteps'))};
    };
    write_value(intpol2d(freq,read_value(readfile),rfreq,dim=1),spaste(filename,'.int.sos'))
    save(rfreq,spaste(filename,'.int.sos.freqaxis'))
    pixels:=read_value(spaste(filename,'.int.sos'))
  } else {frq:=F}; # endif exist freqfile && freqres
  if (!frq) {
    if (!tim) {pixels:=read_value(spaste(filename,'.sos'))}
    else {pixels:=read_value(spaste(filename,'.int.sos'))};
    if (tim&&fexist(spaste(filename,'.sos.freqaxis')))
    {save(load(spaste(filename,'.sos.freqaxis')),spaste(filename,'.int.sos.freqaxis'))};
  };
  # dsplot #
  if (bandlimx!=-1) {
    freq:=((0:(shape(pixels)[2]-1))/shape(pixels)[2]*dif(range(bandlim))+bandlim[1])*1e6
    sel:=valpos(freq/1e6,bandlimx[1],big=T):valpos(freq/1e6,bandlimx[2],big=F)
    freq:=freq[sel]; pixels:=pixels[,sel]
    bandlim:=range(freq)/1e6
  };
  if (bright==-1) {bright:=0};
print shape(pixels),range(pixels),len(step),range(step),range(dif(step))
  name:=spaste(filename~globalconst.FilenameExtractor,'_',round(bandlim[1]),'t',round(bandlim[2]),'_',bright)
  imgplot(pixels,psdir='',name=name,axisorder=[2,1],bandlim=bandlim,freqdir=1,
    dmin=0,dmax=bright,xax=T,evnums=0,timvals=step,timunit='s',ps=F)
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: imgplot
#----------------------------------------------------------------------
#Type: function
#Doc:  plots a 2D pixel array as an image in the viewer
#Par:  pixels    = [0,0,...] - data array
#Par:  psdir     = '/ps/'    - psfile directory
#Par:  name      = 'plot'    - name of plot
#Par:  axisorder = [fst,sec] - order of axis (first and second)
#Par:  band      = [min,max] - frequency band
#Par:  freqdir   = 1,-1      - frequency axis direction
#Par:  dmin      = 0         - data minimum for plot coloring
#Par:  dmax      = 1000      - data maximum for plot coloring
#Par:  xax       = T,F       - T: tabular xaxis F: linear xaxis
#Par:  evnums    = [1,2,...] - eventnumbers for linear xaxis
#Par:  timvals   = [1:10,..] - time values for tabular xaxis
#Par:  ps        = T,F       - T: print to ps F: do not print to ps
#Ref:  flip2d, coordsys, writops, username
#Example: pix:=array(0,2,2); pix[1:4]:=[1,2,3,4]; imgplot(pix,timvals=0,timunit='-',name='dynamic_spectrum',axisorder=[2,1],bandlim=[0,40]*1e6,freqdir=1,dmin=0,dmax=0,xax=T,evnums=0,wedge=F,gif=T,psname='ds',psdir='',ret=F,done=T)
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 04.06.04 @ KUN
#----------------------------------------------------------------------
imgplot:=function(ref pixels=[],
                  name='Dynamic Spectrum',
                  sosdir='',
                  axisorder=[2,1],
                  bandlim=[0,40]*1e6,
                  freqdir=1,
                  dmin=0,
                  dmax=1,
                  xax=T,
                  evnums=0,
                  timvals=0,
                  timunit='-',
                  ps=F,
                  psdir='',
                  psname='ds',
                  gif=F,
                  done=F,
                  ret=F,
                  wedge=F,
                  wtxt='Intensity',
                  chk=T
                 )                   
{ # INPUT # 
  axsfilename:=spaste(name,'.axs.sos')
  pixfilename:=spaste(name,'.pix.sos')
  absfilename:=spaste(name,'.abs.sos')
  if (sum(shape(pixels))<2) {
    if (fexist(spaste(sosdir,'/',axsfilename))&&fexist(spaste(sosdir,'/',pixfilename))) {
      timvals:=load(spaste(sosdir,'/',axsfilename)).step
      freq:=read_value(spaste(sosdir,'/',axsfilename)).freq
      sel:=valpos(freq,bandlim[1],big=T):valpos(freq,bandlim[2],big=F)
      if (fexist(absfilename)) {pixels:=read_value(spaste(sosdir,'/',absfilename))[,sel]; print 'ABS'}
      else {pixels:=as_float(abs(read_value(spaste(sosdir,'/',pixfilename))[,sel]))};
      ##write_value(as_float(abs(read_value(spaste(sosdir,'/',pixfilename))[,sel])),'hlp.sos')
      ##pixels:=read_value('hlp.sos'); shell('rm -f hlp.sos')
      bandlim:=range(freq[sel]); freq:=[]
    } else {print 'IMGPLOT: no inputmatrix and no sos-files loaded !!!'; return F}; # endif files
  }; # endif no imput matrix
if (chk) {print 'IMGPLOT:',shape(pixels),len(timvals)};
if (shape(pixels)[1]<2) {xax:=F};

  # INCLUDE #
  include 'images.g';
  include 'quanta.g';
  include 'viewer.g';

  # PARAMETERS #
  freqmin:=bandlim[1];
  freqmax:=bandlim[2];
  bandwidth:=freqmax-freqmin;
  timlen:=shape(pixels)[1]
  fftlen:=shape(pixels)[2]

  # PREPARATION #
  freqref:=freqmin;
  freqbin:=bandwidth/(fftlen-1);

 # XAXIS #
  linear:=0; tabular:=T; type:='tabular'; increment:=1; axislabs:=['Frequency','Blocks']
  if (timunit=='') {timunit:='-'};
  if (xax) {
    # time values
    if (timvals!=24&&len(timvals)<timlen) {timvals:=0}; # number of timvals too small
    if (len(timvals)>timlen) {timvals:=timvals[1:timlen]};
       #{timvals:=0; timunit:='-'}; # number of timvals too big
    if (len(timvals)==1&&timvals==0) {timvals:=1:timlen}
    else {axislabs:='Frequency Time'}; # time OR column numbers
    if (timvals==24&&timlen>2500) {timvals:=(1:timlen)/timlen*24}; # 24h evn>2500
    # event numbers
    if (evnums==0) {evnums:=1:len(timvals)};
  }; # if tab xaxis

  # FREQUENCYFLIP #
  if (freqdir==-1) {pixels:=flip2d(pixels,1); freqref:=freqmax; freqbin*:=-1};
chk:=F; if (chk) {
  print 'IMGPLOT freqmin:',freqmin
  print 'IMGPLOT freqbin:',freqbin
  print 'IMGPLOT freqref:',freqref+freqbin/2
};

  # CSYS #
  cs:=coordsys(linear=linear,tabular=tabular,spectral=T);
  # general #
    cs.setnames(value=axislabs)
    #cs.setepoch(dm.epoch('utc',qnt.quantity(data.head('Date'))));
    #cs.settelescope(data.head('Observatory'));    
    cs.setobserver(username());
    cs.reorder(axisorder);
  # frequency (spectral/direction) axis #!!!
    cs.setreferencepixel(1,'spectral');
    cs.setreferencevalue(dq.quantity(spaste((freqref+freqbin/2),'Hz')),'spectral');
    cs.setincrement(dq.quantity(spaste((freqbin),'Hz')),'spectral');
  # time (linear/tabular) axis #
    cs.setunits(timunit,type=type,overwrite=T);
    cs.setincrement(increment,type);
  # special #
    if (xax) {cs.settabular(pixel=evnums, world=timvals)};
    #cs.summary(list=F)

  # IMAGE #
  ##img:=imagefromarray(pixels=as_float(abs(pixels)),csys=cs)
  img:=imagefromarray(pixels=pixels,csys=cs) # "as_float(abs(pixels))" has to be done before (dstb)
  cs.done()

  # BRIGHTNESS #
  #if (dmax==0) {dmax:=0.5};
  if (dmax==0) {
    histo:=hist(pixels,bins=100,ret=T,disp=F)
    dmax:=histo.val[valpos(histo.vlc,max(histo.vlc))]*10
  };
  #if (dmax==0) {dmax:=max(as_float(abs(pixels)))};
  #  pixelsmin:=as_float(pixels)
  #  for (row in 1:timelen) {pixelsmin[row,]:=runminc(as_float(pixels[row,]),bs=10)}
  #  tdmax:=mean(pixelsmin)+1.5*stddev(pixelsmin)
  #}; #{dmin:=min(as_float(pixels)); dmax:=max(as_float(pixels))};
  
  # CLEAN UP #
  pixels:=[]
  
  # NAME #
  #if (timvals=0) {timvals:=''}; #'_',timvals[1])
  name:=spaste(name,' ',round(bandlim[1]/1e6*10)/10,'t',round(bandlim[2]/1e6*10)/10,
               ' ',spaste(split(as_string(round(dmin*10)/10),'')),'t',
               spaste(split(as_string(round(dmax*10)/10),'')))

  # VIEWER #
  vid:=dv.loaddata(img,'raster',autoregister=T)

  vid.setoptions([
  # general #
    axislabelswitch=T,
    axislabels=T,
    titletext=name,
    xgridtype=['Tick marks'],xgridcolor='white',
    ygridtype=['Tick marks'],ygridcolor='foreground',
    axislabelspectralunit=['MHz'],
    colormap=['Hot Metal 1'],
  # wedge #
    wedge=[dlformat='wedge',listname='Color Wedge',ptype='boolean',
           value=wedge,default=T,context='Color_Wedge'],
    wedgeyaxistext=[dlformat='wedgeaxistext',listname='axis label',ptype='string',value=wtxt,
                    default='Intensity',allowunset=F,context='Color_Wedge'],
    wedgeyaxistextcolor=[dlformat='wedgeaxistextcolor',listname='axis label color',ptype='userchoice',
                         popt='foreground background black white red green blue yellow',
                         value='foreground',default='foreground',allowunset=F,context='Color_Wedge'],
  # data limits #
    datamin=[value=dmin],datamax=[value=dmax]])
    
  vdp:=dv.newdisplaypanel()
  vdp.register(vid)

  # PS & GIF #
  if (psname=='') {psname:=name};
  if (ps|gif) {writops(handle=vdp,psdir=psdir,name=psname,gif=gif,done=done)};
  
  # RETURN #
  if (ret) {return vdp};
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: plotdim
#----------------------------------------------------------------------
#Type: function
#Doc: plots each column of a matrix
#Example: plotdim(log(abs(data.get('FFT',1))),xvals=data.get('Frequency')/1e6,title='Power Spectra',xlab='Frequency [MHz]',ylab='Spectral Power Density [absFFT(ADCcounts)]'); plotdim(log(abs(data.get('FFT',2))),xvals=data.get('Frequency')/1e6,add=T)
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 13.03.05 @ KUN
#----------------------------------------------------------------------
#Symbols: sr:=-100:100; plotdim([0.1,-.1],xvals=range(sr),sym=-2); for (sn in sr) {plotdim(0,xvals=sn,sym=sn,add=T)}
#----------------------------------------------------------------------
plotdim:=function(ary,dim=1,xvals=[=],xrange=[=],yrange=[=],head='plotdim',
                  title='',axs=0,xlab='',ylab='',ptxt=[],col=[],sym=-1,sty=1,wid=1,
                  pwedg=F,ps=F,psdir='',psname='',gif=F,add=F,rpl=F,ret=F,done=F)
{
  # PARAMETER
  psize:=[600,450] # standard size
  foreground:='black'; background:='white' # basic colors
  inv:=F   # inverse fore- & background color
  axs:=axs # controls the plotting of axes, tick marks, etc:
           # -2 : draw no box, axes or labels
           # -1 : draw box only
           #  0 : draw box and label it with coordinates
           #  1 : same as AXIS = 0, but also draw the coordinate axes (X = 0, Y = 0)
           #  2 : same as AXIS = 1, but also draw grid lines at major increments of the coordinates;
           # 10 : draw box and label X-axis logarithmically
           # 20 : draw box and label Y-axis logarithmically
           # 30 : draw box and label both axes logarithmically
  plw:=wid # width of line in units of 0.005 inch (0.13 mm) in range 1-201
  pfn:=1   # 1 simple single-stroke font, 2 roman font, 3 italic font, 4. script font
  pch:=1.5 # character height as multiple of 1/40 the height of the view surface
  pls:=sty # 1 (full line), 2 (dashed), 3 (dot-dash-dot-dash), 4 (dotted), 5 (dash-dot-dot-dot)
  plc:=col # 1-black, 2-red, 3-green, 4-blue, 5-cyan, 6-magenta, 7-yellow, 8-orange, 9-lightgreen,
           # 10-türkis, 11-lightblue, 12-purple, 13-rosa, 14-darkgray, 15-lightgray, 16-black...
           # GUIants: 1-blue, 2-cyan, 3-magenta, 4-yellow, 5-orange, 6-green,
           # 7-türkis, 8-lightblue, 9-purple, 10-rosa, 11-darkgray, 12-lightgray, 13-black...
  
  # DECISION
  if (inv) {foreground:='white'; background:='black'};
  if (sym==-1) {plotlines:=T} else {plotlines:=F};

  # PLOTTING
  if (!add) {
    #if (is_defined('plotdh')) {plotdh.done()};
    global plotdh:=pgplotter(size=psize,foreground=foreground,background=background)
    plotdh.clear(); plotdh.title(head); plotdh.slw(plw); plotdh.scf(pfn); plotdh.sch(pch)
    if (len(plc)==0) {plc:=2};
    plotdh.plc:=plc
  }; # else {global plotdh};
  if (len(shape(ary))==2)
  {
    if (dim==1)
    {
      arylen:=shape(ary)[1];
      if (len(xvals)<len(ary[1,])) {xxvals:=1:len(ary[1,])};
      if (shape(xrange)==0) xrange:=[min(xvals),max(xvals)];
      if (shape(yrange)==0) yrange:=[min(ary),max(ary)];
      if (len(title)<arylen) {title:=array('',arylen)};
      for (row in 1:arylen)
      {if (shape(xvals)==shape(ary)) {xxvals:=xvals[row,]};
       plotdh.env(xrange[1],xrange[2],yrange[1],yrange[2],0,axs)
       plotdh.plotxy(xxvals,ary[row,],newplot=F,plotlines=plotlines,ptsymbol=sym)
       plotdh.lab(toplbl=title[row],xlbl=xlab,ylbl=ylab)}
    };
    if (dim==2)
    {
      arylen:=shape(ary)[2];
      if (len(xvals)<len(ary[,1])) {xxvals:=1:len(ary[,1])};
      if (shape(xrange)==0) xrange:=[min(xvals),max(xvals)];
      if (shape(yrange)==0) yrange:=[min(ary),max(ary)];
      if (len(title)<arylen) {title:=array('',arylen)};
      for (col in 1:arylen)
      {if (shape(xvals)==shape(ary)) {xxvals:=xvals[,col]};
       plotdh.env(xrange[1],xrange[2],yrange[1],yrange[2],0,axs)
       plotdh.plotxy(xxvals,ary[,col],newplot=F,plotlines=plotlines,ptsymbol=sym)
       plotdh.lab(toplbl=title[col],xlbl=xlab,ylbl=ylab)}
    };
  }
  else
  {
    if (len(xvals)==0) {xvals:=1:len(ary)};
    if (shape(xrange)==0) {xrange:=[min(xvals),max(xvals)]; xspace:=dif(xrange)/40} else {yspace:=0};
    if (shape(yrange)==0) {yrange:=[min(ary),max(ary)]; yspace:=dif(yrange)/10} else {yspace:=0};
    if (!add&&!rpl) {plotdh.env(xrange[1]-xspace,xrange[2]+xspace,
                     yrange[1]-yspace,yrange[2]+yspace,0,axs)};
    if (add) {global plotdh; if (len(plc)==0) {plc:=plotdh.plc+1}; plotdh.plc:=plc};
    #plotdh.setxscale(x1,x2); plotdh.setyscale(y1,y2)
    plotdh.sls(pls); plotdh.sci(plc); plotdh.slw(plw)
    if (plotlines) {plotdh.line(xvals,ary)} else {plotdh.pnts(xvals,ary,symbol=sym)};
    #plotdh.plotxy(xvals,ary,newplot=F,plotlines=plotlines,ptsymbol=sym)
    #plotdh.plotxy1(xvals,ary,plotlines=plotlines,ptsymbol=sym)
    if (len(ptxt)!=0) {plotdh.ptxt(xrange[1]+dif(xrange)/20,yrange[1]+dif(yrange)/20,0,0,ptxt)};
    if (!rpl) {plotdh.sci(1); plotdh.lab(toplbl=title,xlbl=xlab,ylbl=ylab)};
    if (pwedg) {plotdh.sci(1); plotdh.wedg('R',1,10,1,0,'wedg')};
  };

  # PS & GIF #
  if (psname=='') {psname:=title};
  if (ps|gif) {writops(handle=plotdh,psdir=psdir,name=psname,gif=gif,done=done)};

  # RET #
  if (ret) {return plotdh};
} #end function plotdim

if (1!=1) {
handle:=pgplot(foreground='black',background='white',width=100,height=100)

pgb->slw(1)   # line width
pgb->scf(1)   # font
pgb->sch(1.5) # character height (1/40 of viewsurface)
pgb->sci(2)   # line color
pgb->env(xrange[1],xrange[2],yrangelc[1],yrangelc[2],0,0)
pgb->sci(plc)
pgb->line(timeax,light)
pgb->sci(1); pgb->lab(toplbl=xlablc,xlbl=ylablc,ylbl=titlelc)
}
#######################################################################
#if (ps) {plotdh.psprinttofile(file=spaste(psdir,title,'.ps'),color=T,landscape=T)};
#writops(handle=plotdh,psdir=psdir,name=title,lands=F,gif=T,done=F)

#######################################################################
#----------------------------------------------------------------------
#Name: writops
#----------------------------------------------------------------------
#Type: function
#Doc: generates psfile from viewer and pgplotter
#Par: gif  convert ps to gif (lands=F)
#Example: writops(handle=pgp,psdir='',name='psprinttofile')
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 04.06.04 @ KUN
#----------------------------------------------------------------------
writops:=function(handle=F,psdir='',name='writops',delay=2,lands=T,gif=F,done=F)
{
  if (psdir==''&&fexist('ps/')) {psdir:='ps/'};
  psfile:=spaste(psdir,name,'.ps')
  gifile:=spaste(psdir,name,'.gif')
  # canvasprintmanager #
  if (len(field_names(handle)[field_names(handle)=='canvasprintmanager'])==1)
  {
    shell(spaste('sleep ',delay,'s')); #WAIT
    if (!(handle==F))
    {
      if (!fexist(psdir)) {shell(spaste('mkdir ',psdir))};
      if (gif) {lands:=F};
      handle.canvasprintmanager().
      writeps(filename=psfile,media='A4',landscape=lands,dpi=300,zoom=1,eps=F);
      if (fexist(psfile)) {
        print spaste('[writops] ',psfile,' saved')
        if (gif) {shell(spaste('cd ',psdir,'; ps2gif ',name,'.ps'))};
        ##if (gif) {shell(spaste('ps2gif ',psfile))};
      }; # endif fexist ps-file
      shell(spaste('sleep ',delay,'s')); #WAIT
    };
  }; # canvas
  # psprinttofile #
  if (len(field_names(handle)[field_names(handle)=='psprinttofile'])==1)
  {
    if (!(handle==F))
    {
      if (!fexist(psdir)) {shell(spaste('mkdir ',psdir))};
      handle.psprinttofile(file=psfile,color=T,landscape=lands);
      if (fexist(psfile)) {
        print spaste('[writops] ',psfile,' saved')
        if (gif&&lands) {shell(spaste('cd ',psdir,'; psrot ',name,'.ps'))};
        if (gif) {shell(spaste('cd ',psdir,'; ps2gif ',name,'.ps'))}; # ps2gif
        ##if (gif&&lands) {shell(spaste('psrot ',psfile))}; # rotR90
        ##if (gif) {shell(spaste('ps2gif ',psfile))}; # ps2gif
        # lands=T:psrot,ps2gif & lands=F:ps2gif
      }; # endif fexist ps-file
    };
  }; # psprint
  # close #
  if (fexist(gifile)) {print spaste('[writops] ',gifile,' saved')};
  if (done) {handle.done()};
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: timestamps
#----------------------------------------------------------------------
#Type: function
#Doc: function checks whether time infomation file exists
#     if not file exists it tries to create one
#     if there are not enough time information elements it extrapolates
#     if there are too many time information elements it notifies
#Example: filename:='/home/anigl/data/lopesgui/lopesdyn/sos/lopes-040113/mp/ds_040113_0_evx_mp1to8.sos'; timestamps(filename=filename,frac=F,rec=F,sav=T)
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 06.03.05 @ MPIfR
#----------------------------------------------------------------------
#filename:='/home/anigl/data/lopesgui/lopesdyn/sos/lopes-041221/mp/ds_041221_0_evx_mp_allant.sos'; viewersteps:=timestamps(filename=filename,frac=F,rec=F,sav=T)
#----------------------------------------------------------------------
func timestamps(filename='',frac=F,rec=F,sav=T) {
#/home/anigl/data/lopesgui/lopesdyn/sos/lopes-040113/mp/ds_040113_0_evx_mp1to8.sos
  
  # check for timestamp file #
  if (frac) {sfile:=spaste(filename,'.timestamps')} else {sfile:=spaste(filename,'.timesteps')};
  if (sav) {print 'TIMESTAMPS:',sfile};
  if (!rec&&fexist(sfile)) {
    viewersteps:=read_value(sfile)
print 'TIMESTAMPS: file loaded'
  } else {viewersteps:=0};

  # generate timestamps #
  if (len(viewersteps)==1) {
print 'TIMESTAMPS: file error, generate timestamps, please wait...'
    date:=split(filename~globalconst.FilenameExtractor,'')[4:9]
    evdir:=spaste(datadir,'LOPES/20',spaste(date[1:2]),'/',spaste(date[3:4]),'/',spaste(date[5:6]),'/')
    files:=filesel(evdir,ext='*.event*',first=1,last=-1,disp=F)~globalconst.FilenameExtractor;
    if (len(files)==1) {print files; if (!is_string(files)) {if (!(files))
    {print 'TIMESTAMPS: no event files !!!'}}}
    else {
      timesteps:=fileinfo(files,events=T,blockinfo=[1,-1,2^16],frac=frac,rel=F,sep=F,disp=F,ps=F);
      if (!is_double(timesteps)&!is_integer(timesteps)) {if (!(timesteps)) return F};
      if (sav) {write_value(timesteps,sfile)} else {viewersteps:=timesteps};
    }; # end if files
    if (sav) {  
      if (fexist(sfile)) {
        print spaste('TIMESTAMPS: ',sfile,' successfully saved !!!')
        viewersteps:=read_value(sfile);
      } else {print spaste('TIMESTAMPS: ',sfile,' does not exist'); return 0};
    }; # endif file saving
  }; # end viewersteps
  
  # length check #
  if (fexist(filename)) {
    pixlen:=shape(read_value(filename))[1]
    viwlen:=len(viewersteps)
#print 'TIMESTAMPS:',pixlen,viwlen
    if (len(viewersteps)<pixlen) {
      dist:=mean(dif(viewersteps))
      if ((viewersteps[viwlen]+dist*(pixlen-viwlen))>24) {dist:=(24-viewersteps[viwlen])/(pixlen-viwlen)}
      viewersteps[(viwlen+1):pixlen]:=viewersteps[viwlen]+dist*(1:(pixlen-viwlen))
      if (sav) {write_value(viewersteps,sfile)};
      print 'TIMESTAMPS: number of timestamps is smaller than size of dynamic spectrum'
      print 'TIMESTAMPS: corrected with extrapolation'
      print 'TIMESTAMPS: time stamps above',viewersteps[viwlen],'are unreliable'
    };
    if (len(viewersteps)>pixlen)
    {print 'TIMESTAMPS: number of time info files is bigger than size of dynamic spectrum'};
  }; # endif pixel sos file exists
  
  # return #
print 'TIMESTAMPS:',pixlen,len(viewersteps)
#print 'TIMESTAMPS: return'
  return viewersteps
}

#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: fileinfo
#----------------------------------------------------------------------
#Type: function
#Doc: Function creates xaxis time values for plotting. It can extract
#     the time infomation from (1) event file(s), (2) event file(s)
#     with a different ending (packed event files) or (3) project files.
#     The parameter FRAC enables the timestamps in hours,
#     but standard is a timestamp in the form HOUR,MIN.
#(!!! the filesize for eventfiles should remain below one minute !!!)
#Example: [1] files:=filesel(spaste(datadir,'LOPES/2005/02/03/'),first=1,last=-1); viewersteps:=fileinfo(files=files,events=T,blockinfo=[1,-1,2^16],rel=F,sep=F,disp=T,ps=F);
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 28.05.03 @ MPIfR
#----------------------------------------------------------------------
# DEMONSTRATION #
# single event
#include 'dynspectools.g'; files:=filesel(spaste(datadir,'LOPES/2005/02/03/'),first=1,last=1); viewersteps:=fileinfo(files=files,events=T,blockinfo=[1,-1,2^16],frac=F,rel=T,sep=F,disp=F,ps=F);
# events
#include 'dynspectools.g'; files:=filesel(spaste(datadir,'LOPES/2005/02/03/'),first=1,last=10); viewersteps:=fileinfo(files=files,events=T,blockinfo=[1,-1,2^16],frac=F,rel=T,sep=F,disp=F,ps=F);
# packed events
#include 'dynspectools.g'; files:=filesel(spaste(datadir,'LOPES/2003/10/28/'),first=1,last=10); viewersteps:=fileinfo(files=files,events=F,blockinfo=[1,-1,2^16],frac=F,rel=T,sep=F,disp=F,ps=F);
# project (monoton)
#include 'dynspectools.g'; viewersteps:=fileinfo(files='',events=F,blockinfo=[1,-1,2^16],frac=F,rel=T,sep=F,disp=F,ps=F);
#----------------------------------------------------------------------
# TIMESTEPSFILE # (saving)
#include 'dynspectools.g'; files:=filesel(spaste(datadir,'LOPES/2004/12/24/'),first=2001,last=2500); viewersteps:=fileinfo(files=files,events=T,blockinfo=[1,-1,2^16],frac=F,rel=F,sep=F,disp=F,ps=F); write_value(viewersteps,'/raid/pv2/LOPES/sun/sos/lopes-041224/mp/ds_041224_0_ev2001_mp_10ants.sos.timesteps')
#----------------------------------------------------------------------
#viewersteps:=fileinfo(files=files,events=T,blockinfo=[1,-1,2^16],frac=F,rel=T,sep=F,disp=F,ps=F);
#----------------------------------------------------------------------
#Update: take start block into account
#----------------------------------------------------------------------
fileinfos:=function(files='',events=F,blockinfo=[1,-1,2^16],intblockn=1,frac=F,rel=T,sep=F,disp=T,ps=F)
{
  # FILESTEPS / TIMESTEPS -----------------------------------------------
  files:=files~globalconst.FilenameExtractor # extract filenames for dates
   
  # DATA TYPE #
  dates:='' # variable for datum / data
  
  # EVENT FILE(S) [events=T & files is not empty & filenames smaller then packed filenames]
  #2003.08.01.00:00:13.162.event
  #123456789X123456789X123456789
#print events,files[1],len((split(files[1],'')))
  if (events&&files!=''&&len((split(files[1],'')))<30) {
    for (filenum in 1:len(files)) {
      datesplit:=split(files[filenum],'')
      if (len(datesplit)==29) {dates[filenum]:=spaste(datesplit[1:23])}
      else {rec:=[=]; input_event(rec,files[filenum]); dates[filenum]:=rec.head('Date')};
    }
    print 'FILEINFO: event file(s)',dates[1];
  };
  
  # (packed event) FILE(S) [events=F & files is not empty & filename is longer then 29 chars & including event]
  #2003.08.01.00:00:13.162.event.gz
  #123456789X123456789X123456789X12
#print events,files[1],len((split(files[1],''))),spaste(split(files[1],'')[25:29])
  if (!events&&files!=''&&len((split(files[1],'')))>29&&spaste(split(files[1],'')[25:29])=='event') {
    for (filenum in 1:len(files)) {dates[filenum]:=spaste(split(files[filenum],'')[1:23])}
    print 'FILEINFO: (packed event) file(s)',dates[1];
  };
  
  # PROJECT [event=F & files empty & data is a record and loaded before]
#print events,files[1],data.len
  if (!events&&files==''&&is_record(data)) {
    dates:=data.head('Date')
    print 'FILEINFO: project file',dates;
  };
  
  # RETURN
  if (dates=='') {print 'FILEINFO: no time information available'; return};

  # STEPS #
  numfiles:=len(files);
  if (frac) {type:=60} else {type:=100}; # [type=100:] 10,51=10:51 & [type=60:] 10,6=10:36
  for (filenum in 1:numfiles) {
    fs:=split(dates[filenum],'')
    if (len(fs)<23) {fs[len(fs):23]:='0'};
    fn:=as_integer(fs)
    if (len(fs)==23) {msec:=fn[23]+fn[22]*10+fn[21]*100} else {msec:=0}; # millisecond
    secs:=fn[19]+fn[18]*10 # second
    mins:=fn[16]+fn[15]*10 # minute
    hour:=fn[13]+fn[12]*10 # hour
    days:=fn[10]+fn[9]*10  # day
    #month:=fn[7]+fn[6]*10  # month
    #year:=fn[4]+fn[3]*10+fn[2]*100+fn[1]*1000 # year
    filesteps[filenum]:=msec/(100*1000*type)+secs/(100*type)+mins/type+hour #hours:min,min (for tabular axis)
    timesteps[filenum]:=as_integer(spaste(spaste(fs[12:13]),spaste(fs[15:16]),
                                   spaste(fs[18:19]),spaste(fs[21:23]))) #hhmmssfff (for linear axis)
    #'hhmmssfff' more than that is not possible due to size/capacity of integer variables
#print timesteps[filenum],filesteps[filenum],len(fs),msec,secs,mins,hour,days
  } # end numfiles
  if (!events&&files!=''&&spaste(split(files[1],'')[25:29])=='event') {events:=T};

  # BLOCKING ------------------------------------------------------------
  # number of blocks (per file)
  
  # BLOCKINFO #
  if (is_record(data)) {
    filesize:=data.getmeta('Filesize')
    samplerateunit:=data.headf('SamplerateUnit');
    samplerate:=data.headf('Samplerate')*samplerateunit;
  } else {
    filesize:=blockinfo[3]
    samplerateunit:=1e6
    samplerate:=80*samplerateunit
  };
#print 'FILEINFO: samplerate:',samplerate
  if (blockinfo[2]==-1) blockinfo[2]:=filesize/blockinfo[3];
  
  # TIMERESOLUTION # 
  timeres:=1/samplerate #seconds
  if (!rel) {if (events) {timeres/:=type}}; #hh:mm,mm

  # BLOCKTIMESTEPS #
  for (filenum in 1:numfiles) {
    step:=filesteps[filenum]

    # start time
    abstime:=0
    if (!rel) {
      if (events) {abstime:=step}                       #EVENTS: absolute start time hh:mm,mm
      else {abstime:=(step*type-floor(step*type))*100}; #OTHER:  absolute start time in seconds
      ###else {abstime:=step-floor(step*100)/100};
    };
    # MONOTON: blockstarttime PLUS a half blocksteptime is the blocktime in [s] to be added to obsstarttime
    # EVENTFILES: filesteptime MINUS offset from trigger applies for the event file center
    
    # event files time offset #
    if (events&&is_record(data)) {offset:=-data.getmeta('Blocksize')/samplerate/2}
    else {offset:=-2^16/samplerate/2}; # eventtriggeroffset [data.get('Time')[1]:=-0.0004095875 ???]
    if (events) {if (!rel) {offset/:=type}; abstime+:=offset}; #!!!#100belowminute

    # block start
    if (blockinfo[2]>1) {
      if (sep) {start:=(filenum-1)*(blockinfo[2]+1)+1} # some blocks per file with spacing
      else {start:=(filenum-1)*blockinfo[2]+1}} # some blocks per file
    else {start:=filenum}; # one block per file

    # time shift
    shift:=1/2 # for time at mid of block

#print 'abstime:',abstime
#print 'blocktime:',blockinfo[3]*timeres*(1-shift)

    # blocks
    for (block in (start):(start+blockinfo[2]-1)) {
      viewersteps[block]:=(abstime+blockinfo[3]*timeres*(block-shift))
    }

  } # end numfiles
  
  # BLOCK INTEGRATION
  viewersteps:=blockint(viewersteps,intblockn)

  # MEAN TIME STEP DISTANCE # !!! modify to display viewersteps
  meantimestepdistance:=0;
  if (numfiles>2)
  {
    for (i in 1:(numfiles-2)) {dist[i]:=timesteps[i+1]-timesteps[i]} #; print dist[i]/100}
    meantimestepdistance:=mean(dist/1000);
    # displaying #
    if (disp)
    {
      pgp:=pgplotter();
      pgp.plotxy(1:len(timesteps),timesteps);
    };
    # psfile #
    if (ps)
    {
      if (!fexist(psdir)) {shell(spaste('mkdir ',psdir))};
      pgp.psprinttofile(spaste('times_ev',efirst,'t',elast,'.ps'));
    };
  };

  # PRINT #
  #print 'FILEINFO: block time resolution:',timeres*blockinfo[3];
  #print 'FILEINFO: steps from files:',len(filesteps),max(filesteps),filesteps[1]  
  #print 'FILEINFO: returned steps:',len(viewersteps),max(viewersteps),viewersteps[1]
  #if (numfiles>2) print 'FILEINFO: average time difference:',meantimestepdistance,'s'

  # RETURN #
  return viewersteps;
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: plotantpos
#----------------------------------------------------------------------
#Type: function
#Doc:  plots antenna positions from data structure
#Par:  file =   - directory and filename
#Par:  type = T - lines or F: points
#Ret:  tmp   2D-coordinates of antennas in respect to ref antenna
#Ref:  input_event, CalcAntPos
#Example: input_project(data,'/raid/pv3/ITS/jupiter/2004.06.22-15:37:34.00-jupiter06/',ants=1); plotantpos(data)
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 04.08.03 @ MPIfR
#----------------------------------------------------------------------
plotantpos:=function(ref rec,type=F,ret=F)
{
  #input_event(data,file)
  ants:=rec.selant()
  nants:=len(rec.selant())
  antpos:=CalcAntPos(rec,ants,1,1)
  print "antennas:",ants
  pos:=array(0.,nants,3);
  for (i in 1:nants) {pos[i,1:3]:=antpos[i].position}
  tmp:=pos[1:nants,1:2];
  pgp:=pgplotter(foreground='black',background='white')
  pgp.plotxy(tmp[1:nants,2],tmp[1:nants,1],plotlines=type,ptsymbol=8)
  title:=spaste(rec.head('Observatory'),' Antenna Positions')
  pgp.lab(toplbl=title,xlbl='WEST - EAST',ylbl='NORTH - SOUTH');
  if (ret) return tmp;
}
#######################################################################

#######################################################################
#Section: <a name="tools">TOOLS</a> ###################################
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: envelope
#----------------------------------------------------------------------
#Type: function
#Doc:  returns data array with envelope of input data
#Par:  yary = - input array
#Par:  fary = - filtered input array
#Par:  bs   = - blocksize (odd number for window center value)
#Par:  side = - 1 = envelope of upper side or 2 = lower side of input
#Par:  bd   = - blockdistance (between beginning of two blocks)
#Par:  sig  = - sigma factor for decision level
#Ret:  outary
#Example:
#[1]# tic(); env:=envelope([1,2,3,2,1],[1,2,3,4,5],1,3,1,2,F,T); toc()
#[2]# include 'dynspectools.g'; yary:=read_value('jup6_meanspec.sos'); fary:=0; runningavgstdnew(runminc(yary),fary,0,50); freq:=((0:(len(yary)-1))/len(yary)*6+25)*1e6; env:=envelope(yary,fary=fary,xary=freq,side=-1,bs=15,bd=1,sig=3,disp=T); env.plotdh.lab(toplbl='Power Spectrum',xlbl='Frequency [MHz]',ylbl='absFFT')
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 19.05.04 @ Kleve
#----------------------------------------------------------------------
func envelope(yary,fary=[],xary=[],side=1,bs=15,bd=1,sig=3,dsig=1,gau=F,disp=F)
{
  arylen:=len(yary)
  if (len(xary)>0) {if (len(xary)!=arylen) {return F};} else {xary:=1:arylen};
  if (len(fary)>0) {if (len(fary)!=arylen) {return F};} else {fary:=yary};
  if (bs<arylen&&bs>1) # if blocksize smaller arylen
  {
    # subfunction
    func envpnt(ref ac,ref env,ref dary,ref fary,ref yary,ref xary,i,sel,sig) {
      mary[i]:=mean(fary[sel]); sary[i]:=stddev(dary[sel])
      dec:=sig*sary[i]; ysum:=sum(dary[sel])
      #if (ysum<dec&&ysum>-dec&&side*fary[i]>side*mary[i]) {val ac+:=1 # no gaussfit for minima
      if (ysum<dec&&ysum>-dec&&fary[i]>mary[i]) {val ac+:=1 # decision by stddev & mean of window[i]
        if (gau) { # gaussfit on window with maximum
          ret:=gaussfit(yary,n1=sel[1],n2=sel[len(sel)],center='-1',height='-1',width=1,maxiter=30)
          # > gauss fits are to wide and maxima too low (but would compensate extremum resolution dependency)
#print i,ret.result.center+sel[1]-1
#print yary[i],ret.result.height
          env.xary[ac]:=xary[ret.result.center+sel[1]-1]
          env.xerr[ac]:=ret.result.center_error
          env.yary[ac]:=ret.result.height
          env.yerr[ac]:=ret.result.height_error
          env.chisq[ac]:=ret.result.chisq
        #} else {env.yary[ac]:=yary[i]; env.xary[ac]:=xary[i]} # point at decision maximum
        #} else {env.yary[ac]:=max(yary[sel]); env.xary[ac]:=xary[i]} # max yvalue of window at xdecision
        } else {env.yary[ac]:=max(yary[sel]); env.xary[ac]:=xary[sel][valpos(yary[sel],env.yary[ac])]}
        # max yvalue of window and its xpos (> accuracy due to resolution dependency ?)
      }; # end if decision for abscissa intersection
    } # end subfunction

    # prepare
    if (side==-1) {yary:=1/yary; fary:=1/fary}; # minima to maxima for gaussfit
    dary:=dif(fary) # first derivative for abscissa intersection for extrema
    left:=floor((bs-1)/2); right:=ceil((bs-1)/2) # search window size
    env:=[=]; env.yary:=0; env.xary:=0; ac:=0 # envpnt subfunction parameters

    # first part (x smaller than bs)
    for (i in seq(1,left,bd)) {
      sel:=1:(i+right)
      envpnt(ac,env,dary,fary,yary,xary,i,sel,sig)
    } # end for first part of input
    
    # main part
    for (i in seq(left+1,arylen-right-1,bd)) {
      sel:=(i-left):(i+right)
      envpnt(ac,env,dary,fary,yary,xary,i,sel,sig)
    } # end for main part of input

    # last part (arylen-x smaller than bs)
    for (i in seq(arylen-right,arylen-1,bd)) {
      sel:=(i-left):(arylen-1)
      envpnt(ac,env,dary,fary,yary,xary,i,sel,sig)
    } # end for last part of input
    
    # side
    if (side==-1) {yary:=1/yary; fary:=1/fary; env.yary:=1/env.yary}; # maxima to minima

    # mean maxima
    envlen:=len(env.xary)
    if (envlen>1) {
      dsig:=1 # distance between neighbouring values in fractions of sigma !!!
      doppelt:=(1:(len(env.xary)-1))[dif(env.xary)<stddev(dif(env.xary))/dsig];
      valc:=0; for (valpos in doppelt) {
        env.xary[valpos-valc+1]:=mean(env.xary[valpos-valc],env.xary[valpos-valc+1])
        env.yary[valpos-valc+1]:=mean(env.yary[valpos-valc],env.yary[valpos-valc+1])
        env.xary:=valext(env.xary,valpos-valc); env.yary:=valext(env.yary,valpos-valc)
      valc+:=1}
    }; # endif envelope length
    
    # disp
    if (disp) {
      plotdim(yary,xvals=xary)
      plotdim(fary,xvals=xary,add=T)
      env.plot:w
dh:=plotdim(env.yary,xvals=env.xary,ret=T,add=T)
    }

  } else {env:=F};

  # return
  return env

} # end of function envelope
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: xcor
#----------------------------------------------------------------------
#Type: Function
#Doc:  calculates cross correlation of two arrays 
#Par:  ary1,ary2 = - arrays for cross-correlation
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 11.10.05 @ LESIA
#----------------------------------------------------------------------
func xcor(ary1,ary2) {sum(ary1*ary2)/(sqrt(sum(ary1^2))*sqrt(sum(ary2^2)))}
#ary1-:=mean(ary1); ary1-:=mean(ary2)
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: xcorwin
#----------------------------------------------------------------------
#Type: Function
#Doc:  calculates cross correlation in a window for two arrays
#      cross correlating ALL ELEMENTS of both input arrays with each other
#      and rotates second input array by SINGLE samples (no window extraction)
#Par:  ary1,ary2 = - arrays for cross-correlation (average to zero for arrays before FFT)
#Par:  pix       = - parameter for correlation window
#Par:  off       = - offset for input arrays
#Par:  shift     = - shfiting for envelope: sqrt(xcorÂ²+xcorpiÂ²)
#Ret:  ret       = - array with normalized correlation coefficients
#Update: step width for xcor could be subsamples for freq and time method if necessary
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 13.03.05 @ NS
#----------------------------------------------------------------------
#Example: Demo
#include 'xcorwin.cpp.g'; arylen:=10; include 'mathematics.g'; rand:=randomnumbers(); ary1:=rand.uniform(-1,1,arylen); rand.reseed(); include 'fftserver.g'; tool:=fftserver(); ary2:=tool.shift(ary1,1); pix:=2; ret:=xcorwin(ary1,ary2,pix=pix,off=0,shift=1); print ret.cor; plotdim(ret.cor,xvals=(1:len(ret.cor))-1-pix); print ret.scor; plotdim(ret.scor,xvals=(1:len(ret.scor))-1-pix); plotdim(sqrt(ret.cor^2+ret.scor^2)/sqrt(2),xvals=(1:len(ret.cor))-1-pix)
#Example: Wavepackets (30MHz waveform with 100Âµs/10kHz wavepackets in a window of half the period of a sin)
#fc:=30e6; sr:=80e6; bs:=2^16; wptime:=100e-6; rec:=fft2wf(fft=[1+1i],frq=[fc],sr=sr,ds=bs,sign=1,progres=T); rec.x*:=sin(2*pi*1/wptime*rec.t)*sin(2*pi*1/(bs/samplerate)/2*rec.t); plotdim(rec.x,xvals=rec.t*1e3,title='Waveform',xlab='Time (ms)',ylab='Amplitude'); ary1:=rec.x; include 'fftserver.g'; tool:=fftserver(); ary2:=tool.shift(ary1,1); include 'xcorwin.cpp.g'; pix:=5; ret:=xcorwin(ary1,ary2,pix=pix,off=0,shift=1/fc/4*sr); print ret.cor; plotdim(ret.cor,xvals=(1:len(ret.cor))-1-pix); print ret.scor; plotdim(ret.scor,xvals=(1:len(ret.scor))-1-pix); plotdim(sqrt(ret.cor^2+ret.scor^2)/sqrt(2),xvals=(1:len(ret.cor))-1-pix)
#----------------------------------------------------------------------
func xcorwin(ary1,ary2,freq=[],sr=[],pix=[],off=0,shift=0,progres=F) {fcor:=F
  func shft(ref ary,shift,fcor=F,freq=[],sr=[]) {
    include 'fftserver.g'
    if (fcor) {return ary*exp(1i*2*pi*freq*shift/sr)}
    else {return fftserver().shift(ary,shift)};
  } # end func shft
  if (len(ary1)!=len(ary2)|len(pix)==0) {return F};
  if (len(sr)!=0&&len(freq)!=0&&(is_complex(ary1)|is_dcomplex(ary1)))
    {fcor:=T; print '[xcorwin]: freq cor'};
  if (fcor) {ary1[1]:=0; ary2[1]:=0; ary1:=conj(ary1)}
  else {ary1-:=mean(ary1); ary2-:=mean(ary2)};
  if (off!=0) {ary2:=shft(ary2,-off,fcor,freq,sr)}; #(+)off=left
  ary2:=shft(ary2,-pix-1,fcor,freq,sr) #leftedge
  arylen:=pix*2+1; ret:=[=]
  if (shift!=0) {
    sary2:=shft(ary2,shift,fcor,freq,sr)
    ret.scor:=array(0.,arylen); shift:=T
  } else {shift:=F};
  ret.cor:=array(0.,arylen); old:=0
  for (lag in -pix:pix) {count:=lag+pix+1; prz:=count/arylen*100
    if (progres&&floor(prz/10)>=floor(old/10)+1)
      {old:=round(prz); print old,'% (',count,'/',arylen,')'};
    ary2:=shft(ary2,1,fcor,freq,sr)
    if (shift) {sary2:=shft(sary2,1,fcor,freq,sr)}; #one2right
    if (fcor) {norm:=sqrt(sum(abs(ary1)^2))*sqrt(sum(abs(ary2)^2))}
    else {norm:=sqrt(sum(ary1^2))*sqrt(sum(ary2^2))};
    ret.cor[lag+pix+1]:=sum(ary1*ary2)/norm
    if (shift) {ret.scor[lag+pix+1]:=sum(ary1*sary2)/norm};
  } # end lags
  return ret
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: xcor2avg
#----------------------------------------------------------------------
#Type: Function
#Doc:  calculates average cross correlation of two matrices
#      cross correlation of two matrices column- or row-wise (dim)
#      returns one array with the average of the cross correlations
#Par:  mat1,mat2 = - matrices two dimensional cross-correlation
#Par:  win       = - window left & right from xcor maximum for gaussfit
#Ref:  rest()
#Example: ary:=abs(data.get('f(x)',1)); ret:=xcor2avg(ary,fftserver().shift(ary,5),disp=T)
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 13.02.05 @ KUN
#----------------------------------------------------------------------
#ary1:=abs(data.get('f(x)',1)); ary2:=abs(data.get('f(x)',2)); ret:=xcor2avg(ary1,ary2,disp=T)
#----------------------------------------------------------------------
#ret:=read_value('ds_compare.sos')
#ret:=xcor2avg(ret.dynspec,ret.dynamicspectrum); ret.mxcor/:=max(ret.mxcor); plotdim(ret.mxcor)
#ret:=xcor2avg(ret.dynspec,ret.lopestools); ret.mxcor/:=max(ret.mxcor); plotdim(ret.mxcor,add=T)
#ret:=xcor2avg(ret.lopestools,ret.dynamicspectrum); ret.mxcor/:=max(ret.mxcor); plotdim(ret.mxcor,add=T)
#----------------------------------------------------------------------
func xcor2avg(mat1,mat2,dim=1,win=10,disp=F,ret=F) {

  # preparation #
  smat1:=shape(mat1); smat2:=shape(mat2)
  if (len(smat1)>0&&smat1!=smat2) {return F};
  include 'mathematics.g'
  tool:=fftserver()

  # matrices #
  if (len(smat1)==2)
  {
  
    # columns #
    if (dim==1)
    {
      columns:=smat1[1]
      mxcor:=array(0.,smat1[2])
      for (column in 1:columns) {mxcor+:=tool.crosscorr(mat1[column,],mat2[column,])}
      mxcor/:=columns
    };
  
    # rowx #
    if (dim==2)
    {
      rows:=smat1[2]
      mxcor:=array(0.,smat1[1])
      for (row in 1:rows) {mxcor+:=tool.crosscorr(mat1[,row],mat2[,row])}
      mxcor/:=rows
    }

  # arrays #
  } else {mxcor:=tool.crosscorr(mat1,mat2)}; # end if matrices

  # return arrays #
  xlen:=len(mxcor)
  xary:=[ceil(-xlen/2):(floor(xlen/2)-!as_boolean(rest(xlen,2)))] 
  maxv:=max(mxcor); maxp:=valpos(mxcor,maxv)
  res:=gaussfit(mxcor,xary=xary,n1=maxp-win,n2=maxp+win,center=maxp,height=maxv,width=1,maxiter=30)
  stddel:=floor(xlen/2)-maxp+1
  gaudel:=stddel-10+res.result.center[1]-1

  # disp #
  if (disp) {
    plotdim(mxcor,xvals=xary)
    plotdim(res.yary,xvals=res.xary,add=T)
    #plotdim(mxcor/max(mxcor),xvals=xary); plotdim(res.yary/max(res.yary),xvals=res.xary,add=T)
    print 'XCOR2D: delay between input arrays at correlation maximum [samples]:',stddel
    if (res.result.converged) {print 'XCOR2D: delay for maximum of gaussfit on correlation [samples]:',gaudel};
  };
  
  # return #
  if (ret) {
    cor:=[=]
    cor.mxcor:=mxcor       # mean xcor
    cor.xary:=xary         # xvals
    cor.stddel:=stddel     # delay between input arrays at correlation maximum
    cor.gaudel:=gaudel     # delay for maximum of gaussfit on correlation
    cor.result:=res.result # gaussfit results
    return cor
  };

}; # end funtion xcor2avg
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: xcor2win
#----------------------------------------------------------------------
#Type: Function
#Doc:  calculates 2d cross correlation in a window on two matrices
#Par:  mat1,mat2 = - matrices for two dimensional cross-correlation
#Par:  res       = - parameter for correlation window
#Ret:  xcor      = - matrix with normalized correlation coefficients
#Todo: not failsafe for wrong inputs
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 07.10.05 @ LESIA
#----------------------------------------------------------------------
#Example: include 'dynspectools.g'; rows:=8; cols:=10; res:=[1,2]; mat1:=array(0.,rows,cols); include 'mathematics.g'; rand:=randomnumbers(); for (row in 1:rows) {mat1[row,]:=rand.uniform(-1,1,10); rand.reseed()}; mat1:=round(mat1*10)/10; mat2:=mat1; include 'fftserver.g'; tool:=fftserver(); for (line in 1:cols) {mat2[,line]:=tool.shift(mat2[,line],-1)}; xcor:=xcor2win(mat1,mat2,res=res,off=[-1,0],blc=[3,3],trc=[rows-1,cols-2],disp=F); xcor
#yvals:=1:rows; xvals:=1:cols; fvals:=((1:shape(xcor)[1])-1)*mean(dif(yvals))-res[1]; tvals:=((1:shape(xcor)[2])-1)*mean(dif(xvals))-res[2]; peak:=valpos(xcor,max(xcor)); plotdim(xcor[,peak[2]],xvals=fvals,title='Cross-Correlation: Frequency',xlab='Frequency Lag',ylab='Correlation Coefficient'); plotdim(mean2d(xcor,1),xvals=fvals,add=T); plotdim(xcor[peak[1],],xvals=tvals,title='Cross-Correlation: Time',xlab='Time Lag',ylab='Correlation Coefficient'); plotdim(mean2d(xcor,2),xvals=tvals,add=T)
#----------------------------------------------------------------------
func xcor2win(mat1,mat2=-1,pix=[-1,-1],off=[0,0],xvals=-1,yvals=-1,
              blc=-1,trc=-1,bright=[-1,-1],dataset='',names=['',''],disp=F,ps=F,psdir='',
              verbose=T) {tic() #[time(s),freq(Hz)]
## verbose
ret:=[=]; ret.acor:=sum(mat2==-1)==1
if (verbose) {
print ''
printvar(xvals,id='[xcor2win] xvals [s]:')
printvar(yvals,id='[xcor2win] yvals [Hz]:')
print '[xcor2win] pix [s,Hz]:',pix
print '[xcor2win] off [s,hz]:',off
print '[xcor2win] blc [s,Hz]:',blc
print '[xcor2win] trc [s,Hz]:',trc
print '[xcor2win] acor (mat2 empty):',ret.acor
#print '[xcor2win] bright:',bright
#print '[xcor2win] disp:',disp
};
  ## input check & conversion
  include 'fftserver.g'; tool:=fftserver()
  if (blc==-1&&trc==-1) {return F};
  if (xvals!=-1&&yvals!=-1) { #!!!# consistency checks (see dstb_file.g load)
    blc[2]:=valpos(yvals,blc[2],big=T); blc[1]:=valpos(xvals,blc[1],big=F)
    trc[2]:=valpos(yvals,trc[2],big=T); trc[1]:=valpos(xvals,trc[1],big=F)
    ydif:=mean(dif(yvals)); xdif:=mean(dif(xvals));
    if (sum(off)!=0) {off[2]:=round(off[2]/ydif); off[1]:=round(off[1]/xdif)};
    if (sum(pix)!=-2) {pix[2]:=round(pix[2]/ydif); pix[1]:=round(pix[1]/xdif)};
  } else {return F};
  ## matix shaping
  if (mat2==-1&&trc+off+pix*2+1>shape(mat1)) {return F}; #acor
  if (mat2!=-1&&trc+off+pix*2+1>shape(mat2)) {return F}; #xcor
  ret.mat1w:=mat1[blc[1]:trc[1],blc[2]:trc[2]]
  if (sum(pix)==-2) {pix:=[floor(shape(ret.mat1w)/2)]};
# verbose #
if (verbose) {
print ''
#print '[xcor2win] window (mat1,mat2,blc,trc,off) [samples]:',shape(mat1),shape(mat2),blc,trc,off
#print '[xcor2win] steps [samples]:',pix
print '[xcor2win] pix, trc, blc, off, window size, matrix sizes:',
      pix,trc,blc,off,trc+off+pix*2+1,shape(mat1),shape(mat2)
print range(((blc[1]-pix[1]):(trc[1]+pix[1]))+off[1]),range(((blc[2]-pix[2]):(trc[2]+pix[2]))+off[2])
};
  if (mat2==-1) {bright[2]:=bright[1]; names[2]:=names[1]; ret.acor:=T
    mat2:=mat1[((blc[1]-pix[1]):(trc[1]+pix[1]))+off[1],((blc[2]-pix[2]):(trc[2]+pix[2]))+off[2]]
  } else {
    mat2:=mat2[((blc[1]-pix[1]):(trc[1]+pix[1]))+off[1],((blc[2]-pix[2]):(trc[2]+pix[2]))+off[2]]
  }; mat1:=[]
  ret.bandlim1:=[yvals[blc[2]],yvals[trc[2]]]
  ret.bandlim2:=[yvals[blc[2]+off[2]],yvals[trc[2]+off[2]]]
  ret.timvals1:=(blc[1]:trc[1])*xdif-xdif/2
  ret.timvals2:=((blc[1]:trc[1])+off[1])*xdif-(off[1]+1)*xdif-xdif/2
  #ret.timvals1:=xvals[blc[1]:trc[1]]; #ret.timvals2:=xvals[(blc[1]:trc[1])+off[1]]-xvals[off[1]+1]
  dim:=shape(mat2); ret.mat2:=mat2[(pix[1]+1):(dim[1]-pix[1]),(pix[2]+1):(dim[2]-pix[2])]
#print pix,dim,shape(mat2)
  ## displaying (ds)
  if (disp) {
    if (bright==-1) {bright[1]:=max(ret.mat1w); bright[2]:=max(ret.mat2)};
## verbose
if (verbose) {
print ''
printvar(ret.mat1w,id='[xcor2win] mat1w')
printvar(ret.mat2,id='[xcor2win] mat2')
printvar(ret.timvals1,id='[xcor2win] timvals1')
printvar(ret.timvals2,id='[xcor2win] timvals2')
print '[xcor2win] bandlim1:',ret.bandlim1
print '[xcor2win] bandlim2:',ret.bandlim2
print '[xcor2win] psname1:',spaste(dataset,'_dsxcor_ds',names[1])
print '[xcor2win] psname2:',spaste(dataset,'_dsxcor_ds',names[2])
print '[xcor2win] ps & psdir:',ps,psdir
};
    ## dynamic spectra
    print ''
    ret.ids:=spaste(dataset,'_dsxcor_ds',names[1])
    imgplot(ret.mat1w,name=names[1],bandlim=ret.bandlim1,
            timvals=ret.timvals1,timunit='s',dmax=bright[1],
            gif=ps,psdir=psdir,psname=ret.ids)
    print ''
    ret.nds:=spaste(dataset,'_dsxcor_ds',names[2])
    imgplot(ret.mat2,name=names[2],bandlim=ret.bandlim2,
            timvals=ret.timvals2,timunit='s',dmax=bright[2],
            gif=ps,psdir=psdir,psname=ret.nds)
    ## break (user input <return>)
    print ''; print '[xcor2win] PRESS ENTER / CTRL-C'; shell('echo; read') # break for ds check (return=continue & strg-c=stop)
  }; # endif disp
## verbose
if (verbose) {
print '[xcor2win] mat shapes:',shape(ret.mat1w),shape(mat2)
};
  ## cross correlation
  print '[xcor2win] shifts:',pix*2+1
  wrows:=shape(ret.mat1w)[1]; wcols:=shape(ret.mat1w)[2]
  ret.mat1w-:=mean(ret.mat1w)
  ret.xcor:=array(0.,pix[1]*2+1,pix[2]*2+1)
  for (row in -pix[1]:pix[1]) {
    print 'row:',row+pix[1]+1,'/',pix[1]*2+1 # startup rowshift
    for (col in -pix[2]:pix[2]) {
      mat2w:=mat2[(pix[1]+row+1):(wrows+pix[1]+row),(pix[2]+col+1):(wcols+pix[2]+col)]
      mat2w-:=mean(mat2w)
      ret.xcor[row+pix[1]+1,col+pix[2]+1]:=sum(ret.mat1w*mat2w)/(sqrt(sum(ret.mat1w^2))*sqrt(sum(mat2w^2)))
    } # end cols
  } # end rows
  ## time consumption
  print ''; toc(id='[xcor2win] time:')
  ## return
  ret.pix:=pix
  ret.off:=off
  ret.names:=names
  return ret
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: xcor2ful
#----------------------------------------------------------------------
#Type: Function
#Doc:  calculates two dimensional cross correlation
#Par:  mat1,mat2 = - matrices for two dimensional cross-correlation
#Par:  pix       = - parameter for correlation window
#Ret:  ret       = - matrix with normalized correlation coefficients
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 05.10.05 @ LESIA
#----------------------------------------------------------------------
#Example: include 'dynspectools.g'; rows:=8; cols:=10; mat1:=array(0.,rows,cols); include 'mathematics.g'; rand:=randomnumbers(); for (row in 1:rows) {mat1[row,]:=rand.uniform(-1,1,10); rand.reseed()}; mat2:=mat1; include 'fftserver.g'; tool:=fftserver(); for (line in 1:cols) {mat2[,line]:=tool.shift(mat2[,line],-1)}; xcor:=xcor2ful(mat1,mat2,pix=[1,2],off=[-1,0]); xcor
#----------------------------------------------------------------------
func xcor2ful(mat1,mat2,pix=[-1,-1],off=[0,0]) {
  include 'fftserver.g'; tool:=fftserver()
  if (shape(mat1)!=shape(mat2)) {return F};
  if (sum(off)!=0) {
    for (srow in 1:shape(mat2)[1]) {mat2[srow,]:=tool.shift(mat2[srow,],-off[2])} #left
    for (scol in 1:shape(mat2)[2]) {mat2[,scol]:=tool.shift(mat2[,scol],-off[1])} #up
  };
  rows:=shape(mat1)[1]; cols:=shape(mat2)[2]
  mean1:=mean(mat1); mean2:=mean(mat2)
  mat1-:=mean1; mat2-:=mean2
  for (srow in 1:rows) {mat2[srow,]:=tool.shift(mat2[srow,],-pix[1]-1)} #leftedge
  for (scol in 1:cols) {mat2[,scol]:=tool.shift(mat2[,scol],-pix[2]-1+(pix[2]*2+1))} #(top)bottom
  ret:=array(0.,pix[2]*2+1,pix[1]*2+1)
#row:=0; col:=0
  for (row in -pix[1]:pix[1]) {
    for (srow in 1:rows) {mat2[srow,]:=tool.shift(mat2[srow,],1)} #right
    for (scol in 1:cols) {mat2[,scol]:=tool.shift(mat2[,scol],-(pix[2]*2+1))} #resetop
    for (col in -pix[2]:pix[2]) {
      for (scol in 1:cols) {mat2[,scol]:=tool.shift(mat2[,scol],1)} #down
      ret[col+pix[2]+1,row+pix[1]+1]:=sum(mat1*mat2)/(sqrt(sum(mat1^2))*sqrt(sum(mat2^2)))
#print row,col,round(ret*10)/10
    } # end cols
  } # end rows 
  return ret
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: runavgstd
#----------------------------------------------------------------------
#Type: function
#Doc:  
#Par:  ary    - data to be averaged
#      avg    - array to store the blocks average
#      std    - array to store the blocks standard deviation
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 19.03.05 @ Kleve
#----------------------------------------------------------------------
func runavgstd(ary,ref avg,ref std,bs=10,bd=1) {

  # variables #
  arylen:=len(ary)
  sel:=seq(1,arylen,bd)
  val avg:=array(0.,len(sel))
  val std:=array(.0,len(sel))

} #end function runavgstd
#######################################################################
   
#######################################################################
#----------------------------------------------------------------------
#Name: blockint
#----------------------------------------------------------------------
#Type: function
#Doc:  reduces array length by averaging blocks
#Par:  ary - data to be averaged
#      bs  - block size over which to average
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 21.12.06 @ RU
#----------------------------------------------------------------------
func blockint(ary,bs) {

  # variables #
  arylen:=len(ary)
  fblockn:=floor(arylen/bs)
  blockn:=ceil(arylen/bs)
  outary:=array(0,blockn)
  
  # calculation #
  for (block in 1:fblockn) {outary[block]:=mean(ary[((block-1)*bs+1):(block*bs)]);}
  if (fblockn<blockn) {outary[blockn]:=mean(ary[((blockn-1)*bs+1):arylen])};
  
  # return #
  return outary;

} #end function blockaint
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: blockavgstd
#----------------------------------------------------------------------
#Type: function
#Doc:  calculates average and standard deviation for a number of blocks
#      from an array and returns two arrays with the same size
#Par:  ary    - data to be averaged
#      avg    - array to store the blocks average
#      std    - array to store the blocks standard deviation
#      blocks - block size over which to average
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 10.09.04 @ ASTRON
#----------------------------------------------------------------------
func blockavgstd(ary,ref avg,ref std,blocks) {

  # variables #
  arylen:=len(ary)
  val avg:=array(0.,arylen)
  val std:=array(.0,arylen)

  # blocksize #
  bs:=floor(arylen/blocks)
  blocksize[2:blocks-1]:=bs
  blocksize[1]:=bs+floor((arylen-bs*blocks)/2)
  blocksize[blocks]:=bs+ceil((arylen-bs*blocks)/2)
#print 'blocksize:',blocksize

  # calculation #
  for (block in 1:blocks) {
    ende:=sum(blocksize[1:block])
    start:=ende-blocksize[block]+1
    avg[start:ende]:=mean(ary[start:ende]); val avg
    std[start:ende]:=stddev(ary[start:ende]); val std
  }

} #end function blockavgstd
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: blockavgstd2
#----------------------------------------------------------------------
#Type: function
#Doc:  calculates average and standard deviation for a number of blocks
#      of equal size from an array
#Par:  ary    - data to be averaged
#      avg    - array to store the blocks average
#      std    - array to store the blocks standard deviation
#      blocks - block size over which to average
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 10.09.04 @ ASTRON
#----------------------------------------------------------------------
func blockavgstd2(ary,ref avg,ref std,blocks) {

  # variables #
  val avg:=array(0.,blocks)
  val std:=array(.0,blocks)

  # parameters #
  arylen:=len(ary)
#print arylen
  if (blocks>1) {blocks:=2*floor(blocks/2)};
  if (arylen>1) {arylen:=2*floor(arylen/2)};
  blocksize:=arylen/blocks
#print 'blocksize:',blocksize

  # calculation #
  for (block in 1:blocks) {
    start:=(block-1)*blocksize+1
    ende:=block*blocksize
#print start,ende,mean(ary[start:ende]),stddev(ary[start:ende])
    val avg[block]:=mean(ary[start:ende])
    val std[block]:=stddev(ary[start:ende])
  }

} #end function blockavgstd
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: runmin
#----------------------------------------------------------------------
#Type: function
#Doc:  returns the running minimum of an ary for a defined block size
#Par:  ary
#Ret:  outary
#Example: tic(); ary:=runmin([1,2,3,4,5],3); toc()
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 13.05.04 @ KUN
#----------------------------------------------------------------------
runmin:=function(ary,bs=10)
{
  if (bs<len(ary)&&bs>1)
  {
    arylen:=len(ary)
    left:=floor((bs-1)/2); right:=ceil((bs-1)/2) #even bs: right=left+1
    for (i in 1:left) {outary[i]:=min(ary[1:(i+right)])}; #first
    for (i in (arylen-right+1):(arylen)) {outary[i]:=min(ary[(i-right):arylen])}; #last (right for symmetry!?)
    hlp:=[0,ary[1:(bs-1)]]
    for (i in (left+1):(arylen-right))
    {hlp:=[hlp[2:bs],ary[i+right]]; outary[i]:=min(hlp)} #main2.228
    ##for (i in (left+1):(arylen-right)) {outary[i]:=min(ary[(i-left):(i+right)])}; #main2.280
  } else {outary:=ary};
  return outary;
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: run2sum
#----------------------------------------------------------------------
#Type: function
#Doc:  returns the running difference between two ary for a defined block size
#Par:  ary
#Example: rundiff([1,2,3,4,5],[5,4,3,2,1],a,b,2)
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 13.05.04 @ KUN
#----------------------------------------------------------------------
func rundiff(ary1,ary2,ref sqsum,ref sumsq,bs=10)
{
  arylen:=len(ary1);
  if (len(ary2)==arylen&&bs<arylen&&bs>1)
  {
    mary:=array(0,arylen+bs-1);
    #SQSUM
    for (i in 1:bs)
      {mary[i]:=sum((ary1[1:i]-ary2[1:i])^2)};
    for (i in (bs+1):(arylen))
      {mary[i]:=sum((ary1[(i-bs):i]-ary2[(i-bs):i])^2)};
    for (i in (arylen+1):(len(mary)))
      {mary[i]:=sum((ary1[(i-bs+1):arylen]-ary2[(i-bs+1):arylen])^2)};
    val sqsum:=mary[(1+bs/2):(arylen+bs/2)];
    #SUMSQ
    for (i in 1:bs)
      {mary[i]:=sum(ary1[1:i]-ary2[1:i])^2};
    for (i in (bs+1):(arylen))
      {mary[i]:=sum(ary1[(i-bs):i]-ary2[(i-bs):i])^2};
    for (i in (arylen+1):(len(mary)))
      {mary[i]:=sum(ary1[(i-bs+1):arylen]-ary2[(i-bs+1):arylen])^2};
    val sumsq:=mary[(1+bs/2):(arylen+bs/2)];
  } else {sqsum:=array(0,arylen); sumsq:=sqsum};
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: str2ary
#----------------------------------------------------------------------
#Type: function
#Doc:  conversion of string containing an ary to a float or integer ary
#Par:  string
#Ret:  ary
#Example: str2ary('[12.234234,234234.232,324.4235234,23.23234343434]')
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 28.07.03 @ MPIfR
#----------------------------------------------------------------------
str2ary:=function(string='',sep=',')
{
  chars:=split(string,'');
  splitlen:=len(chars);
  if (string=='[]') {return []};
  count:=0; aryent:=0; numcount:=0; tmp:=''
  stringary:=[=] # record of strings
  flag:=F; # will be an integer array...
  while (count<splitlen-1) # for number of characters in chars
  { count+:=1;
    if (count>1) { if (!(chars[count-1]=='.')) {numcount:=0; tmp:=''}; }; # zeroset for next array entry
    if ((!(chars[count]=='['))&(!(chars[count]==sep))&(!(chars[count]=='.'))&(!(chars[count]==']'))) # if character is an integer
    { aryent+:=1; # increase array index
      numcount+:=1; # increase array entry index
      tmp[numcount]:=chars[count]; # save as first entry of new string-array for the first array entry
      while ((!(chars[count+1]=='['))&(!(chars[count+1]==sep))&(!(chars[count+1]=='.'))&(!(chars[count+1]==']')))
      { count+:=1;
        numcount+:=1; # also next character for this array entry
        tmp[numcount]:=chars[count]; # save next character(s) in array-entry
      }
      stringary[aryent]:=spaste(tmp);
    }; # end character is integer
    if (chars[count+1]=='.')
    { flag:=T; # will be a float array
      count+:=1;
      aryent-:=1; # decrease array index to continue current value
      numcount+:=1; # next char for arry entry
      tmp[numcount]:=chars[count];
    };
  } # end while chars length
  if (flag) { for (i in 1:len(stringary)) { ary[i]:=as_float(stringary[i]) } } # assembling of a floatvalue-array
  else { for (i in 1:len(stringary)) { ary[i]:=as_integer(stringary[i]) } } # assembling of an integervalue-array
  return ary;
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: radec2azel
#----------------------------------------------------------------------
#Type: function
#Doc: conversion of RA and DEC to AZEL
#Example: sdir:=radec2azel(date='',obs='',raj='5h20m30.2',decj='-30d15m12.5')
#         sdir:=radec2azel(date='2004/06/22/15:37:34.000',obs='',raj='08:14:59.50',decj='+74:29:5.69')
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 01.01.05 @ KUN
#----------------------------------------------------------------------
func radec2azel(date='',obs='',raj=0,decj=0) {

  # inclusion #
  include 'measures.g'

  # time # 
  example:='2003/10/28/11:05:49.161'
  utclen:=len(split(example,''))
  utc:=split(date,'');
#print example,spaste(utc),utclen,len(utc)
  if (date==''|len(utc)!=utclen)
  {epoch:=dm.epoch('utc','today'); print 'RADEC2AZEL: date set to NOW'}
  else {epoch:=dm.epoch('utc',spaste(utc[1:utclen]))};
  
  # observatory #
  if (obs=='') {obs:='LOFAR-ITS'}

  # calculation #
  dm.doframe(epoch);
  dm.doframe(dm.observatory(obs));
  angles:=dm.measure(dm.direction('J2000',raj,decj),'azel');
  azmrad:=angles.m0.value;
  elvrad:=angles.m1.value;
  sdir.xs:=cos(elvrad)*cos(azmrad);
  sdir.ys:=cos(elvrad)*sin(azmrad);
  sdir.zs:=sin(elvrad);
  sdir.azm:=azmrad/pi*180;
  sdir.elv:=elvrad/pi*180;

  # return #
  return sdir;
}
#######################################################################

#######################################################################
#-----------------------------------------------------------------------
#Name: res
#-----------------------------------------------------------------------
#Type: Function
#Doc: calculates the resolution of a telescope in degrees
#     on observing frequency and maximum baseline
#Par: freq - observing center frequency
#Par: base - maximum baseline
#Example: res(185.95197,26.5e6)
#-----------------------------------------------------------------------
func res(base=0,freq=0) {
  c:=299792458
  return (c/freq)/base*180/pi
}
#######################################################################

#######################################################################
#-----------------------------------------------------------------------
#Name: error
#-----------------------------------------------------------------------
#Type: Function
#Doc: calculates relative (and absolute) error
#Example:
#-----------------------------------------------------------------------
#Created: by Andreas Nigl @ 08.08.06 @ RUN
#-----------------------------------------------------------------------
func error(ref ary,name='',disp=F,ret=F) {
  res:=[=]
  res.nval:=len(ary)
  res.mean:=mean(ary)
  res.stddev:=stddev(ary)
  res.abserr:=res.stddev/sqrt(res.nval)
  res.relerr:=(res.stddev/sqrt(res.nval))/res.mean
  if (disp) {print spaste(name,' (mean,abserr,relerr):'),res.mean,res.abserr,res.relerr};
  if (ret) {return res};
  if (!disp) {return res.relerr};
}
#######################################################################

#######################################################################
#-----------------------------------------------------------------------
#Name: sigsel
#-----------------------------------------------------------------------
#Type: Function
#Doc: returns selection of an array for values within a sigma level
#Example:
#-----------------------------------------------------------------------
#Created: by Andreas Nigl @ 09.08.06 @ RUN
#-----------------------------------------------------------------------
func sigsel(ary,sigmas=3,iter=50,disp=F)  {
  if (disp) {oary:=ary; xvals:=1:len(oary); plotdim(oary,xvals=xvals)};
  sel:=array(T,len(ary))
  for (i in 1:iter) {
    ary[!sel]:=mean(ary[sel])
    below:=ary<(mean(ary)+sigmas*stddev(ary))
    above:=ary>(mean(ary)-sigmas*stddev(ary))
    sel:=sel&(above&below)
    if (disp) {plotdim(oary[sel],xvals=xvals[sel],add=T)};
  }
  return sel
} # end sigsel
#######################################################################


#######################################################################
#Section: <a href="tricks">TRICKS</a> #################################
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: stdev
#----------------------------------------------------------------------
#Type: functions
#Doc: calculates standard deviation with STDDEV
#     in addition to STDDEV it checks the input array length
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 03.08.06 @ RUN
#----------------------------------------------------------------------
func stdev(ary) {if (len(ary)>1) {return stddev(ary)} else {return F};};
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: tic & toc
#----------------------------------------------------------------------
#Type: functions
#Doc: functions for time registration
#Par: tic() = - start of time registration
#Par: toc() = - stop of time registration and disp the elapsed time
#               in seconds
#Ref: time() = - Returns the system time in seconds since 00:00
#                Universial Coordinated Time, January 1, 1970.
#                The value returned is of type double.
#Example: tic(); input_event(data,'example.event'); toc()
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 13.03.05 @ KUN
#----------------------------------------------------------------------
func tic() {global startimevalue:=time()}
func toc(id='',ret=F) {
  stoptimevalue:=time()-startimevalue
  if (ret) {return stoptimevalue}
  else {print id,stoptimevalue,'s'};
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: xflag
#----------------------------------------------------------------------
#Type: function
#Doc: flags yvalues for regions on the xaxis to a certain value
#Par: ydata   = - yaxis data
#Par: xdata   = - xaxis data
#Par: flaglim = - multidimensional array (x by 2)
#Par: flagval = - target yvalues of flag regions: ['zero','mean']
#Ret: string with selected chars
#Example: ydata:=data.get('FFT'); xdata:=data.get('Frequency'); flaglim:=array(.0,2,2); flaglim[1,]:=[49.97,50.03]*1e6; flaglim[2,]:=[62.16,62.22]*1e6; xflag(ydata,xdata,flaglim)
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 13.09.04 @ KUN
#----------------------------------------------------------------------
func xflag(ref ydata,xdata,flaglim,flagval='mean') {
  if (flagval=='zero') {flagv:=0};
  if (flagval=='mean') {flagv:=mean(ydata)};
  for (flagn in 1:shape(flaglim)[1]) {
    flagsel:=xdata>flaglim[flagn,1]&xdata<flaglim[flagn,2]
    ydata[flagsel]:=flagv; val ydata
  }
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: charsel
#----------------------------------------------------------------------
#Type: function
#Doc: extraction of specified chars out of a string
#Par: string   = - input string
#Par: charnums = - numbers of chars to extract
#Ret: string with selected chars
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 28.07.03 @ MPIfR
#----------------------------------------------------------------------
charsel:=function(string,charnums)
{
  charext:=function(string,charnum)
  {
    char:=split(string,'')[charnum]
    return char;
  }
  for (i in charnums) { chars[i]:=charext(string,i)  }
  return spaste(chars);
}
#######################################################################

#######################################################################
#Name: flip2d
#Type: function
#Doc: flips 2D matrix
#----------------------------------------------------------------------
flip2d:=function(matrix,dim)
{
  for (dimc in 1:shape(matrix)[dim])
  {
    if (dim==1) {matrix[dimc,]:=matrix[dimc,shape(matrix)[2]:1]};
    if (dim==2) {matrix[,dimc]:=matrix[shape(matrix)[1]:1,dimc]};
  }
  return matrix;
}
#######################################################################

#######################################################################
#Name: matary2d
#Type: function
#Doc: substraction of ary from matrix
#----------------------------------------------------------------------
matary2d:=function(matrix,ary,dim)
{
  if (shape(matrix)[dim]==len(ary)) {matrix:=F} else
  {
    for (dimc in 1:shape(matrix)[dim])
    {
    print dimc;
      if (dim==1) {matrix[dimc,]:=matrix[dimc,]-ary};
      if (dim==2) {matrix[,dimc]:=matrix[,dimc]-ary};
    }
  }
  return matrix;
}
#######################################################################

#######################################################################
#Name: mean2d  average of matrix column or row
#Type: function
#Doc: average of matrix column or row
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 03.03.06 @ NS
#----------------------------------------------------------------------
mean2d:=function(ref matrix,dim=1) {
  include 'mathematics.g'
  mat:=F;
  if (len(shape(matrix))==2) {
  for (dimc in 1:shape(matrix)[dim]) {
    if (dim==1) {mat[dimc]:=mean(matrix[dimc,])};
    if (dim==2) {mat[dimc]:=mean(matrix[,dimc])};
  }; } else {mat:=mean(matrix)}; return mat;
} # end mean2d
#######################################################################

#######################################################################
#Name:    stdv2d  standard deviation of matrix column or row
#Type:    function
#Doc:     standard deviation of matrix column or row 
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 03.03.06 @ NS
#----------------------------------------------------------------------
func stdv2d(matrix,dim=1) {
  include 'mathematics.g'
  mat:=F
  if (len(shape(matrix))==2) {
  for (dimc in 1:shape(matrix)[dim]) {
    if (dim==1) {mat[dimc]:=stddev(matrix[dimc,])};
    if (dim==2) {mat[dimc]:=stddev(matrix[,dimc])};
  }; } else {mat:=stddev(matrix)}; return mat;
} # end stdv2d
#######################################################################

#######################################################################
#Name: transpose2d
#Type: function
#Doc: calculates transpose of matrix
#----------------------------------------------------------------------
transpose2d:=function(matrix)
{
  mat:=array(.0,shape(matrix)[2],shape(matrix)[1]);
  for (row in 1:shape(matrix)[1])
  {mat[,row]:=matrix[row,]}
  return mat;
}
#######################################################################

#######################################################################
#Name: funclog
#Type: function
#Doc: creates log file and adds open number of items to it
#     (loging of function parameter)
#Example: funclog('test.log','hallo',1,T); shell('kwrite test.log &')
#----------------------------------------------------------------------
func funclog(filename='',title='', ...) {
  textfile:=open(spaste('>',filename));
  write(textfile,title);
  for (strnum in 1:num_args(...))
  {write(textfile,nth_arg(strnum,...))}; # add saving of item name
}
#######################################################################

#######################################################################
#Name: maxpos
#Type: function
#Doc: returns position of maximum in ary
#----------------------------------------------------------------------
func maxpos(ary) {return order(ary)[len(ary)]};
#  maximum:=max(ary)
#  numary:=1:len(ary)
#  posary:=ary==maximum
#  pos:=numary[posary]
#  return pos
#######################################################################

#######################################################################
#Name: minpos
#Type: function
#Doc: returns position of minimum in ary
#----------------------------------------------------------------------
func minpos(ary) {return order(ary)[1]};
#######################################################################

#######################################################################
#Name: valpos
#Type: function
#Par: big=[]  exact
#     big=-1  closest
#     big=T   next bigger
#     big=F   next smaller
#Doc: returns position of value in ary
#----------------------------------------------------------------------
func valpos(ary,value,big=[]) {
  numary:=ind(ary) # equal to 1:len(ary)
  valen:=len(value)
  if (len(shape(ary))==2) {pos:=array(0,valen,2)} else {pos:=array(0,valen)};
  for (vnum in 1:valen) {
    if (is_nan(value[vnum])) {posary:=is_nan(ary)} # finding NAN
    else {
      posary:=numary[ary==value[vnum]] # searches for exact value
      if (len(posary)==0&&big==-1) { # returns closest value
        posary:=ary==value[vnum]
        bigger:=min(numary[ary>=value[vnum]])
        smaller:=max(numary[ary<=value[vnum]])
#print bigger,smaller
        if (smaller<0) {smaller:=1}; if (bigger>len(ary)) {bigger:=len(ary)};
        if ((ary[bigger]-value[vnum])>(value[vnum]-ary[smaller])) {posary:=smaller} else {posary:=bigger};
      }
      if (len(big)!=0&&big==T) {posary:=min(numary[ary>=value[vnum]])}; # returns next bigger
      if (len(big)!=0&&big==F) {posary:=max(numary[ary<=value[vnum]])}; # returns next smaller
    };
#print posary
    if (len(posary)>0) {
      if (len(posary)>1) {posary:=posary[1]};
      if (len(shape(ary))==2) { # converts position to matrix coordinates
        row:=posary%shape(ary)[1]
        if (row==0) {row:=shape(ary)[1]};
        pos[vnum,]:=[row,(1:shape(ary)[2])[ary[row,]==max(ary)]]
      } else {pos[vnum]:=numary[posary]};
    } else {pos:=[]};
  }; # endfor number of positions
  return pos
}
#######################################################################
###for (col in 1:shape(ary)[1]) {if (max(ary[col,])==max(ary)) {peakpos[1]:=col};}
###for (row in 1:shape(ary)[2]) {if (max(ary[,row])==max(ary)) {peakpos[2]:=row};}

#######################################################################
#Name: valext
#Doc: extracts one value from input array by position
#Ref: valpos()
#Example: ary:=[1,2,5,3,4]; value:=5; pos:=valpos(ary,value); ary:=valext(ary,pos)
#----------------------------------------------------------------------
func valext(ary,pos) {
  arylen:=len(ary)
  return ary[(1:arylen)[(1:arylen)!=pos]]
}
#!extract all entries with a special value
#value:=ary[valpos(ary,value)]
#ary:=ary[ary!=value]
#######################################################################

#######################################################################
#Name: hist
#Type: function
#Doc: returns record including array with different numbers
#     and array with their frequency
#     NEW: now it can also fit a gauss on the histogram distribution [gauss=T]
#Example: hist([1,2,3,4,4,5,6,8,9,9,10,14,15,16],bins=3,disp=T,sym=32)
#Example: include 'mathematics.g'; rand:=randomnumbers(); ary:=rand.normal(0,1,100000); rand.reseed(); hist(ary,bins=100,gaus=T,disp=T,progres=F) #ary:=rand.uniform(-1,1,100000);
#----------------------------------------------------------------------
#mode: a.val[valpos(a.vlc,max(a.vlc))] # häufigster wert
#----------------------------------------------------------------------
func hist(ref ary,bins=-1,title='',xlab='values',ylab='counts',sym=1,gaus=T,disp=T,progres=T,ret=F) {
#ary:=[1,2,3,4,4,5,6,8,9,9,10,14,15,16]; bins:=3; disp:=F
  res:=[=]; all:=F
  if (bins==-1) {bins:=len(unique(ary)); all:=T}
  else {binstep:=(dif(range(ary)))/bins; hlp:=ary-min(ary)};
  if (bins>1000) {print 'HIST: now wait for the huge number of bins:',bins};
  res.bins:=bins
  bin:=1
  old:=0
  for (bin in 1:bins) {
    prz:=bin/bins*100
    if (progres&&floor(prz/10)>=floor(old/10)+1)
    {old:=round(prz); print old,'% (',bin,'/',bins,')'};
    if (all) {
      res.val[bin]:=uni[bin]
      res.vlc[bin]:=len(ary[ary==uni[bin]])
    } else {bine:=as_string(bin)
      res.bin[bine]:=ary[hlp>(bin-1)*binstep&hlp<bin*binstep]
#print (bin-1)*binstep,bin*binstep
      res.val[bin]:=bin*binstep-binstep/2
      res.vlc[bin]:=len(res.bin[bine])
    };
  }
  if (max(res.vlc)==1) {disp:=F; print 'HIST: all values are unique -> select bin-size !!!'};
  if (gaus) {
    guess.height:=max(res.vlc)
    guess.center:=res.val[valpos(res.vlc,guess.height)]
    guess.width:=stdev(ary)*3
    guess.maxiter:=30
    include 'fitting.g'; gaussfitter:=gauss1dfitter()
    gaussfitter.setstate(guess)
    gauss:=gaussfitter.fit(res.val,res.vlc)
    gauss.fit:=gaussian1d(res.val,gauss.height,gauss.center,gauss.width)
  };  
  if (disp) {
    #pgplotter().hist(ary,datmin=min(ary)-min(dif(unique(ary)))/2,datmax=max(ary)+
    #min(dif(unique(ary)))/2,nbin=(max(ary)-min(ary))/min(dif(unique(ary))),pgflag=F)
    plotdh:=plotdim(res.vlc,xvals=res.val,title=spaste('HISTOGRAM: ',title),
                    xlab=xlab,ylab=ylab,sym=sym)
    if (gaus) {plotdim(gauss.fit,xvals=res.val,add=T)};
  };
  if (ret) {return res};
}
#######################################################################

#######################################################################
#Name: int & dif
#Type: functions
#Doc: integrates and differentiates inputarray
#----------------------------------------------------------------------
func int(ary) {for (i in 1:len(ary)) {ret[i]:=sum(ary[1:i])} return ret}
func dif(ary) {for (i in 1:(len(ary)-1)) {ret[i]:=ary[i+1]-ary[i]} return ret}
#######################################################################

#######################################################################
#Name: rest
#Type: function
#Par: num = - numverator
#Par: den = - denominator
#Doc: calculates rest for division
#Example: ary1:=abs(data.get('f(x)',1)); ary2:=abs(data.get('f(x)',2)); yary:=fftserver().crosscorr(ary1,ary2); alen:=len(ary1); xary:=[ceil(-alen/2):(floor(alen/2)-!as_boolean(rest(alen,2)))]; plotdim(yary,xvals=xary)
#----------------------------------------------------------------------
func rest(num,den) {return (num/den-as_integer(num/den))*den}
#######################################################################
#MODULO: two numbers a & b are congruent if they have the same modul m,
#        which is a divisor leaving the same rest for both numbers
#        3 & 8 (mod 5), 3:5=0 Rest3 & 8:5=1 Rest3

#######################################################################
#Name: doc
#Type: function
#Doc: opens browser with aips++ documentation
#----------------------------------------------------------------------
doc:=function() {shell('konqueror http://aips2.nrao.edu/docs/search/search.html &'); return};
#######################################################################

#######################################################################
#Name: printvar
#Type: function
#Doc: displays any variable
#Example: printvar(data)
#Alternative: showGlishObject(data)
#----------------------------------------------------------------------
func printvar(var,id='PRINTVAR:') {
  if (is_record(var)) {
    for (f in field_names(var)) {ok:=F
      if (is_string(var[f])|is_boolean(var[f])) {ok:=T
        if (len(var[f])<2) {print id,f,var[f]}
        else {print id,f,shape(var[f])};}; #'<string or boolean>'};
      if (is_function(var[f])) {print id,f,'<function>'; ok:=T};
      if (is_record(var[f])) {print id,f,'<record>'; ok:=T} #; ,printvar(var[f]); ok:=T};
      if (is_numeric(var[f])&&!is_boolean(var[f])) {ok:=T
        if (len(var[f])<2) {print id,f,var[f]}
        else {print id,f,shape(var[f]),range(var[f]),range(dif(var[f]))};}; #,'<numeric>'};};
      if (!ok) {print id,f,type_name(var[f])}; #'<unknown>'};
    } # end of fields
  } else {ok:=F
    if (is_string(var)|is_boolean(var)) {ok:=T
      if (len(var)<2) {print id,var}
      else {print id,shape(var)};}; #'<string or boolean>'};
    if (is_function(var)) {print id,'<function>'; ok:=T};
    if (is_record(var)) {print id,'<record>'; ok:=T} #; ,printvar(var); ok:=T};
    if (is_numeric(var)|is_boolean(var)) {ok:=T
      if (len(var)<2) {print id,var}
      else {print id,shape(var),range(var),range(dif(var))};}; #,'<numeric>'};};
    if (!ok) {print id,type_name(var)}; #'<unknown>'};
  }; # endif record
} # end of function
#----------------------------------------------------------------------
#func printvar(var) {
#  if (is_record(var)) {ary:='field_names(var)'; tst:='var[f]'}
#  else {ary:=''; tst:='var'};
#  for (f in eval(ary)) {ok:=F
#    if (is_string(eval(tst))|is_boolean(eval(tst))) {print 'PRINTVAR:',f,eval(tst); ok:=T}; #'<string or boolean>'};
#    if (is_function(eval(tst))) {print 'PRINTVAR:',f,'<function>'; ok:=T};
#    if (is_record(eval(tst))) {print 'PRINTVAR:',f,'<record>'; ok:=T} #; ,printvar(eval(tst)); ok:=T};
#    if (is_numeric(eval(tst))&&!is_boolean(eval(tst))) {
#      if (len(eval(tst))==1) {print 'PRINTVAR:',f,eval(tst)}
#      else {print 'PRINTVAR:',f,shape(eval(tst)),range(eval(tst)),range(dif(eval(tst)))};}; #,'<numeric>'};};
#    if (!ok) {print 'PRINTVAR:',f,type_name(eval(tst))}; #'<unknown>'};
#  } # end of fields
#} # end of function
#######################################################################
#for (f in field_names(rec)) {rs:=spaste('rec.',f); data:=''; if (len(eval(rs))<=100) {data:=eval(rs)}; print f,shape(eval(rs)),data}

#######################################################################
#Name: sam2dist
#Type: function
#Doc: displays & returns light travel distance for a given number of samples
#----------------------------------------------------------------------
func sam2dist(samples) {
  global samplerate, lightspeed
  timx:=samples*1/samplerate; print 'Time [ns]:',time*10^9
  dist:=time*lightspeed
print 'Dist [m]:',dist
  return dist
}
#######################################################################

#######################################################################
#Name: rnd
#Type: function
#Doc: rounds input array to a number of digitis
#Example: rnd(pi,dig=2)
#----------------------------------------------------------------------
func rnd(x,dig=2) {return round(x*10^dig)/10^dig}
#######################################################################


#######################################################################
#Section: STORAGE #####################################################
#######################################################################

#######################################################################
#Name: TIM40readybeamget
#----------------------------------------------------------------------
TIM40readybeamget:=function(field,ref rec,index=1,inv=F, reffield='',start=-1,end=-1)
{return read_value("TIM40readybeam.sos")};
#######################################################################

#######################################################################
func datamon(ref rec,loc='',ants=-1) {
  if (ants==-1) {ants:=1:rec.len};
  if (loc!='') {loc:=spaste(loc,': ')};
  for (ant in ants) {
    print spaste('[',ant,'] ',loc,'FFT len=',len(rec.get('FFT',ant)),
                 ', PhaseGrad len=',len(rec.get('PhaseGrad',ant)),
                 ', Blocknum=',rec.getmeta('Blocknum',ant),
                 ', Blocksize=',rec.getmeta('Blocksize',ant),
                 ', FFTSize=',rec.getmeta('FFTSize',ant),
                 ', FFTlen=',rec.getmeta('FFTlen',ant),
                 ', Offset=',rec.getmeta('Offset',ant),
                 ', ZeroOffset=',rec.getmeta('ZeroOffset',ant),
                 ', FileBlocksize=',rec.getmeta('FileBlocksize',ant),
                 ', Filesize=',rec.getmeta('Filesize',ant)
                 )
  }
}
#######################################################################

#######################################################################
#Name: evdates
#Doc: extracts events date information from event files
#Example: evdats:=evdates('040501')
#----------------------------------------------------------------------
func evdates(date='',evdir=spaste(datadir,'LOPES/')) {
  date:=split(date,'')
  evfiles:=shell(spaste('cd ',evdir,'20',spaste(date[1:2]),'/',spaste(date[3:4]),'/',spaste(date[5:6]),'; ls'))
  evdats:=str2lim(evfiles,lim=[1:23])
  return evdats
}
#######################################################################
#Name: str2lim
#Doc: extracts subsets of chararcters and replaces desired from/in
#     an array of strings
#Example: evdat:=str2lim('2003.08.01.00:00:13.162.event',lim=[1:23])
#----------------------------------------------------------------------
func str2lim(strs,lim=[],pos=[],cha=[]) {
  for (snum in 1:len(strs)) { #print 'STR2LIM:',snum
    str:=split(strs[snum],'')
    strlen:=len(str)
    #if (strlen!=0) {
      if (lim[1]<1) {lim[1]:=1}; if (lim[2]<lim[1]) {lim[2]:=len(str)};
      if (shape(lim)!=0&&strlen>=lim[1]&&strlen>=lim[2]) {str:=str[lim]}
      else {print '-',snum,spaste(str)};
      #print spaste('STR2LIM: num ',snum,', str ',spaste(str),', len ',strlen,', lim ',lim[1],':',lim[len(lim)])
      if (shape(pos)!=0) {for (cn in 1:len(cha)) {str[pos[cn]]:=cha[cn]}};
    #}; #strlen
    strs[snum]:=spaste(str)
  } #for strings
  return strs
} #funcend
#######################################################################

#######################################################################
#!!! NON-LINEAR FUNCTIONS DO NOT WORK !!!
#f:=[=]; f.fct:='p0*sin(x/p1-p2)+p3'; f.par:=[4,10,0,0]; fixed:=[F,F,F,F,F]
#f:=[=]; f.fct:='p0*sin(x/p1-0)+0'; f.par:=[1,1]; fixed:=[F]
#ret:=xfit(yary=2*sin((1:100)/10-3)+5,xary=1:100,fct=f.fct,par=f.par,fixed=fixed,disp=T)
#######################################################################
#-----------------------------------------------------------------------
#Name: xfit
#-----------------------------------------------------------------------
#Type: function
#Doc: fit input function to data
#Ret: res  - Record with the fit results
#Par: freq - Array with frequency (x-axis) values
#Par: pwr  - Array with spectral power (y-axis) values
#Par: err  - Array with y-errors, ignore if err=[]
#-----------------------------------------------------------------------
#Example: [1]
#include 'dynspectools.g'; fct:='p0*x^2+p1*x+p2'; x:=1:100; par:=[1,0,0]; for (pn in 1:len(par)) {eval(spaste('p',pn-1,':=par[',pn,']'))}; p2:=1000; yary:=eval(fct); fixed:=array(F,len(par)); ret:=xfit(yary=yary,xary=x,fct=fct,par=par,fixed=fixed,disp=T); plotdim(yary,xvals=x); plotdim(ret.f(x),xvals=x,add=T)
#-----------------------------------------------------------------------
#Example: [2]
#include 'dynspectools.g';  fct:='p0*sin(2*pi*p1*(x/25)^p2-p3)+p4'; freq:=((0:9999)/10000*6+25); par:=[2,40,4,pi,10]; for (pn in 1:len(par)) {eval(spaste('p',pn-1,':=par[',pn,']'))}; p0:=2; x:=freq; pwr:=eval(fct); fixed:=[F,F,F,F,F]; ret:=xfit(yary=pwr,xary=freq,fct=fct,par=par,fixed=fixed,disp=T); plotdim(pwr,xvals=freq); plotdim(ret.f(freq),xvals=freq,add=T)
#-----------------------------------------------------------------------
#Example: [3]
#linclude('dynspectools.g'); x:=1:100; y:=10*x+3; plotdim(y,xvals=x,sym=1); f:=[=]; f.fct:='p0*x+p1'; f.par:=[1,0]; for (pn in 1:len(f.par)) {eval(spaste('p',pn-1,':=f.par[',pn,']'))}; fixed:=array(F,len(f.par)); ret:=xfit(yary=y,xary=x,fct=f.fct,par=f.par,fixed=fixed,disp=T); plotdim(ret.f(x),xvals=x,add=T,sym=-1); plotdim(y*(x*ret.par[1]),xvals=x,sym=1,add=T)
#-----------------------------------------------------------------------
func xfit(yary=[],xary=[],fct=[],par=[],err=[],fixed=[],disp=F){
  
  # check (!!! consistent length of parameter in function and input variable par !!!)
  if (len(xary)==0) {xary:=1:len(yary)};
  if (len(xary)!=len(yary)) {return F};

  # modules
  include 'functionals.g'
  include 'fitting.g'
  
  # tools
  ff:=functionfitter()
  fc:=functionals()

  # function
  if (is_record(fct)) {cfct:=fct} else {cfct:=fc.compiled(fct,par)};
  if (len(cfct)==0) {print 'linear'; cfct:='p0*x+p1'; par:=[1,0]; fixed:=array(F,len(par))};
  if (len(par)==0) {par:=cfct.parameters()};
  if (len(fixed)!=len(par)) {fixed:=array(F,len(par))};
  ff.setfunction(cfct)

  # parameter
  ff.setparameters(par)

  # data
  if (len(err)==0) {ff.setdata(xary,yary)} else {ff.setdata(xary,yary,err)};

  # fit (result)
  ret:=[=]
  ret.par:=ff.fit(linear=F,fixed=fixed)
  ret.sigma:=stddev(ff.getresidual())

  # errors fit
  if (len(err)==0) {
     ff.setdata(xary,yary,array(ret.sigma,len(xary)));
     ret.par:=ff.fit(linear=F)
  };

  # return fct
  cfct.setparameters(ret.par)

  # statistics
  ret.parerr:=ff.geterror();
  ret.chisq:=ff.getchisq();
  ret.chisqred:=ret.chisq/(len(xary)-len(par))

  # print
  if (disp) {
    print 'inp par  =',par
    print 'fit par  =',ret.par
    print 'par err  =',ret.parerr
    print 'sigma    =',ret.sigma
    print 'chisq    =',ret.chisq
    print 'chisqred =',ret.chisqred
  };

  # disp
  if (disp) {ff.plot()};

  # return
  ret.f:=cfct.f; ret.ff:=ff  
  return ret
}
#######################################################################

#######################################################################
#-----------------------------------------------------------------------
#Name: gaussfit
#-----------------------------------------------------------------------
#Type: function
#Doc: fits a gauss to an plots it with the input data
#Ret: record with the fit results
#Par: xn1           - start of fit region
#Par: xn1           - end of fit region
#Par: nfit = 1      - number of Gaussian lines that were fitted, only used if nfit=1 for initialization
#Par: height = '-1' - starting peak height for the fit, if height='-1' use maximum
#Par: center = '-1' - starting peak center for the fit (array position), if center='-1' use location of maximum
#Par: width = 4     - starting peak width for the fit
#Par: maxiter = 30  - max number of iterations for the fit
#Example:
#1#input_event(data,'example.event'); #input_event(data,'/data/LORUN/eventdata/candidates/2005.03.08.23:48:03.673.event'); n1:=data.getxn(-32e-6,'Time'); n2:=data.getxn(-30e-6,'Time'); yary:=read_value('lorun_example_timepwr_filtered.sos'); xary:=data.get('Time',1); ret:=gaussfit(yary,xary=xary,n1=n1,n2=n2,nfit=1,center='-1',height='-1',width=1,maxiter=30)
#plotdim(yary,xvals=xary); plotdim(ret.yary,xvals=ret.xary,add=T)
#2#n1:=1; n2:=-1; ary1:=abs(data.get('f(x)',1)); ary2:=abs(data.get('f(x)',2)); yary:=fftserver().crosscorr(ary1,ary2); alen:=len(ary1); xary:=[ceil(-alen/2):(floor(alen/2)-!as_boolean(rest(alen,2)))]; ret:=gaussfit(yary,xary=xary,n1=n1,n2=n2,nfit=1,center='-1',height='-1',width=1,maxiter=30,disp=T)
#plotdim(yary/max(yary),xvals=xary); plotdim(ret.yary/max(ret.yary),xvals=ret.xary,add=T)
#delay:=-(alen/2-ret.result.center[1])
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 13.03.05 @ KUN
#-----------------------------------------------------------------------
gaussfit:=function(yary,xary=[],n1=1,n2=-1,nfit=1,height='-1',center='-1',width=1,maxiter=30,disp=F) {

  # input
  ylen:=len(yary); xlen:=len(xary)
  if (n2==-1) {n2:=ylen};
  yary:=yary[n1:n2]
  if (xlen==0) {xary:=1:ylen; xlen:=ylen};

  # fit
  guess:=[=]
  if (!is_string(height)) {guess.height:=height}
                     else {guess.height:=max(yary)};
  if (!is_string(center)) {guess.center:=center-n1+1}
                     else {guess.center:=valpos(yary,guess.height)};
  guess.width:=width
  guess.maxiter:=maxiter
  include 'fitting.g'; gaussfitter:=gauss1dfitter()
  gaussfitter.setstate(guess)
  result:=[=]
  result:=gaussfitter.fit(ind(yary),yary);

  # solutions
  for (soln in ind(result.center)) {
    if (result.center[soln]>xlen-1||result.center[soln]<2||is_nan(result.center[soln])) {
    # no peak at / beyond edges of input array
      result.center[soln]:=as_integer((n2-n1+1)/2) # set to center (left) of input array ??? +1 ???
      result.height[soln]:=0; result.width[soln]:=1
    }
  } # end for solutions
 
  # maxwidth
  maxwidth:=max(result.width)
  maxwidthpos:=valpos(result.width,maxwidth)
  maxcenter:=result.center[maxwidthpos];
  if (maxwidth>n2-n1+1) {maxwidth:=(n2-n1+1); maxcenter:=as_integer((n2-n1)/2+1)}; # found peak width larger than input array and set to center (right) of input array ??? +1 ???

  # xvalues ???
  #xwidth:=xary[floor(result.center)+1]-xary[floor(result.center)]
  #result.xwidth:=result.width*xwidth
  #result.xcenter:=xary[floor(result.center)]+(result.center-floor(result.center))*xwidth

  # plotting
  start:=min(as_integer(maxcenter-7*maxwidth),1)
  end:=max(as_integer(maxcenter+7*maxwidth),n2-n1+1)
  if (start+n1-1<ylen) {start:=2-n1};
  if (end+n1-1>ylen) {end:=ylen-n1+1};
  fit:=[=]; if (nfit==1) {fit.yary:=array(0.,ylen)}; #??? recursiv ???
  for (soln in ind(result.center)) {
    fit.yary[(start+n1-1):(end+n1-1)]+:=
    gaussian1d([start:end],result.height[soln],result.center[soln],result.width[soln])
  }

  # disp
  if (disp) {print 'GAUSSFIT: fit.result: ',result};

  # return
  fit.xary:=xary
  fit.result:=result
  return fit #result
}
#######################################################################

#######################################################################
#-----------------------------------------------------------------------
#Name: sinfit
#-----------------------------------------------------------------------
#Type: Function
#Doc: Fit a sinus function to an array of values 
#Ret: res  - Record with the fit results
#Par: freq - Array with frequency (x-axis) values
#Par: pwr  - Array with spectral power (y-axis) values
#Par: err  - Array with y-errors, ignore if err=[]
#-----------------------------------------------------------------------
# p0-amplitude p1-freqfactor p2-pwrlaw p3-phaseoffset p4-amplitudeoffset
#-----------------------------------------------------------------------
#Example: freq:=((0:9999)/10000*6+25); p0:=2; p1:=10; p2:=4; p3:=pi; p4:=10; pwr:=p0*sin(2*pi*p1*(freq/25)^p2-p3)+p4; plotdim(pwr,xvals=freq)
#include 'dynspectools.g'; par:=[2,50,4,pi,10]; fixed:=[F,F,F,F,F]; fct:='p0*sin(2*pi*p1*(x/25)^p2-p3)+p4'; res:=sinfit(pwr=pwr,freq=freq,par=par,fixed=fixed); plotdim(pwr,xvals=freq); plotdim(res.f(freq),xvals=freq,add=T)
#-----------------------------------------------------------------------
func sinfit(pwr=[],freq=[],fct='',par=[],err=[],fixed=[],disp=F){
  
  # module
  include 'functionals.g'
  include 'fitting.g'
  
  # tool
  ff:=functionfitter()
  fc:=functionals()

  # function
  #fct:='p0*sin(2*pi*p1*x^p2-p3)+p4' #fct:='p0*sin(2*pi*p1*x^p2-p3)+p4'
  cfct:=fc.compiled(fct,par) #cfct:=fc.functional('compile',fct) #functionals().compiled('p0*x^p1',[1,1])
  ff.setfunction(cfct) # p0-amplitude p1-pwrlaw p2-phaseoffset x-freq
  
  # parameter
  #par:=[.1,2e-14,2,0,.2]
  ff.setparameters(par)
  fixed:=array(F,len(par))

  # data
  if (len(err)==0) {ff.setdata(freq,pwr)} else {ff.setdata(freq,pwr,err)};

  # fit (result)
  res:=[=]
  res.par:=ff.fit(linear=F,fixed=fixed)
  res.sigma:=stddev(ff.getresidual())

  # errors fit
  if (len(err)==0) {
     ff.setdata(freq,pwr,array(res.sigma,len(freq)));
     res.par:=ff.fit(linear=F)
  };
  cfct.setparameters(res.par)

  # statistics
  res.parerr:=ff.geterror();
  res.chisq:=ff.getchisq();
  res.chisqred:=res.chisq/(len(freq)-len(par))

  # print
  print 'par      =',res.par
  print 'par err  =',res.parerr
  print 'error    =',res.sigma
  print 'chisq    =',res.chisq
  print 'chisqred =',res.chisqred

  # disp
  if (disp) {ff.plot()};

  # return
  res.f:=cfct.f
  return res
}
#######################################################################

#######################################################################
#lc:=lightcurve(pixels,period=24,add=F)
func lightcurve(pixels,period=24,add=F) { # integrated power of spectra with absolute block minimum
  for (row in 1:shape(pixels)[1]) {lc[row]:=mean(runminc(abs(pixels[row,]),bs=10))}
  timeax:=(1:shape(pixels)[1])*period/shape(pixels)[1] #[h]
  plotdim(lc,xvals=timeax,ylab='reduced Power',title='Light Curve',add=add)
  return lc
}
#######################################################################

#######################################################################
#as:=avgspec(pixels,bandlim=[40,80],add=F)
func avgspec(pixels,bandlim=[40,80],add=F) { # average spectrum
  as:=mean2d(abs(pixels),dim=2)
  freqax:=((1:shape(pixels)[2])/shape(pixels)[2]*dif(bandlim)+bandlim[1])*1e6 #[Hz]
  plotdim(log(abs(as)),xvals=freqax/1e6,xlab='Frequency (MHz)',ylab='absFFT',title='Average Spectrum',add=add)
  return as
}
#######################################################################

#######################################################################
func save(variable,filename) {
  if (is_string(variable)) {print 'SAVE:',filename,variable}
  else {if (is_record(variable)) {print 'SAVE:',filename,'<record>'} #; printvar(variable)}
  else {print 'SAVE:',filename,shape(variable),range(variable),range(dif(variable))};};
  write_value(variable,filename)
}
#######################################################################

#######################################################################
func load(filename) {
  if (fexist(filename)) {
    variable:=read_value(filename)
    if (is_string(variable)) {print 'LOAD:',filename,variable}
    else {if (is_record(variable)) {print 'LOAD:',filename,'<record>'} #; printvar(variable)}
    else {print 'LOAD:',filename,shape(variable),range(variable),range(dif(variable))};};
  } else {print 'LOAD: file does not exist'; variable:=F};
  return variable
}
#######################################################################

#######################################################################
func man(name='aips') {
  print 'MAN: aips glish system (available manuals / parameter for function)'
  if (name=='aips') {shell('acroread Doc/manuals/aips.prog.pdf &')};
  if (name=='glish') {shell('kghostview Doc/manuals/glish.ps &')};
  if (name=='system') {shell('acroread Doc/manuals/aips.sys.pdf &')};
}
#######################################################################

## READKEY ##
func readkey() {
  print 'Press any key!'
  shell('echo; read')
  #print '..............'
}

func incl(file) {include spaste(globalconst.LOPESCODE,'/oldGlish/',file)}
