#######################################################################
#
# Functions and other stuff for the Tektronix Digital Oszilloscopes
#
########################################################################


########################################################################
# CGpipeAccess-Setup
########################################################################

global_CGaccess_client := client(concat_dir(globalconst.LOPESBIND,'CGaccessClient'));

obsrec := [=]
for (obs in obspar.observatories) {
  if (has_field(obspar.obs[obs],'CalTablePath') && 
      is_dir(obspar.obs[obs].CalTablePath[1])){
    obsrec[obs] := obspar.obs[obs].CalTablePath[1];
  };
};
if (!global_CGaccess_client->initPipeline(obsrec)){
  print '###### ERROR!!! ##### Could not initialize the global_CGaccess_client!';
  global_CGaccess_client := F;
};


#-----------------------------------------------------------------------
#Name: TEKHeader
#-----------------------------------------------------------------------
#Type: Function
#Doc: Creates a default header entry for undocumented TEK datafiles.
#Ret: Data header record
#
#-----------------------------------------------------------------------

CGaccessHeader:=function(
	Project='Test-Measurements',
	DataType='CGaccess',
	Filename='test',
    File='testfile',
	ID=globalpar.DataID
){
header:=[Project=Project,
	DataType=DataType,
	Filename=Filename,
    File=File,
	ID=ID];
return header
}




########################################################################
#-----------------------------------------------------------------------
#Name: CGaccess(gen|conv)ZZZ
#-----------------------------------------------------------------------
#Type: Function
#Doc: Converts coordinates from one field to another
#Par: ary		   - array with values to be converted
#Par: rec		   - array with data sets (header is needed)
#Par: index		   - index number of data set
#Par: inv		   - reverse direction of conversion
#Ret: array with converted values
#-----------------------------------------------------------------------

CGaccessGetTime := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  # update the cache if neccessary
  if (!(rec.head('File') == rec.CGAcache.TimeFile)) {
    rec.CGAcache.Time := global_CGaccess_client->GetTime();
    rec.CGAcache.TimeFile := rec.head('File');
  };
  if (start<1) start:=1;
  start := min(start,len(rec.CGAcache.Time));
  if (end<1) end := len(rec.CGAcache.Time);
  end := min(end,len(rec.CGAcache.Time));
  return rec.CGAcache.Time[start:end]
};

CGaccessGetFrequency := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  # update the cache if neccessary
  if (!(rec.head('File') == rec.CGAcache.FrequencyFile)) {
    rec.CGAcache.Frequency := global_CGaccess_client->GetFrequency();
    rec.CGAcache.FrequencyFile := rec.head('File');
  };
  if (start<1) start:=1;
  start := min(start,len(rec.CGAcache.Frequency));
  if (end<1) end := len(rec.CGAcache.Frequency);
  end := min(end,len(rec.CGAcache.Frequency));
  return rec.CGAcache.Frequency[start:end]
};

CGaccessGetFX := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  # update the cache if neccessary
  if (!(rec.head('File') == rec.CGAcache.FXFile)) {
    rec.CGAcache.FX := global_CGaccess_client->GetFX();
    rec.CGAcache.FXFile := rec.head('File');
  };
  ll := shape(rec.CGAcache.FX)[1]
  if (start<1) start:=1;
  start := min(start,ll);
  if (end<1) end := ll;
  end := min(end,ll);
  return rec.CGAcache.FX[[start:end],index]
};

CGaccessGetVoltage := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  # update the cache if neccessary
  if (!(rec.head('File') == rec.CGAcache.VoltageFile)) {
    rec.CGAcache.Voltage := global_CGaccess_client->GetVoltage();
    rec.CGAcache.VoltageFile := rec.head('File');
  };
  ll := shape(rec.CGAcache.Voltage)[1]
  if (start<1) start:=1;
  start := min(start,ll);
  if (end<1) end := ll;
  end := min(end,ll);
  return rec.CGAcache.Voltage[[start:end],index]
};

CGaccessGetFFT := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  # update the cache if neccessary
  if (!(rec.head('File') == rec.CGAcache.FFTFile)) {
    rec.CGAcache.FFT := global_CGaccess_client->GetFFT();
    rec.CGAcache.FFTFile := rec.head('File');
  };
  ll := shape(rec.CGAcache.FFT)[1]
  if (start<1) start:=1;
  start := min(start,ll);
  if (end<1) end := ll;
  end := min(end,ll);
  return rec.CGAcache.FFT[[start:end],index]
};

CGaccessGetCalFFT := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  # update the cache if neccessary
  if (!(rec.head('File') == rec.CGAcache.CalFFTFile)) {
    rec.CGAcache.CalFFT := global_CGaccess_client->GetCalFFT();
    rec.CGAcache.CalFFTFile := rec.head('File');
  };
  ll := shape(rec.CGAcache.CalFFT)[1]
  if (start<1) start:=1;
  start := min(start,ll);
  if (end<1) end := ll;
  end := min(end,ll);
  return rec.CGAcache.CalFFT[[start:end],index]
};

CGaccessGetFilteredFFT := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  # update the cache if neccessary
  if (!(rec.head('File') == rec.CGAcache.FilteredFFTFile)) {
    rec.CGAcache.FilteredFFT := global_CGaccess_client->GetFilteredFFT();
    rec.CGAcache.FilteredFFTFile := rec.head('File');
  };
  ll := shape(rec.CGAcache.FilteredFFT)[1]
  if (start<1) start:=1;
  start := min(start,ll);
  if (end<1) end := ll;
  end := min(end,ll);
  return rec.CGAcache.FilteredFFT[[start:end],index]
};

CGaccessconvFFT2Power := function(ref ary, ref rec,index=1,inv=F, reffield='',...) {
  return (abs(ary)^2);
};

CGaccessGetTCXP := function(ref rec,Ignoreflag=F){
  if (!((rec.head('File') == rec.CGAcache.TCXPFile) &&
      (rec.get('Distance',1) == rec.CGAcache.TCXPDist) &&
      all(rec.get('XY',1) == rec.CGAcache.TCXPxy) &&
      all(rec.get('AZEL',1) == rec.CGAcache.TCXPazel) &&
      (all(rec.select() == rec.CGAcache.TCXPflag)||Ignoreflag) )) {
    rec.CGAcache.TCXPDist := rec.get('Distance',1);
    rec.CGAcache.TCXPxy   := rec.get('XY',1);
    rec.CGAcache.TCXPazel := rec.get('AZEL',1);
    rec.CGAcache.TCXPflag := rec.select(); 
    getrec := [=];
    getrec.Az := rec.CGAcache.TCXPazel[1];
    getrec.El := rec.CGAcache.TCXPazel[2];
    getrec.XC := rec.CGAcache.TCXPxy[1];
    getrec.YC := rec.CGAcache.TCXPxy[2];
    getrec.distance := rec.CGAcache.TCXPDist;
    getrec.FlaggedAnts := rec.CGAcache.TCXPflag;
    outrec :=  global_CGaccess_client->GetTCXP(getrec);       
    rec.CGAcache.FieldStrength := outrec.FieldStrength;
    rec.CGAcache.CCBeam := outrec.CCBeam;
    rec.CGAcache.XBeam := outrec.XBeam;
    rec.CGAcache.PBeam := outrec.PBeam;
    rec.CGAcache.TCXPFile := rec.head('File');
    rec.CGAcache.recentFit := F;
  };
}

CGaccessGetFieldStrength := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  # update the cache if neccessary
  CGaccessGetTCXP(rec,Ignoreflag=T);
  ll := shape(rec.CGAcache.FieldStrength)[1]
  if (start<1) start:=1;
  start := min(start,ll);
  if (end<1) end := ll;
  end := min(end,ll);
  return rec.CGAcache.FieldStrength[[start:end],index]
};

CGaccessGetCCBeam := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  # update the cache if neccessary
  CGaccessGetTCXP(rec);
  ll := len(rec.CGAcache.CCBeam)
  if (start<1) start:=1;
  start := min(start,ll);
  if (end<1) end := ll;
  end := min(end,ll);
  return rec.CGAcache.CCBeam[start:end]
};

CGaccessGetXBeam := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  # update the cache if neccessary
  CGaccessGetTCXP(rec);
  ll := len(rec.CGAcache.XBeam)
  if (start<1) start:=1;
  start := min(start,ll);
  if (end<1) end := ll;
  end := min(end,ll);
  return rec.CGAcache.XBeam[start:end]
};

CGaccessGetPBeam := function(ref ary,ref rec,index=1,inv=F, reffield='',start=-1,end=-1,...) {
  # update the cache if neccessary
  CGaccessGetTCXP(rec);
  ll := len(rec.CGAcache.PBeam)
  if (start<1) start:=1;
  start := min(start,ll);
  if (end<1) end := ll;
  end := min(end,ll);
  return rec.CGAcache.PBeam[start:end]
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

CGAccessgenEmptyRec := function() {
	ci := [=];
	
	ci.Time:=[Units='Seconds', DefAxis='x', Domain='T', Stored=F, 
		Ref='-', Type='real', DomDefault='Tx',
		GenFunc=symbol_value('CGaccessGetTime'),IsInput=T];
	ci.Frequency:=[Units='Hz', DefAxis='x', Domain='F', Stored=F, 
		Ref='-', Type='real', DomDefault='Fx',
		GenFunc=symbol_value('CGaccessGetFrequency')];
	ci.FX:=[Units='Counts', DefAxis='y', Domain='T', Stored=F, 
		Ref='-', Type='real',
        GenFunc=symbol_value('CGaccessGetFX')];
	ci.Voltage:=[Units='Volt', DefAxis='y', Domain='T', Stored=F, 
		Ref='-', Type='real', DomDefault='Ty',
		GenFunc=symbol_value('CGaccessGetVoltage')];
	ci.FFT:=[Units='units', DefAxis='y', Domain='F', Stored=F, 
		Ref='-', Type='complex',
		GenFunc=symbol_value('CGaccessGetFFT')];
	ci.CalFFT:=[Units='units', DefAxis='y', Domain='F', Stored=F, 
		Ref='-', Type='complex',
		GenFunc=symbol_value('CGaccessGetCalFFT')];
	ci.FilteredFFT:=[Units='units', DefAxis='y', Domain='F', Stored=F, 
		Ref='-', Type='complex',
		GenFunc=symbol_value('CGaccessGetFilteredFFT')];
	ci.Power:=[Units='hanno-Watt/Bin', DefAxis='y', Domain='F', Stored=F, 
		Ref='FFT', Type='real',
		ConvFunc=symbol_value('CGaccessconvFFT2Power')];
	ci.CalPower:=[Units='hanno-Watt/Bin', DefAxis='y', Domain='F', Stored=F, 
		Ref='CalFFT', Type='real', DomDefault='Fy',
		ConvFunc=symbol_value('CGaccessconvFFT2Power')];
	ci.FilteredPower:=[Units='hanno-Watt/Bin', DefAxis='y', Domain='F', Stored=F, 
		Ref='FilteredFFT', Type='real',
		ConvFunc=symbol_value('CGaccessconvFFT2Power')];
	ci.FieldStrength:=[Units='Volt/m/MHz', DefAxis='y', Domain='T', Stored=F, 
		Ref='-', Type='real', 
		GenFunc=symbol_value('CGaccessGetFieldStrength')];
	ci.CCBeam:=[Units='Volt/m/MHz', DefAxis='y', Domain='T', Stored=F, 
		Ref='-', Type='real', 
		GenFunc=symbol_value('CGaccessGetCCBeam')];
	ci.XBeam:=[Units='Volt/m/MHz', DefAxis='y', Domain='T', Stored=F, 
		Ref='-', Type='real', 
		GenFunc=symbol_value('CGaccessGetXBeam')];
	ci.PBeam:=[Units='Volt/m/MHz', DefAxis='y', Domain='T', Stored=F, 
		Ref='-', Type='real', 
		GenFunc=symbol_value('CGaccessGetPBeam')];


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
	return rec;
}

input_CGA_event := function(ref rec, file, dir='',index=[]) {
  global obspar; global globalpar
  newdata(rec,type='CGaccess');
  globalpar.DataID+:=1;
  MinBlocksize:=globalpar.BlocksizeForDisplay;
  rec.CGAcache.TimeFile := '';
  rec.CGAcache.FrequencyFile := '';
  rec.CGAcache.FXFile := '';
  rec.CGAcache.VoltageFile := '';
  rec.CGAcache.FFTFile := '';
  rec.CGAcache.CalFFTFile := '';
  rec.CGAcache.FilteredFFTFile := '';
  rec.CGAcache.TCXPFile := '';

  filename := spaste(dir,file);
  evhead := global_CGaccess_client->LoadEvent(filename)
  if (is_record(evhead) && has_field(evhead,'AntennaIDs') && has_field(evhead,'Date')){
    h := CGaccessHeader(File=file);
    h.GT := evhead.Date;
    h.Date:=dq.time([value=(evhead.Date+3506716800),unit='s'],form="ymd");
    h.Time:=dq.time([value=(evhead.Date+3506716800),unit='s']);
    h.Observatory := evhead.Observatory;
    for (i in seq(len(evhead.AntennaIDs)) ) {
      arylen := 2^16;
      local lhead;
      lhead := h;
      lhead.AntennaID := evhead.AntennaIDs[i];
      lhead.AntPos := paste(as_string(evhead.AntPos[i,1]),',',
                      as_string(evhead.AntPos[i,2]),',',as_string(evhead.AntPos[i,3]));
      lhead.Filename := spaste(as_string(evhead.Date),'-',as_string(lhead.AntennaID))
      antind:=rec.newentry(lhead);
      rec.put((-1.*evhead.AntPos[i,]),'Center',antind);
    };
  };
  rec.crid := function(){return as_string(rec.head('GT',1))}
  rec.load:=function(i=1,fileno=-1,doshifts=T){
     global evtab; 
     return loadcr(rec,i=i,fileno=fileno,doflag=F,doshifts=doshifts,useCGA=T)
  };
};
