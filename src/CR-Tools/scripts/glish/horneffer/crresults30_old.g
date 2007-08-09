#data.load(1,F,5); include '../guipipeline-tmp2/guipipeline-results.1-14-sticky.g'; include '../guipipeline-tmp2/guipipeline-results.data.g';
#linclude('crresults30.g')

good_events := [1,2,3,5,6,7,8,10,11,12,13,14]
korrfakt:= [2.004,1.826,1.987,1.80568,2.006, 1.961,1.792,1.8229,1.6169,1.866, 1.882,2.003,1.5335,1.941]

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

globalpar.PrintingPlots:=T

ncr:=len(sel);
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
  angd[i]:= ang2B(evtab.AZ[i],evtab.ZE[i]);
  nant[i]:=len(flag[i][flag[i]])
  meandist[i] := mean(dists[i][flag[i]]);
  gerr_f[i] := get_gainerror_fact(evtab.AZ[i],evtab.ZE[i],dalpha=0.3)
}
ncorr := korr_Ne_Nmu(evtab.SIZE,evtab.LMUO)
logE := getCRen(ncorr.Ne,ncorr.Nmu,evtab.ZE)

good := array(F,ncr)
good[good_events] := T;

detect:=select:=good;

bad:=(!detect) & select;
inclined:=good & evtab.ZE>0.3

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

hcomp *:= korrfakt;
hc *:= korrfakt;
errs *:= korrfakt;

#-BEGIN------------------------------------------------------------------
# Calculate the Horneffer-Formula with errors
#-END--------------------------------------------------------------------

LOPES10_est := function(angb,dist,Nmu=0.,logE=0.,ZE=10.){
  erg := [=]
  if (Nmu != 0.) {
    erg.value := 8.4*(1.07-cos(angb))*exp(dist/-290)*(Nmu/1e6)^1;
    errp1 := ((1.07-cos(angb))*exp(dist/-290)*(Nmu/1e6)^1)*1.2
    errp2 := (8.4*exp(dist/-290)*(Nmu/1e6)^1)*0.02
    errp3 := (8.4*(1.07-cos(angb))*exp(dist/-290)*(Nmu/1e6)^1)*(dist/(290^2))*97;
    errp4 := (8.4*(1.07-cos(angb))*exp(dist/-290)*(Nmu/1e6)^1)*ln(Nmu/1e6)*0.06;
    erg.err := sqrt(errp1^2+errp2^2+errp3^2+errp4^2);
  } else if ((logE != 0.) & (ZE !=10.)) {
    erg.value := 2.1*(1.1-cos(angb))*exp(dist/-200)*(10^(logE-8)^0.91)*cos(ZE);
    errp1 := ((1.1-cos(angb))*exp(dist/-200)*(10^(logE-8)^0.91*cos(ZE)))*0.3
    errp2 := (2.1*exp(dist/-200)*(10^(logE-8)^0.91*cos(ZE)))*0.03
    errp3 := (2.1*(1.1-cos(angb))*exp(dist/-200)*(10^(logE-8)^0.94*cos(ZE)))*(dist/(200^2))*45;
    errp4 := (2.1*(1.1-cos(angb))*exp(dist/-200)*(10^(logE-8)^0.94*cos(ZE)))*ln(10^(logE-8))*0.07;
    erg.err := sqrt(errp1^2+errp2^2+errp3^2+errp4^2);
  } else {
    print 'Neither Nmu nor (logE and ZE) specified! Aborting.';
    erg := F;
  }
  return erg;
}

LOPES30_est := function(angb,dist,Nmu=0.,logE=0.,ZE=10.){
  erg := [=]
  if (Nmu != 0.) {
    erg.value := 43*(1.07-cos(angb))*exp(dist/-290)*(Nmu/1e6)^1;
    errp1 := ((1.07-cos(angb))*exp(dist/-290)*(Nmu/1e6)^1)*6.6
    errp2 := (43*exp(dist/-290)*(Nmu/1e6)^1)*0.02
    errp3 := (43*(1.07-cos(angb))*exp(dist/-290)*(Nmu/1e6)^1)*(dist/(290^2))*97;
    errp4 := (43*(1.07-cos(angb))*exp(dist/-290)*(Nmu/1e6)^1)*ln(Nmu/1e6)*0.06;
    erg.err := sqrt(errp1^2+errp2^2+errp3^2+errp4^2);
  } else if ((logE != 0.) & (ZE !=10.)) {
    erg.value := 12*(1.1-cos(angb))*exp(dist/-200)*(10^(logE-8)^0.91)*cos(ZE);
    errp1 := ((1.1-cos(angb))*exp(dist/-200)*(10^(logE-8)^0.91*cos(ZE)))*1.8
    errp2 := (12*exp(dist/-200)*(10^(logE-8)^0.91*cos(ZE)))*0.03
    errp3 := (12*(1.1-cos(angb))*exp(dist/-200)*(10^(logE-8)^0.91*cos(ZE)))*(dist/(200^2))*45;
    errp4 := (12*(1.1-cos(angb))*exp(dist/-200)*(10^(logE-8)^0.91*cos(ZE)))*ln(10^(logE-8))*0.07;
    erg.err := sqrt(errp1^2+errp2^2+errp3^2+errp4^2);
  } else {
    print 'Neither Nmu nor (logE and ZE) specified! Aborting.';
    erg := F;
  }
  return erg;
}

lopes_en := function(hc,angb=0.,dist=0.,ZE=10.){
  return (hc/6.5/(1.1-cos(angb))/exp(dist/-200)/cos(ZE))^(1/0.94)
};


#-BEGIN------------------------------------------------------------------
# Fit the fudge-factor from the LOEPS10 results
#-END--------------------------------------------------------------------

myfit := functionfitter();
# x0 = geomagnetic angle; x1 = meandist; x2 = muon number/energy; x3 = zenith
#funcstring := 'p0*8.4*(1.07-cos(x0))*exp(x1/-290)*(x2/1e6)^1'
funcstring := 'p0*2.1*(1.1-cos(x0))*exp(x1/-200)*(x2/1e8)^0.91*cos(x3)'
allfunc := dfs.compiled(funcstring)
myfit.setfunction(allfunc)
params := [1];
paramerrs := [1];
myfit.setparameters(params)
npar:=1
fixed:=array(F,npar)
y := hc[good]
#x := rbind(angd[good],meandist[good],ncorr.Nmu[good])
#x := x[1:(len(y)*3)]
x := rbind(angd[good],meandist[good],(10^(logE[good])),evtab.ZE[good])
x := x[1:(len(y)*4)]
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


L10vals_mu := LOPES10_est(angd,meandist,ncorr.Nmu);
L10vals_en := LOPES10_est(angd,meandist,logE=logE,ZE=evtab.ZE);

fvals := (hc/L10vals_mu.value)
ferrs := sqrt((1/L10vals_mu.value*errs)^2+(hc/(L10vals_mu.value^2)*L10vals_mu.err)^2)
Yint := sum((fvals/ferrs)[good]);
WEIGHTint := sum((1/ferrs)[good]);
Ysqint := sum(((fvals^2)/ferrs)[good]);
fudge_mu := Yint/WEIGHTint;
fudge_mu_err := sqrt((Ysqint/WEIGHTint-fudge_mu^2)/len(fvals[good]));

fvals := (hc/L10vals_en.value)
ferrs := sqrt((1/L10vals_en.value*errs)^2+(hc/(L10vals_en.value^2)*L10vals_en.err)^2)
Yint := sum((fvals/ferrs)[good]);
WEIGHTint := sum((1/ferrs)[good]);
Ysqint := sum(((fvals^2)/ferrs)[good]);
fudge_en := Yint/WEIGHTint;
fudge_en_err := sqrt((Ysqint/WEIGHTint-fudge_en^2)/len(fvals[good]));


print sprintf('Fudge factors: Muon: %4.2f+/-%4.3f; Energy: %4.2f+/-%4.3f;',fudge_mu,fudge_mu_err,fudge_en,fudge_en_err),'!'

xarr := good_events;
#xarr := seq(len(good_events));
plot(xarr,hc[good],err=errs[good],clear=T,xlabel='Event No',ylabel='Pulse height',title='',col=4,yrange=[0.5,19.5],symbol=2)
plot(xarr+0.1,L10vals_mu.value[good]*fudge_mu,err=L10vals_mu.err[good]*fudge_mu,clear=F,col=2,psfile=F,symbol=3)
plot(xarr-0.1,L10vals_en.value[good]*fudge_en,err=L10vals_en.err[good]*fudge_en,clear=F,col=5,psfile=F,symbol=4)

L30vals_mu := LOPES30_est(angd,meandist,ncorr.Nmu);
L30vals_en := LOPES30_est(angd,meandist,logE=logE,ZE=evtab.ZE);
plot(xarr,hc[good],err=errs[good],clear=T,xlabel='Event Number',ylabel='Pulse height [uV/m/MHz]',title='',col=4,yrange=[0.5,19.5],symbol=2)
plot(xarr+0.15,L30vals_mu.value[good],err=L30vals_mu.err[good],clear=F,col=2,psfile=F,symbol=3)
plot(xarr-0.15,L30vals_en.value[good],err=L30vals_en.err[good],clear=F,col=5,psfile=F,symbol=4)

include 'CGpipe2/CGpipeline-results-all.g'
newerrs := sqrt((CCheight_error)^2+(gerr_f*CCheight)^2+(rmsCC)^2+(CCheight*relgainerror)^2);
plot(([1:14]+0.3),CCheight,err=newerrs,clear=F,col=3,psfile=F,symbol=2)
