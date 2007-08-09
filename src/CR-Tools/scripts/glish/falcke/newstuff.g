#This is Heino's work-file which contains unfinished, new routines that are
#being tested ....

include 'gaussian.g'
#-----------------------------------------------------------------------
#Name: flattenRFI
#-----------------------------------------------------------------------
#Type: Function
#Doc: Iteratively cuts out lines that are n*sigma above the mean spectrum
#     and replaces them with the running mean. 
#Ret: Array with lines cut-out and replaced by the mean
#
#Par: ary =	  		- array of input  data
#Par: nsigma =      - array with nsigmas, iteratively flag data which 
#                     are nsigma above the mean. The length of the array
#                     determine the number of iterations
#Par: smooth =      - form the running mean by smoothing the data with
#                     a window of width n=smooth samples
#Par: smooth2 =      - smooth the array that is to be cut also before determining
#                      deviating samples (usefull for broader lines). Return array will not
#                      be affected /smoothed.
#Par: nx1,nx2 =      - determine the noise (for sigma) in the inner region between
#                    - sample nx1,nx2. If negative take -nx1/2% off the left and
#                      right boundary
#Par: cutneg =       - Cuts on positive deviations 
#Par: cutneg =       - Cuts (also) neagtive deviations 
#Par: retratio=      - Returns the ratio between uncut and cut array rather then the
#                      cut array itself (usefull for flagging operations).
#-----------------------------------------------------------------------
flattenRFI:=function(ary,smooth1=100,smooth2=10,nx1=-10,nx2=-10){

  if (nx1<0) {n1:=l/abs(nx1)} else {n1:=nx1}
  if (nx2<0) {n2:=l-l/abs(nx1)} else {n2:=nx2}

  aryy:=ary[n1:n2]; l:=len(aryy);

# Set end points to zero (e.g., take out DC offset)
  avgy1:=blockaverage(aryy,smooth1)
  avgy2:=blockaverage(aryy,smooth2)

  m:=mean(aryy); s:=stddev(aryy);
  m1:=mean+s*smooth1;
  
  aryy[aryy < (mean+s*smooth1)]:=avgy1;

  devy:=avgy2/avgy
  cuty:=aryy
  cut:=sigma*stddev(devy[n1:n2]);
  print 'Pass #',n,'- Cutting Threshold:',roundn(1+cut),'* mean power.'
  for (i in seq(l)) {
    if (i>n1 && i<n2 && ((devy[i] > 1+cut && cutpos) ||  (devy[i] < 1-cut) && cutneg)) {
            cuty[i]:=avgy[i]
    }
  }
  aryy:=cuty
if (retratio) {return aryy/dezero(ary)}
else {return aryy}
}

mydir:="/data/evdata/sel2/"
loadcr:=function(ref rec,i=2,tabfile='/data/evdata/sel2/runs-4700-4744-pipe-sel_large.sos'){
global evtab
evtab:=read_value(tabfile)
filename:=evtab.filename[i]~ globalconst.FilenameExtractor
print i,filename,evtab.XC[i],evtab.YC[i], evtab.AZ[i]/degree,(90-evtab.ZE[i]/degree)
input_event(rec,spaste(mydir,filename))
rec.put([evtab.XC[i],evtab.YC[i]],'XY',-1)
azel:=[evtab.AZ[i]/degree,(90-evtab.ZE[i]/degree)];

plotgui.azent->delete('start','end');
plotgui.azent->insert(as_string(azel[1]))
plotgui.elent->delete('start','end');
plotgui.elent->insert(as_string(azel[2]))
plotgui.setphase()
plotgui.replot()
}

# input_event(data,"/data/evdata/sel/2004.01.12.00:28:11.577.event")


calcrelgain:=function(calcrel=F,smoothwidth=50,refant=7){
global globalpar,globalconst
global aryx,divy,aryy,avgy,avgy2,devy,cuty,resy,gaussary
global n1,n2,l,cut,mn

files:=[
"2003.10.28.11:05:32.805.event",
                      "2003.10.28.11:05:34.781.event",
                       "2003.10.28.11:05:49.161.event",
                       "2003.10.28.11:06:02.393.event",
                       "2003.10.28.11:06:29.245.event",
                       "2003.10.28.11:06:49.518.event",
                       "2003.10.28.11:07:37.853.event"]

files:=[
"sel/2004.01.11.12:05:32.454.event",
"sel/2004.01.12.00:28:11.577.event",
"sel/2004.01.14.06:50:46.139.event",
"sel/2004.01.15.05:11:57.823.event",
"sel/2004.01.15.07:29:10.226.event",
"sel/2004.01.19.06:51:04.009.event",
"sel/2004.01.21.00:22:02.630.event",
"sel/2004.01.21.16:15:46.382.event",
"sel/2004.01.23.05:56:54.939.event",
"sel/2004.01.23.09:51:36.691.event",
"sel/2004.01.27.16:21:55.602.event",
"sel/2004.01.28.01:01:07.012.event",
"sel/2004.01.28.06:44:22.445.event"];


globalpar.dorelgain := F;
input_event(data,spaste("/data/evdata/",files[len(files)]))
aryx:=data.get('Frequency',1)
l:=len(aryx); n1:=l/12; n2:=l-l/9
aryy:=array(0,(max(data.len,10)),l);
cuty:=aryy;
resy:=aryy;
gaussary:=aryx
mn:=array(1,data.len)
n:=0;
print 'Reading',len(files),'files.'
for (f in files) {
  n+:=1
  globalpar.dorelgain := F;
  input_event(data,spaste("/data/evdata/",f))
  print 'Read file #',n,f,'#Antennas=',data.len
  for (j in seq(data.len))  aryy[j,]+:=data.get('AbsFFT',j)
 }
for (j in seq(data.len))  {
  aryy[j,]/:=data.len;
  aryy[j,1]:=aryy[j,2];
  aryy[j,l]:=aryy[j,l-2];
}
if (calcrel) {
  for (j in seq(data.len))  {if (j!=refant) aryy[j,]/:=aryy[refant,]};
  aryy[refant,]/:=aryy[refant,];
} else {
  m2:=mean(aryy[refant,]);
  for (j in seq(data.len)) aryy[j,]/:=m2
}
print 'Blockaveraging data to detect RFI lines.'

for (j in seq(data.len))  {
  cuty[j,]:=filterlines(aryy[j,],nx1=n1,nx2=n2)
  mn[j]:=mean(cuty[j,n1:n2]);
}

print 'Relative gain factors (divide):'
for (j in seq(data.len)) print mean(cuty[j,5000:28000])

width:=smoothwidth; 
print 'Creating Gaussian of Width: ', width
for (i in seq(l)) gaussary[i]:=gaussian1d(i,1,l/2.,width)

print 'Convolving with Gaussian.'
for (j in seq(data.len))  {
   resy[j,]:=global_fftserver.convolve(cuty[j,],gaussary)
   resy[j,1:n1]:=resy[j,n1]; resy[j,n2:l]:=resy[j,n2]
print 'Mean gain factor:', mn[j]
resy[j,]:=resy[j,]/mean(resy[j,n1:n2])
}

if (is_defined('pg')) pg.clear();
for (j in seq(data.len)) plot(log(resy[j,]));

relgains:=[=]
relgains.n:=data.len; 
relgains.gainfacs:=mn; 
relgains.xary:=aryx; 
relgains.yarys:=resy; 
filen:=spaste(globalpar.relgainfile,'.trial')
print 'Writing gains to file',filen,'(remove ".trial" manually)' 
write_value(relgains,filen)
globalpar.dorelgain := T;
};

noturnphase:=function(phi){
  phiret:=(phi+180)%360; 
  if (phiret<=0) {phiret:=phiret+180} else {phiret:=phiret-180};
  return phiret
}


gettvphase:=function(){
global dphi,dphim,dev,sdev
nant:=8
n:=0



files:=[
#"sel/2004.01.11.12:05:32.454.event",
"sel/2004.01.12.00:28:11.577.event"]


files:=[
"2003.10.28.10:58:16.304.event",
"2003.10.28.10:58:32.071.event",
"2003.10.28.10:58:58.861.event",
"2003.10.28.10:59:42.699.event",
"2003.10.28.11:00:44.952.event"]

files:=['lopes-040112-f0001/2004.01.12.00:00:44.066.event',
'lopes-040112-f0001/2004.01.12.00:01:34.607.event',
'lopes-040112-f0001/2004.01.12.00:01:36.066.event',
'lopes-040112-f0001/2004.01.12.00:01:44.315.event',
'lopes-040112-f0001/2004.01.12.00:04:05.843.event',
'lopes-040112-f0001/2004.01.12.00:04:17.261.event',
'lopes-040112-f0001/2004.01.12.00:04:19.914.event',
'lopes-040112-f0001/2004.01.12.00:04:24.897.event',
'lopes-040112-f0001/2004.01.12.00:04:29.553.event',
'lopes-040112-f0001/2004.01.12.00:04:51.941.event',
'lopes-040112-f0001/2004.01.12.00:05:25.876.event',
'lopes-040112-f0001/2004.01.12.00:06:15.061.event',
'lopes-040112-f0001/2004.01.12.00:06:16.822.event',
'lopes-040112-f0001/2004.01.12.00:06:29.059.event',
'lopes-040112-f0001/2004.01.12.00:06:43.591.event',
'lopes-040112-f0001/2004.01.12.00:06:50.281.event',
'lopes-040112-f0001/2004.01.12.00:07:10.670.event',
'lopes-040112-f0001/2004.01.12.00:08:41.282.event',
'lopes-040112-f0001/2004.01.12.00:09:44.170.event',
'lopes-040112-f0001/2004.01.12.00:10:17.041.event',
'lopes-040112-f0001/2004.01.12.00:10:19.521.event',
'lopes-040112-f0001/2004.01.12.00:10:33.113.event',
'lopes-040112-f0001/2004.01.12.00:10:35.174.event',
'lopes-040112-f0001/2004.01.12.00:10:46.005.event',
'lopes-040112-f0001/2004.01.12.00:10:56.649.event',
'lopes-040112-f0001/2004.01.12.00:11:16.006.event',
'lopes-040112-f0001/2004.01.12.00:11:36.722.event',
'lopes-040112-f0001/2004.01.12.00:12:03.902.event',
'lopes-040112-f0001/2004.01.12.00:12:11.121.event',
'lopes-040112-f0001/2004.01.12.00:12:39.617.event',
'lopes-040112-f0001/2004.01.12.00:12:44.230.event',
'lopes-040112-f0001/2004.01.12.00:12:49.380.event',
'lopes-040112-f0001/2004.01.12.00:13:29.431.event',
'lopes-040112-f0001/2004.01.12.00:13:43.463.event',
'lopes-040112-f0001/2004.01.12.00:14:14.612.event',
'lopes-040112-f0001/2004.01.12.00:15:09.023.event',
'lopes-040112-f0001/2004.01.12.00:15:45.100.event',
'lopes-040112-f0001/2004.01.12.00:16:59.629.event',
'lopes-040112-f0001/2004.01.12.00:17:07.845.event',
'lopes-040112-f0001/2004.01.12.00:17:28.843.event',
'lopes-040112-f0001/2004.01.12.00:17:42.002.event',
'lopes-040112-f0001/2004.01.12.00:17:46.787.event',
'lopes-040112-f0001/2004.01.12.00:18:11.192.event',
'lopes-040112-f0001/2004.01.12.00:18:37.195.event',
'lopes-040112-f0001/2004.01.12.00:18:42.976.event',
'lopes-040112-f0001/2004.01.12.00:19:04.912.event',
'lopes-040112-f0001/2004.01.12.00:19:20.192.event',
'lopes-040112-f0001/2004.01.12.00:19:27.033.event',
'lopes-040112-f0001/2004.01.12.00:19:30.333.event',
'lopes-040112-f0001/2004.01.12.00:19:59.270.event',
'lopes-040112-f0001/2004.01.12.00:20:59.344.event',
'lopes-040112-f0001/2004.01.12.00:21:15.063.event',
'lopes-040112-f0001/2004.01.12.00:21:29.402.event',
'lopes-040112-f0001/2004.01.12.00:22:07.689.event',
'lopes-040112-f0001/2004.01.12.00:22:27.636.event',
'lopes-040112-f0001/2004.01.12.00:22:31.459.event',
'lopes-040112-f0001/2004.01.12.00:22:33.758.event',
'lopes-040112-f0001/2004.01.12.00:23:02.248.event',
'lopes-040112-f0001/2004.01.12.00:23:10.413.event',
'lopes-040112-f0001/2004.01.12.00:23:46.072.event',
'lopes-040112-f0001/2004.01.12.00:24:07.066.event',
'lopes-040112-f0001/2004.01.12.00:24:20.302.event',
'lopes-040112-f0001/2004.01.12.00:25:06.758.event',
'lopes-040112-f0001/2004.01.12.00:25:43.705.event',
'lopes-040112-f0001/2004.01.12.00:25:54.187.event',
'lopes-040112-f0001/2004.01.12.00:25:56.818.event',
'lopes-040112-f0001/2004.01.12.00:26:12.490.event',
'lopes-040112-f0001/2004.01.12.00:26:20.128.event',
'lopes-040112-f0001/2004.01.12.00:27:33.880.event',
'lopes-040112-f0001/2004.01.12.00:27:54.895.event',
'lopes-040112-f0001/2004.01.12.00:28:11.577.event',
'lopes-040112-f0001/2004.01.12.00:28:22.224.event',
'lopes-040112-f0001/2004.01.12.00:28:25.568.event',
'lopes-040112-f0001/2004.01.12.00:28:49.350.event',
'lopes-040112-f0001/2004.01.12.00:29:13.666.event',
'lopes-040112-f0001/2004.01.12.00:29:17.241.event',
'lopes-040112-f0001/2004.01.12.00:30:16.179.event',
'lopes-040112-f0001/2004.01.12.00:30:21.294.event',
'lopes-040112-f0001/2004.01.12.00:30:23.869.event',
'lopes-040112-f0001/2004.01.12.00:30:49.593.event',
'lopes-040112-f0001/2004.01.12.00:30:59.722.event',
'lopes-040112-f0001/2004.01.12.00:31:22.834.event',
'lopes-040112-f0001/2004.01.12.00:31:34.578.event',
'lopes-040112-f0001/2004.01.12.00:32:34.973.event',
'lopes-040112-f0001/2004.01.12.00:33:10.447.event',
'lopes-040112-f0001/2004.01.12.00:33:33.772.event',
'lopes-040112-f0001/2004.01.12.00:34:05.628.event',
'lopes-040112-f0001/2004.01.12.00:34:35.159.event',
'lopes-040112-f0001/2004.01.12.00:34:44.445.event',
'lopes-040112-f0001/2004.01.12.00:36:04.479.event',
'lopes-040112-f0001/2004.01.12.00:36:23.512.event',
'lopes-040112-f0001/2004.01.12.00:36:51.570.event']

files:=files[seq(13)]

files:=[
"sel/2004.01.11.12:05:32.454.event",
"sel/2004.01.12.00:28:11.577.event",
"sel/2004.01.14.06:50:46.139.event",
"sel/2004.01.15.05:11:57.823.event",
"sel/2004.01.15.07:29:10.226.event",
"sel/2004.01.19.06:51:04.009.event",
"sel/2004.01.21.00:22:02.630.event",
"sel/2004.01.21.16:15:46.382.event",
"sel/2004.01.23.05:56:54.939.event",
"sel/2004.01.23.09:51:36.691.event",
"sel/2004.01.27.16:21:55.602.event",
"sel/2004.01.28.01:01:07.012.event",
"sel/2004.01.28.06:44:22.445.event"];

files:=[
"sel/2004.01.23.09:51:36.691.event",
"sel/2004.01.27.16:21:55.602.event",
"sel/2004.01.28.01:01:07.012.event",
"sel2/2004.01.31.14:32:27.910.event",
"sel2/2004.02.01.20:36:44.157.event",
"sel2/2004.02.04.04:17:17.122.event",
"sel2/2004.02.06.04:08:15.811.event",
"sel2/2004.02.06.11:20:19.696.event",
"sel2/2004.02.09.18:02:11.922.event",
"sel2/2004.02.10.01:48:14.776.event",
"sel2/2004.02.14.18:16:30.134.event",
"sel2/2004.02.16.02:49:54.174.event"]


  dphi:=array(0,len(files),nant)
  dphim:=array(0,nant)
  sdev:=dphim
  dev:=dphi
  for (f in files) {
    n+:=1
    input_event(data,spaste('/data/evdata/',f))
    print 'Read file #',n,f,'#Antennas=',data.len
    data.setphase();
    for (i in seq(2,nant)) {
      p:=data.get('PhaseDiff',i)[seq(18174,18195,1)];
      mn:=(max(p)+min(p))/2;
      p1:=mean(p[p<mn]);
      p2:=mean(p[p>mn]);
      if (p2-p1<180) {
        dphi[n,i]:=(p1+p2)/2
#        if (n>1 && (dphi[n,i]-dphi[n,i]>180)) dphi[n,i]+:=360
        if (dphi[n,i]<0) dphi[n,i]+:=360
      } else {
        dphi[n,i]:=(p1+360+p2)/2
      }
        dphim[i]+:=dphi[n,i];
    printf('%s%3.2i %s%3.2i %s%7.2f %s%7.2f %s%7.2f %s%7.2f\\n','#',n,'Ant:',i,'p1=',p1,'p2=',p2,'diff=',p2-p1,'phase=',dphi[n,i]);
    }
  }
nsig:=2
dphim/:=n
if (n>1) {
  for (j in seq(nant)) sdev[j]:=stddev(dphi[,j])
  print ' Preflag Average and Standarddeviations:'; 
  printf('%7.2f, ,',dphim); print ' '
  print ' '; printf('%7.2f, ,',sdev); print ' '
  for (j in seq(nant)) {for (i in seq(n)) dev[i,j]:=abs(dphi[i,j]-dphim[j])/sdev[j]}
  for (j in seq(nant)) dphim[j]:=mean(dphi[,j][dev[,j]<nsig])
  for (j in seq(nant)) sdev[j]:=stddev(dphi[,j][dev[,j]<nsig])
}
print 'flag:',dev<nsig
print 'dev:',dev
print 'Result (horizontal: antennas, vertical: events):'
 for (j in seq(n)) {printf('%7.2f, ,',dphi[j,]);print ' '}
  print 'Postflag Average and Standarddeviations:'; 
  printf('%7.2f, ,',dphim); print ' '
  print ' '; printf('%7.2f, ,',sdev); print ' '
}

findtv:=function(){
  global phii,dphi,phioff,azary
  f:=62.194e6
  xn:=data.getxn(f,'Frequency',1)
  fac:=2*pi*f/degree
  n:=data.len;
  dphi:=array(0.,360,n,n)
  phioff:=array(0.,360,n)
  azary:=array(0,360); 
  phii:=array(0,n);
  for (i in seq(n)) {
    data.put([0,90],'AZEL',i);
    data.setphase(index=i)
    phii[i]:=data.getval('Phase',i,xn)+data.getval('PhaseGrad',i,xn)
  }
  for (az in seq(360)) {
    print 'az=',az
    for (i in seq(n)) {
    antpos:=CalcAntPos(data,data.ant(),i)
      for (j in seq(n)) {
       if (i!=j) {
         phiij:=noturnphase(phii[i]-phii[j])
         phimodel:=noturnphase(fac*calcdelay([az,0],antpos[j]))
#        print 'az=',az,'ij=[',i,j,']','phiij=',phiij,'Model=',phimodel
         dphi[az,i,j]:=phimodel-phiij
         phioff[az,i]+:=(dphi[az,i,j])^2
         }
        }
        phioff[az,i]:=sqrt(phioff[az,i])/(n-1)
      }
  azary[az]:=sum(phioff[az,])/(n-1)
  }
}

#phii: ohne fakes
#[-10.4470845 75.4299923 16.0334489 -18.2914306 -19.9083757 -49.2476024 -88.898631 7.43310634]
#


reduceevent:=function(n=2){
global xary,pow,beam,sel
  sel:=array(F,13,8)
  sel[4,]:=[F,F,T,T,T,T,F,F]
  sel[5,]:=[F,T,F,T,F,T,T,T]
  sel[7,]:=[F,F,F,T,T,T,T,T]
  sel[8,]:=[F,F,T,T,T,T,F,F]
  sel[12,]:=[F,F,F,F,T,T,F,F]
  sel[13,]:=[F,F,T,T,T,F,F,F]
#7,8,9?
#6,10,11: zu weit weg
#12 knapp daneben, kohärenter spike in der Mitte??
  loadcr(data,n)
  for (i in seq(data.len)) data.aux.put(sel[n,i],'Select',i)
  plotantf(plotgui.plotantpg,data,azel=plotgui.AZEL*degree,xy=plotgui.xy)
  data.setphase()
for (j in seq(2)) {
  for (i in seq(data.len)[data.select()]) {
        data.putflag( data.getflag('AbsFFT',index=i)*filterlines(data.get('AbsFFT',i),cutneg=F,smooth2=1,nsigma=[3],retratio=T),field='AbsFFT',index=i); 
  }
}
plotgui.replot();plotgui.aux.replot();plotgui.aux.aux.replot();
plotgui.plotantpg.pg.ptxt(150,160,0,0.5,as_string(n))
plotgui.plotantpg.pg.psprinttofile(spaste('event-sub-',as_string(n),'-layout.ps'))
plotgui.pg.psprinttofile(spaste('event-sub-',as_string(n),'-power.ps'))
plotgui.aux.pg.psprinttofile(spaste('event-sub-',as_string(n),'-e-field.ps'))
plotgui.aux.aux.pg.psprinttofile(spaste('event-sub-',as_string(n),'-beam.ps'))
#   globalpar.SpawnID+:=1; 
#   data.aux:=[=];
#   SpawnRecord(data,data.aux);
#   data.aux:=[=];
#   SpawnRecord(data.aux,data.aux.aux,totype='TIM40beam');
pow:=data.aux.get('f(x)',1)*0.
for (i in seq(1,data.len)[sel[n,]]) pow+:=data.aux.get('f(x)',i)^2
pow/:=len(seq(1,data.len)[sel[n,]])
pow:=blockaverage(pow,3)
beam:=blockaverage(data.aux.aux.get('f(x)',1)^2,3)
xary:=data.get('Time',1)*1e6;
pg.clear(); 
pg.plotxy1(xary[3.26e4:3.27e4],pow[3.26e4:3.27e4]); 
pg.plotxy1(xary[3.26e4:3.27e4],beam[3.26e4:3.27e4]); 
pg.ptxt(-2,500,0,0.5,as_string(n))
pg.psprinttofile(spaste('event-sub-',as_string(n),'-comparison.ps'))
}
# pg.plotxy1(xary[3.26e4:3.27e4],(beam[3.26e4:3.27e4]/pow[3.26e4:3.27e4]))



corrdata:=function(ref rec,field='f(x)',index=-1){
  global pow,corr,rel
  if (len(index)==1 && index<1) {idx:=seq(rec.len)} else {idx:=index}
  corr:=0;
  pow:=rec.get(field,idx[len(idx)])^2
  n:=0; m:=1
  for (i in seq(len(idx)-1)) {
  pow:=pow+rec.get(field,idx[i])^2
  m+:=1
    for (j in seq(i+1,len(idx))) {
      n+:=1;
      print n, i,'*',j
      corr:=corr+rec.get(field,idx[i])*rec.get(field,idx[j]);
    }
  }
pow/:=m
corr/:=n
rel:=corr/pow
return corr
}


