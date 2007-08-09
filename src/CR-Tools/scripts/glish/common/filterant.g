#-------------------------------------------------------------------------------
# $Id$
#-------------------------------------------------------------------------------
#
# A collection of routines for ...?
#
#-------------------------------------------------------------------------------


#-----------------------------------------------------------------------
#Name: getFfilter
#-----------------------------------------------------------------------
#Type: Function
#Doc: generate a frequency-filter, that filter out all peaks inm the Spectrum
#Ret: (1-dim) array with the filter
#
#Par: rec    				- record with the data               
#Par: from   				- index to work on
#Par: filter 				- predetermined filter
#-----------------------------------------------------------------------

getFfilter := function(ref rec, from, filter=T) {
  weight := 10
  while (sum(weight) > 1) {
    weight := peakary2(rec.get('AbsFFT',from[1])*filter,3,0);
    filter := ((!weight) & filter);
    print sum(weight);
  };

  return filter;
};

#-----------------------------------------------------------------------
#Name: getFfilter1a
#-----------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  rec     - record with the data
#Par:  from    - index to work on
#Par:  filter  - predetermined filter
#-----------------------------------------------------------------------

getFfilter1a := function(ref rec, from, filter=T) {
  for (i in [1:3]) {
    weight := peakary2(rec.get('AbsFFT',from[1])*filter,3,0);
    filter := ((!weight) & filter);
    if (sum(weight)<10) break;
  };

  return filter;
};

#-----------------------------------------------------------------------
#Name: getFfilter2
#-----------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  ary     - Array of data to which the filter is applied.
#Par:  filter  - predetermined filter
#-----------------------------------------------------------------------

getFfilter2 := function(ary , filter=T) {
  weight := 10
  while (sum(weight) > 1) {
    weight := peakary2(ary*filter,3,0);
    filter := ((!weight) & filter);
  };

  return filter;
};

#-----------------------------------------------------------------------
#Name: getFfilter_fast
#-----------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  rec     - record with the data
#Par:  from    - index to work on
#Par:  filter  - predetermined filter
#-----------------------------------------------------------------------

getFfilter_fast := function(ref rec, from, filter=T) {
  for (i in [1:3]) {
    weight := peakary(sqrt(rec.get('CalPower',from[1]))*filter,4,0);
    filter := ((!weight) & filter);
    if (sum(weight)<10) break;
  };
  return filter;
};


#-----------------------------------------------------------------------
#Name: filterONEant
#-----------------------------------------------------------------------
#Type: Function
#Doc: frequency-filter a single antenna 
#      i.e. remove all peaks in the spectrum of one data-entry by setting them to
#      zero, and fft it back into a new data-entry index (toind = -1) or 
#      overwrite a given data-entry index (toind >0) 
#Ret: index of filtered data
#
#Par: rec         - record with the data               
#Par: fromind     - index from where to get the data
#Par: toind       - index where to put the data (-1=new)
#Par: filter      - predetermined filter
#-----------------------------------------------------------------------

filterONEant := function(ref rec, fromind, toind=-1, filter=T){
  filter := getFfilter(rec, fromind, filter);
  toind := RecoverEntry(rec,fromind,toind,weight=filter);
  return toind;
};



#-----------------------------------------------------------------------
#Name: freq_cut
#-----------------------------------------------------------------------
#Type: Function
#Doc: select a certain band in frequency
#Ret: array with filtered time-domain data
#
#Par: rec    				- record with the data               
#Par: fromind   			- index from where to get the data
#-----------------------------------------------------------------------

freq_cut := function(rec, index, lofreq=43e6, hifreq=76e6,window=F)
{
  farr := rec.get('Frequency',index);
  filter := (farr>lofreq)&(farr<hifreq);
  fromary := rec.get('Voltage',index);
  size := len(fromary);
  if (len(window) == len(fromary)){
    fromary := fromary * window;
  };
  fromary := fftvec(fromary);
  if (size != rec.getmeta('FFTSize',index)) {
    print '(size != rec.getmeta("FFTSize",index))';
    return F;
  };
  ary := FFTtoFx(fromary, size, filter);
#  return (((abs(fromary) * 2) ^ 2)/size/sum(window) / 50);

  return real(ary);
};


#-----------------------------------------------------------------------
#Name: genant
#-----------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  rec       - record with the data 
#Par:  from      - 
#Par:  pulse     - 
#Par:  toind     - 
#Par:  delay     - 
#Par:  no        - 
#Par:  rfidelay  - 
#-----------------------------------------------------------------------

genant := function (ref rec,
                    from,
                    pulse,
                    toind=-1, 
                    delay=[1:10],
                    no=10,
                    rfidelay=0)
{
  if (!(is_integer(no) && (shape(no) == 1 ))) {
    print '"no" needs to be an scalar integer!';
    return F;
  }
  if (len(delay) != no) {delay := [1,no];};
  if (toind == -1) {
    toind := array(0,no);
      for (i in [1:no]) {
        toind[i] := RecoverEntry(rec,from[1]);
      };
  };

  psiz := len(pulse);
  fl := len(from);
  ary := rec.get('f(x)',from[1]);
  size := len(ary);
  hsize := as_integer(size/2);

  for (i in [1:no]) {
    tmpary := rec.get('f(x)',from[(((i-1)%fl)+1)]);
    if (len(rfidelay) == no) {
      tmpary[1:(size-abs(rfidelay[i]))] :=
        tmpary[max(1,(1+rfidelay[i])):min(size,(size+rfidelay[i]))];
    };
    tmpary[(hsize+delay[i]):(hsize+delay[i]+psiz-1)] := 
      tmpary[(hsize+delay[i]):(hsize+delay[i]+psiz-1)] + pulse;
    rec.put(tmpary,'f(x)',toind[i]);
  };
  erg.delay := delay;
  erg.toind := toind;
  return erg;
};


#-----------------------------------------------------------------------
#Name: AntennaOverlay
#-----------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  rec         - record with the data 
#Par:  fromind     - 
#Par:  tmpind      - 
#Par:  weight      - 
#Par:  timeweight  - 
#Par:  delay       - 
#Par:  no          - 
#Par:  returnall   - 
#-----------------------------------------------------------------------

AntennaOverlay := function (ref rec,
                            fromind,
                            tmpind=-1,
                            weight=-1, 
                            timeweight=1,
                            delay=[1:10],
                            no=10,
                            returnall=F)
{
  global recovant;
	if (!(is_integer(no) && (shape(no) == 1 ))) {
	  print '"no" needs to be an scalar integer!';
	  return F;
	};
	if (len(delay) != no) {
	  print '"len(delay) != no"!',len(delay), no;
	  return F;
	};
	if (len(fromind) != no) {
	  print '"len(fromind) != no"!',len(fromind), no;
	  return F;
	};
	if ((tmpind<1)||(tmpind>rec.len)) {
	  tmpind := RecoverEntry(rec,1);
	};
	tmpary := rec.get('Voltage',fromind[1]);
	fullsize := len(tmpary); 
	lower := max(1, (-min(delay)+1) );
	upper := min(fullsize, (fullsize-max(delay)) );
	tmpary := array(0.,fullsize);
	recovant := array(0.,no,fullsize);
	for (i in [1:no]) {
	  RecoverEntry(rec,fromind[i],tmpind,weight=weight);
	  #RecoverEntry(rec,fromind[i],tmpind,weight=weight,timeweight=timeweight);
	  recovant[i,] := rec.get('Voltage',tmpind);
	  tmpary[lower:upper] := tmpary[lower:upper]+ 
	    #recovant[i,(lower+delay[i]):(upper+delay[i])]
	    (recovant[i,]*timeweight)[(lower+delay[i]):(upper+delay[i])]
	    #print (lower+delay[i]),(upper+delay[i]);
	};
	tmpary := tmpary/no;
	if (returnall) {return recovant;};
	return tmpary;
};


#-----------------------------------------------------------------------
#Name: filterant
#-----------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  rec          - record with the data
#Par:  fromind      - 
#Par:  tmpind       - 
#Par:  weight       - 
#Par:  timeweight   - 
#Par:  delay        - 
#Par:  no           - 
#Par:  filt         - 
#Par:  thresh       - 
#-----------------------------------------------------------------------

filterant := function (ref rec,
                       fromind,
                       tmpind=-1,
                       weight=-1,
                       timeweight=1, 
                       delay=[1:10],
                       no=10,
                       filt=6,
                       thresh=4)
{
  global timewgh;

	if (!(is_integer(no) && (shape(no) == 1 ))) {
	  print '"no" needs to be an scalar integer!';
	  return F;
	};
	if (len(delay) != no) {
	  print '"len(delay) != no"!',len(delay), no;
	  return F;
	};
	if (len(fromind) != no) {
	  print '"len(fromind) != no"!',len(fromind), no;
	  return F;
	};
	if ((tmpind<1)||(tmpind>rec.len)) {
	  tmpind := RecoverEntry(rec,fromind[1]);
	};
	tmpary := rec.get('Voltage',fromind[1]);
	fullsize := len(tmpary);;
	lower := max(1, (-min(delay)+1) );
	upper := min(fullsize, (fullsize-max(delay)) );
	tmpary := array(0.,fullsize);
	recovant := array(0.,no,fullsize);
	for (i in [1:no]) {
	  RecoverEntry(rec,fromind[i],tmpind,weight=weight,timeweight=timeweight);
	  recovant[i,] := rec.get('Voltage',tmpind);
	  tmpary[lower:upper] := tmpary[lower:upper]+
	    (recovant[i,]*timeweight)[(lower+delay[i]):(upper+delay[i])]
	};
	tmpary := tmpary/no;

	timewgh := array(0.,no,fullsize);
	for (i in [1:no]) {
 	  powfilt := cMeanFilter((tmpary^2),filt);
 	  power := cMeanFilter((recovant[i,]^2),filt);
	  pfiltmom := moments(2,powfilt);
	  powmom := moments(2,power);
	  peakarr := power>(powmom.mean+powmom.stddev*thresh);
	  timewgh[i,] := (thresh/2*powfilt[1:fullsize]/pfiltmom.stddev)>(power[1:fullsize]/powmom.stddev)*peakarr;
	  timewgh[i,10:(fullsize-10)]:= (cMeanFilter(timewgh[i,]*1,20)==1)[10:(fullsize-10)];
	}
	tmpary := array(0.,fullsize);
	for (i in [1:no]) {
	  tmpary[lower:upper] := tmpary[lower:upper]+
	    (recovant[i,]*timeweight*timewgh[i,])[(lower+delay[i]):(upper+delay[i])]
	};
	tmpary := tmpary/no;

	return tmpary;
};

#-----------------------------------------------------------------------
#Name: genfilt
#-----------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  rec     - 
#Par:  from    - 
#Par:  pulse   - 
#Par:  weight  - 
#-----------------------------------------------------------------------

genfiltdata.toind  := -1;
genfiltdata.tmpind := -1;
genfiltdata.samplefreq := 40.e6;

genfilt := function (ref rec,
                     from,
                     pulse,
                     weight=-2)
{
  tmpdata := ref genfiltdata;
  tmp := genant(rec,from,pulse,toind=genfiltdata.toind)
  tmpdata.toind  := tmp.toind;
  if (genfiltdata.tmpind == -1) {
    tmpdata.tmpind := RecoverEntry(rec,1);
  };
	if (weight == -2) {
	  weight := T;
	  filter := 10;
	  while (sum(filter) > 1) {
	    #filter := peakary(rec.get('AbsFFT',from[1])*weight,5,0);
	    filter := peakary2(rec.get('AbsFFT',from[1])*weight,3,0);
	    weight := ((!filter) & weight);
	  #print sum(filter),sum(!weight);
	  };
	  #print sum(filter),sum(!weight);
	};
	tmp := filterant(rec,genfiltdata.toind,genfiltdata.tmpind,weight=weight,
			delay=tmp.delay);
	print sum(peakary((tmp^2),8,6));
	return tmp;
};

#-----------------------------------------------------------------------
#Name: genfilt2
#-----------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  rec     - 
#Par:  from    - 
#Par:  pulse   - 
#Par:  weight  - 
#Par:  filt    - 
#Par:  thresh  - 
#Ref:  genant
#-----------------------------------------------------------------------

genfilt2 := function (ref rec,
                      from,
                      pulse,
                      weight=-2,
                      filt=6,
                      thresh=4)
{
  tmpdata := ref genfiltdata;
  tmp := genant(rec,from,pulse,toind=genfiltdata.toind)
	tmpdata.toind  := tmp.toind;
	if (genfiltdata.tmpind == -1) {
	  tmpdata.tmpind := RecoverEntry(rec,1);
	};
	if (weight == -2) {
	  weight := T;
	  filter := 10;
	  while (sum(filter) > 1) {
	    #filter := peakary(rec.get('AbsFFT',from[1])*weight,5,0);
	    filter := peakary2(rec.get('AbsFFT',from[1])*weight,3,0);
	    weight := ((!filter) & weight);
	  #print sum(filter),sum(!weight);
	  };
	  #print sum(filter),sum(!weight);
	};
	temp := AntennaOverlay(rec,genfiltdata.toind,genfiltdata.tmpind,weight=weight,
			delay=tmp.delay);

 	  powfilt := cMeanFilter((temp^2),filt);
 	  power := cMeanFilter((rec.get('Voltage',genfiltdata.tmpind)^2),filt);
	  pfiltmom := moments(2,powfilt);
	  powmom := moments(2,power);
	  peakarr := powfilt>(pfiltmom.mean+pfiltmom.stddev*thresh);
	  size := min(len(powfilt),len(power));
	  #timeweight := array(T,len(power));
	  timeweight := (powfilt[1:size]/pfiltmom.stddev)>(power[1:size]/powmom.stddev)*peakarr;
	  timeweight[10:(size-10)]:= (cMeanFilter(timeweight*1,20)==1)[10:(size-10)];

	  #timeweight := timeweight + (!timeweight)* sqrt(powmom.stddev/power);
	  #timeweight[(timeweight > 1)] := 1.;
	
	erg.sera := temp;
	erg.tw := timeweight;
	erg.serb := AntennaOverlay(rec,genfiltdata.toind,genfiltdata.tmpind,weight=weight,
			timeweight=timeweight,delay=tmp.delay);
	
	filtser := cMeanFilter((erg.serb^2),2);
	ppos := order(-filtser)[1];
	gf := gauss1dfitter();
	gf.setstate([height=filtser[ppos], width=1, center=ppos, maxiter=100]);
	gfstate := gf.fit([1:size],filtser);
	erg.gf := gfstate;
	erg.gfarr := gf.eval([1:size]);
	
	return erg;
};


#-----------------------------------------------------------------------
#Name: skytrial
#-----------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  dataarr      - 
#Par:  region       - 
#Par:  antennapos   - 
#Par:  beamwidth    - 
#Par:  startze      - 
#Ref:  stoverlay
#-----------------------------------------------------------------------

skytrial := function (dataarr,
                      region, 
                      antennapos=0,
                      beamwidth=0,
                      startze=0.)
{
  shap := shape(dataarr);
  if (len(shap) != 2) {
    print 'skytrial: need 2-dim data!';
    return F;
  };
	if ( (len(region)==2) & ((min(region)<1) | (max(region)>shap[2])) ) {
	  print 'skytrial: region to large!'
	  return F;
	};
	if (len(shape(antennapos)) != 2) {
	  print '*** Warning! *** Using dummy antenna positions! *** Warning! ***';
	  antennapos := array(0,shap[1],3);
	  for (i in [1:shap[1]]) { antennapos[i,1] :=i; };
	};
	if (beamwidth == 0) {
	  beamwidth := 10;
	};
	maxze := as_integer(90./beamwidth);
	minze := as_integer(startze/beamwidth);
	numpos:=0;
	for (ize in [minze:maxze]) {
	  maxaz := as_integer(360/beamwidth*sin(ize*beamwidth*pi/180));
	  numpos:= numpos+maxaz+1;
	};
	erg := array(0.,numpos,3);
	global starray := array(0.,numpos,shape(dataarr)[2]);
	i:=0;
	for (ize in [minze:maxze]) {
	  maxaz := as_integer(360/beamwidth*sin(ize*beamwidth*pi/180));
	  for (iaz in [0:maxaz]) {
	    i:=i+1;
	    ze := (ize*beamwidth);
	    az := (360*iaz/(maxaz+1));
	    delays := angle2delay(antennapos, ze, az);
	    delays := as_integer(delays*genfiltdata.samplefreq);
	    ary := stoverlay(dataarr, delays);
	    if ( len(region)>2 ) {
	       power := sum((ary[region])^2);
	    } else {
	       power := sum((ary[region[1]:region[2]])^2);
	    };
	    starray[i,] := ary;
	    erg[i,] := [ze,az,power];    
	  };
	};
	return erg;

};


#-----------------------------------------------------------------------
#Name: stoverlay
#-----------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  dataarr  - 
#Par:  delays   - 
#-----------------------------------------------------------------------

stoverlay := function (dataarr,
             delays)
{
  shap := shape(dataarr);
  if (shap[1] != len(delays) ) {
    print 'shape(dataarr)[1] != len(delays)';
    return F;
  };
	delays := as_integer(delays);
	lower := max(1, (-min(delays)+1) );
	upper := min(shap[2], (shap[2]-max(delays)) );
	tmpary := array(0.,shap[2]);
	for (i in [1:shap[1]]) {
	  tmpary[lower:upper] := tmpary[lower:upper] +
	    dataarr[i,(lower+delays[i]):(upper+delays[i])]
	};
	tmpary := tmpary/shap[1];
	return tmpary;
};


#-----------------------------------------------------------------------
#Name: genall
#-----------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  rec        - 
#Par:  from       - 
#Par:  pulse      - 
#Par:  ze         - 
#Par:  az         - 
#Par:  weight     - 
#Par:  beamwidth  - 
#Par:  rze        - 
#Par:  raz        - 
#Ref:  angle2delay, AntennaOverlay, skytrial
#-----------------------------------------------------------------------

genall := function (ref rec,
                    from,
                    pulse,
                    ze=20,
                    az=0,
                    weight=-2,
                    beamwidth=10,
                    rze=0,
                    raz=0)
{
  tmpdata := ref genfiltdata;
  delays := as_integer(angle2delay(genfiltdata.antpos, ze, az)*genfiltdata.samplefreq);
	rdelays := as_integer(angle2delay(genfiltdata.antpos, rze, raz)*genfiltdata.samplefreq);
	print 'delays: ',delays , '  RFI-delays: ',rdelays;
	tmp := genant(rec,from,pulse,toind=genfiltdata.toind,delay=delays,rfidelay=rdelays)
	tmpdata.toind  := tmp.toind;
	if (genfiltdata.tmpind == -1) {
	  tmpdata.tmpind := RecoverEntry(rec,1);
	};
	if (weight == -2) {
	  weight := T;
	  filter := 10;
	  while (sum(filter) > 1) {
	    filter := peakary2(rec.get('AbsFFT',from[1])*weight,3,0);
	    weight := ((!filter) & weight);
	  };
	};
	temp := AntennaOverlay(rec,genfiltdata.toind,genfiltdata.tmpind,
				weight=weight,delay=tmp.delay,returnall=T);
	shap := shape(temp);
	region := [(as_integer(shap[2]/2)-5),(as_integer(shap[2]/2)+len(pulse)+5)];
	erg := skytrial(temp,region,antennapos=tmpdata.antpos,beamwidth=beamwidth);
	return erg;
};



