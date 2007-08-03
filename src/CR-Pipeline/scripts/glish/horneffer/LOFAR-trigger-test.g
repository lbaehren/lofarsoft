#some routines to help me develop the LOFAR trigger

#-----------------------------------------------------------------------
#Name: filter
#-----------------------------------------------------------------------
#Type: Function
#Doc: do an IIR filtering of the data
#     (implemented in glish so probably _slow_)
#Ret: filtered data
#
#Par: ary 	= 		- unfiltered (input) data  
#Par: freq	= 		- center frequency of the filter (in MHz)
#Par: bw	= 		- bandwidth of the filter (in MHz)
#Par: SR	= 80	- sampling rate of the data (in MHz)       
#
#-----------------------------------------------------------------------

filter := function(ary,freq,bw,SR=80){
  if (freq>(SR/2)) {freq:= SR-freq};
  n:=len(ary);
  if (n<4) { print "filter: input array too short!"; return F;};
  erg := array(0.,n);

  Q := freq/(2*pi*bw);
  w0 := 2*pi*freq/SR;
  alpha := sin(w0)/(2*Q);

  a := [(-1-alpha),(2*cos(w0)),(-1+alpha)];
  b := [1,(-2*cos(w0)),1]
#  a := a/abs(a[1]);
print a,b,sum(a);
  for (i in [4:n]) { erg[i] := sum(erg[(i-1):(i-2)]*a[2:3],ary[i:(i-2)]*b); }
  return erg;
}

#-----------------------------------------------------------------------
#Name: fpga_runing_avg
#-----------------------------------------------------------------------
#Type: Function
#Doc: calculate an approximated "running average" as one would do it on an FPGA
#
#Ret: record with .avg and .sig
#
#Par: ary 	= 		- input data  
#Par: block = 100   - blocksize
#
#-----------------------------------------------------------------------

fpga_runing_avg := function(ary,block=100){
  n:=len(ary);
  erg.avg := array(1.,n);
  erg.sig := array(1.,n);
  wgh1 := 1/block;
  wgh2 := (block-1)/block;
  ary := abs(ary)
  for (i in [2:n]){
    erg.avg[i] := wgh1*ary[i] + wgh2*erg.avg[(i-1)];
    erg.sig[i] := wgh1*(ary[i]^2)+erg.avg[(i-1)]^2-erg.avg[i]^2+wgh2*erg.sig[(i-1)];
  }
  erg.sig := sqrt(erg.sig);
  return erg;
}


# good := quality3==1 & hcomp> 10*errs
test_trig := function(ary,n=3,block=100){
  runnings := fpga_runing_avg(ary,block);
  erg := abs(ary) > n*runnings.sig + abs(runnings.avg);
  return erg;
}

test_all_unfilt := function(ants,n=3,block=100){
  nant := len(ants)
  erg := 0;
  for (i in ants){
    erg := erg+ test_trig(data.aux.get('f(x)',i),n=n,block=block);
  };
  return erg;
}

test_all_filt := function(ants,n=3,block=100){
  nant := len(ants)
  erg := 0;
  for (i in ants){
    unfilt := data.aux.get('f(x)',i);
    #event 62
    filt := filter(filter(filter(filter(filter(unfilt,62.2,2),50,0.5),60,1),55.3,1),67.8,1);
    erg := erg+ test_trig(filt,n=n,block=block);
  };
  return erg;
}


getRMS := function(ants,dofilter=T){
  unfRMS := array(0.,max(ants))
  unfSNR := array(0.,max(ants))
  filtRMS := array(0.,max(ants))
  filtSNR := array(0.,max(ants))
  for (i in ants) {
    unfilt := data.aux.get('f(x)',i);
    unfRMS[i] := stddev(unfilt[15000:30000]);
    unfSNR[i]:= max(abs(unfilt[32620:32627]))/stddev(abs(unfilt[15000:30000]));
    if (dofilter) {
      filt := filter(filter(filter(filter(unfilt,62.2,2),50,0.5),60,1),67.8,1);
      filtRMS[i] := stddev(filt[15000:30000])
      filtSNR[i]:= max(abs(filt[32623:32630]))/stddev(abs(filt[15000:30000]));
    }
  };
  print 'Unfiltered RMS:',min(unfRMS[ants]),mean(unfRMS[ants]),max(unfRMS[ants]);
  print 'Unfiltered SNR:',min(unfSNR[ants]),mean(unfSNR[ants]),max(unfSNR[ants]);
  print 'Filtered RMS:',min(filtRMS[ants]),mean(filtRMS[ants]),max(filtRMS[ants]);
  print 'Filtered SNR:',min(filtSNR[ants]),mean(filtSNR[ants]),max(filtSNR[ants]);
}

#[8 62 107 108 280 300 320 328 332 337 347 365 382 396 402]
#8: 8,8,8
#62: 4,4,1
#107:
#108: 5,3,1
#280: 6,1,1
#300: 8,6,2
#320: 9,7,7
#328: 6,7,3
#332:
#337:
#347:
#365:
#382:
#396:
#402:
