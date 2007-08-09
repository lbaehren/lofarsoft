
#-----------------------------------------------------------------------
#Name: round
#-----------------------------------------------------------------------
#Type: Function
#Doc: Round a floating point number to the nearest integer
#Ret: Integer
#
#Par: x				- floating point number
#-----------------------------------------------------------------------
round:=function(x){return floor(x+0.5)}

#-----------------------------------------------------------------------
#Name: oldangle2delay
#-----------------------------------------------------------------------
#Type: Function
#Doc: calculate the time delays for a given direction
#Ret: (1-dim) array with the delays (in s)
#
#Par: posarry				- 2-dim arary with antenna positions 
#						(in m)
#					  1st dim: pos values are North of 0
#					  2nd dim: pos values are East of 0
#					  3rd dim: pos values are above 0
#
#Par: zenithangle			- zenithangle of the direction 
#						(degrees, 0 = zenith)
#Par: azimutangle			- azimut angle of the direction 
#						(degrees, 0 = north, clockwise)
#-----------------------------------------------------------------------

oldangle2delay := function(posarry, azimutangle, zenithangle, refant=1){
	shap := shape(posarry)
	if (shap[2] != 3) {
	  print 'Second dim of position-array has to be [SN,EW,up-down]';
	  return F;
	};
	ze:=zenithangle*pi/180;
	az:=pi/2+(azimutangle*pi/180);	

	l := sin(ze)*sin(az);
	m := -sin(ze)*cos(az);
	n := cos(ze);

	erg := array(0.,shap[1]);
	for ( i in [1:shap[1]] ) {
	  d := posarry[i,1]*l- posarry[i,2]*m+ posarry[i,3]*n;
	  erg[i] := d/lightspeed;
	};
	return erg;
};


#-----------------------------------------------------------------------
#Name: mapsky
#-----------------------------------------------------------------------
#Type: Function
#Doc: Map the entire sky with the antennas given in index at the maximum
#     resolution by phasing the array. Currently assumes that maximum 
#     resolution is in NS direction!
#Example: mapsky(data,[1,2],refant=2,unit=0.4)
#
#Ret: 2dim map in az/el.
#
#Par: rec	= 			- Record, where data is stored
#Par: index	= 			- Index of data sets which are to be
#					  included
#Par: field     = 'f(x)'		- field to average
#Par: nfreq     = 1		        - number of frequency channels
#					  to produce	
#					  
#Par: refant	= 1			- Reference antenna for which delay
#					  is by definition zero
#Par: unit	= 1 			- Input unit of length in meters
#
#Par: distance	= F			- distance of source in meters
#					  (not yet implemented)
#Ref: CalcAntPos, angle2delay
#-----------------------------------------------------------------------
 
mapsky := function(rec,index,field='f(x)',nfreq=1,refant=1,unit=1,distance=F){

#Produce array with proper positions from header information
  antpos:=CalcAntPos(rec,index,refant,unit);
  nant:=len(antpos);
  print 'Using ',nant,' Antennas.'
  samplerate:=data.headf('Samplerate',index[1])*data.headf('SamplerateUnit',index[1]);
#Find Resolution of array in delay pixels/samples. This assumes that
  maxdelay:=max(round(angle2delay(antpos,0,0,distance)*samplerate));
  mindelay:=min(round(angle2delay(antpos,180,0,distance)*samplerate));
  nsteps:=maxdelay-mindelay;
  nasteps:=2*nsteps+1;
  print 'Delay across array from: ',mindelay,' to ',maxdelay,' Samples in ',nasteps,'x',nsteps,' steps.'

  dazm:=180/nsteps;
  delv:=90/(nsteps-1);  
  print 'dazm=',dazm,' delv=',delv
  azel:=array(0,nasteps,nsteps,2);
  pwr:=array(0,nasteps,nsteps,nfreq);
  arylen:=len(rec.get(field,antpos[1].antenna))-nsteps;
  for (nelv in seq(nsteps)) {
    for (nazm in seq(nasteps)) {
    azm:=(nazm-1)*dazm; elv:=(nelv-1)*delv;
    azel[nazm,nelv,1]:=azm; azel[nazm,nelv,2]:=elv;
    delays:=round(angle2delay(antpos,azm,elv,distance)*samplerate);
    ary:=array(0,arylen);  
    for (n in seq(nant)) {
#      print n,delays[n],1+maxdelay-delays[n],1+maxdelay-delays[n]+arylen-1
      ary:=ary+rec.get(field,antpos[n].antenna,start=1+maxdelay+delays[n],end=1+maxdelay+delays[n]+arylen-1)
      }
    ary:=ary/nant; 
    pwr[nazm,nelv,1]:=mean(ary^2);
#    pwr[nazm,nelv,1]:=median(ary^2);
    print 'nazm=',nazm,'nelv=',nelv,'  azm=',azm,', elv=',elv,' delays=',delays,'  pwr=',pwr[nazm,nelv,1]
    }
  }
result:=[=];
result.pwr:=pwr;
result.azel:=azel;
return result
};


#-------------------------------------------------------------------------------
#Name: angle2delay
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  calculate the time delays for a given direction
#Ret:  (1-dim) array with the delays (in s). The delay is positive for events
#      that are later. 
#      (I.e. a pulse from direction [az,el] arrives at times [delays] at the
#      antennas == the time series data at indices [delays] have to be put at 
#      the same position.)
#Par: antpos      - n-dim record with 
#					.antenna: record index of antenna
#				    .position: arary with antenna x,y,z positions (in m)
#                    1: pos values are North of 0
#                    2: pos values are East of 0
#                    3: pos values are above 0
#Par: azimutangle - azimut angle of the direction 
#					(degrees, 0 = north, clockwise)
#                   (90=east, 180=south, 270=west)
#Par: elevation	  - elevation of the direction 
#					(degrees, 90 = zenith)
#Par: distance    - distance of source in meters
#                   (not yet implemented)
#Rem: Later should also include a distance to a source in the nearfield!
#Ref: CalcAntPos
#-----------------------------------------------------------------------
 
angle2delay := function(antpos, azimutangle, elevation, distance=F)
{
  nant := len(antpos)
  el:=elevation*pi/180;
  az:=azimutangle*pi/180;	

  x := cos(el)*cos(az);
  y := cos(el)*sin(az);
  z := sin(el);

  erg := array(0.,nant);
  for (i in [1:nant]) {
    d := -1*(antpos[i].position[1]*x + antpos[i].position[2]*y + antpos[i].position[3]*z);
    erg[i] := d/lightspeed;
  };

  return erg;
};


#-------------------------------------------------------------------------------
#Name: CalcAntPos
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Calculate the positons in x,y,z, coordinates relative to a reference
#      antenna. A number of subsequent translations can be performed, to obtain
#      the antenna positions w.r.t. to a user-defined reference position:
#      <ol>
#        <li>Translation to a coordinate system centered on a reference antenna;
#        by default this is antenna 1, marking the origin of the antenna array 
#        based coordinate system; 'refant' must mark a valid antenna, otherwise
#        not shift is performed.
#        <li>Translation to a coordinate system centered on position of the
#        Kascade central detector, as measured w.r.t. the origin of the antenna
#        based coordinate system (i.e. antenna 1); applied if 'phasecenter=T'.
#        <li>Translation to an (arbitrary) reference position, as defined w.r.t.
#        the origin of the antenna based coordinate system (i.e. antenna 1);
#        applied if the 'refpos' is a vector of length 3.
#      </ol>
#Par:  rec	   =    - Record, where data is stored
#Par:  index       = -1 - Index of data sets which are to be included; if set to
#                         '-1' the antenna positions of all selected datasets
#                         will be returned.
#Par:  refant      = 1  - Reference antenna for which delay is by definition zero
#Par:  unit        = 1  - Input unit of length in meters
#Par:  phasecenter = F  - Shift reference antenna positions to phase center
#Par:  refpos      = F  - A 3-dim vector. This position is used as a reference
#                         position relative to the reference antennas.
#                         Logically, the position is relative to the array
#                         (phase) center if phasecenter=T.
#Ret: antpos            - 3-dim arary with antenna positions (in m)
#                          1st dim: pos values are North of 0 (Y-Axis!)
#                          2nd dim: pos values are East of 0  (X-Axis!)
#                          3rd dim: pos values are above 0    (Z-Axis)
#Ref: angle2delay
#Example: xy:=data.get('XY'); antpos := CalcAntPos(data,index=[1:8],refpos=[xy[2],xy[1]])
#-----------------------------------------------------------------------

CalcAntPos := function(rec, index=-1, refant=1, unit=1, phasecenter=F,
                       refpos=F, verboseON=F)
{
  j:=0;
  refpos0 := [0,0,0];
  antpos  := [=];

  if (index==-1) index := [1:rec.len][rec.select()];

  # ------------------------------------------------------------------
  # (1) GPS measurement position of the reference antenna; by default
  #     this is antenna 1, marking the origin of the antenna array 
  #     based coordinate system; 'refant' must mark a valid antenna,
  #     otherwise not shift is performed.

  refantPos := StringToVector(rec.head('AntPos',max(refant,1)));

  if (refant>0) refpos0 +:= refantPos;

  # ------------------------------------------------------------------
  # (2) Position of the phase center w.r.t. the origin of the antenna
  #     array based coordinate system, i.e. the position of the Kascade
  #     central detector w.r.t. to antenna 1.

  centeroffPos := rec.get('Center',max(refant,1));

  if (phasecenter) refpos0 +:= centeroffPos;

  # ------------------------------------------------------------------
  # (3) Consideration of an (arbitrary) reference position, as defined 
  #     w.r.t. reference antenna of the LOPES array.

  if (len(refpos)==3) refpos0 +:= refpos;

  # ------------------------------------------------------------------
  # Debugging output (if enabled)

  if (verboseON) {
    print "[map::CalcAntPos]";
    print " - Selected antennae ........ :",index;
    print " - Reference antenna ........ :",refant;
    print " - Phase center ............. :",phasecenter;
    print " - Reference position ....... :",refpos;
    print " - Center offset position ... :",centeroffPos;
    print " - Total position shift ..... :",refpos0;
  }

  # compute the relative antenna positions

  for (i in index) {
    j+:=1;
    antpos[j]:=[=];
    antpos[j].antenna:=i;
    antpos[j].position:=StringToVector(rec.head('AntPos',i));

    if (len(antpos[j].position) != 3) {
      print '[phasing::CalcAntPos] Error: position (',rec.head('AntPos',i),')',
            ' of Antenna ',i,' is not a 3dim array. Position set to zero!';
      antpos[j].position:=[0,0,0];  
      }
    else {
      antpos[j].position:=(antpos[j].position-refpos0)*unit;
    }
  }

  return antpos;
}

#-------------------------------------------------------------------------------
#Name: StringToVector
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Converts a string with numbers separated by `sep' into a vector array 
#Ret:  Numerical vector
#Par:  sary	=      - String with elements of an array
#Par:  sep	= ','  - characters separating elements
#-------------------------------------------------------------------------------

StringToVector := function(sary,sep=',')
{
  i:=0; 
  for (s in split(sary,sep)) {
      i+:=1;
      pos[i]:=as_float(s)
  }
  return pos
}

#-----------------------------------------------------------------------
#Name: gettime
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Return the time and date of an observation from the Header
#      variables as a quantity
#
#Ret:  quantity (record)
#
#Par:  rec	=      - Record, where data is stored
#Par:  index	= 1    - Index of data set to be used
#-----------------------------------------------------------------------
gettime:=function(ref rec,index=1){
return qnt.quantity(spaste(rec.head('Date',index) ~ s/[.]/\//g,'/',rec.head('Time',index)))
}

#-----------------------------------------------------------------------
#Name: skyimage
#-----------------------------------------------------------------------
#Type: Function
#Doc: Generate an image-tool of the entire sky with the antennas given in 
#     index with the angular resolution given in res (degrees)
#
#Example: skyimage(data,[1,2],res=10,refant=2,unit=0.4)
#
#Ret: aips++ image-tool 
#
#Par: rec	= 			- Record, where data is stored
#Par: index	= 			- Index of data sets which are to be
#					  included
#Par: field     = 'f(x)'		- field to average
#Par: res       = 10			- angular resolution in degrees
#Par: nfreq     = 1		        - number of frequency channels
#					  to produce
#					  
#Par: refant	= 1			- Reference antenna for which delay
#					  is by definition zero
#Par: unit	= 1 			- Input unit of length in meters
#
#Par: distance	= F			- distance of source in meters
#					  (not yet implemented)
#Ref: CalcAntPos, angle2delay
#-----------------------------------------------------------------------

skyimage := function(rec,index,field='f(x)',res=10,nfreq=1,refant=1,
		unit=1,distance=F) {
  if (len(index)<1) {return F;};
#Produce array with proper positions from header information
  antpos:=CalcAntPos(rec,index,refant,unit);
  nant:=len(antpos);
  print 'Using ',nant,' Antennas.'
  samplerate:=data.headf('Samplerate',index[1])*data.headf('SamplerateUnit',index[1]);
#get min/max delays; assumes the antennas are roughly in a horizontal plane
  Ndelay := round(angle2delay(antpos,0,0,distance)*samplerate);
  Edelay := round(angle2delay(antpos,90,0,distance)*samplerate);
  Sdelay := round(angle2delay(antpos,180,0,distance)*samplerate);
  Wdelay := round(angle2delay(antpos,270,0,distance)*samplerate);
  maxdelay:=max(Ndelay,Edelay,Sdelay,Wdelay,0)+2;
  mindelay:=min(Ndelay,Edelay,Sdelay,Wdelay,0)-2;
  nsteps:=maxdelay-mindelay;
  size := len(rec.get(field,antpos[1].antenna));
  arylen:=size-nsteps;
  numblocks := as_integer(rec.getmeta('FileBlocksize',index[1])/size);

  azsteps := as_integer(360/res); zeroaz := as_integer(azsteps/2);
  elsteps := as_integer(90/res);
#elsteps := 6; #azsteps := azsteps+1;

  pixarray := array(0,azsteps,elsteps,nfreq);

  tmpindex := rec.newentry(head=rec.get('Header',index[1]));
  ci := rec.get('Coordinfo',tmpindex);
  ci[field].IsInput := T;
  ci[field].Stored := T;
  ci := rec.put(ci,'Coordinfo',tmpindex);
  rec.setregion([1,arylen],'T',tmpindex);
  rec.putmeta(arylen,'FileBlocksize',tmpindex);
  rec.putmeta(arylen,'Blocksize',tmpindex);
  for (nblk in [0:(numblocks-1)]) {
    datacache := [=];
j:=0;
    for (nelv in [1:elsteps] ) {
      elv:=((nelv-1)*res)+(res/2-1);
      for (nazm in seq(azsteps) ) {
        azm:=((nazm-zeroaz)*res)-res/2;
        delays:=round(angle2delay(antpos,azm,elv,distance)*samplerate); 
        delayhash := spaste(as_string(delays));
#print elv, azm, delays;
        if (! has_field(datacache,delayhash) ) { 
	print 'Calculating Pixel ',j,' Az:',azm,'El:',elv;
          ary:=array(0,arylen);  
          for (n in seq(nant)) {
            ary:=ary+rec.get(field,antpos[n].antenna,start=((nblk*size)+1-mindelay+delays[n]),end=  ((nblk*size)-mindelay+delays[n]+arylen))
          };
          ary:=ary/nant; 
          rec.put(ary,field,tmpindex); 
          for (ff in rec.coord(tmpindex)) {rec.touch(ff,tmpindex,version=0)}
          rec.touch(field,tmpindex);
          Tary := rec.get('NoiseT',tmpindex);
          l := len(Tary);
          for (freqno  in seq(nfreq) ) {
            tmp[freqno] := mean(Tary[ceil(((freqno-1)*l/nfreq)+1):floor(freqno*l/nfreq)]);
          };        
          datacache[delayhash] := tmp;
        };
j:=j+1;
        pixarray[nazm,nelv,] := pixarray[nazm,nelv,] + datacache[delayhash];
      };
    };
write_value(pixarray,spaste('/data/tmp/pixarray',(nblk+1),'.sos'));
print 'Calculated Data for ', j, ' Pixels',nblk,numblocks;
    if (is_agent(rec.newsagent)) {rec.newsagent->message(sprintf('skyimage: done with %i out of %i blocks',(nblk+1),numblocks))};
  };
  pixarray := pixarray/numblocks;
#construct the coordsys-tool
  #cs := coordsys(direction=T,sp); 
  cs := coordsys(direction=T,spectral=T);
### direction coordinates
  cs.settelescope('LOFAR-ITS');   # Needs to be changed!!!(Define a LOPES-telescope)
  #cs.settelescope('EFFEL');   
  cs.setepoch(dm.epoch('utc',qnt.quantity(rec.head('Date',1))))
  #cs.setepoch(dm.epoch('utc',gettime(rec,index[1])));
  cs.setreferencecode('AZEL','dir');
  cs.setprojection('CAR');
#  cs.setreferencevalue(['0deg','0deg']);
#  cs.setreferencevalue([spaste((-res/2),'deg'),spaste((res/2-1),'deg')]);
  cs.setreferencevalue([spaste((-res/2),'deg'),'0deg'],'dir');
#  cs.setreferencevalue([spaste((-res/2),'deg'),'0deg'],'dir');
  cs.setincrement([spaste(res,'deg'),spaste(res,'deg')],'dir');
#  cs.setreferencepixel([zeroaz, 1],'dir');
  cs.setreferencepixel([zeroaz, 0.5],'dir');
### spectral coordinate
  fary := rec.get('Frequency',tmpindex); l:=len(fary);
  cs.setreferencepixel(1,'spectral');
  cs.setreferencevalue(dq.quantity(spaste(fary[as_integer(l/nfreq/2)],'Hz')),'spectral');
  cs.setincrement(dq.quantity(spaste(((fary[l] - fary[1])/nfreq),'Hz')),'spectral');
  cs.summary();

  rec.del(tmpindex); #delete temporary entry
  return imagefromarray(pixels=pixarray,csys=cs);
};


#-----------------------------------------------------------------------
#Name: allskycs
#-----------------------------------------------------------------------
#Type: coordsys-tool
#Doc: An aips++ coordsys-tool, that covers the whole sky, with the J2000
#     reference-code. To use with the imager-tool.
#
#Example: newimage := oldimage.regrid(csys=allskycs,method='nearest',shape=[358,179]);
#-----------------------------------------------------------------------
allskycs := coordsys(direction=T);
allskycs.setreferencevalue(['0deg','0deg']);
allskycs.setprojection('CAR');
allskycs.setreferencecode('J2000','dir',adjust=F);
allskycs.setreferencepixel([180, 90],'dir');
allskycs.setincrement(['1deg','1deg'],'dir');


#-----------------------------------------------------------------------
#Name: annotated_map
#-----------------------------------------------------------------------
#Type: Function
#Doc: Display an image of the entire sky including some known sources.
#
#Example: annotated_map(data,[1,2],res=10,refant=2,unit=0.4)
#
#Ret: record with the different aips++ tools 
#
#Par: rec	= 			- Record, where data is stored
#Par: index	= 			- Index of data sets which are to be
#					  included
#Par: field     = 'f(x)'		- field to average
#Par: res       = 10			- angular resolution during generation
#Par: nfreq     = 1		        - number of frequency channels
#					  to produce (not yet implemented)
#					  
#Par: refant	= 1			- Reference antenna for which delay
#					  is by definition zero
#Par: unit	= 1 			- Input unit of length in meters
#
#Par: planetlist  = "sun moon jupiter"	- list of "planets" to include
#Par: sourcecat  = "defsourcecat.tbl"	- skycatalog of the sources to include
#
#Ref: skyimage
#-----------------------------------------------------------------------
tempimage.n:=F;
annotated_map := function(ref rec,index,field='f(x)',res=10,nfreq=1,refant=1,
          unit=1,planetlist="Sun Moon Jupiter",sourcecat="defsourcecat.tbl") {
  global tempimage;
  erg := [=];
  tmpimg := skyimage(ref rec,index,field=field,res=res,nfreq=nfreq,refant=refant,
                     unit=unit,distance=F);
  cs := tmpimg.coordsys();
  cs.setreferencevalue(['0deg','0deg']);
  cs.setprojection('CAR');
#  cs.setreferencecode('J2000','dir',adjust=F);
  cs.setreferencecode('GALACTIC','dir',adjust=F);
  cs.setreferencepixel([180, 90],'dir');
  cs.setincrement(['1deg','1deg'],'dir');
  cs.summary();
  imshap := tmpimg.shape();
  imshap[1:2] := [358,179]

  if (is_function(tempimage.delete)) {
    if (is_function(tempimage.dp.done))  tempimage.dp.done();
    tempimage.delete();
  };
  tempimage :=  tmpimg.regrid(outfile='/var/tmp/tmpimage',overwrite=T,csys=cs,axes=[1,2],method='nearest',shape=imshap);
  erg.img := tempimage;
  cs := tmpimg.coordsys();
  dm.doframe(cs.epoch());
  dm.doframe(cs.telescope(T));

  fd := open('> tmpskycat.tab');
  fprintf(fd,'Name RA DEC DirType\n');
  fprintf(fd,'A  D D  A\n');
  for (planet in planetlist) {
    tmppos := dm.measure(dm.direction(planet),'J2000');
    fprintf(fd,'%s %fdeg %fdeg J2000\n', planet ,
      dq.convert(dm.getvalue(tmppos)[1],'deg').value,   
      dq.convert(dm.getvalue(tmppos)[2],'deg').value);
  };
  fd := F;

  shell('rm -rf tmpskycat.tbl')
  sca := skycatalog('tmpskycat.tbl');
  sca.fromascii('tmpskycat.tab',hasheader=T,longcol='RA',latcol='DEC',dirtype='J2000');
  sca.done();

  titelstring := paste(rec.head('Project',index[1]),rec.head('Date',index[1]),rec.head('Time',index[1]));
  erg.dp := dv.newdisplaypanel()
tempimage.dp := erg.dp;
  erg.ddimag := dv.loaddata(erg.img,'raster')
  erg.dp.register(erg.ddimag)
  erg.ddimag.setoptions([axislabelswitch=[value=T],
                xgridtype=[value='Full grid'],ygridtype=[value='Full grid'],
                titletext=[value=titelstring],spectralunit=[value='MHz']])

  if (planetlist) {
    erg.dtplanets := dv.loaddata('tmpskycat.tbl','skycatalog')
    erg.dp.register(erg.dtplanets)
    erg.dtplanets.setoptions([namecolumn=[value="Name"],labelcharsize=[value=1.3]]);
  };

  if (sourcecat) {
    erg.dtsources := dv.loaddata(sourcecat,'skycatalog')
    erg.dp.register(erg.dtsources)
    erg.dtsources.setoptions([namecolumn=[value="Name"],labelcharsize=[value=1.1]]);
  };

  return erg
};

#-----------------------------------------------------------------------
#Name: annotate_image
#-----------------------------------------------------------------------
#Type: Function
#Doc: Display an image of the entire sky including some known sources.
#
#Example: annotated_map(data,[1,2],res=10,refant=2,unit=0.4)
#
#Ret: record with the different aips++ tools 
#
#Par: rec	= 			- Record, where data is stored
#Par: index	= 			- Index of data sets which are to be
#					  included
#Par: field     = 'f(x)'		- field to average
#Par: res       = 10			- angular resolution during generation
#Par: nfreq     = 1		        - number of frequency channels
#					  to produce (not yet implemented)
#					  
#Par: refant	= 1			- Reference antenna for which delay
#					  is by definition zero
#Par: unit	= 1 			- Input unit of length in meters
#
#Par: planetlist  = "sun moon jupiter"	- list of "planets" to include
#Par: sourcecat  = "defsourcecat.tbl"	- skycatalog of the sources to include
#
#Ref: skyimage
#-----------------------------------------------------------------------
annotate_image:=function(ref img,planetlist="Sun Moon Jupiter Venus Mars",sourcecat="defsourcecat.tbl",refframe='J2000') {
  erg := [=];

  erg.img := img;
  cs:=img.coordsys();
  dm.doframe(cs.epoch())
  dm.doframe(dm.observatory(cs.telescope()));

  fd := open('> tmpskycat.tab');
  fprintf(fd,'Name RA DEC DirType\n');
  fprintf(fd,'A  D D  A\n');
  for (planet in planetlist) {
    tmppos := dm.measure(dm.direction(planet),refframe);
    fprintf(fd,'%s %fdeg %fdeg %s\n', planet ,
      dq.convert(dm.getvalue(tmppos)[1],'deg').value,   
      dq.convert(dm.getvalue(tmppos)[2],'deg').value,
      refframe);
  };
  fd := F;

  shell('rm -rf tmpskycat.tbl')
  sca := skycatalog('tmpskycat.tbl');
  sca.fromascii('tmpskycat.tab',hasheader=T,longcol='RA',latcol='DEC',dirtype=refframe);
  sca.done();

#  titelstring := paste(rec.head('Project'),rec.head('Date'),rec.head('Time'));
  titelstring:='Annotated Skymap';
  erg.dp := dv.newdisplaypanel()
  erg.ddimag := dv.loaddata(erg.img,'raster')
  erg.dp.register(erg.ddimag)
  erg.ddimag.setoptions([axislabelswitch=[value=T],
                xgridtype=[value='Full grid'],ygridtype=[value='Full grid'],
                titletext=[value=titelstring],spectralunit=[value='MHz'],
                colormap=['Hot Metal 1']]);

  if (planetlist) {
    erg.dtplanets := dv.loaddata('tmpskycat.tbl','skycatalog')
    erg.dp.register(erg.dtplanets)
    erg.dtplanets.setoptions([namecolumn=[value="Name"],labelcharsize=[value=1.3]]);
  };

  if (sourcecat) {
    erg.dtsources := dv.loaddata(sourcecat,'skycatalog')
    erg.dp.register(erg.dtsources)
    erg.dtsources.setoptions([namecolumn=[value="Name"],labelcharsize=[value=1.1]]);
  };
  return erg
};


#-----------------------------------------------------------------------
#Name: merge_pixarrays
#-----------------------------------------------------------------------
#Type: Function
#Doc: merge three n-dim arrays (e.g. pixarrays) with active values!=0
#     of the same shape
#
#Ret: merged array
#
#Par: ary(1|2|3)           - input arrays, have to be of the same shape!
#-----------------------------------------------------------------------
merge_pixarrays := function(ary1,ary2,ary3=F){
  shap := shape(ary1);
  if (!all(shap==shape(ary2))) { 
    print 'Shape of ary1 and ary2 differ! Aborting!';
    return F;
  };
  print 'Merging ary1 and ary2, with shape:',shap;
  erg := ary1;
  erg[ary2!=0] := ary2[ary2!=0];
  if (all(shap==shape(ary3))) { 
    print 'Merging ary3 to erg';
    erg[ary3!=0] := ary3[ary3!=0];
  };
  return erg;
};

#-----------------------------------------------------------------------
#Name: shrink_pixarray
#-----------------------------------------------------------------------
#Type: Function
#Doc: shrink a 3-dim pixarray in the frequency-axis
#
#Ret: new array
#
#Par: inary           	- input array
#-----------------------------------------------------------------------
shrink_pixarray := function(inary,nchan=1,frange=[0,0],normalize=F) {
  shap := shape(inary);
  if ((frange[1]<1)||(frange[1]>shap[3])) {frange[1]:=1}; 
  if ((frange[2]<1)||(frange[2]>shap[3])) {frange[2]:=shap[3]}; 
  global normfakt := array(1.,shap[3]);
  if (normalize) { #calculate real normalizations-faktors;
    for (i in [frange[1]:frange[2]]){
      normfakt[i] := mean(inary[,,i]);
    };
    base := min(normfakt[frange[1]:frange[2]]);
    normfakt := base/normfakt;
  };
  print 'regridding out of',frange,'into',nchan,'channels; Normrange:',range(normfakt);
  erg := array(0.,shap[1],shap[2],nchan);
  l:=frange[2]-frange[1]+1;
  for (i in seq(shap[1])) {
    for (j in seq(shap[2])) {
      for (k in seq(nchan)) {
#print i,j,ceil(((k-1)*l/nchan)+frange[1]),floor(k*l/nchan+frange[1]-1)
        erg[i,j,k] := mean(inary[i,j,ceil(((k-1)*l/nchan)+frange[1]):floor(k*l/nchan+frange[1]-1)]*normfakt[ceil(((k-1)*l/nchan)+frange[1]):floor(k*l/nchan+frange[1]-1)]);
      };
    };
  };

  return erg;
};

#-----------------------------------------------------------------------
#Name: gencs
#-----------------------------------------------------------------------
#Type: Function
#Doc: generate a coordinate-system object
#
#Ret: cs-object
#
#Par: 
#-----------------------------------------------------------------------
gencs := function(inary,telescope='EFFEL',date='today'){
  shap := shape(inary);
  nfreq := shap[3];
  res := 90/shap[2];
  if (nfreq>1) { #include a spectral coordinate;
    cs := coordsys(direction=T,spectral=T);
    ### spectral coordinate
    fary := [40e3:80e3]*1000; l:=len(fary);
    cs.setreferencepixel(1,'spectral');
    cs.setreferencevalue(dq.quantity(spaste(fary[as_integer(l/nfreq/2)],'Hz')),'spectral');
    cs.setincrement(dq.quantity(spaste(((fary[l] - fary[1])/nfreq),'Hz')),'spectral');
  } else {
    print 'Length of spectral dimension <=1. No spectral coordonate included!';
    cs := coordsys(direction=T);
  };
### direction coordinates
  cs.settelescope(telescope); 
  cs.setepoch(dm.epoch('utc',qnt.quantity(date)))
  cs.setreferencecode('AZEL','dir');
  cs.setprojection('CAR');
  cs.setreferencevalue([spaste((-res/2),'deg'),'0deg'],'dir');
  cs.setincrement([spaste(res,'deg'),spaste(res,'deg')],'dir');
  zeroaz := as_integer(as_integer(360/res)/2);
  cs.setreferencepixel([zeroaz, 0.5],'dir');

  return cs;
};

########################################################################
# Initialization-Stuff
########################################################################



########################################################################
#EXPERIMENTAL!
########################################################################

#- cs := im.coordsys(axes=[1,2])
#- im.done()
#- im2 := imagefromarray(pixels=array(1.0, 32, 64), csys=cs)
#- cs.done()

#Specify position of Bonn (actually EB)
#obspos:=cnv.position('ITRF','4033947.4796m','486990.5252m','4900430.8024m')
#qnt.form.long(obspos)

#Set current frame to observation time:
#obstime:=cnv.epoch('utc',gettime(data,1))
#obstime:=cnv.epoch('utc',qnt.quantity('2002/6/23/18:00:00'))
#cnv.doframe(obstime)
#cnv.doframe(obspos)

#VLA: cnv.position('ITRF','-1601185.2077m','-5041977.1729m','3554875.7033m')
#Compare with cnv.observatory('VLA')
#Define a direction as AZEL
#cnv.direction('AZEL','0d','0d')
#Define position of the sun
#cnv.measure(cnv.direction('sun'),'AZEL')


#res:=mapsky(data,seq(2),refant=2,unit=0.4)
#map:=imagefromarray(pixels=res.pwr)
#map.view()

#









skyimage2 := function(rec,index,field='f(x)',res=10,nfreq=1,refant=1,
		unit=1,distance=F,doels=F) {
  if (len(index)<1) {return F;};
#Produce array with proper positions from header information
  antpos:=CalcAntPos(rec,index,refant,unit);
  nant:=len(antpos);
  print 'Using ',nant,' Antennas.'
  samplerate:=data.headf('Samplerate',index[1])*data.headf('SamplerateUnit',index[1]);
#get min/max delays; assumes the antennas are roughly in a horizontal plane
  Ndelay := round(angle2delay(antpos,0,0,distance)*samplerate);
  Edelay := round(angle2delay(antpos,90,0,distance)*samplerate);
  Sdelay := round(angle2delay(antpos,180,0,distance)*samplerate);
  Wdelay := round(angle2delay(antpos,270,0,distance)*samplerate);
  maxdelay:=max(Ndelay,Edelay,Sdelay,Wdelay);
  mindelay:=min(Ndelay,Edelay,Sdelay,Wdelay);
  nsteps:=maxdelay-mindelay;
  size := len(rec.get(field,antpos[1].antenna));

  azsteps := as_integer(360/res); zeroaz := as_integer(azsteps/2);
  elsteps := as_integer(90/res);
#elsteps := 6; #azsteps := azsteps+1;

  elarry := [1:elsteps];
  if (doels) {
    print 'Doing',doels,'elevation steps out of',elarry;
    elarry := doels;
  };

  pixarray := array(0,azsteps,elsteps,nfreq);
          
  pixelrec.nch := nant;
  pixelrec.nfreq := nfreq;
  pixelrec.bsize := size;
  pixelrec.files := array('',nant);
  for (n in [1:nant]) {
    pixelrec.files[n] := rec.head('Files',antpos[n].antenna)
  };

  FreqBin := rec.headf("Samplerate") * rec.headf("SamplerateUnit")/size;
  eichfaktor := ((rec.headf('MaxVolt')/ rec.headf('ADCMaxChann')/size*2)^2)/50/FreqBin/1.3806e-23;;
  FreqBin := FreqBin*(size/2.)/nfreq;
  datacache := [=];
j:=0;
  for (nelv in elarry) {
    elv:=((nelv-1)*res)+(res/2-1);
    for (nazm in seq(azsteps) ) {
      azm:=((nazm-zeroaz)*res)-res/2;
      delays:=round(angle2delay(antpos,azm,elv,distance)*samplerate); 
      delayhash := spaste(as_string(delays));
      j:=j+1;
#print elv, azm, delays;
      if (! has_field(datacache,delayhash) ) { 
	pixelrec.delays := delays;
	print 'Calculating Pixel ',j,' Az:',azm,'El:',elv;
        datacache[delayhash] := global_lopestools_client->calc_pixel(pixelrec)[nfreq:1];
      };
      pixarray[nazm,nelv,] := datacache[delayhash]*eichfaktor;
    };
  };
write_value(pixarray,spaste('/data/tmp/',rec.head('Filename',index[1]),as_string(doels),'-2nd_pixarray.sos'));
#construct the coordsys-tool
  #cs := coordsys(direction=T,sp); 
  cs := coordsys(direction=T,spectral=T);
### direction coordinates
#  cs.settelescope('LOFAR-ITS');   # Needs to be changed!!!(Define a LOPES-telescope)
  cs.settelescope('EFFEL');   
  cs.setepoch(dm.epoch('utc',gettime(rec,index[1])));
  cs.setreferencecode('AZEL','dir');
  cs.setprojection('CAR');
#  cs.setreferencevalue(['0deg','0deg']);
#  cs.setreferencevalue([spaste((-res/2),'deg'),spaste((res/2-1),'deg')]);
  cs.setreferencevalue([spaste((-res/2),'deg'),'0deg'],'dir');
#  cs.setreferencevalue([spaste((-res/2),'deg'),'0deg'],'dir');
  cs.setincrement([spaste(res,'deg'),spaste(res,'deg')],'dir');
#  cs.setreferencepixel([zeroaz, 1],'dir');
  cs.setreferencepixel([zeroaz, 0.5],'dir');
### spectral coordinate
  cs.setreferencepixel(1,'spectral');
  cs.setreferencevalue(dq.quantity(spaste((40e6+FreqBin/2),'Hz')),'spectral');
  cs.setincrement(dq.quantity(spaste((FreqBin),'Hz')),'spectral');
  cs.summary();

  return imagefromarray(pixels=pixarray,csys=cs);
};















#img := skyimage3(data,[1:8],res=5,refant=1,unit=1,nfreq=10000)
skyimage3 := function(rec,index,field='f(x)',res=10,nfreq=1,refant=1,
		unit=1,distance=F) {
  if (len(index)<1) {return F;};
#Produce array with proper positions from header information
  antpos:=CalcAntPos(rec,index,refant,unit);
  nant:=len(antpos);
  print 'Using ',nant,' Antennas.'
  samplerate:=data.headf('Samplerate',index[1])*data.headf('SamplerateUnit',index[1]);
#get min/max delays; assumes the antennas are roughly in a horizontal plane
  Ndelay := round(angle2delay(antpos,0,0,distance)*samplerate);
  Edelay := round(angle2delay(antpos,90,0,distance)*samplerate);
  Sdelay := round(angle2delay(antpos,180,0,distance)*samplerate);
  Wdelay := round(angle2delay(antpos,270,0,distance)*samplerate);
  maxdelay:=max(Ndelay,Edelay,Sdelay,Wdelay);
  mindelay:=min(Ndelay,Edelay,Sdelay,Wdelay);
  nsteps:=maxdelay-mindelay;
  size := len(rec.get(field,antpos[1].antenna));

  azsteps := as_integer(360/res); zeroaz := as_integer(azsteps/2);
  elsteps := as_integer(90/res);
#elsteps := 6; #azsteps := azsteps+1;

          
  pixelrec.nch := nant;
  pixelrec.nfreq := nfreq;
  pixelrec.bsize := size;
  pixelrec.files := array('',nant);
  for (n in [1:nant]) {
    pixelrec.files[n] := rec.head('Files',antpos[n].antenna)
  };


nclients:=2;clientstring:=['c-progs/lopestools','c-progs/lopestools'];
  pixclient:= [=];
  datacache := [=];
  for (i in [1:nclients]){
    pixclient[i] := client(clientstring[i]);
    pixclient[i].running := F;
    whenever pixclient[i]->pixeldata do {
      local ind := i;
      delayhash := spaste(as_string($value.delays));
      datacache2[delayhash] := $value.pixdata[nfreq:1];
      print 'got pixdata for delays: \"',delayhash,'\" len: ',len($value.pixdata),' from: ',$value.id;
      pixclient[$value.id].running := F;
    };
    print 'started client ',i;
  };
  

  ncalc:=0;npix:=0;
  delary := array('',azsteps,elsteps);
  for (nelv in [1:elsteps] ) {
    elv:=((nelv-1)*res)+(res/2-1);
    for (nazm in seq(azsteps) ) {
      npix:=npix+1;
      azm:=((nazm-zeroaz)*res)-res/2;
      delays:=round(angle2delay(antpos,azm,elv,distance)*samplerate); 
      delayhash := spaste(as_string(delays));
#print elv, azm, delays;
      if (! has_field(datacache,delayhash) ) { 
        datacache[delayhash] := T;
        ncalc:=ncalc+1;
	pixelrec.delays := delays;
	print 'Calculating Pixel ',ncalc,' out of',npix,' Az:',azm,'El:',elv,'del',delayhash,'!';
        jobstarted:=F;
        for (i in [1:nclients]){
          if (!jobstarted & !pixclient[i].running) {
            pixelrec.id:=i;
            pixclient[i]->calc_pixel(pixelrec);
            pixclient[i].running := T;
            jobstarted:=T;
            #print 'started job on client ',i;
          };
        };
        if (!jobstarted) {
          await pixclient[1]->pixeldata, pixclient[2]->pixeldata;
          for (i in [1:nclients]){
            if (!jobstarted & !pixclient[i].running) {
              pixelrec.id:=i;
              pixclient[i]->calc_pixel(pixelrec);
              pixclient[i].running := T;
              jobstarted:=T;
              #print 'started job on client ',i;
            };
          };
        };
        delary[nazm,nelv] := delayhash;
      };
    };
  };
  FreqBin := rec.headf("Samplerate") * rec.headf("SamplerateUnit")/size;
  eichfaktor := ((rec.headf('MaxVolt')/ rec.headf('ADCMaxChann')/size*2)^2)/50/FreqBin/1.3806e-23;;
  FreqBin := FreqBin*(size/2.)/nfreq;
  pixarray := array(0,azsteps,elsteps,nfreq);
  for (nelv in [1:elsteps] ) {
    for (nazm in seq(azsteps) ) {
      pixarray[nazm,nelv,] := datacache2[(delary[nazm,nelv])]*eichfaktor;
    };
  };

write_value(pixarray,spaste('/data/tmp/',rec.head('Filename',index[1]),'-3rd_pixarray.sos'));
#construct the coordsys-tool
  #cs := coordsys(direction=T,sp); 
  cs := coordsys(direction=T,spectral=T);
### direction coordinates
#  cs.settelescope('LOFAR-ITS');   # Needs to be changed!!!(Define a LOPES-telescope)
  cs.settelescope('EFFEL');   
  cs.setepoch(dm.epoch('utc',gettime(rec,index[1])));
  cs.setreferencecode('AZEL','dir');
  cs.setprojection('CAR');
#  cs.setreferencevalue(['0deg','0deg']);
#  cs.setreferencevalue([spaste((-res/2),'deg'),spaste((res/2-1),'deg')]);
  cs.setreferencevalue([spaste((-res/2),'deg'),'0deg'],'dir');
#  cs.setreferencevalue([spaste((-res/2),'deg'),'0deg'],'dir');
  cs.setincrement([spaste(res,'deg'),spaste(res,'deg')],'dir');
#  cs.setreferencepixel([zeroaz, 1],'dir');
  cs.setreferencepixel([zeroaz, 0.5],'dir');
### spectral coordinate
  cs.setreferencepixel(1,'spectral');
  cs.setreferencevalue(dq.quantity(spaste((40e6+FreqBin/2),'Hz')),'spectral');
  cs.setincrement(dq.quantity(spaste((FreqBin),'Hz')),'spectral');
  cs.summary();

  return imagefromarray(pixels=pixarray,csys=cs);
};

#------------------------------------------------------------------------
#Name: map_survey
#Doc: Displays an image of a radio all-sky survey (here 4008 MHz)
#    which was regridded to match in size and coordinates an arbitrary
#    input image. The epoch is also taken from the input image.
#
#Par: img  - input image tool with valid coordinate system
#     infile - filename of the survey in globalconst.SysDataDir;
#------------------------------------------------------------------------
map_survey:=function(ref img,infile='EB-408MHz-3deg-Car.fits'){
#construct the coordsys-tool
  global img408MHz;
  erg:=[=];
  dir:=globalconst.SysDataDir;
  if (!dexist(dir)) { print 'Directory',dir,'is missing from your lopestools installation! Cannot generate 408 MHz map.'; return}
  infile:=concat_dir(dir,infile);
  tmp1file:=concat_dir(dir,'tmp1.img');
  tmp2file:=concat_dir(dir,'tmp2.img');
  mapfile:=concat_dir(dir,'skymap.img');
  outfile:=infile ~ s/\.fits/\.img/
#  if (is_image(img1)) img1.done();
  img1:=imagefromfits(tmp1file,infile,overwrite=T) 
  shp:=img1.shape();
  box:=drm.box(blc=[2,2,1],trc=[shp[1],shp[2],1]);
#  if (is_image(img2)) img2.done();
  img2:=imagefromimage(outfile=tmp2file,infile=tmp1file,region=box,overwrite=T)
  img1.delete();
  cs2:=img2.coordsys();
  cs0:=img.coordsys();
  incr:=cs2.increment('s')
  cs := coordsys(direction=T,spectral=T);
  cs.settelescope('LOFAR-ITS'); 
  cs.setepoch(cs0.epoch());
  cs.setreferencecode('J2000','dir');
  cs.setprojection('CAR');
  cs.setreferencevalue(['0deg','0deg'],'dir');
  cs.setincrement([incr[1],incr[2]],'dir');
  cs.setreferencepixel([(shp[1]-1)/2, (shp[2]-1)/2],'dir');
  cs.setreferencepixel(1,'spectral');
  cs.setreferencevalue(dq.quantity(spaste(408e6,'Hz')),'spectral');
  cs.setincrement(dq.quantity('1e6Hz'),'spectral');
  cs.summary();
  img2.setcoordsys(cs);
  if (is_image(img408MHz)) img408MHz.done();
  shp:=img.shape()[1:3]; shp[3]:=1;
  img408MHz:=img2.regrid(outfile,shape=shp,csys=cs0,axes=[1,2],overwrite=T)
  img2.delete();
  img408MHz.view(axislabels=T);

#  erg.dp := dv.newdisplaypanel()
#  erg.ddimag := dv.loaddata(erg.img,'raster')
#  erg.dp.register(erg.ddimag)
#  erg.ddimag.setoptions([axislabelswitch=[value=T],
#                xgridtype=[value='Full grid'],ygridtype=[value='Full grid'],
#                titletext=[value=titelstring],spectralunit=[value='MHz'],                    colormap=['Hot Metal 1']])
 return T
}

#img2.view(adjust=T,axislabels=T)
#img3.view(adjust=T,axislabels=T)

