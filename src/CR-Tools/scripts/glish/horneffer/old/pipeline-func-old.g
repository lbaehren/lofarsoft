########################################################################
#
# functions used by the script controlling the LOPES analysis pipeline
#
########################################################################

#-----------------------------------------------------------------------
#Name: jdr2ldate
#-----------------------------------------------------------------------
#Doc: convert the JDR to 6-digit date-string 
#-----------------------------------------------------------------------
jdr2ldate := function(jdr=time()){
  tmp := split(dq.time([value=(jdr+3506716800),unit='s'],
                       form=['ymd','no_time']),'')
  return spaste(tmp[3],tmp[4],tmp[6],tmp[7],tmp[9],tmp[10]);
};

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
#Name: jdr2path
#-----------------------------------------------------------------------
#Doc: get the path to an event file (on the ray machine in Nijmegen) from 
#     the timestamp
#-----------------------------------------------------------------------
jdr2path := function(jdr=time()){
  erg := [=];
  tmp := split(jdr2ldate(jdr),'');
  erg.path := spaste('20',tmp[1],tmp[2],'/',tmp[3],tmp[4],'/',tmp[5],tmp[6]);
  timestring := jdr2string(jdr);
  erg.filename := spaste(jdr2string(jdr),'.event');
  erg.fullpath := spaste(erg.path,'/',erg.filename);
  return erg;
};

#-----------------------------------------------------------------------
#Name: fname2path
#-----------------------------------------------------------------------
#Doc: get the path to an event file (on the ray machine in Nijmegen) from 
#     the filename
#-----------------------------------------------------------------------
fname2path := function(fname){
  erg := [=];
  fname := fname~globalconst.FilenameExtractor
  tmp := split(fname,'');
  erg.path := spaste('20',tmp[3],tmp[4],'/',tmp[6],tmp[7],'/',tmp[9],tmp[10]);
  erg.fullpath := spaste(erg.path,'/',fname);
  return erg;
};


#-----------------------------------------------------------------------
#Name: kretain
#-----------------------------------------------------------------------
#Type: Function
#Doc: read in one ascii-file with the KRETA data
#Ret: record with KRETA data 
#
#Par: kretafile	 	- path to the kretafile to read
#Par: longfile	= T	- is this a long kretafile 
#-----------------------------------------------------------------------
kretain := function(kretafile,longfile=T){
  if (!fexist(kretafile)) return F;
  fd := open(paste('<',kretafile));
  erg := [=];
  edep := [=];
  fnum:=0;
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
    erg.logEguess[fnum] := 0.;
####Diplomarbeit von Jurriaan van Buren, Seite 30
    if ((erg.SIZE[fnum]>0) & (erg.LMUO[fnum]>0)){
      if ((erg.ZE[fnum]>0) & (erg.ZE[fnum]<0.31416)) {
        erg.logEguess[fnum] := 2.326 + 0.7893*log(erg.LMUO[fnum]) + 0.1909*log(erg.SIZE[fnum]);
      } else if ((erg.ZE[fnum]>=0.31416) & (erg.ZE[fnum]<0.43634)) {
        erg.logEguess[fnum] := 2.375 + 0.7897*log(erg.LMUO[fnum]) + 0.1945*log(erg.SIZE[fnum]);
      } else {
        erg.logEguess[fnum] := 2.865 + 0.7901*log(erg.LMUO[fnum]) + 0.1945*log(erg.SIZE[fnum]);
      };
    }; 
    erg.Kevtime[fnum] := erg.GT[fnum] + erg.MMN[fnum]/1.e9;
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
  
  print 'read in ',fnum,'events';
  return erg;
};


#-----------------------------------------------------------------------
#Name: ldirs_from_ktable
#-----------------------------------------------------------------------
#Type: Function
#Doc: generate a list of the directories with the LOPES data for a given
#     kreta table
#Ret: sorted array of strings with the pathnames of the directories
#
#Par: kretatable =	- record with KRETA data
#-----------------------------------------------------------------------
ldirs_from_ktable := function(kretatable){
  klength := len(kretatable.GT);

###get array of date-srings for all days of run
  kstartdate := (kretatable.GT[1])
  kenddate := (kretatable.GT[klength])
  numdays := 1;
  kdates[1] := kstartdate;
  while (as_integer(kdates[numdays]) < as_integer(kenddate)) {
    kdates[numdays+1] :=  (kretatable.GT[1]+(86400*numdays)) 
    numdays := numdays+1;
  };
  print 'Run',kretatable.run[1],'started',kstartdate,'stoped',kenddate,'==',numdays,'days';

###get array of lopes event-dirs for all days of run
  numdirs := 0;
  for (date in kdates) {
    archfiles := shell(spaste('ls ',pipepar.eventarchdir,'lopes-',date,'*'));
    for (file in archfiles) {
      dirname := file~globalconst.FilenameExtractor;
      dirname := dirname~s/\..*//;
      dirpath := spaste(pipepar.eventdatadir,dirname,'/');
      if (!is_dir(dirpath)){
        ### directory doesn't exist -> untar the archive!
        postfix := file~s/.*\.//;
        if (postfix == 'tbz'){
          print 'Unpacking archive',file;
          shell(paste('cd',pipepar.eventdatadir,';tar -xjf',file));
        } else if (postfix == 'tgz'){
          print 'Unpacking archive',file;
          shell(paste('cd',pipepar.eventdatadir,';tar -xzf',file));
        } else {
          print 'unknown postfix',postfix,'on file',file;
        };
      };
      if (is_dir(dirpath)){
        numdirs := numdirs+1;
        lopesdirs[numdirs] := dirpath;
        print 'Added dir:',lopesdirs[numdirs],'to the list';
      } else {
        print "Can't find oder generate dir:",dirpath;
      };
    };
  };
  return lopesdirs;
};

#-----------------------------------------------------------------------
#Name: ldirs_from_ktable_ray
#-----------------------------------------------------------------------
#Type: Function
#Doc: generate a list of the directories with the LOPES data for a given
#     kreta table, on the ray machine in Nijmegen
#Ret: sorted array of strings with the pathnames of the directories
#
#Par: kretatable =	- record with KRETA data
#-----------------------------------------------------------------------
ldirs_from_ktable_ray := function(kretatable){
  klength := len(kretatable.GT);

###get array of date-strings for all days of run
  kstartdate := (kretatable.GT[1])
  kenddate := (kretatable.GT[klength])
  numdays := 1;
  kdates[1] := kstartdate;
  while (as_integer(jdr2ldate(kdates[numdays])) < as_integer(jdr2ldate(kenddate))) {
    kdates[numdays+1] := (kretatable.GT[1]+(86400*numdays)) 
    numdays := numdays+1;
  };
  print 'Run',kretatable.run[1],'started',jdr2ldate(kstartdate),'stoped',
        jdr2ldate(kenddate),'==',numdays,'days';

###get array of lopes event-dirs for all days of run
  numdirs := 0;
  for (date in kdates) {
    tmp := jdr2path(date);
    dirpath := spaste(pipepar.eventdatadir,tmp.path,'/');
    if (is_dir(dirpath)){
      numdirs := numdirs+1;
      lopesdirs[numdirs] := dirpath;
      print 'Added dir:',lopesdirs[numdirs],'to the list';
    } else {
      print "Can't find dir:",dirpath;
    };
  };
  return lopesdirs;
};

#-----------------------------------------------------------------------
#Name: lopesin
#-----------------------------------------------------------------------
#Type: Function
#Doc: read in the headers of lopes eventfiles
#Ret: record with lopes headerdata
#
#Par: lopesdirs = 	- sorted array of strings with the pathnames of 
#			  the directories
#-----------------------------------------------------------------------
lopesin := function(lopesdirs){
#get number of LOPES-eventfiles
  nofiles:=0;
  for (ldir in lopesdirs) {
    files := shell(paste('ls',ldir));
    files := files ~ globalconst.FilenameExtractor;
    dirstring := ldir;
    if (!fexist(spaste(dirstring,files[1]))) dirstring := ldir ~ globalconst.DirExtractor; 
    if (!fexist(spaste(dirstring,files[1]))) {
      print "Can't extract directory from filepattern!",ldir;
      return F;
    };
    nofiles := nofiles+len(files);
  };
  
  print 'Reading in',nofiles,'LOPES events';
#read in the LOPES-Events
  erg.Levtime := array(0.,nofiles);
  erg.fname   := array('',nofiles);
  fnum:=0; 
  evdesc.readall := T;
  for (ldir in lopesdirs) {
    files := shell(paste('ls',ldir));
    files := files ~ globalconst.FilenameExtractor;
    dirstring := ldir;
    if (!fexist(spaste(dirstring,files[1]))) dirstring := ldir ~ globalconst.DirExtractor; 
    for (file in files) {
      fnum:=fnum+1;
      erg.fname[fnum] := spaste(dirstring,file);
      evdesc.filename := erg.fname[fnum];
      event := globalpar.evsave_client->readevent(evdesc);
      erg.Levtime[fnum]:=event.JDR+event.TL/5.e6;
      if (fnum%500 == 0) print 'Processed',fnum,'out of',nofiles,'Events';
    };
  };
  print 'read in ',nofiles,'eventfiles';
  return erg;
};

#-----------------------------------------------------------------------
#Name: klmerge
#-----------------------------------------------------------------------
#Type: Function
#Doc: merge a kreta and a lopes table
#Ret: the kretatable with attached lopes filenames
#
#Par: ktab = 		- record with kreta data
#Par: ltab = 		- record with lopes headerdata
#Par: window = 0.2	- maximal time difference (in s) for kreta and LOPES 
#			  data to be considered the same event
#Par: ofsrange = 5	- range (in s) to search for the offset between K & L
#Par: searchrange = 100	- number of Kretaevents to look at during search for 
#			  the offset
#Par: cldelay = (0.|.2)	- delay between the event-time of KASCADE and LOPES
#-----------------------------------------------------------------------
klmerge := function(ktab,ltab,window=0.2,ofsrange=5,searchrange=100,
                    cldelay=0.){
  knum := len(ktab.run);
  lnum := len(ltab.Levtime);
  ktab.filename := array('',knum);
  ktab.KLdiff := array(0.,knum);
###find the starting events
  Lstart:=1;
  Kstart:=1;
  adiff := ktab.Kevtime[Kstart] - ltab.Levtime[Lstart];
  while ( Lstart<lnum ) {
    diff := ktab.Kevtime[Kstart] - ltab.Levtime[Lstart+1];
    if (abs(diff) > abs(adiff)) break;
    adiff := diff;
    Lstart:=Lstart+1;
  };
  adiff := ltab.Levtime[Lstart] - ktab.Kevtime[Kstart];
  while ( Kstart<knum ) {
    diff := ltab.Levtime[Lstart] - ktab.Kevtime[Kstart+1];
    if (abs(diff) > abs(adiff)) break;
    adiff := diff;
    Kstart:=Kstart+1;
  };
  adiff := ktab.Kevtime[Kstart] - ltab.Levtime[Lstart];
  while ( Lstart<lnum ) {
    diff := ktab.Kevtime[Kstart] - ltab.Levtime[Lstart+1];
    if (abs(diff) > abs(adiff)) break;
    adiff := diff;
    Lstart:=Lstart+1;
  };
print 'ktime',ktab.Kevtime[Kstart],'kstart',Kstart,'ltime',ltab.Levtime[Lstart],'lstart',Lstart,'diff',ktab.Kevtime[Kstart]-ltab.Levtime[Lstart];
###find the correct offset;
  searchrange := min(searchrange,(knum-Kstart));
  ofsary := [(-ofsrange):(ofsrange)]-cldelay;
  matches := array(0.,len(ofsary));
  for (i in seq(len(ofsary))){
    lpos := Lstart;
    for (kpos in [Kstart:(Kstart+searchrange)]){
      while ((lpos<lnum) && 
             (ktab.Kevtime[kpos] > (ltab.Levtime[lpos]+ofsary[i])) &&
             (abs(ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+ofsary[i]))>=window)){
        lpos := lpos+1;
      };
      if (abs(ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+ofsary[i]))<window) {
        matches[i] := matches[i]+1;
      };
    };
  };
  ofs := ofsary[maxpos(matches)];
print 'Using offset:',ofs,'at pos', maxpos(matches),'of:', matches;

###do the matching
  maxdiff := 0.
  lpos := Lstart;
  for (kpos in[Kstart:knum]) {
    while ((lpos<lnum) && 
           (ktab.Kevtime[kpos] > (ltab.Levtime[lpos]+ofs)) &&
           (abs(ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+ofs))>=window)){
      lpos := lpos+1;
    };
    if (abs(ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+ofs))<window) {
      ktab.filename[kpos] := ltab.fname[lpos];
      ktab.KLdiff[kpos] :=  ktab.Kevtime[kpos]-ltab.Levtime[lpos];
      if (abs(ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+ofs)) > maxdiff) {
        maxdiff := abs(ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+ofs));
        print 'KPos:',kpos,'new maxdiff:',(ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+ofs));
      };
      lpos := lpos+1;
    };

    if (lpos>lnum) {
      print 'breaking at ',jdr2string(ktab.Kevtime[kpos]),kpos,lpos;
      break;
    };
  };

  return ktab;
};

#-----------------------------------------------------------------------
#Name: event_from_ktime
#-----------------------------------------------------------------------
#Type: Function
#Doc: find the event file for a given kreta time
#Ret: the full pathname (if successfull) or false
#
#Par: ktime   = 		- kreta time (with subsecond accuracy)
#Par: datadir = '/pedata/'	- root directory where the data is stored
#				  (including trailing '/')
#Par: window  = 0.1		- maximal time difference (in s) for kreta and
#				  LOPES data to be considered the same event
#Par: offsets = [-0.2,0.8]	- test these offsets between ltime and ktime
#-----------------------------------------------------------------------
event_from_ktime := function(ktime,datadir='/pedata/',window=0.2,
                             offsets=[-0.2,0.8]) {
  tmprec := jdr2path(ktime);
  tmpstring := spaste(split(tmprec.filename,'')[1:18])
  shellstring := spaste('ls ',datadir,tmprec.path,tmpstring,'*')
print ':::',shellstring,':::'
  files := shell(shellstring);
print files;
  evdesc.readall := T;
  for (ff in files){
    evdesc.filename := ff;
    event := globalpar.evsave_client->readevent(evdesc);
    if (is_record(event)){
      ltime :=event.JDR+event.TL/5.e6;
      for (ofs in offsets){
        if (abs(ktime-(ltime+ofsary[i]))<window) {
	  return ff;
        };
      };
    };
  };
  return F;
};

#-----------------------------------------------------------------------
#Name: klmerge2
#-----------------------------------------------------------------------
#Type: Function
#Doc: merge two irregular kreta and lopes tables
#     (less precise than "klmerge")
#Ret: the kretatable with attached lopes filenames
#
#Par: ktab = 		- record with kreta data
#Par: ltab = 		 - record with lopes headerdata
#Par: window = 0.1	 - maximal time difference (in s) for kreta and LOPES 
#			   data to be considered the same event
#Par: offsets = [-.2,.8] - test these offsets between ltime and ktime
#-----------------------------------------------------------------------
klmerge2 := function(ktab,ltab,window=0.1,offsets=[-0.2,0.8]){
  knum := len(ktab.run);
  lnum := len(ltab.Levtime);
print knum,lnum;
  ktab.filename := array('',knum);
  ktab.KLdiff := array(0.,knum);

  maxdiff := 0.
  lpos := 1;
  mofs := min(offsets);
  for (kpos in seq(knum)) {
    searching := (lpos<lnum);
if (kpos>10690) {print kpos,lpos,sum(ktab.filename!=''),ktab.KLdiff[kpos],searching ;};
    if (!searching) {
print kpos,lpos,sum(ktab.filename!=''),searching
      break;
    };
    while (searching){
      for (ofs in offsets) {
        if (abs(ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+ofs))<window){
          ktab.filename[kpos] := ltab.fname[lpos];
          ktab.KLdiff[kpos] :=  ktab.Kevtime[kpos]-ltab.Levtime[lpos];
          searching:=F;
          break; #break the for loop;
        };
      };
      #break the while loop if we can't find a lopes event for this kreta event
      if ((ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+mofs))<window ){break;};
      lpos := lpos+1;
      if (lpos>lnum) { searching := F;};
    };
if (kpos%1000==0) {print kpos,lpos,sum(ktab.filename!=''),ktab.KLdiff[kpos] ;};
if (kpos>10690) {print kpos,lpos,sum(ktab.filename!=''),ktab.KLdiff[kpos],searching ;};
  };
  return ktab;
};

read_evlist := function(filename){
  tmp := shell(paste('wc',filename));
  tmp := split(tmp);
  nolines := as_integer(tmp[1]);
print 'reading in',nolines,'lines';
  fpoint := open(paste('<',filename));
  erg := [=];
  erg.Levtime := array(0.,nolines)
  erg.fname := array('',nolines);
  y := [s/\./\//,s/\./\//,s/\./\//,s/\.event//]
atime := time();
stime := atime;
  for (i in seq(nolines)){
    line := read(fpoint)~ s/\n$//;
    erg.fname[i] := line;
    tmp := split(line,'.');
    tmp := spaste(tmp[1],'/',tmp[2],'/',tmp[3],'/',tmp[4],'.',tmp[5]);
#    tmp :=  (((line ~ s/\./\//) ~ s/\./\//)~ s/\./\//)~ s/\.event//
#    tmp :=  line ~ y
    erg.Levtime[i] := jdr2string(tmp,F)
    if (i%5000==0) {
      ntime := time();
      eta := (nolines-i)*(ntime-atime)/5000
      print i,tmp,erg.fname[i],erg.Levtime[i],(ntime-atime),eta/60,
            (ntime-stime+eta)/60 ;
      atime := ntime;
    };
  };
  return erg;
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
    if (has_field(erg,'filtered')) {
      for (i in seq(len(erg.GT))[pattern]) {
        erg.filtered[i]  := inrecb.filtered[i];
        erg.shfilt[i]  := inrecb.shfilt[i];
      };
    };
    if (has_field(erg,'RFImaxfakt')) {
      erg.RFIpow[pattern,,] := inrecb.RFIpow[pattern,,];
      erg.RFImax[pattern,]  := inrecb.RFImax[pattern,];
      erg.RFImaxfakt[pattern,] := inrecb.RFImaxfakt[pattern,];
    };
    if (has_field(erg,'dphi')) {
      erg.dphi[pattern,,] := inrecb.dphi[pattern,,];
      erg.flag[pattern,] := inrecb.flag[pattern,];
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
merge_data_records := function(files){
  l := len(files);
  erg := read_value(files[1]);
  for (i in [2:l]) {
    erg := merge_data_record(erg,read_value(files[i]));
  };
  return erg;
};


#-----------------------------------------------------------------------
#Name: pipeSelectEvents
#-----------------------------------------------------------------------
#Type: Function
#Doc: selects events according to different kriteria
#Ret: record with KRETA data and filenames
#
#-----------------------------------------------------------------------
pipeSelectEvents := function(infiles, minsize=1, maxsize=1e99, minlmuo=1,maxlmuo=1e99,maxrad=91,extsel=T,maxze=1.6,minze=-1.,minlogeguess=0.,totalsizes=F,nofit=T){
  
doit := T;
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
    logEar := inrec.logEguess>minlogeguess;
    if ((len(extsel)!=1)&len(extsel)!=len(inrec.filename)) {
      print 'incompatible extsel!',len(extsel),len(inrec.filename);
      return F;
    };
    
    if (totalsizes){ sizesel := sizear | lmuoar; 
    } else { sizesel := sizear & lmuoar; 
    };

    selar := filear & sizesel & radar & zear & logEar & extsel;
    print 'Files:',sum(filear),'SIZE:',sum(sizear),'LMUO:',sum(lmuoar),'Size:',sum(sizesel),'Rad:',sum(radar),'ZE:',sum(zear),'logE',sum(logEar),'Extern:',sum(extsel),'  Total:',sum(selar)

    l:= sum(selar);
    if (l>0) {
      print fnum,(fnum+l-1);
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
if (doit) {
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
      inindex := [1:len(selar)][selar];
      if (has_field(inrec,'filtrange')) erg.filtrange := inrec.filtrange;
      if (has_field(inrec,'shfilt')&&has_field(inrec,'filtered')) {
	if (fnum==1) {
          erg.shfilt := [=];
          erg.filtered := [=];
        };
        for (i in seq(l)){
          erg.filtered[(fnum+i-1)] := inrec.filtered[(inindex[i])];
          erg.shfilt[(fnum+i-1)] := inrec.shfilt[(inindex[i])];
        };
      };
      if  (has_field(inrec,'shfit')&&has_field(inrec,'fiterg')&&!nofit) {
	if (fnum==1) {
          erg.shfit := [=];
        };
        erg.shfit.height[fnum:(fnum+l-1)] := inrec.shfit.height[selar];
        erg.shfit.width[fnum:(fnum+l-1)] := inrec.shfit.width[selar];
        erg.shfit.center[fnum:(fnum+l-1)] := inrec.shfit.center[selar];
        tmparray := array(0.,l,10);
        nochann := shape(inrec.fiterg[1])[2];
	if (fnum==1) {
          erg.fiterg := [=];
          tmparray[,[1:nochann]] := inrec.fiterg.height[selar,];
          erg.fiterg.height := tmparray;
          tmparray[,[1:nochann]] := inrec.fiterg.width[selar,];
          erg.fiterg.width := tmparray;
          tmparray[,[1:nochann]] := inrec.fiterg.center[selar,];
          erg.fiterg.center := tmparray;
        } else {
          tmparray[,[1:nochann]] := inrec.fiterg.height[selar,];
          erg.fiterg.height := rbind(erg.fiterg.height,tmparray);
          tmparray[,[1:nochann]] := inrec.fiterg.width[selar,];
          erg.fiterg.width := rbind(erg.fiterg.width,tmparray);
          tmparray[,[1:nochann]] := inrec.fiterg.center[selar,];
          erg.fiterg.center := rbind(erg.fiterg.center,tmparray);
        };
      };
      if (has_field(erg,'RFImaxfakt')) {
	if (fnum==1) {
          erg.RFIpow := inrec.RFIpow[selar,,];
          erg.RFImax := inrec.RFImax[selar,];
          erg.RFImaxfakt := inrec.RFImaxfakt[selar,];
        } else {
          erg.RFIpow := rbind(erg.RFIpow,inrec.RFIpow[selar,,])
          erg.RFImax := rbind(erg.RFImax,inrec.RFImax[selar,])
          erg.RFImaxfakt :=  rbind(erg.RFImaxfakt,inrec.RFImaxfakt[selar,]);
        };
      };
};
      fnum:=fnum+l;
    };
  };

#erg.cdist := sqrt((erg.XC+25)^2+(erg.YC-25)^2)

  return erg;
}

#-----------------------------------------------------------------------
#Name: genCopyScript
#-----------------------------------------------------------------------
#Type: Function
#Doc: generate a shell-script to copy all eventfiles from their "ray-dir"
#     to a transferdir
#Ret: 
#-----------------------------------------------------------------------
genCopyScript := function(evtab,scriptname='copyscript',tdir='temp'){
  fp := open(paste('>', scriptname));
  for (fname in evtab.filename){
    tmp := fname2path(fname);
    write(fp,paste('cp',tmp.fullpath,tdir));
  };
  fp := [=];
  return T;
};
