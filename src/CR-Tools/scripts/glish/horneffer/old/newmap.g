#-----------------------------------------------------------------------
#Name: new_skyimage
#-----------------------------------------------------------------------
#Type: Function
#Doc: Generate an image-tool for a given coordsys/shape (or half the sky in 
#     STG projection
#
#Ret: aips++ image-tool 
#
#Par: rec	= 		- Record, where data is stored
#Par: index	= 		- Index of data sets which are to be included
#Par: field     = 'f(x)'	- field to average
#Par: cs 	= F		- coordsys tool to use (F==use default)
#				  needs to have a direction coordinate (2 axes)
#				  and a spectral coordinate (1 axis)
#Par: imshap	= [1,1]		- shape of the image
#Par: nfreq     = 1		- number of frequency channels
#Ref: CalcAntPos, angle2delay
#-----------------------------------------------------------------------

new_skyimage := function(rec,index,field='f(x)',cs=F,imshap=[1,1],nfreq=1){
  
  if ((type_name(cs)!='record')&&(!has_field(cs,'type'))) { #construct default coordsys-tool 
    cs := coordsys(direction=T,spectral=T);
    cs.setreferencecode('AZEL','dir');
    cs.setprojection('STG');
    cs.setreferencevalue(['180 deg','90 deg'],'dir');
    cs.setreferencepixel([15.5,15.5],'dir');
    cs.setincrement(['7.7 deg','7.7 deg'],'dir');
    imshap := [30,30];
  };    
#currently only referencecode AZEL is supported
  cs.setreferencecode('AZEL','dir',adjust=F);
#Set stuff from data-header
  ###conversion to epoch fails because fu*ing aips++ whines about leap seconds
  #cs.setepoch(dm.epoch('utc',gettime(rec,index[1])));
  cs.setepoch(dm.epoch('utc',qnt.quantity(rec.head('Date',1))));
  cs.settelescope(rec.head('Observatory'));   
#Set stuff for spectral part of cs
  FreqBin := rec.headf("Samplerate") * rec.headf("SamplerateUnit")/2./nfreq;
  cs.setreferencepixel(1,'spectral');
  cs.setreferencevalue(dq.quantity(paste((40e6+FreqBin/2),'Hz')),'spectral');
  cs.setincrement(dq.quantity(paste((FreqBin),'Hz')),'spectral');

#this are the arrays to be filled  
  pixarray := array(0,imshap[1],imshap[2],nfreq); #this is the array to be filled
  maskarray := array(T,imshap[1],imshap[2],nfreq);

#go over all pixels and fill them
  imshap := shape(pixarray);
print imshap
  for (x in seq(imshap[1])){
    for (y in seq(imshap[2])){
#cs.toworld gives position in rad (and Frequency in Hz but this is discarded)
      pos_in_rad := cs.toworld([x,y],'n')[1:2];
      AZ := pos_in_rad[1]*180/pi;
      EL := pos_in_rad[2]*180/pi;
      if (EL > 0.) { #only calculate pixel if elevation0.
#        pixarray[x,y,] := get_power_at_position(rec,index,field,nfreq=1,
#                                                azimutangle=AZ,elevation=EL);
pixarray[x,y,] := AZ; #patch, solange "get_power_at_position" nicht geschrieben ist...
      } else {
        maskarray[x,y,] := F;
      };
    };
  };
#build the image tool
  img := imagefromarray(pixels=pixarray,csys=cs);
  img.putregion(pixelmask=maskarray);
  return img;
};



gain_image := function(rec,cs=F,imshap=[1,1],nfreq=1,fstart=4e7,fstop=8e7,timtotal=F,timEW=F){
  
  if ((type_name(cs)!='record')&&(!has_field(cs,'type'))) { #construct default coordsys-tool 
    cs := coordsys(direction=T,spectral=T);
    cs.setreferencecode('AZEL','dir');
    cs.setprojection('ZEA');
    cs.setreferencevalue(['180 deg','90 deg'],'dir');
#    cs.setreferencepixel([106.5,106.5],'dir');
    cs.setreferencepixel([150.5,150.5],'dir');
    cs.setincrement(['.54 deg','.54 deg'],'dir');
#    imshap := [212,212];
    imshap := [300,300];
  };    
#currently only referencecode AZEL is supported
  cs.setreferencecode('AZEL','dir',adjust=F);
#Set stuff from data-header
  ###conversion to epoch fails because fu*ing aips++ whines about leap seconds
  #cs.setepoch(dm.epoch('utc',gettime(rec,index[1])));
  #cs.setepoch(dm.epoch('utc',qnt.quantity(rec.head('Date',1))));
  cs.settelescope(rec.head('Observatory'));   
#Set stuff for spectral part of cs
  cs.setreferencepixel(1,'spectral');
  cs.setreferencevalue(dq.quantity(paste(fstart,'Hz')),'spectral');
  if(nfreq>1) {
    FreqBin := (fstop-fstart)/(nfreq-1)
    fary := ([1:nfreq]-1)*FreqBin+fstart;
    cs.setincrement(dq.quantity(paste((FreqBin),'Hz')),'spectral');
  } else {
    fary := ([1:41]-1)*1e6+fstart;
    cs.setincrement(dq.quantity(paste(((fstop-fstart)/2),'Hz')),'spectral');
  };

#this are the arrays to be filled  
  pixarray := array(0,imshap[1],imshap[2],nfreq); #this is the array to be filled
  maskarray := array(T,imshap[1],imshap[2],nfreq);

#go over all pixels and fill them
  imshap := shape(pixarray);
  lasttime := time()
print imshap
  for (x in seq(imshap[1])){
    for (y in seq(imshap[2])){
#cs.toworld gives position in rad (and Frequency in Hz but this is discarded)
      pos_in_rad := cs.toworld([x,y],'n')[1:2];
      AZ := pos_in_rad[1]*180/pi;
      EL := pos_in_rad[2]*180/pi;
      if (EL > 0.) { #only calculate pixel if elevation0.
#        pixarray[x,y,] := get_power_at_position(rec,index,field,nfreq=1,
#                                                azimutangle=AZ,elevation=EL);
        if(nfreq>1) {
          pixarray[x,y,] := getgains(AZ,EL,fary,globalpar.AntennaGainRec)
        } else if (timtotal){
          pixarray[x,y,1] := get_tim_total(az=(AZ*degree),ze=((90-EL)*degree))
        } else if (timEW){
          pixarray[x,y,1] := tim_EW_strength(az=(AZ*degree),ze=((90-EL)*degree),dist=0)
       } else {
          pixarray[x,y,1] :=mean(getgains(AZ,EL,fary,globalpar.AntennaGainRec))
        }
      } else {
        maskarray[x,y,] := F;
      };
    };
if (x%10 == 0) {
print 'Line',x,'took',time()-lasttime,'seconds'
lasttime := time()
};
  };
#build the image tool
  img := imagefromarray(pixels=pixarray,csys=cs);
  img.putregion(pixelmask=maskarray);
  return img;
};



join_images := function(infiles, outfile='join_images-out.sos'){
  nofiles := len(infiles)
  erg := [=]
  starttime := time();
  img := image(infiles[1])
  if (img.type()!='image') { 
    print infiles[1],'is no image!'
    return F;
  };
  img.getregion(erg.pixels,erg.mask);
  erg.num := as_integer(erg.mask);
  cs := img.coordsys();
  erg.cs := cs.torecord();
  img.done();
  for (i in [2:nofiles]){
    img := image(infiles[i])
    if (img.type()!='image') { 
      print infiles[i],'is no image!'
      return F;
    };
    img.getregion(pixels,mask);
    erg.pixels +:= pixels*mask;
    erg.num +:= mask;
    img.done();
    if (i%10==0) {
      ttime := time()-starttime
      print 'Image',i,'out of',nofiles,' used time:',ttime,'estimated total time:',ttime/i*nofiles;
    };
  };
  erg.pixels /:= erg.num;
write_value(erg,outfile);
  print 'Calculating averaged spectrum.'
  shap := shape(erg.pixels);
  erg.spec := array(0.,shap[3])
  for (i in seq(shap[3])){
    erg.spec[i] := mean(erg.pixels[,,i,1]);
  };
  print 'Calculating araraged pixels.'
  erg.avpix := array(0.,shap[1],shap[2],1,1)
  j := 0;
  for (i in seq(shap[3])[erg.specmask]){
    erg.avpix[,,1,1] +:= erg.pixels[,,i,1]
    j +:=1;
  };  
  erg.avpix /:=j; 
  print j;

  return erg;
};



#-----------------------------------------------------------------------
#Name: gen_ccmat
#-----------------------------------------------------------------------
#Type: Function
#Doc: Generate the cross-correlation matrix (cube) for the files loaded
#     into the gui.
#
#Ret: F
#
#Par: rec	= 		- Record, where data is stored
#Par: index	= 		- Index of data sets which are to be included
#Par: tsize	= 2048		- length of one datablock (i.e. frequency resolution)
#Par: savedir	= '/data/horneff/' - directory where to save the output
#Par: tinc	= F		- number of samples to go forward for each block
#				  (If tinc<tsize then there is some overlap between 
#				  adjacent blocks.)
#Ref: 
#-----------------------------------------------------------------------
gen_ccmat := function(rec,index,tsize=2048,savedir='/data/horneff/',tinc=F){
  nfreq := as_integer((tsize+2)/2);

  ccmat := [=];
  ccmat.filearray := rec.headarr('Files')
  ccmat.epoch := dm.epoch('utc',qnt.quantity(rec.head('Date',1)));
  ccmat.observatory := rec.head('Observatory')
  ccmat.tsize := tsize;
  ccmat.nfreq := nfreq;

  ccrecord := [=];
  ccrecord.files := ccmat.filearray;
  ccrecord.nant := len(ccmat.filearray);
  ccrecord.tsize := tsize;
  if(tinc) { ccrecord.tinc := tinc; }

  clientstring:=concat_dir(globalconst.cprogDir,'dsky');
  dsky_client := client(clientstring);
  ccmat.ccMatrix := dsky_client->generate_ccmat(ccrecord);

  fname := ccmat.filearray[1]~globalconst.FilenameExtractor;
  savename := spaste(savedir,'CC-Matrix-',fname,'.sos');
  write_value(ccmat,savename);

  return F;
}

#-----------------------------------------------------------------------
#Name: skyimage_from_ccmat
#-----------------------------------------------------------------------
#Type: Function
#Doc: Generate an image-tool for a given coordsys/shape from a CC-Matrix
#     currently generates a glish array with the datacube
#
#Ret: aips++ image-tool 
#
#Par: rec	= 		- Record, where data is stored
#Par: ccfile	=		- name (path) of the file with the CC-Matrix
#Par: index	= -1		- Index of data sets which are to be used
#Par: cs 	= F		- coordsys tool to use (F==use default)
#				  needs to have a direction coordinate (2 axes)
#				  and a spectral coordinate (1 axis)
#Par: imshap	= [1,1]		- shape of the image
#Par: savedir	= '/data/horneff/' - directory where to save the output
#Ref: CalcAntPos, angle2delay
#-----------------------------------------------------------------------
skyimage_from_ccmat := function(rec,ccfile,index=-1,cs=F,imshap=[1,1],savedir='/data/horneff/'){
  
  ccmat := read_value(ccfile);
  ccshap := shape(ccmat.ccMatrix);

  nfreq := ccshap[1];
  if ((type_name(cs)!='record')||(!has_field(cs,'type'))) { #construct default coordsys-tool 
    cs := coordsys(direction=T,spectral=T);
    cs.setreferencecode('AZEL','dir');
    cs.setprojection('ZEA');
#    cs.setreferencevalue(['180 deg','90 deg'],'dir');#Nord-Sued tauschen
    cs.setreferencevalue(['00 deg','90 deg'],'dir');
#    cs.setreferencepixel([106.5,106.5],'dir');
    cs.setreferencepixel([150.5,150.5],'dir');
#    cs.setincrement(['.54 deg','.54 deg'],'dir'); #Ost-West tauschen
    cs.setincrement(['-0.54 deg','.54 deg'],'dir');
#    imshap := [212,212];
    imshap := [300,300];
  };    
#currently only referencecode AZEL is supported
  cs.setreferencecode('AZEL','dir',adjust=F);
#Set stuff from data-header
  cs.setepoch(ccmat.epoch);
  cs.settelescope(ccmat.observatory);   
#Set stuff for spectral part of cs
  FreqBin := rec.headf("Samplerate") * rec.headf("SamplerateUnit")/2./nfreq;
  cs.setreferencepixel(1,'spectral');
  cs.setreferencevalue(dq.quantity(paste((40e6+FreqBin/2),'Hz')),'spectral');
  cs.setincrement(dq.quantity(paste((FreqBin),'Hz')),'spectral');

  if (index[1]<0) {
    index := seq(rec.len);
    ccMatrix := ccmat.ccMatrix;
  } else {
    ccMatrix := array(0.+0.i,ccshap[1],len(index),len(index));
    for (i in seq(len(index))){
      for (j in seq(len(index))){
         ccMatrix[,i,j] := ccmat.ccMatrix[,index[i],index[j]];
      };
    };
  };
  ccshap := shape(ccMatrix);
  nant := ccshap[2];

#this are the arrays to be filled  
global  dsky := [=]
  dsky.cs := cs.torecord();
  dsky.pixarray := array(0.,imshap[1],imshap[2],nfreq); #this is the array to be filled
  dsky.maskarray := array(T,imshap[1],imshap[2],nfreq);
  dsky.donearray := array(F,imshap[1],imshap[2]);
  dsky.indexarray := array(0,imshap[1]*imshap[2],2);
  dsky.delayarray := array(0,nant,imshap[1]*imshap[2]);
  

  refant:=rec.get('RefAnt',1);
  doz:=rec.get('DoZ',1);
  xy:=rec.get('XY',1);
  if (xy==[0,0])  antpos:=CalcAntPos(rec,index,refant)
  else antpos:=CalcAntPos(rec,index,refant,refpos=[xy[2],xy[1],0],phasecenter=T)
  fbandw:=rec.headf('Bandwidth',1)*rec.headf('FrequencyUnit',1);
  flowlim:=rec.getmeta('FrequencyLow',1);
  fftlen:=rec.getmeta('FFTlen',1)

  starttime := time()
  lasttime := starttime;
  nopixel:=0;
  for (x in seq(imshap[1])){
    for (y in seq(imshap[2])){
      pos_in_rad := cs.toworld([x,y],'n')[1:2];
      AZ := pos_in_rad[1]*180/pi;
      EL := pos_in_rad[2]*180/pi;
      if (EL > 0.) { #only calculate pixel if elevation0.
        nopixel := nopixel+1;
        dsky.indexarray[nopixel,] := [x,y];
        for (i in seq(nant)){
           dsky.delayarray[i,nopixel]:=calcdelay([AZ,EL],antpos[i],doz,distance=0)+
              rec.get('Corrdelay',antpos[i].antenna)+rec.get('Caldelay',antpos[i].antenna); 
        };
      } else {
        dsky.maskarray[x,y,] := F;
      };      
    };
if (x%10 == 0) {
print 'Line',x,'took',time()-lasttime,'seconds'
lasttime := time()
};
  };
  setuptime := time();
  print 'Duration of setup:',setuptime-starttime,'found',nopixel,'out of',imshap[1]*imshap[2],'pixels';


  nclients:=2;
  clientstring:=[concat_dir(globalconst.cprogDir,'dsky'),concat_dir(globalconst.cprogDir,'dsky')]
  maxpixperclient := [400,400];

  pixclient := [=];
  for (i in [1:nclients]){
    pixclient[i] := client(clientstring[i]);
    whenever pixclient[i]->pixelsdata do {
print 'Got values for IDs:',$value.ids[1],'to',$value.ids[maxpixperclient[$value.clid]];
      xary := dsky.indexarray[$value.ids,1];
      yary := dsky.indexarray[$value.ids,2];
      for (id in seq(len($value.ids))) {
        dsky.pixarray[xary[id],yary[id],] := $value.pixdata[[nfreq:1],id];
        dsky.donearray[xary[id],yary[id]] := T;
      };
      pixclient[$value.clid].running := F;
    };
    pixclient[i].running := F;
    print 'started client ',i;
  };

  savetime := time();
  dsky.filearray := rec.headarr('Files')
  ncalc:=0
  while (ncalc < nopixel) {
    for (clid in [1:nclients]){
      if (ncalc < nopixel){
        npix := min(maxpixperclient[clid],(nopixel-ncalc))
        dskyrec := [=];
        dskyrec.clid := clid;
        dskyrec.ids := [(ncalc+1):(ncalc+npix)];
        dskyrec.npixel := npix;
        dskyrec.nant := nant;
        dskyrec.delays := dsky.delayarray[,dskyrec.ids]
        dskyrec.ccmat := ccMatrix;
        pixclient[clid]->get_pixels_from_ccmat(dskyrec);
        pixclient[clid].running := T;
        ncalc := ncalc+npix;
      };
    };
    for (clid in [1:nclients]){
      if (pixclient[i].running) {
        await pixclient[i]->pixelsdata;
      };
    };
    if ((time()-savetime)>1800){
      savename := spaste(savedir,'dsky-saves-',as_string(sum(dsky.donearray)),'.sos')
      write_value(dsky,savename);
      savetime := time();
    };
  };
  savename := spaste(savedir,'dsky-saves-all.sos')
  write_value(dsky,savename);


#build the image tool
#  img := imagefromarray(pixels=dsky.pixarray,csys=cs);
#  img.putregion(pixelmask=dsky.maskarray);
#  return img;
};




driss_skyimage := function(rec,index,cs=F,imshap=[1,1],tsize=2048,savedir='/data/horneff/'){
  
  nfreq := as_integer((tsize+2)/2);
  if ((type_name(cs)!='record')||(!has_field(cs,'type'))) { #construct default coordsys-tool 
    cs := coordsys(direction=T,spectral=T);
    cs.setreferencecode('AZEL','dir');
    cs.setprojection('ZEA');
    cs.setreferencevalue(['180 deg','90 deg'],'dir');
    cs.setreferencepixel([106.5,106.5],'dir');
#    cs.setreferencepixel([150.5,150.5],'dir');
    cs.setincrement(['.54 deg','.54 deg'],'dir');
    imshap := [212,212];
#    imshap := [300,300];
  };    
#currently only referencecode AZEL is supported
  cs.setreferencecode('AZEL','dir',adjust=F);
#Set stuff from data-header
  cs.setepoch(dm.epoch('utc',qnt.quantity(rec.head('Date',1))));
  cs.settelescope(rec.head('Observatory'));   
#Set stuff for spectral part of cs
  FreqBin := rec.headf("Samplerate") * rec.headf("SamplerateUnit")/2./nfreq;
  cs.setreferencepixel(1,'spectral');
  cs.setreferencevalue(dq.quantity(paste((40e6+FreqBin/2),'Hz')),'spectral');
  cs.setincrement(dq.quantity(paste((FreqBin),'Hz')),'spectral');
  nant := len(index);

#this are the arrays to be filled  
global  dsky := [=]
  dsky.cs := cs.torecord();
  dsky.pixarray := array(0.,imshap[1],imshap[2],nfreq); #this is the array to be filled
  dsky.maskarray := array(T,imshap[1],imshap[2],nfreq);
  dsky.azarray := array(0.,imshap[1],imshap[2]);
  dsky.donearray := array(F,imshap[1],imshap[2]);
  dsky.indexarray := array(0,imshap[1]*imshap[2],2);
  dsky.delayarray := array(0,nant,imshap[1]*imshap[2]);

  refant:=rec.get('RefAnt',1);
  doz:=rec.get('DoZ',1);
  xy:=rec.get('XY',1);
  if (xy==[0,0])  antpos:=CalcAntPos(rec,index,refant)
  else antpos:=CalcAntPos(rec,index,refant,refpos=[xy[2],xy[1],0],phasecenter=T)
  fbandw:=rec.headf('Bandwidth',1)*rec.headf('FrequencyUnit',1);
  flowlim:=rec.getmeta('FrequencyLow',1);
  fftlen:=rec.getmeta('FFTlen',1)

  starttime := time()
  lasttime := starttime;
  nopixel:=0;
  for (x in seq(imshap[1])){
    for (y in seq(imshap[2])){
      pos_in_rad := cs.toworld([x,y],'n')[1:2];
      AZ := pos_in_rad[1]*180/pi;
      EL := pos_in_rad[2]*180/pi;
      dsky.azarray[x,y] := AZ;
      if (EL > 0.) { #only calculate pixel if elevation0.
        nopixel := nopixel+1;
        dsky.indexarray[nopixel,] := [x,y];
        for (i in seq(nant)){
           dsky.delayarray[i,nopixel]:=calcdelay([AZ,EL],antpos[i],doz,distance=0)+
              rec.get('Corrdelay',antpos[i].antenna)+rec.get('Caldelay',antpos[i].antenna); 
        };
      } else {
        dsky.maskarray[x,y,] := F;
      };      
    };
if (x%10 == 0) {
print 'Line',x,'took',time()-lasttime,'seconds'
lasttime := time()
};
  };
  setuptime := time();
  print 'Duration of setup:',setuptime-starttime,'found',nopixel,'out of',imshap[1]*imshap[2],'pixels';



  nclients:=2;
  clientstring:=[concat_dir(globalconst.cprogDir,'dsky'),concat_dir(globalconst.cprogDir,'dsky')]
  maxpixperclient := [100,100];

  pixclient := [=];
  for (i in [1:nclients]){
    pixclient[i] := client(clientstring[i]);
    whenever pixclient[i]->pixelsdata do {
print 'Got values for IDs:',$value.ids;
      xary := dsky.indexarray[$value.ids,1];
      yary := dsky.indexarray[$value.ids,2];
      for (id in seq(len($value.ids))) {
        dsky.pixarray[xary[id],yary[id],] := $value.pixdata[[nfreq:1],id];
        dsky.donearray[xary[id],yary[id]] := T;
      };
      pixclient[$value.clid].running := F;
    };
    pixclient[i].running := F;
    print 'started client ',i;
  };

  savetime := 0.;
  dsky.filearray := rec.headarr('Files')
  ncalc:=0
  while (ncalc < nopixel) {
    for (clid in [1:nclients]){
      if (ncalc < nopixel){
        npix := min(maxpixperclient[clid],(nopixel-ncalc))
        dskyrec := [=];
        dskyrec.clid := clid;
        dskyrec.ids := [(ncalc+1):(ncalc+npix)];
        dskyrec.npixel := npix
        dskyrec.nant := nant;
        dskyrec.tsize := tsize;
        dskyrec.delays := dsky.delayarray[,dskyrec.ids]
        dskyrec.files := dsky.filearray;
        pixclient[clid]->calc_pixels(dskyrec);
        pixclient[clid].running := T;
        ncalc := ncalc+npix;
      };
    };
    for (clid in [1:nclients]){
      if (pixclient[i].running) {
        await pixclient[i]->pixelsdata;
      };
    };
    if ((time()-savetime)>1800){
      savename := spaste(savedir,'dsky-saves-',as_string(sum(dsky.donearray)),'.sos')
      write_value(dsky,savename);
      savetime := time();
    };
  };
  savename := spaste(savedir,'dsky-saves-all.sos')
  write_value(dsky,savename);


#build the image tool
#  img := imagefromarray(pixels=dsky.pixarray,csys=cs);
#  img.putregion(pixelmask=dsky.maskarray);
#  return img;
};





