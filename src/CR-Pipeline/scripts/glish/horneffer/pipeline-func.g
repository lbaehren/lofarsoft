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
#Name: newKretain
#-----------------------------------------------------------------------
#Type: Function
#Doc: read in the KRETA data from four files produced with nt/scan from
#     the hbook files
#Ret: record with KRETA data 
#
#Par: kretafiles	 	- path to the four files with the nt/scan output 
#                         to read the files must cover the same events
#-----------------------------------------------------------------------
newKretain := function(kretafiles){
  if (len(kretafiles) != 4) {
    print 'Need four input files!';
    return F;
  };
  fd := [=];
  for (i in [1:4]) {
    if (!fexist(kretafiles[i])) {
      print 'file',kretafiles[i],'seems not to exist.';
      return F;
    };
    fd[i] := open(paste('<',kretafiles[i]));
    print 'opend kretafile "',paste('<',kretafiles[i]),'" :',fd[i]
  };
  erg := [=];
  fnum := 0;
  while ( line1 := read(fd[1]) ) {
    line2 := read(fd[2]);
    line3 := read(fd[3]);
    line4 := read(fd[4]);
    arr1 := split(line1,'|');
    arr2 := split(line2,'|');
    arr3 := split(line3,'|');
    arr4 := split(line4,'|');
    if ( len(arr1) == 1 ||  arr1 =~ m/Event/ ) {next};
    evno :=  as_integer(arr1[1]);
    GT := as_integer(arr1[4])
    if (evno != as_integer(arr2[1]) || evno != as_integer(arr3[1]) ||
        evno != as_integer(arr4[1]) || GT !=  as_integer(arr2[2]) ||
        GT !=  as_integer(arr3[2]) || GT !=  as_integer(arr4[2])  ) {
      print 'kretafiles seem to contain different events!';
      return F;
    }
    fnum:=fnum+1;
    erg.run[fnum] := as_integer(arr1[2]);
    erg.evnum[fnum] := as_integer(arr1[3]);
    erg.GT[fnum] := as_integer(arr1[4]);
    erg.MMN[fnum] := as_integer(arr1[5]);
    erg.YMD[fnum] := as_integer(arr1[6]);
    erg.HMS[fnum] := as_integer(arr1[7]);
    erg.clu0[fnum] := as_integer(arr1[8]);
    erg.AZ[fnum] := as_float(arr1[9]);

    erg.ZE[fnum] := as_float(arr2[3]);
    erg.XC[fnum] := as_float(arr2[4]);
    erg.YC[fnum] := as_float(arr2[5]);
    erg.SIZE[fnum] := as_float(arr2[6]);
    erg.age[fnum] := as_float(arr2[7]);
    erg.nmu[fnum] := as_float(arr2[8]);
    erg.LMUO[fnum] := as_float(arr2[9]);

    erg.T0[fnum] := as_float(arr3[3]);
    erg.trgs[fnum] := as_integer(arr3[4]);
    erg.xcg[fnum] := as_float(arr3[5]);
    erg.ycg[fnum] := as_float(arr3[6]);
    erg.azg[fnum] := as_float(arr3[7]);
    erg.zeg[fnum] := as_float(arr3[8]);
    erg.sizeg[fnum] := as_float(arr3[9]);

    erg.ageg[fnum] := as_float(arr4[3]);
    erg.sizmg[fnum] := as_float(arr4[4]);
    erg.fanka[fnum] := as_float(arr4[5]);
    if (fnum%10000 == 0) {
      print 'Processed event No.',fnum;  
    };
  };
  erg.Kevtime := erg.GT + erg.MMN/1.e9;
#do systematic correction of electron and muon number
  ks := korr_Ne_Nmu(erg.SIZE,erg.LMUO)
#calculate the primary energy with Ralph Glasstetters formula
  erg.logEguess := getCRen(ks.Ne,ks.Nmu,erg.ZE)

  return erg;
};

#-----------------------------------------------------------------------
#Name: QualListIn
#-----------------------------------------------------------------------
#Type: Function
#Doc: read in the data from one of Steffens qual-list files
#Ret: record with  data 
#
#Par: quallist	 	- path to the qual-list file
#-----------------------------------------------------------------------
QualListIn := function(quallist){
  if (!fexist(quallist)) return F;
  fd := open(paste('<',quallist));
  erg := [=];
  erg.dc_offset := [=];
  erg.mean_noise := [=];
  erg.peaks := [=];
  erg.error := [=];
  fnum:=0;
  while ( line := read(fd) ) {
    arr := split(line);
    if (len(arr) < 123) {
      print 'evnumber:',fnum,': line too short:',len(arr),arr[1];
    }
    fnum := fnum + 1;
    erg.JDR[fnum] := as_integer(arr[1]);
    erg.TL[fnum] := as_integer(arr[2]);
    erg.filename[fnum] := arr[3];
    erg.dc_offset[fnum] := as_float(arr[([1:30]*4)]);
    erg.mean_noise[fnum] := as_float(arr[([1:30]*4+1)]);
    erg.peaks[fnum] := as_float(arr[([1:30]*4+2)]);
    erg.error[fnum] := as_float(arr[([1:30]*4+3)]);
  };
  erg.Levtime := erg.JDR + erg.TL/1e9;
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
#Par: window = 0.001	- maximal time difference (in s) for kreta and LOPES 
#			  data to be considered the same event
#Par: ofsrange = 5	- range (in s) to search for the offset between K & L
#Par: searchrange = 100	- number of Kretaevents to look at during search for 
#			  the offset
#Par: cldelay = 0.	- delay between the event-time of KASCADE and LOPES
#-----------------------------------------------------------------------
klmerge := function(ktab,ltab,window=0.001,ofsrange=5,searchrange=100,
                    cldelay=0.){
  knum := len(ktab.run);
  lnum := len(ltab.Levtime);
  ktab.filename := array('',knum);
  ktab.KLdiff := array(0.,knum);
  ktab.dc_offset := [=];
  ktab.mean_noise := [=];
  ktab.peaks := [=];
  ktab.error := [=];
  for (i in [1:knum]){
    ktab.dc_offset[i] := [];
    ktab.mean_noise[i] := [];
    ktab.peaks[i] := [];
    ktab.error[i] := [];
  };
###Do not start at event 0
  Kstart:=max(1,sum(ktab.Kevtime<(ltab.Levtime[1]-10)));
  Lstart:=max(1,sum(ltab.Levtime<(ktab.Kevtime[Kstart]-10)));

###### while loop over all runs  
  while ((Kstart<knum)&&(Lstart<lnum)) {
    thisrun := ktab.run[Kstart];

    ###find the starting events
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
    adiff := ltab.Levtime[Lstart] - ktab.Kevtime[Kstart];
    while ( Kstart<knum ) {
      diff := ltab.Levtime[Lstart] - ktab.Kevtime[Kstart+1];
      if (abs(diff) > abs(adiff)) break;
      adiff := diff;
      Kstart:=Kstart+1;
    };
 #   print 'Run:',thisrun,'ktime',ktab.Kevtime[Kstart],'kstart',Kstart,'ltime',ltab.Levtime[Lstart],'lstart',Lstart,'diff',ktab.Kevtime[Kstart]-ltab.Levtime[Lstart];

  ###find the correct offset;
    offestfound := F
    while (!offestfound) { #continue to search for an offset till one is found
      thissearch := min(searchrange,sum(ktab.run[Kstart:knum]==thisrun));
      ofsary := [(-ofsrange):(ofsrange)]-cldelay;
      matches := array(0.,len(ofsary));
      for (i in seq(len(ofsary))){
        lpos := Lstart;
        for (kpos in [Kstart:(Kstart+thissearch-1)]){
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
      #crappy data at the begining of a run, e.g. due to calibration measurements
      if ((searchrange==thissearch)&&(sum(matches)==0)){
        Kstart := Kstart + thissearch;
        Lstart := lpos;
      } else {
        offestfound := T;
      };
    };
    ofs := ofsary[maxpos(matches)];
    print 'Run:',thisrun,'using offset:',ofs,'at pos', maxpos(matches),'of:', matches,thissearch;

  ###do the matching
    maxdiff := 0.
    lpos := Lstart;
    for (kpos in [Kstart:knum]) {
      ### break the for-loop at the end of the run;
      if (ktab.run[kpos] != thisrun) { break;} 
      while ((lpos<lnum) && 
             (ktab.Kevtime[kpos] > (ltab.Levtime[lpos]+ofs)) &&
             (abs(ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+ofs))>=window)){
        lpos := lpos+1;
      };
      if (abs(ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+ofs))<window) {
        ### found a matching pair, now add the data to the kreta-table
        ktab.filename[kpos] := ltab.filename[lpos];
        ktab.dc_offset[kpos] := ltab.dc_offset[lpos];
        ktab.mean_noise[kpos] := ltab.mean_noise[lpos];
        ktab.peaks[kpos] := ltab.peaks[lpos];
        ktab.error[kpos] := ltab.error[lpos];
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
    }; # end of for-loop
    Kstart := kpos;
    Lstart := lpos;
  }; # end of while-loop
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
event_from_ktime := function(ktime,datadir='/data/LOPES/',window=0.2,
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
#Par: window = 0.001	 - maximal time difference (in s) for kreta and LOPES 
#			   data to be considered the same event
#Par: offsets = [0.,1.] - test these offsets between ltime and ktime
#-----------------------------------------------------------------------
klmerge2 := function(ktab,ltab,window=0.001,offsets=[0.,1.]){
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
      #break the while loop if we cannot find a lopes event for this kreta event
      if ((ktab.Kevtime[kpos]-(ltab.Levtime[lpos]+mofs))<window ){break;};
      lpos := lpos+1;
      if (lpos>lnum) { searching := F;};
    };
if (kpos%1000==0) {print kpos,lpos,sum(ktab.filename!=''),ktab.KLdiff[kpos] ;};
if (kpos>10690) {print kpos,lpos,sum(ktab.filename!=''),ktab.KLdiff[kpos],searching ;};
  };
  return ktab;
};


#-----------------------------------------------------------------------
#Name: reduceTable
#-----------------------------------------------------------------------
#Type: Function
#Doc: reduce merged table to only those events, that are sucessfully merged
#Ret: reduced merged table
#
#-----------------------------------------------------------------------
reduceTable := function(mtab){
  erg := [=];
  sel := mtab.filename != '';
  for (field in field_names(mtab)){
    erg[field] :=  mtab[field][sel];
  };
  return erg;
};


#-----------------------------------------------------------------------
#Name: pipeSelectEvents
#-----------------------------------------------------------------------
#Type: Function
#Doc: selects events according to different kriteria
#Ret: record with KRETA data and filenames
#-----------------------------------------------------------------------
pipeSelectEvents := function(infiles, minsize=1, maxsize=1e99, minlmuo=1,maxlmuo=1e99,maxrad=91,extsel=T,maxze=1.6,minze=-1.,totalsizes=F,goodKASCADE=T,minYMD=0,maxYMD=999999,minFieldEst=0.){
  
  erg := [=];
  fnum := 1;
  for (file in infiles)  {
    inrec := read_value(file);
    if(fnum == 1){
      for (field in field_names(inrec)){
        if (is_record(inrec[field])){
          erg[field] := [=];
        };
      };
    };
    filear := inrec.filename != '';
    sizear := inrec.SIZE > minsize & inrec.SIZE < maxsize;
    lmuoar := inrec.LMUO > minlmuo & inrec.LMUO < maxlmuo;
    YMDar := inrec.YMD >= minYMD & inrec.YMD <= maxYMD;
    radar :=  sqrt(inrec.XC^2+inrec.YC^2) < maxrad;
    zear := inrec.ZE<maxze & inrec.ZE>minze;
    if ((len(extsel)!=1)&len(extsel)!=len(inrec.filename)) {
      print 'incompatible extsel!',len(extsel),len(inrec.filename);
      return F;
    };
    if (totalsizes){ sizesel := sizear | lmuoar; 
     } else { sizesel := sizear & lmuoar; };
    if (goodKASCADE){ 
      reconstar := (inrec.run > 0) & (inrec.age>0.2) & (inrec.age<2.1);
     } else { reconstar := T };

    ## NMU corrected muon number
    inrec.Nmu := korr_Ne_Nmu(inrec.SIZE,inrec.LMUO).Nmu
  
    ## GEOANG geomagnetic angle
    inrec.geoang := ang2B(inrec.AZ,inrec.ZE)
    
    ## DIST average antenna distance to shower axis
    inrec.dist:=array(0,len(inrec.AZ))
    ref_pos:=[=]
    for (i in 1:len(inrec.XC)) {
      eas_pos:=mx.rotate([inrec.XC[i],inrec.YC[i]],angle=15,deg=T)
        # position of shower by KASCADE (CT from KASCADE frame to north by 15 deg)
      avg_pos := [9.05843174,34.8338669,0] # !!! should be calculated for the seceltion of antennae of each event
        # centeroid of LOPES30 (04/2005-06/2006) in respect to KASCADE center point
        # data.get('Center',1) = [-36.1095688 84.5346026 0] coordinates of reference antenna to center point
      ref_pos[1].position := [eas_pos,0] + avg_pos
        # shifts array centeroid by shower position and the latter to the origin
      inrec.dist[i] := ShowerDist([(inrec.AZ[i]/degree),(90-(inrec.ZE[i]/degree))],ref_pos)
        # calculates distance of position to axis originating in origin in direction of AZ & ZE
    } # end for event shower coordinate transformation
   # the distance can not be smaller than the half of the minimum distance between two antennae
   inrec.dist[(inrec.dist<13.5)] := 13.5
             
    ## EPS_EST Horneffer formula
    eps_est := [=]
    eps_est := LOPES30_est(inrec.geoang,inrec.dist,inrec.Nmu);
    hsel := eps_est.value > minFieldEst
    inrec.eps_est_val:=eps_est.value
    inrec.eps_est_err:=eps_est.err

    ## selection
    selar := filear & sizesel & radar & zear & YMDar & reconstar & hsel & extsel;
    print 'Files:',sum(filear),'SIZE:',sum(sizear),'LMUO:',sum(lmuoar),'Size:',sum(sizesel),'Rad:',sum(radar),'ZE:',sum(zear),'YMD:',sum(YMDar),'reconst',sum(reconstar),'FieldEst',sum(hsel),'Extern:',sum(extsel),'  Total:',sum(selar)

    l:= sum(selar);
    if (l>0) {
      print fnum,(fnum+l-1);
      erg.run[fnum:(fnum+l-1)] := inrec.run[selar];
      for (field in field_names(inrec)){
        erg[field][fnum:(fnum+l-1)] :=  inrec[field][selar];
      };
      fnum:=fnum+l;
    };
  };

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

#-----------------------------------------------------------------------
#Name: genCopyScript2
#-----------------------------------------------------------------------
#Type: Function
#Doc: Improved version of the copy-script, now checks if file is there
#     and adds pattern for compressed events
#Ret: 
#-----------------------------------------------------------------------
genCopyScript2 := function(evtab,scriptname='copyscript',tdir='temp'){
  fp := open(paste('>', scriptname));
  for (fname in evtab.filename){
    if (!fexist(concat_dir(tdir,fname))){
     tmp := fname2path(fname);
     write(fp,spaste('cp ',tmp.fullpath,'* ',tdir));
    };
  };
  fp := [=];
  return T;
};


#-----------------------------------------------------------------------
#Name: LOPES30_est
#-----------------------------------------------------------------------
#Type: Function
#Doc: Calculates the expected radio pulse height acording to the Horneffer-Formula v1.3
#     
#Ret: record with:
#                  .value  - The radio pulse height
#                  .err    - The statistical error from the Horneffer-Formula
#Ref: ShowerDist() [data-event-func.g] -> toShower() [phasing.g]
#-----------------------------------------------------------------------
LOPES30_est := function(geoang,dist,Nmu){

  erg := [=]
  erg.value := 43*(1.07-cos(geoang))*exp(dist/-290)*(Nmu/1e6)^1;
  errp1 := ((1.07-cos(geoang))*exp(dist/-290)*(Nmu/1e6)^1)*6.6
  errp2 := (43*exp(dist/-230)*(Nmu/1e6)^1)*0.02
  errp3 := (43*(1.07-cos(geoang))*exp(dist/-290)*(Nmu/1e6)^1)*(dist/(290^2))*97;
  errp4 := (43*(1.07-cos(geoang))*exp(dist/-290)*(Nmu/1e6)^1)*ln(Nmu/1e6)*0.06;
  erg.err := sqrt(errp1^2+errp2^2+errp3^2+errp4^2);

  return erg;
}
