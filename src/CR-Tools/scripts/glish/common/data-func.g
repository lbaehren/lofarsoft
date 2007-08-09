########################################################################
#
# (un)sorted functions for manipulating the data
#
########################################################################

global_fftserver := fftserver();


#-----------------------------------------------------------------------
#Name: fieldexists
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  --/--
#Par:  rec   - Reference to the data structure
#Par:  field - 
#Par:  index -
#-----------------------------------------------------------------------

fieldexists := function (ref rec, field, index) {
  f:=field_names(rec.data[index]); 
  return(len(f[f==field])>0)
}

#-----------------------------------------------------------------------
#Name: time2block
#-----------------------------------------------------------------------
#Type; GLish function
#Doc:  Convert time to datablock number
#Par:  rec     - Reference to the data structure
#Par:  t       - Time, in [s]
#Par:  bs = -1 - Blocksize; if unset, the current blocksize from the 
#                data record is used.
#Ret:  float
#Created
#-----------------------------------------------------------------------

time2block := function (ref rec, t=0.0, bs=-1)
{
  if (bs<0) bs := rec.getmeta('Blocksize');
  
  samplefreq := rec.headf('Samplerate')*rec.headf('SamplerateUnit');

  return (t*samplefreq)/bs;
}

#-----------------------------------------------------------------------
#Name: padLoopIndex
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Left sided padding of a loop counter to the number of characters
#      in the maximum loop number.
#Par:  num          - Number of the current loop iteration
#Par:  nof          - Total number of loop iterations
#Par:  fill  = '0'  - Padding character.
#Ret:  index        - 'num' padded up to the length of 'nof'
#Created: 2004/12/12 (Lars Baehren)
#-----------------------------------------------------------------------

padLoopIndex := function (num, nof, fill='0') {

  charsTotal := as_integer(log(nof));
  
  if (num > 0) charsCurrent := as_integer(log(num));
  else charsCurrent := 0;

  diff := charsTotal - charsCurrent;

  index := '';
  if (diff > 0) { 
    for (i in 1:diff) index := spaste(as_string(index),as_string(fill));
  }
  index := spaste(as_string(index),as_string(num));

  return index;
}

#-----------------------------------------------------------------------
#Name: sqrt_signed
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns the square root of the absolute value of the argument times the sign of the argument:  sqrt_signed(x) = sqrt(abs(x))*sign(x)
#Ret: float
#Par: x =              - argument
#Example:
#- sqrt_signed(-4) = -2
#-----------------------------------------------------------------------
sqrt_signed:=function(x){
return sqrt(abs(x))*sign(x)
}

#-----------------------------------------------------------------------
#Name: sqr_signed
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns the square of the argument times the sign of the argument:  sqr_signed(x) = x^2*sign(x)
#Ret: float
#Par: x =              - argument
#Example:
#- sqr_signed(-2) = -4
#-----------------------------------------------------------------------
sqr_signed:=function(x){
return x^2*sign(x)
}

#-----------------------------------------------------------------------
#Name: exclude
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns a vector excluding elements that are present in another vector
#Ret: vector
#Par: exclude =              - which elements to exclude
#Par: vector  =              - the vector to return and exclude elements from
#Example:
#- exclude([1,4],[1,2,4,19.5])                                  
#[2 19.5]  
#-----------------------------------------------------------------------
exclude:=function(exclude,vector){
bool:=array(T,len(vector));
for (i in ind(exclude)) {bool:=bool & (vector!=exclude[i])};
return vector[bool]
}

#-----------------------------------------------------------------------
#Name: intersection
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns a vector with all elements that are present in both vectors
#Ret: vector
#Par: vector1 - first list 
#Par: vector2 - second list
#Example:
#- intersection([1,3,4],[1,2,4,19.5])                                  
#[1 4]  
#-----------------------------------------------------------------------
intersection:=function(vector1,vector2){
bool:=array(T,len(vector1));
for (i in ind(vector1)) {bool[i]:=(is_member(vector1[i],vector2)>0)}
return vector1[bool]
}

#-----------------------------------------------------------------------
#Name: is_member
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns whether an element is a member of a vector. Returns the number
#     of occurances or Zero if none.
#Ret: Integer
#Par: element =              - which element to look for
#Par: vector  =              - the vector to search
#
#-----------------------------------------------------------------------
is_member:=function(element,vector){
return len(vector[vector==element])
}

#-----------------------------------------------------------------------
#Name: is_where
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns the positions of where an element occurs in a vector.
#Ret: Integer
#Par: element =              - which element to look for
#Par: vector  =              - the vector to search
#
#-----------------------------------------------------------------------
is_where:=function(element,vector){
bool:=vector==element;
return ind(vector)[bool]
}


#-----------------------------------------------------------------------
#Name: as_inputvec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns a string, containing the glish input format of the vector.
#The input vector can also be a record if is structured like a vector.
#Ret: String
#Par: vector =              - Input vector
#Example:
# - as_inputvec([1,2,3,4])  <br>
# [1,2,3,4]  <br>
# - a:=[=] <br>
# - a[1]:=2  <br>
# - as_inputrec(a)  <br>                                 
# [2]  <br>
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
as_inputvec:=function(vector){
if (is_record(vector)) {
  ret:='[';
  if (len(vector)==0) return '[ ]';
  if (len(vector)==1) return spaste('[',as_string(vector[1]),']');
  ret:=spaste(ret,sprintf('%s',as_string(vector[1])));
  for (i in seq(2,len(vector))) {ret:=spaste(ret,sprintf(',%s',as_string(vector[i])))}
  return spaste(ret,']');
  } else {
  return spaste('[',paste(as_string(vector),sep=','),']')
  }
}

#-----------------------------------------------------------------------
#Name: as_time
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns a time value of a measure to a time format string
#Ret: String
#Par: inp = ''             - Input (measure or image)
#Par: form="ymd time"      - Outputr Time format according to measures.g standards.
#
#-----------------------------------------------------------------------
as_time:=function(inp,form="ymd time"){
  if (is_measure(inp)) return dq.time(dm.getvalue(inp)[1], form="ymd time");
  if (is_image(inp)) return dq.time(dm.getvalue(inp.coordsys().epoch())[1], form="ymd time") ;
  if (has_field(inp,"head") && has_field(inp,"coordinfos")) return inp.head('Date');
  if (is_string(inp)) return inp;
return ""
}


#-----------------------------------------------------------------------
#Name: last_char
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns the last character in a string.
#Ret: String
#Par: string = ''             - Input string
#-----------------------------------------------------------------------
last_char:=function(string){return split(string,'')[strlen(string)]}

#-----------------------------------------------------------------------
#Name: CopyDataRec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Copy an entire record of a data set
#Ret: T
#
#Par: rec	= 			- Set, where data is stored
#Par: from	= 1			- Index of record to copy
#Par: torec	= 0			- Index of the copy. 0 means
#					  append.
#Ref: PutDataToMem, GetDataFromMem,  readfilesNOEMI
#-----------------------------------------------------------------------
CopyDataRec:=function(ref rec,from=1,torec=0) {
  if ((from<1) || (from>rec.len)) {return F}
  if (torec<1) {dest:=rec.len+1} else {dest:=torec}
  for (f in field_names(rec.data[from])) {
	       rec.put(rec.get(f,from),f,dest)
}
  return T
}

#-----------------------------------------------------------------------
#Name: FillDataRec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Fill fields in a record with one value. Mainly used to
#initialize data record with a defined value.
#
#Ret: T
#
#Par: rec	= 			- Set, where data is stored
#Par: fields	= ''			- Which fields to fill
#Par: index	= 1			- Index of record to fill. O=
#					  last record.
#Par: fill	= 0			- Index of the copy. 0 means
#					  fill last record.
#Ref: PutDataToMem, GetDataFromMem, readfilesNOEMI
#-----------------------------------------------------------------------
FillDataRec:=function(ref rec,fields='',index=0,fill=0) {
  if (index>rec.len) return F
  if (index<1) {idx:=rec.len} else {idx:=index}
  for (f in fields) {
      l:=len(rec.get(f,idx))
      rec.put(array(fill,l),f,idx)}
  return T
}

#-----------------------------------------------------------------------
#Name: SumPowers
#-----------------------------------------------------------------------
#Type: Function
#Doc: Reads time series data and sums the powers 
#Ret: Array with time series data of powers
#Par: infiles	= ''			- List of filenames
#Par: blocksize	= -1			- Length of blocks to read
#                                         blocksize<=0 -> read all
#Par: offset    = 0			- Starting positions in file
#-----------------------------------------------------------------------

SumPowers:=function(infiles,blocksize=-1,offset=0){
  sum:=array(0,blocksize);
  for (file in infiles) {
      sum+:=readdat(file,blocksize=blocksize,offset=offset)^2;
      }
  if (len(infiles)>0) 
     sum:=sum/len(infiles)
  else 
     sum:=F
  return sum
};

#-----------------------------------------------------------------------
#Name: fftvec
#-----------------------------------------------------------------------
#Type: Function
#Doc: Create the FFT for one vector and just return on of the redundant
#     halfs.
#Ret: T/F
#
#Par: ary =	  			- vector to fft
#Par: invdir = T			- invert the FFT (right-to-left)
#Par: forward = F			- compute forward FFT, the forward FFT
#                             is the conjugate complex of the backward
#                             FFT divided by the length of the input
#                             vector: FFTb=conj(FFTf)*len(ary)
#Ref: 
#-----------------------------------------------------------------------

fftvec:=function(ary,invdir=T,forward=T) {
    # print 'Calculating FFT!'		      
    if (forward) global_fftserver.complexfft(ary,1)
      else global_fftserver.complexfft(ary,-1)
    if (!invdir) return conj(ary[(as_integer(len(ary)/2)+1):1]);
    else return ary[1:(as_integer(len(ary)/2)+1)];
    #else return ary;
}


#-----------------------------------------------------------------------
#Name: ComputeMeanPower
#-----------------------------------------------------------------------
#Type: Function
#Doc: computes the mean power by averaging over all blocks
#Ret: none
#
#Par: rec =	  			- Data record
#-----------------------------------------------------------------------

ComputeMeanPower := function(ref rec) {
    nsum:=0; 
    if (is_agent(rec.newsagent)) {rec.newsagent->message('Computing Mean-FFTs');};
    for (i in [1:rec.len][rec.select()]) {
        if (nsum==0) {
           type:=rec.head("DataType",i); 
	   ID:=rec.head("ID",i); 
	 }	   
#Apply only if type and ID match
         if (rec.head("DataType",i)==type && rec.head("ID",i)==ID) {
	   if (is_agent(rec.newsagent)) { rec.newsagent->message(paste(
			'Computing Mean-FFT for index',as_string(i)));};
	   size := rec.getmeta('Blocksize',i);
	   numblocks := as_integer(rec.getmeta('FileBlocksize',i)/size);
	   #OldOffset := rec.getmeta('Offset',i);
	   fftsum := array(0,as_integer(size/2+1));
	   #windowary := 1-abs( ([1:size]-size/2)/(size/2) ); #Bartlett window
	   windowary := 1-( ([1:size]-size/2)/(size/2) )^2;   #Welch window
	   #windowary := array(1.,size);                      # = No window
	   normfactor := size*sum(windowary^2);
	   oldregion := rec.getregion('T',i);
	   for (j in [1:numblocks]) {
		#rec.putmeta((size*(j-1)),'Offset',i);
		rec.setregion([(size*(j-1)+1),(size*j)],'T',i);
		yary:=rec.get('Voltage',i);
		yary := yary * windowary;
		global_fftserver.complexfft(yary,1); 
		#fftsum := fftsum + abs(yary)[1:as_integer(size/2+1)];
		fftsum := fftsum + abs(yary[1:as_integer(size/2+1)])^2;
if (((j%1000) == 0) & is_agent(rec.newsagent)) {rec.newsagent->message(paste('Done ',as_string(j),' of ',as_string(numblocks),' blocks of index ',as_string(i)));};
	   }
	   rec.setregion(oldregion,'T',i);
	   fftsum := fftsum/numblocks;
	   rec.put( (fftsum*2/normfactor/50) ,'MeanPower',i);
	   #rec.put( (((fftsum/size*2)^2)/50) ,'MeanPower',i);
	   #rec.putmeta(OldOffset,'Offset',i);
	 }
    }	
    if (is_agent(rec.newsagent)) {rec.newsagent->message('Mean-FFTs computed');};
}

#-----------------------------------------------------------------------
#Name: getcalvals
#-----------------------------------------------------------------------
#Type: Function
#Doc: computes the calibration values for a set of frequencies; 
#     linear interpolation between calibration points.
#Ret: array of float with calibration values
#
#Par: freq =	  	- array with frequencies (the returned array will
#			  have the same shape as this array)
#Par: calarr =	  	- 2 dim array with frequencies and the corresponding 
#			  calibration values 
#			    calarr[1,i] -> Frequency (have to be sorted)
#			    calarr[2,i] -> corresponding gain value
#-----------------------------------------------------------------------

getcalvals := function(freq, calarr) {
	shap := shape(calarr);
	erg := array(0,len(freq)); 
	donemask := array(F,len(freq)); 
	for (i in [1:(shap[2]-1)]) {  #step over all intervalls of the calarr
	  lf  := calarr[1,i];     #low frequency of intervall
	  hf := calarr[1,i+1];    #high frequency of intervall
	  lg := calarr[2,i];      #gain at low frequency
	  hg := calarr[2,i+1];    #gain at high frequency
	  filt := ((freq >= lf) & (freq < hf)); #mask for frequencies in itnervall
	  donemask := filt | donemask;  #remember which frequencies we already did
	  erg := erg+ (lg+ (hg-lg)*(freq-lf)/(hf-lf))*filt; #linear interpolation 
	};
	erg := erg + min(calarr[2,])*(!donemask) #fill all empty frequencies with the minimum gain
	return erg;
};	

#-----------------------------------------------------------------------
#Name: getCalField
#-----------------------------------------------------------------------
#Type: Function
#Doc: retrieve and 1d-interpolate calibration values from the CalTable
#Ret: array of float with calibration values
#
#Par: obs =	  	            - name of the observatory 
#Par: obs =	 gainField 	    - name of the field that is to be interpolated
#Par: obs =	 freqField 	    - name of the field acording to which th interpolation is done
#Par: obs =	 freqArr	    - array acording to which th interpolation is done
#Par: obs =	 date   	    - date for which the data is to be retrieved
#Par: obs =	 AntID  	    - ID of the antenna for which the data is to be retrieved
#-----------------------------------------------------------------------
getCalField := function(obs, gainField, freqField, freqArr, date, AntID){
  retval := array(1.,len(freqArr));
  if (is_agent(globalpar.CalTable_client[obs])) {
    getrec := [=];
    getrec.date := date;
    getrec.ant := AntID;
    getrec.field := gainField;
    gainvals := globalpar.CalTable_client[obs]->getData(getrec);
    getrec.field := freqField;
    freqvals := globalpar.CalTable_client[obs]->getData(getrec);
    if ( !(len(gainvals)==0) && !(len(freqvals)==0) && (len(freqvals)==len(gainvals)) ) {
      if (is_dcomplex(gainvals)) {
        retval := ComplexInterp1d(freqvals,gainvals,freqArr);
      } else {
        inter := interpolate1d();
        inter.initialize(freqvals,gainvals,'linear');
        retval := inter.interpolate(freqArr);
      }
    };
  };
  return retval
}


#-----------------------------------------------------------------------
#Name: getgains
#-----------------------------------------------------------------------
#Type: Function
#Doc: computes the antenna gain for the given direction and frequencies
#Ret: array of float with gain values
#
#Par: az =	  	- azimut in degrees (scalar!)
#Par: el =	  	- elevation in degrees (scalar!)
#Par: freq =	  	- array with frequencies (the returned array will
#			  have the same shape as this array)
#Par: calrec =	  	- record with the calibration values
#-----------------------------------------------------------------------

getgains := function(az,el,freq,calrec) {
  shap := shape(calrec.dir);
  azpos := az/calrec.azinp[1]+calrec.azinp[2];
  elpos := el/calrec.elinp[1]+calrec.elinp[2];
  if (azpos<1||elpos<1||azpos>shap[2]||elpos>shap[3]){
    #print 'getgains: position does not fit into direction array.';
    return F;
  }
  freqpos := freq/calrec.freqinp[1]+calrec.freqinp[2];
  if (min(freqpos)<1 || max(freqpos)>=shap[1]) {
    #print 'getgains: frequency range to large',min(freq),min(freqpos),max(freq),max(freqpos),shap[1];
    return F;
  }
#print azpos,elpos,freqpos;
  w[1] := sqrt((azpos-floor(azpos))^2+(elpos-floor(elpos))^2);
  w[2] := sqrt((azpos-floor(azpos))^2+(elpos-ceil(elpos))^2);
  w[3] := sqrt((azpos-ceil(azpos))^2+(elpos-floor(elpos))^2);
  w[4] := sqrt((azpos-ceil(azpos))^2+(elpos-ceil(elpos))^2);
#print w
  if (min(w)>0.0001) {
    w := 1/w;
    w := w/sum(w);
    freqvals :=  calrec.dir[,floor(azpos),floor(elpos)]*w[1]+
                 calrec.dir[,floor(azpos),ceil(elpos)]*w[2]+
                 calrec.dir[,ceil(azpos),floor(elpos)]*w[3]+
                 calrec.dir[,ceil(azpos),ceil(elpos)]*w[4];
  } else {
    if (w[1] == min(w)) {
      freqvals :=  calrec.dir[,floor(azpos),floor(elpos)];
    } else if (w[2] == min(w)) {
      freqvals :=  calrec.dir[,floor(azpos),ceil(elpos)];
     } else if (w[3] == min(w)) {
      freqvals :=  calrec.dir[,ceil(azpos),floor(elpos)];
    } else {
      freqvals :=  calrec.dir[,ceil(azpos),ceil(elpos)];
    };
  };
#print len(freqvals),freqvals
  erg := freqvals[floor(freqpos)]+((freqvals[floor(freqpos)+1]-freqvals[floor(freqpos)])*(freqpos-floor(freqpos)))
 return erg;
};


#-----------------------------------------------------------------------
#Name: FFTtoFx
#-----------------------------------------------------------------------
#Type: Function
#Doc: computes a new time-series from fft-data 
#Ret: array with the new time-series
#
#Par: from =	  		- array with the original FFT-data (complex!)
#Par: fftsize =	  		- size of the dataset, that was fft'ed
#Par: weight =			- array with a weight for the FFT-data
#-----------------------------------------------------------------------

FFTtoFx := function( from, fftsize, weight=-1,invdir=T) {
	l := len(from);
    if (!invdir) {from := conj(from[len(from):1])};
	if (fftsize < l) { 
	  print 'FFTtoFx: inconsistent data: fftsize < len(from)';
	  return F;
	};
	if (fftsize/2 > l) { 
	  print 'FFTtoFx: inconsistent data: fftsize/2 > len(from)',(fftsize/2),l;
	  return F;
	};
	if ( len(weight) == l) { from := from*weight }; #apply the weight
	ary := array(0+0i,fftsize);
    ary[1:l] := from
	halfsize := as_integer(fftsize/2);
	if ((fftsize%2) == 1) {
	  ary[(fftsize-halfsize+1):(fftsize)] := conj(from[(halfsize):1]);
	} else { 
	  ary[(fftsize-halfsize+1):(fftsize)] := conj(from[(halfsize+1):2]);
	};
	global_fftserver.complexfft(ary,-1) 
	return ary;
};

#-----------------------------------------------------------------------
#Name: RecoverEntry
#-----------------------------------------------------------------------
#Type: Function
#Doc: generates a new data-entry from the reverse-fft'ed of an existing.
#     [the new data-entry will have the following as time-series input:
#        new_time_ser = invFFT( weight* FFT( old_time_ser*timeweight)) 
#     if weight and timeweight are arrays of (0|1) then weight can be used to
#     remove peaks in the spectrum and timeweight to can be used to remove 
#     peaks in time-series.]
#Ret: index of the new data-entry
#
#Par: rec =	  		- Data record to identify dataset
#Par: froment =	  		- index of source data-entry
#Par: toent =	  		- index of destination data-entry 
#				  (<0 -> create new entry)
#Par: weight =			- array with a weight for the FFT-data
#Par: timeweight =		- array with a weight for the time-series data
#-----------------------------------------------------------------------

RecoverEntry := function( ref rec, froment, toent=-1, weight=-1,timeweight=1) {
	fromary := rec.get('Voltage',froment);
	if (len(timeweight) == len(fromary)) {
      invdir:=rec.headf('NyquistZone')%2==0;
	  fromary := fftvec(fromary*timeweight,invdir=invdir);
	} else {
	  fromary := rec.get('FFT',froment);
	};
	size := rec.getmeta('FFTSize',froment);
	ary := FFTtoFx(fromary, size, weight);
	if (ary == F) {
		print 'RecoverEntry: reverse-fft failed. Aborting!';
		return F;
	};
	peak := max(abs(ary));
	maxwink := asin(max(abs(imag(ary))/(abs(ary)+(peak/1e5))))/pi*180 
	if (maxwink > 10) print 'Attention! Large complex angle:',maxwink,'deg'
	if (toent<1 || toent > rec.len) {
	   head := rec.get('Header',froment);
#	   head.DataType := spaste(head.DataType,'-rec');
	   head.SourceIndex := froment;
	   head.SourceVersion := rec.version('FFT',froment);
	   toent := rec.newentry(head);
	}; 
	ary := NOEMIconvFX2Voltage(ary,rec,toent,inv=T);
	for (ff in rec.get('Input',toent)) rec.put(real(ary),ff,toent);
	for (ff in rec.coord(toent)) {rec.touch(ff,toent,version=0)}
	blocksize := min(globalpar.BlocksizeForDisplay,len(ary));
	rec.setregion([1,blocksize],'T',toent)
        rec.putmeta(0,'FileOffset',toent);
        rec.putmeta(len(ary),'FileBlocksize',toent);
	rec.putmeta(0,'Offset',toent);
	rec.putmeta(blocksize,'Blocksize',toent);
	if (is_agent(rec.newsagent)) {rec.newsagent->checkdata();};
	return toent;
};

#-----------------------------------------------------------------------
#Name: zeaz2kart
#-----------------------------------------------------------------------
#Type: Function
#Doc: convert zenit-azimut angle to kartesic position
#Ret: 
#
#Par: 
#-----------------------------------------------------------------------

zeaz2kart := function(ze,az){
	r := ze/90;
	erg.x := r*sin(az*pi/180);
	erg.y := r*cos(az*pi/180);
	return erg;
};

zeaz2kartarr := function(inarr){
	shap := shape(inarr);
	erg := array(0,shap[1],shap[2])
	erg[,1] := inarr[,1]/90*sin(inarr[,2]*pi/180);
	erg[,2] := inarr[,1]/90*cos(inarr[,2]*pi/180);
	return erg;
};

#-----------------------------------------------------------------------
#Name: roundn
#-----------------------------------------------------------------------
#Type: Function
#Doc: Rounds a number to n digits after the decimal
#Ret: float
#
#Par: num 	  		- A number in any form (float, int, string)
#Par: n=2			- Number of digits after the decimal
#-----------------------------------------------------------------------

roundn:=function(num,n=2) {
  return round(as_float(num*10^n))/10^n
}

#-----------------------------------------------------------------------
#Name: runningavg
#-----------------------------------------------------------------------
#Type: Function
#Doc:  calculate the running average over a set of data
#Ret:  nothing
#
#Par:  ary         data to be averaged
#      mean        array to store running average
#      bs          block size over which to average
#-----------------------------------------------------------------------
runningavg := function(ary,ref mean, bs) {
  mary := array(0,len(ary)+bs-1);

  if (bs < len(ary) && bs > 1) {
    for (i in 1:bs) {
      mary[i] := sum(ary[[1:i]])*as_double(bs)/as_double(i);
    }
    for (i in (bs+1):(len(ary))) {
      mary[i] := mary[i-1]+ary[i]-ary[i-bs];
    }
    for (i in (len(ary)+1):(len(mary))) {
      mary[i] := sum(ary[[(i-bs+1):len(ary)]])*as_double(bs)/as_double(len(mary)-i+1);
    }
    val mean := mary[[(1+bs/2):(len(ary)+bs/2)]]/as_double(bs);
  } else if (bs >= len(ary)) {
    val mean := sum(ary)/as_double(len(ary));
  } else if (bs == 1) {
    val mean := ary;
  } else {
    print 'Failed to calculate running average: blocksize =', bs, 'out of range';
  }
}

#-----------------------------------------------------------------------
#Name: runningavgstdnew
#-----------------------------------------------------------------------
#Type: Function
#Doc:  calculate the running average and the standard deviation over a set of data
#Ret:  nothing
#
#Par:  ary       - data to be averaged
#      mean      - array to store running average
#      var       - array to store running standard deviation
#      bs        - block size over which to average
#Example: runningavgstdnew([1,2,3,4,5],mea,std,3)
#-----------------------------------------------------------------------
runningavgstdnew := function(ary,ref mean, ref var, bs) {
  mary := array(0,len(ary)+bs-1);
  vary := array(0,len(ary)+bs-1);

  if (bs < len(ary) && bs > 1) {
    for (i in 1:bs) {
      mary[i]  := sum(ary[1:i])/as_double(i);
      vary[i]  := sum((ary[1:i]-mary[i])*(ary[1:i]-mary[i]))/as_double(i);
    }
    for (i in (bs+1):(len(ary))) {
      mary[i]  := mary[i-1]+(ary[i]-ary[i-bs])/as_double(bs);
      ##vary[i]  := vary[i-1]+((ary[i]-mary[i])*(ary[i]-mary[i])-(ary[i-bs]-mary[i-bs])*(ary[i-bs]-mary[i-bs]))/as_double(bs);
      vary[i]:=vary[i-1]+mary[i-1]^2-mary[i]^2+(ary[i]^2-ary[i-bs]^2)/bs
    }
    for (i in (len(ary)+1):(len(mary))) {
      mary[i]  := sum(ary[(i-bs+1):len(ary)])/as_double(len(mary)-i+1);
      ##vary[i]  := sum((ary[(i-bs+1):len(ary)]-mary[i])*(ary[(i-bs+1):len(ary)]-mary[i]))/as_double(len(mary)-i+1);
      for (j in (i-bs+1):len(ary)) {vary[i]+:=(ary[j]-mary[i])^2}; vary[i]/:=(len(mary)-i+1)
    }
    val mean  := mary[(1+bs/2):(len(ary)+bs/2)];
    val var   := sqrt(vary[(1+bs/2):(len(ary)+bs/2)]);
  } else if (bs >= len(ary) || bs < 1) {
    val mean  := array(sum(ary)/as_double(len(ary)),len(ary));
    val var   := array(sqrt(sum((ary-mean)*(ary-mean))/len(ary)),len(ary));
  } else if (bs == 1) {
    val mean  := ary;
    val var   := 0;
  }
#print mean[1:100];
#print vary[1:100];
}

#-----------------------------------------------------------------------
#Name: runningavgstd
#-----------------------------------------------------------------------
#Type: Function
#Doc:  calculate the running average and the standard deviation over a set of data
#Ret:  nothing
#
#Par:  ary         data to be averaged
#      mean        array to store running average
#      var         array to store running standard deviation
#      bs          block size over which to average
#Example: runningavgstd([1,2,3,4,5],mea,std,3)
#-----------------------------------------------------------------------
runningavgstd := function(ary,ref mean, ref std, bs) {
	inp := [=];
  ret := [=];

	inp.ary := ary;
	inp.strength := bs;

	ret := global_lopestools_client->avgstdfilter(inp);

	val mean := ret.mary;
	val std  := ret.sary;
}

#-----------------------------------------------------------------------
#Name: cMeanFilter
#-----------------------------------------------------------------------
#Type: Function
#Doc: applies a mean-filter to an array of data
#Ret: array with filtered data
#
#Par: ary =	  		- array with data to filter
#Par: strength =	  	- strength of the filter
#-----------------------------------------------------------------------

cMeanFilter := function(ary, strength=3){
	filter := [=];
	
	filter.ary := ary;
	filter.strength := strength;

	return global_lopestools_client->meanfilter(filter);
};
cMeanFilter2 := function(ary, strength=3){
	filter := [=];
	
	filter.ary := ary;
	filter.strength := strength;

	return global_lopestools_client->meanfilter2(filter);
};
#-----------------------------------------------------------------------
#Name: cVarFilter
#-----------------------------------------------------------------------
#Type: Function
#Doc: applies a "Variance-filter" to an array of data (i.e. compute a running Variance)
#Ret: array with filtered data
#
#Par: ary =	  		- array with data to filter
#Par: strength =	  	- strength of the filter
#-----------------------------------------------------------------------

cVarFilter := function(ary, strength=3){
	filter := [=];
	
	filter.ary := ary;
	filter.strength := strength;
	
	zwisch := global_lopestools_client->meanfilter(filter)^2;
	
	filter.ary := ary^2;
	
	return sqrt(global_lopestools_client->meanfilter(filter) - zwisch);
};
#-----------------------------------------------------------------------
#Name: cMedFilter
#-----------------------------------------------------------------------
#Type: Function
#Doc: applies a median filter to an array of data
#Ret: array with filtered data
#
#Par: ary =	  		- array with data to filter
#Par: strength =	  	- strength of the filter
#-----------------------------------------------------------------------

cMedFilter := function(ary, strength=3){
	filter := [=];
	
	filter.ary := ary;
	filter.strength := strength;

	return global_lopestools_client->medfilter(filter);
};

#-----------------------------------------------------------------------
#Name: cInterpolate2d
#-----------------------------------------------------------------------
#Type: Function
#Doc: interpolate 2-dim data 
#Ret: 2-dim quadratic array with interpolated data
#
#Par: source =	  		- 2-dim array [[x,y,value],n] of source data
#Par: shape  =		  	- description of return array 
#                                 [(x/y)min, (x/y)max, no_points]
#-----------------------------------------------------------------------

cInterpolate2d := function(source,shape){
	
	interp.source := source;
	interp.shape :=	shape;

	return global_lopestools_client->interpolate2d(interp);
};

#-----------------------------------------------------------------------
#Name: ComplexInterp1d
#-----------------------------------------------------------------------
#Type: Function
#Doc: interpolate complex 1-dim data 
#Ret: interploated complex data
#
#Par: x =	  		    - independent, real data variable
#Par: y  =		  	    - dependent, complex data variable
#Par: points  =		  	- perform the interpolation for these positions
#Par: style  ='linear' 	- interpolation method
#-----------------------------------------------------------------------

ComplexInterp1d := function(x,y,points,style='linear'){
  rinter := interpolate1d();
  rinter.initialize(x,real(y),style);
  realval := rinter.interpolate(points);
  iinter := interpolate1d();
  iinter.initialize(x,imag(y),style);
  imagval := iinter.interpolate(points);
  return complex(realval,imagval)
};


#-----------------------------------------------------------------------
#Name: identical_pairs
#-----------------------------------------------------------------------
#Type: Function
#Doc: find the number of times, that suceeding value pairs are identical,
#     as is e.g. generated by bad sync between RML and TIm board
#Ret: relative amount of identical pairs
#
#Par: ary =	  		- array of  data
#-----------------------------------------------------------------------
identical_pairs := function(ary) {
  erg := 0;
  ind := 1;
  while (ind < len(ary)-3) {
    if ((ary[ind] == ary[ind+2])&(ary[ind+1] == ary[ind+3])) {
      erg := erg+1;
      ind := ind+1;
    };
    ind := ind+1;
  };
  return erg/len(ary);
}

#-----------------------------------------------------------------------
#Name: maxpos
#-----------------------------------------------------------------------
#Type: Function
#Doc: find the position of the maximum in an 1-d array
#Ret: position of the maximum
#
#Par: ary =	  		- array of  data
#-----------------------------------------------------------------------
maxpos := function(ary) {return order(ary)[len(ary)]; };




ComputeMeanPower2 := function(ref rec) {
    nsum:=0; 
    if (is_agent(rec.newsagent)) {rec.newsagent->message('Computing Mean-FFTs');};
    for (i in [1:rec.len][rec.select()]) {
        if (nsum==0) {
           type:=rec.head("DataType",i); 
	   ID:=rec.head("ID",i); 
	 }	   
#Apply only if type and ID match
         if (rec.head("DataType",i)==type && rec.head("ID",i)==ID) {
	   if (is_agent(rec.newsagent)) { rec.newsagent->message(paste(
			'Computing Mean-FFT for index',as_string(i)));};
	   size := rec.getmeta('Blocksize',i);
	   eichfaktor := ((rec.headf('MaxVolt')/ rec.headf('ADCMaxChann'))^2)*2/50;
	   pixelrec.nch := 1;
	   pixelrec.bsize := size;
	   pixelrec.nfreq := as_integer((size+2)/2);
	   pixelrec.files := rec.head('Files',i);
	   pixelrec.delays := 0;
           fftsum := global_lopestools_client->calc_pixel(pixelrec)[pixelrec.nfreq:1];

	   rec.put( (fftsum*eichfaktor) ,'MeanPower',i);
	   #rec.put( (((fftsum/size*2)^2)/50) ,'MeanPower',i);
	   #rec.putmeta(OldOffset,'Offset',i);
	 }
    }	
    if (is_agent(rec.newsagent)) {rec.newsagent->message('Mean-FFTs computed');};
}


#-----------------------------------------------------------------------
#Name: filterlines
#-----------------------------------------------------------------------
#Type: Function
#Doc: Iteratively cuts out lines that are n*sigma above the mean spectrum
#     and replaces them with the running mean. 
#Ret: Array with lines cut-out and replaced by the mean
#
#Par: ary =         - array of input  data
#Par: nsigma =      - array with nsigmas, iteratively flag data which 
#                     are nsigma above the mean. The length of the array
#                     determine the number of iterations
#Par: smooth =      - form the running mean by smoothing the data with
#                     a window of width n=smooth samples
#Par: smooth2 =     - smooth the array that is to be cut also before determining
#                     deviating samples (usefull for broader lines). Return
#                     array will not be affected /smoothed.
#Par: nx1,nx2 =     - determine the noise (for sigma) in the inner region
#                     between
#                   - sample nx1,nx2. If negative take -nx1/2% off the left and
#                     right boundary
#Par: cutpos =      - Cuts on positive deviations 
#Par: cutneg =      - Cuts (also) neagtive deviations 
#Par: retratio=     - Returns the ratio between uncut and cut array rather then
#                     the cut array itself (usefull for flagging operations).
#-----------------------------------------------------------------------

filterlines:=function(ary,
                      nsigma=[1,2],
                      smooth=500,
                      smooth2=3,
                      nx1=-10,
                      nx2=-10,
                      cutneg=T,
                      cutpos=T,
                      retratio=F)
{
  aryy:= ary; 
  l:=len(aryy); 
  n:=0

# Set end points to zero (e.g., take out DC offset)
  aryy[1]:=0;
  aryy[l]:=0;
 
  if (nx1<0) {n1:=l/abs(nx1)} else {n1:=nx1}
  if (nx2<0) {n2:=l-l/abs(nx1)} else {n2:=nx2}

  for (sigma in nsigma) {
    n+:=1;
    avgy:=cMeanFilter(aryy,smooth)
    if (smooth2>1) {
      avgy2:=cMeanFilter(aryy,smooth2)
    } else {
     avgy2:=aryy
    };
    devy:=avgy2/avgy
    cuty:=aryy
    cut:=sigma*stddev(devy[n1:n2]);
    print 'Pass #',n,'- Cutting Threshold:',roundn(1+cut),'* mean power.'
    if (cutpos){
      sel := devy>1+cut;
#      sel[1:n1] := F;
#      sel[n2:l] := F;
      cuty[sel] := avgy[sel];
    };
    if (cutneg){
      sel := devy<1-cut;
#      sel[1:n1] := F;
#      sel[n2:l] := F;
      cuty[sel] := avgy[sel];
    };
#    for (i in seq(l)) {
#      if (i>n1 && i<n2 && ((devy[i]>1+cut && cutpos)||(devy[i] < 1-cut) && cutneg)) {
#        cuty[i]:=avgy[i]
#      }
#    }
    aryy:=cuty
  };
  if (retratio) {
     return aryy/dezero(ary)
  } else {
    return aryy
  };
}

#-----------------------------------------------------------------------
#Name: newfilterlines
#-----------------------------------------------------------------------
#Type: Function
#Doc: Iteratively cuts out lines that are n*sigma above the mean spectrum
#     and replaces them with the running mean. 
#Ret: Array with lines cut-out and replaced by the mean
#
#Par: ary =	  	- array of input  data
#Par: nsigma = [3,3]	- array with nsigmas, iteratively flag data which 
#                	  are nsigma above the mean. The length of the array
#              		  determines the number of iterations
#Par: nx1,nx2 = -1  	- positions of the borders to cut the array into
# 			  three regions
#Par: cutpos =       	- Cuts on positive deviations 
#Par: cutneg =       	- Cuts (also) neagtive deviations 
#Par: retratio=      	- Returns the ratio between uncut and cut array rather
#                         then the cut array itself (usefull for flagging
#                         operations).
#-----------------------------------------------------------------------

newfilterlines := function(ary,
                           nsigma=[3,3],
                           nx1=-1,
                           nx2=-1,
                           cutneg=T,
                           cutpos=T,
                           retratio=F)
{
  aryy := ary;
  l    := len(aryy); 
  n    := 0;

  if (nx1<0) {nx1:=1;}
  if (nx2<0) {nx2:=l;}

  linary := array(0.,l);
  devy   := array(0.,l);
  cut    := array(0.,l);

  for (sigma in nsigma) {
    n+:=1;

    #-----------------------------------------------------------------
    # generate "avaraged" array by fitting three segments of lines
    # [1] linear regression through the sub-array
    # [2] Array containing values of the linear regression
    # [3] Difference between original data in the sub-band and the 
    #     values of the linear regression
    # [4] Compute statistical moments of the difference array compute
    #     in [3]

    # Segment 1
    
    seg := linreg([1:nx1],aryy[1:nx1]);     # [1]
    linary[1:nx1] := seg.eval               # [2]
    devy[1:nx1]:=aryy[1:nx1]-linary[1:nx1]  # [3]
    devmom := moments(2,devy[1:nx1]);       # [4]  
    cut[1:nx1] := (devy[1:nx1]-devmom.mean)/devmom.stddev;

    # Segment 2
    
    seg := linreg([nx1:nx2],aryy[nx1:nx2]);
    linary[nx1:nx2] := seg.eval
    devy[nx1:nx2]:=aryy[nx1:nx2]-linary[nx1:nx2]
    devmom := moments(2,devy[nx1:nx2]);
    cut[nx1:nx2] := (devy[nx1:nx2]-devmom.mean)/devmom.stddev;

    # Segment 3
    
    seg := linreg([nx2:l],aryy[nx2:l]);
    linary[nx2:l] := seg.eval
    devy[nx2:l]:=aryy[nx2:l]-linary[nx2:l]
    devmom := moments(2,devy[nx2:l]);
    cut[nx2:l] := (devy[nx2:l]-devmom.mean)/devmom.stddev;

    linary[linary<0] := 0.
    print 'Pass #',n,'- Standard Deviation:',devmom.stddev
    if (cutpos){
      sel := cut>sigma;
      aryy[sel] := linary[sel];
    };
    if (cutneg){
      sel := cut<(-1*sigma);
      aryy[sel] := linary[sel];
    };
  };
  if (retratio) {
     return aryy/dezero(ary)
  } else {
    return aryy
  };
}



#-----------------------------------------------------------------------
#Name: (modified_)hanning
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns a (modified-) hanning window 
#Ret: array with the window
#
#Par: size =	  			- length of the array
#-----------------------------------------------------------------------
modified_hanning := function(size){ 
  erg := array(1.,size);
  cut := floor(size/4)
  erg[1:cut] := 0.5+0.5*cos(pi*([1:cut]-cut)/cut) 
  erg[(size-cut+1):size] := 0.5+0.5*cos(pi*([cut:1]-cut)/cut) 
  return erg;
}
#hanning := function(size){
# return 0.5+0.5*cos(2*pi*([1:size]-size/2)/size) 
#}
hanning := function(size,alpha=0.5){ #hanning: alpha=0.5; hamming: alpha=0.54
 return alpha-(1-alpha)*cos(2*pi*[0:(size-1)]/size)
}
func step_hanning(size,steps=5){
  ret:=array(1.,size);
  ret[1:(steps+1)]:=0.5+0.5*cos(pi*([1:(steps+1)]-(steps+1))/(steps+1)) 
  ret[(size-steps+1):size]:=0.5+0.5*cos(pi*([steps:1]-steps)/steps) 
  return ret;
}

#-----------------------------------------------------------------------
#Name: sign
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns the sign of a real number
#Ret: -1,+1 (the latter also for 0)
#
#Par: r =	  			- real number
#-----------------------------------------------------------------------

sign:=function(r) {
  ret:=array(1,len(r));
  b := (r<0);
  ret[b]:=-1
  return ret
}

#-----------------------------------------------------------------------
#Name: phase
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns the phase of a complex number
#Ret: phase in degrees
#
#Par: c =	  			- complex number
#-----------------------------------------------------------------------

phase:=function(c) {
  global degree
  return sign(imag(c))*acos(real(c)/abs(c))/degree
}

#-----------------------------------------------------------------------
#Name: linreg
#-----------------------------------------------------------------------
#Type: Function
#Doc: performs a linear regression on a set of x- and y-values
#Ret: record with results
#     erg.a	- first coefficient of y = a + b * x
#     erg.b	- second coefficient of y = a + b * x
#     erg.chq	- chisqared of the fit
#     erg.eval	- values of the resulting function at positions x
#
#Par: x =	- x-axis values to be fitted
#     y = 	- y-axis values to be fitted
#-----------------------------------------------------------------------
linreg := function(x,y,eval=T){
  if (len(x)!=len(y)){
   print 'len(x)!=len(y) !!!';
   return F;
  }
  n := len(x)
  xq := sum(x)/n;
  yq := sum(y)/n;
  b := sum((x-xq)*(y-yq))/sum((x-xq)^2);
  a := yq-b*xq;
  erg:=[=];
  erg.a := a;
  erg.b := b;
  if (eval){
    erg.eval := a+b*x;
    erg.chq := sum((y-erg.eval)^2);
  };
  return erg;
};


#-----------------------------------------------------------------------
#Name: readsimtable
#-----------------------------------------------------------------------
#Type: Function
#Doc: read an (ascii) file with Tim's simulation data
#Ret: record with results
#     erg.time		- the time in seconds
#     erg.data		- the field-strength in Volts/meter
#     erg.intdata	- the field-strength interpolated onto a 80MSPS grid
#
#Par: file =	- the filename
#-----------------------------------------------------------------------
readsimtable := function(file){
  if (!fexist(file)){
    print 'Can\'t find file:',file,' Exiting!';
    return F;
  };
  tmp := shell(paste('wc',file));
  nopts := as_integer(split(tmp)[1]);
  erg := [=];
  erg.time := array(0.,nopts);
  erg.data := array(0.,nopts);
  i:=1;
  fd := open(paste('<',file));
  while (line := read(fd)){
    tmp := as_float(split(line));
    erg.time[i] := tmp[1];
    erg.data[i] := tmp[2]*2.99792458e4;
    i+:=1;
    if(i>nopts) break;
  }
  erg.range[1] := as_integer(min(erg.time*80e6));
  erg.range[2] := as_integer(max(erg.time*80e6));
  interp := interpolate1d()     
  interp.initialize(erg.time*80e6,erg.data,'nearest neighbor')
  erg.intdata := interp.interpolate([erg.range[1]:erg.range[2]]);
  return erg;
};

########################################################################
#-----------------------------------------------------------------------
#Name: runminc
#-----------------------------------------------------------------------
#Type: Function
#Doc:  calculate the running minimum
#Ret:  ary
#Par:  ary         data to be averaged
#      bs          block size over which to average
#Example: tic(); ary:=runminc([1,2,3,4,5],3); toc()
#-----------------------------------------------------------------------
func runminc(ary,bs=10) {
  inp:=[=]
  inp.ary:=ary
  inp.bs:=bs
  ary:=global_lopestools_client->runmin(inp)
  return ary
}
########################################################################
