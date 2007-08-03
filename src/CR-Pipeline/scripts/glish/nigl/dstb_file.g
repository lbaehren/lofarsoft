#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#Name: dstb
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#Load: include 'dstb_file.g'; res:=dstb(act=F,nods=F,name='lopes',bandlim=[40,80]*1e6,newlim=[40,80]*1e6,timelim=[0,-1],sav=F,sosdir='sos/',disp=T,ps=T,bright=10000,ret=T,done=F);
#-------------------------------------------------------------------------------
#Avgspectrum: include 'dstb_file.g'; resE:=dstb(name='jupX3nda',act=T,nods=T,datatype='NDABeam',loc='/data/NDA/jupX3/',ants=1,blockinfo=[1,5,2^20],offset=0,bandlim=[0,40]*1e6,sav=F,sosdir='jsos/',disp=T,bright=0,ps=T,ret=T,done=F)
#Plot: plotdim(log(resEold.avgspec),xvals=resEold.avgspec/1e6,title='Average Power Spectrum',xlab='Frequency (MHz)',ylab='Power (dB)')
#-------------------------------------------------------------------------------
#Project: include 'dstb_file.g'; res:=dstb(name='project',act=T,nods=F,datatype='NDABeam',loc='/data/NDA/jupX3/',ants=1,blockinfo=[1,5,2^15],intblockn=1,offset=0,bandlim=[21,24]*1e6,sav=F,sosdir='',disp=T,bright=0,ps=T,ret=T,done=F) #4e4
#-------------------------------------------------------------------------------
#Event: include 'dstb_file.g'; res:=dstb(ref data,act=T,nods=F,name='event',datatype='LopesEvent',loc='',head='*.hdr',ants=1,blockinfo=[1,1,2^8],offset=0,bandlim=[40,80]*1e6,newlim=[43,76]*1e6,timelim=[0,-1],sav=F,sosdir='',disp=T,ps=F,bright=0,ret=T,done=F) #10000 #/data/LOPES/2006/07/24/
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#Eventfiletest(1File&1Block): ant:=2; bs:=2^15; input_event(data,'example.event',index=-1); include 'dstb_file.g'; res:=dstb(ref data,act=T,nods=F,name='event',datatype='LopesEvent',loc='test/',ants=ant,blockinfo=[1,1,bs],offset=0,azel=[0,90],bandlim=[40,80]*1e6,timelim=[0,-1],sav=F,sosdir='',disp=F,ps=F,bright=0,ret=T,done=F); data.setblock(blocksize=bs,index=ant); data.unflag('Time',index=ant); data.unflag('FFT',index=ant); dummy:=data.get('Dummy',ant); dummy[1:5]; fftserver().complexfft(dummy,1); dummy[1:5]; data.get('FFT',ant)[1:5]*2048; plotdim(log(res.avgspec/max(res.avgspec)),xvals=res.freq/1e6); plotdim(log(abs(data.get('FFT',ant))/max(abs(data.get('FFT',ant)))),xvals=data.get('Frequency',ant)/1e6,add=T); plotdim(log(abs(fft(data.get('Voltage',ant),dir=1,nyquist=2))/max(abs(fft(data.get('Voltage',ant),dir=1,nyquist=2)))),xvals=data.get('Frequency',ant)/1e6,add=T)
#-------------------------------------------------------------------------------
#Eventfiletest(XBlocks): bs:=2^16; input_event(data,'example.event'); include 'dstb_file.g'; res:=dstb(ref data,act=T,nods=F,name='event',datatype='LopesEvent',loc='',ants=1,blockinfo=[1,-1,bs],offset=0,azel=[0,90],bandlim=[40,80]*1e6,timelim=[0,-1],sav=F,sosdir='',disp=F,ps=F,bright=0,ret=T,done=F); data.unflag('Time'); data.unflag('FFT'); data.setblock(n=data.getmeta('Filesize')/bs-1,blocksize=bs); for (blockn in 1:2) {print data.get('Dummy')[1:5]; print data.get('FFT')[1:5]*2048; if (data.getmeta('Filesize')>=(data.getmeta('Blocknum')+blockn)*bs) {data.nextblock()};}
#-------------------------------------------------------------------------------
#Eventfiletest(XBlocks&BF): ants:=[1,2,3]; bs:=2^15; input_event(data,'example.event',index=-1); include 'dstb_file.g'; res:=dstb(ref data,act=T,nods=F,name='event',datatype='LopesEvent',loc='test/',ants=ants,blockinfo=[1,-1,bs],offset=0,azel=[0,90],bandlim=[40,80]*1e6,timelim=[0,-1],sav=F,sosdir='',disp=F,ps=F,bright=0,ret=T,done=F); data.unflag('Time',index=-1); data.unflag('FFT',index=-1); data.setblock(n=data.getmeta('Filesize')/bs-1,blocksize=bs,index=-1); for (blockn in 1:2) {beam:=0; for (ant in ants) {print "dummy: ",data.get('Dummy',ant)[1:5]; dummy:=fft(data.get('Dummy',ant),dir=1,nyquist=2)/2^(12-ln(bs)/ln(2))*2048; print "fft: ",dummy[1:5]; beam+:=dummy}; print "beam: ",beam[1:5]; if (data.getmeta('Filesize')>=(data.getmeta('Blocknum')+blockn)*bs) {data.nextblock()};}; plotdim(log(res.avgspec/max(res.avgspec)),xvals=res.freq/1e6); plotdim(log(abs(beam)/max(abs(beam))),xvals=data.get('Frequency',ant)/1e6,add=T);
#1/range(abs(fft(data.get('Dummy',1),dir=1,nyquist=2))/2^(12-ln(bs)/ln(2))/abs(data.get('FFT',1)))
#plotdim(log(fft(data.get('Dummy',ant),dir=1,nyquist=2)/2^(12-ln(bs)/ln(2))*2048)); plotdim(log(data.get('FFT',ant)*2048),add=T)
#-------------------------------------------------------------------------------
#Eventfiletest(XFiles&XBlocks)['/raid/pv1/2006/12/04/' OR 'eventfiles/']: loc:='/raid/pv1/2006/12/04/'; bs:=2^15; include 'dstb_file.g'; res:=dstb(act=T,nods=F,name='event',datatype='LopesEvent',loc=loc,ants=1,blockinfo=[1,-1,bs],fileinfo=[1,2],offset=0,azel=[0,90],bandlim=[40,80]*1e6,timelim=[0,-1],sav=F,sosdir='',disp=T,ps=F,bright=0,ret=T,done=F); filenames:=filesel(dir=loc,ext='*.event',first=1,last=-1,disp=F); for (filen in 2) {input_event(data,filenames[filen],index=-1); data.unflag('Time'); data.unflag('FFT'); data.setblock(n=data.getmeta('Filesize')/bs-1,blocksize=bs); for (blockn in 1:2) {print data.get('Dummy')[1:5]; print data.get('FFT')[1:5]; if (data.getmeta('Filesize')>=(data.getmeta('Blocknum')+blockn)*bs) {data.nextblock()};}}
#-------------------------------------------------------------------------------
#Eventfiletest(XFiles&XBlocks&XAnts): bs:=2^15; loc:='/raid/pv1/2006/12/04/'; include 'dstb_file.g'; res:=dstb(act=T,nods=F,name='event',datatype='LopesEvent',loc=loc,ants=[1,2,3],blockinfo=[1,-1,bs],fileinfo=[1,2],offset=0,azel=[0,90],bandlim=[40,80]*1e6,timelim=[0,-1],sav=F,sosdir='',disp=T,ps=F,bright=0,ret=T,done=F); filenames:=filesel(dir=loc,ext='*.event',first=1,last=-1,disp=F); for (filen in 2) {input_event(data,filenames[filen],index=-1); data.unflag('Time'); data.unflag('FFT'); data.setblock(n=data.getmeta('Filesize')/bs-1,blocksize=bs); for (blockn in 1:2) {beam:=0; for (ant in [3,2,1]) {print data.get('Dummy',ant)[1:5]; dummy:=data.get('FFT',ant); print dummy[1:5]; beam+:=dummy}; print beam[1:5]; if (data.getmeta('Filesize')>=(data.getmeta('Blocknum')+blockn)*bs) {data.nextblock()};}}
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#Projectfiletest: bs:=2^15; input_project(data,'/data/NDA/jupX3/'); include 'dstb_file.g'; res:=dstb(name='nda',act=T,nods=F,datatype='NDABeam',loc='/data/NDA/jupX3/',ants=1,blockinfo=[1,5,bs],intblockn=1,offset=0,bandlim=[0,40]*1e6,sav=F,sosdir='',disp=T,bright=0,ps=T,ret=T,done=F); data.setblock(blocksize=bs); data.setphase(); data.unflag('Time'); data.unflag('FFT'); dummy:=data.get('Dummy'); print dummy[1:5]; fftserver().complexfft(dummy,1); print dummy[1:5]; data.get('CalFFT')[1:5]; plotdim(log(res.avgspec/max(res.avgspec)),xvals=res.freq/1e6); plotdim(log(abs(data.get('CalFFT'))/max(abs(data.get('CalFFT')))),xvals=data.get('Frequency')/1e6,add=T); plotdim(log(abs(fft(data.get('Voltage'),dir=1,nyquist=1))/max(abs(fft(data.get('Voltage'),dir=1,nyquist=1)))),xvals=data.get('Frequency')/1e6,add=T)
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#NDA/ITS:
#bs:=2^16; include 'dstb_file.g'; res:=dstb(act=T,nods=F,name='NDA',datatype='NDABeam',loc='/data/NDA/jupX3/',ants=1,blockinfo=[1,12000,bs],intblockn=8,offset=0,direction='',azel=[0,90],bandlim=[20,25]*1e6,sav=T,sosdir='sos/',disp=T,ps=T,bright=4e4,ret=T,done=F);
#bs:=2^16; include 'dstb_file.g'; res:=dstb(act=T,nods=F,name='ITS',datatype='ITSCapture',loc='/data/ITS/jupX3/',ants=-1,blockinfo=[1,-1,bs],intblockn=8,offset=0,direction='',azel=[148.381411,19.2168745],bandlim=[20,25]*1e6,newlim=[21,23]*1e6,timelim=[0,0.4],sav=F,sosdir='sos/',disp=T,ps=T,bright=0,ret=T,done=F); #!!!Xaxis&2ndHalf&AvgSpec
#-------------------------------------------------------------------------------
#LORUN: #bs:=2^16; include 'dstb_file.g'; res:=dstb(act=T,nods=F,name='lorun_a2',datatype='LopesEvent',loc='/data/LORUN/eventdata/test/061214/',ants=3,blockinfo=[1,-1,bs],intblockn=7,fileinfo=[1,7000],offset=0,direction='',azel=[0,90],bandlim=[40,80]*1e6,newlim=[40,80]*1e6,timelim=[1,3],sav=F,sosdir='sos/',disp=T,ps=T,bright=0,ret=T,done=F);
#-------------------------------------------------------------------------------
#LOPES: bs:=2^12; include 'dstb_file.g'; res:=dstb(act=T,nods=F,name='lopes_061214',datatype='LopesEvent',loc='/raid/pv1/2006/12/18/',ants=-1,blockinfo=[1,-1,bs],intblockn=2^5,fileinfo=[1,-1],offset=0,direction='',azel=[0,90],bandlim=[40,80]*1e6,sav=T,sosdir='sos/',disp=T,ps=T,bright=10000,ret=T,done=F);
#DS (/avgspec OR /PZbackgndsubtraction)
#C++-Prog (take everything in and start aips from code, produce pic)
#Html-Script (starts C++-Prog and makes page and pics for LOPES page)
#??? Aips & Glishbus OR imgplot... from c++, c++ comands - SVEN
#-------------------------------------------------------------------------------
func dstb(ref rec=F,act=T,nods=F,name='',datatype='',loc='',head='*.hdr',
          method=1,ants=-1,direction='',azel=[0,90],
          blockinfo=[1,1,2^8],fileinfo=[1,1],offset=0,intblockn=1,
          bandlim=[0,40]*1e6,newlim=[-1,-1],timelim=[0,-1],
          alpha=1,sav=F,sosdir='',disp=F,psdir='ps/',ps=F,bright=0,ret=F,done=F)
{tic();
  ## test
  #loc:='/data/ITS/jup6/'
  #ants:=1
  #blockinfo:=[1,1,2^16]
  #offset:=0
  #bandlim:=[-1,-1]
  
  ## datatypes
  #datatype=='LopesEvent'
  #datatype=='ITSCapture'
  #datatype=='ITSBeam'
  #datatype=='NDABeam'

  ## hallo
print ''
print '[dstb_file.g] DSTB - Dynamic Spectrum on Time(Beam)series'
  
  ## act
  if (!act) {lod:=T};
  if (act) {lod:=F

  ## load event filename(s)
  if (datatype=='LopesEvent') {
    filenames:=filesel(dir=loc,ext='.event',first=fileinfo[1],last=fileinfo[2],disp=F)
    if (filenames=='') {print '[dstb_files.g] no files !!!'; return F};
  };
  ## load data
  if (is_record(rec)) {wider rec # external record
  } else { # generate record
    rec:=[=]
    if (datatype=='LopesEvent') { # event file
print '[dstb_file.g] loading event file:',filenames[1]
      input_event(rec,filenames[1],index=-1)
    } else { # project data
      filenames:=loc
print '[dstb_file.g] loading project data:',loc
      input_project(rec,dir=loc,head=head,ants=-1,blocksize=blockinfo[3])
    }; # end loading
  }; # end load data record
print '[dstb_file.g] datatype:',datatype
  dataformat:=rec.head('DataFormat')
print '[dstb_file.g] dataformat:',dataformat

  ## client & datafileformat
  global cli:=client(spaste(globalconst.LOPESBIND,'/dstb'))

  ## antennas
print '[dstb_file.g] fixing antenna selection...'
print '[dstb_file.g] - ants:',ants
print '[dstb_file.g] - rec.selant:',rec.selant()
  if (ants==-1) {ants:=rec.selant()}; # else {ants:=rec.selant()[ants]};
  asel:=array(F,len(rec.ant())); asel[ants]:=T
print '[dstb_file.g] - asel:',asel
print '[dstb_file.g] - ants:',ants

  ## blockinfo
print '[dstb_file.g] fixing blockinfo...'
  filesize:=rec.getmeta('Filesize')
  if (blockinfo[3]>filesize) {
    print '[timebeam.g] !!! Blocksize too big:',blockinfo[3],'!!!'
    return F
  };
  max_nblocks:=floor(filesize/(blockinfo[3]-2*maxdel))
  if (blockinfo[2]==-1) {nblocks:=max_nblocks}
  else {if (blockinfo[2]>max_nblocks) {nblocks:=max_nblocks} else {nblocks:=blockinfo[2]};};
print '[dstb_file.g] - blockinfo [os,fb,nb,bs]:',[offset,blockinfo[1],nblocks,blockinfo[3]]
  rec.setblock(n=blockinfo[1],blocksize=blockinfo[3],offset=offset,index=-1)
  #SetBlock(rec,n=blockinfo[1],blocksize=blockinfo[3],offset=offset,index=-1)
  rec.setphase()

  ## fileinfo
print '[dstb_file.g] fixing fileinfo...'
print '[dstb_file.g] - number of files:',len(filenames)
print '[dstb_file.g] - number of files selected:',fileinfo[2]
  if (fileinfo[2]==-1) {fileinfo[2]:=len(filenames)};
  intfilen:=ceil(intblockn/nblocks)
  fileinfo[2]:=floor(fileinfo[2]/intfilen)*intfilen # right number of blocks for integration
  if (fileinfo[2]==0) {print '[dstb_file.g] Too many blocks for integration (reduce blocksize or load more files) !!!'; return F};
print '[dstb_file.g] - number of files to be loaded:',fileinfo[2]
  
  ## bandlim
print '[dstb_file.g] extracting frequency band limits...'
  if (bandlim[1]==-1||bandlim[2]==-1||bandlim==[-1,-1]) {
    frequnit:=rec.headf('FrequencyUnit')
    freqmin:=rec.headf('FrequencyLow')*frequnit
    freqband:=rec.headf('Bandwidth')*frequnit
    freqmax:=freqmin+freqband
    bandlim[1]:=freqmin
    bandlim[2]:=freqmax
  };
print '[dstb_file.g] - bandlim:',bandlim

  ## frequencies
  freq:=rec.get('Frequency')
  freqsel:=array(F,len(freq))
  freqsel[valpos(freq,bandlim[1],big=T):valpos(freq,bandlim[2],big=F)]:=T
  freq:=freq[valpos(freq,bandlim[1],big=T):valpos(freq,bandlim[2],big=F)]
print '[dstb_file.g] - frequencies:',len(freq),range(freq)
print '[dstb_file.g] - frequencies selection:',len(freqsel),range(freqsel)

  ## azel
  if (method!=1) {
    date:=split(rec.head('Date'),''); date[12:13]:=['1','2']
    date[15:16]:='0'; date[18:19]:='0'; date[21:23]:='0'
    if (direction!='') {azel:=sourcedir(rec=rec,obs='',direction=direction,date=date,disp=T).azel};
    if (direction=='zenith') {azel:=[0,90]};
  } else {azel:=[0,0]; print '[dstb_file.g] method: average antennae power'}; 

  ## steps
  if (datatype=='LopesEvent') {
    print '';
    step:=fileinfos(files=filenames,events=T,
      blockinfo=blockinfo,intblockn=intblockn,
      frac=F,rel=F,sep=F,disp=F,ps=F);
    timunit:='h'
  } else {
    samplerate:=as_integer(rec.head('Samplerate'))*eval(rec.head('SamplerateUnit'))
    step:=(0:(nblocks/intblockn-1))*blockinfo[3]*intblockn/samplerate+
      blockinfo[3]*intblockn/samplerate/2
    timunit:='s'
  };
printvar(id='[dstb_file.g] step:',var=step)
#print 'PRESS ENTER / CTRL-C'; shell('echo; read')
#return step
  
  ## record
print '[dstb_file.g] building client record...'
  clirec:=[=]
  clirec.nods             := nods
  clirec.datatype         := datatype
  clirec.method           := method
  clirec.filenames        := filenames
  clirec.blocksize        := blockinfo[3]
  clirec.nblocks          := nblocks
  clirec.startblock       := blockinfo[1]
  clirec.offset           := offset #[bytes] http://www.astron.nl/~bahren/coding/lopestools/ !!!
  clirec.intblockn        := intblockn
  clirec.antennas         := rec.ant()-1
  clirec.antsel           := ants-1
  clirec.antpos           := getAntennaPositions(rec,refant=1,index=ants)
  clirec.freq             := freq
  clirec.freqsel          := freqsel
  clirec.azel             := azel
  clirec.alpha            := alpha
  clirec.fft2calfft       := array(1+0i,rec.getmeta('FFTlen'),len(ants)) #delete
  clirec.adc2voltage      := array(1,len(ants)) #delete
#  clirec.selectedChannels := freqsel #delete
#  clirec.selectedAntennas := ants-1 # correction for c++ counting #delete
#  clirec.antennaSelection := asel #delete

  ## execute
print '[dstb_file.g] execute client...'
print ''
  printvar(id='[dstb_file.g] clirec:',var=clirec)
  clires:=cli->dstb(clirec)
  clirec:=[=]
  clires.absds:=as_float(clires.absds)
  #printvar(id='[dstb_file.g] clires:',var=clires)

  ## act
  }; # endif act

  ## load
  # loading does not work for big dynamic spectra
  # because of conversion from complex to double with abs()
  if (lod) {
    print ''
    print '[dstb_file.g] loading sos-files...'
    loadname:=spaste(name,'_',round(bandlim[1]/1e6*10)/10,'t',round(bandlim[2]/1e6*10)/10)
    pixname:=spaste(sosdir,'dstb_',loadname,'.pix.sos')
    axsname:=spaste(sosdir,'dstb_',loadname,'.axs.sos')
    absname:=spaste(sosdir,'dstb_',loadname,'.abs.sos')
    step:=read_value(axsname).step; freq:=read_value(axsname).freq
    if (timelim[1]==0|timelim[1]<min(step)|timelim[1]>timelim[2]) {timelim[1]:=min(step)}
    if (timelim[2]==-1|timelim[2]>max(step)|timelim[2]<timelim[1]) {timelim[2]:=max(step)}
    if (newlim[1]==-1|newlim[1]<min(freq)|newlim[1]>newlim[2]) {bandlim[1]:=min(freq)}
    else {bandlim[1]:=newlim[1]};
    if (newlim[2]==-1|newlim[2]>max(freq)|newlim[2]<newlim[1]) {bandlim[2]:=max(freq)}
    else {bandlim[2]:=newlim[2]};
printvar(id='[dstb_file.g] step:',var=step)
printvar(id='[dstb_file.g] freq:',var=freq)
print '[dstb_file.g] timelim & bandlim:',timelim,bandlim
    tsel:=valpos(step,timelim[1],big=T):valpos(step,timelim[2],big=F); step:=step[tsel]
    fsel:=valpos(freq,bandlim[1],big=T):valpos(freq,bandlim[2],big=F); freq:=freq[fsel]
printvar(id='[dstb_file.g] stepsel:',var=tsel)
printvar(id='[dstb_file.g] freqsel:',var=fsel)
    clires:=[=]
    if (fexist(axsname)) {
      clires.avgspec:=read_value(axsname).avgspec[fsel]
      clires.avgpwrc:=read_value(axsname).avgpwrc[tsel]
      print '[dstb_file.g]',axsname,'loaded'
    } else {print '[dstb_file.g]',axsname,'does no exist'};
    if (fexist(absname)) {
      clires.absds:=read_value(absname)[tsel,fsel]
      if (!is_float(clires.absds)) {clires.absds:=as_float(abs(clires.absds))};
      print '[dstb_file.g]',absname,'loaded'
    } else {print '[dstb_file.g]',absname,'does no exist'; disp:=F};
    if (!fexist(absname)) {if (fexist(pixname)) {
      clires.absds:=as_float(abs(read_value(pixname)[tsel,fsel]))
      print '[dstb_file.g]',pixname,'loaded'
    } else {print '[dstb_file.g]',pixname,'does no exist'};};
    datename:=read_value(axsname).datename
    antsname:=read_value(axsname).antsname
    timunit:=read_value(axsname).timunit
 };

  ## presave
  if (act) {
    datename:=split(rec.head('Date'),'')
    datename:=spaste(spaste(datename[3:4]),spaste(datename[6:7]),spaste(datename[9:10]))
    antsname:=spaste(len(ants),'ant')
  } # end if act
 
  ## save
  if (sav) {
    print ''
    print '[dstb_file.g] saving...'
    # filenames
    savename:=spaste(name,'_',round(range(freq)[1]/1e6*10)/10,'t',round(range(freq)[2]/1e6*10)/10)
    if (lod&&loadname==savename) {savename:=spaste(savename,'_new')};
    pixname:=spaste(sosdir,'dstb_',savename,'.pix.sos')
    absname:=spaste(sosdir,'dstb_',savename,'.abs.sos')
    axsname:=spaste(sosdir,'dstb_',savename,'.axs.sos')
    # ds
    #if (!nods&&!lod) {write_value(clires.ds,pixname)};
    if (!nods) {write_value(clires.absds,absname)};
    if (fexist(pixname)) {print '[dstb_file.g] saved: ',pixname}
                    else {print '[dstb_file.g]',pixname,'NOT saved !!!'};
    if (fexist(absname)) {print '[dstb_file.g] saved: ',absname}
                    else {print '[dstb_file.g]',absname,'NOT saved !!!'};
    # axes
    axs:=[=]; axs.step:=step; axs.freq:=freq; axs.timunit:=timunit;
    axs.avgspec:=clires.avgspec; axs.avgpwrc:=clires.avgpwrc
    axs.datename:=datename; axs.antsname:=antsname
    save(axs,axsname); axs:=[]
  }; # endif save
  
  ## display
  if (disp) {
print ''  
print '[dstb_file.g] disp...'
#printvar(id='[dstb_file.g] step:',var=step)
#printvar(id='[dstb_file.g] freq:',var=freq)
    print ''
    timename:=spaste(round(range(step)[1]),'t',round(range(step)[2]))
    freqname:=spaste(round(range(freq)[1]/1e6*10)/10,'t',round(range(freq)[2]/1e6*10)/10)
    filename:=spaste(name,'_',timename,'_',freqname,'_',antsname)
    # dynamic spectrum
    if (!nods) {
      #clires.ds:=[]
      dsname:=spaste('ds_',filename)
      dsdispname:=spaste(datename,' ',timename,' ',antsname)
      imgplot(clires.absds,name=dsdispname,timvals=step,bandlim=range(freq),sosdir=sosdir,
              axisorder=[2,1],freqdir=1,dmin=0,dmax=bright,
              xax=T,evnums=0,timunit=timunit,psname=dsname,psdir=psdir,gif=ps,done=done)
      print ''
    }; # if not nods
    # average spectrum
    spname:=spaste('sp_',filename)
    plotdim(log(abs(clires.avgspec)),xvals=freq/1e6,psname=spname,psdir=psdir,gif=ps,done=done,
            title='Average Spectrum',xlab='Frequency (MHz)',ylab='Power (dB)')
    # lightcurve
    if (len(clires.avgpwrc)==len(step)) {
      lcname:=spaste('lc_',filename)
      plotdim(clires.avgpwrc,xvals=step,psname=lcname,psdir=psdir,gif=ps,done=done,
              title='Lightcurve',xlab=spaste('Time (',timunit,')'),ylab='Power (arb.units)')
    };

    # publish
    docname:=spaste(psdir,'doc_',filename)
    if (ps) {if (!nods) {
      # merge plots (pdf-file)
      print ''; print '[dstb_file.g] Writing PDF-file:',docname
      shell(spaste('convert -density 72x72 ',psdir,dsname,'.ps ',psdir,'help.ps'))
      #shell(spaste('mv ',psdir,'help.ps ',psdir,dsname,'.ps')) # replace big by smale ds-ps-file
      shell(spaste('ps2doc ',docname,'.ps 2 "',psdir,'help.ps ',psdir,spname,'.ps ',psdir,'help.ps ',psdir,lcname,'.ps"'))
      shell(spaste('rm -f ',psdir,'help.ps ',docname,'.ps')) # remove small ds-ps-file & doc-ps-file
    } else {
      shell(spaste('ps2doc ',docname,'.ps 1 "',psdir,spname,'.ps ',psdir,lcname,'.ps"'))
      shell(spaste('rm -f ',docname,'.ps')) # remove doc-ps-file
    };}; # if ps- & pdf-file(s) generation
  }; # endif disp
  
  ## return
  if (ret) {
    ret:=[=]
    ret.freq:=freq
    ret.step:=step
    ret.avgspec:=clires.avgspec
    ret.avgpwrc:=clires.avgpwrc
    ret.filenames:=filenames
    ret.absds:=clires.absds
    if (sav) {ret.savename:=savename};
    print ''
    toc(id='[dstb_file.g] time:')
    return ret
  } else {return T};
  #write_value(res,'dstb.sos')
  
  ## plotting
  #ds
  #imgplot(name='dstb_jup4tbX',sosdir='jsos/',bandlim=[18,20]*1e6,axisorder=[2,1],freqdir=1,dmin=0,dmax=.5,xax=T,evnums=0,timunit='s',psname='',psdir='ps/',gif=T,done=F)
  #avgspec
  #res:=read_value('jsos/dstb_jup5tbX.axs.sos')
  #plotdim(log(abs(res.avgspec)),xvals=res.freq/1e6,title='Average Spectrum: jup5tbX',xlab='Frequency (MHz)',ylab='log abs FFT')
  #plotdim(log(blockaverage(runminc(abs(res.avgspec),bs=10),block=5)),xvals=res.freq/1e6,add=T)
  #lightcurve
  #res:=read_value('jsos/dstb_jup5tbX.axs.sos')
  #plotdim(log(abs(res.avgspec)),xvals=res.freq/1e6,title='Average Spectrum: jup5tbX',xlab='Frequency (MHz)',ylab='log abs FFT')
  #plotdim(log(blockaverage(runminc(abs(res.avgspec),bs=10),block=5)),xvals=res.freq/1e6,add=T)
}
#-------------------------------------------------------------------------------
