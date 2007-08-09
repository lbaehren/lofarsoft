#  generation of dynamic spectrum maps for a number of event-files
#  Andreas Nigl @ 28.05.03 @ MPIfR
#  Last update @ 10.10.03 @ KUN
#######################################################################

#######################################################################
#Section: TOC #
###############
# <li>dsmap       generate dynamic spectrum map
# <li>timedel     beamforming via delays in time domain
# <li>fftshiftcpp beamforming via phase gradients in frequency domain
# <li>fftshift    beamforming via phase gradients in frequency domain
# <li>meanpwr     average over antenna power
# <li>specs       plots spectras
# <li>read_merge  merges arrays from sos files to matrix

#######################################################################
#Section: FUNCTIONS ###################################################
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: dsmap
#----------------------------------------------------------------------
#Type: function
#Doc:  generates a dynamic spectrum map from a sos-file
#Par:  ants =    - antenna selection, ants=-1 -> all antennas
#Par:  act  = T   - T = uses DSary, F = generates map from sos-file
#Par:  disp = T  - T = combines sos-files if ACT=F and generates images,
#                  F = no displaying at all and returns pixelarray!
#Par:  plots = T - T = generates images and viewer with dynamic spectrum,
#                  F = no images and no viewer
#Par:  info = F  - T = all important informations for the dynamic
#                      spectrum are saved in a sos file (/log/name.sos)
#Todo: 1) for a big number of antennas data.ant()>antpos->delays&ant_check(data) sould be trimmed to antenna selection 2) centeroid of antennas
#Example: +s :o) </br>
#----------------------------------------------------------------------
#[1 example] include 'dynspec.g'; tic(); pix:=dsmap(name='example',act=T,events=T,eventdir='example/',efirst=1,elast=1,ants=-1,azmelv=[0,90],method=3,blockinfo=[1,-1,2^14],chans=-1,bandlim=[40,80],disp=T,plots=T,ps=F,sosdir=sosfiledir,sfirst=1,slast=-1,bright=0,ret=T); toc() </br>
#[2 fftshift]  </br>
#[1] include 'dynspec.g'; tic(); pix:=dsmap(name='test',act=T,events=T,eventdir='example/',efirst=1,elast=1,ants=-1,azmelv=[0,90],method=2,blockinfo=[1,-1,2^15],bandlim=[40,80],chans=4000,disp=T,plots=T,ps=F,sosdir=sosfiledir,bright=0); toc() </br>
#[2] include 'dynspec.g'; tic(); pix:=dsmap(name='test',act=T,events=T,eventdir='',efirst=1,elast=1,ants=-1,azmelv=[0,0],method=2,blockinfo=[1,-1,2^15],chans=-1,disp=T,plots=T,ps=T,bright=1); toc() </br>
#[3 dscpp]  </br>
#[1] include 'dynspec.g'; tic(); pix:=dsmap(name='test',act=T,events=T,eventdir='example/',efirst=1,elast=1,ants=-1,azmelv=[0,90],method=4,blockinfo=[1,-1,2^15],bandlim=[40,80],chans=4000,disp=T,plots=T,ps=F,sosdir=sosfiledir,sfirst=1,slast=-1,bright=0); toc() </br>
#[2] include 'dynspec.g'; ants:=-1; input_project(data,dir='/raid/pv3/ITS/jupiter/2004.06.22-15:37:34.00-jupiter06/',head='*.hdr',ants=ants); tic(); pix:=dsmap(name='ds_jup6_beam_b1t64_bs2t20',act=T,events=F,eventdir='',efirst=1,elast=1,ants=ants,azmelv=[163.77,44.35],method=4,blockinfo=[1,64,2^20],bandlim=[0,40],chans=4000,disp=T,plots=T,ps=F,sosdir=sosfiledir,sfirst=1,slast=-1,bright=0); toc()
#----------------------------------------------------------------------
#<!- Andreas Nigl @ 26.06.03 @ MPIfR ->
#----------------------------------------------------------------------
#-#POSITION
# Kascade WGS84 +088.26.16.670, +049.06.02.478, 124.597m
#----------------------------------------------------------------------
dsmap:=function(
 name='dynspec',
 act=T,
 events=T,
 eventdir='',
 efirst=1,elast=-1,
 ants=-1,
 bants=[],
 direction='',
 azmelv=[0,90],
 method=2,
 blockinfo=[1,-1,2^16],
 offset=0,
 chans=4000,
 bandlim=[43.6,76.1],
 han=0,
 xaxis=T,
 disp=F,
 plots=F,
 ps=F,
 sosdir='',
 sfirst=1, slast=-1,
 bright=0,
 sep=F,
 rel=F,
 info=F,
 ret=F
)
{
  ### Methods ###
  if (method>5) {method:=3};
  mtxt[1]:='timeshifting'
  mtxt[2]:='phaseshifting'
  mtxt[3]:='meanpower noiset'
  mtxt[4]:='meanpower absfft'
  mtxt[5]:='ds++'
  
  ### Parameter Decisions ###
  fdisp:=F;                                # fileinfo
  pact:=T; pdisp:=F; pplots:=F; pps:=F     # phaseid
  if (method==0)                           # phaseid
  {
    if ( act==F)  {act  :=T; pact:=F};
    if (disp==T)  {pdisp:=T; disp:=F};
    if (plots==T) {pplots:=T; pps:=T};
    if (pact==T)  {pdisp:=F};
  };
  if (disp==F) {fdisp:=F};                 # fileinfo
  if (plots==F) {ps:=F};                   # dsdisp

  ### DSary (Information) ###
  #----------------------------------------------------------------------

  # Print #
  print ''
  print '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%'
  print 'DYNAMIC SPECTRUM'
  print '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%'
  print 'dynspec name:',name
  print 'event directory:',eventdir
  if (events) {print 'number of files:',elast-efirst+1} else {print 'data type: project-file(s)'};
  if (method!=3) {
    if (direction!='') {print 'direction:',direction}
    else {print 'direction [azimuth(0..360) & elevation(0..90up)]:',as_string(azmelv)};
  }; #endif method
  if (ants==-1) {print 'antennas: all'} else {print 'antennas:',as_string(ants)};
  print spaste('blocksize: ', blockinfo[3]);
  if (chans==-1) {print 'chans: all'} else {print 'chans:',chans};
  print 'bandlimits:',as_string(bandlim);
  if (method>0) {print 'method:',mtxt[method]};
  if (method==0) {print 'method: phasedelayid'};
  print '';

  # Files #
  files:=''; numfiles:=1;
  if (events) {
    print 'FILES:';
    files:=filesel(eventdir,ext='*.event',first=efirst,last=elast,disp=fdisp);
    numfiles:=len(files);
    if (numfiles==1) {if (!is_string(files)) {if (!(files)) return F}};
  };
    
  # Event Files #
  if (events&&method!=0) {
  
    # Antennas #
    input_event(data,files[1])
    
    # Naming #
    if (act) {name:=spaste(name,'_',nants,'ants')}
    else {
      name:=shell(spaste('ls ',sosdir,'*ev*.sos'))~globalconst.FilenameExtractor
      if (len(name)==0) {print 'DSMAP: no sos-files in sos-directory !'; return};
      if (len(name)>1) {
        name:=name[1]; name:=split(name,'')
        name:=spaste(spaste(name[1:14]),'x',spaste(name[19:(len(name)-4)]))
      } else {name:=split(name,''); name:=spaste(name[1:(len(name)-4)])}; # files
    }; # act
  
  }; # events...

  # Antenna Selection #
  if (ants==-1) {ants:=data.selant()}
  else {data.select(-1,F); for (ant in ants) {if (ant<=max(data.ant())) {data.select(ant,T)};}};
  if (len(bants)>0) {for (ant in bants) {if (ant<=max(data.ant())) {data.select(ant,F)}};};
  ants:=data.selant(); nants:=len(ants)
  
  ### DSary (Calculation) ###
  #----------------------------------------------------------------------
  if (act)  {

    # Directories #
    if (sosdir=='') {sosdir:='sos/'};
    if (!fexist(sosdir)) {shell(spaste('mkdir ',sosdir))};

    # Viewersteps #
    sfile:=spaste(sosdir,name,'.sos.timesteps');
    if (xaxis) {
      #print ''
      print 'DSMAP: generating viewersteps';
#print len(files),files,events,blockinfo,rel,sep,fdisp;
      timesteps:=fileinfo(files,events=events,blockinfo=blockinfo,rel=rel,sep=sep,disp=fdisp,ps=F);
#print !is_double(timesteps),!is_integer(timesteps),timesteps
      if (!is_double(timesteps)&!is_integer(timesteps)) {if (!(timesteps)) return F};
      print 'DSMAP: saving viewersteps';
      write_value(timesteps,sfile); # save timesteps-variable to extra file
      if (fexist(sfile)) {print spaste('DSARY: ',sfile,' successfully saved !!!')}
      else {print spaste('DSARY: ',sfile,' NOT saved !!!')};
    } else {
      timesteps:=0
      if (fexist(sfile)) {shell(spaste('rm ',sfile))};
    }; #end else xaxis

    # PREPARATION #
    if (method!=0) { #&1!=1 !!!
      # load #
      if (events) input_event(data,files[1]);
      # blocksize #
      #SetBlock(data,n=blockinfo[1],blocksize=blockinfo[3],offset=offset) #!!!
print 'BLOCKINFO:',[blockinfo,offset]
      data.setblock(n=blockinfo[1],blocksize=blockinfo[3],offset=offset,index=1)
      freq:=data.get('Frequency')
      fftlen:=len(freq)
      if (chans==-1) chans:=fftlen;
      # blocknum & separation #
      filesize:=data.getmeta('Filesize')
      if (blockinfo[2]==-1) blockinfo[2]:=filesize/blockinfo[3];
      if (blockinfo[2]>1) {
        if (sep) {columns:=numfiles*(blockinfo[2]+1)-1}
        else {columns:=numfiles*blockinfo[2]}} # some blocks per file
      else {columns:=numfiles}; # one block per file
      # bandlim #
      bandwidth:=data.headf('Bandwidth')*samplerateunit
      frequnit:=data.headf('FrequencyUnit')
      freqmin:=data.getmeta('FrequencyLow')
#print bandlim
      hlp:=[freq[valpos(freq,bandlim[1]*frequnit,big=F)],freq[valpos(freq,bandlim[2]*frequnit,big=T)]]
      bandsel:=freq>=hlp[1]&freq<=hlp[2]
      speclen:=sum(bandsel==T);
      if (chans>speclen) {chans:=speclen};
#print 'INIT PIXARRAY COLUMNS & CHANS:',columns,chans;
      global pixarray:=array(0.,columns,chans)
print ''
print 'DSMAP PIXARRAY:',shape(pixarray)
print ''

    };

if (info) {
    # INFO #
    info.title:=title
    info.name:=name
    info.act:=act
    info.events:=events
    info.eventdir:=eventdir
    info.efirst:=efirst
    info.elast:=elast
    info.numfiles:=numfiles
    info.ants:=ants
    info.datalen:=data.len
    info.method:=method
    info.direction:=direction
    info.azmelv:=azmelv
    info.filesize:=filesize
    info.blockinfo:=blockinfo
    info.columns:=columns
    info.bandlim:=bandlim
    info.xaxis:=xaxis
    info.chans:=chans
    info.pixarray:=shape(pixarray)
    info.freqmin:=freqmin
    info.bandwidth:=bandwidth
    info.frequnit:=frequnit
    info.disp:=disp
    info.plots:=plots
    info.ps:=ps
    info.sosdir:=sosdir
    info.sfirst:=sfirst
    info.slast:=slast
    info.bright:=bright
    info.rel:=rel
    info.sep:=sep
    info.numtimesteps:=len(timesteps)
    info.files:=files
    info.timesteps:=timesteps
    sfile:=spaste(sosdir,name,'.info.sos');
print 'SAVING INFO !!!';
    write_value(info,sfile); # save timesteps-variable to extra file
    if (fexist(sfile)) {print spaste('DSMAP: ',sfile,' successfully saved !!!')}
    else {print spaste('DSMAP: ',sfile,' NOT saved !!!')};
};

    # LOG #
    i:=1;
    title:='DYNAMIC SPECTRUM LOG FILE';
    logstrary[i]:=title; i+:=1
    logstrary[i]:=paste('name:',name); i+:=1
    logstrary[i]:=paste('act:',act); i+:=1
    logstrary[i]:=paste('events:',events); i+:=1
    logstrary[i]:=paste('eventdir:',eventdir); i+:=1
    logstrary[i]:=paste('efirst:',efirst); i+:=1
    logstrary[i]:=paste('elast:',elast); i+:=1
    logstrary[i]:=paste('numfiles:',numfiles); i+:=1
    logstrary[i]:=paste('ants:',ants,1:data.len); i+:=1
    logstrary[i]:=paste('method:',method); i+:=1
    logstrary[i]:=paste('direction:',direction); i+:=1
    logstrary[i]:=paste('azmelv:',azmelv); i+:=1
    logstrary[i]:=paste('filesize:',filesize); i+:=1
    logstrary[i]:=paste('blockinfo:',blockinfo); i+:=1
    logstrary[i]:=paste('columns:',columns); i+:=1
    logstrary[i]:=paste('bandlim:',bandlim); i+:=1
    logstrary[i]:=paste('xaxis:',xaxis); i+:=1
    logstrary[i]:=paste('chans:',chans); i+:=1
    logstrary[i]:=paste('pixarray:',shape(pixarray)); i+:=1
    logstrary[i]:=paste('freqmin:',freqmin); i+:=1
    logstrary[i]:=paste('bandwidth:',bandwidth); i+:=1
    logstrary[i]:=paste('frequnit:',frequnit); i+:=1
    logstrary[i]:=paste('disp:',disp); i+:=1
    logstrary[i]:=paste('plots:',plots); i+:=1
    logstrary[i]:=paste('ps:',ps); i+:=1
    logstrary[i]:=paste('sosdir:',sosdir); i+:=1
    logstrary[i]:=paste('sfirst:',sfirst); i+:=1
    logstrary[i]:=paste('slast:',slast); i+:=1
    logstrary[i]:=paste('bright:',bright); i+:=1
    logstrary[i]:=paste('rel:',rel); i+:=1
    logstrary[i]:=paste('sep:',sep); i+:=1
    logstrary[i]:=paste('timesteps:',len(timesteps),max(timesteps)); i+:=1
    logstrary[i]:=paste('files:',files); i+:=1
    logstrary[i]:=paste('timesteps:',timesteps); i+:=1
    dir:='log/'; if (!fexist(dir))
      {shell(spaste('mkdir ',dir)); print 'Created Directory:',dir};
    filename:=spaste('log/',name,'.dslog')
    textfile:=open(spaste('>',filename))
    for (strnum in 1:len(logstrary)) {write(textfile,logstrary[strnum])}
    if (fexist(filename)) {print spaste('DSMAP: ',filename,' successfully saved !!!')}
    else {print spaste('DSMAP: ',filename,' NOT saved !!!')};
    #funclog(filename,title,
    #  name,act,events,eventdir,efirst,elast,ants,direction,
    #  azmelv,method,blockinfo,chans,bandlim,disp,plots,ps,
    #  sosdir,sfirst,slast,bright,sep,shape(pixarray)
    #);

    ### 0) PHASE DELAY ID ###
    if (method==0)
    {print 'plots:',pplots
      phasedelays:=phaseid(
        name=name,act=pact,files=files,
        freqrange=[62165865.6,62223239.5,67673778,67712097,67925426,67946548],
        ants=ants,nsigma=2,sosdir=sosdir,disp=pdisp,plots=pplots,ps=pps
      );
      return phasedelays;
    };

    ### 1) TIMEDEL (method=1) ###
    if (method==1) {
      pixarray:=timedel(
        files=files,events=events,ants=ants,chans=chans,
        direction=direction,azmelv=azmelv,blockinfo=blockinfo,bandlim=bandlim
      );
    }; # endif timedel

    ### 2) FFTSHIFT (method=2) ###
    if (method==2) {
      pixarray:=fftshift(
        files=files,events=events,ants=ants,chans=chans,
        direction=direction,azmelv=azmelv,blockinfo=blockinfo,
        offset=offset,bandlim=bandlim,han=han
      );
    }; # endif fftshift

    ### 3) MEANPOWER NOISET (method=3) ### !!! daily dynamic spectra
    if (method==3) {
      pixarray:=meanpwr(
        files=files,events=events,ants=ants,chans=chans,
        blockinfo=blockinfo,bandlim=bandlim
      )
    }; # endif meanpwr
    
    ### 4) MEANPOWER ABSFFT (method=4) ###
    if (method==4) {
      pixarray:=meanpwr(
        files=files,events=events,ants=ants,chans=chans,
        blockinfo=blockinfo,offset=offset,bandlim=bandlim,pwr=T,han=han
      )
    }; # endif complex meanpwr

    ### 5) DSCPP (method=4) ###
    if (method==5) {
      pixarray:=dscpp(
        files=files,events=events,ants=ants,chans=chans,
        azmelv=azmelv,blockinfo=blockinfo,bandlim=bandlim
      )
    }; # endif dscpp
    
  }; # end act ### DSary (Calculation) ###
  #----------------------------------------------------------------------

  ### DSdisp (Displaying) ###
  #----------------------------------------------------------------------
  if (method!=0) {
    dsdisp(
      #pixarray,
      act=act,name=name,events=events,
      bandlim=bandlim,bright=bright,
      timesteps=timesteps,rel=rel,
      sosdir=sosdir,sfirst=sfirst,slast=slast,
      disp=disp,plots=plots,ps=ps,ret=ret
    );
  } # endif method
  #----------------------------------------------------------------------

} # end function DSmap
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: dsdisp
#----------------------------------------------------------------------
#Type: function
#Doc:  saving and displaying of pixarray
#Example:
#----------------------------------------------------------------------
#<!- Andreas Nigl @ 15.07.04 @ KUN ->
#----------------------------------------------------------------------
dsdisp:=function(
  #pixarray,
  act=T,name='test',events=F,
  bandlim=[40,80],bright=0.001,
  timesteps='',rel=T,
  sosdir='sos/',sfirst=1,slast=1,
  disp=T,plots=T,ps=F,ret=F
)
{
  ### DSary (Saving) ###
  #----------------------------------------------------------------------

  if (act)  {

    if (disp)
    {
      # infos #
      print 'SINGLE FILE SAVING...';
      print 'pixarray:',shape(pixarray),max(pixarray);
      print spaste('sfile: ',name,'.sos');
    };

    # saving #
    if (!fexist(sosdir)) {shell(spaste('mkdir ',sosdir))};
    sfile:=spaste(sosdir,name,'.sos');
    write_value(pixarray,sfile); # save pixel-array to sos-file
    if (fexist(sfile)) {print spaste('DSDISP: ',sfile,' successfully saved !!!')}
    else {print spaste('DSDISP: ',sfile,' NOT saved !!!')};
  }; # end act ### DSary (Part2 - Saving) ###
  #----------------------------------------------------------------------

  ### DSimg ###
  #----------------------------------------------------------------------

  # load sos-filenames # if (disp&act) -> nur 'x' !!!
  sosfnames:=filesel(sosdir,ext='*.sos',first=sfirst,last=slast); numfiles:=len(sosfnames)
  if (act) {sosfnames:=spaste(sosdir,name,'.sos'); slast:=1; numfiles:=1} #direct displaying of just one file
  else {if (numfiles==1) {
print '!!! NEW NAMING FOR ONE SOS-FILE !!!'
print sosfnames,'->',sfile
    shell(spaste('mv ',sosfnames,' ',sfile))
    shell(spaste('mv ',spaste(sosfnames,'.timesteps'),' ',spaste(sfile,'.timesteps')))
    sosfnames:=sfile
  }};

  # info #
  if (disp)
  {
    print '';
    print 'IMAGING...';
    print 'sosfile directory:',sosdir;
    print 'sosfile(s) [number, name, size]:';
    #for (filenum in 1:numfiles) {print(spaste('[[',as_string(filenum),']] ',sosfnames[filenum]))}
  }; #endisp

  # load array & timesteps from sos-files #
  arylen:=0
  pixelary:=[=]
  timesteps:=[=]
#print numfiles
  for (filenum in 1:numfiles)
  {
print 'PIXELARY:',filenum,sosfnames[filenum]
    pixelary[filenum]:=read_value(sosfnames[filenum]) # pixarray
print 'PIXELARY:',shape(pixelary)
print 'PIXELARY:',pixelary[filenum][1:10]
      freqlen:=shape(pixelary[filenum])[2]
      filecolumns[filenum]:=shape(pixelary[filenum])[1]
    timestepfile:=spaste(sosfnames[filenum],'.timesteps')
#print timestepfile
      if (fexist(timestepfile)) {timesteps[filenum]:=read_value(timestepfile); steps:=T}
      else {steps:=F; print 'DSDISP: no viewerstepsfile found'};
    print spaste('[',filenum,']'),
                 sosfnames[filenum]~globalconst.FilenameExtractor,filecolumns[filenum],
                 freqlen,max(pixelary[filenum]);
    if (filenum>1) {if(freqlen!=arylen) {print 'DSDISP: files contain different matrix sizes'}};
    arylen:=freqlen
  }

  # combination of arrays # (data & timesteps)
  timelen:=sum(filecolumns);
  pixels:=array(0.,timelen,freqlen);
  viewersteps:=array(0.,timelen);
  start:=1;
  if (numfiles>1) {
    bar:=progress(1,numfiles,'Calculating Dynamic Spectrum')
    bar.activate()
  } else {bar:=F};
#print 'pixelary:',shape(pixelary[1]),numfiles
#print 'pixels:',shape(pixels),start,start+filecolumns[numfiles]-1
  for (block in 1:numfiles)
  {
#print block,numfiles
#print filecolumns
#print shape(pixelary),shape(timesteps)
    pixels[start:(start+filecolumns[block]-1),]:=pixelary[block];
    if (steps) {viewersteps[start:(start+filecolumns[block]-1)]:=timesteps[block]};
    start:=(start+filecolumns[block])
    if (bar) {bar.update(block)};
  };
  if (bar) {bar.done()};

  # timestamps #
  sfile:=spaste(sosdir,name,'.sos')
if (events) {shell(spaste('rm *evx*.sos.timesteps')); steps:=F}; # !!! timestepsfile concatenation does not work REPAIR !!!
  if (!steps&&events) {viewersteps:=timestamps(filename=sfile); steps:=T}; # !!! implement loadynspeclopes_cbfunc !!!
  print spaste('[X] ...timestamps'),len(viewersteps),max(viewersteps)
  # imgage plot #
  if (disp)
  {
    xax:=F; if (steps) xax:=T;
    timunit:='s'; if (events&&!rel) {timunit:='h'};
    if (plots) {
#print shape(pixels),name,bandlim,bright,xax,shape(viewersteps),timunit,ps
      vdp:=imgplot(pixels,name=name,axisorder=[2,1],bandlim=bandlim,freqdir=1,
                   dmin=0,dmax=bright,xax=xax,evnums=0,timvals=viewersteps,
                   timunit=timunit,ps=ps,done=F);
    };
  }; #endisp

  # saving #
  if ((!act)&(numfiles>1)) # save pixel-array & timesteps to sos-file if generated from a number of sos-files
  {
    # infos #
    if (disp)
    {
      print ''
      print 'COMBINED FILES SAVING...'
      print 'pixarray:',shape(pixels),max(pixels)
      print 'sfile:',sfile~globalconst.FilenameExtractor
      print ''
    }; #endisp
    # write #
    write_value(pixels,sfile);
    if (fexist(sfile)) {print spaste('DSIMG: ',sfile,' successfully saved !!!')}
    else {print spaste('DSIMG: ',sfile,' NOT saved !!!')};
    if (steps) {
      timesteps:=viewersteps
      print 'DSDISP: saving (combined) viewersteps'
      write_value(timesteps,spaste(sfile,'.timesteps')) # save timesteps-variable to extra file
    if (fexist(spaste(sfile,'.timesteps')))
      {print spaste('DSIMG: ',sfile,'.timesteps successfully saved !!!')}
    else {print spaste('DSIMG: ',sfile,' NOT saved !!!')};
    }; # endif steps
  }; # endif saving

  # return #
  global pixels
  if (ret) {
    pix:=[=]
    pix.pixels:=pixels
    pix.time:=viewersteps
    pix.name:=name
    global freqsel
    pix.freq:=freqsel
    pix.freqlen:=len(freqsel) #sum(bandsel==T)
    return pix
  }

  #----------------------------------------------------------------------

} # end function dsdisp
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: dscpp
#----------------------------------------------------------------------
#Type: function
#Doc:  beamforming via phase gradients in frequency domain
#      using Dynamic Spectrum by Lars Baehren
#Par:  files     = 'example.event' - filenames
#Par:  ants      = 1:8             - antenna selection, ants=-1 -> all antennas
#Par:  bandlim   = [40,80]         - frequency band limitations
#Par:  chans     = 4000            - frequency channels, chans=-1 -> max number of chans
#Par:  azmelv    = [0,90]          - direction for beam (delays)
#Par:  blockinfo = [1,-1,2^16]     - [first block,number of blocks,blocksize]
#Par:  sep       = F               - T = space between blocks of files
#Ret:  ary       =                 - return array with beamformed data
#Ref:  input_event(), channels()
#Todo: time consuming: input_event & SetBlock / rec.setblock()
#Example: [1] include 'dynspec.g'; tic(); pix:=dscpp(files='example.event',events=T,ants=-1,bandlim=[40,80],chans=4000,azmelv=[0,90],blockinfo=[1,-1,2^12],sep=F); toc(); imgplot(pix)
#----------------------------------------------------------------------
#<!- Andreas Nigl @ 11.03.04 @ KUN ->
#----------------------------------------------------------------------
# BEAMFORMING
# example
#include 'dynspec.g'; tic(); pix:=dscpp(files='example.event',events=T,ants=-1,bandlim=[40,80],chans=4000,azmelv=[0,90],blockinfo=[1,-1,2^15],sep=F); toc(); imgplot(pix)
# project
#include 'dynspec.g'; ants:=-1; input_project(data,dir='example.its/',head='*.hdr',ants=ants); tic(); pix:=dscpp(files='',events=F,ants=-1,bandlim=[0,40],chans=4000,azmelv=[0,90],blockinfo=[1,-1,data.getmeta('Blocksize')],sep=F); toc(); imgplot(pix)
# MEAN POWER ???
# example
#include 'dynspec.g'; tic(); pix:=dscpp(files='example.event',events=T,ants=-1,bandlim=[40,80],chans=4000,blockinfo=[1,-1,2^15],sep=F); toc(); imgplot(pix)
#----------------------------------------------------------------------
dscpp:=function(
  files='example.event',events=F,ants=-1,bandlim=[40,80],chans=4000,
  azmelv=[],blockinfo=[1,-1,2^16],sep=F
)
{
  # client
  global DynamicSpectrumCLI:=client(spaste(globalconst.cprogDir,'/DynamicSpectrum'));
  recClient:=[=];
  
  # preload
print ''; print 'please wait ...'      
  rec:=[=]; if (events) {input_event(rec,files[1])} else {rec:=ref data};
  
  # antenna selection
  refant:=1
  if (ants==-1) {ants:=data.selant()};
#print 'dscpp: ants:',ants
 
  # method
  if (len(azmelv)==0) {mtxt:='meanPower'; azmelv:=[0,0]} else {mtxt:='beamforming'};
   
  # frequency
  frequnit:=rec.headf('FrequencyUnit');
  freqmin:=rec.getmeta('FrequencyLow');
  bandwidth:=rec.headf('Bandwidth')*samplerateunit;
  
  # bandlim
  bandlim:=VerifyFrequencyBand(rec,bandlim)*frequnit

  # blockinfo
  if (blockinfo[2]==-1) {
    filesize:=rec.getmeta('Filesize')
    blockinfo[2]:=ceil(filesize/blockinfo[3])
  }
  ##blockinfo:=VerifyBlockinfo(rec,blockinfo); #!!!
 
  # print #
  print ''
  print 'BEAMFORMING...'
  print 'method:',mtxt
  print 'azel:',azmelv
  print 'first block:',blockinfo[1]
  print 'number of blocks:',blockinfo[2]
  print 'blocksize:',blockinfo[3]
  print ''
#print 'Filesize:',rec.getmeta('Filesize')
#print 'FileBlocksize:',rec.getmeta('FileBlocksize')
#print 'Blocksize:',rec.getmeta('Blocksize')
#print ''
    
  # setblock
print 'please wait ...'; print ''
  #SetBlock(rec,n=blockinfo[1],blocksize=blockinfo[3]) #!!!
  rec.setblock(n=blockinfo[1],blocksize=blockinfo[3],index=ants)
  
  # file number
  numfiles:=len(files);
  
  # time information
  timeStart:=mean(rec.get('Time'));
  timeIncr:=blockinfo[3]/(rec.headf('Samplerate')*rec.headf('SamplerateUnit'));
  
  #-----------------------------------------------------------------    
  # record for client
  epoch:=qnt.quantity(rec.head('Date'));
  recClient.Epoch            := epoch;
  recClient.epochValue       := epoch.value;
  recClient.epochUnit        := epoch.unit;
  recClient.ObservatoryName  := rec.head('Observatory');
  recClient.antennaSet       := ants;
  recClient.CoordinateSystem := 'AZEL'
  recClient.Projection       := 'CAR'
  recClient.AntennaPositions := getAntennaPositions(rec,refant=refant);
  recClient.Coordinates      := azmelv
  DataToRecord(rec,recClient,'Frequency')
  recClient.FrequencyBand    := bandlim;
  recClient.Blocksize        := as_integer(rec.getmeta('Blocksize'));
  recClient.dsMethod         := mtxt
  recClient.inputData        := 'rawTime'
  recClient.statistics       := F;
  recClient.differential     := F;
#printrec(recClient)
  
  # beamformer initialisation
  replyCLI:=DynamicSpectrumCLI->initBeamformer(recClient);
  chansCLI:=len(replyCLI.frequencies)
  if (chans==-1|chans>len(freq)) {chans:=chansCLI};

  # frequency selection
  global freq
  freq:=rec.get('Frequency');
  bandsel:=freq>=bandlim[1]&freq<=bandlim[2];
  global freqsel
  freqsel:=freq[bandsel]
  
  # progress bar
  filebar:=progress(1,numfiles,'[Dynamic Spectrum] Files ...')
  if (is_record(filebar)) {filebar.activate()};

  #-----------------------------------------------------------------    
  # print 
  #print ''
global pixarray
#pixarray:=array(0.,numfiles*blockinfo[2],chans) # for usage of only the function
print 'DSCPP PIXARRAY:',shape(pixarray),max(pixarray)
  print ''
  print '[[X]] File'
  print ' [X]  Block'
  print ''
  
  # files #
  filenum:=0;
  for (file in files) # for number of event-files
  { filenum+:=1;
      
    # event #
    if (events) {
print 'please wait ...'
      input_event(rec,file); nants:=rec.len;
      while (max(ants)>nants) {ants:=ants[ants!=max(ants)]}; #selectioncorrection
      nants:=len(ants);
    }; # end events
    
    # setblock #
    #SetBlock(rec,n=blockinfo[1],blocksize=blockinfo[3]) #!!!
    rec.setblock(n=blockinfo[1],blocksize=blockinfo[3],index=ants)
    
    # print filenum # (flags)
    #print '';
    print(spaste('[[',as_string(filenum),']] ',spaste(as_string(rec.flag())),' ',files[filenum]));

    # variables
    datablock:=array(0,chansCLI,nants)
    if (calcDifferential) {diffSpectrum:=array(0,blockinfo[2]-1,chansCLI)};
    spectrum:=array(0,blockinfo[2],chansCLI);
    powerAnt:=array(0,blockinfo[2],chansCLI);
    
    # block preparation #
    if (blockinfo[2]>1) {
      # file start block
      if (sep) {start:=(filenum-1)*(blockinfo[2]+1)}
      else {start:=(filenum-1)*blockinfo[2]} # some blocks per file
      # progress bar
      blockbar:=progress(1,blockinfo[2],'[Dynamic Spectrum] Blocks ...')
      if (is_record(filebar)) {blockbar.activate()};
    } else {start:=filenum-1}; # one block per file  
      
    #-----------------------------------------------------------------
    # blocks
    for (block in 1:blockinfo[2]) {print spaste('[',block,']')
    
      # antenna fft data extraction
      ant:=0; for (i in ants) {ant+:=1; datablock[,ant]:=rec.get('CalFFT',i)[bandsel]}
      
      # next block
      #rec.nextblock(n=1,index=-1)
      if ((rec.getmeta('Offset')+rec.getmeta('Blocksize'))<rec.getmeta('Filesize')) 
      {rec.nextblock(n=1,index=-1)};
  
      # processing
      recAntennaSignals:=[=];
      recAntennaSignals.antennaSignals:=datablock;
#printrec(recAntennaSignals)      
      recSpectrum:=DynamicSpectrumCLI->processDatablock(recAntennaSignals);
#printrec(recSpectrum)
      spectrum[block,]:=recSpectrum.spectrum;
      powerAnt[block,]:=recSpectrum.power;
      if (calcDifferential && block>1) {diffSpectrum[block-1,]:=recSpectrum.diffSpectrum};
  
      if (is_record(blockbar)) {blockbar.update(block)};
    } # end for blocks
    if (is_record(blockbar)) {blockbar.done()};
  
    # channeling #
#if (shape(pixarray)[2]!=chans) {pixarray:=array(0.,numfiles*blockinfo[2],chans)};
#print start,block,chans
#print shape(pixarray)
    for (block in 1:blockinfo[2])
    {pixarray[start+block,]:=channels(spectrum[block,],bandsel=bandsel,chans=chans)};    
#print shape(pixarray),max(pixarray)
       
    if (is_record(filebar)) {filebar.update(filenum)};
  } # end of files
  if (is_record(filebar)) {filebar.done()};

  # print #
  print ''
  print 'DSCPP PIXARRAY:',shape(pixarray),max(pixarray)
  print ''

  # return #
  return pixarray

} # end function
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: fftshift
#----------------------------------------------------------------------
#Type: function
#Doc:  beamforming via phase gradients in frequency domain
#Par:  files     = 'example.event' - filenames
#Par:  ants      = 1:8             - antenna selection, ants=-1 -> all antennas
#Par:  bandlim   = [40,80]         - frequency band limitations
#Par:  chans     = 4000            - frequency channels, chans=-1 -> max number of chans
#Par:  azmelv    = [0,90]          - direction for beam (delays)
#Par:  blockinfo = [1,-1,2^16]     - [first block,number of blocks,blocksize]
#Par:  sep       = F               - T = space between blocks of files
#Ret:  ary       =                 - return array with beamformed data
#Ref:  input_event(), channels(), sundir(), antdelays()
#----------------------------------------------------------------------
#<!- Andreas Nigl @ 24.07.03 @ MPIfR ->
#----------------------------------------------------------------------
#sfft:=beamc(data,[-1,2,-6,-2,-13,-4,7,11],1:8,len(data.get('Frequency',1)),40e6,40e6);
  beamc:=function(ref rec,delay,ants,fftlen,bandwidth,freqmin)
  {
    # crosscorrelations #
    nants:=len(ants);
    nbase:=1/2*nants*(nants-1); # N_base = 1/2 * N_ants * (N_ants -1)
    scross:=array(0.,fftlen);
    grad:=array(0.,nants,fftlen);

    # phasegradients #
    fak:=1i*2*pi;
    for (ant in 1:nants)
    {grad[ant,]:=exp(fak*rec.get('Frequency',ant)*delay[ant])};

    # beamforming #
    for (sec in 2:nants) # for all baselines NBASE
    { for (fst in 1:(nants-1))
    { if (!(fst==sec)&(fst<sec))
    {
      # for antennas without failure in datas
      if (rec.flag()[ants[fst]]|rec.flag()[ants[sec]])
      {
        sfft1:=rec.get('CalFFT',fst)*grad[fst,];
        sfft2:=rec.get('CalFFT',sec)*grad[sec,];
        cross:=sfft1*conj(sfft2); #crosscorrelation
        across:=abs(cross);
        scross+:=sqrt(across)*
                 exp(sign(imag(cross))*acos(real(cross)/across));
      } else {nbase-:=1}
    };}} # end baselines

    # mean value #
    scross/:=nbase;

    # return #
    return scross;
  }
#----------------------------------------------------------------------
#sfft:=beam(data,[-1,2,-6,-2,-13,-4,7,11],1:8,data.get('Frequency'));
#pixarray[start+block,]:=channels(ary,bandsel=bandsel,chans=chans);
  beam:=function(ref rec,delay,ants,bandsel)
  {
#print data.len,delay,ants,sum(bandsel==T);
    # phaseshifting #
    nants:=len(ants);
    fftlen:=len(bandsel);
    sfft:=array(0.,fftlen);
#print len(sfft),len(rec.get('CalFFT',ants[1])),len(rec.get('Frequency',ants[1]));
    #for (ant in 1:nants) # for antenna preselection
    for (ant in 1:nants) # for dynspec antenna selection
    {
#print ants[ant];
      if (rec.flag()[ants[ant]]) # for antennas without failure in data
      {
        grad:=exp(1i*2*pi*rec.get('Frequency',ants[ant])*delay[ant]);
#print len(grad),max(grad);
        sfft+:=rec.get('CalFFT',ants[ant])*grad;
#print len(sfft),max(sfft);
      } else {nants-:=1}
    } # end ant

    # mean value #
    sfft/:=nants;
#print len(sfft),max(sfft);

    # return #
    return sfft;
  }
#----------------------------------------------------------------------
#sfft:=beamlim(data,[-1,2,-6,-2,-13,-4,7,11],1:8,array(T,len(data.get('Frequency'))))
#pixarray[start+block,]:=channels(ary,bandsel=0,chans=chans)
  beamlim:=function(ref rec,delay,ants,bandsel)
  {
#print data.len,delay,ants,sum(bandsel==T);
    # phaseshifting #
    nants:=len(ants);
    fftlen:=sum(bandsel==T);
    sfft:=array(0.,fftlen);
#print len(sfft),len(rec.get('CalFFT',ants[1])[bandsel]),len(rec.get('Frequency',ants[1])[bandsel]);
    #for (ant in 1:nants) # for antenna preselection
    for (ant in 1:nants) # for dynspec antenna selection
    {
#print ants[ant]
#print rec.flag() 
      #!!!#if (rec.flag()[ants[ant]]&&rec.head('Observatory')!='LOPES') # for antennas without failure in data
      #{
        grad:=exp(1i*2*pi*rec.get('Frequency',ants[ant])[bandsel]*delay[ant]);
#print len(grad),max(grad),grad[1:10]
        sfft+:=rec.get('CalFFT',ants[ant])[bandsel]*grad;
#print len(sfft),max(sfft),sfft[1:10]
      #} else {nants-:=1} # endif flag
    } # end ant

    # mean value #
    sfft/:=nants;
#print len(sfft),max(sfft),sfft[1:10]

    # return #
    return sfft;
  }
#----------------------------------------------------------------------
fftshift:=function(
  files='',events=F,ants=-1,bandlim=[40,80],chans=4000,
  direction='',azmelv=[0,90],offset=0,blockinfo=[1,-1,2^16],han=F,sep=F,lim=T
)
{
  # Print
  if (lim) {print 'FFTSHIFT: fast & direct band limit extraction'}
  
  # Data #
  if (events) {input_event(data,files[1])};
  if (ants==-1) {ants:=data.selant()}; #1:data.len}
print 'fftshift: ants:',ants
  #SetBlock(data,n=blockinfo[1],blocksize=blockinfo[3],offset=offset) #!!!
  data.setblock(n=blockinfo[1],blocksize=blockinfo[3],offset=offset,index=ants);

  # Freqsel #
  freq:=data.get('Frequency')
  frequnit:=data.headf('FrequencyUnit')
  bandsel:=freq>=bandlim[1]*frequnit&freq<=bandlim[2]*frequnit
  fftlen:=sum(bandsel==T);
#print len(freq),bandlim,sum(bandsel==T);
  global freqsel
  freqsel:=freq[bandsel]
  if (lim) {freqsel:=channels(freqsel,bandsel=0,chans=chans)}
  else {freqsel:=channels(freqsel,bandsel=bandsel,chans=chans)};

  # Parameter #
  if (chans==-1) {chans:=fftlen};
  refant:=data.get('RefAnt');
  antpos:=CalcAntPos(data,ants,refant,1);
  samplerateunit:=data.headf('SamplerateUnit');
  samplerate:=data.headf('Samplerate')*samplerateunit;
  numfiles:=len(files);

  # Blocks #
  if (blockinfo[2]==-1) {
print 'Filesize:',data.getmeta('Filesize')
print 'FileBlocksize:',data.getmeta('FileBlocksize')
print 'Blocksize:',data.getmeta('Blocksize')
    filesize:=data.getmeta('Filesize')
    blockinfo[2]:=ceil(filesize/blockinfo[3])
  }
#print blockinfo,ants;

  # Print #
  print ''
  print 'BEAMFORMING...'
  print 'first block:',blockinfo[1]
  print 'number of blocks:',blockinfo[2]
  print 'blocksize:',blockinfo[3]
  print ''
  print '[[X]] File'
  print ' [X]  Block'
  print ''

  # Preparation #
  global pixarray
print 'FFTSHIFT PIXARRAY:',shape(pixarray),max(pixarray);

  # Beamforming #
  filenum:=0;
  for (file in files) # for number of event-files
  { filenum+:=1;

    # Event #
    if (events) {
      input_event(data,file); nants:=data.len;
      while (max(ants)>nants) {ants:=ants[ants!=max(ants)]}; #selectioncorrection
      nants:=len(ants);
    }; # end events

    # Flags #
    print '';
    print(spaste('[[',as_string(filenum),']] ',spaste(as_string(data.flag())),' ',files[filenum]));
    
    # Direction #
    sdir.obs:=data.head('Observatory'); sdir.name:=direction;
    sdir.azm:=azmelv[1]; sdir.elv:=azmelv[2];
    if (sdir.name!='') {
      print 'AZEL:',sdir.name;
      date:=files[filenum]~globalconst.FilenameExtractor;
      sdir:=sourcedir(rec=data,obs=sdir.obs,direction=sdir.name,date=date,disp=T);
    };

    # Delays #
    delays:=antdelays(sdir.azm,sdir.elv,antpos);
    print 'delays:',round(delays*samplerate*100)/100;

    # Blocks #
    #SetBlock(data,n=blockinfo[1],blocksize=blockinfo[3],offset=offset) #!!!
    data.setblock(n=blockinfo[1],blocksize=blockinfo[3],offset=offset,index=ants)
    if (blockinfo[2]>1) {
      if (sep) {start:=(filenum-1)*(blockinfo[2]+1)}
      else {start:=(filenum-1)*blockinfo[2]}} # some blocks per file
    else {start:=filenum-1}; # one block per file
    if (blockinfo[2]>1) {
      bar:=progress(1,blockinfo[2],'Calculating Dynamic Spectrum over Blocks')
      bar.activate()
    } else {bar:=F};

    # Beamfiles #
    secdir:='beamfiles/'; if (!fexist(secdir)) {shell(spaste('mkdir ',secdir))};

    for (block in 1:blockinfo[2])
    {
      # Print #
      print spaste('[',block,']');

#write_value(data.get('CalFFT'),'calfft.sos')
      # Hanning #
      if (han!=0) {data.putflag(hanning(blockinfo[3],alpha=han),'Voltage',-1)}
#write_value(data.get('CalFFT'),'hamming.sos')

      # Phaseshifting #
#print len(delays),len(ants),sum(bandsel==T);
      if (lim) {ary:=beamlim(data,delays,ants,bandsel)} #for bandlim faster
      else {ary:=beam(data,delays,ants,bandsel)}; #for whole band faster
#print len(ary),max(ary);

      # Save # (intermediate saving of beam data in frequency domain)
      #write_value(ary,spaste(secdir,'/',file~globalconst.FilenameExtractor,'_',block,'.sos'));

      power:=F;
      # fft2pwr # !!!
      if (power) {ary:=abs(ary)^2};

      noise:=F;
      # fft2noise #
      #data.put(F,'Doshift',-1);
      if (noise) {ary:=dataconv(data,'CalFFT',ary,'NoiseT')};
#print len(ary),max(ary);

      pwr:=F;
      # pwr2noise #
      if (pwr) {ary:=dataconv(data,'Power',ary,'NoiseT')};
#print len(ary),max(ary);

      # frequency channels #
#print start,block,chans;
#print shape(pixarray);
      if (lim) {pixarray[start+block,]:=channels(ary,bandsel=0,chans=chans)}
      else {pixarray[start+block,]:=channels(ary,bandsel=bandsel,chans=chans)};

      #data.nextblock(n=1,index=-1)
      if ((data.getmeta('Offset')+data.getmeta('Blocksize'))<data.getmeta('Filesize')) 
      {data.nextblock(n=1,index=-1)};
#print 'nextblock'
      if (is_record(bar)) {bar.update(block)};
    } # end of blocks
    if (is_record(bar)) {bar.done()};

  } # end of files

  # Delete # (intermediate saved files with beam data in frequency domain)
  dir:='beamfiles/'; if (fexist(dir)) {shell(spaste('rm -R ',dir))};

  # Print #
  print ''
  print 'FFTSHIFT PIXARRAY:',shape(pixarray),max(pixarray)
  print ''

  # Return #
  return pixarray
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: meanpwr
#----------------------------------------------------------------------
#Type: function
#Doc:  Dynamic spectrum computation by averaging over antenna power
#Par:  files = - filenames
#Par:  chans = - frequency channels
#Ret:  pixarray
#Ref:  input_event(), channels(), dataconv()
#Example: include 'dynspec.g'; tic(); pix:=meanpwr(files='example.event',events=T,ants=-1,bandlim=[40,80],chans=4000,blockinfo=[1,-1,2^12],sep=F,lim=T); toc()
#----------------------------------------------------------------------
#<!- Andreas Nigl @ 07.11.03 @ KUN ->
#----------------------------------------------------------------------
meanpwr:=function(files='',events=F,ants=-1,bandlim=[40,80],chans=4000,blockinfo=[1,-1,2^16],offset=0,sep=F,lim=T,pwr=F,han=0)
{
  # Print
  if (lim) {print 'MEANPWR: fast & direct band limit extraction'}
  
  # Data #
  if (events) {input_event(data,files[1])};

  # Antenna Selection #
  if (ants==-1) {ants:=data.selant()}
  else {data.select(-1,F); for (ant in ants) {if (ant<=max(data.ant())) {data.select(ant,T)};}};
  ants:=data.selant(); nants:=len(ants)
print 'MEANPWR: antenna selection:',ants,nants
  
  # Blockinfo #
  data.setblock(n=blockinfo[1],blocksize=blockinfo[3],offset=offset,index=ants);
  #SetBlock(data,n=blockinfo[1],blocksize=blockinfo[3],offset=offset) #!!!  

  # Freqsel #
  freq:=data.get('Frequency');
  frequnit:=data.headf('FrequencyUnit');
  bandlim:=[freq[valpos(freq,bandlim[1]*frequnit,big=F)],freq[valpos(freq,bandlim[2]*frequnit,big=T)]]
  bandsel:=freq>=bandlim[1]&freq<=bandlim[2];
  fftlen:=sum(bandsel==T);
#print len(freq),bandlim,sum(bandsel==T);
  global freqsel
  freqsel:=freq[bandsel]
  if (lim) {freqsel:=channels(freqsel,bandsel=0,chans=chans)}
  else {freqsel:=channels(freqsel,bandsel=bandsel,chans=chans)};

  # Parameter #
  if (chans==-1) {chans:=fftlen};
  numfiles:=len(files);

  # Blocks #
#print blockinfo
  if (blockinfo[2]==-1) {
    filesize:=data.getmeta('Filesize');
    blockinfo[2]:=ceil(filesize/blockinfo[3]);
  }
#print blockinfo

  # Print #
  print ''
  print 'SUMMATION...'
  print 'first block:',blockinfo[1]
  if (blockinfo[2]==-1) {print 'number of blocks: all'} else {print 'number of blocks:',blockinfo[2]};
  print 'blocksize:',blockinfo[3]
  print ''
  print '[[X]] File'
  print ' [X]  Block'
  print ''

  # Preparation #
  global pixarray;
print 'MEANPWR PIXARRAY:',shape(pixarray),max(pixarray);

  # Summing #
  filenum:=0;
  for (file in files) # for number of event-files
  { filenum+:=1;

    # Event #
    if (events) {input_event(data,file)}; # end events

    # Flags #
    if (blockinfo[2]>1) {print ''};
    print(spaste('[[',as_string(filenum),']] ',spaste(as_string(data.flag())),' ',files[filenum]));
    
    # Blocks #
    data.setblock(n=blockinfo[1],blocksize=blockinfo[3],offset=offset,index=ants)
    #SetBlock(data,n=blockinfo[1],blocksize=blockinfo[3],offset=offset) #!!!
    if (!events) {data.setphase()};
    if (blockinfo[2]>1) {
      if (sep) {start:=(filenum-1)*(blockinfo[2]+1)}
      else {start:=(filenum-1)*blockinfo[2]}} # some blocks per file
    else {start:=filenum-1}; # one block per file
     if (blockinfo[2]>1) {
       bar:=progress(1,blockinfo[2],'Calculating Dynamic Spectrum over Blocks')
       bar.activate()
     } else {bar:=F};
    for (block in 1:blockinfo[2])
    {
      # Blocknumber #
      if (blockinfo[2]>1) {print spaste('[',block,']')};
      # Hanning #
      if (han!=0) {data.putflag(hanning(blockinfo[3],alpha=han),'Voltage',ants)};
      # Sum #
      spec:=array(0.,fftlen);
      for (ant in 1:nants)
      {
        #if (data.flag()[ants[ant]]) # for antennas without failure in data
        #{
          if (pwr) {
            if (lim) {spec+:=abs(data.get('FFT',ants[ant])[bandsel])^2}
            else {spec+:=abs(data.get('FFT',ants[ant]))^2};
          } else {
            if (lim) {spec+:=data.get('NoiseT',ants[ant])[bandsel]} #!!!# 'NoiseT' (Kelvin)
            else {spec+:=data.get('NoiseT',ants[ant])};
          };
#print filenum,block,blockinfo[2],blockinfo[3],data.len,ants[ant],len(spec),max(spec);
        #} else {nants-:=1};
      } # end ants
      # Averageing #
      spec/:=nants;
#print 'MEANPWR SPECTRUM:',shape(spec),max(spec);

      # frequency channels #
#print start,block,chans;
#print shape(pixarray);
      if (lim) {pixarray[start+block,]:=channels(spec,bandsel=0,chans=chans)}
      else {pixarray[start+block,]:=channels(spec,bandsel=bandsel,chans=chans)};
#print shape(pixarray),max(pixarray);

      #data.nextblock(n=1,index=-1)
      if ((data.getmeta('Offset')+data.getmeta('Blocksize'))<data.getmeta('Filesize'))
      {data.nextblock(n=1,index=-1)};
      if (is_record(bar)) {bar.update(block)};
    } # end of blocks
    if (is_record(bar)) {bar.done()};

  } # end of files

  # Print #
  print ''
  print 'MEANPWR PIXARRAY:',shape(pixarray),max(pixarray)
  print ''

  # Return #
  return pixarray
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: timedel
#----------------------------------------------------------------------
#Type: function
#Doc:  beamforming via delays in time domain
#Par:  files = - filenames
#Par:  chans = - frequency channels
#Ret:  noise
#Ref:  input_event(), channels(), dataconv(), sundir(), antdelays()
#----------------------------------------------------------------------
#<!- Andreas Nigl @ 24.07.03 @ MPIfR ->
#----------------------------------------------------------------------
timedel:=function(
  files='',events=F,ants=-1,bandlim=[40,80],chans=4000,
  direction='',azmelv=[0,90],blockinfo=[1,-1,2^16],sep=F,lim=T
)
{
  # Print
  if (lim) {print 'TIMEDEL: fast & direct band limit extraction'}
  
  # Data #
  if (events) {input_event(data,files[1])};
  if (ants==-1) {ants:=data.selant()} #1:data.len}
print 'timedel: ants:',ants
  #SetBlock(data,n=blockinfo[1],blocksize=blockinfo[3]) #!!!
  data.setblock(n=blockinfo[1],blocksize=blockinfo[3],index=ants);

  # Freqsel #
  freq:=data.get('Frequency');
  frequnit:=data.headf('FrequencyUnit');
  bandsel:=freq>=bandlim[1]*frequnit&freq<=bandlim[2]*frequnit;
  fftlen:=sum(bandsel==T);
#print len(freq),bandlim,sum(bandsel==T);
  global freqsel
  freqsel:=freq[bandsel]
  if (lim) {freqsel:=channels(freqsel,bandsel=0,chans=chans)}
  else {freqsel:=channels(freqsel,bandsel=bandsel,chans=chans)};

  # Parameter #
  if (chans==-1) {chans:=fftlen};
  refant:=data.get('RefAnt');
  antpos:=CalcAntPos(data,ants,refant,1);
  samplerateunit:=data.headf('SamplerateUnit');
  samplerate:=data.headf('Samplerate')*samplerateunit;
  numfiles:=len(files);

  # Blocks #
  if (blockinfo[2]==-1) {
print 'Filesize:',data.getmeta('Filesize')
print 'FileBlocksize:',data.getmeta('FileBlocksize')
print 'Blocksize:',data.getmeta('Blocksize')
    filesize:=data.getmeta('Filesize')
    blockinfo[2]:=ceil(filesize/blockinfo[3])
  }
#print blockinfo,ants;

  # Print #
  print ''
  print 'BEAMFORMING...'
  print ''
  print 'first block:',blockinfo[1]
  print 'number of blocks:',blockinfo[2]
  print 'blocksize:',blockinfo[3]
  print ''
  print '[[X]] File'
  print ' [X]  Block'
  print ''

  # Preparation #
  global pixarray;
print 'TIMEDEL PIXARRAY:',shape(pixarray),max(pixarray);

  # Beamforming #
  filenum:=0;
  for (file in files) # for number of event-files
  { filenum+:=1;

    # Event #
    if (events) {
      input_event(data,file); nants:=data.len;
      while (max(ants)>nants) {ants:=ants[ants!=max(ants)]}; #selectioncorrection
      nants:=len(ants);
    }; # end events

    # Flags #
    print '';
    print(spaste('[[',as_string(filenum),']] ',spaste(as_string(data.flag())),' ',files[filenum]));
    
    # Sun # !!DIR!!
    direction.azm:=azmelv[1]; direction.elv:=azmelv[2];
    if ((direction.azm==0)&(direction.elv==0)) {
      print 'AZEL: SUN';
      direction:=sundir(rec=data,date=files[filenum]~globalconst.FilenameExtractor,disp=T);
    };

    # Delays #
    delays:=round(antdelays(direction.azm,direction.elv,antpos)*samplerate);
    print 'delays:',round(delays*samplerate*100)/100;
    maxdelays:=max(max(delays),-min(delays));
    mindelays:=max(max(delays),-min(delays));

    # Blocks #
    #SetBlock(data,n=blockinfo[1],blocksize=blockinfo[3]) #!!!    
    data.setblock(n=blockinfo[1],blocksize=blockinfo[3],index=ants)
    if (blockinfo[2]>1) {
      if (sep) {start:=(filenum-1)*(blockinfo[2]+1)}
      else {start:=(filenum-1)*blockinfo[2]}} # some blocks per file
    else {start:=filenum-1}; # one block per file
    if (blockinfo[2]>1) {
      bar:=progress(1,blockinfo[2],'Calculating Dynamic Spectrum over Blocks')
      bar.activate()
    } else {bar:=F};
    for (block in 1:blockinfo[2])
    {
print spaste('[',block,']');

      # Timeshifting #
#print blockinfo[3]-maxdelays-mindelays;
      ary:=array(0.,blockinfo[3]-maxdelays-mindelays); # time signal array
#print shape(ary);
      for (ant in 1:nants) # for number of antennas
      {
#print 1+maxdelays+delays[ants[ant]],blockinfo[3]-mindelays+delays[ants[ant]];
        if (data.flag()[ants[ant]]) { # for antennas without failure in data
          ary+:=data.get('f(x)',ant,start=1+maxdelays+delays[ants[ant]],
                                    end=blockinfo[3]-mindelays+delays[ants[ant]])
        } else {nants-:=1};
      }; # end of ants
      # Averageing #
      ary/:=nants;
#print 'TIMEDEL ARY:',shape(ary),max(ary);

      noise:=F;
      # f(x)2noise #
      if (noise) {ary:=dataconv(data,'f(x)',ary,'NoiseT')};

      # frequency channels #
#print start,block,chans;
#print shape(pixarray);
      pixarray[start+block,]:=ary[1:fftlen]; #!!!
      #pixarray[start+block,]:=channels(ary,bandsel=bandsel,chans=chans);
#print shape(pixarray),max(pixarray);

      if ((data.getmeta('Offset')+data.getmeta('Blocksize'))<data.getmeta('Filesize')) 
      {data.nextblock(n=1,index=-1)};
      if (is_record(bar)) {bar.update(block)};
    } # end of blocks
    if (is_record(bar)) {bar.done()};

  } # end of files

  # Print #
  print ''
  print 'TIMEDEL PIXARRAY:',shape(pixarray),max(pixarray)
  print ''

  # Return #
  return pixarray
}
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: specs
#----------------------------------------------------------------------
#Type: Function
#Doc:  plots spectracs for a number of events
#Par:  eventdir  = - event-files directory
#Par:  efirst    = - first file from directory
#Par:  elast    = - last file from directory
#Par:  flag       = - switch to save mean spec plots
#----------------------------------------------------------------------
#<!- Andreas Nigl @ 05.08.03 @ MPIfR ->
#----------------------------------------------------------------------
#-#ds:=specs(events=T,eventdir=spaste(workdir,'lopes-040330-f1501/'),efirst=1,elast=100,ants=-1,azmelv=[0,90],method=2,blockinfo=[1,-1,2^16],chans=0,bandlim=[43.6,76.1],disp=T,ps=T);
#-#ds:=specs(name='test',events=T,eventdir=spaste(datadir,'2004/03/30/'),efirst=1540,elast=1540,ants=-1,azmelv=[0,0],method=2,blockinfo=[1,-1,2^15],chans=-1,bandlim=[50,70],disp=T,ps=T,bright=0.01);
#-#ds:=specs(name='test',events=T,eventdir='',efirst=1,elast=1,ants=-1,direction='sun',method=2,blockinfo=[1,-1,2^15],chans=-1,bandlim=[40,80],disp=T,ps=T,bright=0.01);
specs:=function
(
 name='spectra',
 events=T,
 eventdir=datadir,
 efirst=1, elast=0,
 ants=-1,
 direction='',
 azmelv=[0,90],
 method=2,
 blockinfo=[1,-1,2^16],
 chans=4000,
 bandlim=[43.6,76.1],
 xaxis=F,disp=T,ps=T,
 bright=50000
)
{ if (!(disp)) {ps:=F};

  # Print #
  print '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%';
  print '';
  print 'SPECTRA';
  print '';

  # Fileinfo #
  #print 'Files:';
  #print '';
  if (events) {files:=filesel(eventdir,ext='*.event',first=efirst,last=elast)}
  else {files:=''};

  # Spectra #
  dsmap(
    act=T,
    name=name,
    events=events,
    eventdir=eventdir,efirst=efirst,elast=elast,
    ants=ants,
    direction=direction,azmelv=azmelv,
    method=method,
    blockinfo=blockinfo,chans=chans,bandlim=bandlim,
    xaxis=xaxis,disp=F,ps=ps,
    bright=bright);
  spectra:=pixarray;

  # Blockinfo #
  numspecs:=shape(spectra)[1];
  blockinfo[2]:=numspecs/(elast-efirst+1); #!!!

  # Filtering #
  eventnum:=1;
  for (specnum in 1:numspecs)
  {
    ################

    #spectra[specnum,]:=freqfilter(spectra[specnum,],chans=chans);

    if (1!=1) {
      mean:=0; var:=0;
      runningavgvar(spectra[specnum,],mean,var,500);
      spectra[specnum,]:=mean;
    }

    ################

    # Plot #
    if (disp) {plot(log(spectra[specnum,]))};
    titles[specnum]:=files[eventnum]~globalconst.FilenameExtractor;
    if (specnum/blockinfo[2]==eventnum) {eventnum+:=1};
   }

  # Frequency #
  numchans:=shape(spectra)[2];
#print 'numchans:',numchans
  if (chans==-1|chans>numchans) {chans:=numchans};
  if (events) {input_event(data,files[1])};
  yrange:=[log(min(abs(spectra)))-1,log(max(abs(spectra)))+1];
  frequnit:=data.headf('FrequencyUnit');
  samplerateunit:=data.headf('SamplerateUnit');
  freqmin:=data.getmeta('FrequencyLow',refant);
  bandwidth:=data.headf('Bandwidth')*samplerateunit;
  freq:=data.get('Frequency');
print 'freqlen:',len(freq)
  bandsel:=freq>=bandlim[1]*frequnit&freq<=bandlim[2]*frequnit;
#print 'chans:',chans
  freq:=channels(freq,bandsel=bandsel,chans=chans);
print 'freqlen:',len(freq)

  # Aio #
  if (disp)
  {
    # Displaying #
    pgph:=plotdim(log(spectra),dim=1,xvals=freq/frequnit,yrange=yrange,
                  title=titles,xlab='frequency',ylab='log power');
    if (ps) {writops(handle=pgph,psdir='',name=spaste('noiset_spectra_page_',name))};
  }; #else {

    # Return #
#print 'spectrashape:',shape(spectra)
    ret:=[=]
    ret.spectra:=spectra
    ret.freq:=freq
    return ret;
  #}; #else disp
}
#######################################################################

#######################################################################
#Section: EXTRA #######################################################
#######################################################################

#######################################################################
#----------------------------------------------------------------------
#Name: read_merge
#----------------------------------------------------------------------
read_merge:=function(dir='beamfiles/')
{
  files:=filesel(dir,ext='*.sos',first=1,last=-1,disp=T);
print files;
  filelen:=len(read_value(files[1]));
print filelen;
  pixelmat:=array(0.,len(files),filelen);
  filenum:=0;
  for (file in files) {filenum+:=1;
    pixelmat[filenum,]:=read_value(file);
  }
  return pixelmat;
}
#######################################################################
