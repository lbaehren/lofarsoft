########################################################################
#
# functions used by the script controlling the LOPES analysis pipeline
#
########################################################################

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
#Name: newKretainKgr
#-----------------------------------------------------------------------
#Type: Function
#Doc: read in the KRETA data from four files produced with nt/scan from
#     the hbook files and set flag for KASCADE or GRANDE usage
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
#Name: pipeSelectEventsKGr
#-----------------------------------------------------------------------
#Type: Function
#Doc: selects events according to different criteria
#     in the KASCADE-Grande array  
#Ret: record with KRETA data and filenames
#
#-----------------------------------------------------------------------
pipeSelectEventsKGr := function(infiles, minsize=1, maxsize=1e99, minlmuo=1,maxlmuo=1e99,extsel=T,maxze=1.6,minze=-1.,totalsizes=F,goodKASCADE=T,goodGRANDE=F,minYMD=0,maxYMD=999999,posx=0,posy=0,dlen=600){
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
    YMDar := inrec.YMD >= minYMD & inrec.YMD <= maxYMD;
    if ((len(extsel)!=1)&len(extsel)!=len(inrec.filename)) {
      print 'incompatible extsel!',len(extsel),len(inrec.filename);
      return F;
    };
    if ((posx > 100)|(posy > 100)|(posx < -600)|(posy < -500)){
      print 'Fiducial coordinates outside allowed range';
      print 'Inside -600 < x < 100, -500 < y < 100'
      return F;
    };
    selar := filear & YMDar & extsel;
    print 'Files:',sum(filear),'YMD:',sum(YMDar),'Extern:',sum(extsel),' Total:',sum(selar);

   # find best core for usage, KASCADE or Grande
   # if needed, KASCADE overwritten by Grande values
   for (check in 1:len(selar)){
     if ( selar[check]== T ){

      # in between KASCADE and Grande
      if (((inrec.XC[check]<0)|(inrec.YC[check]<0))&(sqrt(inrec.XC[check]^2+inrec.YC[check]^2) < 91)){
#        print 'Between KASCADE and Grande';
        # cores closer then 10 m use KASCADE, or not then Grande
        if ( sqrt((inrec.XC[check]-inrec.xcg[check])^2 + (inrec.YC[check]-inrec.ycg[check])^2) < 10 ){
        } else {
          inrec.XC[check] := inrec.xcg[check];
          inrec.YC[check] := inrec.ycg[check];
          inrec.AZ[check] := inrec.azg[check];
          inrec.ZE[check] := inrec.zeg[check];
          inrec.age[check]  := inrec.ageg[check];
          inrec.SIZE[check] := inrec.sizeg[check];
          inrec.nmu[check]  := inrec.sizmg[check];
          goodKASCADE := F;
          goodGRANDE := T;
        };
      };

      # only Grande good reconstruction
      if ((inrec.XC[check]<-91)|(inrec.YC[check]<-91)){
#        print 'Grande allowed range';
        inrec.XC[check] := inrec.xcg[check];
        inrec.YC[check] := inrec.ycg[check];
        inrec.AZ[check] := inrec.azg[check];
        inrec.ZE[check] := inrec.zeg[check];
        goodKASCADE := F;
        goodGRANDE := T;
      };
     };
    };
    # selection based on fidual area with posx, posy, dlen
    area  := (abs(inrec.XC-posx)<dlen/2) & (abs(inrec.YC-posy)<dlen/2);
    print 'Area:',sum(area);
    border := inrec.XC>-600 & inrec.YC>-500 & inrec.XC<100 & inrec.YC<100;
    print 'Border:',sum(border);
    if (goodKASCADE){ 
      reconstar := (inrec.run > 0) & (inrec.age>0.2) & (inrec.age<2.1);
      } else { reconstar := T };
    if (goodGRANDE){ 
      reconstar := (inrec.run > 0) & (inrec.ageg>0.4) & (inrec.ageg<1.4) & (inrec.trgs>19);
      } else { reconstar := T 
    };
    sizear := inrec.SIZE > minsize & inrec.SIZE < maxsize;
    print 'Size:',sum(sizear);
    lmuoar := inrec.LMUO > minlmuo & inrec.LMUO < maxlmuo;
    print 'Muon:',sum(lmuoar);
    zear := inrec.ZE<maxze & inrec.ZE>minze;
    if (totalsizes){ sizesel := sizear | lmuoar; 
     } else { sizesel := sizear & lmuoar; };

    selar2 := sizesel & zear & selar & area & border & reconstar;

    print 'Files:',sum(filear),'SIZE:',sum(sizear),'LMUO:',sum(lmuoar),'Size:',sum(sizesel),'ZE:',sum(zear),'YMD:',sum(YMDar),'reconst',sum(reconstar),'Extern:',sum(extsel),'  Total:',sum(selar2)

    l:= sum(selar2);
    if (l>0) {
      print fnum,(fnum+l-1);
      erg.run[fnum:(fnum+l-1)] := inrec.run[selar2];
      for (field in field_names(inrec)){
        erg[field][fnum:(fnum+l-1)] :=  inrec[field][selar2];
      };
      fnum:=fnum+l;
    };
  };

  return erg;
}
