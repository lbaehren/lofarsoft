#######################################################################
#----------------------------------------------------------------------
#Name: dsgui
#----------------------------------------------------------------------
#Type: function
#Doc: creates dynamic spectrum tool
#Example: linclude('dstool.g'); dstool(demo=T)
#----------------------------------------------------------------------
#Created: by Andreas Nigl @ 26.05.05 @ RU
#----------------------------------------------------------------------
func dstool(file='',title='',bandlim=[40,80],timelim=[0,24],timeunit='h',
            bright=0,inv=F,pwedg=T,mplots=F,demo=F) {
  
  # demo
  if (demo) {
    #file:='/raid/pv2/LOPES/sun/sos/lopes-050117/mp/ds_050117_0_evx_mp_9ants' # or 15th
    #title:='Dynamic Spectrum @ 17.01.2005' # or 15th
    #bandlim:=[40,80]; timelim:=[0,24]; timeunit:='h'
    ##file:='dstb_jupX3tbE'; sosdir:='jsos/'
    ##title:='[Jupiter Burst] Dynamic Beam Spectrum @ 30.11.2005'
    ##bandlim:=[0,40]; timelim:=[0,2^29/80e6]; timeunit:='s'
    file:='dstb_jup6tbX'; sosdir:='jsos/'
    title:='[Jupiter Burst] Dynamic Beam Spectrum @ 22.06.2004 15:37:34 +/- 3.5s'
    bandlim:=[22,32]; timelim:=[0,2^29/80e6]; timeunit:='s'
    inv:=F # inverse fore- & background color
    pwedg:=T # power wedge (map intensity scale & unit)
    #bright:=-1 # recalculates maximum for colormap
  };

  # title
  print ''; print '## DSTOOL ########################################'; print''
  
  # include
  linclude('dynspectools.g') # valpos()...
  #include 'sun.g' # driftspd()
  
  # parameter
  fsize:=[1024,768] #[1280,954] # standard size [600,450]
  head:='Dynamic Spectrum Tool'
  xlab:=spaste('Time (',timeunit,')')
  ylab:='Frequency (MHz)'
  zlab:='abs FFT'
  comp:=0 # solar burst componnent (0=fundamental,1=harmonic)
  
  # load data
  print 'data loading...'
  tic()
  if (fexist(spaste(file,'.sos'))) {pixels:=as_float(abs(read_value(spaste(file,'.sos'))))}
  else {if (fexist(spaste(sosdir,file,'.pix.sos')))
    {pixels:=as_float(abs(read_value(spaste(sosdir,file,'.pix.sos'))))}
  else {return F};};
  if (fexist(spaste(file,'.sos.timesteps')))
    {timeax:=read_value(spaste(file,'.sos.timesteps')); freqax:=[]}
  else {if (fexist(spaste(sosdir,file,'.axs.sos')))
    {axs:=read_value(spaste(sosdir,file,'.axs.sos')); timeax:=axs.step; freqax:=axs.freq}
  else {timeax:=[]; freqax:=[]};};
  toc(); print''
  # axes & limits
  numsp:=shape(pixels)[1]; numlc:=shape(pixels)[2]
  timeax:=[] # IMAG does not allow individual time steps
  if (len(timeax)==0) {
    timeax:=(0:(numsp-1))*dif(timelim)/numsp+timelim[1]
    timeax+:=mean(dif(timeax))/2
  }; # end timeax
  if (len(freqax)==0) {freqax:=(0:(numlc-1))/(numlc-1)*dif(bandlim)+bandlim[1]};
  timedif:=mean(dif(timeax)); freqdif:=dif(bandlim)/(numlc-1)
printvar(timeax,id=spaste('[dstool.g] timeax(',timeunit,'):'))
printvar(freqax,id='[dstool.g] freqax(MHz):')
  xrange:=[timeax[1],timeax[len(timeax)]]; yrange:=bandlim
  zrange:=range(pixels)
print spaste('[dstool.g] xrange(',timeunit,') yrange(MHz) zrange(absFFT): ',xrange,' ',yrange,' ',zrange)
  
  # gui
  foreground:='black'; background:='white' # basic colors
  plw:=1   # width of line in units of 0.005 inch (0.13 mm) in range 1-201
  pfn:=1   # 1 simple single-stroke font, 2 roman font, 3 italic font, 4. script font
  pch:=1.5 # character height as multiple of 1/40 the height of the view surface
  pls:=1   # 1 (full line), 2 (dashed), 3 (dot-dash-dot-dash), 4 (dotted), 5 (dash-dot-dot-dot)
  pec:=1   # env color (black)
  plc:=2   # line color (red)
  if (inv) {foreground:='white'; background:='black'};
  
  #######################################################################
  # FRAMES #
  
  # tkhold
  tk_hold()
  
  # frames
  dsgui:=[=]
  dsgui.mainframe:=frame(title=head,side='left',relief='flat')
  dsgui.leftmainframe:=frame(dsgui.mainframe,side='top',relief='flat',expand='both')
  dsgui.rightmainframe:=frame(dsgui.mainframe,side='top',relief='flat',expand='both')
  dsgui.rightframe:=frame(dsgui.rightmainframe,side='top',relief='flat',expand='both')
  dsgui.botframe:=frame(dsgui.rightmainframe,side='top',relief='flat',expand='both')
  dsgui.leftframe:=frame(dsgui.leftmainframe,side='top',relief='flat',expand='both')
  dsgui.xframe:=frame(dsgui.leftmainframe,side='bottom',relief='sunken',expand='both') #groove
  
  # tkrelease
  tk_release()
  
  #######################################################################
  # PGPLOT #
  
  # pgplots
  edg:=300
  dsgui.pgr:=pgplot(dsgui.rightframe,foreground=foreground,background=background,width=fsize[1]-edg,height=fsize[2]-edg)
  dsgui.pgb:=pgplot(dsgui.botframe,foreground=foreground,background=background,height=edg) #,width=fsize[1]-edg)
  dsgui.pgl:=pgplot(dsgui.leftframe,foreground=foreground,background=background,width=edg,height=fsize[2]-525)
  #http://aips2.nrao.edu/stable/docs/reference/Glish/node102.html
  
  
  #######################################################################
  # COMMANDS #
  
  # cursor position info
  dsgui.xylabel:=label(dsgui.xframe,width=25)
  dsgui.fpwrlabel:=label(dsgui.xframe,width=25)
  dsgui.tpwrlabel:=label(dsgui.xframe,width=25)
  
  # mouse
  dsgui.pgr->bind('<Motion>','motion')
  dsgui.pgr->bind('<Button-1>','b1')
  dsgui.pgr->bind('<Button-2>','b2')
  dsgui.pgr->bind('<Button-3>','b3')
  
  # keyboard
  dsgui.hlp:=[=]; dsgui.hlp.head:='KEY FUNCTION               MOUSE: (BUTTONS)FUNCTIONS'
  dsgui.pgr->bind('<Key-b>','b'); dsgui.hlp.b:='[b] brightness adjustment (l)input (m)         (r)'
  dsgui.pgr->bind('<Key-s>','s'); dsgui.hlp.s:='[s] sun driftspeed        (l)daq   (m)comp     (r)calc'
  dsgui.pgr->bind('<Key-j>','j'); dsgui.hlp.j:='[j] jupiter driftspeed    (l)daq   (m)drift    (r)calc'
  dsgui.pgr->bind('<Key-z>','z'); dsgui.hlp.z:='[z] zoom tool             (l)in    (m)out      (r)win'
  dsgui.pgr->bind('<Key-e>','e'); dsgui.hlp.e:='[e] event loading         (l)gui   (m)dircor   (r)dirbeam'
  dsgui.pgr->bind('<Key-f>','f'); dsgui.hlp.f:='[f] spectra applications  (l)dirds (m)shortime (r)movie'
  dsgui.pgr->bind('<Key-t>','t'); dsgui.hlp.t:='[t] timebeam calculation  (l)      (m)         (r)'
  dsgui.pgr->bind('<Key-c>','c'); dsgui.hlp.c:='[c] cr pipeline execution (l)      (m)         (r)'
  dsgui.pgr->bind('<Key-p>','p'); dsgui.hlp.p:='[p] plot printing tool    (l)main  (m)spec     (r)light'
  dsgui.pgr->bind('<Key-h>','h'); dsgui.hlp.h:='[h] help & key menu       (l)main  (m)spec     (r)light'
  dsgui.pgr->bind('<Key-x>','x'); dsgui.hlp.x:='[x] closing gui...'
    
  #######################################################################
  # PLOTS #

  # map brightness (plus average spectrum and average lightcurve)
  if (bright==-1|mplots) {for (row in 1:numsp) {pixelsmin[row,]:=as_float(runminc(pixels[row,],bs=10))}};
    
  # bottom plot
  if (demo&&fexist('light.sos')) {light:=read_value('light.sos')}
  else {if (mplots) {light:=mean2d(pixelsmin,dim=1)} #light:=as_float(mean2d(pixelsmin,dim=1))}
  else {light:=pixels[,1]};}; # endif demo
  lczrange:=[0,max(light)*2]
  yrangelc:=lczrange
print '[dstool.g] lightplot (xrange,yrange):',xrange,yrangelc
printvar(light,id='light:')
printvar(timeax,id='time:')
  lightplot(pgb=dsgui.pgb,light=light,timeax=timeax,xrange=xrange,yrangelc=yrangelc,
            plw=1,pfn=1,pch=1.5,pec=1,plc=2,title='Light Curve',xlab=xlab,ylab='Power')
##    dsgui.pgb->slw(plw); dsgui.pgb->scf(pfn); dsgui.pgb->sch(pch)
##    dsgui.pgb->sci(pec)
##    dsgui.pgb->env(xrange[1],xrange[2],yrangelc[1],yrangelc[2],0,0)
##    dsgui.pgb->sci(plc)
##    lightplot(pgb=dsgui.pgb,light=light,timeax=timeax)
##    titlelc:='Light Curve'; ylablc:='Power'; xlablc:=xlab
##    dsgui.pgb->sci(1); dsgui.pgb->lab(toplbl=xlablc,xlbl=ylablc,ylbl=titlelc)
  dsgui.tpwrlabel->text(sprintf('tpwr: %1.2e',sum(light),' arb.units'))
  #dsgui.tpwrlabel->text(spaste('tpwr: ',as_string(sum(round(light*100)/100))))
  
  # right plot (main plot)
  brightmap:=10^(((0:(numlc-1))/(numlc-1)*ln(dif(zrange)/(numlc-1))))
    ##brightmap:=((0:(numlc-1))*dif(zrange)/(numlc-1))
    ##brightmap:=10^(((0:(numlc-1))/(numlc-1)*log(dif(zrange)/(numlc-1))))
  if (demo&&fexist('bright.sos')) {bright:=read_value('bright.sos')}
  else {
    if (bright==-1) {bright:=mean(pixelsmin)+1.5*stddev(pixelsmin)};
    if (bright==0) {bright:=brightmap[ceil(len(brightmap)*.4)]}; # first approach value
print '[dstool.g] map brightness (absFFT):',bright
  }; # endif demo
  if (title=='') {title:=name};
  mainplot(pgr=dsgui.pgr,pixels=pixels,bright=bright,timedif=timedif,bandlim=bandlim,
    numlc=numlc,pwedg=pwedg,xrange=xrange,yrange=yrange,plw=plw,pfn=pfn,pch=pch,
    title=title,xlab=xlab,ylab=ylab,zlab=zlab)
  
  # left plot
  dsgui.pgl->slw(plw); dsgui.pgl->scf(pfn); dsgui.pgl->sch(pch)
  if (demo&&fexist('spec.sos')) {spec:=read_value('spec.sos')}
  else {if (mplots) {spec:=mean2d(pixels,dim=2)} #spec:=as_float(mean2d(pixels,dim=2))}
  else {spec:=pixels[1,]};}; # endif demo
  if (zrange[1]==0|zrange[2]==0) {xrangesp[1]:=0} else {xrangesp:=log(zrange)};
  ##xrangesp:=log(range(spec)) #!!!
print '[dstool.g] specplot (xrange,yrange):',xrangesp,yrange
printvar(log(spec),id='spec:')
printvar(freqax,id='freq:')
  specplot(pgl=dsgui.pgl,spec=log(spec),freqax=freqax,xrangesp=xrangesp,yrange=yrange,
           plw=1,pfn=1,pch=1.5,pec=1,plc=2,title='Spectrum',xlab=ylab,ylab='absFFT')
##    dsgui.pgl->slw(plw); dsgui.pgl->scf(pfn); dsgui.pgl->sch(pch)
##    dsgui.pgl->sci(pec)
##    dsgui.pgl->env(xrangesp[2],xrangesp[1],yrange[1],yrange[2],0,0)
##    dsgui.pgl->sci(plc)
##    specplot(pgl=dsgui.pgl,spec=log(spec),freqax=freqax)
##    titlesp:='Spectrum'; xlabsp:=spaste('log ',zlab); ylabsp:=ylab
##    dsgui.pgl->sci(1); dsgui.pgl->lab(toplbl=xlabsp,xlbl=ylabsp,ylbl=titlesp)
  dsgui.fpwrlabel->text(sprintf('fpwr: %1.2e',sum(spec),' arb.units'))
  #dsgui.fpwrlabel->text(spaste('fpwr: ',as_string(sum(round(spec*100)/100))))
  
  # clean up
  pixelsmin:=[]
  
  #######################################################################
  # WHENEVER #
  
  #----------------------------------------------------------------------
  # MOUSE MOTION #
  whenever dsgui.pgr->motion do { # motion # xy & plot updates
  # cursor #
    xy:=$value.world
    if (xy[1]<=xrange[2]&&xy[1]>=xrange[1]&&xy[2]<=yrange[2]&&xy[2]>=yrange[1])
    {spn:=valpos(timeax,xy[1],T); lcn:=valpos(freqax,xy[2],T); z:=pixels[spn,lcn]} else {z:=0};
    dsgui.xylabel->text(sprintf('xyz: %6.2f %5.2f %1.2e',xy[1],xy[2],z,' arb.units'))
    #dsgui.xylabel->text(spaste(paste('xyz: ',as_string(round([xy,z]*100)/100))))
  # spectrum # (update in the left plot)
    if (xy[1]<=xrange[2]&&xy[1]>=xrange[1]) {
      spn:=valpos(timeax,xy[1],T)
      if (spn<=numsp&&spn>=1) {
print '[dstool.g] specplot (speclen,specmax,freqrange,xrange,yrange):',
  len(spec),max(log(spec)),range(freqax)/1e6,xrangesp,yrange
        specplot(pgl=dsgui.pgl,spec=log(pixels[spn,]),freqax=freqax,xrangesp=xrangesp,yrange=yrange,
                 plw=1,pfn=1,pch=1.5,pec=1,plc=2,title='Spectrum',xlab=ylab,ylab=spaste('log ',zlab))
        ##specplot(pgl=dsgui.pgl,spec=log(pixels[spn,]),freqax=freqax)
#print xrangesp,xy[2]
        dsgui.pgl->line([xrangesp[1],xrangesp[2]],[xy[2],xy[2]]) # horizontal indication line (behind) spectrum
        dsgui.fpwrlabel->text(sprintf('fpwr: %1.2e',sum(spec),' arb.units'))
      }; # endif spn
    }; # endif xy
  # lightcurve # (update in the right plot)
    if (xy[2]<=yrange[2]&&xy[2]>=yrange[1]) {
      lcn:=valpos(freqax,xy[2],T)
      if (lcn<=numlc&&lcn>=1) {
        light:=runminc(pixels[,lcn],bs=3) #light:=pixels[,lcn]
        #light:=as_float(runminc(pixels[,lcn],bs=10))
        if (max(light)>lczrange[2]) {yrangelc:=[0,max(light)]} else {yrangelc:=lczrange};
print '[dstool.g] lightplot (lightlen,lightmax,timerange,xrange,yrange):',
  len(light),max(light),range(timeax),xrange,yrangelc
        lightplot(pgb=dsgui.pgb,light=light,timeax=timeax,xrange=xrange,yrangelc=yrangelc,
                  plw=1,pfn=1,pch=1.5,pec=1,plc=2,title='Light Curve',xlab=xlab,ylab='Power')
        #lightplot(pgb=dsgui.pgb,light=light,timeax=timeax)
#print xy[1],yrangelc
        dsgui.pgb->line([xy[1],xy[1]],[yrangelc[1],yrangelc[2]]) # vertical indication line (behind) light curve
        dsgui.tpwrlabel->text(sprintf('tpwr: %1.2e',sum(light),' arb.units'))
      }; # endif lcn
    }; # endif xy
  } # end whenever motion
  
  # KEY STROKES #
  key:='b' # default key
  whenever dsgui.pgr->b do {key:='b'; print '[dstool.g]',dsgui.hlp[key]}
  whenever dsgui.pgr->s do {key:='s'; print '[dstool.g]',dsgui.hlp[key]; tc:=0; fc:=0}
  whenever dsgui.pgr->j do {key:='j'; print '[dstool.g]',dsgui.hlp[key]; tc:=0; fc:=0}
  whenever dsgui.pgr->z do {key:='z'; print '[dstool.g]',dsgui.hlp[key]}
  whenever dsgui.pgr->e do {key:='e'; print '[dstool.g]',dsgui.hlp[key]; wider plotgui; global globalpar}
  whenever dsgui.pgr->f do {key:='f'; print '[dstool.g]',dsgui.hlp[key]}
  whenever dsgui.pgr->t do {key:='t'; print '[dstool.g]',dsgui.hlp[key]}
  whenever dsgui.pgr->c do {key:='c'; print '[dstool.g]',dsgui.hlp[key]}
  whenever dsgui.pgr->p do {key:='p'; print '[dstool.g]',dsgui.hlp[key]}
  whenever dsgui.pgr->h do {key:='h'; print '[dstool.g]',dsgui.hlp[key]; print ''; print '[DSTOOL HELP]'
    for (name in field_names(dsgui.hlp)) {print dsgui.hlp[name]}; print ''}
  whenever dsgui.pgr->x do {key:='x'; print '[dstool.g]',dsgui.hlp[key]; dsgui:=[] # close gui
    pixels:=[]; freqax:=[]; timeax:=[]; spec:=[]; light:=[]}                       # clean up

  #----------------------------------------------------------------------
  # LEFT MOUSE BUTTON #
      
    whenever dsgui.pgr->b1 do { # left mousebutton
print '[dstool.g] LEFT MOUSE BUTTON:',key # change help for mouse buttons (non-capitals and function help) !!!
  # BRIGHTNESS # input of brightness
      if (key=='b') {
        xy:=$value.world
        if (xy[1]<=xrange[2]&&xy[1]>=xrange[1]&&xy[2]<=yrange[2]&&xy[2]>=yrange[1]) {
          spn:=valpos(timeax,xy[1],T)
print '[dstool.g] spectrumnumber, zrange(absFFT), brange(-),brightness:',
      spn,zrange,range(brightmap),brightmap[spn]
          mainplot(pgr=dsgui.pgr,pixels=pixels,bright=brightmap[spn],timedif=timedif,bandlim=bandlim,
            numlc=numlc,pwedg=pwedg,xrange=xrange,yrange=yrange,plw=plw,pfn=pfn,pch=pch,
            title=title,xlab=xlab,ylab=ylab,zlab=zlab)
#          mainplot(pgr=dsgui.pgr,pixels=pixels,bright=bright,
#                   timedif=timedif,bandlim=bandlim,numlc=numlc,pwedg=pwedg)
        }; # endif xy
      }; # endif key 'b'
  # SUN & JUPITER DRIFT SPEED # data point acquisition
      if (key=='s'|key=='j') {
        xy:=$value.world
        if (xy[1]<=xrange[2]&&xy[1]>=xrange[1]&&xy[2]<=yrange[2]&&xy[2]>=yrange[1]) {
          spn:=valpos(timeax,xy[1],T)
          lcn:=valpos(freqax,xy[2],T)
          if (spn<=numsp&&spn>=1&&lcn<=numlc&&lcn>=1) {
            tcor:=1; if (timeunit=='h') {tcor:=60*60}; # conversion from hours to seconds
            tc+:=1; time_[tc]:=xy[1]*tcor #s
            fc+:=1; freq_[fc]:=xy[2] #MHz
            print time_[tc],freq_[fc]
          }; # endif spn&lcn
        }; # endif xy
      }; # endif key 's' & 'j'
  # ZOOM # zoom in
      if (key=='z') {
        xy:=$value.world
#print 'xy:',xy
#print 'yrange:',yrange
        if (xy[2]<=yrange[2]&&xy[2]>=yrange[1]) {
        ####

        ####
        }; # endif xy
      }; # endif key 'z'
  # EVENTS # load event in GUI
      if (key=='e') {
        xy:=$value.world
        if (xy[1]<=xrange[2]&&xy[1]>=xrange[1]) {
          evn:=valpos(timeax,xy[1],T)
          if (evn<=numsp&&evn>=1) {
print '[dstool.g] evn:',evn
            datum:=split(file~globalconst.FilenameExtractor,'')[4:9] #ds_050117_0_evx_mp_9ants.sos
print '[dstool.g] datum:',spaste(datum)
            evdir:=spaste(datadir,'LOPES/')
            evfile:=evdates(date=spaste(datum),evdir=evdir)[evn] #evdates: if (!evdir==F) {};
            evdir:=spaste(evdir,'20',spaste(datum[1:2]),'/',
                          spaste(datum[3:4]),'/',spaste(datum[5:6]),'/')
            loadfile:=spaste(evdir,evfile,'.event')
print '[dstool.g] file:',loadfile
            if (fexist(loadfile)) {input_event(data,loadfile)}
            else {input_event(data,'example.event'); print '[dstool.g] "example.event" loaded !!!'};
            globalpar.cur_event_dir:=evdir
            globalpar.cur_event_file:=evfile
            globalpar.cur_event_num:=evn
            if (is_defined('plotgui')&&len(plotgui)<2)
            {plotgui.refreshgui()} else {gui()}; # start gui
#if ((!is_defined('plotgui')|len(plotgui)<1)&&globalpar.LOPESGUI) {gui()} # start gui
#else {plotgui.refreshgui()};
          }; # endif spn&lcn
        }; # endif xy
      }; # endif key 'e'
  # SPECTRA # DS for specific direction
      if (key=='f') {
      
      }; # endif key 'f'
  # TIMEBEAM #
      if (key=='t') {
      
      }; # endif key 't'
  # CR PIPELINE #
      if (key=='c') {
      
      }; # endif key 'c'
  # END OF ACTIONS #

    } # end whenever b1 (left mouse button)

  #----------------------------------------------------------------------
  # MIDDLE MOUSE BUTTON #
     
    whenever dsgui.pgr->b2 do { # middle mousebutton
print '[dstool.g] MIDDLE MOUSE BUTTON:',key
  # SUN DRIFT SPEED # change solar burst component
      if (key=='s') {comp:=as_integer(!comp)};
  # JUPITER DRIFT SPEED # drift rate [MHz/s]
      if (key=='j') {
        time:=mean(dif(time_)) #s
        freq:=mean(dif(freq_)) #MHz
        drft:=freq/time #MHz/s
        print '[dstool.g] drift rate: r =',drft,'MHz/s'
      }; # endif key 'j'
  # ZOOM # pull zoom window
      if (key=='z') {
        ####

        ####    
      }; # endif key 'z'
  # EVENTS # correlation for direction
      if (key=='e') {

      }; # endif key 'e'
  # SPECTRA # calculation of shortime variation      
      if (key=='f') {
      
      }; # endif key 'f'
  # TIMEBEAM #
      if (key=='t') {
      
      }; # endif key 't'
  # CR PIPELINE #
      if (key=='c') {
      
      }; # endif key 'c'
  # END OF ACTIONS #

    } # end whenever b2 (middle mouse button)

  #----------------------------------------------------------------------
  # RIGHT MOUSE BUTTON #

    whenever dsgui.pgr->b3 do { # right mouse button
print '[dstool.g] RIGHT MOUSE BUTTON:',key
  # SUN & JUPITER DRIFT SPEED # drift speed calculation
      if (key=='s'&&fexist('sun.g')) {include 'sun.g'; driftspd(time_,freq_,comp); tc:=1; fc:=1};
      if (key=='j'&&fexist('jup.g')) {include 'jup.g'; driftspd(time_,freq_,comp); tc:=1; fc:=1};
  # ZOOM # zoom out
      if (key=='z') {
        ####

        ####    
      }; # endif key 'z'

      if (key=='e') {
      }; # endif key 'e'
  # SPECTRA # movie of spectra in dynamic spectrum
      if (key=='f') {
      }; # endif key 'f'
  # TIMEBEAM #
      if (key=='t') {
      }; # endif key 't'
  # CR PIPELINE #
      if (key=='c') {
      }; # endif key 'c'
  # END OF ACTIONS #

    } # end whenever b3 (right mouse button)
  
  #----------------------------------------------------------------------
  # END MOUSE BUTTONS #
  whenever dsgui.pgr->end do {print 'END'};  

} # end function
###########################################################################################
func mainplot(ref pgr=[],ref pixels=[],bright=[],timedif=[],bandlim=[],numlc=[],pwedg=F,
              xrange=[],yrange=[],plw=1,pfn=1,pch=1.5,title='Dynamic Spectrum',
              xlab='Time (h)',ylab='Frequency (MHz)',zlab='abs FFT') {
  if (!is_agent(pgr)) {return F};
  if (len(pixels)==0|len(bright)==0|len(timedif)==0|len(bandlim)==0|len(numlc)==0|
      len(xrange)==0|len(yrangelc)==0) {return F};
    pgr->slw(plw); pgr->scf(pfn); pgr->sch(pch)
    pgr->ctab([0,.5,1],[0,1,1],[0,.5,1],[0,0,1],1,.5) # orange
    pgr->env(xrange[1],xrange[2],yrange[1],yrange[2],0,0)
  pgr->imag(pixels,0,bright,[0,timedif,0,bandlim[1],0,1/numlc*dif(bandlim)]) #seconds
    pgr->sci(1); pgr->lab(toplbl=xlab,xlbl=ylab,ylbl=title)
  if (pwedg) {pgr->sci(1); pgr->wedg('RI',.5,4,0,bright,zlab)};
}
func lightplot(ref pgb=[],ref light=[],timeax=[],xrange=[],yrangelc=[],plw=1,pfn=1,pch=1.5,pec=1,plc=2,
               title='Light Curve',xlab='Time (h)',ylab='Power') {
  if (!is_agent(pgb)) {return F};
  if (len(light)==0|len(timeax)==0|len(xrange)==0|len(yrangelc)==0) {return F};
    pgb->slw(plw); pgb->scf(pfn); pgb->sch(pch)
    pgb->sci(pec)
    pgb->env(xrange[1],xrange[2],yrangelc[1],yrangelc[2],0,0)
    pgb->sci(plc)
  pgb->line(timeax,light)
    titlelc:=title; ylablc:=ylab; xlablc:=xlab
    pgb->sci(1); pgb->lab(toplbl=xlablc,xlbl=ylablc,ylbl=titlelc)
}
func specplot(ref pgl=[],ref spec=[],freqax=[],xrangesp=[],yrange=[],plw=1,pfn=1,pch=1.5,pec=1,plc=2,
              title='Spectrum',xlab='Frequency (MHz)',ylab='absFFT') {
  if (!is_agent(pgl)) {return F};
  if (len(spec)==0|len(freqax)==0|len(xrangesp)==0|len(yrange)==0) {return F};
    pgl->slw(plw); pgl->scf(pfn); pgl->sch(pch)
    pgl->sci(pec)
    pgl->env(xrangesp[2],xrangesp[1],yrange[1],yrange[2],0,0)
    pgl->sci(plc)
  pgl->line(spec,freqax)
    titlesp:=title; xlabsp:=ylab; ylabsp:=xlab
    pgl->sci(1); pgl->lab(toplbl=xlabsp,xlbl=ylabsp,ylbl=titlesp)
}       
###########################################################################################

# PGPLOT #
if (1!=1) {
dsgui:=[=]
dsgui.mainframe:=frame(title=head,side='left',relief='flat')
dsgui.pgm:=pgplot(dsgui.mainframe,width=fsize[1]-100,height=fsize[2]-100,foreground=foreground,background=background)
#dsgui.pgm->subp(2,2)
#a:=dsgui.pgm->qvp(0)
#b:=dsgui.pgm->qwin()
dsgui.pgm->svp(.07,.9,.1,.9)
dsgui.pgm->swin(1,24,13407.1289,122775.062)
dsgui.pgm->box('ABCNST',.0,0,'ABCNST',.0,0)
dsgui.pgm->line(timeax,light)
}
