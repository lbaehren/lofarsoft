#######################################################################
#
# Functions and other stuff for the 40MHz bandwidth TIM-Board
#
########################################################################


########################################################################
# timread-setup
########################################################################

global_timread_client := [ = ];
  
for (i in [1:globalpar.TIMno]) {
  global_timread_client[i].a := F;
};


#-----------------------------------------------------------------------
#Name: TIM40Header
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates a default header entry for undocumented TIM40 datafiles.
#Ret: Data header record
#
#-----------------------------------------------------------------------

TIM40Header:=function(
	Project='Test-Measurements',
	DataType='TIM40',
	Filename='test',
	ID=globalpar.DataID,
	Measurement='1',
	FrequencyUnit='1e6',
	SamplerateUnit='1e6',
	Samplerate='80',
    NyquistZone='2',
	Location='unknown!',
	AntPos='0,0,0',
	Weather='Undefined',
	Timezone='00',
	Time='Undefined',
	Date='Undefined',
	ADCMaxChann='2048',
	ADCMinChann='-2048',
	MaxVolt='1',
	LocalCalFile='empty.tab',
	AntennaCalFile='empty.tab',
	CalFileFreqUnit='1e6',
	Files=''
){
header:=[Project=Project,
	DataType=DataType,
	Filename=Filename,
	ID=ID,
	Measurement=Measurement,
	FrequencyUnit=FrequencyUnit,
	SamplerateUnit=SamplerateUnit,
	Samplerate=Samplerate,
    NyquistZone=NyquistZone,
	Location=Location,
	AntPos=AntPos,
	Weather=Weather,
	Timezone=Timezone,
	Time=Time,
	Date=Date,
	ADCMaxChann=ADCMaxChann,
	ADCMinChann=ADCMinChann,
	MaxVolt=MaxVolt,
	LocalCalFile=LocalCalFile,
	AntennaCalFile=AntennaCalFile,
	CalFileFreqUnit=CalFileFreqUnit,
	Files=Files];

	header.Bandwidth:=(as_float(Samplerate)/2 *as_float(SamplerateUnit)/as_float(FrequencyUnit));

return header
}


########################################################################
#-----------------------------------------------------------------------
#Name: TIM40getFX
#-----------------------------------------------------------------------
#Type: Function
#Doc: get the requested part of the data from a file
#Par: ary		   - array with values to be converted
#Par: rec		   - array with data sets (header is needed)
#Par: index		   - index number of data set
#Par: inv		   - reverse direction of conversion
#Ret: array with converted values
#-----------------------------------------------------------------------

TIM40getDummy := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
   global globalpar
#  print 'TIM40getDummy: Reading from Disk',rec.head('Files',index);
   globalpar.lastaction:=spaste('Reading from Disk',rec.head('Files',index));
  return readdat(rec.head('Files',index),offset=rec.getmeta('Offset',index),blocksize=rec.getmeta('Blocksize',index))
};

TIM40getTIMfx := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
 return get_from_tim(ref rec,offset=(start-1),blocksize=(end-start+1));
};

########################################################################
#-----------------------------------------------------------------------
#Name: TIM40(gen|conv)ZZZ
#-----------------------------------------------------------------------
#Type: Function
#Doc: Converts coordinates from one field to another
#Par: ary		   - array with values to be converted
#Par: rec		   - array with data sets (header is needed)
#Par: index		   - index number of data set
#Par: inv		   - reverse direction of conversion
#Ret: array with converted values
#-----------------------------------------------------------------------

TIM40convDummy2fx := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
return  rec.flagdat(ary,'f(x)',index)
}

TIM40convFX2PowerT := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
return  ary^2
}

TIM40convVolt2RawFFT := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
    invdir:=rec.headf('NyquistZone')%2==0
    if ((len(ary)>2^20)&is_agent(rec.newsagent)) {rec.newsagent->message(paste('computing FFT for index',index));};
    return fftvec(ary,invdir=invdir);
};

TIM40convRawFFT2FFT := function(ref ary, ref rec,index=1,inv=F, reffield='',...)
{
  if (rec.get('Doshift',index)) {
    PhaseGrad := rec.get('PhaseGrad',index); #phase gradient from constant delays
    if (len(ary) != len(PhaseGrad)) {
      print "[data-TIM40::TIM40convRawFFT2FFT] Mismatch in array sizes!";
      print "- len(ary) ......... :",len(ary);
      print "- len(PhaseGrad) ... :",len(PhaseGrad);
      print "- Antenna index .... :",index;
    }
    retary := ary*exp(1i*PhaseGrad*degree)
  } else {retary:=ary}
  retary := retary*TIM40GetPhaseCal(rec, index);
  return rec.flagdat(retary,'FFT',index)
};

TIM40convFFT2PhaseGrad := function(ref ary, ref rec,index=1,inv=F, reffield='', ...) {
  fftlen:=rec.getmeta('FFTlen',index)
  fbandw:=rec.headf('Bandwidth',index)*rec.headf('FrequencyUnit',index);
  flowlim:=rec.getmeta('FrequencyLow',index);
  delay:= as_double(rec.get('Delay',index)) #convert "F" to "0." if 'Delay' is not set
  return calcphasegrad(delay,fftlen,flowlim=flowlim,fbandw=fbandw);
};

TIM40convFFT2AntennaGain := function(ref ary, ref rec,index=1,inv=F, reffield='', ...) {
  freq := rec.get('Frequency',index);
  azel := rec.get('AZEL',index);
  if (is_string(rec.head('Polarization',index)) && rec.head('Polarization',index)!=globalpar.DefPolarization) {
    if (azel[1]<0) {azel[1]+:=360}
    azel[1] := (azel[1]+90.)%360.;
  };
#print azel[1],azel[2],len(freq),field_names(globalpar.AntennaGainRec)
  return getgains(azel[1],azel[2],freq,globalpar.AntennaGainRec);
};

TIM40genPos := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
if (inv) return ary
else return [start:end]
};

TIM40genFlagT := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
 return rec.getflag('f(x)',index)
};

TIM40genFlagF := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
 return rec.getflag('FFT',index)
};

TIM40convPos2Time := function(ary,rec,index=1,inv=F, reffield='',...) {
if (inv) {
  return (ary * rec.headf("Samplerate",index) / rec.headf("SamplerateUnit",index))+rec.headf('presync',index)}
else {
  return (ary-rec.headf('presync',index)) / (rec.headf("Samplerate",index) * rec.headf("SamplerateUnit",index))}
};

TIM40convFX2Voltage := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
if (inv) {
  return  ary * rec.headf('ADCMaxChann')/rec.headf('MaxVolt')
} else {
  return   ary / rec.headf('ADCMaxChann')*rec.headf('MaxVolt')
}
};

#TIM40convTime2IF  := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
#	size:=len(ary);
#        return (-([1:(as_integer(size/2)+1)]-((size-1)/2)-1) /
#		(size-1)/(ary[2]-ary[1]))
#};
#TIM40convIF2Frequency := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
#  #both directions (IF -> Freq & Freq -> IF) are the same
#	return  rec.headf("Samplerate",index) * rec.headf("SamplerateUnit",index) - ary
#};

TIM40genIF  := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
        nosteps := rec.getmeta('FFTlen',index)-1;
        invdir:=rec.headf('NyquistZone')%2==0
        if (invdir) return rec.getmeta('FreqBin',index)*[nosteps:0]
        else {return rec.getmeta('FreqBin',index)*[0:nosteps]}
};

TIM40genFrequency  := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
       nosteps := rec.getmeta('FFTlen',index)-1;
       return rec.getmeta('FrequencyLow',index)+rec.getmeta('FreqBin',index)*[0:nosteps]
};

TIM40convFFT2AbsFFT := function(ref ary, ref rec,index=1,inv=F, reffield='', ...) {
	return abs(ary)
};

TIM40convFFT2Phase := function(ref ary, ref rec,index=1,inv=F, reffield='', ...) {
	return phase(ary)
};

TIM40convFFT2CalFFT := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
#This makes implicit assumption about the frequency array (same size)
#  cal := TIM40GetCal2(rec, index);
#  return 10^(cal/20)*ary*globalpar.GainFudgeFactor;
  return ary * TIM40GetGainCal(rec,index) *globalpar.GainFudgeFactor;
};

TIM40convFFT2Power := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
	size := rec.getmeta('FFTSize',index);
	return abs(ary)^2/size/rec.getmeta('PowerNormFaktor',index)/25; #ANIGL: data.getmeta(... -> rec...
#	return ((abs(ary)/size)^2)*2/50;
};

#TIM40funcMeanPower := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
#	size := rec.getmeta('FFTSize',index);
#	return ((ary/size*2)^2)/50;
#};

TIM40convPower2CalPower := function(ref pow, ref rec,index=1,inv=F, reffield='',...) {
#This makes implicit assumption about the frequency array (same size)
	cal := TIM40GetCal2(rec, index);
	return 10^(cal/10)*pow;
};

TIM40convPower2NoiseT := function(ref calpow, ref rec,index=1,inv=F, reffield='',...) {
  # Use: Temp = Power/(Bandwidth * kB) 
	return calpow/rec.getmeta('FreqBin',index)/1.3806e-23;
};

TIM40convMeanPower2MNT := function(ref mpow,ref rec,index=1,inv=F, reffield='',...) {
	calpow := TIM40convPower2CalPower(mpow, rec, index);
	return  TIM40convPower2NoiseT(calpow, rec, index);
};

TIM40convFreq2SkyT := function(ref freq,ref rec,index=1,inv=F, reffield='',...) {
#Average Northern Sky temperature (in K) from Falcke & Gorham (2002)
	return 32*( (freq/408e6)^-2.5 );
};

TIM40convTime2Lag := function(ref ary,ref rec,index=1,inv=F, reffield='',...) {
    return ary-ary[len(ary)/2+1];
};

TIM40convfx2CrossCorr := function(ref ary, ref rec,index=1,inv=F, reffield='', ...) {
    ary1:=rec.get('f(x)',rec.get('RefAnt',index))
    ary1:=ary1-mean(ary1)
    ary2:=ary-mean(ary)
	return global_fftserver.crosscorr(ary1,ary2)
};

TIM40convPhase2PhaseDiff := function(ref ary2, ref rec,index=1,inv=F, reffield='', ...) {
    ary1:=rec.get('Phase',rec.get('RefAnt',index))
    tmp := ary2-ary1;
    tmp := tmp-(360*(tmp>180));
    tmp := tmp+(360*(tmp<-180));
    return tmp;
};

TIM40convVoltage2BeamT := function(ref ary2, ref rec,index=1,inv=F, reffield='', ...) {
    ary1:=rec.get('Voltage',rec.get('RefAnt',index))
	return (ary2+ary1)/2
};

TIM40convFFT2BeamF := function(ref ary2, ref rec,index=1,inv=F, reffield='', ...) {
    ary1:=rec.get('FFT',rec.get('RefAnt',index))
	return abs((ary2+ary1)/2)
};

TIM40convPower2Flux := function(ref pow, ref rec,index=1,inv=F, reffield='',...) {
#This makes implicit assumption about the frequency array (same size)
  freq := rec.get('Frequency',index);

#  Aeff :=  globalpar.antennagain/4*pi*(c/freq)^2
#  calary := 1e26/rec.getmeta('FreqBin',index)/Aeff
  calary := 4e26*pi*freq^2/globalpar.antennagain/lightspeed^2/rec.getmeta('FreqBin',index)
  if (inv) {
    return  pow /calary;
  } else {
    return  pow *calary;
  };

};


#-----------------------------------------------------------------------
#Name: TIM40coordinforec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates the record with coordinate information for the different
#     fields for the TIM40 system.
#
#Ret: Record with fields for vital header information
#Ref: TIM40genEmptyRec,TIM40fgenEmptyRec
#-----------------------------------------------------------------------
TIM40coordinforec:=function(){
	ci := [=];
	ci.Pos:=[Units='Sample', DefAxis='x', Domain='T', Stored=F, 
		Ref='-', Type='real',
		GenFunc=symbol_value('TIM40genPos')];
	ci.Time:=[Units='Seconds', DefAxis='x', Domain='T', Stored=F, 
		Ref='Pos', Type='real', DomDefault='Tx',
		ConvFunc=symbol_value('TIM40convPos2Time')];
	ci['Dummy']:=[Units='Counts', DefAxis='y', Domain='T', Stored=T, 
		Ref='-', Type='unreal',GenFunc=symbol_value('TIM40getDummy'),
		IsInput=T];
	ci['f(x)']:=[Units='Counts', DefAxis='y', Domain='T', Stored=F, 
		Ref='Dummy', Type='real',ConvFunc=symbol_value('TIM40convDummy2fx'),
		IsInput=F];
	ci.Voltage:=[Units='Volt', DefAxis='y', Domain='T', Stored=F, 
		Ref='f(x)', Type='real', DomDefault='Ty',
		ConvFunc=symbol_value('TIM40convFX2Voltage')];
	ci.IntermedFreq:=[Units='Hz', DefAxis='x', Domain='F', Stored=F, 
		Ref='FFT', Type='real',
		ConvFunc=symbol_value('TIM40genIF')];
	ci.Frequency:=[Units='Hz', DefAxis='x', Domain='F', Stored=F, 
		Ref='FFT', Type='real', DomDefault='Fx',
		ConvFunc=symbol_value('TIM40genFrequency')];
	ci.RawFFT:=[Units='units', DefAxis='y', Domain='F', Stored=T, 
		Ref='Voltage', Type='complex',
		ConvFunc=symbol_value('TIM40convVolt2RawFFT')];
	ci.FFT:=[Units='units', DefAxis='y', Domain='F', Stored=F, 
		Ref='RawFFT', Type='complex',
		ConvFunc=symbol_value('TIM40convRawFFT2FFT')];
	ci.AbsFFT:=[Units='units', DefAxis='y', Domain='F', Stored=F, 
		Ref='FFT', Type='real',
		ConvFunc=symbol_value('TIM40convFFT2AbsFFT')];
	ci.CalFFT:=[Units='units', DefAxis='y', Domain='F', Stored=F, 
		Ref='FFT', Type='complex',
		ConvFunc=symbol_value('TIM40convFFT2CalFFT')];
	ci.Power:=[Units='Watt/Bin', DefAxis='y', Domain='F', Stored=F, 
		Ref='AbsFFT', Type='real',
		ConvFunc=symbol_value('TIM40convFFT2Power')];
	ci.MeanPower:=[Units='Watt/Bin', DefAxis='y', Domain='F', Stored=T, 
		Ref='-', Type='real',
		GenFunc=symbol_value('TIM40genMeanFFT')];
	ci.CalPower:=[Units='Watt/Bin', DefAxis='y', Domain='F', Stored=F, 
		Ref='CalFFT', Type='real', DomDefault='Fy',
		ConvFunc=symbol_value('TIM40convFFT2Power')];
	ci.NoiseT:=[Units='K', DefAxis='y', Domain='F', Stored=F, 
		Ref='CalPower', Type='real',
		ConvFunc=symbol_value('TIM40convPower2NoiseT')];
	ci.SkyT:=[Units='K', DefAxis='y', Domain='F', Stored=F, 
		Ref='Frequency', Type='real',
		ConvFunc=symbol_value('TIM40convFreq2SkyT')];
	ci.MNT:=[Units='K', DefAxis='y', Domain='F', Stored=F, 
		Ref='MeanPower', Type='real',
		ConvFunc=symbol_value('TIM40convMeanPower2MNT')];
	ci.Phase:=[Units='Degree', DefAxis='y', Domain='F', Stored=F, 
		Ref='FFT', Type='real',
		ConvFunc=symbol_value('TIM40convFFT2Phase')];
	ci.PhaseDiff:=[Units='Degree', DefAxis='y', Domain='F', Stored=F, 
		Ref='Phase', Type='real',
		ConvFunc=symbol_value('TIM40convPhase2PhaseDiff')];
	ci.PhaseGrad:=[Units='Degree', DefAxis='y', Domain='F', Stored=T, 
		ConvFunc=symbol_value('TIM40convFFT2PhaseGrad'),
		Ref='f(x)', Type='real'];
	ci.AntennaGain:=[Units='', DefAxis='y', Domain='F', Stored=T, 
		ConvFunc=symbol_value('TIM40convFFT2AntennaGain'),
		Ref='RawFFT', Type='real'];
	ci.BeamT:=[Units='Volt', DefAxis='y', Domain='T', Stored=F, 
		Ref='Voltage', Type='real', DomDefault='Ty',
		ConvFunc=symbol_value('TIM40convVoltage2BeamT')];
	ci.BeamF:=[Units='units', DefAxis='y', Domain='F', Stored=F, 
		Ref='FFT', Type='real',
		ConvFunc=symbol_value('TIM40convFFT2BeamF')];
	ci.TimeLag:=[Units='Seconds', DefAxis='x', Domain='T', Stored=F, 
		Ref='Time', Type='real', DomDefault='Tx',
		ConvFunc=symbol_value('TIM40convTime2Lag')];
	ci.PosLag:=[Units='Samples', DefAxis='x', Domain='T', Stored=F, 
		Ref='Pos', Type='real', DomDefault='Tx',
		ConvFunc=symbol_value('TIM40convTime2Lag')];
	ci.CrossCorr:=[Units='Units', DefAxis='y', Domain='T', Stored=T, 
		Ref='f(x)', Type='real',
		ConvFunc=symbol_value('TIM40convfx2CrossCorr')];
	ci.PowerT:=[Units='counts^2', DefAxis='y', Domain='T', Stored=F, 
		Ref='f(x)', Type='real', DomDefault='Ty',
		ConvFunc=symbol_value('TIM40convFX2PowerT')];
	ci.FlagF:=[Units='Weight', DefAxis='y', Domain='F', Stored=F, 
		Ref='-', Type='real',
		GenFunc=symbol_value('TIM40genFlagF')];
	ci.FlagT:=[Units='Weight', DefAxis='y', Domain='T', Stored=F, 
		Ref='-', Type='real',
		GenFunc=symbol_value('TIM40genFlagT')];
    ci.Flux :=[Units='Jy', DefAxis='y', Domain='F', Stored=F, 
             Ref='CalPower', Type='real', DomDefault='Fy',
             ConvFunc=symbol_value('TIM40convPower2Flux')];
return ci
}

#-----------------------------------------------------------------------
#Name: TIM40finishcoordinforec
#-----------------------------------------------------------------------
#Type: Function
#Doc: finishes up the empty record with the different
#     fields for the TIM40 system.
#
#Ret: Record with fields for vital header information
#Ref: TIM40genEmptyRec,TIM40fgenEmptyRec
#-----------------------------------------------------------------------
TIM40finishcoordinforec:=function(ref rec, ci){
	rec.Coordinfo := ci;
#    rec.DomDefault:=[=];
#    rec.Coord:=[=];
#    rec.Units:=[=];
	i:=0;j:=0;
	for (ff in field_names(ci) ) {
	  if (has_field(ci[ff],'IsInput') && ci[ff].IsInput) {
	    j+:=1;
	    rec.Input := ff;
	  };
	  if (ci[ff].Type == 'real' && ff != 'DummyXYZ') {
	    i+:=1; 
	    rec.Coord[i] := ff;
	    rec.Units[i] := ci[ff].Units;
	  };
	  if (has_field(ci[ff],'DomDefault')) { 
		rec.DomDefault[ci[ff].DomDefault] := ff; 
       }
	};
#	if (j != 1) {
#	  print 'Data-Record with more than one input!',j;
#	};
return rec
}


#-----------------------------------------------------------------------
#Name: TIM40genEmptyRec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates the data field records and definiton for the different
#     fields for the TIM40 system.
#
#Ret: Record with fields for vital header information
#-----------------------------------------------------------------------
TIM40genEmptyRec := function() {
    rec:=[=]
    ci := TIM40coordinforec();
    return TIM40finishcoordinforec(rec,ci);
}


#-----------------------------------------------------------------------
# Name: FixDelays
#-----------------------------------------------------------------------
# Type: Function
# Doc : This ugly function sets some fixed delays for the antennas
#
# Ret :
#-----------------------------------------------------------------------
FixDelays:=function(ref rec,index=-1,delays=-1){
  if (delays == -1) {dtau:=globalpar.delays} else {dtau:=delays}
  if (index<0) {idx:=seq(rec.len)} else {idx:=index}
  for (i in idx) {
    if (i > 0 && i <=len(dtau)) {
      tunit:=1/(rec.headf('Samplerate',i)*rec.headf('SamplerateUnit',i))
      rec.put(dtau[i]*tunit,'Corrdelay',i)
    }
  }
}


########################################################################
# Calibration
########################################################################

TIM40GetCal := function(ref rec, index=1) {
	LOCalFile := rec.head('LocalCalFile',index);
	AntCalFile := rec.head('AntennaCalFile',index);
	CalUnit := rec.headf('CalFileFreqUnit',index);
	LOCalAr := rec.readcalfile(LOCalFile);
	AntCalAr := rec.readcalfile(AntCalFile);
	freqar := TIM40getFrequency(rec,'Frequency',index)/CalUnit;
	cal := getcalvals(freqar ,LOCalAr);
	freqar := TIM40getFrequency(rec,'Frequency',index,lofreq=F)/CalUnit;
	cal := cal+getcalvals(freqar ,AntCalAr);
	return cal;
};

TIM40GetCal2 := function(ref rec, index=1) {
  LOCalFile := rec.head('LocalCalFile',index);
  AntCalFile := rec.head('AntennaCalFile',index);
  CalUnit := rec.headf('CalFileFreqUnit',index);
  LOCalAr := rec.readcalfile(LOCalFile);
  AntCalAr := rec.readcalfile(AntCalFile);
  freqar := rec.get('IntermedFreq',index)/CalUnit;
  cal := getcalvals(freqar ,LOCalAr);
  freqar := rec.get('Frequency',index)/CalUnit;
  cal := cal+getcalvals(freqar,AntCalAr);
  if (len(cal) != rec.getmeta('FFTlen')) {
    print "[data-TIM40::TIM40GetCal2]";
    print "- FFT length .... :",rec.getmeta('FFTlen');
    print "- Frequency array :",len(freqar);
    print "- Calibrated data :",len(cal);
  }
  return cal;
};

TIM40GetPhaseCal := function(ref rec, index=1) {
  #cannot just get the frequency as this would cause an ifinite loop
  freqar := TIM40genFrequency([], rec,index);
  date := jdr2string(rec.head('Date',index),F)
  if (date<946695168) { # 2000/01/01.12:00:00.00
    print "Invalid date! (If if is past 2038 you need a new version of this software)";
    return 1;
  }
  AntID := as_integer(rec.head('AntID',index));
  # if no AnID is given then just return 1, i.e. no phase change
  if (AntID==0) { return 1 }; 
  if (!has_field(rec.CalTable.phasecal.dates,as_string(AntID)) || 
       (rec.CalTable.phasecal.dates[as_string(AntID)] != date) || 
       (len(freqar) != len(rec.CalTable.phasecal[as_string(AntID)])) ) {
    tmp := getCalField(rec.head('Observatory',index), 'PhaseCal', 'PhaseCalFreq', freqar, date, AntID);
    rec.CalTable.phasecal[as_string(AntID)] := tmp/abs(tmp);
    rec.CalTable.phasecal.dates[as_string(AntID)] := date
  }
  return rec.CalTable.phasecal[as_string(AntID)];
}

TIM40GetGainCal := function(ref rec, index=1) {
  #cannot just get the frequency as this would cause an ifinite loop
  freqar := TIM40genFrequency([], rec,index);
  date := jdr2string(rec.head('Date',index),F)
  if (date<946695168) { # 2000/01/01.12:00:00.00
    print "Invalid date! (If if is past 2038 you need a new version of this software)";
    return 1;
  }
  AntID := as_integer(rec.head('AntID',index));
  if (AntID==0) { # if no AnID is given try to get one from the defaults or return 1
    if (index <= len(globalpar.AntIDdefs)) {
       AntID := as_integer(globalpar.AntIDdefs[index]);
    } else {
      return 1;
    };
  }; 
  if (!has_field(rec.CalTable.gaincal.dates,as_string(AntID)) || 
      (rec.CalTable.gaincal.dates[as_string(AntID)] != date) ||
      (len(freqar)!=len(rec.CalTable.gaincal[as_string(AntID)])) ) {
    rec.CalTable.gaincal[as_string(AntID)] := getCalField(rec.head('Observatory',index), 'ElGainCal', 'ElGainCalFreq', freqar, date, AntID);
    rec.CalTable.gaincal.dates[as_string(AntID)] := date
  }
  return rec.CalTable.gaincal[as_string(AntID)] ;
}

#-----------------------------------------------------------------------
#Name: get_from_tim
#-----------------------------------------------------------------------
#Type: Function
#Doc: Reads data from the memory of the TIM-BOARD
#
#Par: rec = 				- Data record to identify dataset
#Par: offset    = 0			- Starting position 
#Par: blocksize	= -1			- Length of block to read
#                                         blocksize<=0 -> default-size
#-----------------------------------------------------------------------
get_from_tim := function(ref rec,offset,blocksize) {
  getrec.presync := offset;
  getrec.blocklen := blocksize;
};

#-----------------------------------------------------------------------
#Name: readTIMboard 
#-----------------------------------------------------------------------
#Type: Function
#Doc: Starts a new measurement and reads data from the TIM-BOARD
#
#Par: rec = 				- Data record to identify dataset
#Par: blocksize	= -1			- Length of block to read
#                                         blocksize<=0 -> default-size
#Par: offset    = 0			- Starting position 
#Par: pad	= F			- Pad the remaining fields
#					  with padval if file shorter
#					  than blocksize?
#Par: padval	= 0			- Value used for padding file  
#-----------------------------------------------------------------------
readTIMboard := function (ref rec,blocksize=-1,offset=0,pad=F,padval=0,init=T)  {
    global global_timread_client;

    if (init) newdata(rec); #hack for backward compatibility

    globalpar.DataID+:=1;
    MinBlocksize:=globalpar.BlocksizeForDisplay;
    nfiles:=0; 

#    if (is_agent(global_ppsync_client)) { a := global_ppsync_client->setall(); print 'set veto',a;}

    for (Bno in [1:globalpar.TIMno] ) {
      global_timread_client[Bno].data := F;
      a := global_timread_client[Bno]->getdata_sync(globalpar.DataSize);
print 'started board ', Bno,a;
    };
    shell('sleep 1');

#    if (is_agent(global_ppsync_client)) { a := global_ppsync_client->remall(); print 'cleared veto',a;}
    if (is_agent(global_ppsync_client)) { a := global_ppsync_client->gensync(); print 'generated sync',a;}
    if (global_timread_client[1].data == F) await global_timread_client[1]->newdata;

    for (Bno in [1:globalpar.TIMno] ) {
      sync(global_timread_client[Bno]);
    };


     head.Date:=program.datestring();
     head.Time:=program.timestring();
     head.ID:=globalpar.DataID;
        
    for (Bno in [1:globalpar.TIMno] ) {
	#mat := global_timread_client[Bno]->getdata(globalpar.DataSize);
	mat := global_timread_client[Bno].data;
	print 'read data',Bno,shape(mat),max(mat), min(mat);
#	mat  := (mat-4096)*(mat>=2048)+mat*(mat<2048)

	shap := shape(mat)
	for (Cno in [1:shap[2]]) {
#  Initializing and storing data
          head.Filename := sprintf('%s-%s-B%-3.3i-C%-2.2i',head.Date,head.Time,Bno,Cno);
	  nfiles+:=1;
	  newhead:=TIM40Header(
	      Filename=head.Filename,
	      ID=head.ID,
	      Time=head.Time,
	      Date=head.Date,
	      Files=head.Filename,
	      AntPos=globalpar.TIMpos[nfiles]);
	  i:=rec.newentry(head=newhead);
          newdatasets[nfiles]:=i;
	  for (ff in rec.get('Input',i)) { rec.put(mat[,Cno],ff,i); print 'wrote data', Cno;}
	  for (ff in rec.coord(i)) {rec.touch(ff,i,version=0)}
	  rec.setregion([1,min(MinBlocksize,shap[1])],'T',i)
          rec.putmeta(offset,'FileOffset',i);
          rec.putmeta(shap[1],'FileBlocksize',i);
	  rec.putmeta(0,'Offset',i);
	  rec.putmeta(min(MinBlocksize,shap[1]),'Blocksize',i);
	}
    }
return newdatasets
}


configTIMboardGUI := function() {
  global global_timread_client;

  TCgui.frame := frame(title='TIM Board Configuration',width=1000,height=350,tpos='c');

  TCgui.fstat := frame(TCgui.frame,side="left");
  TCgui.cscale :=  scale(TCgui.fstat,1,globalpar.TIMno,text='Board No.');TCgui.cscale.value :=1;
  TCgui.statlabel := label(TCgui.fstat,'The TIM-board is: ');
  TCgui.status := label(TCgui.fstat,'',relief='groove');
 
  TCgui.regframe := frame(TCgui.frame,side="top",relief='groove');
  TCgui.f1 := frame(TCgui.regframe,side="left");
  TCgui.flab := frame(TCgui.f1);  TCgui.lablabel := label(TCgui.flab,' ');
  TCgui.fcur := frame(TCgui.f1); TCgui.currlabel := label(TCgui.fcur,'current');
  TCgui.fset := frame(TCgui.f1);  TCgui.setlabel := label(TCgui.fset,'set');

  #TCgui.fmode := frame(TCgui.frame,side="left");
  TCgui.modelabel := label(TCgui.flab,'Mode-Register:      ');
  TCgui.modedisp := label(TCgui.fcur,'0x00',width=5,relief='ridge');
  TCgui.modeent := entry(TCgui.fset,width=5);

  #TCgui.fcontr := frame(TCgui.frame,side="left");
  TCgui.contrlabel := label(TCgui.flab,'Control-Register:    ');
  TCgui.contrdisp := label(TCgui.fcur,'0x00',width=5,relief='ridge');
  TCgui.contrent := entry(TCgui.fset,width=5);

  #TCgui.fseg := frame(TCgui.frame,side="left");
  TCgui.seglabel := label(TCgui.flab,'Segment-Register: ');
  TCgui.segdisp := label(TCgui.fcur,'0x00',width=5,relief='ridge');
  TCgui.segent := entry(TCgui.fset,width=5);

  #TCgui.fclock := frame(TCgui.frame,side="left");
  TCgui.clocklabel := label(TCgui.flab,'Clock-Register:      ');
  TCgui.clockdisp := label(TCgui.fcur,'0x00',width=5,relief='ridge');
  TCgui.clockent := entry(TCgui.fset,width=5);

  TCgui.setbuttframe := frame(TCgui.regframe,side="left");
  TCgui.setbuttspace := label(TCgui.setbuttframe,'',width=23);
  TCgui.setbuttlabel := label(TCgui.setbuttframe,'Set Registers:');
  TCgui.setallbut := button(TCgui.setbuttframe,'Set All');
  TCgui.setbut := button(TCgui.setbuttframe,'Set Board');

  TCgui.blockstr := split('0123456789ab','')
  TCgui.fblock := frame(TCgui.frame,side="top",relief='groove');
  TCgui.fb1 := frame(TCgui.fblock,side="left");
  TCgui.fb2 := frame(TCgui.fblock,side="left");
  TCgui.fb3 := frame(TCgui.fblock,side="left");
  TCgui.blockbu0 := button(TCgui.fb1,'16 Samples',width=9,type='radio',group=TCgui.fblock);
  TCgui.blockbu1 := button(TCgui.fb1,'32 Samples',width=9,type='radio',group=TCgui.fblock);
  TCgui.blockbu2 := button(TCgui.fb1,'64 Samples',width=9,type='radio',group=TCgui.fblock);
  TCgui.blockbu3 := button(TCgui.fb1,'128 Sampl.',width=9,type='radio',group=TCgui.fblock);
  TCgui.blockbu4 := button(TCgui.fb2,'512K Sampl.',width=9,type='radio',group=TCgui.fblock);
  TCgui.blockbu5 := button(TCgui.fb2,'2M Samples',width=9,type='radio',group=TCgui.fblock);
  TCgui.blockbu6 := button(TCgui.fb2,'8M Samples',width=9,type='radio',group=TCgui.fblock);
  TCgui.blockbu7 := button(TCgui.fb2,'32M Sampl.',width=9,type='radio',group=TCgui.fblock);
  TCgui.blockbu8 := button(TCgui.fb3,'128M Sampl.',width=9,type='radio',group=TCgui.fblock);
  TCgui.blockbu9 := button(TCgui.fb3,'512M Sampl.',width=9,type='radio',group=TCgui.fblock);
  TCgui.blockbua := button(TCgui.fb3,'1G Samples',width=9,type='radio',group=TCgui.fblock);
  TCgui.blockbub := button(TCgui.fb3,'2G Samples',width=9,type='radio',group=TCgui.fblock,disabled=T);
  TCgui.setblockframe := frame(TCgui.fblock,side="left");
  TCgui.setblockspace := label(TCgui.setblockframe,' ',width=21); 
  TCgui.setblocklabel := label(TCgui.setblockframe,'Set Blocklength:');
  TCgui.setallblockbut := button(TCgui.setblockframe,'Set All');
  TCgui.setblockbut := button(TCgui.setblockframe,'Set Board');

  TCgui.fssh := frame(TCgui.frame,side="left");
  TCgui.sshlab := label(TCgui.fssh,'ssh-string: ');
  TCgui.sshent := entry(TCgui.fssh,width=26);
  TCgui.sshbut := button(TCgui.fssh,'activate',type='check',width=6);


  TCgui.fstart := frame(TCgui.frame,side="left");
  TCgui.startallbut := button(TCgui.fstart,'Start All',background='green');
  TCgui.startbut := button(TCgui.fstart,'Start Board');
  TCgui.startspace := frame(TCgui.fstart);
  TCgui.endbut := button(TCgui.fstart,'Dismiss',background='orange');

  TCgui.blockbu9->state(T);
  TCgui.sshbut->state(T);
  TCgui.sshent->delete('start','end');  
  TCgui.sshent->insert(globalpar.TIMstart[1]);

  TCgui.setestabilshed := function() {
    wider TCgui;

    TCgui.status->text('active');
    TCgui.modeent->disabled(F);TCgui.modeent->relief('sunken');
    TCgui.contrent->disabled(F);TCgui.contrent->relief('sunken');
    TCgui.segent->disabled(F);TCgui.segent->relief('sunken');
    TCgui.clockent->disabled(F);TCgui.clockent->relief('sunken');
    TCgui.sshbut->disabled(T); 
    TCgui.timconf := global_timread_client[TCgui.cscale.value]->getconf();
    tmpstr := sprintf('0x%02x',TCgui.timconf.ModeRegister);
    TCgui.modeent->delete('start','end'); 
    TCgui.modeent->insert(tmpstr); TCgui.modedisp->text(tmpstr);
    TCgui[spaste('blockbu',(split(tmpstr,'')[3]))]->state(T);
    tmpstr := sprintf('0x%02x',TCgui.timconf.StatusRegister);
    TCgui.contrent->delete('start','end'); 
    TCgui.contrent->insert(tmpstr); TCgui.contrdisp->text(tmpstr);
    tmpstr := sprintf('0x%02x',TCgui.timconf.SegmentRegister);
    TCgui.segent->delete('start','end'); 
    TCgui.segent->insert(tmpstr); TCgui.segdisp->text(tmpstr);
    tmpstr := sprintf('0x%02x',TCgui.timconf.ClockRegister);
    TCgui.clockent->delete('start','end'); 
    TCgui.clockent->insert(tmpstr); TCgui.clockdisp->text(tmpstr);
  };

  TCgui.setdiscon := function() {
    wider TCgui;
    TCgui.status->text('inactive');
    TCgui.modeent->disabled(T);TCgui.modeent->relief('raised');
    TCgui.contrent->disabled(T);TCgui.contrent->relief('raised');
    TCgui.segent->disabled(T);TCgui.segent->relief('raised');
    TCgui.clockent->disabled(T);TCgui.clockent->relief('raised');
    TCgui.sshbut->disabled(F); 
    TCgui.sshent->delete('start','end');  
    TCgui.sshent->insert(globalpar.TIMstart[TCgui.cscale.value]);
  };

  setregisters:= function(cnum) {
    global global_timread_client;
    wider TCgui;
    if (!has_field(global_timread_client[cnum],'established')) {
      print 'Set Registers: Card ',cnum,' not connected!';
    } else {
      print 'Sorry! Not implemented yet!';   
      #global_timread_client[cnum]->setconf(t);
      #TCgui.setestabilshed();
    };
  };    

  setblocklen := function(cnum) {
    global global_timread_client;
    wider TCgui;
    if (!has_field(global_timread_client[cnum],'established')) {
      print 'Set Blocklenght: Card ',cnum,' not connected!';
    } else {
      i := len(TCgui.blockstr);
      while( !(TCgui[spaste('blockbu',(TCgui.blockstr[i]))]->state()) & (i>1)){
        i := i-1;
      };
      t := [=];
      t.blocklen := i-1;
print t;
      global_timread_client[cnum]->setconf(t);
      TCgui.setestabilshed();
    };    
  };

  startclient := function(cnum) {
    global global_timread_client;
    wider TCgui;
    if (has_field(global_timread_client[cnum],'established')) {
      print 'Card already connected!';
    } else {
      #global_timread_client[cnum] := client('/home/horneffer/tim2/src/timclient',async=T);
      global_timread_client[cnum] := client(globalpar.TIMexec[cnum],async=T);
      act_string := global_timread_client[cnum].activate;
      #get ip-address (and remove trailing space) (works on SUSE-Linux but noton e.g. solaris)
      ipaddr := shell('getip'); ipaddr =~ s/ //g; 
#     ipaddr := shell('hostname -i'); ipaddr =~ s/ //g; 
#     ipaddr:='134.104.17.219';
      j:=1; while (act_string[j] != '-host') {j:=j+1;};
      if (act_string[j] == '-host') { act_string[j+1] := ipaddr;};
      if (TCgui.sshbut->state()) {
        tim_shell_string := paste('\"' , act_string[1],
        spaste('\'',paste(act_string[2:len(act_string)],sep='\' \''),'\''),'\"');
        if (TCgui.cscale.value == cnum) { 
          ssh_string := TCgui.sshent->get();
        } else {
          ssh_string := globalpar.TIMstart[cnum]
        };
        tim_shell_string := paste(ssh_string,tim_shell_string);
        print 'Starting shell client with string: ',tim_shell_string;
        tim_sh_client := shell(tim_shell_string,async=T);
        whenever tim_sh_client->stdout do {print 'shellclient ',cnum,' says: ',$value; };
        whenever tim_sh_client->stderr do {print 'shellclient ',cnum,' reports: ',$value;};
        shell('sleep 2');
        sync(global_timread_client[cnum]);
      } else {
        tim_shell_string := paste('' , act_string[1],
        spaste('\'',paste(act_string[2:len(act_string)],sep='\' \''),'\''),'');
        TCgui.fm := frame(TCgui.frame,side="top");
        TCgui.messagelab := label(TCgui.fm,'Start the following programm on the DAQ-PC and press the button.');
        TCgui.fm2 := frame(TCgui.fm,side="left");
        TCgui.messageent := entry(TCgui.fm2,width=strlen(spaste(tim_shell_string)));
        TCgui.messagebut := button(TCgui.fm2,'O.K.');
        TCgui.messageent->delete('start','end');  
        TCgui.messageent->insert(tim_shell_string);
        await TCgui.messagebut->press;
        sync(global_timread_client[cnum]);
      };        
      if (has_field(global_timread_client[cnum],'established')) {
        sync(global_timread_client[cnum]);
        global_timread_client[cnum].data := F;
        ok := global_timread_client[cnum]->cardinit(globalpar.TIMfiles[cnum]);
        if (ok) { 
          whenever global_timread_client[cnum]->newdata do {
            global_timread_client[cnum].data := $value;
            print 'got new data from timclient ',cnum;
          };
          print 'Timreadclient ',cnum,' started and initialized.';
          if (TCgui.cscale.value == cnum) TCgui.setestabilshed();
        } else {
          print 'Error while initializing timreadclient ',cnum,'!';
          global_timread_client[cnum] := F;
        };
      } else {
        print 'Unknown error has occured. Apparently not connected...';
        if (TCgui.cscale.value == cnum) TCgui.setdiscon();
      };
      TCgui.fm :=F;
    };
  };

 
  if (has_field(global_timread_client[1],'established')) {
    TCgui.setestabilshed();	
  } else {
    TCgui.setdiscon();  
  };

  whenever TCgui.cscale->value  do {
    TCgui.cscale.value := $value;
    if (has_field(global_timread_client[$value],'established')) {
      TCgui.setestabilshed();
    } else {
      TCgui.setdiscon();
    };
  };

  whenever TCgui.setallbut->press do {
    for (i in [1:globalpar.TIMno]) setregisters(i);
  };
  whenever TCgui.setbut->press do {
    setregisters(TCgui.cscale.value);
  };

  whenever TCgui.setallblockbut->press do {
    for (i in [1:globalpar.TIMno]) setblocklen(i);
  };
  whenever TCgui.setblockbut->press do {
    setblocklen(TCgui.cscale.value);
  };

  whenever TCgui.startallbut->press do {
    for (i in [1:globalpar.TIMno]) startclient(i);
    if (is_string(globalpar.ppsyncclient)) {
      global global_ppsync_client;
      global_ppsync_client := client(globalpar.ppsyncclient[2],async=T);
      act_string := global_ppsync_client.activate;
      #get ip-address (and remove trailing space) (works on SUSE-Linux but noton e.g. solaris)
      ipaddr := shell('getip'); ipaddr =~ s/ //g; 
#     ipaddr := shell('hostname -i'); ipaddr =~ s/ //g; 
#     ipaddr:='134.104.17.219';
      j:=1; while (act_string[j] != '-host') {j:=j+1;};
      if (act_string[j] == '-host') { act_string[j+1] := ipaddr;};
      if (TCgui.sshbut->state()) {
        pps_shell_string := paste('\"' , act_string[1],
        spaste('\'',paste(act_string[2:len(act_string)],sep='\' \''),'\''),'\"');
        pps_shell_string := paste(globalpar.ppsyncclient[1],pps_shell_string);
        print 'Starting shell client with string: ',pps_shell_string;
        pps_sh_client := shell(pps_shell_string,async=T);
        shell('sleep 2');
        sync(global_ppsync_client);
      } else {
        pps_shell_string := paste('' , act_string[1],
        spaste('\'',paste(act_string[2:len(act_string)],sep='\' \''),'\''),'');
        TCgui.fm := frame(TCgui.frame,side="top");
        TCgui.messagelab := label(TCgui.fm,'Start the following programm on the DAQ-PC and press the button.');
        TCgui.fm2 := frame(TCgui.fm,side="left");
        TCgui.messageent := entry(TCgui.fm2,width=strlen(spaste(pps_shell_string)));
        TCgui.messagebut := button(TCgui.fm2,'O.K.');
        TCgui.messageent->delete('start','end');  
        TCgui.messageent->insert(pps_shell_string);
        await TCgui.messagebut->press;
        sync(global_ppsync_client);
        TCgui.fm :=F;
      };        
      if (!has_field(global_ppsync_client,'established')) {
        global_ppsync_client:=F;
      } else {
        print '*** All clients successfully started! ***';
      };
    };
  };

  whenever TCgui.startbut->press do {
    startclient(TCgui.cscale.value);
  };

  whenever TCgui.endbut->press do {
    TCgui.frame := F;
    TCgui := F;
  };

};



########################################################################
# *** Hack! ***
########################################################################

fixTIMdata := function(ref rec, index=-1){
  if ( index >0) {
     rec.data[index]['f(x)'] := 
         (rec.data[index]['f(x)']-4096)*(rec.data[index]['f(x)']>=2048) +
           rec.data[index]['f(x)']*(rec.data[index]['f(x)']<2048);
  } else {
     for (i in [1:rec.len]) {
       if (rec.head('DataType',i)=='TIM40') {
         rec.data[i]['f(x)'] := 
            (rec.data[i]['f(x)']-4096)*(rec.data[i]['f(x)']>=2048) +
              rec.data[i]['f(x)']*(rec.data[i]['f(x)']<2048);
       };
     };
  };
  return index;
};

########################################################################
#
# other Stuff
#
########################################################################

tims2file := function(ref rec) {
    head.Date:=program.datestring();
    head.Time:=program.timestring();

    Filename := array('',globalpar.TIMno,2);
    for (Bno in [1:globalpar.TIMno] ) {
      Filename[Bno,1]:= sprintf('%s-%s-B%-3.3i-C%-2.2i',head.Date,head.Time,Bno,1);
      Filename[Bno,2]:= sprintf('%s-%s-B%-3.3i-C%-2.2i',head.Date,head.Time,Bno,2);
      cfiles[1] := sprintf('/data/%s',Filename[Bno,1]);
      cfiles[2] := sprintf('/data/%s',Filename[Bno,2]);
      a := global_timread_client[Bno]->data2disk(cfiles);
      print 'started client ',Bno,a;
    };
    shell('sleep 1');
    if (is_agent(global_ppsync_client)) { a := global_ppsync_client->gensync(); print 'generated sync',a;}

    await global_timread_client[1]->todisk_done;
    for (Bno in [1:globalpar.TIMno] ) {
      sync(global_timread_client[Bno]);
    };

    nfiles:=1;
    for (Bno in [1:globalpar.TIMno] ) {
      sary := split(globalpar.TIMstart[Bno]);
      sary := paste(sary[2:len(sary)]);
      for (Cno in [1,2]) {
        shellstring := spaste('scp ',sary,':/data/',Filename[Bno,Cno],' /data/TIM40/',Filename[Bno,Cno],'.dat');
        print shellstring;
        shell(shellstring);

        nfiles+:=1;
        newhead:=TIM40Header(
           Filename=Filename[Bno,Cno],
           Time=head.Time,
           Date=head.Date,
           Files=spaste('/data/TIM40/',Filename[Bno,Cno],'.dat'),
           AntPos=globalpar.TIMpos[nfiles]);
        file:=open(spaste('> ',Filename[Bno,Cno],'.hdr'));
        for (f in field_names(newhead)) {write(file,sprintf('#%s: %s',f,as_string(newhead[f])))}
        file := F;
      };
    };
};
