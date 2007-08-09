########################################################################
#
# functions used to calulate the values for  the LOPES analysis pipeline
#
########################################################################

allFilterFast := function(ref rec, index){
  filter := T;
  for (i in index) {
    cpary := sqrt(rec.get('CalPower',i));
    for (j in [1:3]) {
      weight := peakary(cpary*filter,4,0);
      filter := ((!weight) & filter);
      if (sum(weight)<10) break;
    };
  };
  filter[1:(as_integer(len(filter)*0.002)+1)] := F;
  filter[(as_integer(len(filter)*0.998)+1):len(filter)] := F;
  return filter;
};


#-----------------------------------------------------------------------
#Name: generate_data_record
#-----------------------------------------------------------------------
#Type: Function
#Doc: calculate the pulse-parameters for the radio-data and add them to
#     the evtab (aka the "kreta-record")
#
#Par: rec =		- Data record to identify dataset
#Par: evtab =		- merged table with the information from kreta and
#			  the filenames of the corresponding eventfiles
#Par: range = [1,1e100]	- first and last event from evtab to work on
#Par: keepfilt = 0	- 0: don't keep filtered data
#			  1: keep filtered data around the trigger
#			  2: keep all fintered data
#Ret: extended evtab-table
#-----------------------------------------------------------------------
generate_data_record := function(ref rec, evtab, range=[1,1e100], keepfilt=0){
  loadcr(rec,evtab,i=1,disp=F,keepdir=T,doflag=F);
  noind := rec.len;
  maxind := globalpar.maxants;
  siz := len(evtab.filename);
  range[1] := as_integer(max(min(range[1],siz),1));
  range[2] := as_integer(max(min(range[2],siz),1));
  if (keepfilt>0) {
    evtab.filtered := [=];
    evtab.shfilt := [=];
    if (keepfilt==1) {
      evtab.filtrange := (([1:65536]-32768)/80);
      evtab.filtrange := evtab.filtrange[32000:33000];
      for (i in seq(siz)) {
        evtab.filtered[i] := [=];
        evtab.shfilt[i] := 0.;
      };
    };
  };
  evtab.flag :=  array(T,siz,maxind);
  evtab.corrdelays :=  array(0.,siz,maxind);
  evtab.delays :=  array(0.,siz,maxind);
  evtab.dphi :=  array(0.,siz,maxind,3);
  evtab.RFIpow :=  array(0.,siz,maxind,2);
  evtab.RFImax :=  array(0.,siz,maxind);
  evtab.RFImaxfakt :=  array(0.,siz,maxind);

  meaninter := [=];
  stddevinter := [=];
  ant2edep := [1,2,3,4,0,7,5,6,0,0];
  if (evtab.GT[range[1]]<1.07296227e+09){
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


  starttime := time();
  for (i in [range[1]:range[2]]){
    atime := time();
    if(evtab.filename[i]=='') next;
    loadcr(rec,evtab,i=i,disp=F,keepdir=T,doflag=F); #load the next CR-event
    btime := time();
    phaseerg := phasecmp(rec,disp=F,pipeline=T);       #calculate corrdelays from TV-station
    filter := allFilterFast(rec,seq(noind)); #calculate a frequency filter
    ctime := time();
    beam := 0.; nobeam:=0;
    for (j in seq(noind)){
      fromary := rec.get('CalFFT',j);
      size := rec.getmeta('FFTSize',j);
      ary := FFTtoFx(fromary, size, filter);
      peak := max(abs(ary));
      maxwink := asin(max(abs(imag(ary))/(abs(ary)+(peak/1e5))))/pi*180 
      if (maxwink > 10) print 'get_event_param:  Attention! Large complex angle:',maxwink,'deg';
      filtered := TIM40convFX2Voltage(real(ary),rec,index=j,inv=T);
      if (keepfilt>1) {
        evtab.filtered[i] := filtered;
      } else if (keepfilt>0) {
        evtab.filtered[i][j] := filtered[32000:33000];
      };
      evtab.RFIpow[i,j,1] := sum(filtered[32001:32500]^2)/500.;
      evtab.RFIpow[i,j,2] := sum(filtered[32608:32728]^2)-evtab.RFIpow[i,j,1]*121.;
      evtab.RFImax[i,j] := max((filtered[32608:32728]^2));
      if (ant2edep[j]>0){
        mval := meaninter[j].interpolate(evtab.edepsum[i,(ant2edep[j])])
        sval := stddevinter[j].interpolate(evtab.edepsum[i,(ant2edep[j])])
        evtab.RFImaxfakt[i,j] := (evtab.RFImax[i,j]-mval)/sval;
      };

      if (phaseerg.flag[j]){
        beam := beam+filtered; 
        nobeam := nobeam+1;
      };
    };
    dtime := time();
    beam := beam/nobeam;
    if (keepfilt>1) {
      evtab.shfilt[i] := beam
    } else if (keepfilt>0) {
      evtab.shfilt[i] := beam[32000:33000];
    };
    evtab.delays[i,seq(noind)] := rec.getary('Delay')
    evtab.corrdelays[i,seq(noind)] := rec.getary('Corrdelay');
    evtab.flag[i,seq(noind)] := phaseerg.flag[seq(noind)];
    evtab.dphi[i,seq(noind),1] := phaseerg.dphi[1,seq(noind)]
    evtab.dphi[i,seq(noind),2] := phaseerg.dphi[2,seq(noind)]
    evtab.dphi[i,seq(noind),3] := phaseerg.dphi[3,seq(noind)]

    endtime := time();
    print 'Done with index:',i,'total time:',sprintf('%5.2f',endtime-starttime),'parts:',sprintf('%5.2f',[btime-atime,ctime-btime,dtime-ctime,endtime-dtime]);
    starttime := endtime;
  };

  return evtab;
};


#-----------------------------------------------------------------------
#Name: calculate_more_beams
#-----------------------------------------------------------------------
#Type: Function
#Doc: calculate more types of beams (e.g. cc-beam, Xbeam) and adds them
#     to the evtab
#
#Par: evtab =		- merged table with the information from kreta and
#			  the filenames of the corresponding eventfiles
#Par: fields="ccbeam Xbeam"	- names of the beams to add 
#Par: index = []	- indices of the antennas to use
#
#Ret: extended evtab-table
#-----------------------------------------------------------------------
calculate_more_beams := function(ref table, fields="ccbeam Xbeam",index=[]){
  nev := len(table.GT);
  if (sum('ccbeam' == fields)>0) {
    ccbeam:=T;
    table.ccbeam := [=];
  } else {ccbeam:=F;};
  if (sum('Xbeam' == fields)>0) {
    Xbeam:=T;
    table.Xbeam := [=];
  } else {Xbeam:=F;};
  if (sum('Pbeam' == fields)>0) {
    Pbeam:=T;
    table.Pbeam := [=];
  } else {Pbeam:=F;};

  for (i in seq(nev)){
    nant := len(table.filtered[i]);
    if (nant<1) {next;};
    if (len(index)==0){ idx := seq(nant)[table.flag[i,[1:nant]]];
    } else { idx := index;};
    l := len(idx);
    tary := array(0.,l,len(table.filtered[i][1]));
    for (j in seq(l)){
      tary[j,] := table.filtered[i][idx[j]]; 
    };
    if (ccbeam) {
      corr:=0;
      n:=0;
      for (j in seq(l-1)) {
        for (k in seq(j+1,l)) {
          n+:=1;
          corr+:=tary[j,]*tary[k,];
        };
      };
      table.ccbeam[i] := corr/n;
    };
    if (Xbeam) {
     table.Xbeam[i] := TIM40tary2xbeam(tary);
    };
    if (Pbeam) {
      pb := 0.;
      n:=0;
      for (j in seq(l)) {
        n+:=1;
        pb := pb + tary[j,]^2
      };
      table.Pbeam[i] := pb/n;
    };
  };
  return table;
};


