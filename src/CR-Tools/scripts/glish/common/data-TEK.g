#######################################################################
#
# Functions and other stuff for the Tektronix Digital Oszilloscopes
#
########################################################################


########################################################################
# timread-setup
########################################################################


#-----------------------------------------------------------------------
#Name: TEKHeader
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates a default header entry for undocumented TEK datafiles.
#Ret: Data header record
#
#-----------------------------------------------------------------------

TEKHeader:=function(
	Project='Test-Measurements',
	DataType='TEK',
	Filename='test',
	ID=globalpar.DataID,
	Measurement='1',
	FrequencyUnit='1e6',
	SamplerateUnit='1e6',
	Samplerate='1',
	#Bandwidth='40',
	Location='unknown!',
	AntPos='0,0,0',
	Weather='Undefined',
	Time='Undefined',
	Date='Undefined',
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
	#Bandwidth=Bandwidth,
	Location=Location,
	AntPos=AntPos,
	Weather=Weather,
	Time=Time,
	Date=Date,
	LocalCalFile=LocalCalFile,
	AntennaCalFile=AntennaCalFile,
	CalFileFreqUnit=CalFileFreqUnit,
	Files=Files];
return header
}


########################################################################
#-----------------------------------------------------------------------
#Name: TEK(gen|conv)ZZZ
#-----------------------------------------------------------------------
#Type: Function
#Doc: Converts coordinates from one field to another
#Par: ary		   - array with values to be converted
#Par: rec		   - array with data sets (header is needed)
#Par: index		   - index number of data set
#Par: inv		   - reverse direction of conversion
#Ret: array with converted values
#-----------------------------------------------------------------------

TEKconvVolt2FFT := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
    rec.putmeta(len(ary),'FFTSize',index);
    rec.putmeta(rec.headf("Samplerate",index)*rec.headf("SamplerateUnit",index)/(len(ary)-1),'FreqBin',index);
    #if (is_agent(rec.newsagent)) {rec.newsagent->message(paste('computing FFT for index',index));};
    return fftvec(ary)
};

TEKgenPos := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
if (inv) return ary
else return [start:end]
};

TEKconvPos2Time := function(ary,rec,index=1,inv=F, reffield='',...) {
if (inv) {
  return ary * (rec.headf("Samplerate",index) / rec.headf("SamplerateUnit",index))}
else {
  return ary / (rec.headf("Samplerate",index) * rec.headf("SamplerateUnit",index))}
};

TEKconvFX2Voltage := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
if (inv) {
  return  ary;
} else {
  return  ary;
}
};

TEKconvTime2IF  := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
	size:=len(ary);
        return (-(seq(1,(as_integer(size/2)+1))-((size-1)/2)-1) /
		(size-1)/(ary[2]-ary[1]))
};

TEKconvIF2Frequency := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
  #both directions (IF -> Freq & Freq -> IF) are the same
	return  ary;
};

TEKconvFFT2AbsFFT := function(ref ary, ref rec,index=1,inv=F, reffield='', ...) {
	return (abs(ary))
};

TEKconvFFT2Power := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
	size := rec.getmeta('FFTSize',index);
	return ((abs(ary)/size)^2)*2/50;
};


#TEKfuncMeanPower := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
#	size := rec.getmeta('FFTSize',index);
#	return ((ary/size*2)^2)/50;
#};

TEKconvPower2CalPower := function(ref pow, ref rec,index=1,inv=F, reffield='',...) {
	return pow;
};
TEKconvPower2NoiseT := function(ref calpow, ref rec,index=1,inv=F, reffield='',...) {
  # Use: Temp = Power/(Bandwidth * kB) 
	return calpow/rec.getmeta('FreqBin',index)/1.3806e-23;
};

TEKconvMeanPower2MNT := function(ref mpow,ref rec,index=1,inv=F, reffield='',...) {
	calpow := TEKconvPower2CalPower(mpow, rec, index);
	return  TEKconvPower2NoiseT(calpow, rec, index);
};

TEKconvFreq2SkyT := function(ref freq,ref rec,index=1,inv=F, reffield='',...) {
#Average Northern Sky temperature (in K) from Falcke & Gorham (2002)
	return 32*( (freq/408e6)^-2.5 );
};


#-----------------------------------------------------------------------
#Name: TEKgenEmptyRec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates the data field records and definiton for the different
#     fields for the TEK system.
#
#Ret: Record with fields for vital header information
#-----------------------------------------------------------------------

TEKgenEmptyRec := function() {
	ci := [=];
	
	ci.Pos:=[Units='Sample', DefAxis='x', Domain='T', Stored=F, 
		Ref='-', Type='real',
		GenFunc=symbol_value('TEKgenPos')];
	ci.Time:=[Units='Seconds', DefAxis='x', Domain='T', Stored=F, 
		Ref='Pos', Type='real', DomDefault='Tx',
		ConvFunc=symbol_value('TEKconvPos2Time')];
	ci['f(x)']:=[Units='Counts', DefAxis='y', Domain='T', Stored=T, 
		Ref='-', Type='real', 
		IsInput=T];
	ci.Voltage:=[Units='Volt', DefAxis='y', Domain='T', Stored=F, 
		Ref='f(x)', Type='real', DomDefault='Ty',
		ConvFunc=symbol_value('TEKconvFX2Voltage')];
	ci.IntermedFreq:=[Units='Hz', DefAxis='x', Domain='F', Stored=F, 
		Ref='Time', Type='real',
		ConvFunc=symbol_value('TEKconvTime2IF')];
	ci.Frequency:=[Units='Hz', DefAxis='x', Domain='F', Stored=F, 
		Ref='IntermedFreq', Type='real', DomDefault='Fx',
		ConvFunc=symbol_value('TEKconvIF2Frequency')];
	ci.FFT:=[Units='units', DefAxis='y', Domain='F', Stored=T, 
		Ref='Voltage', Type='complex',
		ConvFunc=symbol_value('TEKconvVolt2FFT')];
	ci.AbsFFT:=[Units='units', DefAxis='y', Domain='F', Stored=F, 
		Ref='FFT', Type='real',
		ConvFunc=symbol_value('TEKconvFFT2AbsFFT')];
	ci.Power:=[Units='Watt/Bin', DefAxis='y', Domain='F', Stored=F, 
		Ref='AbsFFT', Type='real',
		ConvFunc=symbol_value('TEKconvFFT2Power')];
	ci.MeanPower:=[Units='Watt/Bin', DefAxis='y', Domain='F', Stored=T, 
		Ref='-', Type='real',
		GenFunc=symbol_value('TEKgenMeanFFT')];
	ci.CalPower:=[Units='Watt/Bin', DefAxis='y', Domain='F', Stored=F, 
		Ref='Power', Type='real', DomDefault='Fy',
		ConvFunc=symbol_value('TEKconvPower2CalPower')];
	ci.NoiseT:=[Units='K', DefAxis='y', Domain='F', Stored=F, 
		Ref='CalPower', Type='real',
		ConvFunc=symbol_value('TEKconvPower2NoiseT')];
	ci.SkyT:=[Units='K', DefAxis='y', Domain='F', Stored=F, 
		Ref='Frequency', Type='real',
		ConvFunc=symbol_value('TEKconvFreq2SkyT')];
	ci.MNT:=[Units='K', DefAxis='y', Domain='F', Stored=F, 
		Ref='MeanPower', Type='real',
		ConvFunc=symbol_value('TEKconvMeanPower2MNT')];

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
#-----------------------------------------------------------------------
#Name: input_TEK
#-----------------------------------------------------------------------
#Type: Function
#Doc: reads in data from one or more TEK-files
#-----------------------------------------------------------------------
input_TEK := function(ref rec, files, dir='',env=F) {
  newdata(rec);
  globalpar.DataID+:=1;
  MinBlocksize:=globalpar.BlocksizeForDisplay;

  for (filename in files) {
    filename := spaste(dir,filename);
    feld := readdat(filename,type='tab');
    h := TEKHeader(Project='LOPES: Measurement with Tektronics DSO');
    h.Date:= dq.time([value=(time()+3506716800),unit='s'],form='ymd no_time');
    h.Time:= dq.time([value=(time()+3506716800),unit='s']);
    h.Files := filename;
    h.Samplerate := 1/(feld[2]*as_float(h.SamplerateUnit));
    i:=rec.newentry(h);
    arylen := len(feld)-5;
    fdata := feld[5:(arylen+5)];
    if (env) {
      h.Samplerate := h.Samplerate/2;
      arylen := as_integer(arylen/2);
      for (j in seq(arylen)) {
        fdata[j] := (fdata[((2*j)-1)]+fdata[(2*j)])/2
      };
      fdata := fdata[1:arylen];
    };
    for (ff in rec.get('Input',i)) {
      rec.put(fdata,ff,i)
    };
    for (ff in rec.coord(i)) {rec.touch(ff,i,version=0)}
    rec.setregion([1,min(MinBlocksize,arylen)],'T',i)
    rec.putmeta(0,'FileOffset',i);
    rec.putmeta(arylen,'FileBlocksize',i);
    rec.putmeta(0,'Offset',i);
    rec.putmeta(min(MinBlocksize,arylen),'Blocksize',i);
  };
};

