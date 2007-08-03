#######################################################################
#
# Functions and other stuff tof the event datafiles
#
########################################################################

evmeta.metafilename := '';

#-----------------------------------------------------------------------
#Name: jdr2string
#-----------------------------------------------------------------------
#Doc: convert the JDR to human-readable string (or vice versa)
#-----------------------------------------------------------------------
jdr2string := function(jdr=time(),forward=T){
  if (forward) {
    return (dq.time([value=(jdr+3506716800),unit='s'],form='ymd')~ s/\//./g);
  } else {
    return  dq.convert(dq.quantity(jdr),'s')['value']-3506716800;
  };
};

#-----------------------------------------------------------------------
#Name: include_data_master
#-----------------------------------------------------------------------
#Doc: include the event meta-data master-file
#-----------------------------------------------------------------------
include_data_master := function(ref rec, masterfile='event-master'){
  fstrings := shell(paste('cat',masterfile));
  rec.evmaster := [=];
  i := 0;
  for (line in fstrings) {
    sary := split(line);
    if (sary[1] !~ m/#/) {
      date := dq.quantity(sary[1]);
      if (!is_fail(date)){
        i := i+1;
	jdr := jdr2string(sary[1],F);
        rec.evmaster[i].jdr := jdr;
        rec.evmaster[i].file := add_dir(sary[2]);
      };
    };
  };
}

#-----------------------------------------------------------------------
#Name: jdr_to_meta
#-----------------------------------------------------------------------
#Type: Function
#Doc: get meta-information (antenna positions etc.) from JDR;
#Ret: record with header entries
#
#Par: rec	= 			- Record, where data is stored
#Par: jdr	= 			- datestamp of the event
#Par: obs   = 'LOPES'   - Name of the observatory
#-----------------------------------------------------------------------
jdr_to_meta := function(ref rec, jdr,obs='LOPES'){
  global evmeta;
  if (!has_field(rec,'evmaster') || !has_field(rec,'evmasterobs') || (rec.evmasterobs != obs) ) {
    if (obs == 'LOPES') {
      include_data_master(rec,globalpar.evmaster);
      rec.evmasterobs := 'LOPES'
    } else if (obs == 'LORUN') {
      include_data_master(rec,globalpar.LORUNevmaster);
      rec.evmasterobs := 'LORUN'   
    };
  };
#get include-file from JDR
  index:=1;
  for (i in seq(len(rec.evmaster))) {
    if ( (rec.evmaster[i].jdr <= jdr) & 
         (rec.evmaster[i].jdr >= rec.evmaster[index].jdr) ) {
      index := i;
    };
  };
#include include-file if needed
  if (evmeta.metafilename != rec.evmaster[index].file) {
    evmeta := [=];
    include rec.evmaster[index].file;
    evmeta.metafilename := rec.evmaster[index].file;
  };
#return the data
  return evmeta;
};

#-----------------------------------------------------------------------
#Name: input_event
#-----------------------------------------------------------------------
#Type: Function
#Doc:  reads in data from one or more event-files
#Par:  rec    - Reference to the data structure.
#Par:  files  - List of files to open
#Par:  dir    - Directory within which the <tt>files</tt> are stored;
#               not required if the full path is already specified via
#               the <tt>files</tt> parameter
#Par:  index  - Set of antennae
#Ref:  newdata, TIM40Header, jdr_to_meta, field_names, SpawnRecord
#-----------------------------------------------------------------------
input_event := function(ref rec, files, dir='',index=[]) {
  global obspar; global globalpar
  newdata(rec);
  globalpar.DataID+:=1;
  MinBlocksize:=globalpar.BlocksizeForDisplay;

  for (filename in files) {
    filename := spaste(dir,filename);
    flen := as_integer(stat(filename).size);
    if (flen < 84e6) {
      evdesc.filename := filename;
      evdesc.readall := T;
      event := globalpar.evsave_client->readevent(evdesc);
      if (event.type == 1) {
        h := TIM40Header(Project='LOPES: Event measurements');
        h.Date:=dq.time([value=(event.JDR+3506716800),unit='s'],form="ymd");
        h.Time:=dq.time([value=(event.JDR+3506716800),unit='s']);
        h.JDR := event.JDR;
        h.TL := event.TL;
        h.presync := event.presync;
        if (has_field(event,'observatory') && event.observatory == 1) {
          mdata := jdr_to_meta(rec,event.JDR,'LORUN');
	    } else {
          mdata := jdr_to_meta(rec,event.JDR);
        };
        for (ff in field_names(mdata.allhead)) {
          h[ff] := mdata.allhead[ff];
        };
	if (has_field(h,'Observatory')) {
          obspar.toglobalpar(h['Observatory'],h.Date);
        }
        chnames := field_names(event)[field_names(event) ~ m/ch[0-9]/];
        if ((len(index)>0)&(min(index)>0)&(max(index)<=len(chnames))) {
           chnames :=chnames[index];
        };
        for (chan in chnames) {
          local lhead;
          lhead := h;
          lhead.AntID    := event[chan].ID;
          lhead.Basefile := filename;
	  lhead.Files    := spaste('Event',event.JDR,'-',(event[chan].ID));
          for (ff in field_names(mdata.ch[as_string(event[chan].ID)])) {
            lhead[ff] := mdata.ch[as_string(event[chan].ID)][ff];
          };
          i:=rec.newentry(lhead);
          arylen := len(event[chan].data);
	  for (ff in rec.get('Input',i)) {
            rec.put(event[chan].data,ff,i)
          };
	  for (ff in rec.coord(i)) {
            rec.touch(ff,i,version=0);
          }
          rec.put(globalpar.centeroffset,'Center',i)
          rec.putmeta(0,'FileOffset',i);
          rec.putmeta(arylen,'FileBlocksize',i);
          rec.putmeta(arylen,'Filesize',i);
          rec.setblock(n=1,blocksize=min(MinBlocksize,arylen),offset=0,index=i);
        }
        #obspar.toglobalpar(rec.head('Observatory'));
        rec.doframe();
        rec.setphase();
        rec.peaks := [=];
        rec.peaks_sens := -1;
        rec.peaks_bs   := -1;
        if (len(globalpar.badantennas)>0) {
          print 'De-selecting antennas:',globalpar.badantennas;
          for (j in globalpar.badantennas) {
            if (j<=rec.len) rec.select(j,F); # ANIGL: fixed data.len -> rec.len
          };
        }
rec.putflag(modified_hanning(rec.getmeta('Blocksize')),'f(x)',-1);
      } else {
        return; #unknown type, should be prevented by evsave_client;
      };
    if (has_field(rec,"aux")) SpawnRecord(rec,rec.aux)
    } else {
      print 'File :',filename,' too large! Not loaded!';
    };
  };
};

#-----------------------------------------------------------------------
#Name: genTSary
#-----------------------------------------------------------------------
#Type: Function
#Doc: generates a time-spectrum array from a directory of event-files
#
#TSary := genTSary(data,index=[2,3,4],filepattern='/data/TIM40/events/',nfreq=500)
#
#TSary := genTSary(data,index=[1,3,4,5,6],filepattern='/data/evdata/',nfreq=4000)
#-----------------------------------------------------------------------
genTSary := function(ref rec, index=[2,3,4], filepattern='/data/TIM40/*event',
                     nfreq=100,azm=0,elv=90,size=32768)
{
  files := shell(paste('ls',filepattern));
  files := files ~ globalconst.FilenameExtractor;
files := files[6901:7192];
files := files[1:300];
#files := files[301:600];
#files := files[601:900];
#files := files[901:1200];
#files := files[1201:1500];
#files := files[1501:1800];
#files := files[1801:2150];
#files := files[2151:2500];
#files := files[2501:2816];
#files := files[1:600];
#files := files[601:1200];
#files := files[1201:1800];
#files := files[1801:2400];
#files := files[2401:2816];
  dirstring := filepattern;
  if (!fexist(spaste(dirstring,files[1]))) dirstring := filepattern ~ globalconst.DirExtractor;
  if (!fexist(spaste(dirstring,files[1]))) {
    print "Can't extract directory from filepattern!";
    return F;
  };
  nofiles := len(files);
  pixarray := array(0.,nofiles,nfreq);
  input_event(rec,spaste(dirstring,files[1]));
  antpos:=CalcAntPos(rec,index,1,1);
  nant:=len(antpos);
  samplerate:=data.headf('Samplerate',index[1])*data.headf('SamplerateUnit',index[1]);
  delays:=round(angle2delay(antpos,azm,elv,F)*samplerate);
print delays;
  maxdelay := max(delays);
  mindelay := min(delays);
  nsteps:=maxdelay-mindelay;
  flen := rec.getmeta('FileBlocksize',index[1]);
  flen := 2^16; ############# ugly hack!
  arylen:= size;
  if ((size+nsteps) > flen) {
    arylen:=flen-nsteps;
  };
print len(files),arylen;
print spaste(dirstring,files[1]);

  tmp := array(0.,nfreq);
  fnum:=0;
altmem := alloc_info();
  for (file in files) {
    input_event(rec,spaste(dirstring,file));
    fnum:=fnum+1;
     ary:=array(0,arylen); 
     for (n in seq(nant)) {
       ary:=ary+rec.get('Dummy',antpos[n].antenna,start=(1+maxdelay+delays[n]),end=(maxdelay+delays[n]+arylen));
     };
     ary:=ary/nant; 
     tmpindex := rec.newentry(head=rec.get('Header',index[1]));
     ci := rec.get('Coordinfo',tmpindex);
     ci['Dummy'].IsInput := T;
     ci['Dummy'].Stored := T;
     ci := rec.put(ci,'Coordinfo',tmpindex);
     rec.setregion([1,arylen],'T',tmpindex);
     rec.putmeta(arylen,'FileBlocksize',tmpindex);
     rec.putmeta(arylen,'Blocksize',tmpindex);
     rec.put(ary,'Dummy',tmpindex); 
     for (ff in rec.coord(tmpindex)) {rec.touch(ff,tmpindex,version=0)}
     rec.touch('Dummy',tmpindex);
     Tary := rec.get('NoiseT',tmpindex);
     l := len(Tary);
     for (freqno  in seq(nfreq) ) {
       tmp[freqno] := mean(Tary[ceil(((freqno-1)*l/nfreq)+1):floor(freqno*l/nfreq)]);
     };
     pixarray[fnum,]:=tmp;
     if((fnum%100)==0) { 
       print 'done',fnum,'out of',nofiles,'Events.';
       print 'new used: ',(alloc_info().used-altmem.used),' unused:',(alloc_info().unused-altmem.unused),'total:',(alloc_info().used+alloc_info().unused-altmem.used-altmem.unused);
       altmem := alloc_info();
     };
  };
  write_value(pixarray,spaste('/data/tmp/eventarray',files[1],'.sos'));
  return pixarray;
};

#-----------------------------------------------------------------------
#Name: shortTSary
#-----------------------------------------------------------------------
#Type: Function
#Doc: generates a time-spectrum array from a directory of event-files
#
#TSary := shortTSary(data,index=[2,3,4],filepattern='/data/TIM40/events/',nfreq=500)
#
#TSary := shortTSary(data,index=[1,3,4,5,6],filepattern='/data/evdata/',nfreq=4000) 
#-----------------------------------------------------------------------
shortTSary := function(ref rec, index=[2,3,4], filepattern='/data/TIM40/*event',
                     size=32768)
{
  files := shell(paste('ls',filepattern));
  files := files ~ globalconst.FilenameExtractor;
#files := files[1:600];
#files := files[601:1200];
#files := files[1201:1800];
#files := files[1801:2400];
#files := files[2401:2816];
  dirstring := filepattern;
  if (!fexist(spaste(dirstring,files[1]))) dirstring := filepattern ~ globalconst.DirExtractor; 
  if (!fexist(spaste(dirstring,files[1]))) {
    print "Can't extract directory from filepattern!";
    return F;
  };
  nofiles := len(files);
  pixarray := array(0.,nofiles,nfreq);


  input_event(rec,spaste(dirstring,files[1]));
  arylen := len(rec.get('Power',index[1]));
print len(files),arylen;
  
print spaste(dirstring,files[1]);

  tmp := array(0.,nfreq);
  fnum:=0;
altmem := alloc_info();
  for (file in files) {
     input_event(rec,spaste(dirstring,file));
     fnum:=fnum+1;
     ComputeMeanPower(rec)
     ary:=array(0,arylen); 
     for (n in index) {
      ary:=ary+rec.get('MNT',n);
     };
     ary:=ary/len(index); 
     ci := rec.put(ci,'Coordinfo',tmpindex);
     rec.setregion([1,arylen],'T',tmpindex);
     rec.putmeta(arylen,'FileBlocksize',tmpindex);
     rec.putmeta(arylen,'Blocksize',tmpindex);
     rec.put(ary,'Dummy',tmpindex); 
     for (ff in rec.coord(tmpindex)) {rec.touch(ff,tmpindex,version=0)}
     rec.touch('Dummy',tmpindex);
     Tary := rec.get('NoiseT',tmpindex);
     l := len(Tary);
     for (freqno  in seq(nfreq) ) {
       tmp[freqno] := mean(Tary[ceil(((freqno-1)*l/nfreq)+1):floor(freqno*l/nfreq)]);
     };
     pixarray[fnum,]:=tmp;
     if((fnum%100)==0) { 
       print 'done',fnum,'out of',nofiles,'Events.';
       print 'new used: ',(alloc_info().used-altmem.used),' unused:',(alloc_info().unused-altmem.unused),'total:',(alloc_info().used+alloc_info().unused-altmem.used-altmem.unused);
       altmem := alloc_info();
     };
  };
  write_value(pixarray,spaste('/data/tmp/eventarray',files[1],'.sos'));
  return pixarray;
};

#-----------------------------------------------------------------------
#Name: matchEV2kreta
#-----------------------------------------------------------------------
#Type: Function
#Doc: matches LOPES-Event filenames to a KRETA datatable
#Ret: record with KRETA data and filenames
#
#-----------------------------------------------------------------------
matchEV2kreta :=  function(filepattern='/data/evdata/*event',
                             kretafile='LOPES-daten-4232.tab',longfile=F) {

#get number of LOPES-eventfiles
  nofiles:=0;
  for (filepat in filepattern) {
    files := shell(paste('ls',filepat));
    files := files ~ globalconst.FilenameExtractor;
    dirstring := filepat;
    if (!fexist(spaste(dirstring,files[1]))) dirstring := filepat ~ globalconst.DirExtractor; 
    if (!fexist(spaste(dirstring,files[1]))) {
      print "Can't extract directory from filepattern!",filepat;
      return F;
    };
    nofiles := nofiles+len(files);
  };
  
  print 'Reading in',nofiles,'LOPES events';
#read in the LOPES-Events
  Levtime := array(0.,nofiles);
  fname   := array('',nofiles);
  fnum:=0; 
  evdesc.readall := T;
  for (filepat in filepattern) {
    files := shell(paste('ls',filepat));
    files := files ~ globalconst.FilenameExtractor;
    dirstring := filepat;
    if (!fexist(spaste(dirstring,files[1]))) dirstring := filepat ~ globalconst.DirExtractor; 
    for (file in files) {
      fnum:=fnum+1;
      fname[fnum] := spaste(dirstring,file);
      evdesc.filename := fname[fnum];
      event := globalpar.evsave_client->readevent(evdesc);
      Levtime[fnum]:=event.JDR+event.TL/5.e6;
      if (fnum%500 == 0) print 'Processed',fnum,'out of',nofiles,'Events';
    };
  };
  print 'read in ',nofiles,'eventfiles';

  erg := [=];
  edep := [=];
  fnum:=0;

#read in the Kreta-file
  for (filename in kretafile) {
    fd := open(paste('<',filename));
    while ( line := read(fd) ) {
      arr := split(line);
      if (arr[1] ~ m/^#/ ) {
        next;
      };
      fnum:=fnum+1;
      erg.run[fnum] := as_integer(arr[1]);
      erg.evnum[fnum] := as_integer(arr[2]);
      erg.GT[fnum] := as_integer(arr[3]);
      erg.MMN[fnum] := as_integer(arr[4]);
      erg.HMS[fnum] := as_integer(arr[5]);
      erg.clnum[fnum] := as_integer(arr[6]);
      erg.AZ[fnum] := as_float(arr[7]);
      erg.ZE[fnum] := as_float(arr[8]);
      erg.XC[fnum] := as_float(arr[9]);
      erg.YC[fnum] := as_float(arr[10]);
      erg.SIZE[fnum] := as_float(arr[11]);
      erg.LMUO[fnum] := as_float(arr[12]);
      erg.T0[fnum] := as_float(arr[13]);
      line := read(fd)
      arr := split(line);
      edep[fnum] := as_float(arr[1:24]);
      if (longfile) {
        line := read(fd)
        arr := split(line);
        edep[fnum][25:48] := as_float(arr[1:24]);
      }
      erg.filename[fnum] := ''; erg.logEguess[fnum] := 0.;
      #Diplomarbeit von Jurriaan van Buren, Seite 30
      if ((erg.SIZE[fnum]>0) & (erg.LMUO[fnum]>0)){
        if ((erg.ZE[fnum]>0) & (erg.ZE[fnum]<0.31416)) {
          erg.logEguess[fnum] := 2.326 + 0.7893*log(erg.LMUO[fnum]) + 0.1909*log(erg.SIZE[fnum]);
        } else if ((erg.ZE[fnum]>=0.31416) & (erg.ZE[fnum]<0.43634)) {
          erg.logEguess[fnum] := 2.375 + 0.7897*log(erg.LMUO[fnum]) + 0.1945*log(erg.SIZE[fnum]);
        } else {
          erg.logEguess[fnum] := 2.865 + 0.7901*log(erg.LMUO[fnum]) + 0.1945*log(erg.SIZE[fnum]);
        };
      }; 
      Kevtime[fnum] := erg.GT[fnum] + erg.MMN[fnum]/1.e9;
    };
  };
  nquad := as_integer(len(edep[1])/4);
  erg.edep := array(0.,fnum,len(edep[1]));
  erg.edepsum := array(0.,fnum,nquad);
  for ( i in seq(fnum)) {
    erg.edep[i,] := edep[i];
    for (j in seq(nquad)){
      erg.edepsum[i,j] := sum(edep[i][(1+(j-1)*4):(j*4)]);
    };
  };
  erg.KLdiff := array(0.,fnum);
  
  print 'read in ',fnum,'events';

  mindif := 1;
  for (i in seq(len(Levtime)-1)) {
    if ((Levtime[i+1]-Levtime[i])<mindif) mindif := (Levtime[i+1]-Levtime[i]);
  };
print "Mindiff-l:",mindif;
  mindif := 1;
  for (i in seq(len(Kevtime)-1)) {
    if ((Kevtime[i+1]-Kevtime[i])<mindif) mindif := (Kevtime[i+1]-Kevtime[i]);
  };
print "Mindiff-k:",mindif;

  Lstart:=1;
  Kstart:=1;
#find a time-offset
  if (Kevtime[1] < Levtime[1]) { 
    adiff := Levtime[1] - Kevtime[1];
    while ( Kstart<fnum ) {
      diff := Levtime[1] - Kevtime[Kstart+1];
      if (abs(diff) > abs(adiff)) break;
      adiff := diff;
      Kstart:=Kstart+1;
    };
  } else { 
    adiff := Kevtime[1] - Levtime[1];
    while ( Lstart<nofiles ) {
      diff := Kevtime[1] - Levtime[Lstart+1];
      if (abs(diff) > abs(adiff)) break;
      adiff := diff;
      Lstart:=Lstart+1;
    };
  };
  #ofs := as_integer(Kevtime[Kstart]-Levtime[Lstart]);
  ofs :=-0.2 ;
  #ofs :=0.8 ;

  print 'ktime',Kevtime[Kstart],'kstart',Kstart,'ltime',Levtime[Lstart],'lstart',Lstart,'diff',Kevtime[Kstart]-Levtime[Lstart];
  print jdr2string(Kevtime[Kstart]),jdr2string(Levtime[Lstart]);

  diff := 0.;
  Lpos := Lstart;
  npos := 400;
  for (Kpos in[Kstart:fnum]) {
   while ((Lpos<nofiles)&&(Kevtime[Kpos] > (Levtime[Lpos]+ofs)) && 
          (abs(Kevtime[Kpos]-(Levtime[Lpos]+ofs))>0.38)) {
     Lpos := Lpos+1;
   };
   if (Lpos>nofiles) {
    print 'breaking at ',jdr2string(Kevtime[Kpos]),Kpos,Lpos;
    break;
   };
   if (abs(Kevtime[Kpos]-(Levtime[Lpos]+ofs))<0.38) {
     erg.filename[Kpos] := fname[Lpos];
     erg.KLdiff[Kpos] :=  Kevtime[Kpos]-Levtime[Lpos];
     if (abs(Kevtime[Kpos]-(Levtime[Lpos]+ofs)) > diff) {
       diff := abs(Kevtime[Kpos]-(Levtime[Lpos]+ofs));
       print 'KPos:',Kpos,'new maxdiff:',(Kevtime[Kpos]-(Levtime[Lpos]+ofs));
     };
     if (Kpos>npos) {
       print Kpos,'new offset:', (Kevtime[Kpos]-(Levtime[Lpos]+ofs));
       npos := Kpos+400;
     };
     Lpos := Lpos+1;
   };
  };

  write_value(erg,spaste(kretafile[1],'.sos'));
  return erg;
}

#-----------------------------------------------------------------------
#Name: SelectEvents
#-----------------------------------------------------------------------
#Type: Function
#Doc: selects events according to different kriteria
#Ret: record with KRETA data and filenames
#Par: infiles = 	- array of filenames from which events are to be selected
#Par: minsize =	1	- minimum size (i.e. number of electrons)
#			  size < 0 means KASCADE Array processor failed
#Par: maxsize = 1e99 	- maximum size
#Par: minlmuo = 1 	- minimum truncated muon number
#Par: maxlmuo = 1e99 	- maximum truncated muon number
#Par: maxrad = 91 	- maximum distance of shower core to array center
#Par: maxze = 1.6 	- maximum zenit angle (in radians)
#Par: minze = -1 	- minimum zenit angle (in radians)
#Par: totalsizes = F 	- false: shower has to match both electron and muon cuts
#			  true: shower has to match either electron or muon cuts
#Par: extsel = T 	- boolean array for external selections; has to
# 			  have the same shape for all input files, so 
#			  only useful for one input file; For experts only!
#			  Default is to ignore it.
#Example: erg:=SelectEvents(infiles=shell('ls /data/LOPES/cr/matched/*.sos'))
#-----------------------------------------------------------------------
SelectEvents := function(infiles, minsize=1, maxsize=1e99, minlmuo=1,maxlmuo=1e99,maxrad=91,maxze=1.6,minze=-1.,totalsizes=F,extsel=T){
  
  erg := [=];
  erg.edep := [=];
  fnum := 1;
  for (file in infiles)  {
    inrec := read_value(file);
    filear := inrec.filename != '';
    sizear := inrec.SIZE > minsize & inrec.SIZE < maxsize;
    lmuoar := inrec.LMUO > minlmuo & inrec.LMUO < maxlmuo;
    radar :=  sqrt(inrec.XC^2+inrec.YC^2) < maxrad;
    zear := inrec.ZE<maxze & inrec.ZE>minze;
    if ((len(extsel)!=1)&len(extsel)!=len(inrec.filename)) {
      print 'incompatible extsel!',len(extsel),len(inrec.filename);
      return F;
    };
    
    if (totalsizes){ sizesel := sizear | lmuoar; 
    } else { sizesel := sizear & lmuoar; 
    };

    selar := filear & sizesel & radar & zear &  extsel;
    print 'Files:',sum(filear),'SIZE:',sum(sizear),'LMUO:',sum(lmuoar),'Size:',sum(sizesel),'Rad:',sum(radar),'ZE:',sum(zear),'Extern:',sum(extsel),' Total:',sum(selar),'-> Outsize:',(fnum+sum(selar)-1)

    l:= sum(selar);
    if (l>0) {
      erg.run[fnum:(fnum+l-1)] := inrec.run[selar];
      erg.evnum[fnum:(fnum+l-1)] := inrec.evnum[selar];
      erg.GT[fnum:(fnum+l-1)] := inrec.GT[selar];
      erg.MMN[fnum:(fnum+l-1)] := inrec.MMN[selar];
      erg.HMS[fnum:(fnum+l-1)] := inrec.HMS[selar];
      erg.clnum[fnum:(fnum+l-1)] := inrec.clnum[selar];
      erg.AZ[fnum:(fnum+l-1)] := inrec.AZ[selar];
      erg.ZE[fnum:(fnum+l-1)] := inrec.ZE[selar];
      erg.XC[fnum:(fnum+l-1)] := inrec.XC[selar];
      erg.YC[fnum:(fnum+l-1)] := inrec.YC[selar]; 
      erg.SIZE[fnum:(fnum+l-1)] := inrec.SIZE[selar];
      erg.LMUO[fnum:(fnum+l-1)] := inrec.LMUO[selar];
      erg.T0[fnum:(fnum+l-1)] := inrec.T0[selar];
      erg.filename[fnum:(fnum+l-1)] := inrec.filename[selar];
      if (has_field(inrec,'logEguess')) {
        erg.logEguess[fnum:(fnum+l-1)] := inrec.logEguess[selar];
      };
      if (fnum == 1) {
        erg.edep := inrec.edep[selar,];
        erg.edepsum := inrec.edepsum[selar,];
      } else if ((len(selar)==1) && selar){
        erg.edep := rbind(erg.edep,inrec.edep);
        erg.edepsum := rbind(erg.edepsum,inrec.edepsum);        
      } else {
        erg.edep := rbind(erg.edep,inrec.edep[selar,]);
        erg.edepsum := rbind(erg.edepsum,inrec.edepsum[selar,]);
      };
      fnum:=fnum+l;
    };
  };

  return erg;
}

#######################################################################
# Misc. Functions for the event datafiles
########################################################################
tdiffarray := function(filepattern='/data/TIM40/*event') {
  files := shell(paste('ls',filepattern));
  files := files ~ globalconst.FilenameExtractor;
  dirstring := filepattern;
  if (!fexist(spaste(dirstring,files[1]))) dirstring := filepattern ~ globalconst.DirExtractor; 
  if (!fexist(spaste(dirstring,files[1]))) {
    print "Can't extract directory from filepattern!";
    return F;
  };
  nofiles := len(files);
  erg := array(0.,nofiles);
  altjdr := 0;
  evdesc.readall := T;
  fnum:=0;
  for (file in files) {
    fnum:=fnum+1;
    evdesc.filename := spaste(dirstring,file);
    event := globalpar.evsave_client->readevent(evdesc);
    erg[fnum]:=event.JDR+event.TL/5e6-altjdr;
    altjdr:=event.JDR+event.TL/5e6;
  };
  return erg;
};

check_lwl := function(rec){
  sum := F; top:=0;
  for (i in seq(rec.len)){
    tmp := identical_pairs(data.get('f(x)',i));
    print 'index:',i,'identical_pairs:',tmp,tmp>0.01;
    if (tmp>top) {top := tmp;sum := tmp>0.01;};
  };
  print 'Max:',top,'All:',sum;
  return sum;
};

#-----------------------------------------------------------------------
#Name: comppos
#-----------------------------------------------------------------------
#Type: Function
#Doc: compare two modified GPS-positions files and tell something about 
#     the differencec in the resulting baselines
#
#-----------------------------------------------------------------------
comppos := function(infile1,infile2,n=10) {
  inarray1 := inarray2 := array(0.,n,3);
  
  fd := open(paste('<',infile1));
  for (i in [1:n]) {
    line := read(fd)~ s/,/./g;
    print line;
    larr := split(line,';');
    inarray1[as_integer(larr[1]),] := as_float(larr[2:4]);
  };
  fd := open(paste('<',infile2));
  for (i in [1:n]) {
    line := read(fd)~ s/,/./g;
    print line;
    larr := split(line,';');
    inarray2[as_integer(larr[1]),] := as_float(larr[2:4]);
  };
  nob :=0;dsum1:=0;dsum2:=0;dmax1:=0;dmax2:=0;
  for (i in [1:(n-1)]) {
    for (j in [(i+1):n]) {
      nob:=nob+1;
      base1 := [(inarray1[i,1]-inarray1[j,1]),(inarray1[i,2]-inarray1[j,2]),
              (inarray1[i,3]-inarray1[j,3])];
      base2 := [(inarray2[i,1]-inarray2[j,1]),(inarray2[i,2]-inarray2[j,2]),
              (inarray2[i,3]-inarray2[j,3])];
      diff := sqrt((base1[1]-base2[1])^2+(base1[2]-base2[2])^2+(base1[3]-base2[3])^2);
      diff2 := sqrt((base1[1]-base2[1])^2+(base1[2]-base2[2])^2);
      print 'Baseline ',i,j,'meas1:',base1,'meas2',base2;
      print 'Baseline ',i,j,'difference:',diff,'meters',diff2;
      dsum1:=dsum1+diff; dsum2:=dsum2+diff2;
      dmax1:=max(dmax1,diff);dmax2:=max(dmax2,diff2);
    };
  };
  print 'Mean difference:',dsum1/nob,'meters',dsum2/nob;
  print 'Max difference:',dmax1,'meters',dmax2;
};
