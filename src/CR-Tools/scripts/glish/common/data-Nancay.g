#######################################################################
#
# Functions and other stuff for the Nancay data
#
########################################################################

#-----------------------------------------------------------------------
#Name: NANCAY(gen|conv)ZZZ
#-----------------------------------------------------------------------
#Type: Function
#Doc: Converts coordinates from one field to another
#Par: ary		   - array with values to be converted
#Par: rec		   - array with data sets (header is needed)
#Par: index		   - index number of data set
#Par: inv		   - reverse direction of conversion
#Ret: array with converted values
#-----------------------------------------------------------------------

NANCAYgetDummy := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  global globalpar
  globalpar.lastaction:=spaste('Reading from Disk',rec.head('Files',index));
  if (rec.head('DataType')=='NANCAY') {ofs:=(rec.getmeta('Offset',index)*2+185)}
  else {ofs:=rec.getmeta('Offset',index)*2};
#print 'NANCAYgetDummy: ',rec.head('Files',index),ofs,rec.getmeta('Blocksize',index),rec.head('DataFormat')
  return readdat(rec.head('Files',index),offset=ofs,blocksize=rec.getmeta('Blocksize',index),type=rec.head('DataFormat'))
};
NANCAYconvDummy2fx := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
  #return  rec.flagdat(sqrt_signed(ary),'f(x)',index)
  return  rec.flagdat(ary,'f(x)',index)
}
NANCAYconvIF2Frequency := function(ref ary, ref rec,index=1,inv=F,...) {
  #both directions (IF -> Freq & Freq -> IF) are the same
	return ary+(rec.headf("Frequency",index)-rec.headf("LO",index))*
                   rec.headf("FrequencyUnit",index)
};
NANCAYconvVolt2RawFFT := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
    if (rec.head('Observatory')=='ITS') {invdir:=rec.headf('NyquistZone')%2==1} # just for jup6b (IDLbinary)
    else {invdir:=rec.headf('NyquistZone')%2==0};
    if ((len(ary)>2^20)&is_agent(rec.newsagent)) {rec.newsagent->message(paste('computing FFT for index',index));};
    return fftvec(ary,invdir=invdir);
};
NANCAYconvRawFFT2FFT := function(ref ary, ref rec,index=1,inv=F, reffield='',...)
{
  return rec.flagdat(ary,'FFT',index)
};

#-----------------------------------------------------------------------
#Name: NANCAYgenEmptyRec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates the data field records and definiton for the different
#     fields for the NANCAY data
#
#Ret: Record with fields for vital header information
#-----------------------------------------------------------------------

NANCAYgenEmptyRec := function() {
	ci := [=];
	
	ci.Pos:=[Units='Sample', DefAxis='x', Domain='T', Stored=F, 
		Ref='-', Type='real',
		GenFunc=symbol_value('NOEMIgenPos')];
	ci.Time:=[Units='Seconds', DefAxis='x', Domain='T', Stored=F, 
		Ref='Pos', Type='real', DomDefault='Tx',
		ConvFunc=symbol_value('NOEMIconvPos2Time')];
	ci['Dummy']:=[Units='Counts', DefAxis='y', Domain='T', Stored=F, 
		Ref='-', Type='unreal',GenFunc=symbol_value('NANCAYgetDummy'),
		IsInput=T];
	ci['f(x)']:=[Units='Counts', DefAxis='y', Domain='T', Stored=T, 
		Ref='Dummy', Type='real',ConvFunc=symbol_value('NANCAYconvDummy2fx'),
		IsInput=F];
	ci.Voltage:=[Units='Volt', DefAxis='y', Domain='T', Stored=F, 
		Ref='f(x)', Type='real', DomDefault='Ty',
		ConvFunc=symbol_value('NOEMIconvFX2Voltage')];
	ci.IntermedFreq:=[Units='Hz', DefAxis='x', Domain='F', Stored=F, 
		Ref='FFT', Type='real',
		ConvFunc=symbol_value('TIM40genIF')];
	ci.Frequency:=[Units='Hz', DefAxis='x', Domain='F', Stored=F, 
		Ref='IntermedFreq', Type='real', DomDefault='Fx',
		ConvFunc=symbol_value('NANCAYconvIF2Frequency')];


	ci.RawFFT:=[Units='units', DefAxis='y', Domain='F', Stored=T, 
		Ref='Voltage', Type='complex',
		ConvFunc=symbol_value('NANCAYconvVolt2RawFFT')];
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
	ci.CalPower:=[Units='Watt/Bin', DefAxis='y', Domain='F', Stored=F, 
		Ref='CalFFT', Type='real', DomDefault='Fy',
		ConvFunc=symbol_value('TIM40convFFT2Power')];


	ci.MeanPower:=[Units='Watt/Bin', DefAxis='y', Domain='F', Stored=T, 
		Ref='-', Type='real',
		GenFunc=symbol_value('NOEMIgenMeanFFT')];
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
