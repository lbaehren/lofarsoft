#######################################################################
#
# Functions for the analysis of the event datafiles
#
########################################################################

global_gfitter := gauss1dfitter();

#-----------------------------------------------------------------------
#Name: genspec
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Generates 4 spectra from eventrecord
#Par:  rec    =       - 
#Par:  evrec  =       - 
#Par:  index  = [1:4] - 
#Par:  nochan = 4     - 
#-----------------------------------------------------------------------

genspec := function(ref rec, evrec, index=[1:4], nochan=4) {
  input_event(rec,evrec.filename);
  for (i in seq(len(evrec.filename))) {
    tmpind[1] := filterONEant(rec,index[1]);
    tmpind[2] := filterONEant(rec,index[2]);
    tmpind[3] := filterONEant(rec,index[3]);
    tmpind[4] := filterONEant(rec,index[4]);
    apo := CalcAntPos(rec,index,index[1],1);
    samplerate:=data.headf('Samplerate',index[1])*data.headf('SamplerateUnit',index[1]);
    ary1 := rec.get('f(x)',tmpind[1]);
    l := len(ary1);
    delays := round(angle2delay(apo,(evrec.AZ[i]/pi*180),(90-evrec.ZE[i]/pi*180),F)*samplerate)
    ary1 := rec.get('f(x)',tmpind[1]);
    for (j in [2:4]) {
      if (delays[j]<0) {
        ary1[(1-delays[j]):l] := ary1[(1-delays[j]):l] + 
                                rec.get('f(x)',tmpind[j])[1:(l+delays[j])];
      } else {
        ary1[1:(l-delays[j])] := ary1[1:(l-delays[j])] +
                                rec.get('f(x)',tmpind[j])[(1+delays[j]):l];
      };
    };
    delays := round(angle2delay(apo,evrec.AZ[i]/pi*180+180,90-evrec.ZE[i]/pi*180)*samplerate)
    ary2 := rec.get('f(x)',tmpind[1]);
    for (j in [2:4]) {
      if (delays[j]<0) {
        ary2[(1-delays[j]):l] := ary2[(1-delays[j]):l] + 
                                rec.get('f(x)',tmpind[j])[1:(l+delays[j])];
      } else {
        ary2[1:(l-delays[j])] := ary2[1:(l-delays[j])] +
                                rec.get('f(x)',tmpind[j])[(1+delays[j]):l];
      };
    };
    delays := round(angle2delay(apo,150,0)*samplerate)
    ary3 := rec.get('f(x)',tmpind[1]);
    for (j in [2:4]) {
      if (delays[j]<0) {
        ary3[(1-delays[j]):l] := ary3[(1-delays[j]):l] + 
                                rec.get('f(x)',tmpind[j])[1:(l+delays[j])];
      } else {
        ary3[1:(l-delays[j])] := ary3[1:(l-delays[j])] +
                                rec.get('f(x)',tmpind[j])[(1+delays[j]):l];
      };
    };
    ary4 := rec.get('f(x)',tmpind[1]);
    ary4 := ary4 + rec.get('f(x)',tmpind[2]);
    ary4 := ary4 + rec.get('f(x)',tmpind[3]);
    ary4 := ary4 + rec.get('f(x)',tmpind[4]);
    rec.put(ary1/len(index),'f(x)',tmpind[1]);
    rec.puthead(spaste(rec.head('Filename',index[1]),'Schauer'),'Filename',tmpind[1]);
    rec.put(ary2/len(index),'f(x)',tmpind[2]);
    rec.puthead(spaste(rec.head('Filename',index[1]),'Schauer+180'),'Filename',tmpind[2]);
    rec.put(ary3/len(index),'f(x)',tmpind[3]);
    rec.puthead(spaste(rec.head('Filename',index[1]),'Geb'),'Filename',tmpind[3]);
    rec.put(ary4/len(index),'f(x)',tmpind[4]);
    rec.puthead(spaste(rec.head('Filename',index[1]),'Zenit'),'Filename',tmpind[4]);

    index := index + nochan;
  };
}


#-----------------------------------------------------------------------
#Name: korr_Ne_Nmu
#-----------------------------------------------------------------------
#Type: Function
#Doc: correct for the systematic errors of the measured electron and
#     truncated muon number
#
#Ret: record with corrected Ne and Nmu
#
#Par: size	= 		- electron size
#Par: lmuo	= 		- truncated muon number
#
#-----------------------------------------------------------------------
korr_Ne_Nmu := function(size, lmuo, lgnetrue=[]){
  erg := [=] 

  lgne := log(size);
  lgnmu := log(lmuo);

  if (len(lgnetrue) != len(lgne)) lgnetrue := lgne;

  NeNmu := lgnetrue-lgnmu;
  Ke := -0.0854 + 0.0634 *NeNmu -0.0115 *(NeNmu^2);
  if ((lgne-Ke) > 6.304) {
    Ke := Ke +0.000745 -0.0497 * ((lgne-Ke-6.304)^2);
  };

  erg.Ne := 10^(lgne-Ke);

  lgnetrue := lgne-Ke;
  Kmu := -0.824 +0.472 *lgnetrue  -.0939 *(lgnetrue^2) +0.00639 *(lgnetrue^3);
  if ((lgnmu-Kmu) > 5.22) {
    Kmu := Kmu -0.00293 -0.194*((lgnmu-Kmu-5.22)^2)
  }

  erg.Nmu := 10^(lgnmu-Kmu)

  return erg
};


#-----------------------------------------------------------------------
#Name: getCRen
#-----------------------------------------------------------------------
#Type: Function
#Doc: get the primary particle energy from electron size, truncated muon 
#     number and zenith angle. This is an implementation of the function
#     by R. Glasstetter. 
#     It expects true electron and muon numbers, so for better results 
#     first correct the systematic effects.
#Ret: log(energy in GeV)
#
#Par: size	= 		- (corrected) electron size
#Par: lmuo	= 		- (corrected) truncated muon number
#Par: ze	= 		- zenith angle (in radians)
#
#-----------------------------------------------------------------------

getCRen := function(size, lmuo, ze)
{
  lg10ne := log(size);
  lg10nm := log(lmuo) +0.6877 -0.03835*lg10ne + 0.001186*lg10ne^2;
  
  dwb := 1023.*(1/cos(ze)-1.025);
  ne0 := lg10ne+dwb/158./2.302585093
  nm0 := lg10nm+dwb/823./2.302585093
  
  lgE := 0.3015*ne0 +0.6762*nm0 +1.856
  
  return lgE;
};


#-----------------------------------------------------------------------
#Name: fitgauss
#-----------------------------------------------------------------------
#Type: Function
#Doc: square, smooth and normalize a time spectrum and fit a gauss to it 
#Ret: Record with the fit results
#
#Par: ary	= 		- array with the time spectrum to be fitted
#Par: filter	= 13		- width of the mean-filter
#Par: cregion	= [32000,33600]	- region in which to look for a peak
#                                 to set the starting center for the fit
#Par: height	= 10		- starting height for the fit
#Par: width	= 4		- starting width for the fit
#Par: maxiter	= 30		- max number of iterations for the fit
#
#-----------------------------------------------------------------------

fitgauss := function (ary,
                      filter=13,
                      cregion=[32000,33600],
                      height=10,
                      width=4,
                      maxiter=30)
{
  siz:=len(ary);
  fary := cMeanFilter(abs(ary)^2,filter);  #square and smooth the array
  ind :=[1:(cregion[1]),(cregion[2]):siz];
  mom := moments(2,fary[ind]);
  if (mom.stddev>0.) { 
    fary := (fary-mom.mean)/mom.stddev; #normalize the array to its  stddev 
  };
  guess.height  := height;
  guess.width   := width;
  guess.maxiter := maxiter;
 #find the position of the maximum inside the center region
  guess.center := order(fary[cregion[1]:cregion[2]])[(cregion[2]-cregion[1]+1)]+cregion[1]-1;
  global_gfitter.setstate(guess);
#print gfitter.getstate();
  state := global_gfitter.fit(seq(len(fary)),fary);
#print state;
#state.fary := fary;
#state.fitted := global_gfitter.eval(seq(len(fary)));
  return state;
}


#-----------------------------------------------------------------------
#Name: fitgauss2
#-----------------------------------------------------------------------
#Type: Function
#Doc: smooth a time spectrum and fit one or more gauss(es) to it
#Ret: Record with the fit results
#
#Par: ary	= 		- array with the time spectrum to be fitted
#Par: filter	= 13		- width of the mean-filter
#Par: cregion	= [32000,33600]	- region in which to look for a peak
#                                 to set the starting center for the fit
#Par: height	= 10		- starting height for the fit
#Par: width	= 4		- starting width for the fit
#Par: maxiter	= 30		- max number of iterations for the fit
#
#-----------------------------------------------------------------------

fitgauss2 := function(ary,
                      filter=7,
                      cregion=[1,500],
                      height=0.13,
                      width=0.4,
                      center=-1.5,
                      maxiter=30)
{
  siz:=len(ary);
  fary := cMeanFilter(abs(ary),filter);  #smooth the array
  fary := fary-mean(fary[(cregion[1]):(cregion[2])])# substract constant offset

  guess.height  := height;
  guess.width   := width;
  guess.center  := center
  guess.maxiter := maxiter;
  global_gfitter.setstate(guess);
#print gfitter.getstate();
  state := global_gfitter.fit(seq(len(fary)),fary);
#print state;
#state.fary := fary;
#state.fitted := global_gfitter.eval(seq(len(fary)));
  return state;
}

#-----------------------------------------------------------------------
#Name: get_event_param
#-----------------------------------------------------------------------
#Type: Function
#Doc: get pulse-parameters for a given event
#Ret: Record with the pulse-parameters
#
#Par: rec	= 		- Record, where data is stored
#Par: file	= 		- filename of the event
#Par: index	= 		- indices of the antennas to work on;
#Par: AZ	= 		- azimuth angle from KRETA
#Par: ZE	= 		- zenith angle from KRETA
#Par: debug	= 		- generate debug output
#Par: calcpair	= 		- calculate the pairparameter
#
#-----------------------------------------------------------------------

get_event_param := function (ref rec, 
                             file,
                             index=[], 
                             AZ=0,
                             ZE=90,
                             debug=F,
                             calcpair=T,
                             dofit=T)
{
  erg := [=];
  input_event(rec,file);
  if (len(index)==0) {index:=seq(rec.len);};
  if (max(index)>rec.len) { 
    print 'get_event_param: max(index)>rec.len Aborting!!!';
    return F;
  };
  
  if (calcpair) {
    #calculate the relative number of identical pairs 
    erg.pairpar := [=];
    for (i in index) {
       erg.pairpar[i] := identical_pairs(rec.get('f(x)',i));
       if (debug) print i,erg.pairpar[i];
    };
  };

# ### Use new fftfilter by Rosita
#  filtrec.array := array(0,len(rec.get('f(x)',index[1])),len(index));
#  for (i in seq(len(index))) {
#    filtrec.array[,i] := rec.get('f(x)',index[i]);
#  };
#  filtrec.sel := 0;
#  filtrec.strength := 4;
#  filtrec.filt := global_lopestools_client->fftfiler(filtrec);
#  filtered := [=];
#  for (i in seq(len(index))) {
#    filtered[(index[i])] := filtrec.filt[,i];
#  };

  #get a frequency-filter for all antennas;
  filters:=[=]; filter := T;
  for (i in index) {
    filters[i] := getFfilter_fast(rec,i,filter=filter);
    filter := filter&filters[i];
    if (debug) print i,'filter:',sum(filters[i])/len(filters[i]);
  };
  filter[1:(as_integer(len(filter)*0.002)+1)] := F;
  filter[(as_integer(len(filter)*0.998)+1):len(filter)] := F;
  if (debug) print 'gesfilter:',sum(filter)/len(filter);
  if (sum(filter)<(0.8*len(filter))) {
    print 'get_event_param: filter deletes more than 20% of all frequencys!'
  };
  #get frequency filtered time series
  filtered := [=];
  for (i in index) {
    fromary := rec.get('FFT',i);
    size := rec.getmeta('FFTSize',i);
    cal := TIM40GetCal2(rec, i);
    fromary :=  fromary*10^(cal/20);
    ary := FFTtoFx(fromary, size, filter);
    peak := max(abs(ary));
    maxwink := asin(max(abs(imag(ary))/(abs(ary)+(peak/1e5))))/pi*180 
    if (maxwink > 10) print 'get_event_param:  Attention! Large complex angle:',maxwink,'deg';
    filtered[i] := TIM40convFX2Voltage(ary,rec,index=i,inv=T);
     if (debug) print i,'filtered!';
  };

  if (debug) erg.filtered := filtered;

  if (dofit) {
    #get gaussian fit to the time series
    erg.fiterg := [=];
    for (i in index) {
       erg.fiterg[i] := fitgauss(filtered[i]);
       if (debug) print i,erg.fiterg[i];
    };
  };
    
  #form beam toward shower and fit gaussian to time-series 
   apo := CalcAntPos(rec,index,index[1],1);
  samplerate:=rec.headf('Samplerate',index[1])*rec.headf('SamplerateUnit',index[1]);
  delays := round(angle2delay(apo,(AZ/pi*180),(90-ZE/pi*180),F)*samplerate)
  siz := len(filtered[1]);
  shary := array(0,siz);
  for (i in index) {
    if (delays[i]<0) {
      shary[(1-delays[i]):siz] := shary[(1-delays[i]):siz] + 
                              filtered[i][1:(siz+delays[i])];
    } else {
      shary[1:(siz-delays[i])] := shary[1:(siz-delays[i])] +
                              filtered[i][(1+delays[i]):siz];
    };
  };
  shary := shary/len(index);
  if (debug) erg.shary := real(shary);
  if (dofit) {
    erg.shfit := fitgauss(shary);
    if (debug) print 'after beamforming:',erg.shfit;
  };

  return erg;
};

#-----------------------------------------------------------------------
#Name: generate_data_record_old
#-----------------------------------------------------------------------
#Type: Function
#Doc: calculate the pulse-parameters for the radio-data and add them to
#     the "kreta-record"
#
#  ########### preliminary! ###########
#
#-----------------------------------------------------------------------

generate_data_record_old := function(ref rec,
                                     kretarec,
                                     range=[1,1e100],
                                     keepfilt=0,
                                     dofit=T)
{
  input_event(rec,kretarec.filename[1]);
print rec.len;
  noind := rec.len;
  siz := len(kretarec.filename);
  range[1] := as_integer(max(min(range[1],siz),1));
  range[2] := as_integer(max(min(range[2],siz),1));
  if (dofit){
    kretarec.fiterg := [=];
    kretarec.fiterg.height := array(0.,siz,noind);
    kretarec.fiterg.width  := array(0.,siz,noind);
    kretarec.fiterg.center := array(0.,siz,noind);
    kretarec.shfit.height := array(0.,siz);
    kretarec.shfit.width  := array(0.,siz);
    kretarec.shfit.center := array(0.,siz);
  };
  kretarec.a3dist := get_det_dist(-39,13);
  kretarec.a4dist := get_det_dist(-13,39);
  if (keepfilt>0) {
    kretarec.filtered := [=];
    kretarec.shfilt := [=];
    if (keepfilt==1) {
      kretarec.filtrange := (([1:65536]-32768)/80);
      kretarec.filtrange := kretarec.filtrange[32000:33000];
      for (i in seq(siz)) {
        kretarec.filtered[i] := [=];
        kretarec.shfilt[i] := 0.;
      };
    };
  };

lasttime := 0.
  for (i in [range[1]:range[2]]){
print 'working on index',i,'(needed',lasttime,'sec for last event)';
    atime := time();
    if(kretarec.filename[i]=='') next;
    perg := get_event_param(rec, kretarec.filename[i], AZ=kretarec.AZ[i], 
                      ZE=kretarec.ZE[i],calcpair=F,debug=keepfilt,dofit=dofit);
    if (dofit){
      for (j in seq(noind)){
        kretarec.fiterg.height[i,j] := perg.fiterg[j].height;
        kretarec.fiterg.width[i,j] := perg.fiterg[j].width;
        kretarec.fiterg.center[i,j] := perg.fiterg[j].center;
      };
      kretarec.shfit.height[i] := perg.shfit.height;
      kretarec.shfit.width[i]  := perg.shfit.width;
      kretarec.shfit.center[i] := perg.shfit.center;
    };
    if (keepfilt>1) {
      kretarec.filtered[i] := perg.filtered;
      kretarec.shfilt[i] := perg.shary
    } else if (keepfilt>0) {
      for (j in [1:len(perg.filtered)]) {
        kretarec.filtered[i][j] := array(0.,1000);
        kretarec.filtered[i][j] := real(perg.filtered[j][32000:33000]);
      };
      kretarec.shfilt[i] := perg.shary[32000:33000];
    };

    lasttime := time()-atime;
print 'needed ',lasttime,'seconds for calculation';
  };

  return kretarec;
};

#-----------------------------------------------------------------------
#Name: merge_data_record
#-----------------------------------------------------------------------
#Type: Function
#Doc: merge simillar data records
#
#-----------------------------------------------------------------------
merge_data_record := function(inreca,inrecb){
  if ((field_names(inrecb) != field_names(inrecb)) |
       (len(inreca.filename) != len(inreca.filename) )) {
    print 'shape of record 1 and record 2 differ!';
    print 'Can\'t merge different records!';
    return F; 
  };
  erg := inreca;
  if (has_field(erg,'shfilt')){
    pattern := array(F,len(inrecb.GT));
    for (i in seq(len(inrecb.GT))){
      pattern[i] := len(inrecb.shfilt[i])>1;
    };
print sum(pattern),min(seq(len(inrecb.GT))[pattern]);
    erg.run[pattern] := inrecb.run[pattern];
    erg.evnum[pattern] := inrecb.evnum[pattern];
    erg.GT[pattern] := inrecb.GT[pattern];
    erg.MMN[pattern] := inrecb.MMN[pattern];
    erg.HMS[pattern] := inrecb.HMS[pattern];
    erg.clnum[pattern] := inrecb.clnum[pattern];
    erg.AZ[pattern] := inrecb.AZ[pattern];
    erg.ZE[pattern] := inrecb.ZE[pattern];
    erg.XC[pattern] := inrecb.XC[pattern];
    erg.YC[pattern] := inrecb.YC[pattern] ;
    erg.SIZE[pattern] := inrecb.SIZE[pattern];
    erg.LMUO[pattern] := inrecb.LMUO[pattern];
    erg.T0[pattern] := inrecb.T0[pattern];
    erg.filename[pattern] := inrecb.filename[pattern];
    erg.edep[pattern,] := inrecb.edep[pattern,];
    erg.edepsum[pattern,] := inrecb.edepsum[pattern,];
    if (has_field(erg,'logEguess')) {
      erg.logEguess[pattern] := inrecb.logEguess[pattern];
    };
#    if (has_field(erg,'fiterg')){
#      erg.fiterg.height[pattern,]  := inrecb.fiterg.height[pattern,];
#      erg.fiterg.width[pattern,]   := inrecb.fiterg.width[pattern,];
#      erg.fiterg.center[pattern,]  := inrecb.fiterg.center[pattern,];
#    };
#    erg.shfit.height[pattern]  := inrecb.shfit.height[pattern];
#    erg.shfit.width[pattern]   := inrecb.shfit.width[pattern];
#    erg.shfit.center[pattern]  := inrecb.shfit.center[pattern];
    if (has_field(erg,'filtered')) {
      for (i in seq(len(erg.GT))[pattern]) {
        erg.filtered[i]  := inrecb.filtered[i];
        erg.shfilt[i]  := inrecb.shfilt[i];
      };
    };
    if (has_field(erg,'RFImaxfakt')) {
      erg.RFIpow[pattern,] := inrecb.RFIpow[pattern,];
      erg.RFImax[pattern,] := inrecb.RFIpow[pattern,];
      erg.RFImaxfakt[pattern,] := inrecb.RFIpow[pattern,];
    };
  };
  if (has_field(erg,'ant1')) erg.ant1:=erg.ant1+inrecb.ant1
  if (has_field(erg,'ant2')) erg.ant2:=erg.ant2+inrecb.ant2
  if (has_field(erg,'ant3')) erg.ant3:=erg.ant3+inrecb.ant3
  if (has_field(erg,'ant4')) erg.ant4:=erg.ant4+inrecb.ant4
  if (has_field(erg,'ant5')) erg.ant5:=erg.ant5+inrecb.ant5
  if (has_field(erg,'ant6')) erg.ant6:=erg.ant6+inrecb.ant6
  if (has_field(erg,'ant7')) erg.ant7:=erg.ant7+inrecb.ant7
  if (has_field(erg,'ant8')) erg.ant8:=erg.ant8+inrecb.ant8
  if (has_field(erg,'mask1')) erg.mask1:=erg.mask1+inrecb.mask1
  if (has_field(erg,'mask2')) erg.mask2:=erg.mask2+inrecb.mask2
  if (has_field(erg,'mask3')) erg.mask3:=erg.mask3+inrecb.mask3
  if (has_field(erg,'mask4')) erg.mask4:=erg.mask4+inrecb.mask4
  if (has_field(erg,'mask5')) erg.mask5:=erg.mask5+inrecb.mask5
  if (has_field(erg,'mask6')) erg.mask6:=erg.mask6+inrecb.mask6
  if (has_field(erg,'mask7')) erg.mask7:=erg.mask7+inrecb.mask7
  if (has_field(erg,'mask8')) erg.mask8:=erg.mask8+inrecb.mask8
  if (has_field(erg,'fit')) erg.fit:=erg.fit+inrecb.fit
  if (has_field(erg,'fitmask')) erg.fitmask:=erg.fitmask+inrecb.fitmask

      
  return erg;
};

#-------------------------------------------------------------------------------
#Name: merge_data_records
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  files  - 
#Ret:  erg    - 
#-------------------------------------------------------------------------------

merge_data_records := function(files)
{
  l := len(files);
  erg := read_value(files[1]);
  for (i in [2:l]) {
    erg := merge_data_record(erg,read_value(files[i]));
  };
  return erg;
};

#-------------------------------------------------------------------------------
#Name: extend_data_record
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  --/--
#Par:  table  - 
#Par:  fields - 
#Ret:  ok = T - Status of the function.
#-------------------------------------------------------------------------------

extend_data_record := function(ref table, fields="RFIpow RFImax"){
  nev := len(table.GT);
  if (sum('RFIpow' == fields)>0) {
    RFIpow:=T;
    table.RFIpow := array(0.,nev,10,2);
  } else {RFIpow:=F;};
  if (sum('RFImax' == fields)>0) {
    RFImax:=T;
    table.RFImax := array(0.,nev,10);
  } else {RFImax:=F;};
  if (sum('RFImaxfakt' == fields)>0) {
    RFImaxfakt:=T;
    table.RFImaxfakt := array(0.,nev,10);
    meaninter := [=];
    stddevinter := [=];
    if (table.GT[1]<1.07296227e+09){
      print 'Using old edep-RFImax table';
      RFmaxdata := read_value('edep-RFImax-2003.sos');
    } else {
      RFmaxdata := read_value('edep-RFImax.sos');
    };
    for (i in [1:10]){
      meaninter[i] := interpolate1d();
      meaninter[i].initialize(RFmaxdata[i][,1],RFmaxdata[i][,3] ,'linear')
      stddevinter[i] := interpolate1d();
      stddevinter[i].initialize(RFmaxdata[i][,1],RFmaxdata[i][,4] ,'linear')
    };
    ant2edep := [1,2,3,4,0,7,5,6,0,0];
  } else {RFImaxfakt:=F;};
  for (i in seq(nev)){
    if (RFIpow && (len(table.filtered[i])>0)) {
      for (j in seq(len(table.filtered[i]))){
        table.RFIpow[i,j,1] := sum(table.filtered[i][j][1:500]^2)/500.;
        table.RFIpow[i,j,2] := sum(table.filtered[i][j][609:729]^2)-table.RFIpow[i,j,1]*121.;
      };
    };
    if (RFImax && (len(table.filtered[i])>0)) {
      for (j in seq(len(table.filtered[i]))){
        table.RFImax[i,j] := max((table.filtered[i][j][609:729]^2));
      };
    };
    if (RFImaxfakt && has_field(table,'RFImax')) {
      for (j in seq(len(table.RFImax[i,]))){
        if (ant2edep[j]>0){
          mval := meaninter[j].interpolate(table.edepsum[i,(ant2edep[j])])
          sval := stddevinter[j].interpolate(table.edepsum[i,(ant2edep[j])])
          table.RFImaxfakt[i,j] := (table.RFImax[i,j]-mval)/sval;
if (i==2) print i,j,ant2edep[j],table.edepsum[i,(ant2edep[j])],mval,sval
        };
      };
    };
  };
  return T;
};

#-----------------------------------------------------------------------
#Name: ang2B
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Calculates the angle between a certain direction in AzEL to the
#      Earth magnetic field
#Par:  az - Azimuth angle in radians
#Par:  ze - Zenith angle in radians 
#Ret:  real
#Ref:  crresults.g, loadcr
#-----------------------------------------------------------------------

ang2B := function(az,ze){
  bze := 25.*degree;baz:=180.*degree
  sprod := cos(az)*sin(ze)*cos(baz)*sin(bze) +sin(az)*sin(ze)*sin(baz)*sin(bze) +cos(ze)*cos(bze)
  return acos(sprod)
};

#-----------------------------------------------------------------------
#Name: get_gain_grad
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Get the absolute value of the gradient of the antenna gain.
#
#Par:  azdeg - Azimuth angle in degree
#Par:  eldeg - Zenith angle in degree 
#Ret:  real
#Ref:  crresults.g
#-----------------------------------------------------------------------

get_gain_grad := function(azdeg,eldeg){
  #azdeg := az/degree;
  #eldeg := 90-(ze/degree);
  freqarr := [45,50,55,60,65,70,75]*1e6

  if (azdeg<5) azdeg := 5;
  if (azdeg>355) azdeg := 355;
  if (eldeg<2.5) eldeg := 2.5;  
  if (eldeg>87.5) eldeg := 87.5;

  
  g1 := mean(getgains(azdeg+5,eldeg,freqarr,globalpar.AntennaGainRec));
  g2 := mean(getgains(azdeg-5,eldeg,freqarr,globalpar.AntennaGainRec));
  g3 := mean(getgains(azdeg,eldeg+2.5,freqarr,globalpar.AntennaGainRec));
  g4 := mean(getgains(azdeg,eldeg-2.5,freqarr,globalpar.AntennaGainRec));
#print azdeg,eldeg,':',g1,g2,g3,g4,mean(g1,g2,g3,g4)
  return sqrt( ((g1-g2)/10)^2 + ((g3-g4)/5)^2 )
};

#-----------------------------------------------------------------------
#Name: get_gainerror_fact
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Calculates the factor for the error due to the uncertainty of the
#      antenna gain with the unknown direction
#
#Par:  az               - Azimuth angle in radians
#Par:  ze               - Zenith angle in radians 
#Par:  dalpha = 0.3     - direction uncertainty in degrees
#Ret:  real
#Ref:  crresults.g
#-----------------------------------------------------------------------

get_gainerror_fact := function(az,ze,dalpha=0.3){
  azdeg := az/degree;
  eldeg := 90-(ze/degree);
  freqarr := [45,50,55,60,65,70,75]*1e6

  ggrad := get_gain_grad(azdeg,eldeg);
  gain := mean(getgains(azdeg,eldeg,freqarr,globalpar.AntennaGainRec));
#print azdeg,eldeg,dalpha,gain,ggrad
  return  ggrad/gain*0.5*dalpha
};


#-------------------------------------------------------------------------------
#Name: loadcr
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Load a cosmic ray event file from disk. A list of events is stored
#      in a table specified by "tabfiles". This can contain multiple files
#      and fileno specifies which table to read. The "ith" event listed in 
#      table is then loaded. AZ/EL direction of the cosmic ray from that 
#      table are stored in the event file and displayed in the GUI.
#
#Par: rec     =       - Data record to identify dataset
#Par: evtabin = [=]   - table with informations about the cosmic ray
#                       if fileno!= 0,  global variable evtab is used for
#                       convenience.
#Par: i	      = 1     - Which of the selected cosmic rays to load
#Par: fileno  = 0     - Which of the tabfiles to use: 
#                       (0=none (i.e. evtabin has to be set),
#                        >0 position in tabfiles);
#Par: tabfiles = []   - List of strings with table filenames
#Par: disp     = T    - print some of the parameters of the CR event
#Par: keepdir  = F    - use the directory givent in the table
#Par: angle    = -15  - angle by which to rotate cr position (e.g. going
#                       from LOPES X/Y to NS orientation.
#Par: doflag   = F    - automatically do a 2-pass RFI flag on events
#Par: doshifts = T    - shift the data by the guesstimate of the trigger delay
#Par: useCGA   = F    - load the event not as TIM40 data but into the C++ pipeline
#Ret: T
#Ref: globalpar.DefTabfileNo, globalpar.DefTabfiles,input_event,evtab,
#     data.load, startcr
#-------------------------------------------------------------------------------

loadcr := function(ref rec, evtabin=[=], i=1, fileno=0, tabfiles=[], disp=T,
                   keepdir=F,angle=-15,noload=F,doflag=F,doshifts=T,useCGA=F)
{
  global plotgui,globalpar
  global crtab,evtab;

 flag:=T;
 if (fileno<0) {fileno:=globalpar.DefTabfileNo}
 if (fileno>0) {
   if (len(tabfiles)==0) {tabfiles:=globalpar.DefTabfiles}
   if ((has_field(evtab,'tabfilename')) && (evtab.tabfilename == tabfiles[fileno])) {
     evtabin:=evtab;
   } else {
     print 'Reading evtab file:',tabfiles[fileno]
     evtabin:=read_value(tabfiles[fileno]); 
     if ( !(is_record(evtabin) && has_field(evtabin,'filename')) ) {
       print 'loadcr: Could not read',tabfiles[fileno],'ABORTING!';
       return F;
     };
     evtab:=evtabin;
     evtab.tabfilename := tabfiles[fileno];
     if (is_table(crtab)) {print 'Table crtab exists - done with it.'; crtab.done()};
     print 'Creating aips++ Table "crtab". Browse with crtab.browse()'
     crtab:=makeltab(evtab);
   };
   cur_event_dir:=tabfiles[fileno]~ globalconst.DirExtractor;
  }; 
  filename:=evtabin.filename[i]~ globalconst.FilenameExtractor
  if (keepdir) { filepath:=evtabin.filename[i];
  } else { filepath:=spaste(cur_event_dir,globalconst.DirSeparator,filename);};
  if (noload) return;
  if (!fexist(filepath)) {
    print 'Can\'t load file',filepath,'- file does not exist!';
    return F;
  };
  lopespos := mx.rotate([evtabin.XC[i],evtabin.YC[i]],angle,T);
  globalpar.event_loaded:=i;

  #-------------------------------------------------------------------
  # Print some of the parameters of the CR event

  alphaB:=ang2B(evtabin.AZ[i],evtabin.ZE[i])/degree

  if (disp) {
    print '#------------------------------------------------------------------------'
    print '#Event No.',i,'/',len(evtabin.filename),'in File',filename
    printf('#XC YC: %6.2f, %6.2f  (%6.2f, %6.2f)\\n',evtabin.XC[i],evtabin.YC[i],lopespos[1],lopespos[2])
    printf('#%s %6.2f, %6.2f\\n','AZ EL:',evtabin.AZ[i]/degree,(90-evtabin.ZE[i]/degree))
    printf('#%s %6.2f\\n','Geomagnetic Angle:',alphaB)
    printf('#%s%4.2f, %s%4.2f, %s%7.2g\\n','log(Ne)=',log(evtabin.SIZE[i]),'log(Nmu)=',log(evtabin.LMUO[i]),'Eguess=',10^evtabin.logEguess[i]*1e9)
}

  #-------------------------------------------------------------------
  # Read in the eventfile data from disk

  if (!useCGA){
    input_event(rec,filepath);
  } else {
    input_CGA_event(rec,filepath);
  };
  eventid:=rec.crid();

  rec.put([evtabin.XC[i],evtabin.YC[i]],'XCYC',-1);
  rec.put([lopespos[1],lopespos[2]],'XY',-1);
  rec.put(alphaB,'alphaB',-1);
  rec.put(10^evtabin.logEguess[i]*1e9,'Energy',-1);
  rec.put(spaste(as_string(evtabin.run[i]),'-',as_string(evtabin.evnum[i])),'EventID',-1);
  rec.put(as_string(eventid),'CRID',-1);
  rec.put([evtabin.AZ[i]/degree,(90-evtabin.ZE[i]/degree)],'AZEL');

  if (has_field(globalpar.event_distance,eventid)) {
    rec.put(globalpar.event_distance[eventid],'Distance');

  if (disp) {print '#Setting Distance to',rec.get('Distance'),'m.'}
  } else {
    rec.put(2750,'Distance');
  if (disp) {print '#Setting Distance to default distance 2750 m.'}
  }  

  if (disp) {
    print '#Event ID:',eventid 
    print '#------------------------------------------------------------------------'
  };

  #-------------------------------------------------------------------
  # Shift the data by the guesstimate of the trigger delay

  if (doshifts){
    kt := get_cluster_times(rec);
    triggerdelay := -sort(kt)[10]; #tigger occurs, when the shower reaches the 10th cluster
    triggerdelays := rec.getary('Caldelay')+triggerdelay;
    if (has_field(globalpar.event_shifts,eventid)) {
      print 'Manually correcting time axis of event #',i,'(',eventid,') by',globalpar.event_shifts[eventid]/1e-6,'µs.'
      triggerdelays +:= globalpar.event_shifts[eventid];
    }
    triggerdelays +:= globalpar.CRtriggeroffset; #Offest due to changes in the hardware
    rec.putary(triggerdelays,'Caldelay'); #some hack, but it works for now.
    print 'Correcting for instrumental triggeroffset by',globalpar.CRtriggeroffset/1e-6,'s and triggerdelay by',triggerdelay/1e-6,'s.'
  };

  if (doflag && !useCGA) {
    plotgui.messlabel->text(paste('Start calibration on TV station.'));
    print 'Phase CALIBRATION on TV';
    print 'please wait...';
    flags:=phasecmp(rec);
    rec.setphase(index=-1);
    for (i in seq(rec.len)[rec.select()]) {
      nx1 := sum(rec.get('Frequency',i)<globalpar.Bandpass[1]);
      nx2 := sum(rec.get('Frequency',i)<globalpar.Bandpass[2]);
      printf ('%s%-2i: \n','Antenna #',i);
      rec.putflag(rec.getflag('AbsFFT',index=i)*newfilterlines(abs(rec.get('CalFFT',i)),
                  cutneg=F,nsigma=[3,3],nx1=nx1,nx2=nx2,retratio=T),field='FFT',
                  index=i); 
    }
    if (has_field(globalpar.event_flag,eventid)) {
      for (j in globalpar.event_flag[eventid]) {
        print 'Flagged Antenna #',j
        rec.select(j,F);
        if (has_field(rec,'aux')) {rec.aux.select(j,F)};
      }
    }  
  };
   
  rec.refreshgui(T)
  if (is_record(plotgui)){
    plotgui.setphase();
  } else {
    rec.put(T,'Doshift',-1)
    rec.setphase()
  };
  rec.savedirection()
  rec.saveblockinfo()
  rec.replot(T);

  return flag;
}


#-----------------------------------------------------------------------
#Name: crguis
#-----------------------------------------------------------------------
#Type: Function
#Doc: Load a cosmic ray event file from disk, starts up all the necessary
#     GUIs (i.e., Spawn & Beam but not the default one which is assumed 
#     to be up) and sets the views appropriate for CR inspection. 
#
#Par: n	= 68		- Which of the selected cosmic rays to load
#Par: fileno = 1	- Which of the tabfiles to use: (
#                        0=none, >0 position in tabfiles
#Par: revrev=F      - Spawn a separate windows which contains the
#                     reverse transformation from FFT to time (TIM40revrev) - this
#                     is used for the skymapper to work with frequency
#                     cleaned data and short blocks in time
#Ret: T
#Ref: loadcr, data.load
#-----------------------------------------------------------------------
crguis:=function(n=68,fileno=1,revrev=F){
  global data, plotgui
  if (n>0 && globalpar.event_loaded!=n) data.load(n,fileno=fileno);
  if (!has_field(plotgui,'plotantpg')) {plotgui.plotantpg:=[=]};
  plotantf(plotgui.plotantpg,data,azel=plotgui.AZEL*degree, xy=plotgui.xy, observatory=data.head('Observatory'));
  print 'Spawning filtered data record.';
  SpawnRecord(data,data.aux,totype='TIM40rev');
  plotgui.aux:=plotguif(data.aux, title=spaste('> Spawn'));
  print 'Press !-key (shift-1 on German keyboard) to get CR-optimized view in Spawned window'
  plotgui.aux.view(1);
  
  if (revrev) {
  global data2, plotgui2
    if (has_field(data2,'plotgui')) val data2.plotgui:=F
    val data2:=F; data2:=[=]; 
    SpawnRecord(data.aux,data2,totype='TIM40revrev');
    plotgui2:=plotguif(data2, title=spaste('>>revrev'));
  }

  val data.aux.aux:=F; val data.aux.aux:=[=];
  print 'Spawning beamed data record.';
  SpawnRecord(data.aux,data.aux.aux,totype='TIM40beam');
  val plotgui.aux.aux:=F; val plotgui.aux.aux:=[=]; 
  plotgui.aux.aux:=plotguif(data.aux.aux,title=spaste('+ Beam'));
  print 'Press "-key (shift-2 on German keyboard) to get CR-optimized view in Spawned window'
  plotgui.aux.aux.view(2);
}

#-----------------------------------------------------------------------
#Name: ShowerDist
#-----------------------------------------------------------------------
#Type: Function
#Doc: Calculates the distance to the shower axis (in the shower coordinates)
#
#Ret: array with the distances (in meters)
#
#Par: azel   =	  - direction of the shower [az,el] in degree
#Par: antpos =	  - antenna positions relative to the shower center
#Par: doz    = T  - use the z-coordinates of the antenna
#Ref: toShower
#-----------------------------------------------------------------------
ShowerDist := function(azel,antpos,doz=T){
  erg := array(0.,len(antpos));
  for (i in seq(len(antpos))) {
    coords := toShower(azel,antpos[i],doz=doz)
    erg[i] := sqrt(coords.x^2+coords.y^2);
  };
  return erg;
}

#-------------------------------------------------------------------------------
#Name: makecrmap
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  A function to map a cosmic ray event including stepping through distances 
#      using the skymapper. The function temporarily spawns a new data record
#      which uses as input the RFI cleaned time series (typically in data.aux).
#Par:  rec                         - basic data record (typically "data")
#Par:  drange = [2000,2000]        - Range of distances, in [m], for which to
#Par:  dstep  = 500                - Stepwidth, in [m]
#Par:  azel   =[0,90]               - location of center pixel
#Par:  pixels = 480                 - number of pixels
#Par:  resolution = 0.5             - resolution in degrees
#Par:  xrange = [-1.95e-6,-1.7e-6] - Interval along the x-axis, over which to
#                                    search the maximum
#Par:  field  = 'Time'             - Quantity along the x-axis, e.g. 'Time' or
#                                    'Frequency'.
#Par:  minblocklen =128 		   - Minimum blocklength to use for mapping if xrange is too small
#Par:  index=-1                    - Vector of antenna (number)s to use for the mapping 
#Ret:  T
#Ref:  skymapper, findpointsourcedist
#Example: makecrmap(data,azel=[4.17134867,66.5725254],resolution=0.2,pixels=256)
#-------------------------------------------------------------------------------
makecrmap := function(ref rec, drange=[2000,2000], dstep=250,azel=[0,90],
                                xrange=[-2.05e-6,-1.55e-6], resolution=0.5,
                                pixels=480, field='Time',minblocklen=128,
                                index=-1){

  global im,imageTool
  local rec2

  xnrange     := [rec.getxn(xrange[1],field),rec.getxn(xrange[2],field)]

  print 'Spawning new data record for Skymapper.'
  SpawnRecord(rec.aux,rec2,totype='TIM40revrev');
  print 'Done.'

  f:='XY';rec2.put(rec.get(f),f,-1)
  f:='Center';rec2.put(rec.get(f),f,-1)
  rec2.setphase();
  rec2.resetblockinfo();

  rec.put(F,'DoZ',-1)
  rec.resetdir();
  rec.replot(all=T)

  blocklen    := max(2^ceiling(log(xnrange[2]-xnrange[1]+1)/log(2)),minblocklen)
  blockcenter := ceiling((xnrange[1]+xnrange[2])/2.)
  blockoffset := blockcenter-blocklen/2
  rec2.setblock(blocksize=blocklen, offset=blockoffset);
  rec2.setblock(n=1);
  distrange := [rmin=drange[1],
                incr=dstep,
                slices=ceiling((drange[2]-drange[1])/dstep)+1,
                rmax=drange[2]];
  paramAntenna:=[index=index,refant=-1,doz=T,xy=T]

  print '========================================================================'
  print 'Mapping out 3D-Structure of Event:'
  print '========================================================================'
  print 'AZ/EL        :',azel
  print 'Pixels       :',[pixels,pixels]
  print 'Resolution   :',resolution,'deg'
  print 'Range        :',xrange,rec2.coordinfo(field).Units
  print '             :',xnrange,'Samples'
  print 'Block length :',blocklen,'Samples'
  print 'Block size   :',len(rec2.get('f(x)')),'Samples'
  print 'Block center :',blockcenter,'Samples'
  print 'Block offset :',blockoffset,'Samples'
  print 'Distances    :',distrange,'m'
  print 'Antenna Param:',paramAntenna
  print '========================================================================'

  if (is_image(imageTool)) {imageTool.done()}
     skymapper(rec2,
            paramAntenna=paramAntenna,
            mapTitle='Find Distance to Point Source',
            mapCenterCoords=azel,
            mapCenterWCS='AZEL',
            incrCSC=[resolution,resolution],
            rangeElevation=[0,90],
            mapPixels=[pixels,pixels],
            mapDistance=distrange,
            nofIntegrations=1,
            nofBlocks=1,
            nofSamples=blocklen,
            firstBlock=1,
            SignalQuantity='power',
            SignalDomain='time',
            LaunchViewer=T
           ) 

  print 'Resetting Direction and Block Sizes ...'

  rec.restoredirection(refresh=T);
  rec.put(T,'Doshift',-1)
  rec.put(T,'DoZ',-1)

  val rec2:=F
}


#-------------------------------------------------------------------------------
#Name: findpointsourcedist
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  A very crude search for the optimal distance to a cosmic ray event.
#      This will just step through distances and look for the maximum 
#      using the skymapper. The function temporarily spawns a new data record
#      which uses as input the RFI cleaned time series (typically in data.aux).
#      The function returns a record with the optimal distance, time, and
#      position.
#Par:  rec                         - basic data record (typically "data")
#Par:  rec2                        - data record used for imager (typically "data2")
#                                    this is obtained through spawning with
#                                    crguis(revrev=T)
#Par:  drange = [1000,8000]        - Range of distances, in [m], for which to
#                                    scan. Use [-1,-1] for one slice at infinity.
#Par:  dstep  = 500                - Stepwidth, in [m]
#Par:  xrange = [-1.95e-6,-1.7e-6] - Interval along the x-axis, over which to
#Par:  pixels = 480                 - number of pixels
#Par:  resolution = 0.5             - resolution in degrees
#                                    search the maximum
#Par:  field  = 'Time'             - Quantity along the x-axis, e.g. 'Time' or
#                                    'Frequency'.
#Par:  minblocklen =128            - Minimum blocklength to use for mapping if
#                                    xrange is too small
#Par:  fitxy = F                   - fit for the XY-position of the shower axis
#Par:  findmaxium = F              - Find maximum in maps? Otherwhise only produce
#                                    the maps and do not find maximum.
#Par:  remap=T                     - Call skymapper? If F use old map from previous
#                                    run (if still defined)
#Par:  maxoffset=1.5               - Maximally tolarable offset in degree between
#                                    initial CR position and the one found in the
#                                    map. If the offset is too large the plotpanel
#                                    gui will not be updated.
#Par:  index=-1                    - Vector of antenna (number)s to use for the mapping 
#Ret:  record
#Ref:  skymapper, guipipeline, crprocess
#-------------------------------------------------------------------------------
#Example: data.load(68);
# crguis(68,revrev=T); 
# res:=findpointsourcedist(data,drange=[2000,3000],dstep=500,findmaximum=F)
# data.load(108,T,2);
# crguis(16,fileno=2)
# fmax:=findpointsourcedist(data,xrange=[-1.813e-6,-1.787e-6],drange=[2000,5000],dstep=500,remap=T);
# fmax:=findpointsourcedist(data,xrange=[-1.813e-6,-1.787e-6]);
# include 'data-event-func.g'
# detected_events:=[16,55,63,82,87,91,99,108,115,125,188,201,211,217,231,232]
#                  #[0.5,0.3 1 0.7 0.5 0.5 0.6 1 0.4 0.2 0.9 0.8 0.7 0.2 0.9 0.8]
#fmax:=findpointsourcedist(data,xrange=[-1.813e-6,-1.787e-6],drange=[-1,-1],remap=T)
#-------------------------------------------------------------------------------
#Status: developement
#-------------------------------------------------------------------------------

findpointsourcedist := function(ref rec,
                                drange=[1000,10000], 
                                dstep=250,
                                xrange=[-2.05e-6,-1.55e-6], 
                                resolution=0.2,
                                pixels=25, 
                                field='Time',
                                minblocklen=128,
                                remap=T,
                                fitxy=F,
                                findmaximum=T,
                                maxoffset=1.5,
                                index=-1)
{

  global im,imageTool

  rec.restoredirection()
  rec.restoreblockinfo()
  rec.put(plotgui.refant,'RefAnt',-1)
  rec.replot(all=T)

  azel        := rec.get('AZEL');
  dist        := rec.get('Distance');

  xnrange     := [rec.getxn(xrange[1],field),rec.getxn(xrange[2],field)]
  searchwindow:=floor((xnrange[2]-xnrange[1])/2)

if (remap) {
  local rec2
  print 'Spawning new data record for Skymapper.'
  SpawnRecord(rec.aux,rec2,totype='TIM40revrev');
  print 'Done.'

  f:='XY';rec2.put(rec.get(f),f,-1)
  f:='Center';rec2.put(rec.get(f),f,-1)
  rec2.setphase();
  rec2.resetblockinfo();

  rec.put(F,'DoZ',-1)
  rec.resetdir();
  rec.replot(all=T)

  blocklen    := max(2^ceiling(log(xnrange[2]-xnrange[1]+1)/log(2)),minblocklen)
  blockcenter := ceiling((xnrange[1]+xnrange[2])/2.)
  blockoffset := blockcenter-blocklen/2
  rec2.setblock(blocksize=blocklen, offset=blockoffset);
  rec2.setblock(n=1);
  distrange := [rmin=drange[1],
                incr=dstep,
                slices=ceiling((drange[2]-drange[1])/dstep)+1,
                rmax=drange[2]];
  paramAntenna:=[index=index,refant=-1,doz=T,xy=T]

  print '========================================================================'
  print 'Mapping out 3D-Structure of Event:'
  print '========================================================================'
  print 'AZ/EL        :',azel
  print 'Pixels       :',[pixels,pixels]
  print 'Resolution   :',resolution,'deg'
  print 'Range        :',xrange,rec2.coordinfo(field).Units
  print '             :',xnrange,'Samples'
  print 'Block length :',blocklen,'Samples'
  print 'Block size   :',len(rec2.get('f(x)')),'Samples'
  print 'Block center :',blockcenter,'Samples'
  print 'Block offset :',blockoffset,'Samples'
  print 'Distances    :',distrange,'m'
  print 'Antenna Param:',paramAntenna
  print '========================================================================'

  if (is_image(imageTool)) {imageTool.done()}
     skymapper(rec2,
            paramAntenna=paramAntenna,
            mapTitle='Find Distance to Point Source',
            mapCenterCoords=azel,
            mapCenterWCS='AZEL',
            incrCSC=[resolution,resolution],
            rangeElevation=[0,90],
            mapPixels=[pixels,pixels],
            mapDistance=distrange,
            nofIntegrations=1,
            nofBlocks=1,
            nofSamples=blocklen,
            firstBlock=1,
            SignalQuantity='power',
            SignalDomain='time',
            LaunchViewer=F
           ) 

  print 'Resetting Direction and Block Sizes ...'

  rec.restoredirection(refresh=T);
  rec.put(T,'Doshift',-1)
  rec.put(T,'DoZ',-1)

  print 'Hanning Smoothing the time axis'
  if (is_image(im)) {im.done()}
  im:=imageTool.hanning(axis=4,drop=F)
#  im:=imageTool
  print im.summary()
  val rec2:=F
}
 
if (findmaximum) { 
  shp:=imageTool.shape()
  trc:=shp; blc:=shp; 
  tn1:=shp[4]/2-searchwindow
  tn2:=shp[4]/2+searchwindow
  blc[1:2]:=[1,1]
  blc[4]:=tn1
  trc[4]:=tn2
  slices:=shp[3]
  res:=[=];
  res.az:=array(0.,slices)
  res.el:=array(0.,slices)
  res.r:=array(0.,slices)
  res.t:=array(0.,slices)
  res.y:=array(0.,slices)
  res.x:=array(0.,slices)
  res.z:=array(0.,slices)
  res.pixflux:=array(0.,slices)
  res.flux:=array(0.,slices)
  res.pos:=[=];
  res.dir:=[=]
#  res.statistics:=[=]
#  res.cl:=[=]
  for (s in seq(slices)) {
     blc[3]:=s; trc[3]:=s;

#     print 'Finding Maximum at slice:',s,blc,trc
#     r1:=drm.box(blc,trc)
#     wblc:=imageTool.toworld(blc)
#     wtrc:=imageTool.toworld(trc)
#     r1:=drm.wbox(blc=wblc,trc=wtrc,pixelaxes=[1,2,3,4],csys=imageTool.coordsys())
#     res.statistics[s]:=[=]
#     im.statistics(res.statistics[s],region=r1,list=T)
#     blc[4]:=res.statistics[s].maxpos[4]
#     trc[4]:=res.statistics[s].maxpos[4]
#     r2:=drm.box(blc,trc)
#     res.cl[s]:=im.findsources(nmax=2,region=r2,point=F, width=2/resolution, negfind=F)

     blc[4]:=shp[4]/2-searchwindow
     trc[4]:=shp[4]/2+searchwindow
     mp:=[=]
     mf:=array(0,searchwindow*2+1);     
     n:=0;
     for (t in seq(tn1,tn2)) {
       stat:=F; stat:=[=];
       n+:=1; blc[4]:=t; trc[4]:=t
       r1:=drm.box(blc,trc)
       im.statistics(stat,region=r1,list=T)
       mp[n]:=stat.maxpos;
       mf[n]:=stat.max;
     }
     res.pixflux[s]:=max_with_location(mf,nmax);
     res.pos[s]:=mp[nmax]
     maxworld:=im.toworld(res.pos[s])
     blc[4]:=res.pos[s][4]
     trc[4]:=res.pos[s][4]
     r2:=drm.box(blc,trc)
#     res.cl[s]:=im.maxfit(region=r2,point=T, width=2/resolution, negfind=F, list=T)
     cl:=im.maxfit(region=r2,point=T, width=2/resolution, negfind=F, list=T)
     if (is_fail(cl)) {
       print "im.maxfit() for slice",s,"failed."
       res.dir[s] := [=];
     } else {
       res.dir[s]:=cl.getrefdir(1);
       res.az[s]:=res.dir[s].m0.value/degree
       if ((res.az[s])<0) res.az[s]+:=360
       res.el[s]:=res.dir[s].m1.value/degree
       res.r[s]:=maxworld[3]
       res.t[s]:=maxworld[4]
       res.flux[s]:=cl.getfluxvalue(1)[1]
       res.x[s]:=-sin(res.az[s]*degree)*cos(res.el[s]*degree)*res.r[s]
       res.y[s]:=cos(res.az[s]*degree)*cos(res.el[s]*degree)*res.r[s]
       res.z[s]:=sin(res.el[s]*degree)*res.r[s]
       cl.done()
     }
     print 'Found Maximum at position ',nmax,res.pos[s],res.flux[s], res.r[s],'m',roundn(res.t[s]*1e6,3),'s',roundn([res.az[s],res.el[s]],3)
  } 
plot(res.r,res.flux,lines=T,clear=T,xlabel='Distance [m]',ylabel='Radio Power',title='Search for distance to CR event',col=2)
 
  res.maxflux:=max_with_location(res.flux,nmax)
  res.nmax:=nmax
  res.maxr:=res.r[nmax]
  res.maxt:=res.t[nmax]
  res.maxazel:=[res.az[nmax],res.el[nmax]]
  res.oldazel:=azel
  res.maxdir:=dm.direction('AZEL',spaste(as_string(res.oldazel[1]),'deg'),spaste(as_string(res.oldazel[2]),'deg'))
  res.olddir:=dm.direction('AZEL',spaste(as_string(res.maxazel[1]),'deg'),spaste(as_string(res.maxazel[2]),'deg'))
  res.diroff:=dq.convert(dm.separation(res.olddir,res.maxdir),'deg').value
  print 'Maximum ',res.maxflux,' at Distance =',res.maxr,'AZEL=',roundn(res.maxazel,3),'t=',res.maxt
  print 'Offset: ',roundn(res.diroff,3),'deg'

  if (res.diroff<maxoffset) {
    rec.put(res.maxr,'Distance',-1)
    rec.put(res.maxazel,'AZEL',-1)
  } else {
    print 'Offset too large - not updating plot panel with these numbers ...'
  }
  print 'Replotting all windows (data)...'
  rec.setphase()
  rec.replot(all=T)

  if (fitxy) {
  b:=res.r < res.maxr*2
  b[1:3]:=T
  x:=res.z[b];   

  y:=res.x[b];
  fit.fit(coeff1,coefferrs1,chisq1,as_float(x),as_float(y))
  fit.eval(x0,0,coeff1)
  res.xcoeff:=coeff1
  res.xcoefferrs:=coefferrs1
  print 'x0:',x0,coeff1

  y:=res.y[b];
  fit.fit(coeff1,coefferrs1,chisq1,as_float(x),as_float(y))
  fit.eval(y0,0,coeff1)
  res.ycoeff:=coeff1
  res.ycoefferrs:=coefferrs1
  print 'y0:',y0,coeff1

  res.x0:=x0; res.y0:=y0
  }
  print 'Returning result record. If the program hangs here for some time, press cntrl-c and restart ... '
  return res
} else {
# only producing map 
return T
}
}


##   # --------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: findcrdist
#-------------------------------------------------------------------------------
#Type: Function
#Ret: float
#Doc: A very crude search for the optimal distance to a cosmic ray event.
#     This will just step through distances and look for the maximum 
#     in the beamed window (just using a narrow time range around the CR).
#     It will stop if after three consecutive steps the beamed power is decaying.
#     The function returns the optimal distance.
#     It requires that all GUIs are started (see crguis)
#Par: drange = [1000,8000]        - Range of distances, in [m], for which to
#                                   scan.
#Par: dstep  = 500                - Stepwidth in m
#Par: xrange = [-1.95e-6,-1.7e-6] - Interval along the x-axis, over which to
#                                   integrate; this e.g. can be a time-interval
#                                   or a frequency interval.
#Par: field  = 'Time'             - Quantity along the x-axis, e.g. 'Time' or
#                                   'Frequency'.
#Ref: crguis,IntegrateData
#-------------------------------------------------------------------------------

findcrdist:=function(drange=[1000,8000],dstep=250,xrange=[-1.95e-6,-1.7e-6],
                     field='Time'){
#findcrdist:=function(ref rec,ref plotgui, drange=[1000,8000],dstep=250,xrange=[-1.95e-6,-1.7e-6],
#                     field='Time'){

  global pg,data,plotgui
 if (!has_field(plotgui,'aux')  || !has_field(plotgui.aux,'aux')) {
    print 'Make sure you have started the CR event GUIs! (see "File" in the menu bar)'
  return F}
  if (xrange[1]!=-1.95e-6 ||   xrange[2]!=-1.7e-6)  {plotgui.aux.aux.view(2,xrange=[xrange[1]-(xrange[2]-xrange[1]),xrange[2]+(xrange[2]-xrange[1])])} else {plotgui.aux.aux.view(2)};
  SelectAxis(plotgui.aux.aux,data.aux.aux,naxis=2,axis='CC-Beam',scale='micro');

  ds  := seq(drange[1],drange[2],dstep)
  dmx := array(0,len(ds))
  dmi := array(0,len(ds))
  dmm := array(0,len(ds))

  n:=0; lastmax:=0; lastmaxpos:=1; done:=F;

  print 'Finding the distance (searching maximum flux):'
  print 'dist maximum         integal   max*integral'
  for (d in ds) {
    n+:=1
    if (!done) {
	data.put(d,'Distance',-1)
	data.setphase()
	data.replot(all=T)
	IntegrateData(plotgui.aux.aux.yary,plotgui.aux.aux.result,index=1,x1=data.aux.aux.getxn(xrange[1],field),x2=data.aux.aux.getxn(xrange[2],field),ncall=1)
      }
    dmx[n]:=plotgui.aux.aux.result[1].one.max
    dmi[n]:=plotgui.aux.aux.result[1].one.sum
#    dmm[n]:=dmx[n]*(dmi[n]-dmi[1])/dmx[1]
    dmm[n]:=dmx[n];
    if (dmm[n]>lastmax) {lastmax:=dmm[n]; lastmaxpos:=n};
    if (dmm[n]<lastmax && n > lastmaxpos+11) {done:=T}
    print d,dmx[n],dmi[n],dmm[n]
  }
  print 'Found maximum of',lastmax,'at d=',ds[lastmaxpos],'m.'

  # --------------------------------------------------------
  # Set up a customized PGPlotter tool to display data

  if (!is_defined('pg') || (len(pg)==1)) pg:=pgplotter(foreground='black',background='white')

  valX := [min(ds),max(ds)];
  valY := [min(dmm),max(dmm)];

  pg.clear()
  pg.sci(1)
  pg.env (valX[1],valX[2],valY[1],valY[2],0,0);
  pg.lab('Distance [m]',
         'Normalized Integrated power * Maximum',
         'Search for distance to CR event');
  pg.setcolor(2);
  pg.line(ds,dmm);

  # --------------------------------------------------------

  data.put(ds[lastmaxpos],'Distance',-1)
  data.setphase()
  data.replot(all=T)
  print 'End findcrdist: data.len=',data.len;
  return ds[lastmaxpos]
}


#-------------------------------------------------------------------------------
#Name: findsourcedist
#-------------------------------------------------------------------------------
#Type: Function
#Ret: float
#Doc: A very crude search for the optimal distance to a continuum source.
#     This will just step through distances and look for the maximum 
#     in the beamed window (just using a narrow frequecnzy range).
#     It will stop if after three consecutive steps the beamed power is decaying.
#     The function returns the optimal distance.
#     It requires that the beam GUI is started 
#     Integration is done over the plotted data! I.e. if the logarithm is plotted,
#     you will get a geometric mean/integral ....
#
#Par: drange = [1000,12000] - Range of distances, in [m], for which to
#                             scan.
#Par: dstep  = 1000         - Stepwidth in m
#Par: xrange = [23e6,26e6]  - Interval along the x-axis, over which to
#                             integrate; this e.g. can be a time-interval
#                             or a frequency interval.
#Par: field  = 'Frequency'  - Quantity along the x-axis, e.g. 'Time' or
#                             'Frequency'.
#Par: restrictScan = 10     - Given a maximum is encountered at position n,
#                             the further scan can be restricted to a certain
#                             number of values beyond this position, if no better
#                             maximum is found.
#Ref: crguis,IntegrateData,findcrdist
#-------------------------------------------------------------------------------

findsourcedist:=function(drange=[1000,12000],dstep=1000,xrange=[23e6,26e6],
                         field='Frequency',restrictScan=10){

  global data,plotgui

  ds  := seq(drange[1],drange[2],dstep);
  dmx := array(0,len(ds))
  dmi := array(0,len(ds))

  n:=0; lastmax:=0; lastmaxpos:=0; done:=F;

  for (d in ds) {
    n+:=1
    if (!done) {
	data.put(d,'Distance',-1)
	data.setphase()
	data.replot(all=T)
	IntegrateData(plotgui.aux.yary,plotgui.aux.result,index=1,x1=data.aux.getxn(xrange[1],field),x2=data.aux.getxn(xrange[2],field),ncall=1)
      }
    dmx[n]:=plotgui.aux.result[1].one.max
    dmi[n]:=plotgui.aux.result[1].one.sum
    if (dmx[n]<lastmax) {lastmax:=dmx[n]; lastmaxpos:=n};
    if (restrictScan>0) {
      if (dmx[n]>lastmax && n > lastmaxpos+restrictScan) {done:=T}
    }
    print d,dmx[n],dmi[n],lastmax,lastmaxpos
  }
  print 'Found maximum of',lastmax,'at d=',ds[lastmaxpos],'m.'

  # --------------------------------------------------------
  # Set up a customized PGPlotter tool to display data

  valX := [min(ds),max(ds)];
  valY := [min(dmi),max(dmi)];

  pg := pgplotter();
  pg.env (valX[1],valX[2],valY[1],valY[2],0,0);
  pg.lab('Distance [m]',
         'log(Integrated power)',
         'Search for distance to a continuum source');
  pg.setcolor(2);
  pg.line(ds,dmi);

  # --------------------------------------------------------

  data.put(ds[lastmaxpos],'Distance',-1)
  data.setphase()
  data.replot(all=T)
  return ds[lastmaxpos]
}

#-----------------------------------------------------------------------
#Name: guipipeline
#-----------------------------------------------------------------------
#Type: Function
#Ret: bool
#Doc: A basic pipeline for LOPES data using the GUI routines
#
#Par: i1 = 1    	- Starting event (refers to position in evlist if this is also given)
#Par: i2 = 68   	- Ending event (refers to position in evlist if this is also given)
#Par: fileno = 1 	- Event table file number to choose appropriate selection
#Par: evlist = []   	- List with event numbers, used only if it has non-zero length
#Par: trange = [-1.851e-6,-1.749e-6] - time range over which to search maximum
#Par: drange = [1000,8000]         - Range of distances, in [m], for which to
#                                    scan. Use [-1,-1] for one slice at infinity.
#Par:  maxoffset=1.5               - Maximally tolarable offset in degree between
#                                    initial CR position and the one found in the
#                                    map. If the offset is too large the plotpanel
#                                    gui will not be updated.
#Par: filename = 'guipipeline-results.log'	- name of the logfile
#Par: psfilename = 'guipipeline-'		- prefix for the ps-images
#Par: dirname = ''		- directory to put the files in
#Par: doshifts = T		- shift the data by the guesstimate of 
#				  the trigger delay
#Par:  simplesearch=T     - Seach the distance by keeping the position on the 
#                           sky fixed (using the slow GUI method)
#Par:  useskymapper=F     - Do a full search of the maximum in position, distance 
#                           and time using the skymapper? Right now both methods 
#                           (simple/GUI and skymapper) will be done sequentially if 
#                           simplesearch=T an useskymapper=F
#Par:  remap=F            - Call skymapper in findmaximum? If F use old map from 
#                           previous run (if still defined) - this is mainly used for 
#                           speeding up debugging

#Par: allfiles =F   - Write all outputfiles even if i1==i2 (i.e. just one event)
#Ref: crguis,fitgaussplot,findcrdist
#Example: guipipeline(2,2,dirname='../data/CRs/4') <br>
#Example: guipipeline(dirname='../data/CRs/4')
#Example: guipipeline(102,102,dirname='.',fileno=2)
#Example: guipipeline(1,16,evlist=[16,55,63,82,87,91,99,108,115,125,188,201,211,217,231,232],drange=[-1,-1],dirname='.',fileno=2,simplesearch=F,useskymapper=T)
#Example: guipipeline(1,16,evlist=[87,211],drange=[-1,-1],dirname='.',fileno=2,simplesearch=F,useskymapper=T)
#Example: guipipeline(125,125,dirname='.',drange=[2500,2500],fileno=2,simplesearch=F,useskymapper=T,trange=[-1.813e-6,-1.787e-6])
#-----------------------------------------------------------------------

guipipeline := function(i1=1,
                        i2=69,
                        fileno=1,
                        evlist=[],
                        trange=[-1.89e-6,-1.73e-6],
                        drange=[1000,10000],
                        maxoffset=1.5,
                        filename='guipipeline-results',
                        psfilename='guipipeline-',
                        dirname='guipipeline',
                        doshifts=T,
                        allfiles=F,
                        useskymapper=F,
                        simplesearch=T,
                        remap=T)
{
  global data,plotgui,crtab

  if (!is_table(crtab) || !has_field(plotgui,'aux')  || !has_field(plotgui.aux,'aux')) {crguis(i1,fileno=fileno);};

  print ' '
  print '========================================================================'
  print '                               GUIPIPELINE                              '
  print '========================================================================'
  print ' '
  print 'Results will be stored in Directory',dirname,'. Look for files of the form',spaste(psfilename,'*')
  print 'A summary is given in',spaste(filename,'*.g (GLISH file).')
  print ' '

  if (len(evlist) ==0) evlist:=seq(1,i2)
  nevents:=max(evlist)

  if (!dexist(dirname)) {
    print 'Creating directory ',dirname; 
    cmd.mkdir(dirname)
  }

  if (i2!=i1 || allfiles) {
    filen2:=spaste(filename,'.',as_string(i1),'-',as_string(i2),'.g');
    filen3:=spaste(filename,'.data.g');
    print 'Creating pipeline result file:',filen2
    print 'This file can be read in with the include command in glish.'
    print 'You may have to run make_result first.'
    file2:=open(spaste('>',dirname,'/',filen2));

    write(file2,spaste('nevents:=',nevents),'empty:=array(0,nevents)', 'flag:=[=]', 'sel:=[=]','corr:=[=]','dist:=empty','azel:=[=]','fitX:=[=]','fitF:=[=]','fitC:=[=]','meanr:=empty','quality1:=empty','quality2:=empty','quality3:=empty','quality4:=empty','quality5:=empty','mn:=empty','sd:=empty','rmsF:=empty','rmsC:=empty','rmsX:=empty','diroff:=empty','offX:=[=]','offC:=[=]','offF:=[=]','mns:=[=]','mns2:=[=]','sds:=[=]','dists:=[=]','powers:=[=]','crid:=[=]','selheights:=[=]');
    write(file2,spaste('for (i in seq(nevents)) {'),'flag[i]:=[=]','sel[i]:=[=]','corr[i]:=[=]','mns[i]:=[=]','mns2[i]:=[=]','sds[i]:=[=]','fitX[i]:=[=]','fitF[i]:=[=]','fitC[i]:=[=]','dists[i]:=[=]','powers[i]:=[=]','offX[i]:=[=]','offF[i]:=[=]','offC[i]:=[=]','azel[i]:=[=]','selheights[i]:=[=]','crid[i]:=\'XXXXXXXXXX\'}');
  write(file2,spaste('include \'',filen3,'\''));
  write(file2,'if (fexist(add_dir(\'quality.g\'))) {include add_dir(\'quality.g\')}');
  write(file2,'include add_dir(\'crresults.g\')');
  file2:=F;

  file4:=open(  spaste('>',dirname,'/','make_results'));
  print 'Creating pipeline data make file:',spaste(dirname,'/','make_results');
  write(file4,'#',spaste('rm -f ',filen3),spaste('echo "Creating ',filen3,'"'),spaste('cat ',filename,'.*.one.g > ',filen3),'echo "Done."');
  print shell(spaste('chmod a+x ',dirname,'/','make_results'));
  file4:=F;
}
for (i in evlist[i1:i2]) {#start of the big loop
  print '\nStart CRPROCESS';
  filen:=spaste(filename,'.',sprintf('%5.5i',i),'.one.g')
  print 'Creating pipeline single data file:',filen
  file:=open(spaste('>',dirname,'/',filen));
  print trange
  results := crprocess(i,
                       fileno=fileno,
                       doshifts=doshifts,
                       trange=trange,
                       drange=drange,
                       maxoffset=maxoffset,
                       remap=remap,
                       useskymapper=useskymapper,
                       simplesearch=simplesearch);
  print 'End CRPROCESS';
#  print results;
  flag:=results.flag;
  meanr:=results.meanr;
  xn1:=results.xn1;
  xn2:=results.xn2;

  if ((xn2-xn1)<=4) {print 'Old xn1/2:',xn1,xn2; xn1-:=4; xn2+:=4; print 'Enlarging time ranging for fitting:',xn1,xn2};

  plotgui.aux.aux.plotfit:=array(F,maxnpanels); plotgui.aux.aux.fitresults[1]:=[=];
  write(file,'#-----------------------------------------------------------------------');
  write(file,'i:=',i,'\n',sep='');
  write(file,'crid[',i,']:=\'',data.crid(),'\'\n',sep='');
  write(file,'sel[',i,']:=',as_inputvec(flag),'\n',sep='');
  write(file,'flag[',i,']:=',as_inputvec(data.select()),'\n',sep='');
  write(file,'corr[',i,']:=',as_inputvec(globalpar.corrected_ant),'\n',sep='');
  write(file,'meanr[',i,']:=',meanr,'\n',sep='');
  write(file,'dist[',i,']:=',data.get('Distance'),'\n',sep='');
  write(file,'azel[',i,']:=',as_inputvec(data.get('AZEL')),'\n',sep='');
  write(file,'diroff[',i,']:=',results.fmax.diroff,'\n',sep='');
  write(file,'mn[',i,']:=',results.m,'\n',sep='');
  write(file,'sd[',i,']:=',results.s,'\n',sep='');
  write(file,'mns[',i,']:=',as_inputvec(results.mn),'\n',sep='');
  write(file,'mns2[',i,']:=',as_inputvec(results.mn2),'\n',sep='');
  write(file,'sds[',i,']:=',as_inputvec(results.stddev),'\n',sep='');

  print '\nProducing output files for data set #',i
  pg.postscript(spaste(dirname,'/',psfilename,'distance-',as_string(i),'.ps'))
  plotantf(plotgui.plotantpg,data,azel=plotgui.AZEL*degree, xy=plotgui.xy, observatory=data.head('Observatory'));
  plotgui.plotantpg.pg.postscript(spaste(dirname,'/',psfilename,'layout-',as_string(i),'.ps'))

  plotgui.aux.replot();
  plotgui.aux.pg.postscript(spaste(dirname,'/',psfilename,'All-',as_string(i),'.ps'))

  SelectAxis(plotgui.aux.aux,data.aux.aux,naxis=2,axis='X-Beam',scale='micro');
  plotgui.aux.aux.absy->state(F);
  plotgui.aux.aux.replot();
  fitgaussplot(plotgui.aux.aux,xn1,xn2,panels=1,nfit=1,width=4,maxiter=30)
  res:=plotgui.aux.aux.fitresults[1];
  ly:=len(plotgui.aux.aux.yary[1]);
  rmsX:=stddev(plotgui.aux.aux.yary[1][round(ly/4):round(3*(ly/8))])
  plotgui.aux.aux.replot();
  off1:=IntegrateData(plotgui.aux.aux.yary,plotgui.aux.aux.result,index=1,x1=results.xn1-100,x2=results.xn1,ncall=1)
  plotgui.aux.aux.pg.postscript(spaste(dirname,'/',psfilename,'X-',as_string(i),'.ps'))
  write(file,'fitX[',i,']:=',res,'\n',sep='');

  SelectAxis(plotgui.aux.aux,data.aux.aux,naxis=2,axis='FieldStrength',scale='micro');
  plotgui.aux.aux.absy->state(T);
  plotgui.aux.aux.replot();
  fitgaussplot(plotgui.aux.aux,floor(res.center_samples-4),floor(res.center_samples+4),center=res.center_samples,height=res.height,width=res.width_samples);
  plotgui.aux.aux.replot();
  off2:=IntegrateData(plotgui.aux.aux.yary,plotgui.aux.aux.result,index=1,x1=results.xn1-100,x2=results.xn1,ncall=1)
  res2:=plotgui.aux.aux.fitresults[1];
  rmsF:=stddev(plotgui.aux.aux.yary[1][round(ly/4):round(3*(ly/8))])
  write(file,'fitF[',i,']:=',res2,'\n',sep='');
  plotgui.aux.aux.pg.postscript(spaste(dirname,'/',psfilename,'F-',as_string(i),'.ps'))

  SelectAxis(plotgui.aux.aux,data.aux.aux,naxis=2,axis='CC-Beam',scale='micro');
  plotgui.aux.aux.absy->state(F);
  plotgui.aux.aux.replot();
  fitgaussplot(plotgui.aux.aux,floor(res.center_samples-4),floor(res.center_samples+4),center=res.center_samples,height=res.height,width=res.width_samples);
  plotgui.aux.aux.replot();
  off3:=IntegrateData(plotgui.aux.aux.yary,plotgui.aux.aux.result,index=1,x1=results.xn1-100,x2=results.xn1,ncall=1)
  res3:=plotgui.aux.aux.fitresults[1];
  rmsC:=stddev(plotgui.aux.aux.yary[1][round(ly/4):round(3*(ly/8))])
  write(file,'fitC[',i,']:=',res3,'\n',sep='');
  plotgui.aux.aux.pg.postscript(spaste(dirname,'/',psfilename,'CC-',as_string(i),'.ps'))

  xnmax:=round(res3.center_samples);

  yary1:=data.aux.aux.get('CC-Beam',1);
  yary2:=data.aux.aux.get('P-Beam',1);

  l1:=len(yary1)/10.;
  l2:=len(yary1)/3.;

  quality1:=yary1[xnmax]/yary2[xnmax];
  quality2:=(yary1[xnmax]-mean(yary1[l1:l2]))/(yary2[xnmax]-mean(yary2[l1:l2]));
  quality4:=sum(data.aux.aux.get('Coherence')[(xnmax-1):(xnmax+1)])/3
  quality5:=meanv(data.aux.aux.get('CC-Beam',1))[xnmax]/meanv(abs(data.aux.aux.get('FieldStrength',1)))[xnmax];

  write(file,'rmsF[',i,']:=',rmsF,'\n',sep='');
  write(file,'rmsC[',i,']:=',rmsC,'\n',sep='');
  write(file,'rmsX[',i,']:=',rmsX,'\n',sep='');
  write(file,'quality1[',i,']:=',quality1,'\n',sep='');
  print 'Quality (1) of peak (CC-beam/P-beam) = ',quality1;
  write(file,'quality2[',i,']:=',quality2,'\n',sep='');
  print 'Quality (2) of peak (CC-beam/P-beam, zero baseline) = ',quality2;
  write(file,'quality4[',i,']:=',quality4,'\n',sep='');
  print 'Quality (4) of peak (Coherence) = ',quality4;
  write(file,'quality5[',i,']:=',quality5,'\n',sep='');
  print 'Quality (5) of peak (CC-beam/FieldStrength) = ',quality5;
  write(file,'offX[',i,']:=',off1,'\n',sep='');
  write(file,'offF[',i,']:=',off2,'\n',sep='');
  write(file,'offC[',i,']:=',off3,'\n',sep='');


###calculate the distance to the antennas
  refant:=data.get('RefAnt',1);
  xy:=data.get('XY',1);
  azel:=data.get('AZEL',1)
  doz:=data.get('DoZ',1);
  antpos:=CalcAntPos(data,seq(data.len),refant,refpos=[xy[2],xy[1],0],phasecenter=T) 
  tdist := ShowerDist(azel,antpos,doz);
  dists := array(-1.,data.len)
  for (j in seq(len(antpos))){
    dists[antpos[j].antenna] := tdist[j];
  };
  write(file,'dists[',i,']:=',as_inputvec(dists),'\n',sep='');
###guess the power in the single antennas
  powers := array(-1.,data.len)
  for (j in seq(data.len)){
    powers[j] := sum(data.aux.get('f(x)',j)[xnmax-4:xnmax+4]^2)
  };
  write(file,'powers[',i,']:=',as_inputvec(powers),'\n',sep='');
###pulseheigt with different selections
  selheights := array(-1.,data.len)
  SelectAxis(plotgui.aux.aux,data.aux.aux,naxis=2,axis='CC-Beam',scale='micro');
  plotgui.aux.aux.absy->state(F);
  for (j in [1:data.len][data.aux.getary('Select')]){
    data.aux.select(j,F); #flag one antenna
    plotgui.aux.aux.replot();
    temp := fitgaussplot(plotgui.aux.aux,floor(res.center_samples-4),floor(res.center_samples+4),center=res.center_samples,height=res.height,width=res.width_samples);
    selheights[j] := temp[1].height;
    data.aux.select(j,T); #unflag the antenna
  };
  write(file,'selheights[',i,']:=',as_inputvec(selheights),'\n',sep='');
  file:=F;
  }#end of the big loop

if (i2!=i1 || allfiles) {
  print 'Executing make_result - creating pipeline data result file',filen3
  cmd.shell(cmd.cd(dirname,exec=F),cmd.run('make_results',exec=F));
  }
}

#-----------------------------------------------------------------------
#Name: crprocess
#-----------------------------------------------------------------------
#Type: Function
#Ret: bool
#Doc: A basic data processing routine for one event loaded into the gui
#
#Par: rec    	- Data record
#Par: plotgui  	- plotgui record
#Par: i = 68   	- Event
#Par: fileno = 1 	- Event table file number to choose appropriate selection
#Par: trange=[-1.89e-6,-1.72e-6] - time range (in secs) where the cosmic ray is expected
#Par: doshifts = T		- shift the data by the guesstimate of 
#				  the trigger delay
#Par:  drange = [1000,10000]        - Range of distances, in [m], for which to
#                                    scan. Use [-1,-1] for one slice at infinity.
#Par:  maxoffset=1.5               - Maximally tolarable offset in degree between
#                                    initial CR position and the one found in the
#                                    map. If the offset is too large the plotpanel
#                                    gui will not be updated.
#Par:  simplesearch=T               - Seach the distance by keeping the position on the sky fixed (using the slow GUI method)
#Par:  useskymapper=F               - Do a full search of the maximum in position, distance and time using the skymapper?
#Par:  remap=F                      - Call skymapper in findpointsourcedist (if fullsearch=T)? If F use old map from previous run (if still defined) - this is mainly used for speeding up debugging
#Ref: guipipeline,crguis,fitgaussplot,findcrdist
#-----------------------------------------------------------------------

crprocess := function(i=68,
                      fileno=1,
                      trange=[-1.89e-6,-1.73e-6],
                      doshifts=T, 
                      remap=T,
                      drange=[1000,10000],
                      maxoffset=1.5,
                      useskymapper=F,
                      simplesearch=T)
{
  ## global variables
  
  global data,data2,plotgui

  ## record to take up the returned results

  result:=[=];

  print 'Processing Event #',i,' Selection Table #',fileno,'trange =',trange
  if (!has_field(data,'aux')) {
    print 'Make sure you have started the CR event GUIs or at least have spawned the data! (see "File" in the menu bar)'
  return F}

  if (i!=globalpar.event_loaded) data.load(i,fileno=fileno,doshifts=doshifts);

  result.mn:=array(0,data.len);
  result.mn2:=array(0,data.len);
  result.stddev:=array(0,data.len);
  rd:=array(0,data.len);

  print '\nFlagging data set #',i

# do some basic flagging of antennas
  result.xn1:=data.getxn(trange[1],'Time');
  result.xn2:=data.getxn(trange[2],'Time');

if (has_field(globalpar.event_flag,data.crid())) {
  for (j in globalpar.event_flag[data.crid()]) {
   print 'Flagged Antenna #',j,' due to observer\'s request!.'
   data.select(j,F);
   data.aux.select(j,F);
  }
}  

  for (j in seq(data.len)) {
    result.mn[j]:=mean(abs(data.get('f(x)',j)));
    result.stddev[j]:=stddev(data.get('f(x)',j));
    result.mn2[j]:=max(abs(data.aux.get('f(x)',j)[result.xn1:result.xn2]));
    xy:=data.get('XY',j); 
    rd[j]:=sqrt(sum(CalcAntPos(data,j,refpos=[xy[2],xy[1],0],phasecenter=T)[1].position^2));
    if (result.mn[j]<10) {
      data.select(j,F);
      data.aux.select(j,F);
      print 'Flagged Antenna #',j,' due to lack of signal.'
    }
  }

print 'Mean flux of antennas:    <f(x)>     =',roundn(result.mn,2)
print 'RMS  flux of antennas: RMS(f(x))     =',roundn(result.stddev,2)
print 'Max peak in time slot: Max(f(x1-x2)) =',roundn(result.mn2,2)


#Check whether event hits the muon tunnel
  xy:=data.get('XCYC');
  if (abs(xy[1])<15 && xy[2]>5) {
    print 'Dangerously close to muon tunnel! Flagging Antennas 4,6,8.'
    for (j in [4,6,8]) {data.select(j,F);data.aux.select(j,F)};
  }

result.flag:=seq(data.len)[data.flag()];
result.m:=mean(result.mn2[data.flag()]);
result.s:=stddev(result.mn2[data.flag()]);
print 'Mean Peak of all antennas: Max =',result.m,'+/-',result.s

for (j in seq(data.len)) {
      if (result.mn2[j]>result.m+2.2*result.s) {
        data.select(j,F);
        data.aux.select(j,F);
        print 'Flagged Antenna #',j,' since it was too spiky.'
      }
}

result.meanr:=mean(rd[data.flag()]);
print 'Mean Radius =',result.meanr;

print '\nOptimizing distance for data set #',i

if (simplesearch) { 
  findcrdist(drange=drange,xrange=trange);  
  result.fmax.diroff:=-1;
}

if (useskymapper) {print 'Attention: the skymapper assumes that the peak is within 1 sample of -1.8 mus!! and the results from findcrdist are not autmatically taken into account. You need to manually modify globalpar.event_shifts'}

if (useskymapper) {
  selectedantennas := seq(data.aux.len)[data.aux.select()]

  result.fmax := findpointsourcedist(data,
                                     drange=drange,
                                     dstep=250,
                                     xrange=trange,
                                     index=selectedantennas,
                                     remap=remap,
                                     maxoffset=maxoffset);
  print 'Return from find pointsourcedist.'
}
return result
}

#-------------------------------------------------------------------------------
#Name: phase_up
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#-------------------------------------------------------------------------------

phase_up := function(ref rec,inarray,index,AZ=0,ZE=0,power=F) {
  erg := [=];
  #form beam toward shower and fit gaussian to time-series 
  apo := CalcAntPos(rec,seq(rec.len),index[1],1);
  samplerate:=rec.headf('Samplerate',index[1])*rec.headf('SamplerateUnit',index[1]);
  delays := round(angle2delay(apo,(AZ/pi*180),(90-ZE/pi*180),F)*samplerate)
print delays;
  siz := len(inarray[1]);
  shary := array(0,siz);
  shifted := [=];
  if (!power) {
    for (i in index) {
      shifted[as_string(i)] := array(0,siz);
      if (delays[i]<0) {
        shary[(1-delays[i]):siz] := shary[(1-delays[i]):siz] + 
                                inarray[i][1:(siz+delays[i])];
	shifted[as_string(i)][(1-delays[i]):siz] := inarray[i][1:(siz+delays[i])];
      } else {
        shary[1:(siz-delays[i])] := shary[1:(siz-delays[i])] +
                                inarray[i][(1+delays[i]):siz];
	shifted[as_string(i)][1:(siz-delays[i])] := inarray[i][(1+delays[i]):siz];
      };
    };
  } else {
    for (i in index) {
      if (delays[i]<0) {
        shary[(1-delays[i]):siz] := shary[(1-delays[i]):siz] + 
                                abs(inarray[i][1:(siz+delays[i])]);
      } else {
        shary[1:(siz-delays[i])] := shary[1:(siz-delays[i])] +
                                abs(inarray[i][(1+delays[i]):siz]);
      };
    };
  };
  shary := shary/len(index);
  erg.shary := shary;
  erg.shfit := fitgauss(shary,cregion=[round(len(shary)*.5),round(len(shary)*0.7)]);
  erg.shifted := shifted;
  erg.delays := delays;
  return erg
};

#-------------------------------------------------------------------------------
#Name: phase_up_tab
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#-------------------------------------------------------------------------------

phase_up_tab := function(table, evind,index,AZ=0.,ZE=180.,power=F){
  oldmetafilename := evmeta.metafilename;
  bla := jdr_to_meta(data,table.GT[evind]);
  if (oldmetafilename != evmeta.metafilename) {
    input_event(data,table.filename[evind])
  };
  if (ZE>90) {
    if (ZE != 180.) {
      print 'zenitangle > 90 deg -> using shower direction';
    };
    AZ := table.AZ[evind];
    ZE := table.ZE[evind];
  } else {
    AZ := AZ*pi/180;
    ZE := ZE*pi/180;
  };
  return phase_up(data,table.filtered[evind],index,AZ,ZE,power=power);
};

#-------------------------------------------------------------------------------
#Name: beam_data
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#-------------------------------------------------------------------------------

beam_data := function(delays,index,reset=F){
  if (reset) {
    for (i in seq(len(index))) {
      data.puthead(data.headf('DefPresync',index[i]),'presync',index[i]);
    };
    return T;
  };
  if(len(delays) != len(index)) {
    print 'len(delays) != len(index)';
    return F;
  };
  for (i in seq(len(index))) {
    if (!has_field(data.get('Header',index[i]),'DefPresync')) {
      data.puthead(data.head('presync',index[i]),'DefPresync',index[i]);
    };
    data.puthead((data.headf('DefPresync',index[i])+delays[i]),
                 'presync',index[i]);
  };
  return T;
};

#-------------------------------------------------------------------------------
#Name: toeps
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#-------------------------------------------------------------------------------

toeps := function(power,n=3,Gain=1.9){
  aeps :=cMeanFilter(sqrt(power/(2048^2)/50),n)*sqrt(0.4386/Gain*n)*1e6
  return aeps;
};

#-------------------------------------------------------------------------------
#Name: sumants_tab
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#-------------------------------------------------------------------------------

sumants_tab := function(table, sel, index,n=3,badind=[],eps=F,maxmin=F){
  if (len(table.filtered) != len(sel)) {
      print 'len(table.filtered) != len(sel)';
      return;
  };
  a := table.filtered[1][index]*0.;
  b :=0;
  l := len(a); 
  ergstd := array(0.,l)
  ergmax := array(0.,l,2)
  ergmin := array(1e6,l,2)
  indices := seq(len(sel))[sel];
  for (i in indices) {
    if (sum(i==badind)==0) {
      if (len(table.filtered[i])<index){
        print len(table.filtered[i]),' ',i;
      }else{
        a:=a+abs(table.filtered[i][index]);
        b:=b+1
        if(maxmin){
          ergstd := ergstd+abs(table.filtered[i][index])^2;
          ergmax[,2] := abs(table.filtered[i][index]);
          tmp := global_lopestools_client->arrayminmax(ergmax);
          ergmax[,1] := tmp.max;
          ergmin[,2] := abs(table.filtered[i][index]);
          tmp := global_lopestools_client->arrayminmax(ergmin);
          ergmin[,1] := tmp.min;
        };
      };
    };
  };
  if (eps) {
    erg := cMeanFilter((a/b/(2048^2)/50),n)*sqrt(0.4386/1.9*n)*1e6
  } else {
    erg := cMeanFilter((a/b),n);
  };
  if(maxmin){ 
    aus := [=];
    aus.erg := erg;
    aus.stddev := sqrt(cMeanFilter((ergstd/b),n)-(cMeanFilter((a/b),n)^2));
    aus.max := cMeanFilter(ergmax[,1],n);
    aus.min := cMeanFilter(ergmin[,1],n);
    return aus;
  };
  return erg;
};

#-------------------------------------------------------------------------------
#Name: doublefit_tab
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#-------------------------------------------------------------------------------

doublefit_tab := function(table,normfile='edep-vergleich-fiterg.sos'){
  edsumtab := [1,2,3,4,0,7,5,6,0,0];
  normdata := read_value(normfile);
  size := len(table.filtered);
  table.dbfilt := [=];
  for (ind in seq(size)) {
    table.dbfilt[ind] := [=];
    for (ant in as_integer(field_names(normdata))){
      edsum := table.edepsum[ind,edsumtab[ant]];
      for (i in seq(len(normdata[as_string(ant)]))){
        if (T) {break;};
      };
    };
  };
  
};

#-------------------------------------------------------------------------------
#Name: clean_fits
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#-------------------------------------------------------------------------------

clean_fits := function(inrec){
  pattern := (inrec.fiterg.center<32600)|(inrec.fiterg.center>32700);
  inrec.fiterg.height[pattern] := 0;
  inrec.fiterg.width[pattern] := 0;
  pattern := (inrec.shfit.center<32600)|(inrec.shfit.center>32700);
  inrec.shfit.height[pattern] := 0;
  inrec.shfit.width[pattern] := 0;
  return inrec;
};

#-------------------------------------------------------------------------------
#Name: maketab
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#-------------------------------------------------------------------------------

maketab := function(rec){
  fd := open('> laber.tab');
  write(fd,'#run evnum time AZ ZE XC YC Size lMuo filename (24x)edep (6x)edepsum (4x)height (4x)width (4x)center beamheight beamwidth beamcenter');
  for (i in seq(len(rec.run))){
    write(fd, paste(as_string(rec.run[i]), as_string(rec.evnum[i]), as_string(rec.GT[i]+rec.MMN[i]/1e9), as_string(rec.AZ[i]), as_string(rec.ZE[i]), as_string(rec.XC[i]), as_string(rec.YC[i]), as_string(rec.SIZE[i]), as_string(rec.LMUO[i]), as_string(rec.filename[i]), as_string(rec.edep[i,]), as_string(rec.edepsum[i,]), as_string(rec.fiterg.height[i,]), as_string(rec.fiterg.width[i,]), as_string(rec.fiterg.center[i,]), as_string(rec.shfit.height[i]), as_string(rec.shfit.width[i]), as_string(rec.shfit.center[i]) ));
  };
};

#-------------------------------------------------------------------------------
#Name: sumants
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par:  rec      - Reference to the data structure.
#Par:  kretarec - 
#Par:  range    - 
#-------------------------------------------------------------------------------

sumants := function(ref rec, kretarec, range=[1,1e100]){
  input_event(rec,kretarec.filename[1]);
print rec.len;
  noind := rec.len;
  siz := len(kretarec.filename);
  range[1] := as_integer(max(min(range[1],siz),1));
  range[2] := as_integer(max(min(range[2],siz),1));
  kretarec.fiterg := [=];
  kretarec.fiterg.height := array(0.,siz,noind);
  kretarec.fiterg.width  := array(0.,siz,noind);
  kretarec.fiterg.center := array(0.,siz,noind);
  kretarec.shfit.height := array(0.,siz);
  kretarec.shfit.width  := array(0.,siz);
  kretarec.shfit.center := array(0.,siz);

kretarec.fit := array(0.,65536);
kretarec.ant1 := array(0.,65536);
kretarec.ant2 := array(0.,65536);
kretarec.ant3 := array(0.,65536);
kretarec.ant4 := array(0.,65536);
kretarec.ant5 := array(0.,65536);
kretarec.ant6 := array(0.,65536);
kretarec.ant7 := array(0.,65536);
kretarec.ant8 := array(0.,65536);

kretarec.fitmask := array(0.,65536);
kretarec.mask1 := array(0.,65536);
kretarec.mask2 := array(0.,65536);
kretarec.mask3 := array(0.,65536);
kretarec.mask4 := array(0.,65536);
kretarec.mask5 := array(0.,65536);
kretarec.mask6 := array(0.,65536);
kretarec.mask7 := array(0.,65536);
kretarec.mask8 := array(0.,65536);

  if (range[2]>range[1]) {
  for (i in [range[1]:range[2]]){
print 'working on index',i;
    atime := time();
    if(kretarec.filename[i]=='') next;
    perg := get_event_param(rec, kretarec.filename[i], AZ=kretarec.AZ[i], 
                            ZE=kretarec.ZE[i],calcpair=F,
                            debug=T,dofit=T);



win := abs(real(perg.shary))<4;
kretarec.fit := kretarec.fit + win*real(perg.shary)^2;
kretarec.fitmask := kretarec.fitmask + win;

win := abs(real(perg.filtered[1]))<4;
kretarec.ant1 := kretarec.ant1 + win*real(perg.filtered[1])^2;
kretarec.mask1 := kretarec.mask1 + win;
win := abs(real(perg.filtered[2]))<4;
kretarec.ant2 := kretarec.ant2 + win*real(perg.filtered[2])^2;
kretarec.mask2 := kretarec.mask2 + win;
win := abs(real(perg.filtered[3]))<4;
kretarec.ant3 := kretarec.ant3 + win*real(perg.filtered[3])^2;
kretarec.mask3 := kretarec.mask3 + win;
win := abs(real(perg.filtered[4]))<4;
kretarec.ant4 := kretarec.ant4 + win*real(perg.filtered[4])^2;
kretarec.mask4 := kretarec.mask4 + win;
win := abs(real(perg.filtered[5]))<4;
kretarec.ant5 := kretarec.ant5 + win*real(perg.filtered[5])^2;
kretarec.mask5 := kretarec.mask5 + win;
win := abs(real(perg.filtered[6]))<4;
kretarec.ant6 := kretarec.ant6 + win*real(perg.filtered[6])^2;
kretarec.mask6 := kretarec.mask6 + win;
if (len(perg.filtered)>6) {
  win := abs(real(perg.filtered[7]))<4;
  kretarec.ant7 := kretarec.ant7 + win*real(perg.filtered[7])^2;
  kretarec.mask7 := kretarec.mask7 + win;
  win := abs(real(perg.filtered[8]))<4;
  kretarec.ant8 := kretarec.ant8 + win*real(perg.filtered[8])^2;
  kretarec.mask8 := kretarec.mask8 + win;
};

print max(real(perg.filtered[1])^2),max(real(perg.filtered[2])^2),max(real(perg.filtered[3])^2),max(real(perg.filtered[4])^2)

    for (j in seq(noind)){
      kretarec.fiterg.height[i,j] := perg.fiterg[j].height;
      kretarec.fiterg.width[i,j] := perg.fiterg[j].width;
      kretarec.fiterg.center[i,j] := perg.fiterg[j].center;
    };
    kretarec.shfit.height[i] := perg.shfit.height;
    kretarec.shfit.width[i]  := perg.shfit.width;
    kretarec.shfit.center[i] := perg.shfit.center;


    tdiff := time()-atime;
print 'needed ',tdiff,'seconds for calculation';
  };
  } else { print "Not working on only one index!";};

  return kretarec;
};

#-------------------------------------------------------------------------------
#Name: edepbins
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par:  tab     - 
#Par:  channum - 
#Par:  edepnum - 
#Par:  badev   - 
#Par:  bins    - 
#-------------------------------------------------------------------------------

edepbins := function(tab,channum,edepnum,field='RFIpow',badev=F,
     bins=[1000,2000,3000,4000,5500,7000,10000,15000,20000,25000,31000,40000]){
  astring:=spaste('ant',as_string(channum));
  erg := [=];
  erg[astring]:=T;
  erg.all := array(0,len(bins),9)
  for (i in seq(len(bins)-1)){
    selstring := spaste('mean',as_string(as_integer(bins[i]/1000)));
    stdstring := spaste('std',as_string(as_integer(bins[i]/1000)));
    medstring := spaste('medstd',as_string(as_integer(bins[i]/1000)));
    maxstring := spaste('max',as_string(as_integer(bins[i]/1000)));
    minstring := spaste('min',as_string(as_integer(bins[i]/1000)));
    datastring := spaste('data',as_string(as_integer(bins[i]/1000)));
    sel := (tab.edepsum[,edepnum]>=bins[i] & tab.edepsum[,edepnum]<bins[i+1])&!badev;
    print 'Channel:',channum,'Edep:',edepnum,'Bin:',bins[i],bins[i+1],' -> ',sum(sel);
    if (field == 'RFIpow'){
      temp := tab.RFIpow[sel,channum,2];
    } else {
      temp := tab[field][sel,channum];
    };
    erg[selstring] := mean(temp);
    erg[stdstring] := stddev(temp);
    erg[maxstring] := max(temp);
    erg[minstring] := min(temp);
    erg[medstring] := median(temp);
    erg[datastring] := temp;
    l := len(temp);
    q1ind := order(temp)[max(as_integer(l/4),1)];
    q3ind := order(temp)[max(as_integer(l/4*3),1)];  
    erg.all[i,] := [((bins[i]+bins[i+1])/2),sum(sel),erg[selstring],erg[stdstring],erg[maxstring],erg[minstring],erg[medstring],temp[q1ind],temp[q3ind]];
  };
  i:=len(bins);
  selstring := spaste('mean',as_string(as_integer(bins[i]/1000)));
  stdstring := spaste('std',as_string(as_integer(bins[i]/1000)));
  medstring := spaste('medstd',as_string(as_integer(bins[i]/1000)));
  maxstring := spaste('max',as_string(as_integer(bins[i]/1000)));
  minstring := spaste('min',as_string(as_integer(bins[i]/1000)));
  datastring := spaste('data',as_string(as_integer(bins[i]/1000)));
  sel := (tab.edepsum[,edepnum]>=bins[i])&!badev;
  print 'Channel:',channum,'Edep:',edepnum,'Bin:',bins[i],'inf  -> ',sum(sel);
  if (field == 'RFIpow'){
    temp := tab.RFIpow[sel,channum,2];
  } else {
    temp := tab[field][sel,channum];
  };
  erg[selstring] := mean(temp);
  erg[stdstring] := stddev(temp);
  erg[maxstring] := max(temp);
  erg[minstring] := min(temp);
  erg[medstring] := median(temp);
  erg[datastring] := temp;
  l := len(temp);
  q1ind := order(temp)[max(as_integer(l/4),1)];
  q3ind := order(temp)[max(as_integer(l/4*3),1)];  
  erg.all[i,] := [(1.5*bins[i]-.5*bins[i-1]),sum(sel),erg[selstring],erg[stdstring],erg[maxstring],erg[minstring],erg[medstring],temp[q1ind],temp[q3ind]];
  return erg;
};

#-------------------------------------------------------------------------------
#Name: edepbins2
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par:  tab     - 
#Par:  channum - 
#Par:  edepnum - 
#Par:  n       - 
#Par:  bins    - 
#-------------------------------------------------------------------------------

edepbins2 := function(tab,channum,edepnum,n=7,
     bins=[1000,2000,3000,4000,5500,7000,10000,15000,20000,25000,31000,40000]){
  astring:=spaste('ant',as_string(channum));
  erg := [=];
  erg[astring]:=T;
  for (i in seq(len(bins)-1)){
    selstring := spaste('eps',as_string(as_integer(bins[i]/1000)));
    stdstring := spaste('std',as_string(as_integer(bins[i]/1000)));
    maxstring := spaste('max',as_string(as_integer(bins[i]/1000)));
    minstring := spaste('min',as_string(as_integer(bins[i]/1000)));
    sel := (tab.edepsum[,edepnum]>=bins[i] & tab.edepsum[,edepnum]<bins[i+1]);
    print 'Channel:',channum,'Edep:',edepnum,'Bin:',bins[i],bins[i+1],' -> ',sum(sel);
    tmp := sumants_tab(tab,sel,channum,n=n,maxmin=T);
    erg[selstring] := tmp.erg;
    erg[stdstring] := tmp.stddev;
    erg[maxstring] := tmp.max;
    erg[minstring] := tmp.min;
  };
  i:=len(bins);
  selstring := spaste('eps',as_string(as_integer(bins[i]/1000)));
  stdstring := spaste('std',as_string(as_integer(bins[i]/1000)));
  maxstring := spaste('max',as_string(as_integer(bins[i]/1000)));
  minstring := spaste('min',as_string(as_integer(bins[i]/1000)));
  sel := (tab.edepsum[,edepnum]>=bins[i]);
  print 'Channel:',channum,'Edep:',edepnum,'Bin:',bins[i],'inf  -> ',sum(sel);
  tmp := sumants_tab(tab,sel,channum,n=n,maxmin=T);
  erg[selstring] := tmp.erg;
  erg[stdstring] := tmp.stddev;
  erg[maxstring] := tmp.max;
  erg[minstring] := tmp.min;
  

  return erg;
}

#-------------------------------------------------------------------------------
#Name: get_det_dist
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par:  xc   - 
#Par:  yc   - 
#Ret:  dist - 
#-------------------------------------------------------------------------------

# 6.5 19.5 32.5 45.5 58.5 71.5 84.5 97.5
get_det_dist := function(xc,yc) {
ex:=[-97.5,-84.5,-84.5,-97.5, -71.5,-58.5,-58.5,-71.5, -45.4,-32.5,-32.5,-45.5,
  -19.5,-6.5,-6.5,-19.5,   -45.4,-32.5,-32.5,-45.5, -19.5,-6.5,-6.5,-19.5,
  -71.5,-58.5,-58.5,-71.5, -45.4,-32.5,-32.5,-45.5, -19.5,-6.5,-6.5,-19.5,
  -71.5,-58.5,-58.5,-71.5, -19.5,-6.5,-6.5,-19.5,   -45.4,-32.5,-32.5,-45.5];
ey:=[6.5,6.5,19.5,19.5, 32.5,32.5,45.5,45.5, 6.5,6.5,19.5,19.5, 
  32.5,32.5,45.5,45.5,    58.5,58.5,71.5,71.5,    84.5,84.5,97.5,97.5, 
  -19.5,-19.5,-6.5,-6.5,  -19.5,-19.5,-6.5,-6.5,  -19.5,-19.5,-6.5,-6.5,
  6.5,6.5,19.5,19.5,      6.5,6.5,19.5,19.5,      32.5,32.5,45.5,45.5];
 
  dist := sqrt((ex-xc)^2+(ey-yc)^2);
  return dist;
};

#-------------------------------------------------------------------------------
#Name: ed_dist_func
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par:  record - 
#Par:  distar - 
#Ret:  erg    - 
#-------------------------------------------------------------------------------

ed_dist_func := function(record,distar){
  if (shape(record.edep)[2] != len(distar)) {
    print '(shape(record.edep)[2] != len(distar)) ***Aborting!';
    return F;
  };
  erg := array(0.,shape(record.edep)[1],shape(record.edep)[2]);
  for (i in seq(shape(record.edep)[1]) ) {
    erg[i,] := record.edep[i,]*distar;
  };
  return erg;
};

#-------------------------------------------------------------------------------
#Name: collaps_2daxis
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par:  inarray - 
#Par:  axnum   - 
#Par:  fun     - 
#Ret:  erg     - 
#-------------------------------------------------------------------------------

collaps_2daxis := function(inarray,axnum,fun){
  shap := shape(inarray);
  if (axnum==1) {
    erg := array(0.,shap[2]);
    for (i in seq(shap[2])) {
      erg[i] := fun(inarray[,i]);
    };
  } else if (axnum == 2) {
    erg := array(0.,shap[1]);
    for (i in seq(shap[1])) {
      erg[i] := fun(inarray[i,]);
    };
  } else {
    print 'only 2-dim arrays supported!';
    return F;
  };
  return erg;
};


#-------------------------------------------------------------------------------
#Name: plotfields
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par:  rec - 
#Par:  erg - 
#Par:  pg  - 
#-------------------------------------------------------------------------------

plotfields := function(rec,erg,pg){
  pg.subp(4,3);
  yary := ([1:65536]-32768)/80;
  yary := yary[32525:32775];
  for (i in seq(len(erg.GT))) {
    perg := get_event_param(rec, erg.filename[i], AZ=erg.AZ[i], 
                          ZE=erg.ZE[i],calcpair=F,
                          debug=T,dofit=F);
    ary := real(perg.filtered[1]);
    ary := ary/stddev(ary);
    ary := cMeanFilter((ary[32525:32775])^2,3);
    pg.plotxy(yary,ary);
    pg.sci(1);
    pg.lab('Time in us','Power',paste('Antenne 1 Run',as_string(erg.run[i]),'Event',as_string(erg.evnum[i])));
    ary := real(perg.filtered[2]);
    ary := ary/stddev(ary);
    ary := cMeanFilter((ary[32525:32775])^2,3);
    pg.plotxy(yary,ary);
    pg.sci(1);
    pg.lab('Time in us','Power',paste('Antenne 2  XC:',as_string(erg.XC[i]),'YC:',as_string(erg.YC[i])));
    ary := real(perg.filtered[3]);
    ary := ary/stddev(ary);
    ary := cMeanFilter((ary[32525:32775])^2,3);
    pg.plotxy(yary,ary);
    pg.sci(1);
    pg.lab('Time in us','Power',paste('Antenne 3   ZE',as_string(erg.ZE[i]/pi*180),'AZ',as_string(erg.AZ[i]/pi*180)));
    ary := real(perg.filtered[4]);
    ary := ary/stddev(ary);
    ary := cMeanFilter((ary[32525:32775])^2,3);
    pg.plotxy(yary,ary);
    pg.sci(1);
    pg.lab('Time in us','Power',paste('Antenne 4  log(Ne)',as_string(log(erg.SIZE[i])),'log(Nmu)',as_string(log(erg.LMUO[i]))));

    ary := real(perg.filtered[5]);
    ary := ary/stddev(ary);
    ary := cMeanFilter((ary[32525:32775])^2,3);
    pg.plotxy(yary,ary);
    pg.sci(1);
    pg.lab('Time in us','Power',paste('Antenne 5 Run',as_string(erg.run[i]),'Event',as_string(erg.evnum[i])));
    ary := real(perg.filtered[6]);
    ary := ary/stddev(ary);
    ary := cMeanFilter((ary[32525:32775])^2,3);
    pg.plotxy(yary,ary);
    pg.sci(1);
    pg.lab('Time in us','Power',paste('Antenne 6  XC:',as_string(erg.XC[i]),'YC:',as_string(erg.YC[i])));
    ary := real(perg.filtered[7]);
    ary := ary/stddev(ary);
    ary := cMeanFilter((ary[32525:32775])^2,3);
    pg.plotxy(yary,ary);
    pg.sci(1);
    pg.lab('Time in us','Power',paste('Antenne 7   ZE',as_string(erg.ZE[i]/pi*180),'AZ',as_string(erg.AZ[i]/pi*180)));
    ary := real(perg.filtered[8]);
    ary := ary/stddev(ary);
    ary := cMeanFilter((ary[32525:32775])^2,3);
    pg.plotxy(yary,ary);
    pg.sci(1);
    pg.lab('Time in us','Power',paste('Antenne 8  log(Ne)',as_string(log(erg.SIZE[i])),'log(Nmu)',as_string(log(erg.LMUO[i]))));
  };
};

#-------------------------------------------------------------------------------
#Name: movefiles
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par:  kretarec  - 
#Par:  targetdir - 
#-------------------------------------------------------------------------------

movefiles := function(kretarec,targetdir){
  for (f in kretarec.filename) {
    if (f != '') {
      shell(paste('cp',f,targetdir));
    };
  };
};


#-------------------------------------------------------------------------------
#Name: gettvphase
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par:  files - 
#-------------------------------------------------------------------------------

gettvphase:=function(files){
  global dphi,dphim,dev,sdev
  nant:=8
  n:=0
  dphi:=array(0,len(files),nant)
  dphim:=array(0,nant)
  sdev:=dphim
  dev:=dphi
  for (f in files) {
    n+:=1
    input_event(data,f)
    print 'Read file #',n,f,'#Antennas=',data.len
    data.setphase();
    for (i in seq(2,nant)) {
      p:=data.get('PhaseDiff',i)[seq(18174,18195,1)];
      mn:=(max(p)+min(p))/2;
      p1:=mean(p[p<mn]);
      p2:=mean(p[p>mn]);
      if (p2-p1<180) {
        dphi[n,i]:=(p1+p2)/2
#        if (n>1 && (dphi[n,i]-dphi[n,i]>180)) dphi[n,i]+:=360
        if (dphi[n,i]<0) dphi[n,i]+:=360
      } else {
        dphi[n,i]:=(p1+360+p2)/2
      }
      dphim[i]+:=dphi[n,i];
    printf('%s%3.2i %s%3.2i %s%7.2f %s%7.2f %s%7.2f %s%7.2f\\n','#',n,'Ant:',i,'p1=',p1,'p2=',p2,'diff=',p2-p1,'phase=',dphi[n,i]);
    };
  };
  nsig:=2
  dphim/:=n
  if (n>1) {
    for (j in seq(nant)) sdev[j]:=stddev(dphi[,j])
    print ' Preflag Average and Standarddeviations:'; 
    printf('%7.2f, ,',dphim); print ' '
    print ' '; printf('%7.2f, ,',sdev); print ' '
    for (j in seq(nant)) {for (i in seq(n)) dev[i,j]:=abs(dphi[i,j]-dphim[j])/sdev[j]}
    for (j in seq(nant)) dphim[j]:=mean(dphi[,j][dev[,j]<nsig])
    for (j in seq(nant)) sdev[j]:=stddev(dphi[,j][dev[,j]<nsig])
  }
print 'flag:',dev<nsig
print 'dev:',dev
print 'Result (horizontal: antennas, vertical: events):'
  for (j in seq(n)) {printf('%7.2f, ,',dphi[j,]);print ' '}
  print 'Postflag Average and Standarddeviations:'; 
  printf('%7.2f, ,',dphim); print ' '
  print ' '; printf('%7.2f, ,',sdev); print ' '
};


#-------------------------------------------------------------------------------
#Name: gen_collect_script
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  ../..
#Par:  files     - 
#Par:  targetdir - 
#Par:  outfile   - 
#-------------------------------------------------------------------------------

gen_collect_script := function(files,
                               targetdir='tmp',
                               outfile='cpfile.sh')
{
  fd := open(paste('>',outfile));
  for (ff in files){
    fname := ff~globalconst.FilenameExtractor;
    parts := split(fname,'.')
#    print fname,parts;
    write(fd,spaste('cp ',parts[1],'/',parts[2],'/',parts[3],'/',fname,' ',targetdir));
  };
};



#-----------------------------------------------------------------------
#Name: get_cluster_times
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Get the relative arival times of the shower at the KASCADE clusters
#
#Par:  rec =  - Data record to identify dataset
#Ret:  clusterdelays - 
#Ref:  calcdelay
#-----------------------------------------------------------------------

get_cluster_times := function(ref rec)
{
  global kpos;

  xy   :=rec.get('XY');
  azel :=rec.get('AZEL')
  distance:=rec.get('Distance');
  doz  :=rec.get('DoZ');
  kpos := get_cluster_pos(refpos=[xy[2],xy[1],0]);

  l := len(kpos);
  clusterdelays := array(0.,l);
  for (i in seq(l)){
    clusterdelays[i] := calcdelay(azel,kpos[i],doz,distance=distance)
  };
  return clusterdelays;
};

#-----------------------------------------------------------------------
#Name: get_cluster_pos
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Get the positions of the KASCADE clusters relative to the shower
#      center.
#
#Par:  refpos = [0,0,0]	- position of the shower center.
#Ret:  erg
#-----------------------------------------------------------------------

get_cluster_pos := function(refpos=[0,0,0]){
  kascadeX := [-78,-26, 26, 78,-78,-26, 26, 78,-78,-26,26,78,-78,-26,26,78];
  kascadeY := [-78,-78,-78,-78,-26,-26,-26,-26, 26, 26,26,26, 78, 78,78,78];
  angle := -15.;
  erg := [=];
  for (i in seq(len(kascadeX))){
    pos :=  mx.rotate([kascadeX[i],kascadeY[i]],angle,T)
    erg[i] := [=];
    erg[i].position := [pos[2],pos[1],0]-refpos;
  };
  return erg;  
};

#-----------------------------------------------------------------------
#Name: gen_tim_calibration
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Generate a calibration file for Tims monte carlo simulation.
#
#Par:  outfile =  - filename of the generated file
#-----------------------------------------------------------------------

gen_tim_calibration := function(outfile){
  wwdata := read_value('wainwright.sos');
  wwsel := (wwdata.freq>0&wwdata.freq<86e6);
  freq := wwdata.freq[wwsel];
  calary := array(0.,len(freq));
 ######### Gain der Elektronik und filter
  lcFr := [35e6, 40e6 , 43e6, 44e6,  45e6, 50e6, 55e6, 60e6,  65e6, 70e6,  75e6,  76e6, 80e6, 85e6]
  lcdB := [3.14, 33.14,52.95, 57.3, 58.25, 57.6, 57.4, 56.4, 54.95, 53.62, 51.7, 49.05, 15.8,-14.2]
  donemask := array(F,len(freq));
  erg := array(0,len(freq)); 
  for (i in seq(len(lcFr)-1)){
    lf := lcFr[i];     
    hf := lcFr[i+1];
    lg := lcdB[i];  
    hg := lcdB[i+1];
    filt := ((freq >= lf) & (freq < hf)); 
    donemask := filt | donemask;  
    erg := erg+ (lg+ (hg-lg)*(freq-lf)/(hf-lf))*filt; #linear interpolation 
  };
  erg := erg -30*(!donemask)
  calary := 10^(erg/20)
 ######## Umrechnung Feldstaerke->Spannung
#V = sqrt(gain)/Freq * 21793509.5[m/s] * Feld
  calary := calary*21793509.5*sqrt(globalpar.antennagain)/freq

 ######## Umrechnung Spannung->ADC-Counts
  calary := calary*2048

 ######## Phase
 wwphase := phase(wwdata.S21[wwsel]);
 calary := calary*exp(1.i*wwphase/180*pi);
 ######## output
  fd := open(spaste('>', outfile));
  for (i in seq(len(freq))){
    write(fd,paste(as_string(freq[i]),as_string(real(calary[i])),as_string(imag(calary[i]))))# ,as_string(10*log(abs(calary[i]))) ));
  };
  return T;
};
