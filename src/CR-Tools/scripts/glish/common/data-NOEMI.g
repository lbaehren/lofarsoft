#######################################################################
#
# Functions and other stuff for the NOEMI-AR5000 combo
#
########################################################################


#-----------------------------------------------------------------------
#Name: NOEMIHeader
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates a default header entry for undocumented NOEMI datafiles.
#Ret: Data header record
#
#Par: freq = 10.7	- Observing frequency. Use LO if unknown.
#-----------------------------------------------------------------------

NOEMIHeader:=function(
	Project='Undefined',
	DataType='TIM40',
	Filename='test',
	ID=0,
	Measurement=1,
	FrequencyUnit=1e6,
	SamplerateUnit=1e6,
	LO=10.7,
	Samplerate=40,
	Bandwidth=12.5,
	Location='Undefined',
	AntPos=array(0,3),
	Weather='Undefined',
	Time='Undefined',
	ADCMaxChann=2048,
	ADCMinChann=-2048,
	MaxVolt=0.25,
	Frequency=10.7,
	LocalCalFile='ar5000eich.tab',
	AntennaCalFile='disco-calib.tab',
	CalFileFreqUnit=1e6,
	Files=""
){
header:=[Project=Project,
	DataType=DataType,
	Filename=Filename,
	ID=ID,
	Measurement=Measurement,
	FrequencyUnit=FrequencyUnit,
	SamplerateUnit=SamplerateUnit,
	LO=LO,
	Samplerate=Samplerate,
	Bandwidth=Bandwidth,
	Location=Location,
	AntPos=AntPos,
	Weather=Weather,
	Time=Time,
	ADCMaxChann=ADCMaxChann,
	ADCMinChann=ADCMinChann,
	MaxVolt=MaxVolt,
	Frequency=Frequency,
	LocalCalFile=LocalCalFile,
	AntennaCalFile=AntennaCalFile,
	CalFileFreqUnit=CalFileFreqUnit,
	Files=Files];
return header
}

########################################################################
# Field-Functions 
########################################################################


#-----------------------------------------------------------------------
#Name: NOEMIFieldFuncInit
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates a record of functions for the different fields.
#Ret: record with the FieldFunctions
#-----------------------------------------------------------------------
NOEMIFieldFuncInit := function() {
	rec := [=];
	rec['f(x)'] := function(ref rec, field, index) NOEMIgetTimeSer(ref rec, field, index);
	rec['Pos'] := function(ref rec, field, index) NOEMIgetPos(ref rec, field, index);
	rec['Time'] := function(ref rec, field, index) NOEMIgetTime(ref rec, field, index);
	rec['Frequency'] := function(ref rec, field, index, lofreq=F) NOEMIgetFrequency(ref rec, field, index, lofreq);
	rec['Voltage'] := function(ref rec, field, index) NOEMIgetVoltage(ref rec, field, index); 
	rec['FFT'] := function(ref rec, field, index, raw=F) NOEMIgetFFT(ref rec, field, index, raw);
	rec['Power'] := function(ref rec, field, index) NOEMIgetPower(ref rec, field, index);
	rec['CalPower'] := function(ref rec, field, index) NOEMIgetCalPower(ref rec, field, index);
	rec['NoiseT'] := function(ref rec, field, index) NOEMIgetNoiseTemp(ref rec, field, index);
	rec['MNT'] := function(ref rec, field, index) NOEMIgetMeanNoiseTemp(ref rec, field, index);
	rec['SkyT'] := function(ref rec, field, index) NOEMIgetSkyTemp(ref rec, field, index);
	return rec;
};



NOEMIgetTimeSer := function(ref rec,field="f(x)",index=1) {
	start := rec.getmeta('Offset',index)+1;
	stop := start + rec.getmeta('Blocksize',index)-1;
	return GetDataFromMem(ref rec, 'f(x)', index)[start:stop];
};
NOEMIgetPos := function(ref rec,field="y",index=1) {
	# return GetDataFromMem(ref rec, 'Pos', index);
	start := rec.getmeta('Offset',index)+1;
	stop := start + rec.getmeta('Blocksize',index)-1;
	return [start:stop];
};
NOEMIgetTime := function(ref rec,field="y",index=1) {
	return rec.get('Pos', index) / 
	   (rec.headf("Samplerate",index) * rec.headf("SamplerateUnit",index));
};
NOEMIgetVoltage := function(ref rec,field="y",index=1) {
	return  rec.get('f(x)', index) / 
		  rec.headf('ADCMaxChann')*rec.headf('MaxVolt')
};
NOEMIgetFFT := function(ref rec,field="FFT",index=1,raw=F) {
	if ( (rec.getmeta('FFTOffset',index) != rec.getmeta('Offset',index)) ||
	     (rec.getmeta('FFTSize',index) != rec.getmeta('Blocksize',index)) )
	  { fftfiles(rec); };
	fft := GetDataFromMem(ref rec,'FFT', index);
	if (raw) { return fft; }
	else { return abs(fft); };
};
NOEMIgetFrequency := function(ref rec,field="y",index=1,lofreq=F) {
	range := rec.getmeta('FFTRange',index);
	size := rec.getmeta('FFTSize',index);
	xary := (seq(range[1],range[2])-((size-1)/2)-1)/(size-1)*
                  rec.headf("Samplerate",index)*rec.headf("SamplerateUnit",index);
	if (lofreq) { return -xary; }
	else { return (rec.headf("Frequency",index)+rec.headf("LO",index))*
                        rec.headf("FrequencyUnit",index) + xary; };
};
NOEMIgetPower := function(ref rec,field="y",index=1) {
	fft := rec.get('FFT', index);
	size := rec.getmeta('FFTSize',index);
	#freqbin := rec.headf("Samplerate",index)*rec.headf("SamplerateUnit",index)/(size-1);
	#return ((fft/size*2)^2)/50/freqbin;
	return ((fft/size)^2)*2/50;
};
NOEMIgetCalPower := function(ref rec,field="y",index=1) {
	pow := rec.get('Power', index);
	cal := NOEMIGetCal(rec, index);
	return 10^(cal/10)*pow;
};
NOEMIgetNoiseTemp := function(ref rec,field="y",index=1) {
	calpow := rec.get('CalPower', index);
	size := rec.getmeta('FFTSize',index);
	freqbin := rec.headf("Samplerate",index)*rec.headf("SamplerateUnit",index)/(size-1);
	return calpow/freqbin/1.3806e-23;
};
NOEMIgetMeanNoiseTemp := function(ref rec,field="y",index=1) {
	mpow := rec.get('MeanPower', index);
	cal := NOEMIGetCal(rec, index);
	calpow := 10^(cal/10)*mpow
	size := rec.getmeta('FFTSize',index);
	freqbin := rec.headf("Samplerate",index)*rec.headf("SamplerateUnit",index)/(size-1);
	return calpow/freqbin/1.3806e-23;
};
NOEMIgetSkyTemp := function(ref rec,field="y",index=1) {
	freq := rec.get('Frequency', index);
	return 32*( (freq/408e6)^-2.5 );
};

########################################################################

#-----------------------------------------------------------------------
#Name: NOEMI(gen|conv)ZZZ
#-----------------------------------------------------------------------
#Type: Function
#Doc: Converts coordinates from one field to another
#Par: ary		   - array with values to be converted
#Par: rec		   - array with data sets (header is needed)
#Par: index		   - index number of data set
#Par: inv		   - reverse direction of conversion
#Ret: array with converted values
#-----------------------------------------------------------------------

NOEMIconvVolt2FFT := function(ref ary, ref rec,index=1,inv=F,...) {
    rec.putmeta(len(ary),'FFTSize',index);
    rec.putmeta(rec.headf("Samplerate",index)*rec.headf("SamplerateUnit",index)/(len(ary)-1),'FreqBin',index);
    if (is_agent(rec.newsagent)) {rec.newsagent->message(paste('computing FFT for index',index));};
    return fftvec(ary)
};

NOEMIgenPos := function(ref ary,ref rec,index=1,inv=F,start=-1,end=-1,...) {
if (inv) return ary
else return [start:end]
};

NOEMIconvPos2Time := function(ary,rec,index=1,inv=F,...) {
if (inv) {
  return ary * (rec.headf("Samplerate",index) / rec.headf("SamplerateUnit",index))}
else {
  return ary / (rec.headf("Samplerate",index) * rec.headf("SamplerateUnit",index))}
};

NOEMIconvFX2Voltage := function(ref ary, ref rec,index=1,inv=F,...) {
if (inv) {
  return  ary * rec.headf('ADCMaxChann')/rec.headf('MaxVolt')
} else {
  return  ary / rec.headf('ADCMaxChann')*rec.headf('MaxVolt')
}
};

NOEMIconvTime2IF  := function(ref ary, ref rec,index=1,inv=F,...) {
	size:=len(ary);
	#region := rec.getregion('F',index);
	#si := as_integer(size/2)+1;
	#if ((region[1]<1) || (region[1]>si)) region[1]:=1; 
	#if ((region[2]<1) || (region[2]>si)) region[2]:=si; 
        return (-(seq(1,(as_integer(size/2)+1))-((size-1)/2)-1) /
		(size-1)/(ary[2]-ary[1]))

	#size:=as_integer(len(ary)/2)+1
	#return seq(size-1,0)/(size-1)/(ary[3]-ary[1])
};

NOEMIconvIF2Frequency := function(ref ary, ref rec,index=1,inv=F,...) {
  #both directions (IF -> Freq & Freq -> IF) are the same
	return (rec.headf("Frequency",index)+rec.headf("LO",index))*
                   rec.headf("FrequencyUnit",index) - ary
};

NOEMIconvFFT2AbsFFT := function(ref ary, ref rec,index=1,inv=F, ...) {
	return (abs(ary))
};

NOEMIconvFFT2Power := function(ref ary, ref rec,index=1,inv=F,...) {
	size := rec.getmeta('FFTSize',index);
	return ((abs(ary)/size)^2)*2/50;
};


#NOEMIfuncMeanPower := function(ref ary, ref rec,index=1,inv=F,...) {
#	size := rec.getmeta('FFTSize',index);
#	return ((ary/size*2)^2)/50;
#};

NOEMIconvPower2CalPower := function(ref pow, ref rec,index=1,inv=F,...) {
#This makes implicit assumption about the frequency array (same size)
	cal := NOEMIGetCal2(rec, index);
	return 10^(cal/10)*pow;
};
NOEMIconvPower2NoiseT := function(ref calpow, ref rec,index=1,inv=F,...) {
  # Use: Temp = Power/(Bandwidth * kB) 
	return calpow/rec.getmeta('FreqBin',index)/1.3806e-23;
};

NOEMIconvMeanPower2MNT := function(ref mpow,ref rec,index=1,inv=F,...) {
	calpow := NOEMIconvPower2CalPower(mpow, rec, index);
	return  NOEMIconvPower2NoiseT(calpow, rec, index);
};

NOEMIconvFreq2SkyT := function(ref freq,ref rec,index=1,inv=F,...) {
#Average Northern Sky temperature (in K) from Falcke & Gorham (2002)
	return 32*( (freq/408e6)^-2.5 );
};


#-----------------------------------------------------------------------
#Name: NOEMIgenEmptyRec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates the data field records and definiton for the different
#     fields for the NOEMI system.
#
#Ret: Record with fields for vital header information
#-----------------------------------------------------------------------

NOEMIgenEmptyRec := function() {
	ci := [=];
	
	ci.Pos:=[Units='Sample', DefAxis='x', Domain='T', Stored=F, 
		Ref='-', Type='real',
		GenFunc=symbol_value('NOEMIgenPos')];
	ci.Time:=[Units='Seconds', DefAxis='x', Domain='T', Stored=F, 
		Ref='Pos', Type='real', DomDefault='Tx',
		ConvFunc=symbol_value('NOEMIconvPos2Time')];
	ci['f(x)']:=[Units='Counts', DefAxis='y', Domain='T', Stored=T, 
		Ref='-', Type='real', 
		IsInput=T];
	ci.Voltage:=[Units='Volt', DefAxis='y', Domain='T', Stored=F, 
		Ref='f(x)', Type='real', DomDefault='Ty',
		ConvFunc=symbol_value('NOEMIconvFX2Voltage')];
	ci.IntermedFreq:=[Units='Hz', DefAxis='x', Domain='F', Stored=F, 
		Ref='Time', Type='real',
		ConvFunc=symbol_value('NOEMIconvTime2IF')];
	ci.Frequency:=[Units='Hz', DefAxis='x', Domain='F', Stored=F, 
		Ref='IntermedFreq', Type='real', DomDefault='Fx',
		ConvFunc=symbol_value('NOEMIconvIF2Frequency')];
	ci.FFT:=[Units='units', DefAxis='y', Domain='F', Stored=T, 
		Ref='Voltage', Type='complex',
		ConvFunc=symbol_value('NOEMIconvVolt2FFT')];
	ci.AbsFFT:=[Units='units', DefAxis='y', Domain='F', Stored=F, 
		Ref='FFT', Type='real',
		ConvFunc=symbol_value('NOEMIconvFFT2AbsFFT')];
	ci.Power:=[Units='Watt/Bin', DefAxis='y', Domain='F', Stored=F, 
		Ref='AbsFFT', Type='real',
		ConvFunc=symbol_value('NOEMIconvFFT2Power')];
	ci.MeanPower:=[Units='Watt/Bin', DefAxis='y', Domain='F', Stored=T, 
		Ref='-', Type='real',
		GenFunc=symbol_value('NOEMIgenMeanFFT')];
	ci.CalPower:=[Units='Watt/Bin', DefAxis='y', Domain='F', Stored=F, 
		Ref='Power', Type='real', DomDefault='Fy',
		ConvFunc=symbol_value('NOEMIconvPower2CalPower')];
	ci.NoiseT:=[Units='K', DefAxis='y', Domain='F', Stored=F, 
		Ref='CalPower', Type='real',
		ConvFunc=symbol_value('NOEMIconvPower2NoiseT')];
	ci.SkyT:=[Units='K', DefAxis='y', Domain='F', Stored=F, 
		Ref='Frequency', Type='real',
		ConvFunc=symbol_value('NOEMIconvFreq2SkyT')];
	ci.MNT:=[Units='K', DefAxis='y', Domain='F', Stored=F, 
		Ref='MeanPower', Type='real',
		ConvFunc=symbol_value('NOEMIconvMeanPower2MNT')];

	rec.Coordinfo := ci;
	i:=0;j:=0;
	for (ff in field_names(ci) ) {
	  if (has_field(ci[ff],'IsInput') && ci[ff].IsInput) {
	    j:=j+1;
	    rec.Input := ff;
	  };
	  if (ci[ff].Type == 'real') {
	    i:=i+1;
	    rec.Coord[i] := ff;
	    rec.Units[i] := ci[ff].Units;
	  };
	  if (has_field(ci[ff],'DomDefault')) 
		rec.DomDefault[ci[ff].DomDefault] := ff;
	};
	if (j != 1) {
	  print 'Data-Record with more than one input!',j;
	};
	return rec;
}


########################################################################
# Calibration
########################################################################

NOEMIGetCal := function(ref rec, index=1) {
	LOCalFile := rec.head('LocalCalFile',index);
	AntCalFile := rec.head('AntennaCalFile',index);
	CalUnit := rec.headf('CalFileFreqUnit',index);
	LOCalAr := rec.readcalfile(LOCalFile);
	AntCalAr := rec.readcalfile(AntCalFile);
	freqar := NOEMIgetFrequency(rec,'Frequency',index)/CalUnit;
	cal := getcalvals(freqar ,LOCalAr);
	freqar := NOEMIgetFrequency(rec,'Frequency',index,lofreq=F)/CalUnit;
	cal := cal+getcalvals(freqar ,AntCalAr);
	return cal;
};

NOEMIGetCal2 := function(ref rec, index=1) {
	LOCalFile := rec.head('LocalCalFile',index);
	AntCalFile := rec.head('AntennaCalFile',index);
	CalUnit := rec.headf('CalFileFreqUnit',index);
	LOCalAr := rec.readcalfile(LOCalFile);
	AntCalAr := rec.readcalfile(AntCalFile);
	freqar := rec.get('IntermedFreq',index)/CalUnit;
	cal := getcalvals(freqar ,LOCalAr);
	freqar := rec.get('Frequency',index)/CalUnit;
	cal := cal+getcalvals(freqar,AntCalAr);
	return cal;
};


###Other obsolete stuff
#-----------------------------------------------------------------------
#Name: fftfile
#-----------------------------------------------------------------------
#Type: Function
#Doc: Create the FFT for one selected files.
#Ret: T/F
#
#Par: rec =	  			- Data set
#Par: index =				- Index number of record
#Ref: 
#-----------------------------------------------------------------------

fftfile:=function(ref rec,index=1) {
   fft:=fftvec(rec.get(rec.coordinfo('FFT',i).Ref,i),invdir=T)
   rec.put(fft,'FFT',i);
   rec.putmeta(len(fft),'FFTSize',index);
   rec.putmeta((as_integer(len(fft)/2)+1),'FFTlen',index);
   rec.putmeta(rec.headf("Samplerate",index)*rec.headf("SamplerateUnit",index)/(len(fft)-1),'FreqBin',index);
}

#-----------------------------------------------------------------------
#Name: fftfiles
#-----------------------------------------------------------------------
#Type: Function
#Doc: Create the FFT for selected files.
#Ret: T/F
#
#Par: rec =	  			- Data set
#Ref: 
#-----------------------------------------------------------------------

fftfiles:=function(ref rec) {
    nsum:=0; 
    for (i in [1:rec.len][rec.select()]) {
        if (nsum==0) {
           type:=rec.head("DataType",i); 
	   ID:=rec.head("ID",i); 
	 }	   
#Apply only if type and ID match
         if (rec.head("DataType",i)==type && rec.head("ID",i)==ID) {
	    nsum+:=1;
	    if (is_agent(rec.newsagent)) {rec.newsagent->message(spaste('Computing FFT #',as_string(i)))};
	    fftfile(rec,i)
	  }
    }
    if (is_agent(rec.newsagent)) {rec.newsagent->message('FFTs computed!');};
}
