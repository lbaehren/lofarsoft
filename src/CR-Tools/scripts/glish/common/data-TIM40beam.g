#######################################################################
#
# Functions and other stuff for the 40MHz bandwidth TIM-Board
# Here the patches for spawning a data record are defined
#
########################################################################



########################################################################


#-----------------------------------------------------------------------
#Name: TIM40tary2xbeam
#-----------------------------------------------------------------------
#Type: Function
#Doc: calculate the x-beam from an array with all antenna data
#Par: tary		- 2-dim array with the time-domain data from
#			  all antennas
#
#Ret: array with the x-beam data
#-----------------------------------------------------------------------
TIM40tary2xbeam := function(tary){
  global globalpar;

  shap := shape(tary);
  l := shap[1];
  ll := shap[2];
  sigma:=globalpar.xbeam_nsigma

  corr:=0;
  pow := tary[l,]^2; #initialize with data for last antenna, not in the for-loop
  nc:=0;
  np:=1;
  for (i in seq(l-1)) {
    for (j in seq(i+1,l)) {
      nc+:=1;
      corr+:=tary[i,]*tary[j,];
    }
    pow+:=tary[i,]^2;
    np+:=1;
  }
  pow/:=np;
  corr/:=nc

  if (globalpar.xbeam_print) print 'X-Beam method:',globalpar.xbeam_method
  if (globalpar.xbeam_method==1) {
    mc:=mean(corr[l1:l2]); sc:=stddev(corr[l1:l2]);
    if (globalpar.xbeam_print) { print 'Corr[',l1,',',l2,']: mean=',mc,'rms=',sc };
    corrn:=blockaverage((corr-mc)/sc,globalpar.xbeam_n_avg);

    mp:=mean(pow[l1:l2]); sp:=stddev(pow[l1:l2]);
    if (globalpar.xbeam_print) { print 'Pow[',l1,',',l2,']: mean=',mp,'rms=',sp };
    pown:=blockaverage((pow-mp)/sp,globalpar.xbeam_n_avg);

    ampl:=abs((corrn+sigma)/(pow+sigma))
    return (corr-mc)*ampl
  } 

  if ((globalpar.xbeam_method==2)|(globalpar.xbeam_method==4)) {
    l1:=as_integer(ll*globalpar.xbeam_l1); l2:=l1+ll/4;
    mp:=mean(pow[l1:l2]); sp:=stddev(pow[l1:l2]); sigma:=sp*globalpar.xbeam_nsigma
    mc:=mean(corr[l1:l2]); 
    if (globalpar.xbeam_print) { print 'Pow[',l1,',',l2,']: mean=',mp,'rms=',sp };
    if (globalpar.xbeam_print) { print 'Corr[',l1,',',l2,']: mean=',mc};
    pown:=blockaverage((pow-mp),globalpar.xbeam_n_avg);
    corrn:=blockaverage((corr-mc),globalpar.xbeam_n_avg);
    pown[pown<sigma]:=sigma;
    ampl:=abs(corrn/pown)
    if (globalpar.xbeam_method==2) return corr*ampl;
    if (globalpar.xbeam_method==4) return ampl;
  } 

  if (globalpar.xbeam_method==3) {
    ampl:=abs(corr/pow)
    return corr*ampl
  }

  if (globalpar.xbeam_method==4) {
   print 'look above in the code!';
  } 
  return F;
};



#-----------------------------------------------------------------------
#Name: TIM40beamget
#-----------------------------------------------------------------------
#Type: Function
#Doc: get the requested part of the data from a file
#Par: ary		   - dummy array 
#Par: rec		   - array with data sets (header is needed)
#                    rec.ref must contain a string referring
#                    to the data record that functions as the
#                    data source.
#                    data.filter[domain] contains the filter to be applied.
#Par: index		   - index number of data set
#Par: inv		   - reverse direction of conversion
#Ret: array with converted values
#-----------------------------------------------------------------------

TIM40beamget := function(field,ref rec,index=1,inv=F, reffield='',start=-1,end=-1) {
  res:=0
  for (i in seq(rec.ref.len)[rec.ref.select()]) {res+:=rec.ref.get(field,i)}
  return res/len(rec.ref.select()[rec.ref.select()])
}

#-------------------------------------------------------------------------------
#Name: 
#-------------------------------------------------------------------------------
#Type: 
#Doc:  
#-------------------------------------------------------------------------------

TIM40coherencebeamget := function(field,ref rec,index=1,inv=F, reffield='',start=-1,end=-1) {
  if (len(index)==1 && index<1) {idx:=seq(rec.len)} else {idx:=index}
  corr:=0;
  n:=0;
  idx:=seq(rec.ref.len)[rec.ref.select()]
  l:=len(idx)
  tary:=array(0,l,len(rec.ref.get(field,idx[1])));
  for (i in seq(l)) {tary[i,]:=rec.ref.get(field,idx[i])}
  for (i in seq(l-1)) {
    for (j in seq(i+1,l)) {
      n+:=1;
      corr+:=tary[i,]*tary[j,]/abs(tary[i,]*tary[j,]);
    }
  }
return (corr/n)
}

TIM40ccbeamget := function(field,ref rec,index=1,inv=F, reffield='',start=-1,end=-1) {
  if (len(index)==1 && index<1) {idx:=seq(rec.len)} else {idx:=index}
  corr:=0;
  n:=0;
  idx:=seq(rec.ref.len)[rec.ref.select()]
  l:=len(idx)
  tary:=array(0,l,len(rec.ref.get(field,idx[1])));
  for (i in seq(l)) {tary[i,]:=rec.ref.get(field,idx[i])}
  for (i in seq(l-1)) {
    for (j in seq(i+1,l)) {
      n+:=1;
      corr+:=tary[i,]*tary[j,];
    }
  }
return sqrt_signed(corr/n)*data.getmeta('FieldStrengthFactor',index);
}

TIM40lccbeamget := function(field,ref rec,index=1,inv=F, reffield='',start=-1,end=-1) {
  if (len(index)==1 && index<1) {idx:=seq(rec.len)} else {idx:=index}
  corr:=0;
  n:=0;
  idx:=seq(rec.ref.len)[rec.ref.select()]
  l:=len(idx)
  tary:=array(0,l,len(rec.ref.get(field,idx[1])));
  for (i in seq(l)) {tary[i,]:=rec.ref.get(field,idx[i])}
  for (i in seq(l-1)) {
    for (j in seq(i+1,l)) {
      n+:=1;
      corr+:=log(tary[i,]*tary[j,]);
    }
  }
return sqrt(10^(corr/n))*data.getmeta('FieldStrengthFactor',index)
}

TIM40pbeamget := function(field,ref rec,index=1,inv=F, reffield='',start=-1,end=-1) {
  if (len(index)==1 && index<1) {idx:=seq(rec.len)} else {idx:=index}
  pow:=0;
  n:=0;
  idx:=seq(rec.ref.len)[rec.ref.select()]
  l:=len(idx)
  for (i in seq(l)) {
     n+:=1;
     pow+:=rec.ref.get(field,idx[i])^2;
   }
return sqrt(pow/n)*data.getmeta('FieldStrengthFactor',index)
}

TIM40Xbeamget := function(field,ref rec,index=1,inv=F, reffield='',start=-1,end=-1) {
  if (len(index)==1 && index<1) {idx:=seq(rec.len)} else {idx:=index}

  idx:=seq(rec.ref.len)[rec.ref.select()]
  l:=len(idx)
  ll:=len(rec.ref.get(field,idx[1]));
  tary:=array(0,l,ll);
  for (i in seq(l)) {
    tary[i,]:=rec.ref.get(field,idx[i])
  }
  return sqrt_signed(TIM40tary2xbeam(tary))*data.getmeta('FieldStrengthFactor',index)
}


TIM40lpbeamget := function(field,ref rec,index=1,inv=F, reffield='', start=-1,end=-1) {
  if (len(index)==1 && index<1) {idx:=seq(rec.len)} else {idx:=index}
  pow:=0;
  n:=0;
  idx:=seq(rec.ref.len)[rec.ref.select()]
  l:=len(idx)
  for (i in seq(l)) {
     n+:=1;
     pow+:=log(rec.ref.get(field,idx[i])^2);
   }
return sqrt(10^(pow/n))*data.getmeta('FieldStrengthFactor',index)
}


#-----------------------------------------------------------------------
#Name: TIM40beammodifyRec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Modify the data field records and definiton for the different
#     fields for the TIM40beam system, so that they can use 
#     the FFT data of a time-domain based data as their data
#     source. Will be used to plot data filtered in the FFT domain.
#
#Ret: Record with fields for vital header information
#Ref: GetDataDispatch
#-----------------------------------------------------------------------

TIM40beammodifyrec := function(ref rec) {
	ci := rec.Coordinfo;
	for (f in ['f(x)','FFT','CC-Beam','X-Beam','LgCC-Beam','P-Beam','LP-Beam','P-BeamF','Coherence']){
 	  ci[f].Stored:=F; 
      ci[f].Ref:="-";
      ci[f].DefAxis:="y";
      ci[f].ConvFunc:=F;
      ci[f].GenFunc:=symbol_value('TIM40beamget');
	  ci[f].IsInput:=T
	  ci[f].CrossRef:=f
    }

   ci.CalFFT.ConvFunc:=symbol_value('TIM40revIdentity');

   ci.FieldStrength:=[Units='Volt/m/MHz', DefAxis='y', Domain='T', Stored=F, 
             Ref='Voltage', Type='real', DomDefault='Ty',
             ConvFunc=symbol_value('TIM40revconvVoltage2FieldStrength')];

    ci['CC-Beam'].Units:='Volt/m/MHz';
    ci['CC-Beam'].GenFunc:=symbol_value('TIM40ccbeamget');
    ci['CC-Beam'].DomDefault:='Ty';
    ci['CC-Beam'].Domain:="T";
    ci['CC-Beam'].CrossRef:='Voltage';
    ci['CC-Beam'].Type:='real';

    ci['Coherence'].Units:='';
    ci['Coherence'].GenFunc:=symbol_value('TIM40coherencebeamget');
    ci['Coherence'].DomDefault:='Ty';
    ci['Coherence'].Domain:="T";
    ci['Coherence'].CrossRef:='Voltage';
    ci['Coherence'].Type:='real';

    ci['X-Beam'].Units:='Volt/m/MHz';
    ci['X-Beam'].GenFunc:=symbol_value('TIM40Xbeamget');
    ci['X-Beam'].DomDefault:='Ty';
    ci['X-Beam'].Domain:="T";
    ci['X-Beam'].CrossRef:='Voltage';
    ci['X-Beam'].Type:='real';

    ci['LgCC-Beam'].Units:='Volt/m/MHz';
    ci['LgCC-Beam'].GenFunc:=symbol_value('TIM40ccbeamget');
    ci['LgCC-Beam'].DomDefault:='Ty';
    ci['LgCC-Beam'].Domain:="T";
    ci['LgCC-Beam'].CrossRef:='Voltage';
    ci['LgCC-Beam'].Type:='real';

    ci['P-Beam'].Units:='Volt/m/MHz';
    ci['P-Beam'].GenFunc:=symbol_value('TIM40pbeamget');
    ci['P-Beam'].DomDefault:='Ty';
    ci['P-Beam'].Domain:="T";
    ci['P-Beam'].CrossRef:='Voltage';
    ci['P-Beam'].Type:='real';

    ci['P-BeamF'].Units:='Volt/m/MHz';
    ci['P-BeamF'].GenFunc:=symbol_value('TIM40pbeamget');
    ci['P-BeamF'].DomDefault:='Fy';
    ci['P-BeamF'].Domain:="F";
    ci['P-BeamF'].CrossRef:="FFT";
    ci['P-BeamF'].Type:='real';

    ci['LP-Beam'].Units:='Volt/m/MHz';
    ci['LP-Beam'].GenFunc:=symbol_value('TIM40lpbeamget');
    ci['LP-Beam'].DomDefault:='Ty';
    ci['LP-Beam'].Domain:="T";
    ci['LP-Beam'].CrossRef:='Voltage';
    ci['LP-Beam'].Type:='real';


    TIM40finishcoordinforec(rec,ci);
    return rec
}


