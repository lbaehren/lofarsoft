#######################################################################
#
# Functions and other stuff for the 40MHz bandwidth TIM-Board
# Here the patches for spawning a data record are defined
#
########################################################################



########################################################################

#-----------------------------------------------------------------------
#Name: TIM40revgetFFT
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

TIM40revgetFFT := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  if (inv) return ary
  else {
#  retary:=rec.ref.get('FFT',index);
  retary:=rec.ref.get('CalFFT',index);
  fftsize := rec.ref.getmeta('FFTSize',index)
  rec.setblock(n=1,blocksize=fftsize,offset=0,index=index);
#  rec.putmeta(rec.ref.getmeta('FFTSize',index),'FFTSize',index);
#  rec.putmeta(rec.ref.getmeta('FFTlen',index),'FFTlen',index);
#  rec.putmeta(rec.headf("Samplerate",index)*rec.headf("SamplerateUnit",index)/(len(retary)-1),'FreqBin',index);
  if (rec.get('Doshift',index)) {retary:=retary*exp(1i*rec.get('PhaseGrad',index)*degree)} 
  return rec.flagdat(retary,'FFT',index)

}
};

#-----------------------------------------------------------------------
#Name: TIM40revgetIF
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  
#-----------------------------------------------------------------------

TIM40revgetIF := function (ref ary, ref rec, index=1, inv=F, reffield='',
                           start=-1,end=-1,...) 
{
  if (inv) return ary
  return rec.ref.get('IntermedFreq',index)
};

#-----------------------------------------------------------------------
#Name: TIM40rev(gen|conv)ZZZ
#-----------------------------------------------------------------------
#Type: Function
#Doc: Converts coordinates from one field to another
#Par: ary		   - array with values to be converted
#Par: rec		   - array with data sets (header is needed)
#Par: index		   - index number of data set
#Par: inv		   - reverse direction of conversion
#Ret: array with converted values
#-----------------------------------------------------------------------


TIM40revconvFFT2FX := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
    if (is_agent(rec.newsagent)) {rec.newsagent->message(paste('computing backward FFT for index',index,', version =',rec.version('f(x)',index),', len =',len(ary)));};   
    invdir:=rec.headf('NyquistZone')%2==0;
    fft := FFTtoFx(ary,fftsize=rec.getmeta('FFTSize',index),invdir=invdir)
    return rec.flagdat(real(fft*rec.headf('ADCMaxChann')/rec.headf('MaxVolt')),'f(x)',index)
};

TIM40revconvVoltage2FieldStrength := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
  if (inv) {
    return  ary / data.getmeta('FieldStrengthFactor',index)
  } else {
    return  ary * data.getmeta('FieldStrengthFactor',index)
  };
};

TIM40revIdentity := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
  return  ary;
};


#-----------------------------------------------------------------------
#Name: TIM40revmodifyRec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Modify the data field records and definiton for the different
#     fields for the TIM40rev system, so that they can use 
#     the FFT data of a time-domain based data as their data
#     source. Will be used to plot data filtered in the FFT domain.
#
#Ret: Record with fields for vital header information
#Ref: GetDataDispatch
#-----------------------------------------------------------------------

TIM40revmodifyrec := function(ref rec) {
#patching the original definition of Tim40 for spawning ...
    
   ci := rec.Coordinfo;
	
   ci['f(x)'].Stored:=T; 
   ci['f(x)'].Ref:="FFT";
   ci['f(x)'].ConvFunc:=symbol_value('TIM40revconvFFT2FX');
   ci['f(x)'].GenFunc:=F;
   ci['f(x)'].IsInput:=F

   ci.Voltage.Stored:=F

   ci.IntermedFreq.Ref:='-';
   ci.IntermedFreq.GenFunc:=symbol_value('TIM40revgetIF');
   ci.IntermedFreq.ConvFunc:=F;
   ci.IntermedFreq.CrossRef:='FFT'

   ci.FFT.Stored:=F
   ci.FFT.Ref:='-'
   ci.FFT.GenFunc:=symbol_value('TIM40revgetFFT');
   ci.FFT.ConvFunc:=F;
   ci.FFT.IsInput:=T
   ci.FFT.CrossRef:='CalFFT' #currently not used!

   ci.CalFFT.ConvFunc:=symbol_value('TIM40revIdentity');

   ci.FieldStrength:=[Units='Volt/m/MHz', DefAxis='y', Domain='T', Stored=F, 
             Ref='Voltage', Type='real', DomDefault='Ty',
             ConvFunc=symbol_value('TIM40revconvVoltage2FieldStrength')];

   TIM40finishcoordinforec(rec,ci);
   return rec
}


