#data.load(1,F,1); include '../guipipeline/guipipeline-results.all.g'; include '../guipipeline/guipipeline-results.data.g'; linclude('quality.g');

########################################################################
# This file contains the basic scripts to create plots for the statistical
# anlysis. It makes sub-selections and it fits correction functions.
# This script has produced the figures for the first paper.
#
# The idea is to start the lopestools from the directory where the result
# files have been created. Run the guipipeline-results.*-*.g file with 
# include. It is suggesteded to rename it to results.g so you can modify it:
#
# include 'results.g'
#
# In the same directory you can create a parameters.g and quality.g file.
#
# results.g will automatically load the data file and execute crresults.g.
#
# To rerun the evaluation without including again the data files, just change
# parameters.g and type 
#
# include add_dir('crresults.g')
#
# or type the function
#
# rerun()
#
########################################################################

print '#-----------------------------------------------------------------------'
print '#Loading and executing crresults.g in lopesprog.'


#-Selection Parameters for statistical evaluation------------------------

#------------------------------------------------------------------------
#This are the parameters for the selection of the first LOPES paper ....
#Copy them into parameters.g in the local directory to make your own choices.
#------------------------------------------------------------------------

#-BEGIN------------------------------------------------------------------
#Using Horneffer  second selection file ...
  selectionfile:=2

#Maximum separation between shower core and center of LOPESs antennas.
  rlimit:=70;
#Minimum log number of muons
  lmulimit:=1
#Minimum log energy (formula by R.Glasstaetter)
  lenlimit:=1
#Minimum angle between shower axis and magnetic field
  ang2Blimit:=0*degree
#Minimum number of antennas
  minants:=6
#Minimum value of quality flag to call an event a detection. 
#Right now quality3 is used which doen my eye!
  quallimit:=0.2
#Lower limit on Zenith angle
  zelowlimit:=0*degree
#Upper limit on Zenith angle
  zeuplimit:=50*degree

#Use the rms as error for single events
  doerrors := F
#Default relative gain error in flux for every event
  relgainerror:=0.05

#Number of bins for the pulse-energy profile
  enprofbins := 8;

#Specifies how to treat upper limits 
  sigmafakt:=2

#The maximum number of events that have been calculated in the guipipeline.
#This is neccesary if the pipeline has not yet worked on all events in 
#the selection file.
  maxevents:=412
#Used to define default event to be loaded at start-up
  defaultevent:=1
#-END--------------------------------------------------------------------

#A list of events that have been used in the Nature paper
#[16,55,63,82,87,91,99,108,115,125,188,201,211,217,231,232]
#[0.5 0.3 1 0.7 0.5 0.5 0.6 1 0.4 0.2 0.9 0.8 0.7 0.2 0.9 0.8]
# event 108 is highly inclined -> thrown out
nature_events:=[16,55,63,82,87,91,99,115,125,188,201,211,217,231,232]

if (fexist('parameters.g')) {
  print '#Loading local parameter file: parameters.g'
  include 'parameters.g'

  cmd.awk(add_dir('printparameters.awk'),'parameters.g',print_result=T)

} else {print 'Using default selection parameters - are you sure?'}


print 'Attention: you need to have the correct crtab/evtab loaded!'
print 'This happen automatically if you load any event with data.load.'
print 'Make sure it is loaded from the corresponding selection file.'

if (globalpar.event_loaded<=0) {
  data.load(defaultevent,doflag=F,fileno=selectionfile);
}

if (len(evtab.LMUO)>maxevents) {
  print 'Only taking the first',maxevents,'Events'  
  for (f in field_names(evtab)) {
    evtab[f]:=evtab[f][1:maxevents]
  }
}

ncr:=len(sel);
if (len(quality3a)/2 != ncr){ print 'No matching quality values available!'; print len(quality3a)/2,ncr,(len(quality3a)/2 != ncr)}#exit;}
cr:=seq(ncr);
hc:=array(0,ncr);
hx:=hc;
cx:=hc;
cc:=hc;
wx:=hc;
wc:=hc;
hf:=hc;
wf:=hc;
cf:=hc;
angd:=hc;
logE:=hc;
nant:=hc;
meandist:=hc;
gerr_f := hc;
for (i in cr) {
  if (len(fitF[i])>0) {
    hf[i]:=fitF[i].height
    wf[i]:=fitF[i].width*1000
    cf[i]:=fitF[i].center
  }
  if (len(fitX[i])>0) {
    hx[i]:=fitX[i].height
    wx[i]:=fitX[i].width*1000
    cx[i]:=fitX[i].center
  }
  if (len(fitC[i])>0) {
    cc[i]:=fitC[i].center
    wc[i]:=fitC[i].width*1000
    hc[i]:=fitC[i].height
    hce[i]:=fitC[i].height_error
    convc[i]:=fitC[i].converged
  }
  quality3[i]:=quality3a[i*2]
  angd[i]:= ang2B(evtab.AZ[i],evtab.ZE[i]);
  nant[i]:=len(flag[i][flag[i]])
  meandist[i] := mean(dists[i][flag[i]]);
  gerr_f[i] := get_gainerror_fact(evtab.AZ[i],evtab.ZE[i],dalpha=0.3)
}
ncorr := korr_Ne_Nmu(evtab.SIZE,evtab.LMUO)
logE := getCRen(ncorr.Ne,ncorr.Nmu,evtab.ZE)

#-BEGIN------------------------------------------------------------------
# Correction for the bug in the lopestools. (The "sqrt(Gain)"-bug.)
#-END--------------------------------------------------------------------
rmsC := rmsC*GainErrCorr
hc := hc*GainErrCorr
hce := hce*GainErrCorr

#-BEGIN------------------------------------------------------------------
# Select the events acording to the criteria defined in parameters.g
#-END--------------------------------------------------------------------
select:= (evtab.evnum>=0) & (evtab.LMUO > 10^lmulimit) &  (meandist < rlimit) & (nant >= minants)  & (evtab.ZE>zelowlimit) & (evtab.ZE<zeuplimit) & (angd>ang2Blimit); 
select[8]:=F;   #thunderstorm event
select[54]:=F;   #thunderstorm event
select[184]:=F; #something is wrong with the data/TV cal
select[320]:=F;   #thunderstorm event?


detect:= (quality3 >= quallimit) & (hc>0) ;
good:= detect & select;
bad:=(!detect) & select;
inclined:=good & evtab.ZE>0.3

# uncomment this if you want to look at the events fron the Nature paper
#good := nature_events


print 'Mean (cc) center = ',mean(cc[good]),'+/-',stddev(cc[good]),'musec'
print 'Mean (cc) width  = ',mean(wc[good]),'+/-',stddev(wc[good]),'nsec'
print 'Distance  = ',mean(meandist[good]),'+/-',stddev(meandist[good]),'m'


minrmsC:=min(rmsC[rmsC>0])
rmsCno0:=rmsC;
rmsCno0[rmsCno0==0]:=minrmsC

hcomp := array(0.,ncr);
hcomp[detect] := hc[detect]
hcomp[!detect] := sigmafakt*rmsCno0[!detect]

#-BEGIN------------------------------------------------------------------
# calculation of the statistical error of the pulse height
#-END--------------------------------------------------------------------
errs := sqrt((hce)^2+(gerr_f*hcomp)^2+(rmsC)^2+(hcomp*relgainerror)^2);
errs2 := sqrt((hce)^2+(rmsC)^2+(hcomp*relgainerror)^2);
errs3 := sqrt((gerr_f*hcomp)^2+(rmsC)^2+(hcomp*relgainerror)^2);



print 'Total number of events:',len(select)
print 'Total number selected:',len(cr[select])
print 'Total number detected:',len(cr[detect]),'(',round(len(cr[detect])/len(cr[select])*100),'% )'

print '  Detected Events:',as_inputvec(cr[detect])
print 'Undetected Events:',as_inputvec(cr[select & !detect])

print 'Mean position offset:',mean(diroff[good])
print 'Sigma position offset:',stddev(diroff[good])

print 'rlimit   =',rlimit
print 'lmulimit =',lmulimit
print 'minants  =',minants
print 'quallimit=',quallimit


globalpar.PrintingPlots:=T


#-BEGIN------------------------------------------------------------------
# plot some histograms 
#-END--------------------------------------------------------------------
lmu1:=evtab.LMUO[good];
lmu2:=evtab.LMUO[bad];

xmin:=min([lmu1,lmu2]);
xmax:=max([lmu1,lmu2]);
ymin:=0;
ymax:=110;

if (!is_defined('pg2') || (len(pg2)==1)) pg2:=pgplotter(foreground='black',background='white')
pg2.sci(1)
pg2.clear()
  if (globalpar.PrintingPlots) {
    pg2.sch(globalpar.PrintCharHeight);
    pg2.slw(globalpar.PrintLineWidth);
    pg2.sci(globalpar.PrintLabelCol);
  }
pg2.env(xmin,xmax,ymin,ymax,0,0);
pg2.lab('muon number','N (events)','detected (blue) vs. undetected (red)')
pg2.sci(2)
if (len(lmu2)>0) {pg2.hist(lmu2,xmin,xmax,14,1)}
pg2.sci(4)
if (len(lmu1)>0) {pg2.hist(lmu1,xmin,xmax,14,1)}
pg2.postscript('results-histogram-detected.ps')


xmin:=0;
xmax:=90;
ymax:=45;
pg2.sci(1)
pg2.clear()
pg2.sch(globalpar.PrintCharHeight);
pg2.slw(globalpar.PrintLineWidth);
pg2.sci(globalpar.PrintLabelCol);
pg2.env(xmin,xmax,ymin,ymax,0,0);
pg2.lab('Geomagnetic Angle','N (events)','detected (blue) vs. undetected (red)')
pg2.sci(4)
pg2.hist(angd[good]/pi*180,xmin,xmax,14,1)
pg2.sci(2)
if (len(angd[bad])>0) pg2.hist(angd[bad]/pi*180,xmin,xmax,14,1)
pg2.postscript('results-histogram-detected-angb.ps')


#-BEGIN------------------------------------------------------------------
# some helper functions
#-END--------------------------------------------------------------------

kkoeff := function(x,y){
  xn := mean(x);  yn := mean(y);
  sxy := sum( (x-xn) * (y-yn) );
  sxx := sum((x-xn)^2);  syy := sum((y-yn)^2);
  return sxy/sqrt(sxx*syy);
};

plotarrows := function(xar,yar,col=4,arrlen=0) {
  if (len(xar)<=0) {return}
  if (arrlen==0) {arrlen := abs((yar/10))}; 
  y2ar := yar-arrlen;
  oldch := pg.qch();
  oldcol := pg.qci();
  pg.sch(1);
  pg.sci(col);
  for (i in seq(len(xar))){
    pg.arro(xar[i],yar[i],xar[i],y2ar[i]);
  };
  pg.sch(oldch);
  pg.sci(oldcol);
};


#-BEGIN------------------------------------------------------------------
# do the so called "Kartoffelplots"
#-END--------------------------------------------------------------------
#"Kartoffelplots" :-)
plot(log(ncorr.Nmu[good]),1-cos(angd[good]),xlabel='log(Muon Number)',ylabel='1-cos(Geomagnetic Angle)',clear=T,lines=F,col=4,psfile=F)
#title=spaste('(all events with r < ',as_string(rlimit),' m)')
plot(log(ncorr.Nmu[bad]),1-cos(angd[bad]),clear=F,lines=F,col=2,psfile=T,filename=spaste('results-angB-vs-muon-',as_string(rlimit),'m.ps'))
#plot(log(ncorr.Nmu[bad]),1-cos(angd[bad]),clear=F,lines=F,col=2,symbol=7,psfile=T,filename=spaste('results-angB-vs-muon-',as_string(rlimit),'m.ps'))

plot(logE[good],cos(angd[good]),xlabel='log(Primary Energy/GeV)',ylabel='cos(Geomagnetic Angle)',title='',clear=T,lines=F,col=4,psfile=F)
plot(logE[bad],cos(angd[bad]),clear=F,lines=F,col=2,psfile=T,filename=spaste('results-angB-vs-energy-',as_string(rlimit),'m.ps'))

# sfun1 and sfun2 taken from parameters.g
sfun1 := function(x){return 3.58-0.6*x}
sfun2 := function(x){return 4.32-.71*x};
if (len(better) != len(good)) {better := good& (hc>5*rmsC); };
plot(log(ncorr.Nmu[good]),1-cos(angd[good]),xlabel='log(Muon Number)',ylabel='1-cos(Geomagnetic Angle)',clear=T,lines=F,col=4,psfile=F)
plot(log(ncorr.Nmu[bad]),1-cos(angd[bad]),clear=F,lines=F,col=2,psfile=F)
plot([5,5.5,6],sfun1([5,5.5,6]),clear=F,lines=T,col=3,psfile=F)
pg.sls(2)
plot([5,5.5,6,6.5],sfun2([5,5.5,6,6.5]),clear=F,lines=T,col=3,psfile=F)
pg.sls(3)
plot([5.396,5.396],[0.,1.],clear=F,lines=T,col=3,psfile=F)
plot([4,7],[0.231716476,0.231716476],clear=F,lines=T,col=3,psfile=F)
pg.sls(1)
plot(log(ncorr.Nmu[better]),1-cos(angd[better]),clear=F,lines=F,col=5,psfile=T,filename='results-angB-vs-muon-lines.ps')


#-BEGIN------------------------------------------------------------------
# choose the angular function you like to use and uncomment the 
# corresponding line
#
# ***Keep in mind that except for "1-cos()" the axis label will be wrong!
#-END--------------------------------------------------------------------
angfunc := 1-cos(angd)
#angfunc := sin(angd)
#angfunc := 1-cos(angd)*cos(evtab.ZE)
#angfunc := sin(angd)*cos(evtab.ZE)



#-BEGIN------------------------------------------------------------------
# do some plot of unmodified data
#-END--------------------------------------------------------------------

aryx:=angfunc[good]
aryy:=hc[good]
fitplot(aryx,aryy,err=[],xlabel='1-cos(Geomagnetic Angle)',ylabel='Radio Pulse Height',title='',psfile=T,filename='results-pulseheight-vs-ang2B.ps')
#ylabel='pulse height [µV/m/MHz]'

aryx:=sin(angd[good])
aryy:=hc[good]
fitplot(aryx,aryy,err=[],xlabel='sin(Geomagnetic Angle)',ylabel='Radio Pulse Height',title='',psfile=T,filename='results-pulseheight-vs-ang2B-sin.ps')

aryx:=meandist[good]
aryy:=log(hc[good])
fitplot(aryx,aryy,err=[],xlabel='Distance from Shower Axis [m]',ylabel='log (Radio Pulse Height)',title='',psfile=T,filename='results-pulseheight-vs-radius.ps')

aryx:=cos(angd[good])
aryy:=hc[good]/(10^(logE[good]-8))
resb:=fitplot(aryx,aryy,xlabel='cos(Geomagnetic Angle)',ylabel='Pulse Height/Energy',title='',psfile=T,filename='results-normalized-energy-pulseheight-vs-ang2B.ps')

plot(evtab.ZE[good]/degree,hc[good],lines=F,xlabel='Zenith Angle [deg]',ylabel='Radio Pulse Height',title='',psfile=T,filename='results-pulseheight-vs-zenith-angle.ps',col=4,clear=T)

plot(evtab.AZ[inclined]/degree,hc[inclined],lines=F,xlabel='Azimuth [deg]',ylabel='Radio Pulse Height',title='',psfile=T,filename='results-pulseheight-vs-azimuth-angle.ps',col=4,clear=T)

fitplot(log(ncorr.Nmu[good]),log(hc[good]),xlabel='log(Muon Number)',ylabel='log(Radio Pulse Height)',title='',psfile=T,filename='results-pulseheight-vs-muons.ps')

fitplot(log(ncorr.Ne[good]),log(hc[good]),xlabel='log(Electron Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-pulseheight-vs-electrons.ps')

fitplot(log(ncorr.Nmu[select]),log(hcomp[select]),xlabel='log(Muon Number)',ylabel='log(Radio Pulse Height)',psfile=F)
plotarrows(log(ncorr.Nmu[bad]),log(hcomp[bad]),col=5,arrlen=0.1)
plot(log(ncorr.Nmu[bad]),log(hcomp[bad]),lines=F,clear=F,col=5,psfile=T,filename='results-allevents-pulseheight-vs-muons.ps')

fitplot(log(ncorr.Ne[select]),log(hcomp[select]),xlabel='log(Electron Number)',ylabel='log(Radio Pulse Height)',psfile=F)
plotarrows(log(ncorr.Ne[bad]),log(hcomp[bad]),col=5,arrlen=0.1);
plot(log(ncorr.Ne[bad]),log(hcomp[bad]),lines=F,clear=F,col=5,psfile=T,filename='results-allevents-pulseheight-vs-electrons.ps')


#-BEGIN------------------------------------------------------------------
# do the plots and single fits 
#-END--------------------------------------------------------------------

aryx:=sin(angd[good])
aryy:=hc[good]/ncorr.Nmu[good]*1e6
errb:=[]
if (doerrors) errb:= errs[good]/ncorr.Nmu[good]*1e6;angfunc
resb:=fitplot(aryx,aryy,err=errb,xlabel='sin(Geomagnetic Angle)',ylabel='Pulse Height/Nµ',title='',psfile=T,filename='results-normalized-pulseheight-vs-ang2B-sin.ps',scaled=T)
if (doerrors){
  sinprof := gen_profile(aryx,aryy,weight=1/errb,bins=enprofbins,eqfill=T);
  plot_profile(sinprof,xlabel='sin(Geomagnetic Angle)',ylabel='Pulse Height/Nµ',psfile=T,filename='results-profile-pulseheight-vs-ang2B-sin.ps',dofit=T)
}
corrbsin:=resb.f(1)/resb.f(sin(angd))

aryx:=sin(angd[good])^2
aryy:=hc[good]/ncorr.Nmu[good]*1e6
errb:=[]
if (doerrors) errb:= errs[good]/ncorr.Nmu[good]*1e6;angfunc
resb:=fitplot(aryx,aryy,err=errb,xlabel='sin**2(Geomagnetic Angle)',ylabel='Pulse Height/Nµ',title='',psfile=T,filename='results-normalized-pulseheight-vs-ang2B-sin^2.ps',scaled=T)
if (doerrors){
  sinprof := gen_profile(aryx,aryy,weight=1/errb,bins=enprofbins,eqfill=T);
  plot_profile(sinprof,xlabel='sin**2(Geomagnetic Angle)',ylabel='Pulse Height/Nµ',psfile=T,filename='results-profile-pulseheight-vs-ang2B-sin^2.ps',dofit=T)
}

print '----'

aryx:=angfunc[good]
aryy:=hc[good]/ncorr.Nmu[good]*1e6
errb:=[]
if (doerrors) { errb:= errs[good]/ncorr.Nmu[good]*1e6 }
resb:=fitplot(aryx,aryy,err=errb,xlabel='1-cos(Geomagnetic Angle)',ylabel='Pulse Height/Nµ',title='',psfile=T,filename='results-normalized-pulseheight-vs-ang2B.ps',scaled=T)
if (doerrors){
  cosprof := gen_profile(aryx,aryy,weight=1/errb,bins=enprofbins,eqfill=T);
  plot_profile(cosprof,xlabel='1-cos(Geomagnetic Angle)',ylabel='Pulse Height/Nµ',psfile=T,filename='results-profile-pulseheight-vs-ang2B.ps',dofit=T)
}
corrb:=resb.f(1)/resb.f(angfunc)

aryx:=meandist[good]
aryy:=log(hc[good]*corrb[good]/(ncorr.Nmu[good])*1e6)
err:=[];
if (doerrors) {
  aryy_nolog := 10^aryy
  err1:= errs[good]*corrb[good]/ncorr.Nmu[good]*1e6
  err := log(aryy_nolog+err1)-aryy;
};
resr:=fitplot(aryx,aryy,err=err,xlabel='Distance from Shower Axis [m]',ylabel='log(Radio Pulse Height/Nµ)',title='',psfile=T,filename='results-normalized-pulseheight-vs-radius.ps')

corrr:=10^(resr.f(100))/10^(resr.f(meandist))

aryx:=meandist[good]
aryy:=log(hc[good]/(ncorr.Nmu[good])*1e6)
err:=[];
if (doerrors) {
  aryy_nolog := 10^aryy
  err1:= errs[good]/ncorr.Nmu[good]*1e6
  err := log(aryy_nolog+err1)-aryy;
};
resr:=fitplot(aryx,aryy,err=err,xlabel='Distance from Shower Axis [m]',ylabel='log(Radio Pulse Height/Nµ)',title='',psfile=T,filename='results-normalized-pulseheight-vs-radius-noang2b.ps')


err := []; aryx := evtab.ZE[good]/degree;
aryy := hc[good]*corrr[good]*corrb[good]/ncorr.Nmu[good]*1e6;
if (doerrors) { err:= errs[good]*corrr[good]*corrb[good]/ncorr.Nmu[good]*1e6 }
plot(aryx,aryy,lines=F,xlabel='Zenith Angle [deg]',ylabel='Normalized Pulse Height',title='',psfile=T,filename='results-normalized-pulseheight-vs-zenith-angle.ps',col=4,clear=T)


err := []; aryx := evtab.AZ[inclined]/degree #aryx := 180-abs(180-evtab.AZ[inclined]/degree);
aryy := hc[inclined]*corrr[inclined]*corrb[inclined]/ncorr.Nmu[inclined]*1e6;
if (doerrors) { 
  err:= errs[inclined]*corrr[inclined]*corrb[inclined]/ncorr.Nmu[inclined]*1e6
};
plot(aryx,aryy,lines=F,xlabel='Azimuth Angle [deg]',ylabel='Normalized Pulse Height',title='(events with ZE>17 degree)',psfile=T,filename='results-normalized-pulseheight-vs-azimuth-angle.ps',col=4,clear=T)
plot(aryx,aryy,err,lines=F,xlabel='Azimuth Angle [deg]',ylabel='Normalized Pulse Height',title='',psfile=T,filename='results-normalized-pulseheight-vs-azimuth-angle-errs.ps',col=4,clear=T,yrange=[0.9,12])

plot(aryx,aryy,lines=F,xlabel='Azimuth Angle [deg]',ylabel='Normalized Pulse Height',psfile=T,filename='results-normalized-pulseheight-vs-azimuth-angle-notit.ps',col=4,clear=T)

azprof := gen_profile(aryx,aryy,weight=1/err,bins=enprofbins,eqfill=T);
plot_profile(azprof,xlabel='Azimuth Angle [deg]',ylabel='Normalized Pulse Height',psfile=T,filename='results-profile-pulseheight-vs-azimuth-angle.ps',dofit=F)


fitplot(log(ncorr.Nmu[select]),log(hcomp[select]*corrb[select]*corrr[select]),xlabel='log(Muon Number)',ylabel='log(Radio Pulse Height)',title='',psfile=F)
plotarrows(log(ncorr.Nmu[bad]),log(hcomp[bad]*corrb[bad]*corrr[bad]),col=5,arrlen=0.1)
plot(log(ncorr.Nmu[bad]),log(hcomp[bad]*corrb[bad]*corrr[bad]),lines=F,clear=F,col=5,psfile=T,filename='results-normalized-allevents-pulseheight-vs-muons.ps')

fitplot(log(ncorr.Ne[select]),log(hcomp[select]*corrb[select]*corrr[select]),xlabel='log(Electron Number)',ylabel='log(Radio Pulse Height)',psfile=F)
plotarrows(log(ncorr.Ne[bad]),log(hcomp[bad]*corrb[bad]*corrr[bad]),col=5,arrlen=0.1);
plot(log(ncorr.Ne[bad]),log(hcomp[bad]*corrb[bad]*corrr[bad]),lines=F,clear=F,col=5,psfile=T,filename='results-normalized-allevents-pulseheight-vs-electrons.ps')

goodsize:=good & ncorr.Ne < 5e7

aryx:=ncorr.Ne[goodsize]
aryy:=hc[goodsize]*corrb[goodsize]*corrr[goodsize]
err:=[]
if (doerrors) err := errs[goodsize]*corrb[goodsize]*corrr[goodsize];
resel := fitplot(aryx,aryy,err=err,logx=T,logy=T,xlabel='log(Electron Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-pulseheight-vs-electrons.ps')
correl := resel.f(10^7)/resel.f(ncorr.Ne)

aryx:=log(ncorr.Ne[goodsize])
aryy:=log(hc[goodsize]*corrb[goodsize]*corrr[goodsize])
err := []
if (doerrors) {
  aryy_nolog := 10^aryy;
  err1:= errs[goodsize]*corrb[goodsize]*corrr[goodsize];
  err := log(aryy_nolog+err1)-aryy;
};
fitplot(aryx,aryy,err=err,logx=F,logy=F,xlabel='log(Electron Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-pulseheight-vs-electrons-gfit.ps')

aryy:=hc[good]*corrr[good]
aryx:=ncorr.Nmu[good]
err:=[]
if (doerrors) err:= errs[good]*corrr[good]
resmunob:=fitplot(aryx,aryy,err=err,logx=T,logy=T,xlabel='log(Muon Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-no-angb-pulseheight-vs-muons.ps')

aryy:=hc[good]*corrbsin[good]*corrr[good]
aryx:=ncorr.Nmu[good]
err:=[]
if (doerrors) err:=errs[good]*corrbsin[good]*corrr[good]
resmu:=fitplot(aryx,aryy,err=err,logx=T,logy=T,xlabel='log(Muon Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-pulseheight-vs-muons-sin.ps')

aryy:=log(hc[good]*corrb[good]*corrr[good])
aryx:=log(ncorr.Nmu[good])
err:=[]
if (doerrors) {
  aryy_nolog := 10^aryy
  err1:= errs[good]*corrb[good]*corrr[good]
  err := log(aryy_nolog+err1)-aryy;
};
fitplot(aryx,aryy,err,logx=F,logy=F,xlabel='log(Muon Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-pulseheight-vs-muons-gfit.ps')

aryy:=hc[good]*corrb[good]*corrr[good]
aryx:=ncorr.Nmu[good]
errmu:=[]
if (doerrors) errmu:=errs[good]*corrb[good]*corrr[good]
resmuon:=fitplot(aryx,aryy,err=errmu,logx=T,logy=T,xlabel='log(Muon Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-pulseheight-vs-muons.ps')

corrmuon := resmuon.f(10^6)/resmuon.f(ncorr.Nmu)

aryy:=(hc[good]*corrb[good]*corrr[good])
aryx:=10^logE[good]
err:=[]
if (doerrors) err:= errs[good]*corrb[good]*corrr[good];
resen := fitplot(aryx,aryy,err,logx=T,logy=T,xlabel='log(Primary Energy/GeV)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-pulseheight-vs-energy-LL.ps')

corren := resen.f(10^8)/resen.f(10^logE)

aryy:=hc[good]*corrb[good]*corrr[good]
aryx:=10^logE[good]
err:= errs[good]*corrb[good]*corrr[good];
wgh := 1/err
enprof := gen_profile(aryx,aryy,wgh,bins=enprofbins,eqfill=T);
plot_profile(enprof,xlabel='log(Primary Energy/GeV)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-profile-pulseheight-vs-energy-LL.ps',dofit=T,loglog=T)

aryybad:=hcomp[bad]*corrb[bad]*corrr[bad]
aryxbad:=10^logE[bad]
errbad:= errs[bad]*corrb[bad]*corrr[bad];
wghbad := 1/(2*errbad)
enprofall :=  gen_profile(aryxbad,aryybad,wghbad,prec=enprof);
plot_profile(enprofall,xlabel='log(Primary Energy/GeV)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-profile-allevents-pulseheight-vs-energy-LL.ps',dofit=T,loglog=T)




##### second iteration:
print '############## second iteration:'

aryx:=angfunc[good]
aryy:=hc[good]*corrr[good]*corrmuon[good]
errb:=[]
if (doerrors) { errb:= errs[good]*corrr[good]*corrmuon[good] }
resb2:=fitplot(aryx,aryy,err=errb,xlabel='1-cos(Geomagnetic Angle)',ylabel='Pulse Height/Nµ',title='',psfile=T,filename='results-normalized-pulseheight-vs-ang2B-2nd.ps',scaled=T)
corrb2:=resb2.f(1)/resb2.f(angfunc)

aryx:=meandist[good]
aryy:=log(hc[good]*corrb2[good]*corrmuon[good])
err:=[];
if (doerrors) {
  aryy_nolog := 10^aryy
  err1:= errs[good]*corrb2[good]*corrmuon[good]
  err := log(aryy_nolog+err1)-aryy;
};
resr2:=fitplot(aryx,aryy,err=err,xlabel='Distance from Shower Axis [m]',ylabel='ln(Radio Pulse Height/Nµ)',title='',psfile=T,filename='results-normalized-pulseheight-vs-radius-2nd.ps')
corrr2:=10^(resr2.f(100))/10^(resr2.f(meandist))

aryy:=hc[good]*corrb2[good]*corrr2[good]
aryx:=ncorr.Nmu[good]
errmu:=[]
if (doerrors) errmu:=errs[good]*corrb2[good]*corrr2[good]
resmuon2:=fitplot(aryx,aryy,err=errmu,logx=T,logy=T,xlabel='log(Muon Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-pulseheight-vs-muons-2nd.ps')
corrmuon2 := resmuon2.f(10^6)/resmuon2.f(ncorr.Nmu)

aryy:=(hc[good]*corrb[good]*corrr[good])
aryx:=10^logE[good]
err:=[]
if (doerrors) err:= errs[good]*corrb[good]*corrr[good];
resen2 := fitplot(aryx,aryy,err,logx=T,logy=T,xlabel='log(Primary Energy/GeV)',ylabel='log(Radio Pulse Height)',psfile=F,filename='results-normalized-pulseheight-vs-energy-LL.ps')

corren2 := resen2.f(10^8)/resen2.f(10^logE)

##### third iteration:
print '############## third iteration:'

aryx:=angfunc[good]
aryy:=hc[good]*corrr2[good]*corrmuon2[good]
errb:=[]
if (doerrors) { errb:= errs[good]*corrr2[good]*corrmuon2[good] }
resb3:=fitplot(aryx,aryy,err=errb,xlabel='1-cos(Geomagnetic Angle)',ylabel='Normalized Pulse Height',title='',psfile=T,filename='results-normalized-pulseheight-vs-ang2B-3rd.ps',scaled=T)
corrb3:=resb3.f(1)/resb3.f(angfunc)


cosprof := gen_profile(aryx,aryy,weight=1/errb,bins=enprofbins,eqfill=T);
plot_profile(cosprof,xlabel='1-cos(Geomagnetic Angle)',ylabel='Normalized Pulse Height',psfile=T,filename='results-profile-pulseheight-vs-ang2B-3rd.ps',dofit=T)



aryx:=meandist[good]
aryy:=ln(hc[good]*corrb3[good]*corrmuon2[good])
err:=[];
if (doerrors) {
  aryy_nolog := exp(aryy)
  err1:= errs[good]*corrb3[good]*corrmuon2[good]
  err := ln(aryy_nolog+err1)-aryy;
};
resr3:=fitplot(aryx,aryy,err=err,xlabel='Distance from Shower Axis [m]',ylabel='ln(Radio Pulse Height)',title='',psfile=T,filename='results-normalized-pulseheight-vs-radius-3rd.ps')
corrr3:=exp(resr3.f(100))/exp(resr3.f(meandist))

rprof3 := gen_profile(aryx,aryy,(1/err),bins=enprofbins,eqfill=F,logx=T);
plot_profile(rprof3,xlabel='Distance from Shower Axis [m]',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-profile-pulseheight-vs-radius-3rd.ps',dofit=T,loglog=F,xmean=F)

aryy:=hc[good]*corrb3[good]*corrr3[good] 
aryx:=ncorr.Nmu[good]
errmu:=[]
if (doerrors) errmu:=errs[good]*corrb3[good]*corrr3[good]
resmuon3:=fitplot(aryx,aryy,err=errmu,logx=T,logy=T,xlabel='log(Muon Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-pulseheight-vs-muons-3rd.ps')
corrmuon3 := resmuon3.f(10^6)/resmuon3.f(ncorr.Nmu)

aryy:=hc[good]*corrb3[good]*corrr3[good]
aryx:=ncorr.Nmu[good]
err:= errs[good]*corrb3[good]*corrr3[good];
wgh := 1/err
muprof3 := gen_profile(aryx,aryy,wgh,bins=enprofbins,eqfill=F,logx=T);
plot_profile(muprof3,xlabel='log(Muon Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-profile-pulseheight-vs-muons-3rd.ps',dofit=T,loglog=T,xmean=F)
xval := [5,5.5,6,6.3]
plot(xval,log(resmuon3.f(10^(xval))),lines=T,clear=F,psfile=T,col=3,filename='results-profile-pulseheight-vs-muons-3rd-line.ps')


aryy:=hc[good]*corrb3[good]*corrr3[good]
aryx:=10^logE[good]
err:=[]
if (doerrors) err:= errs[good]*corrb3[good]*corrr3[good];
resen3 := fitplot(aryx,aryy,err,logx=T,logy=T,xlabel='log(Primary Energy/GeV)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-pulseheight-vs-energy-LL-3rd.ps')

corren3 := resen3.f(10^8)/resen3.f(10^logE)

aryy:=hc[good]*corrb3[good]*corrr3[good]
aryx:=10^logE[good]
err:= errs[good]*corrb3[good]*corrr3[good];
wgh := 1/err
enprof3 := gen_profile(aryx,aryy,wgh,bins=enprofbins,eqfill=F,logx=T);
plot_profile(enprof3,xlabel='log(Primary Energy/GeV)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-profile-pulseheight-vs-energy-LL-3rd.ps',dofit=T,loglog=T,xmean=F)
xval := [7.6,8,8.5,8.8]
plot(xval,log(resen3.f(10^(xval))),lines=T,clear=F,psfile=T,col=3,filename='results-profile-pulseheight-vs-energy-LL-3rd-line.ps')


aryy:=hc[good]*corrb3[good]*corrr3[good]
aryx:=ncorr.Ne[good]
err:= errs[good]*corrb3[good]*corrr3[good];
resel:=fitplot(aryx,aryy,err=err,logx=T,logy=T,xlabel='log(Electron Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-pulseheight-vs-electrons-3rd.ps')
wgh := 1/err
elprof3 := gen_profile(aryx,aryy,wgh,bins=enprofbins,eqfill=F,logx=T);
plot_profile(elprof3,xlabel='log(Electron Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-profile-pulseheight-vs-electrons-3rd.ps',dofit=T,loglog=T,xmean=F)
xval := [6.1,6.5,7,7.5,7.9]
plot(xval,log(resel.f(10^(xval))),lines=T,clear=F,psfile=T,col=3,filename='results-profile-pulseheight-vs-electrons-3rd-line.ps')


dwb := 1023*(1/cos(evtab.ZE[good])-1.025);
logne0 := log(ncorr.Ne[good])+dwb/158./2.302585093
ne0 := 10^logne0;
aryy:=hc[good]*corrb3[good]*corrr3[good]
err:= errs[good]*corrb3[good]*corrr3[good];
fitplot(ne0,aryy,err=err,logx=T,logy=T,xlabel='log(Corrected Electron Number)',ylabel='log(Radio Pulse Height)',psfile=T,filename='results-normalized-pulseheight-vs-electrons-zecorr-3rd.ps')



err := []; aryx := evtab.ZE[good]/degree;
#aryx := cos(evtab.ZE[good])
aryy := hc[good]*corrr3[good]*corrb3[good]*corrmuon3[good]
if (doerrors) { err:= errs[good]*corrr3[good]*corrb3[good]*corrmuon3[good] }
plot(aryx,aryy,err,lines=F,xlabel='Zenith Angle [deg]',ylabel='Normalized Pulse Height',title='',psfile=T,filename='results-normalized-pulseheight-vs-zenith-angle-errs.ps',col=4,clear=T,yrange=[0.9,12])
zesel := aryx<100.94
#fitplot(aryx[zesel],aryy[zesel],err[zesel],xlabel='cos(Zenith Angle)',ylabel='Normalized Pulse Height',psfile=T,filename='results-normalized-pulseheight-vs-zenith-angle-fit.ps')
zenprof := gen_profile(aryx,aryy,weight=1/err,bins=enprofbins,eqfill=F);
plot_profile(zenprof,xlabel='Zenith Angle [deg]',ylabel='Normalized Pulse Height',psfile=T,filename='results-profile-pulseheight-vs-zenith-angle.ps',dofit=F)


inclined:=good & evtab.ZE>0.3
err := []; aryx := evtab.AZ[inclined]/degree #aryx := 180-abs(180-evtab.AZ[inclined]/degree);
aryy := hc[inclined]*corrr3[inclined]*corrb3[inclined]*corrmuon3[inclined]
if (doerrors) { 
  err:= errs[inclined]*corrr3[inclined]*corrb3[inclined]*corrmuon3[inclined]
};
plot(aryx,aryy,err,lines=F,xlabel='Azimuth Angle [deg]',ylabel='Normalized Pulse Height',title='',psfile=T,filename='results-normalized-pulseheight-vs-azimuth-angle-errs.ps',col=4,clear=T,yrange=[0.9,7])



#-BEGIN------------------------------------------------------------------
# Do the combined fit
#-END--------------------------------------------------------------------

myfit := functionfitter();
# p0 = scaling factor; p1 = charge-sep. fraction; p2 = typ. Radius; p3 = slope
# x0 = geomagnetic angle; x1 = meandist; x2 = muon number/energy
funcstring := 'p0*(1+p1-cos(x0))*exp(x1*p2/-100)*(x2/1e6)^p3'
#funcstring := 'p0*(1+p1-cos(x0))*exp(x1*p2/-100)*(x2/1e8)^p3*cos(x3)'
allfunc := dfs.compiled(funcstring)
myfit.setfunction(allfunc)
params := [1,0,0.5,1]
paramerrs := [1,0.1,0.1,0.1]
#params := [1,resb3.coeff[2],(resr3.coeff[2]*-100),resmuon3.coeff[3]]
#paramerrs := [1,resb3.coefferrs[2],(resr3.coefferrs[2]*100),resmuon3.coefferrs[2]]
#params := [1,0,(resr3.coeff[2]*-100),resen3.coeff[3]]
#paramerrs := [1,0,(resr3.coefferrs[2]*100),resen3.coefferrs[2]]
myfit.setparameters(params)
npar:=4
fixed:=array(F,npar)
y := hc[good]
x := rbind(angd[good],meandist[good],ncorr.Nmu[good])
x := x[1:(len(y)*3)]
#x := rbind(angd[good],meandist[good],(10^(logE[good])),evtab.ZE[good])
#x := x[1:(len(y)*4)]
err := errs[good]
myfit.setdata(x,y,err);
coeff:=myfit.fit(linear=F,fixed=fixed)
coefferrs:=myfit.geterror();
chisq:=myfit.getchisq();
chisqred:=chisq/(len(y)-npar)
print '*******************************************'
print 'Total-fit for Parameterset:',Parameterset;
print 'Function:',funcstring
print 'params    =',params
print 'paramerrs =',paramerrs
print 'coeffs    =',coeff
print 'coeffserr =',coefferrs
print 'chisq     =',chisq
print 'chisqred  =',chisqred
print '*******************************************'
allfunc.setparameters(coeff)


fun := function(par,angb,dist,Nmu) { return par[1]*(1+par[2]-cos(angb))*exp(dist*par[3])*(Nmu/1e6)^par[4]}

par := coeff
#par := [4.04016443, 0.0665981773, -0.00424513235, 0.912707426]
#aryy := fun(par,angd[bad],meandist[bad],ncorr.Nmu[bad])/errs[bad]
#pg.clear()
#pg.sch(globalpar.PrintCharHeight);
#pg.slw(globalpar.PrintLineWidth);
#pg.sci(globalpar.PrintLabelCol);
#pg.env(0,8,0,19,0,0);
#pg.lab('Estimated Peak Height/Noise','N (events)','')
#pg.sci(4)
#pg.hist(aryy,0,8,35,1)
#pg.sci(1)
#pg.postscript('results-histogram-significane-bad-events.ps')











#-BEGIN------------------------------------------------------------------
# Other stuff
#-END--------------------------------------------------------------------

inclined:=good & evtab.ZE>0.3
aryx := evtab.AZ[inclined]/degree;
aryy := hc[inclined]*corrr3[inclined]*corrb3[inclined]/ncorr.Nmu[inclined]*1e6
err:= errs[inclined]*corrr3[inclined]*corrb3[inclined]/ncorr.Nmu[inclined]*1e6
#plot(aryx,aryy,lines=F,xlabel='Azimuth Angle [deg]',ylabel='Normalized Pulse Height',title='(events with ZE>17 degree)',psfile=T,filename='results-normalized-pulseheight-vs-azimuth-angle.ps',col=4,clear=T)
plot(aryx,aryy,err=[],lines=F,xlabel='Azimuth Angle [deg]',ylabel='Normalized Pulse Height',psfile=T,filename='results-normalized-pulseheight-vs-azimuth-angle-notit.ps',col=4,clear=T)
azprof := gen_profile((aryx),aryy,weight=1/err,bins=8,eqfill=F);
#azprof := gen_profile(((aryx+45)%360-45),aryy,weight=1/err,bins=4,eqfill=F);
plot_profile(azprof,xlabel='Azimuth Angle [deg]',ylabel='Normalized Pulse Height',psfile=T,filename='results-profile-pulseheight-vs-azimuth-angle.ps',dofit=F)

inclsel := select & evtab.ZE>0.3
pg2.sci(1)
pg2.clear()
pg2.sch(globalpar.PrintCharHeight);
pg2.slw(globalpar.PrintLineWidth);
pg2.sci(globalpar.PrintLabelCol);
pg2.env(0,360,0,14,0,0);
pg2.lab('Azimuth angle [deg]','N (events)','')
pg2.sci(1)
pg2.sls(2)
pg2.hist(evtab.AZ[inclsel]/pi*180,0,360,36,1)
pg2.sls(1)
pg2.sci(4)
pg2.hist(evtab.AZ[inclined]/pi*180,0,360,36,1)
pg2.postscript('results-histogram-detected-azimuth-inclined.ps')

get_correction_values := function(){
  freq := data.get('Frequency',1);
  erg := array(0.,len(evtab.GT));
  for (i in seq(len(evtab.GT)) ){
    az := evtab.AZ[i]/degree;
    el := 90-evtab.ZE[i]/degree;
    erg[i] := sqrt(mean(getgains(az,el,freq,globalpar.AntennaGainRec)));
  }
  return erg;
}



#### rerun function
rerun:=function(){include add_dir('crresults.g')}






