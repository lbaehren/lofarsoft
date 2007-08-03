#-------------------------------------------------------------------------------
#
# Various function which can be applied to the data and are available directly 
# through the main GUI.
# <ul>
#   <li><a href="#fitting">Methods for plotting and fitting</a>
#   <li><a href="#GUIs">Graphical frontends for various tools</a>
#   <li><a href="#diskaccess">Access to data stored on disk</a>
#   <li><a href="#stats">Statistical properties of data</a>
# </ul>
#
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#
#Section: <a name="fitting">Methods for plotting and fitting</a>
#
#-------------------------------------------------------------------------------


#-----------------------------------------------------------------------
#Name: lfitfunc
#-----------------------------------------------------------------------
#Type: Function
#Doc: linear function inititalized with the return parameters of a linear fit 
#Ret: float
#
#Par: x            - Array with x-axis values
#Par: res = ''     - result record of linear fit as produced by lfit
#Ref: fitplot,lfit
#-----------------------------------------------------------------------
lfitfunc:=function(x,res,logy=F,logx=F){
if (logy && logx) {return res.coeff[1]+10^res.coeff[2]*x^res.coeff[3]}
else return res.coeff[1]+(res.coeff[2]*x)}


#-----------------------------------------------------------------------
#Name: lfit
#-----------------------------------------------------------------------
#Type: Function
#Doc: Fit a linear function to an array of values 
#Ret: Record with the fit results and compiled function
#
#Par: x1           - Array with x-axis values
#Par: y1           - Array with y-axis values
#Par: err          - Array with y-errors, ignore if err=[]
#Par: logx = F     - Take logarithm of x axis before fitting?
#Par: logy = F     - Take logarithm of y axis before fitting?
#Par: scaled = F   - Use a scaleable function for the linear fit?
#Ref: fitplot,lfitfunc
#-----------------------------------------------------------------------
#Example: x:=-10:10; p0:=0; p1:=2; y:=p0+p1*x; plotdim(y,xvals=x); res:=lfit(x,y); plotdim(res.f(x),xvals=x,add=T)
#-----------------------------------------------------------------------
lfit:=function(x,y,err=[],logy=F,logx=F,scaled=F){
  global dff,dfs

  if (logx && logy) { linear:=F
    fct:=dfs.compiled('p0+10^p1*x0^p2')
    dff.setfunction(fct)
    dff.setparameters([0,log(abs(mean(aryy)/mean(aryx))),1])
    npar:=3
    fixed:=array(F,npar)
    fixed[1]:=T
  } else if (scaled) {
    linear:=F
    fct:=dfs.compiled('p0*(p1+x0)')
    dff.setfunction(fct)
    dff.setparameters([(mean(aryy)/mean(aryx)),0])
    npar:=2
    fixed:=array(F,npar)
  } else { linear:=T
    fct:=dfs.compiled('p0+p1*x0')
    dff.setfunction(fct)
    npar:=2
    fixed:=array(F,npar)
  };

  if (len(err)==0) {
    dff.setdata(x,y);
  } else {
    dff.setdata(x,y,err);
  }
  res.coeff:=dff.fit(linear=linear,fixed=fixed)
  dy:=dff.getresidual()
  res.sigma:=stddev(dy)

  if (len(err)==0&&res.sigma!=0) {
     dff.setdata(x,y,array(res.sigma,len(x)));
     res.coeff:=dff.fit(linear=linear,fixed=fixed)
  }
  fct.setparameters(res.coeff)

  res.coefferrs:=dff.geterror();
  res.chisq:=dff.getchisq();
  res.chisqred:=res.chisq/(len(x)-npar)

  print 'coeffs    =',res.coeff
  print 'coeffserr =',res.coefferrs
  print 'dataerror =',res.sigma
  print 'chisq     =',res.chisq
  print 'chisqred  =',res.chisqred

  res.f:=fct.f
  return res
}

#-----------------------------------------------------------------------
#Name: fitplot
#-----------------------------------------------------------------------
#Type: Function
#Doc: Fit a linear function to an array of values and plot in separate pgplotter
#Ret: Record with the fit results
#
#Par: x1           - Array with x-axis values
#Par: y1           - Array with y-axis values
#Par: err = []      - array with errors for each point, ignore if err=[]
#Par: xlabel = ''  - x-axis Label
#Par: ylabel = ''  - y-axis Label
#Par: logx = F     - Take logarithm of x axis before fitting?
#Par: logy = F     - Take logarithm of y axis before fitting?
#Par: title  = ''  - Tilte of plot window
#Par: psfile = F   - produce a postscript file with plot?
#Par: filename = 'fitplot.ps' - file name of postscript file with plot
#Ref: crresults.g
#-----------------------------------------------------------------------
fitplot:=function(x,y,err=[],xlabel='',ylabel='',title='',psfile=F,filename='fitplot.ps',logy=F,logx=F,scaled=F,xrange=[],yrange=[]){
  global lfitfunc
  local xmin,xmax
  res:=lfit(x,y,err,logy=logy,logx=logx,scaled=scaled);

  xmin:=min(x); xmax:=max(x);
  plot (x,y,err=err,xlabel=xlabel,ylabel=ylabel,title=title,lines=F,clear=T,col=4,xrange=xrange,yrange=yrange,logx=logx,logy=logy)
  plot ([xmin,xmax],res.f([xmin,xmax]),lines=T,clear=F,col=2,psfile=psfile,filename=filename,logx=logx,logy=logy)

  return res
}


#-----------------------------------------------------------------------
#Name: fitgaussplot
#-----------------------------------------------------------------------
#Type: Function
#Doc: Fit a Gauss to the first line in plotpanel
#Ret: Record with the fit results
#
#Par: plotgui       - plotgui record
#Par: xn1           - Start of fit region (integer position in plotgui.yary)
#Par: xn1           - End of fit region (integer position in plotgui.yary)
#Par: nfit = 1        - Number of Gaussian lines that were fitted. Only used if nfit=1 for initialization.
#Par: panels = 1       - which plotpanel window to fit
#Par: height = '-1' - starting height of the fit, if height='-1' use maximum
#Par: center = '-1' - starting center of the fit (array position), if center='-1' use location of maximum
#Par: width = 4 - starting width for the fit
#Par: maxiter = 30 - max number of iterations for the fit
#Example:
# x1:=data.getxn(-1.9e-6,'Time') 

# x2:=data.getxn(-1.55e-6,'Time') 

# fitgaussplot(plotgui.aux.aux,x1,x2,nfit=1,width=4,maxiter=30,panels=1) 

# fitgaussplot(plotgui.aux.aux,x1,x2,nfit=1,center=[32628,32640],height=[2,1],width=[4,10],maxiter=30,panels=1 )

#
#Ref: plotpanel
#-----------------------------------------------------------------------
fitgaussplot:=function(ref plotgui,xn1,xn2,panels=[1],nfit=1,height='-1',center='-1',width=4,maxiter=30){
  allresults:=[=]; n:=0;
  for (panel in panels) {
    n+:=1;
    l:=len(plotgui.yary[panel]);
    yary:=plotgui.yary[panel][xn1:xn2];
    xary:=plotgui.xary[panel][xn1:xn2];
    guess:=[=];
    guess.center:=0;
    guess.height  := max_with_location(yary,guess.center);
    if (!is_string(height)) {guess.height:=height};
    if (!is_string(center)) {guess.center:=center-xn1+1};
    guess.width   := width;
    guess.maxiter := maxiter;
    global_gfitter.setstate(guess);
    result := global_gfitter.fit(ind(yary),yary);
    for (j in ind(result.center)) {
     if (result.center[j]>len(xary)-1 || result.center[j]<2 || is_nan(result.center[j])) {
       result.center[j]:=as_integer((xn2-xn1+1)/2);
       result.height[j]:=0;
       result.width[j]:=1;
     }
    }
    maxcenterp:=1
    maxwidth:=max_with_location(result.width,maxcenterp);
    maxcenter:=result.center[maxcenterp];
    if (maxwidth>xn2-xn1+1) {maxwidth:=(xn2-xn1+1); maxcenter:=as_integer((xn2-xn1)/2+1)}
    start:=min(as_integer(maxcenter-7*maxwidth),1);
    end:=max(as_integer(maxcenter+7*maxwidth),xn2-xn1+1);
    if (start+xn1-1<1) start:=2-xn1;
    if (end+xn1-1>l) end:=l-xn1+1;
    result.width_samples:=result.width;
    result.width:=result.width*(xary[floor(result.center)+1]-xary[floor(result.center)])
    result.center_samples:=result.center+xn1-1;
    result.center:=xary[floor(result.center)]+(result.center-floor(result.center))*(xary[floor(result.center)+1]-xary[floor(result.center)])
    wp:=plotgui.whichpanel[panel+plotgui.startpanel-1][1];
    if (nfit==1) val plotgui.fityary[wp]:=array(0.,l);
    for (j in ind(result.center)) {
#      plotgui.fityary[wp][xn1:xn2]:=plotgui.fityary[wp][xn1:xn2]+gaussian1d(ind(yary),result.height[j],result.center_samples[j]-xn1+1,result.width_samples[j]);
      plotgui.fityary[wp][(start+xn1-1):(end+xn1-1)]:=plotgui.fityary[wp][(start+xn1-1):(end+xn1-1)]+gaussian1d([start:end],result.height[j],result.center_samples[j]-xn1+1,result.width_samples[j]);
    }
    plotgui.fitxary[wp]:=plotgui.xary[panel];
    plotgui.plotfit[wp]:=T;
    plotgui.fitresults[wp]:=result;
    print 'plotgui.fitresults[',wp,']: ',result;
    allresults[n]:=result
  }
  return allresults
}

#-----------------------------------------------------------------------
#Name: gen_profile
#-----------------------------------------------------------------------
#Type: Function
#Doc: generate or add to a profile plot record (like the HBOOK profile histograms)
#Ret: record with the profile plot data
#
#Par: x 	- array with x-values (x and y have to have the same length)
#Par: y 	- array with y-values (x and y have to have the same length)
#Par: weight=[]	- array with corresponding weight (only used if of the same length as x,y)
#Par: prec= [=]	- record with the old profile data
#Par: bins= 10	- number of bins (only used if prec is empty)
#Par: eqfill=F	- arrange the bins, so that each has the same number of entries
#
#Example:
#-------------------------------------------------------------------------------
gen_profile := function(x, y, weight=[], prec=[=], bins=10, eqfill=F, logx=F){
  if (len(x) != len(y)){
    print 'profile_plot: x and y array have different length! Aborting!';
    return F;
  }
  if (len(weight) != len(x)) { weight := array(1.,len(x)); }
  if (len(prec)<4) {
    prec := [=];
    prec.nbin := bins;
    prec.Xint := array(0.,bins);
    prec.Xmean := array(0.,bins);
    prec.Yint := array(0.,bins);
    prec.Ysqint := array(0.,bins);
    prec.WEIGHTint := array(0.,bins);
    prec.WEIGHTmax := array(0.,bins);
    prec.npoints := array(0.,bins);
    prec.Yval := array(0.,bins);
    prec.Yerr := array(0.,bins);
    prec.Yerr1 := array(0.,bins);
    prec.Yerr2 := array(0.,bins);
    xrn := range(x);
    xmin:=xrn[1]-(xrn[2]-xrn[1])*0.01;
    xmax:=xrn[2]+(xrn[2]-xrn[1])*0.01;
    if (eqfill){
      xs := sort(x);
      ind := as_integer([1:(bins-1)]*len(x)/bins)+1
      prec.Xlow := array(0.,bins);
      prec.Xhigh := array(0.,bins);
      prec.Xlow[1] := xmin;
      prec.Xlow[2:bins]  := (xs[ind]+xs[ind-1])/2
      prec.Xhigh[1:(bins-1)] := (xs[ind]+xs[ind-1])/2
      prec.Xhigh[bins] := xmax;
    } else if (logx){
      xrn := log(xrn)
      xmin:=xrn[1]-(xrn[2]-xrn[1])*0.01;
      xmax:=xrn[2]+(xrn[2]-xrn[1])*0.01;
      prec.Xlow  := 10^(xmin + (xmax-xmin)/bins*([1:bins]-1))
      prec.Xhigh := 10^(xmin + (xmax-xmin)/bins*[1:bins])
    } else {
      prec.Xlow  := xmin + (xmax-xmin)/bins*([1:bins]-1)
      prec.Xhigh := xmin + (xmax-xmin)/bins*[1:bins]
    }
    prec.Xcenter := (prec.Xlow+prec.Xhigh)/2
  };
  binarr := [1:prec.nbin]
  for (i in seq(len(x))){
    binid := binarr[(x[i]>=prec.Xlow)&(x[i]<prec.Xhigh)];
    if (len(binid)>0){
      binid := binid[1]
      prec.npoints[binid] := prec.npoints[binid]+1;
      prec.Xint[binid] := prec.Xint[binid] + x[i];
      prec.Yint[binid] := prec.Yint[binid] + y[i]*weight[i];
      prec.Ysqint[binid] := prec.Ysqint[binid] + (y[i]^2)*weight[i];
      prec.WEIGHTint[binid] :=prec.WEIGHTint[binid] + weight[i];
      prec.WEIGHTmax[binid] := max(prec.WEIGHTmax[binid],weight[i]);
    };
  };
  for (i in binarr) {
    if (prec.npoints[i]>1){
      prec.Xmean[i] := prec.Xint[i]/prec.npoints[i];
      prec.Yval[i] := prec.Yint[i]/prec.WEIGHTint[i];
      prec.Yerr[i] := sqrt((prec.Ysqint[i]/prec.WEIGHTint[i]-prec.Yval[i]^2)/prec.npoints[i]);
      prec.Yerr1[i] := sqrt((prec.Ysqint[i]/prec.WEIGHTint[i]-prec.Yval[i]^2)/prec.WEIGHTint[i]*prec.WEIGHTmax[i]);
      prec.Yerr2[i] := prec.Yerr1[i]*sqrt(prec.npoints[i]/(prec.npoints[i]-1))
    } else if (prec.npoints[i]==1){
      prec.Xmean[i] := prec.Xint[i];
      prec.Yval[i] := prec.Yint[i]/prec.WEIGHTint[i];
      prec.Yerr[i] := 1/sqrt(prec.WEIGHTint[i]);
      prec.Yerr1[i] := prec.Yerr[i];
      prec.Yerr2[i] := prec.Yerr[i];
    } else {
      prec.Xmean[i] := prec.Xcenter[i];
      prec.Yval[i] := 0.;
      prec.Yerr[i] := 1.E10;
      prec.Yerr1[i] := 1.E10;
      prec.Yerr2[i] := 1.E10;
    };
  };

  return prec;
};
#-----------------------------------------------------------------------
#Name: plot_profile
#-----------------------------------------------------------------------
#Type: Function
#Doc: 
#Ret: F or record with the fit results
#
#Par: prec        	- record with the old profile data
#Par: xlabel = '' 	- x-axis Label
#Par: ylabel = '' 	- y-axis Label
#Par: title  = '' 	- Tilte of plot window
#Par: psfile = F 	- produce a postscript file with plot?
#Par: filename = 'fitplot.ps' - file name of postscript file with plot
#Par: dofit = F 	- fit a line to the data?
#Par: loglog = F 	- plot on double logarithmic axes?
#Par: xmean = F 	- use mean values of x instead of center?
#
#Example:
#-------------------------------------------------------------------------------
plot_profile := function(prec,xlabel='',ylabel='',title='',psfile=F,filename='fitplot.ps',dofit=F,loglog=F,xmean=F){
  res := F;
  
  binsel := prec.npoints > 0;
  Xlow := min(prec.Xlow[binsel]);
  Xhigh := max(prec.Xhigh[binsel]);
  Ylow := min(prec.Yval[binsel]-prec.Yerr[binsel]);
  Yhigh := max(prec.Yval[binsel]+prec.Yerr[binsel]);
  Xmin := Xlow-(Xhigh-Xlow)*0.05;
  Xmax := Xhigh+(Xhigh-Xlow)*0.05;
  Ymin := Ylow-(Yhigh-Ylow)*0.05;
  Ymax := Yhigh+(Yhigh-Ylow)*0.05;
  if (loglog) {
    Xmin := log(Xlow)-(log(Xhigh)-log(Xlow))*0.05;
    Xmax := log(Xhigh)+(log(Xhigh)-log(Xlow))*0.05;
    Ymin := log(Ylow)-(log(Yhigh)-log(Ylow))*0.05;
    Ymax := log(Yhigh)+(log(Yhigh)-log(Ylow))*0.05;
  }
  if (xmean) { Xval:=prec.Xmean[binsel] } else { Xval:=prec.Xcenter[binsel] };
  Yval := prec.Yval[binsel];
  Yerr := prec.Yerr[binsel];

  if (!loglog){
    plot(Xval,Yval,Yerr,col=4,lines=F,clear=T,xlabel=xlabel,
            ylabel=ylabel, psfile=F,xrange=[Xmin,Xmax],yrange=[Ymin,Ymax]);
    pg.errx(prec.Xlow[binsel],prec.Xhigh[binsel],prec.Yval[binsel],1);
  } else {
    plot(Xval,Yval,Yerr,col=4,lines=F,clear=T,xlabel=xlabel,
      ylabel=ylabel,psfile=F,xrange=log([Xlow,Xhigh]),yrange=log([Ylow,Yhigh]),
      logx=T,logy=T);
    pg.errx(log(prec.Xlow[binsel]),log(prec.Xhigh[binsel]),log(prec.Yval[binsel]),1);
  }    

  if (dofit) {
    if (!loglog){
      res:=lfit(Xval,Yval,Yerr);
      plot([Xlow,Xhigh],lfitfunc([Xlow,Xhigh],res),lines=T,clear=F,col=2,psfile=F)  
    } else {
      res:=lfit(Xval,Yval,Yerr,logy=T,logx=T);
      plot(log([Xlow,Xhigh]),log(lfitfunc([Xlow,Xhigh],res,logy=T,logx=T)),lines=T,clear=F,col=2,psfile=F)
    }
  } else { res := F; };

  if (psfile) { 
    pg.postscript(filename)
    print '=> Writing plot to postscript file ',filename
  }

  return res;
};


#-------------------------------------------------------------------------------
#
#Section: <a name="GUIs">Graphical frontends for various tools</a>
#
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: dynspec_gui
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Graphical frontend for the dynamic spectra generation routine "dsmap"
#      (dynspec.g)
#Ref:  dsmap, vector2string
#-------------------------------------------------------------------------------
# GUI #
dynspec_gui := function()
{
  global globalpar;

# MAIN FRAME #
  tk_hold(); # holds window till displayed totaly
  mapgui.main_frame:=frame(title='Dynamic Spectrum Map Gen',width=1000,height=350,tpos='c');

# NAME #

  mapgui.name := SectionFrame (mapgui.main_frame,'Head',F);

  mapgui.mapnameent := SectionEntry (mapgui.name,'Map name ','dynamic_spectrum');

# DATA #

  mapgui.data_frame := SectionFrame (mapgui.main_frame,
                                     'Data I/O',
                                     T);

	#mapgui.proj_frame:=frame(mapgui.data_frame.Contents,side='top',relief='groove',expand='x');
  mapgui.proj_button:=button(mapgui.data_frame.Contents,'LOFAR data from memory',
	       type='check',borderwidth=1,relief='flat',hlthickness=0,anchor='w',fill='x',justify='center');
	mapgui.proj_button->state(F);

  #mapgui.events_frame:=frame(mapgui.data_frame.Contents,side='top',relief='groove',expand='x');
  mapgui.ev_button:=button(mapgui.data_frame.Contents,'LOPES/KASCADE event-file(s)',
	       type='check',borderwidth=1,relief='flat',hlthickness=0,anchor='w',fill='x',justify='center');
	mapgui.ev_button->state(T);

  mapgui.evdir_frame:=frame(mapgui.data_frame.Contents,side='left',expand='x');
  mapgui.evdirlab:=label(mapgui.evdir_frame,'file directory ',anchor='e');
  mapgui.evdirent:=entry(mapgui.evdir_frame,width=30,background='LemonChiffon');
	mapgui.evdirent->insert('');
	mapgui.evdir_frame->disable();
	mapgui.evdir_but:=button(mapgui.data_frame.Contents,'Browse');
	mapgui.evdir_but->disable();

  mapgui.ef_frame:=frame(mapgui.data_frame.Contents,side='left',expand='x');
  mapgui.eflab:=label(mapgui.ef_frame,'first file ',anchor='e');
  mapgui.efent:=entry(mapgui.ef_frame,width=4,background='LemonChiffon');
  mapgui.efent->insert('1')
	mapgui.ef_frame->disable()

  mapgui.el_frame:=frame(mapgui.data_frame.Contents,side='left',expand='x');
  mapgui.ellab:=label(mapgui.el_frame,'last file  ',anchor='e');
  mapgui.elent:=entry(mapgui.el_frame,width=4,background='LemonChiffon');
  mapgui.elent->insert('1')
	mapgui.el_frame->disable()

# BLOCKS #

  Filesize      := data.getmeta('Filesize');
  nofSamples    := as_integer(data.getmeta('Blocksize'));
  nofBlocks     := as_integer(Filesize/nofSamples);

  mapgui.blocks := SectionFrame (mapgui.main_frame,'Blocks',T);

  mapgui.bsent := SectionEntry (mapgui.blocks,'Blocksize [samples] ',
                                plotgui.blocksizeent->get());

  whenever mapgui.bsent.Entry->return do {
    nofSamples := eval(mapgui.bsent.Entry->get());
    mapgui.bsent.Entry->delete('start','end');
    mapgui.bsent.Entry->insert(as_string(nofSamples));
  }

  mapgui.fbent := SectionEntry (mapgui.blocks,'First block processed ',
                                plotgui.blocknumscale->value());

  mapgui.nbent := SectionEntry (mapgui.blocks,'Number of blocks ',nofBlocks);

  whenever mapgui.nbent.Entry->return do {
    nofBlocks := eval(mapgui.nbent.Entry->get());
    mapgui.nbent.Entry->delete('start','end');
    mapgui.nbent.Entry->insert(as_string(nofBlocks));
  }

# BEAMFORMING #

  mapgui.beam_frame := SectionFrame (mapgui.main_frame,
                                     'Beamforming',
                                     T);

  mapgui.mpwr_button:=button(mapgui.beam_frame.Contents,'average in power',
         type='check',borderwidth=1,relief='flat',hlthickness=0,anchor='w',fill='x',justify='center');
  mapgui.mpwr_button->state(F);

  mapgui.time_button:=button(mapgui.beam_frame.Contents,'time domain TIME->FFT ???',
         type='check',borderwidth=1,relief='flat',hlthickness=0,anchor='w',fill='x',justify='center');
  mapgui.time_button->state(F);
  mapgui.time_button->disable();

  mapgui.freq_button:=button(mapgui.beam_frame.Contents,'frequency domain',
         type='check',borderwidth=1,relief='flat',hlthickness=0,anchor='w',fill='x',justify='center');
  mapgui.freq_button->state(T);

  mapgui.freqcpp_button:=button(mapgui.beam_frame.Contents,'frequency domain (cpp)',
         type='check',borderwidth=1,relief='flat',hlthickness=0,anchor='w',fill='x',justify='center');
  mapgui.freqcpp_button->state(F);

  mapgui.cross_button:=button(mapgui.beam_frame.Contents,'cross correlation !TBI!',
         type='check',borderwidth=1,relief='flat',hlthickness=0,anchor='w',fill='x',justify='center');
  mapgui.cross_button->state(F);
  mapgui.cross_button->disable();

  #mapgui.method_frame:=frame(mapgui.beam_frame.Contents,side='left',expand='x');
  #mapgui.methodlab:=label(mapgui.method_frame,'beamforming method (1-time, 2-freq, 3-meanpwr) ',anchor='e');
  #mapgui.methodent:=entry(mapgui.method_frame,width=1,background='LemonChiffon');
  #mapgui.methodent->insert('3')

  mapgui.ants_frame:=frame(mapgui.beam_frame.Contents,side='left',expand='x');
  mapgui.antslab:=label(mapgui.ants_frame,'antenna selection ',anchor='e');
  mapgui.antsent:=entry(mapgui.ants_frame,width=17,background='LemonChiffon');
  mapgui.antsent->insert(vector2string(seq(data.len)[data.select()]));

  whenever mapgui.antsent->return do {
    ant := eval(mapgui.antsent->get());
    mapgui.antsent->delete('start','end');
    mapgui.antsent->insert(vector2string(ant));
  }

# DIRECTION #
  
  mapgui.dir_frame := SectionFrame (mapgui.main_frame,
                                    'Direction',
                                    T);

  mapgui.sun_button := button(mapgui.dir_frame.Contents,
                              'Sun',
                              type='check',
                              borderwidth=1,
                              relief='flat',
                              hlthickness=0,
                              anchor='w',
                              fill='x',
                              justify='center');
  mapgui.sun_button->state(F);
  mapgui.sun_button->disable();

  mapgui.jup_button := button(mapgui.dir_frame.Contents,
                              'Jupiter',
                              type='check',
                              borderwidth=1,
                              relief='flat',
                              hlthickness=0,
                              anchor='w',
                              fill='x',
                              justify='center');
  mapgui.jup_button->state(F);
  mapgui.jup_button->disable();

  mapgui.azel_button := button(mapgui.dir_frame.Contents,
                               'AZEL',
                               type='check',
                               borderwidth=1,
                               relief='flat',
                               hlthickness=0,
                               anchor='w',
                               fill='x',
                               justify='center');
  mapgui.azel_button->state(T);
  mapgui.azel_button->disable();

  #mapgui.azel_frame:=frame(mapgui.dir_frame.Contents,side='left',expand='x');
  #mapgui.azellab:=label(mapgui.azel_frame,'AZEL                    ',anchor='e');
  #mapgui.azelent:=entry(mapgui.azel_frame,width=15,background='LemonChiffon');
  #mapgui.azelent->insert(spaste('[',as_string([plotgui.AZEL[1],plotgui.AZEL[2]]),']'))
  #mapgui.azel_frame->disable()

  mapgui.azm_frame:=frame(mapgui.dir_frame.Contents,side='left',expand='x');
  mapgui.azmlab:=label(mapgui.azm_frame,'Azimuth               ',anchor='e');
  mapgui.azment:=entry(mapgui.azm_frame,width=15,background='LemonChiffon');
  mapgui.azment->insert(as_string(plotgui.AZEL[1]))
  mapgui.azm_frame->disable()

  mapgui.elv_frame:=frame(mapgui.dir_frame.Contents,side='left',expand='x');
  mapgui.elvlab:=label(mapgui.elv_frame,'Elevation             ',anchor='e');
  mapgui.elvent:=entry(mapgui.elv_frame,width=15,background='LemonChiffon');
  mapgui.elvent->insert(as_string(plotgui.AZEL[2]))
  mapgui.elv_frame->disable()

# PARAMETERS #

#  mapgui.band_frame:=frame(mapgui.par_frame.Contents,side='left',expand='x');
#  mapgui.bandlab:=label(mapgui.band_frame.Contents,'frequency band ',anchor='e');
#  mapgui.bandent:=entry(mapgui.band_frame.Contents,width=10,background='LemonChiffon');
#  mapgui.bandent->insert(spaste('[',as_string([plotgui.freqstartent->get(),plotgui.freqstopent->get()]),']'))

  mapgui.par := SectionFrame (mapgui.main_frame,'Parameters',T);

  mapgui.bandlowent := SectionEntry (mapgui.par,'low frequency band limit ',
                                     plotgui.freqstartent->get());

  mapgui.bandhighent := SectionEntry (mapgui.par,'high frequency band limit ',
                                      plotgui.freqstopent->get());

  mapgui.chanent := SectionEntry (mapgui.par,'# frequency channels ',
                                  ceil(as_double(mapgui.bsent.Entry->get())/2)+1);

  mapgui.brient := SectionEntry (mapgui.par,'map brightness ','.01');

# OPTIONS #
  mapgui.opt_frame:=SectionFrame (mapgui.main_frame,'Options',F);

  mapgui.ps_button:=button(mapgui.opt_frame.Contents,'save plot(s) in ps-file(s)',
	       type='check',borderwidth=1,relief='flat',hlthickness=0,anchor='w',fill='x',justify='center');
  mapgui.ps_button->state(F);

# BUTTONS #
  mapgui.start_frame:=frame(mapgui.main_frame,side='left',expand='x');
  mapgui.mapbut:=button(mapgui.start_frame,'Gen Map',background='green');
  mapgui.specbut:=button(mapgui.start_frame,'Gen Specs',background='green');
  mapgui.specbut->state(F);
  mapgui.space_frame:=frame(mapgui.start_frame);
  mapgui.endbut:=button(mapgui.start_frame,'Dismiss',background='orange');

	tk_release(); # draws window

# DEPENDENCIES #
  whenever mapgui.evdir_but->press do
  { # EVENTDIR
    evdir_cbfunc:=function(dirname='') # EVENTDIR
    {
      mapgui.evdirent->delete('start','end');
      mapgui.evdirent->insert(dirname);
    }
    global dcstarted
    #print dcstarted
    #if (!dcstarted){
    dc.setselectcallback(evdir_cbfunc);
    dc.gui(F,all,F);
    dc.show(dir=mapgui.evdirent->get(),show_types='Directory',writestatus=T);
    dcstarted := T;
    #} # end if dcstarted
  }

  whenever mapgui.ev_button->press do { # EVENTFILES
    mapgui.ev_button->state(T)
    mapgui.proj_button->state(F)
		mapgui.ef_frame->enable()
		mapgui.el_frame->enable()
		mapgui.evdir_frame->enable()
		mapgui.evdir_but->enable()
	}
  whenever mapgui.proj_button->press do { # PROJECTFILES
    mapgui.proj_button->state(T)
    mapgui.ev_button->state(F)
		mapgui.ef_frame->disable()
		mapgui.el_frame->disable()
		mapgui.evdir_frame->disable()
		mapgui.evdir_but->disable()
	}
  whenever mapgui.mpwr_button->press do { # MPWR
    mapgui.mpwr_button->state(T)
		mapgui.time_button->state(F)
    mapgui.freq_button->state(F)
    mapgui.freqcpp_button->state(F)
    mapgui.cross_button->state(F)
    mapgui.sun_button->disable()
    mapgui.jup_button->disable()
    mapgui.azel_button->disable()
		#mapgui.azel_frame->disable()
    mapgui.azm_frame->disable()
		mapgui.elv_frame->disable()
	}
  whenever mapgui.time_button->press do { # TIMEBF
    mapgui.mpwr_button->state(F)
		mapgui.time_button->state(T)
    mapgui.freq_button->state(F)
    mapgui.freqcpp_button->state(F)
    mapgui.cross_button->state(F)
    mapgui.sun_button->enable()
    mapgui.jup_button->enable()
    mapgui.azel_button->enable()
		#mapgui.azel_frame->enable()
    mapgui.azm_frame->enable()
		mapgui.elv_frame->enable()
	}
  whenever mapgui.freq_button->press do { # FREQBF
    mapgui.mpwr_button->state(F)
		mapgui.time_button->state(F)
    mapgui.freq_button->state(T)
    mapgui.freqcpp_button->state(F)
    mapgui.cross_button->state(F)
    mapgui.sun_button->enable()
    mapgui.jup_button->enable()
    mapgui.azel_button->enable()
		#mapgui.azel_frame->enable()
    mapgui.azm_frame->enable()
		mapgui.elv_frame->enable()
	}
  whenever mapgui.cross_button->press do { # CROSSBF
    mapgui.mpwr_button->state(F)
		mapgui.time_button->state(F)
    mapgui.freq_button->state(F)
    mapgui.freqcpp_button->state(F)
    mapgui.cross_button->state(T)
    mapgui.sun_button->enable()
    mapgui.jup_button->enable()
    mapgui.azel_button->enable()
		#mapgui.azel_frame->enable()
    mapgui.azm_frame->enable()
		mapgui.elv_frame->enable()
	}
  whenever mapgui.freqcpp_button->press do { # CROSSBF
    mapgui.mpwr_button->state(F)
		mapgui.time_button->state(F)
    mapgui.freq_button->state(F)
    mapgui.freqcpp_button->state(T)
    mapgui.cross_button->state(F)
    mapgui.sun_button->enable()
    mapgui.jup_button->enable()
    mapgui.azel_button->enable()
		#mapgui.azel_frame->enable()
    mapgui.azm_frame->enable()
		mapgui.elv_frame->enable()
	}
  whenever mapgui.sun_button->press do { # SUN
    mapgui.sun_button->state(T)
		mapgui.jup_button->state(F)
    mapgui.azel_button->state(F)
		#mapgui.azel_frame->disable()
    mapgui.azm_frame->disable()
		mapgui.elv_frame->disable()
	}
  whenever mapgui.jup_button->press do { # JUPITER
    mapgui.sun_button->state(F)
		mapgui.jup_button->state(T)
    mapgui.azel_button->state(F)
		#mapgui.azel_frame->disable()
    mapgui.azm_frame->disable()
		mapgui.elv_frame->disable()
	}
  whenever mapgui.azel_button->press do { # AZEL
    mapgui.sun_button->state(F)
		mapgui.jup_button->state(F)
    mapgui.azel_button->state(T)
		#mapgui.azel_frame->enable()
    mapgui.azm_frame->enable()
		mapgui.elv_frame->enable()
	}

# MAPBUTTON #
  whenever mapgui.mapbut->press do
  {
# Beambuttons #
    if (mapgui.mpwr_button->state()) {method:=3};
    if (mapgui.time_button->state()) {method:=1};
    if (mapgui.freq_button->state()) {method:=2};
    if (mapgui.freqcpp_button->state()) {method:=4};
    if (mapgui.cross_button->state()) {method:=5};
# Dirbuttons #
    if (mapgui.sun_button->state()) {direction:='sun'};
    if (mapgui.jup_button->state()) {direction:='jupiter'};
    if (mapgui.azel_button->state()) {direction:=''};

# print #
  if (1!=1) {
    print "[plotpanel-func::dynspec_gui]"
    print " - Map name ................ :",mapgui.mapnameent.Entry->get();
    print " - Eventfiles from directory :",eval(mapgui.ev_button->state());
    print " - File directory .......... :",spaste(mapgui.evdirent->get());
    print " - First file .............. :",eval(mapgui.efent->get());
    print " - Last file ............... :",eval(mapgui.elent->get());
    ###print str2ary(as_string(mapgui.antsent->get()),sep=' ');
    print " - Antenna selection ....... :",eval(mapgui.antsent->get());
    print " - Direction ............... :",direction;
    ###print str2ary(as_string(mapgui.azelent->get()),sep=' ');
    print " - Azimuth ................. :",eval(mapgui.azment->get());
    print " - Elevation ............... :",eval(mapgui.elvent->get());
    print " - Method .................. :",method;
    print " - First block processed ... :",eval(mapgui.fbent.Entry->get());
    print " - Number of blocks ........ :",eval(mapgui.nbent.Entry->get());
    print " - Blocksize [samples] ..... :",eval(as_string(mapgui.bsent.Entry->get()));
    print " - # frequency channels .... :",eval(mapgui.chanent.Entry->get());
    ###print str2ary(mapgui.bandent->get(),sep=' ');
    print " - Lower band limit ........ :",eval(mapgui.bandlowent.Entry->get());
    print " - Upper band limit ........ :",eval(mapgui.bandhighent.Entry->get());
    print " - Map brightness .......... :",eval(mapgui.brient.Entry->get());
    print " - Export postscript ....... :",eval(mapgui.ps_button->state());
  }

# Dsmap #
      #ants=str2ary(as_string(mapgui.antsent->get()),sep=' '),
      #azmelv=str2ary(mapgui.azelent->get(),sep=' '),
      #bandlim=str2ary(mapgui.bandent->get(),sep=' '),
#if (1!=1) { #!!!
    print "[plotpanel-func::dynspec_gui] Calling dsmap ...";
    pix := dsmap (
      name=as_string(mapgui.mapnameent.Entry->get()),
      events=eval(mapgui.ev_button->state()),
      eventdir=spaste(mapgui.evdirent->get()),
      efirst=as_integer(eval(mapgui.efent->get())),
      elast=as_integer(eval(mapgui.elent->get())),
      ants=eval(as_string(mapgui.antsent->get())),
      direction=direction,
      azmelv=[eval(mapgui.azment->get()),eval(mapgui.elvent->get())],
      method=method,
      blockinfo=[eval(mapgui.fbent.Entry->get()),
                 eval(mapgui.nbent.Entry->get()),
                 eval(as_string(mapgui.bsent.Entry->get()))],
      chans=eval(mapgui.chanent.Entry->get()),
      bandlim=[eval(mapgui.bandlowent.Entry->get()),eval(mapgui.bandhighent.Entry->get())],
      bright=eval(mapgui.brient.Entry->get()),
      ps=eval(mapgui.ps_button->state()));
#} #!!!
    print "[plotpanel-func::dynspec_gui] dsmap completed.";
  } # end whenevever mapbut

# SPECBUTTONS #
  whenever mapgui.specbut->press do
  {
# Beambuttons #
		if (mapgui.mpwr_button->state()) {method:=3};
		if (mapgui.time_button->state()) {method:=1};
		if (mapgui.freq_button->state()) {method:=2};
    if (mapgui.freqcpp_button->state()) {method:=4};
		if (mapgui.cross_button->state()) {method:=5};

# Print #
  if (1!=1) {
    print "[plotpanel-func::dynspec_gui]"
    print " - Map name ................ :",mapgui.mapnameent.Entry->get();
    print " - Eventfiles from directory :",eval(mapgui.ev_button->state());
    print " - File directory .......... :",spaste(mapgui.evdirent->get(),'/');
    print " - First file .............. :",eval(mapgui.efent->get());
    print " - Last file ............... :",eval(mapgui.elent->get());
    ###print str2ary(as_string(mapgui.antsent->get()),sep=' ');
    print " - Antenna selection ....... :",eval(mapgui.antsent->get());
    print " - Direction ............... :",direction;
    ###print str2ary(as_string(mapgui.azelent->get()),sep=' ');
    print " - Azimuth ................. :",eval(mapgui.azment->get());
    print " - Elevation ............... :",eval(mapgui.elvent->get());
    print " - Method .................. :",method;
    print " - First block processed ... :",eval(mapgui.fbent.Entry->get());
    print " - Number of blocks ........ :",eval(mapgui.nbent.Entry->get());
    print " - Blocksize [samples] ..... :",eval(as_string(mapgui.bsent.Entry->get()));
    print " - # frequency channels .... :",eval(mapgui.chanent.Entry->get());
    ###print str2ary(mapgui.bandent->get(),sep=' ');
    print " - Lower band limit ........ :",eval(mapgui.bandlowent.Entry->get());
    print " - Upper band limit ........ :",eval(mapgui.bandhighent.Entry->get());
    print " - Map brightness .......... :",eval(mapgui.brient.Entry->get());
    print " - Export postscript ....... :",eval(mapgui.ps_button->state());
  }

# Specs #
#if (1!=1) { #!!!
		spectra:=specs(
      name=mapgui.mapnameent.Entry->get(),
			events=eval(mapgui.ev_button->state()),
      eventdir=spaste(mapgui.evdirent->get(),'/'),
      efirst=eval(mapgui.efent->get()),
			elast=eval(mapgui.elent->get()),
      ants=eval(as_string(mapgui.antsent->get())),
      direction=direction,
      azmelv=[eval(mapgui.azment->get()),eval(mapgui.elvent->get())],
      method=method,
    	blockinfo=[eval(mapgui.fbent.Entry->get()),
			           eval(mapgui.nbent.Entry->get()),
						     eval(as_string(mapgui.bsent.Entry->get()))],
      chans=eval(mapgui.chanent.Entry->get()),
			bandlim=[eval(mapgui.bandlowent.Entry->get()),eval(mapgui.bandhighent.Entry->get())],
			bright=eval(mapgui.brient.Entry->get()),
      pub=eval(mapgui.ps_button->state()),
			disp=T);
#} #!!!
  } # end whenever specbut

  whenever mapgui.endbut->press do
  {
    val mapgui.main_frame:=F;
    val mapgui:=F;
  };
}


#-------------------------------------------------------------------------------
#
#Section: <a name="diskaccess">Access to data stored on disk</a>
#
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: cur_event_dir
#-------------------------------------------------------------------------------
#Type: Variable
#Doc:  Initialize cur_event_dir at startup
#-------------------------------------------------------------------------------

cur_event_dir := globalpar.cur_event_dir;
if (globalpar.cur_event_dir) {dcstarted := F;} else {dcstarted := T;};

#-------------------------------------------------------------------------------
#Name: readevfiles
#-------------------------------------------------------------------------------
#Type: function
#Doc:  loads event-files via the catalog-tool
#Ref:  input_event(data,filename), datawatch-client, catalog-tool
#-------------------------------------------------------------------------------
# updated @ 050603 (an)
#-------------------------------------------------------------------------------

  readevfiles_cbfunc:=function(filename)
  {
    global globalpar
    cur_event_dir:=globalpar.cur_event_dir
    cur_event_file:=globalpar.cur_event_file
    cur_event_num:=globalpar.cur_event_num
    if (!is_unset(filename)) {
      if (stat(filename)[1]=='regular') {
        cur_event_file:=filename~globalconst.FilenameExtractor
        cur_event_dir:=filename~globalconst.DirExtractor
        cur_event_num:=0; i:=0
        #files:=shell(spaste('ls ',cur_event_dir))
        files:=filesel(cur_event_dir,ext='*.event',first=1,last=-1,disp=F)
        while (cur_event_num==0&&i<len(files)) {i+:=1
          if (cur_event_file==files[i]~globalconst.FilenameExtractor) cur_event_num:=i
        }
        input_event(data,filename)
      } else if (stat(filename)[1]=='directory') {
        cur_event_dir:=filename ~ s/([^\/])$/$1\//
        cur_event_num:=1
        #files:=shell(spaste('ls ',cur_event_dir))
        files:=filesel(cur_event_dir,ext='*.event',first=1,last=-1,disp=F)
        cur_event_file:=files[1]
        input_event(data,cur_event_file)
      }
      data.newsagent->newdata()
      data.newsagent->checkdata()
      plotgui.messlabel->text(spaste('Event directory ',cur_event_dir,
        ' - file ',cur_event_num,'/',len(files),
        ' - ',cur_event_file))
      print 'Event directory', cur_event_dir
      print 'Opened file', cur_event_num, 'of', len(files),
        '-', cur_event_file
    }
    globalpar.cur_event_dir:=cur_event_dir
    globalpar.cur_event_file:=cur_event_file
    globalpar.cur_event_num:=cur_event_num
  }

#-----------------------------------------------------------------------
#Name: readevfiles
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Read event files from disk. This function starts the catalog tool
#      to select the disk file; reading from disk and further handling
#      is performed by 'readevfiles_cbfunc'.
#Ref:  readevfiles_cbfunc
#-----------------------------------------------------------------------

readevfiles := function()
{
  global dcstarted
  dc.setselectcallback(readevfiles_cbfunc);
  dc.gui(F,all,F);
  if (!dcstarted){
    dc.show(cur_event_dir,'All',T);
    dcstarted := T;
  };
}


#-----------------------------------------------------------------------
#Name: readprfiles_cbfunc
#-----------------------------------------------------------------------
#Type: function
#Doc:  Load a ITS project file previously selected via the AIPS++
#      catalog-tool.
#Ref:  readproject, readfilesH, datawatch-client, catalog-tool, concat_dir
#-----------------------------------------------------------------------

readprfiles_cbfunc := function(filename)
{
    global globalpar,globalconst;
    global data;

    if (!is_unset(filename)) {
     proj:=[=];
      if (stat(filename)[1] == 'ascii') {
        lgfile:=filename;
        idir:=(lgfile ~ globalconst.DirExtractor);
      } else if (stat(filename)[1] == 'directory') {
        idir:=filename;
        lgfile:=concat_dir(idir,'lopeshdr.hdr');
        if (!fexist(lgfile)) {
          print 'File ',lgfile,' does not exist.';
          return;
        }
      } else {
        print 'Error: Strange File Selection';
        return;
      }
      globalpar.DefITSDataDir := idir ~ globalconst.DirExtractor;
      # Read in project header file
      readproject(proj,
                  lgfile,
                  headerfiles=T);
      h:=proj[1];
      if (len(h)>0) {
        # Start loading data files mentioned in header file
        for (i in seq(len(h))) {
          h[i].Files    := concat_dir(idir,h[i].Files)
          h[i].Basefile := lgfile;
        }
	readfilesH(data,
                   h,
                   blocksize=globalpar.BlocksizeForRead,
                   offset=globalpar.OffsetForRead);
      } else {
        return;
      }
      data.newsagent->newdata();
      data.newsagent->checkdata();
    }
}

#-----------------------------------------------------------------------
#Name: readprfiles
#-----------------------------------------------------------------------
#Type: function
#Doc:  Loads ITS project-files via the catalog-tool; the selected data
#      are handled via the callback-function 'readprfiles_cbfunc'.
#Ref:  readprfiles_cbfunc
#-----------------------------------------------------------------------

readprfiles := function()
{
  global dcstarted
  dc.setselectcallback(readprfiles_cbfunc);
  dc.gui(F,all,F);
  if (!dcstarted){
    dc.show(globalpar.DefITSDataDir,'All',T);
    dcstarted := T;
  };
}

#-----------------------------------------------------------------------
#Name: selectDirectory
#-----------------------------------------------------------------------
#Type: Glish function
#Par:  callbackFunction - The callback function which will receive the
#      information of the users' selection.
#Created: 2004/11/05 (Lars Baehren)
#-----------------------------------------------------------------------

selectDirectory := function(ref callbackFunction)
{
  global dcstarted
  dc.setselectcallback(callbackFunction);
  dc.gui(F,all,F);
  if (!dcstarted){
    dc.show(globalpar.DefITSDataDir,'All',T);
    dcstarted := T;
  };
}


#-----------------------------------------------------------------------
#Name: loadynspeclopes_cbfunc
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  loads a dynamic spectrum via the catalog-tool from a sos-file
#Ref:  imgplot
#-----------------------------------------------------------------------

loadynspeclopes_cbfunc := function(filename)
{
  # Read the image pixel values from a file

  pixels:=read_value(filename);

  # Check the type of the x-axis

  viewersteps:=timestamps(filename=filename) # generates timestamps, saves them to file and returns them
#print shape(viewersteps)
  sfile:=spaste(filename,'.timesteps')
#print sfile
  if (fexist(sfile)) {
    viewersteps:=read_value(sfile);
    xax:=T
  } else {
    print spaste('LOADYNSPECLOPES_CBFUNC: ',sfile,' does not exist -> linear xaxis !!!');
    if (shape(pixels)[1]>2500) {viewersteps:=24; xax:=T} else {xax:=F}; #24h
  }; # end if fexist2
  if (len(viewersteps)>shape(pixels)[1]) {if (shape(pixels)[1]>2500) {viewersteps:=24; xax:=T}}; # else {xax:=F}};
#print xax
    
  # Create an image tool to display the data

  name:=split(filename~globalconst.FilenameExtractor,'')
  name:=name[1:(len(name)-4)] #without '.sos'
  xpos:=(1:len(name))[name=='x']
  if (len(xpos)==1) {name:=spaste(spaste(name[1:(xpos-1)]),shape(pixels)[1],spaste(name[(xpos+1):len(name)]))};
  imgplot(pixels,psdir='',name=name,
          axisorder=[2,1],bandlim=[40,80],freqdir=1,dmin=0,dmax=0,
          xax=xax,evnums=0,timvals=viewersteps,timunit='h',ps=F);
}

#-----------------------------------------------------------------------
#Name: loadynspeclopes
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Use the AIPS++ catalog tool, to select a '*.sos' file containing
#      a dynamic spectrum generated from LOPES data
#Ref:  loadynspeclopes_cbfunc
#-----------------------------------------------------------------------

loadynspeclopes := function()
{
  global dcstarted
  dc.setselectcallback(loadynspeclopes_cbfunc);
  dc.gui(F,all,F);
  if (!dcstarted){
    dir:='/home/anigl/data/lopesgui/lopesdyn/sos/';
    #if (len(shell(spaste('cd ',dir,'; ls ')))==0) {dir:=globalconst.HomeDir};
    dc.show(dir,'All',T);
    dcstarted := T;
  };
}

#-----------------------------------------------------------------------
#Name: loadynspecits_cbfunc
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Loads dynamic spectrum data and displays an image tool created 
#      therefrom.
#-----------------------------------------------------------------------

loadynspecits_cbfunc := function(filename)
{
  # Read the image pixel values from a file

  pixels:=read_value(filename);

  # Check the type of the x-axis

  if (fexist(spaste(filename,'.timesteps'))) {
    viewersteps:=read_value(spaste(filename,'.timesteps')); 
    xax:=T
  }
  else {
    print '(no timestepfile -> linear xaxis)'; 
    xax:=F
  }

  # Create an image tool to display the data

  imgplot(pixels,timvals=viewersteps,timunit='s',name=filename~globalconst.FilenameExtractor,
          axisorder=[2,1],bandlim=[0,40],freqdir=1,dmin=0,dmax=0,
          xax=xax,evnums=0,wedge=F,ps=F,psdir='',ret=F,done=F)

}

#-----------------------------------------------------------------------
#Name: loadynspecits
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Use the AIPS++ catalog tool, to select a '*.sos' file containing
#      a dynamic spectrum generated from ITS data
#Ref:  loadynspecits_cbfunc
#-----------------------------------------------------------------------

loadynspecits:=function()
{
  global dcstarted
  dc.setselectcallback(loadynspecits_cbfunc);
  dc.gui(F,all,F);
  if (!dcstarted){
	  dir:='/home/anigl/data/lopesgui/lopesdyn/sos/';
    #if (len(shell(spaste('cd ',dir,'; ls ')))==0) {dir:=globalconst.HomeDir};
		dir:=globalconst.HomeDir
    dc.show(dir,'All',T);
    dcstarted := T;
  };
}


#-----------------------------------------------------------------------
#Name: annotated_map_gui
#-----------------------------------------------------------------------
#Type: Function
#Doc:  GUI frontend for the function "annotated_map"
#Ret:
#
#Par: rec  =       - Record, where data is stored
#Ref: annotated_map
#-----------------------------------------------------------------------

annotated_map_gui := function(ref rec) {
  mapgui.frame := frame(title='Annotated Map Generation',width=1000,height=350,tpos='c');

  mapgui.chanlabel := label(mapgui.frame,'Used Channels:');
  mapgui.chanframe := [=]; fnum := 0;
  mapgui.chanbut := [=];
  for (i in seq(rec.len)) {
    if ( ((i-1)%5) == 0) {
      fnum := fnum+1;
      mapgui.chanframe[fnum] := frame(mapgui.frame,side='left');
      tframe := mapgui.chanframe[fnum];
    };
    mapgui.chanbut[i] := button(tframe,paste('Ch',as_string(i)),type='check')
    mapgui.chanbut[i]->state(T);
  };
  mapgui.aresframe := frame(mapgui.frame,side='left');
  mapgui.areslab := label(mapgui.aresframe,'Angular Resolution:     ');
  mapgui.aresent := entry(mapgui.aresframe,width=5);
  mapgui.aresent->insert('10')
  mapgui.areslab2 := label(mapgui.aresframe,'deg');

  mapgui.fresframe := frame(mapgui.frame,side='left');
  mapgui.freslab := label(mapgui.fresframe,'Frequency Resolution: ');
  mapgui.fresent := entry(mapgui.fresframe,width=5);
  mapgui.fresent->insert('1')
  mapgui.freslab2 := label(mapgui.fresframe,'Steps');

  mapgui.lunitframe := frame(mapgui.frame,side='left');
  mapgui.lunitlab := label(mapgui.lunitframe,'Position Unit:              ');
  mapgui.lunitent := entry(mapgui.lunitframe,width=5);
  mapgui.lunitent->insert('1.0')
  mapgui.lunitlab2 := label(mapgui.lunitframe,'m');

  mapgui.plistframe := frame(mapgui.frame,side='left');
  mapgui.plistlab := label(mapgui.plistframe,'Planet List: ');
  mapgui.plistent := entry(mapgui.plistframe,width=20);
  mapgui.plistent->insert('Sun Moon Jupiter')

  mapgui.scatframe := frame(mapgui.frame,side='left');
  mapgui.scatlab := label(mapgui.scatframe,'Source Catalog: ');
  mapgui.scatent := entry(mapgui.scatframe,width=16);
  mapgui.scatent->insert('defsourcecat.tbl')

  mapgui.fstart := frame(mapgui.frame,side="left");
  mapgui.startbut := button(mapgui.fstart,'Generate Map',background='green');
  mapgui.startspace := frame(mapgui.fstart);
  mapgui.endbut := button(mapgui.fstart,'Dismiss',background='orange');

  whenever mapgui.startbut->press do {
    index := []; j:=0
    for (i in seq(rec.len)) {
      if (mapgui.chanbut[i]->state()) { j:=j+1; index[j]:=i;};
    };
    annotated_map(ref rec, index, refant=index[1],
                  res=as_float(mapgui.aresent->get()),
                  nfreq=as_float(mapgui.fresent->get()),
                  unit=as_float(mapgui.lunitent->get()),
                  planetlist=split(mapgui.plistent->get()),
                  sourcecat=mapgui.scatent->get() )
  };

  whenever mapgui.endbut->press do {
    mapgui.frame := F;
    mapgui := F;
  };

}

#-------------------------------------------------------------------------------
#
#Section: <a name="stats">Statistical properties of data</a>
#
#-------------------------------------------------------------------------------


#-----------------------------------------------------------------------
#Name: DataStat
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Calculates some basic statistical qunatitties for a data set
#      such as min, max, mean, median, and rms.
#Ret:  Record with Value of one element
#
#Par: rec     =       - Record, where data is stored
#Par: xfield  = "y"   - 1st Coordinate axis ("x","y",...)
#Par: yfield  = "y"   - 2nd Coordinate axis ("x","y",...)
#Par: index   = 1     - Index of data set to be retrieved
#Par: doprint = T     - Print output?
#Ref:
#-----------------------------------------------------------------------
DataStat:=function(ref rec,xfield="y",yfield="y",index=1,doprint=T) {
  if (index<1 || index>rec.len) return F
  return DataStatXY(rec.get(xfield,index),rec.get(yfield,index),'','',doprint);
}

#-----------------------------------------------------------------------
#Name: DataStatXY
#-----------------------------------------------------------------------
#Type: Function
#Doc: Calculates some basic statistical qunatitties for a data set in an
# X and Y array such as min, max, mean, median, and rms.

#Ret: Record with Value of one element
#
#Par: xary    =       - 1st Coordinate axis array
#Par: yary    =       - 2nd Coordinate axis array
#Par: xunit   =       - String with x-axis Unit
#Par: yunit   =       - String with x-axis Unit
#Par: doprint = T      - Print output?
#Ref: 
#-----------------------------------------------------------------------
DataStatXY:=function(xary,yary,xunit='',yunit='',doprint=T) {
  datainfo:=[=];
  datainfo.min:=min_with_location(yary,datainfo.minlocation);
  datainfo.minlocation:=xary[datainfo.minlocation];
  if (doprint) {print 'Min  =',datainfo.min,' ',yunit,' at x =',datainfo.minlocation,' ',xunit};
  datainfo.max:=max_with_location(yary,datainfo.maxlocation);
  datainfo.maxlocation:=xary[datainfo.maxlocation];
  if (doprint) {print 'Max  =',datainfo.max,' ',yunit,' at x=',datainfo.maxlocation,' ',xunit};
  datainfo.mean:=mean(yary);
  datainfo.stddev:=stddev(yary);
  if (doprint) {print 'Mean =',datainfo.mean,' ',yunit,' +/- ',datainfo.stddev,' ',yunit};
#  datainfo.median:=median(rec.get(yfield,index));
#  if (doprint) {print 'Median=',datainfo.median};
  return datainfo;
}

#-----------------------------------------------------------------------
#
#Section: Misc. Functions of more or less use...
#
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
#Name: dezero
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Removed zero-valued entries from a field of numbers
#Par:  ary  - Array of numbers.
#Ret:  ary  - Array with zero-valued entries removed.
#-----------------------------------------------------------------------

dezero:=function(ary){b:=(ary>0); mn:=min(ary[b]); ary[!b]:=mn; return ary}

#-----------------------------------------------------------------------
#Name: ident
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#Par:  x
#Ret:  x
#-----------------------------------------------------------------------

ident := function(x) {return x;};

#-----------------------------------------------------------------------
#Name: square
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Compute the square of a number.
#Par:  x      - A number.
#Ret:  square - The square of the input value x.
#-----------------------------------------------------------------------

square := function(x) {return x^2;};

#-----------------------------------------------------------------------
#Name: vinvert
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#Par:  ary - 
#Ret:  ary - 
#-----------------------------------------------------------------------

vinvert:=function(ary){return ary[seq(len(ary),1,-1)];};

#-----------------------------------------------------------------------
#Name: dB
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Convert a number to dB scale
#Par:  x  - A value
#Ret:  dB - The input value converted to dB.
#-----------------------------------------------------------------------

dB := function(x,min=1e-35) { return 10*log(abs(x)+min); };

#-----------------------------------------------------------------------
#Name: volt2pow
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Convert Voltage to power
#Par:  x   - Voltage
#Ret:  pow - Input voltage value converted to power.
#-----------------------------------------------------------------------

volt2pow := function(x) { return ((x^2)/50); };

#-----------------------------------------------------------------------
#Name: spec2pow
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

spec2pow := function(x) { return ((abs(x)^2)/50); };

#-----------------------------------------------------------------------
#Name: meanv
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

meanv := function(x) {
  size := len(x);
  meanx := array(0.,size); 
  meanx[1]:=x[1]; meanx[size]:=x[size]
  meanx[2:(size-1)] := (x[2:(size-1)]+x[1:(size-2)]+x[3:size])/3;
  return meanx;
};

#-----------------------------------------------------------------------
#Name: mean5
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

mean5 := function(x) {
  size := len(x);
  if (size<5) return F;

  meanx := array(0.,size); 
  meanx[1]:=x[1]; meanx[2]:=x[2]; 
  meanx[size-1]:=x[size-1]; meanx[size]:=x[size];
  meanx[3:(size-2)] := (x[1:(size-4)]+x[2:(size-3)]+x[3:(size-2)]+
        x[4:(size-1)]+x[5:size])/5;
  return meanx;
};


#-----------------------------------------------------------------------
#Name: rfft
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

rfft := function(data,dir=1) {
  a := data
  global_fftserver.complexfft(a,dir);
  return a;
};

#-----------------------------------------------------------------------
#Name: array2string
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

array2string:=function(ary){
  s:=""; 
  for (i in seq(len(ary))) s[i]:=as_string(ary[i])
  return s
};


#-----------------------------------------------------------------------
#Blockoperations
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
#Name: blockopold
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

blockopold:=function(ary,f,block=2) {
  blk:=min(max(1,block),len(ary));
  idx:=seq(blk); 
  l1:=len(ary); 
  l2:=as_integer((l1+blk-1)/blk)
  out:=array(0,l2);
  for (i in seq(l2-1)) out[i]:=f(ary[idx+(i-1)*blk])
  if ((l2-1)*blk+1<=l1) out[l2]:=f(ary[((l2-1)*blk+1):l1])
  return out
};

#-----------------------------------------------------------------------
#Name: blockop
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

blockop:=function(ary,f,block=2) {
  l:=len(ary); 
  blk:=as_integer(min(max(1,block),l));
  lminus:=as_integer((blk-1)/2);
  lplus:=as_integer(blk/2);
  out:=array(0,l);
  for (i in seq(l)) {
    idx:=seq(max(i-lminus,1),min(i+lplus,l)); 
    out[i]:=f(ary[idx])
  }
  return out
};

#-----------------------------------------------------------------------
#Name: blockaverage
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

blockaverage:=function(ary,block=2) { cMeanFilter(ary,block) };

#-----------------------------------------------------------------------
#Name: blockrms
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

blockrms:=function(ary,block=2) { sqrt(cVarFilter(ary,block)) };


#-----------------------------------------------------------------------
#
#Section: Misc. Functions for analysis of the time series
#
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
#Name: peakary
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

peakary := function(ary,thresh=10,filter=3) {
  if (filter == 0 ) {
    filt := abs(ary);
  } else if ((filter > 1) && (filter<10)) {
    filt := cMeanFilter(abs(ary),filter);
  } else if ((filter < -1) && (filter>-10)) {
    filt := cMedFilter(abs(ary),filter);
  } else if (filter == 10) {
    filt := blockaverage(abs(ary),10);
  };
  arymom := moments(2,filt);
  erg := filt>(arymom.mean+arymom.stddev*thresh);
  return erg;
};

#-----------------------------------------------------------------------
#Name: peakary2
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

peakary2 := function(ary,thresh=10,filter=3,region=100) {
  if (filter == 0 ) {
    filt := abs(ary);
  } else if ((filter > 1) && (filter<10)) {
    if (region == 100) { region := 100*filter;};
    filt := cMeanFilter(abs(ary),filter);
  } else if ((filter < -1) && (filter>-10)) {
    if (region == 100) { region := -100*filter;};
    filt := cMedFilter(abs(ary),filter);
  } else if (filter == 10) {
    filt := blockaverage(abs(ary),10);
  };
  meanarr := cMeanFilter(filt,region);
  vararr := cVarFilter(filt,region);
  return filt>(meanarr+vararr*thresh);
};


#-----------------------------------------------------------------------
#Name: pfilt
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

pfilt := function(ary,thresh=10,filter=3) {
  if (filter == 0 ) {
    filt := abs(ary);
  } else if (filter == 3) {
    filt := meanv(abs(ary));
  } else if (filter == 5) {
    filt := mean5(abs(ary));
  } else if (filter == 10) {
    filt := blockaverage(abs(ary),10);
  };
  l:=len(filt);
  cuts := array(0.,l);
  for ( i in [1:l]) {
    arymom := moments(2,filt[max((i-50),1):min((i+50),l)]);
    cuts[i] := (arymom.mean+arymom.stddev*thresh);
  };
  erg := filt>cuts;
  return erg;
};

#-----------------------------------------------------------------------
#Name: pasum
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

pasum := function(ref rec,index=1,thresh=10,filter=3) {
  for (i in index) {
    if (i == index[1]) {
      erg := peakary(rec.get('Time-Ser',i),thresh,filter);
    } else {
      erg := erg + peakary(rec.get('Time-Ser',i),thresh,filter);
    };
  };
  return erg;
};

#-----------------------------------------------------------------------
#
#Section: Misc. Functions for analysis of the frequency spectrum
#
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
#Name: autoflag
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Remove radio interference from recorded data
#Par:  rec  - Record where the data is stored
#Par:  band - start and stop frequency of the effective bandpass (in MHz)
#-----------------------------------------------------------------------

autoflag := function (ref rec, band=[-1,-1]) {

  wider rec;

  # Check the frequency band within which the RFI mitigation is performed
  if (band[1]==-1) {
    band := globalpar.Bandpass;
  } else { 
    band := VerifyFrequencyBand(rec,band)*as_double(rec.head('FrequencyUnit'));
  };
 
  local bar := progress(0, len(seq(rec.len)[rec.select()]), 'Filtering...');
  bar.activate();
 
  local j := 0;
  for (i in seq(rec.len)[rec.select()]) {
    j +:= 1;
    nx1 := sum(rec.get('Frequency',i)<band[1]);  # 43e6 
    nx2 := sum(rec.get('Frequency',i)<band[2]);  # 76e6
    rec.putflag(rec.getflag('AbsFFT',index=i)*newfilterlines(abs(rec.get('CalFFT',i)),
                cutneg=F,nsigma=[3],nx1=nx1,nx2=nx2,retratio=T),
                field='FFT',index=i); 
    bar.update(j);
  }
 
  bar.done();
}

#-----------------------------------------------------------------------
#Name: PowerArray
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

PowerArray := function (ref rec,index=1,band=1,blocks=1,peakfilt=F) {
    nsum:=0; 
    index := index[1];
      if (is_agent(rec.newsagent)) { rec.newsagent->message(paste(
		'Computing Power-Array for index',as_string(index)));};
      size := rec.getmeta('Blocksize',index);
      numblocks := as_integer(rec.getmeta('FileBlocksize',index)/size);
      OldOffset := rec.getmeta('Offset',index);
      fftsum := array(0,len(band));
      for (j in [1:numblocks]) {
	   rec.putmeta((size*(j-1)),'Offset',index);
	   yary:=rec.get('Voltage',index);
	   if (peakfilt) {
		filtary := peakary(yary,5,3);
		# filtary := filtary | peakary((yary* (! filtary)),5,3);
		yary := yary * (! filtary);
	   };
	   global_fftserver.complexfft(yary,1); 
   	   fftsum := fftsum + abs(yary)[band];
	   if ( (j%blocks) == 0) {
             fftsum := fftsum/blocks;
	     if (j==blocks) { erg := fftsum;
	     } else { erg := rbind(erg,fftsum); };
	     fftsum := array(0,len(band));
	   };
      }
      if ( (j%blocks) > (blocks/2)) {
        fftsum := fftsum/(j%blocks);
        erg := rbind(erg,fftsum); 
      };
      rec.putmeta(OldOffset,'Offset',i);      
    if (is_agent(rec.newsagent)) {rec.newsagent->message('Power-Array computed');};
    return erg;
};

#-----------------------------------------------------------------------
#Name: redim2to1
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  ???
#-----------------------------------------------------------------------

redim2to1  := function(ary,ind=1) {
  shap := shape(ary);
  if (len(shap) != 2) return F;
  if (ind==1) {
    for (i in [1:shap[1]]) {
    erg[i] := mean(ary[i,]);
    };
  } else {
     for (i in [1:shap[2]]) {
    erg[i] := mean(ary[,i]);
    };
  };
  return erg;
};

#-----------------------------------------------------------------------
#Name: xcorr
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  calculate the cross correlation of two arrays
#Example: a:=[0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0]; b:=[0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0]; plotdim(xcorr(a,b).cor,xvals=xcorr(a,b).lag); plotdim(a,xvals=0:(len(a)-1),add=T); plotdim(b,xvals=0:(len(b)-1),add=T);
#-----------------------------------------------------------------------
func xcorr(a,b,maxlag=-1,maxord=5) {
  # cross correalation
  size:=len(a);
  if (len(b)!=size) return F;
  if ((2*maxlag)>=size) return F;
  if (maxlag==-1) {lags:=0:(size-1)} #-(size-1):(size-1)
  else {lags:=-maxlag:maxlag};
  res:=[=]; ind:=0
  x:=fftserver(); ma:=mean(a); mb:=mean(b)
  for (lag in lags) {ind+:=1
    res.lag[ind]:=lag;
    res.cor[ind]:=sum((a-ma)*(x.shift(b,-lag)-mb))/
                  (sqrt(sum((a-ma)^2))*sqrt(sum((x.shift(b,-lag)-mb)^2)))
#print ind,res.lag[ind],res.cor[ind]
  };
  ## order
  ##xlen:=len(res.cor)
  ##print ind-1,xlen-1,maxord,min(xlen-1,maxord)
  ##maxord:=min(xlen-1,maxord)
  ##res.ord:=order(res.cor);
  ##res.ord:=res.ord[(xlen-maxord):xlen];
  # return  
  return res;
};

#-----------------------------------------------------------------------
#Name: crosscorr
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  calculate the correlation of two arrays
#-----------------------------------------------------------------------

crosscorr := function(X,Y,maxlag=5,maxord=5) {
  size := len(X);
  if (len(Y) != size ) return F;
  if ((2*maxlag) >= size) return F;
  ind:=1;
  erg:= [=];
  for (lag in [-maxlag:maxlag] ) {
    erg.lag[ind] := lag;
    erg.corr[ind] := (sum( X[(1+maxlag):(size-maxlag)] 
             * Y[(1+maxlag-lag):(size-maxlag-lag)])
        / (size-(2*maxlag)) );
    ind := ind+1;
  };
  si := len(erg.corr);
  maxord:=min(si-1,maxord);
  erg.ord:= order(erg.corr);
  erg.ord:= erg.ord[(si-maxord):si];

  return erg;
};
  
#-----------------------------------------------------------------------
#Name: crosscorr2
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  calculate the correlation of two arrays with subpixel steps
#-----------------------------------------------------------------------

crosscorr2 := function(X,Y,stepsize=0.25,maxlag=5,maxord=5,linear=F) {
  size := len(X);
  if (len(Y) != size ) return F;
  if ((2*maxlag) >= size) return F;
  ind:=1;
  erg:= [=];
  for (lag in ([(-maxlag/stepsize):(maxlag/stepsize)]*stepsize) ) {
    erg.lag[ind] := lag;
    if (linear) {
      ilag1:=floor(lag); ilag2:=ilag1+1;
      w2:=lag-ilag1; w1:=1-w2;
      shiftY:=Y[(2+maxlag-ilag1):(size-maxlag-ilag1-1)]*w1+Y[(2+maxlag-ilag2):(size-maxlag-ilag2-1)]*w2}
    else {
      shiftY:=global_fftserver.shift(Y,lag)[(1+(maxlag+1)):(size-(maxlag+1))]
    }
    erg.corr[ind]:=(sum(X[(1+(maxlag+1)):(size-(maxlag+1))]*shiftY)
        / (size-2*(maxlag+1)) );
    ind+:=1;
  };
  si := len(erg.corr);
  maxord:=min(si-1,maxord);
  erg.ord:= order(erg.corr);
  erg.ord:= erg.ord[(si-maxord):si];

  return erg;
};



plotxy:=function(x,y) {
global f;
f:=frame();
x1:=min(x); x2:=max(x);
y1:=min(y); y2:=max(y);
pg := pgplot (f);
 draw := function(x,y,x1,x2,y1,y2) {
      pg->env (x1, x2,y1,y2, 0, 0);
      pg->line (x, y);
    }

    draw(x,y,x1,x2,y1,y2); # Initial plot

    whenever f->resize do {
        draw(x,y,x1,x2,y1,y2);  # Redraw at the current size
}
}

#######################################################################
#-----------------------------------------------------------------------
#Name: baselines
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  calculates all antenna combination baselines
#Example: [1] input_project(data,dir='example.its/',head='*.hdr',ants=-1); data.select(1:data.len,T); baselines(data,disp=F,add=T,ret=F); printrec(data.base) [2] input_event(data,'example.event'); base:=baselines(data,add=F,ret=T); printrec(base)
#ToDo: optimize for two antennas
#-----------------------------------------------------------------------
func baselines(ref rec=[],antenna=[],disp=T,add=T,ret=F) {
#input_project(data,'/raid/pv3/ITS/jupiter/2004.06.22-15:37:34.00-jupiter06/',ants=[1,2])
#file:='example.event'; input_event(data,file)

  # print
  if (disp) {
    print '-------------------------Baselines-------------------------'
  };
  
  # error
  if (len(rec)==0) {if (len(antenna)==0) {return F} else {nants:=len(antenna)}}
  else {nants:=len(rec.selant())};
  if (nants<3) {print 'baselines: load more antennas, just',nants,'antenna(s) loaded'; return F};

  # headerinfos
  if (len(rec)!=0) {
    samplerateunit:=rec.headf('SamplerateUnit')
    samplerate:=rec.headf('Samplerate')*samplerateunit
  };

  # positions
  if (len(antenna)==0) {antpos:=CalcAntPos(rec,rec.selant(),rec.get('RefAnt'),1)}
  else {antpos:=antenna};

  # antenna combinations / baselines
  # [ N_base = 1/2 * N_ants * (N_ants -1) ]
  count:=0; num:=nants;
  for (i in 2:num)
  { for (j in 1:(num-1))
  { if (!(j==i)&(j<i))
  { count+:=1}}} #print j,i

  # baselines
  base:=[=];
  basenum:=0;
  for (sec in 2:nants) # position[2]=x & position[1]=y
  { for (fst in 1:(nants-1))
  { if (!(fst==sec)&(fst<sec))
    { basenum+:=1;
      base[basenum].vec:=[antpos[sec].position[2]-antpos[fst].position[2],            
                          antpos[sec].position[1]-antpos[fst].position[1],
                          antpos[sec].position[3]-antpos[fst].position[3]]
      base[basenum].point:=atan(base[basenum].vec[2]/base[basenum].vec[1])*180/pi
      if (base[basenum].vec[1]>=0) {base[basenum].point:=90-base[basenum].point}
      else {base[basenum].point:=270-base[basenum].point};                          
      base[basenum].meter:=sqrt(sum(square(base[basenum].vec)))
      base[basenum].combi:=[fst,sec];
  };}}

  # bubblesort
  for (fst in 2:basenum)
  { for (sec in 1:(basenum-1))
  { if (base[fst].meter<base[sec].meter)
    { tmp:=base[fst];
        base[fst]:=base[sec];
        base[sec]:=tmp;
  };}}

  # print
  if (disp) {
    if (basenum<1000) {
      for (baseline in 1:basenum) {
        print sprintf("[%2d] %6.2f m %7.2f ° (ants: %02d + %02d)",
                      baseline,base[baseline].meter,base[baseline].point,
                      base[baseline].combi[1],base[baseline].combi[2])
        #print baseline,base[baseline].meter,'m','ants',base[baseline].combi
      }
    }; # endif number of baselines
    print 'BASELINES: number of selected antennas:',nants
    print 'BASELINES: number of antenna combinations (baselines):',count
      minbase:=base[1].meter
    print 'BASELINES: minimum baseline length:',minbase,'m'
      maxbase:=base[len(base)].meter
    print 'BASELINES: maximum baseline length:',maxbase,'m'
      maxdelay:=maxbase/lightspeed
    print 'BASELINES: maximum delay (horizon):',maxdelay,
          's (',round(maxdelay*samplerate),'samples )'
      maxdelay45:=(cos(45/180*pi)*maxbase)/lightspeed
    print 'BASELINES: maximum delay (45deg):',maxdelay45,
          's (',round(maxdelay45*samplerate),'samples )'
    print '-------------------------Baselines-------------------------'
  };
  
  # data structure
  if (add&&len(rec)!=0) {
    print '(*) calculated baselines are attached to data structure: data.base'
    rec.base:=base
  };
  
  # return
  if (ret) {return base};

} # end function baselines
#######################################################################
