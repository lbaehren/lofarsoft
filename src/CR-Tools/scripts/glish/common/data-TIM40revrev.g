#######################################################################
#
# Functions and other stuff for the 40MHz bandwidth TIM-Board
#
# This  introduces a new data type for spawning which can be used to spawn from a
#spawned data record. This way we convert from time domain to Fourier domain:
#
#data:   [T -> F]  
#spawning: [F -> T]
#2nd spawning: [T -> F]
#
#This allows one to convert a time series data which was cleaned from RFI in
#the Frequency domain back to FFT and use a small block size for the latter
#operation. This is crucial for mapping cosmic rays with the Skymapper.
#
#How this works is illustrated in the new functions findpointsourcedist and makecrmap.
#
########################################################################

#-----------------------------------------------------------------------
#Name: TIM40revgetFFT
#-----------------------------------------------------------------------
#Type: Function
#Doc: 
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

TIM40revrevgetDummy := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  if (inv) return ary
  else {
  retary:=rec.ref.get('f(x)',index,start=start,end=end);
#  fftsize := rec.ref.getmeta('FFTSize',index)
#  rec.setblock(n=1,blocksize=fftsize,offset=0,index=index);
  return rec.flagdat(retary,'f(x)',index)
  }
};



#-----------------------------------------------------------------------
#Name: TIM40revrevgenEmptyRec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates the data field records and definiton for the different
#     fields for the TIM40f system.
#
#Ret: Record with fields for vital header information
#-----------------------------------------------------------------------

TIM40revrevgenEmptyRec := function() {
    rec:=[=];
    ci := TIM40coordinforec();

   ci['Dummy'].Stored:=F
   ci['Dummy'].Ref:='-'
   ci['Dummy'].GenFunc:=symbol_value('TIM40revrevgetDummy');
   ci['Dummy'].ConvFunc:=F;
   ci['Dummy'].IsInput:=T
   ci['Dummy'].CrossRef:='f(x)'  #currently not used! -- why?

   ci.CalFFT.ConvFunc:=symbol_value('TIM40revIdentity');

    rec:=TIM40finishcoordinforec(rec,ci)
	return rec;
}





