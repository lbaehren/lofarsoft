#######################################################################
#
# *Database Kernel* Definitions of methods etc.
#
########################################################################


########################################################################
#Section: Functions
#This are some of the basic functions that are used to manipulate the
#'data' object.
########################################################################

#-----------------------------------------------------------------------
#Name: delrecaux
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Delete an auxilliary data structure (which before was created e.g.
#      by beamforming)
#Par:  rec   - Reference to the data structure.
#-----------------------------------------------------------------------

delrecaux := function (ref rec){
  if (has_field(rec,'aux')) {delrecaux(rec.aux); val rec.aux:=F}
}

#-----------------------------------------------------------------------
#Name: newdata
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Initializes a new datarecord
#Par:  rec   - Reference to the data structure.
#Par:  type  - Type of which the new data record will be
#-----------------------------------------------------------------------

newdata := function(ref rec,type='TIM40') {
    tmpagent := [=];
    if (is_agent(rec.newsagent)) tmpagent:=rec.newsagent;

    if (len(data)==1) val rec:=[=];
    rec.newsagent:=tmpagent;

#initialize the CalTable cache
    rec.CalTable := [=];
    rec.CalTable.gaincal := [=];
    rec.CalTable.phasecal := [=];
    rec.CalTable.gaincal.dates := [=];
    rec.CalTable.phasecal.dates := [=];

###NEW
# rec.ref contains a string with the name of a reference record that will be used as a
# data source if the record is derived (e.g., rec.ref='data').
    rec.ref:=F

#-----------------------------------------------------------------------
#Name: data.addflag
#-----------------------------------------------------------------------
#Type: Function
#Doc: multiplies the specified flagarry with the current existing flagarray
#
#Par: ary         - array of real numbers that will be multiplied with the existing flagarray
#Par: field="FFT" - field name in data structure
#Par: index=1    - number of data set to be used (-1=all).
#Ret: T
#Ref: data.putflag, data.flagdat, data.unflag, data.getflag
#-----------------------------------------------------------------------
rec.addflag:=function(const ary, field="FFT",index=-1) {
  wider rec
  if (index < 0) {idx:=rec.selant()} else {idx:=index};
  for (i in idx) { rec.putflag(rec.getflag(field,i)*ary,field,i) };
  return T
};
    
#-----------------------------------------------------------------------
#Name: data.putflag
#-----------------------------------------------------------------------
#Type: Function
#Doc:  defines a flagging array that will be applied in the specified domain by
#      multiplying the data with the flag array (can also be a scalar)
# Note there is only one flag array per Fourier domain (i.e. one for frequency and 
# one for time series data.
#
#Par: ary        - array of real numbers that will be multiplied with the data in 'field'
#Par: field='FFT' - field/variable name in data, this is mainly used to determine
#                   the Fourier domain (Frequency or Time).
#Par: index=1    - number of data set to be used (-1=all).
#Ret: T
#Ref: data.flagdat, data.unflag, data.getflag
#-----------------------------------------------------------------------
    rec.putflag:=function(const ary, field='FFT',index=-1) {wider rec;
       if (index < 0) {idx:=seq(rec.len)} else {idx:=index};
       for (i in idx) {
         domain:=rec.coordinfo(field,i)['Domain']
         if (len(ary)==1) {
           rec.data[i].Flag[domain]:=ary;
           arysum := rec.getlen(field,i);
         } else {
           ary2:=array(ary,rec.getlen(field,i));
           rec.data[i].Flag[domain]:=ary2;
           arysum := sum(ary2^2);
         }
         rec.put(T,'Doflag',i)
         #The fields to touch are hardcoded. Not extremly elegant, but better 
         #than recomputing _everything_! (A.Horneffer)
         if (domain == 'T') {  
           rec.touchref('f(x)',i)
           rec.putmeta(arysum,'PowerNormFaktor',i);
         } else if (domain == 'F') { 
           rec.touchref('FFT',i)
         } else { 
           rec.touchref(field,i)
         };
     }
    return T
    };

#-----------------------------------------------------------------------
#Name: data.unflag
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.unflag:=function(field='FFT',index=-1) {wider rec;
       rec.putflag(1,field,index)
    };

#-----------------------------------------------------------------------
#Name: data.getflag
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.getflag:=function(field='FFT',index=1) {wider rec; 
         if (!has_field(rec.data[index],'Flag')) return 1
         domain:=rec.coordinfo(field,index)['Domain']
         if (!has_field(rec.data[index].Flag,domain)) return 1
         return rec.data[index].Flag[domain]
};

#-----------------------------------------------------------------------
#Name: data.flagdat
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.flagdat:=function(ref ary,field='FFT',index=1) {wider rec; 
      if (rec.coordinfo(field,index)['DefAxis']=='y') {
         fary:=rec.getflag(field,index);
         if (len(fary)==len(ary)) {
           return ary*fary;
         } else {
           return ary;
         };
      } else {
     return ary;
     }
    };

#-----------------------------------------------------------------------
#Name: data.setphase
#-----------------------------------------------------------------------
#Type: Function
#Doc:  undocumented!!<br>
#      <b>Note:</b> this typically may correct for the z-component and the
#       corrdelays even if azel=0,90, so the delays will be non-zero
#Par:  field     - Data field on which to apply the phases.
#Par:  index     - Set of antennae, for the phases are set.
#Par:  tozenith  - Use zenith as reference direction?
#Par:  norecalc  - Recalculate the phase gradient values?
#Par:  verboseON - Enable/disable verbose mode during computation
#Ret:  T
#Ref:  data.put,TIM40convFFT2PhaseGrad,calcphasegradrec,data.touchref
#-----------------------------------------------------------------------

rec.setphase := function(field='FFT',index=-1,tozenith=F,norecalc=F,
                         verboseON=F)
{
  wider rec;
  local ary;

  if (index < 0) {idx:=seq(rec.len)} else {idx:=index};

  if (verboseON) {
    print '[data-kern::rec.setphase] Adjusting data set',idx;
    print ' - Filesize ......................... :',rec.getmeta('Filesize')
    print ' - File Blocksize ................... :',rec.getmeta('FileBlocksize')
    print ' - Blocksize ........................ :',rec.getmeta('Blocksize')
    print ' - Input size of the FFT ............ :',rec.getmeta('FFTlen');
    print ' - Output size of the FFT ........... :',rec.getmeta('FFTSize');
    print ' - Length of the phase gradient array :',len(rec.get('PhaseGrad'));
    print ' - Length of the stored raw FFT ..... :',len(rec.get('RawFFT'));
    print ' - Stored phase gradients region .... :',rec.data[1].Metadata.PhaseGradstoredregion;
    print ' - Do shift? ........................ :',rec.getary('Doshift');
    print ' - To Zenith? ....................... :',tozenith;
  }

  for (i in idx) {
    rec.put(F,'Doshift',i);
    if (norecalc) {
      ary := TIM40convFFT2PhaseGrad([],rec,index=i);
    } else {
      ary:=calcphasegradrec(rec,index=i,tozenith=tozenith);
    };
    rec.put(ary,'PhaseGrad',i);
    rec.put(T,'Doshift',i)
    # This should do the job. ('FFT' will be recomputed next time.)
    rec.touch(rec.get('Input',i),i);
    # Well this doesn't really work, FFT and RawFFT also need to be touched
    # for some weird reasons...
#    rec.touch('RawFFT',i);
#    rec.touch('FFT',i);
#    global data
    if (globalpar.has_gainvalues && (is_numeric(rec.get('AntennaGain',i)))) {
      rec.putmeta(globalpar.FieldStrengthMultiplier/sqrt(mean(rec.get('AntennaGain',i))),'FieldStrengthFactor',i)
};

##################################################################
# I don't know, what this is supposed to achieve. But it makes our
# programm recompute _everything_. (A.Horneffer)
##################################################################
# That is exactly what it is supposed to do. Somehow the version 
# mechanism is not working properly and this is the brute force patch.
# (H. Falcke)
###################################################################
         for (f in rec.coord(i)) {
           if (has_field(rec.coordinfo(f,i),'IsInput') && rec.coordinfo(f,i)['IsInput']) {
             if (has_field(rec.coordinfo(f,i),'CrossRef') && rec.coordinfo(f,i)['CrossRef']!='-') {
                rec.ref.touchref(rec.coordinfo(f,i)['CrossRef'],i)
             } else {
              rec.touchref(f,i)
             };
           };
         };
       }
    };
### NEW END

#Define functions for manipulating the data records

#-----------------------------------------------------------------------
#Name: data.newentry
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.newentry:=function(head=[=]) {wider rec;
         CreateEntry(ref rec, head=head);};

#-----------------------------------------------------------------------
#Name: data.put
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.put:=function(   const ary,field='f(x)'  ,index=0) {wider rec;
        if (index<0) {idx:=seq(rec.len)} else if (index==0) {idx:=rec.defant()} else {idx:=index};
        for (i in idx) {PutDataToMem(rec,  ary=ary,field=field,index=i)}}

#-----------------------------------------------------------------------
#Name: data.putary
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.putary:=function(const ary,field='f(x)'){wider rec; for (i in ind(ary)) {rec.put(ary[i],field=field,index=i)} }

#-----------------------------------------------------------------------
#Name: data.get
#-----------------------------------------------------------------------
#Type: Function
#Ret: data vector or data variable
#Doc: This function is the basic method to access data vectors and variables
#     stored in the data object. 
#Par: field	= 'f(x)'	- Coordinate axis ('Pos','f(x)',...) or variable to
#                         to retrieve.
#Par: index	= 0			- Index (i.e. Antennas number) of data set to be
#					      retrieved. (index=0: use default antenna ind data.defant()).
#Par: start = -1        - Start with 'start' element if 'field' is a vector.
#Par: end = -1          - End with 'end' element if 'field' is a vector. The returned
#                         vector will have a length of (end-start+1).
# 
#Example: data.get('Frequency',2) will return an array of Frequency values (channels)
#         for antenna number 2. <br>
#         data.get('AZEL',2) will return the currently stored AZEL position of that
#         antenna.
#
#Ref: data.put, data.getn, data.defant, GetDataDispatch 
#-----------------------------------------------------------------------

rec.get := function(field='f(x)', index=0, start=-1, end=-1)
{
  wider rec;
  if (index==0) {idx:=rec.defant()} else {idx:=index};
  return GetDataDispatch(rec,field=field,index=idx,start=start,end=end);
}

#-----------------------------------------------------------------------
#Name: data.getn
#-----------------------------------------------------------------------
#Type: Function
#Ret: One element of data-vector.
#Doc: This function returns the nth element of a data vector. This is 
#     the same as data.get(field,index,start=n,end=n).
#
#Par: field	= 'f(x)'	- Coordinate axis ('Pos','f(x)',...) or variable to
#                         to retrieve.
#Par: index	= 0			- Index (i.e. Antennas number) of data set to be
#					      retrieved. (index=0: use default antenna ind data.defant()).
#Par: start = -1        - Start with 'start' element if 'field' is a vector.
#Par: end = -1          - End with 'end' element if 'field' is a vector. The returned
#                         vector will have a length of (end-start+1).
# 
#Ref: data.getval, data.put, data.get, data.defant, GetDataDispatch 
#-----------------------------------------------------------------------
    rec.getn:=function(     field='f(x)'  ,index=0    ,n=1) {wider rec;
      if (index==0) {idx:=rec.defant()} else {idx:=index};
      return GetDataVal(rec,field=field,index=idx,n=n)
}

#-----------------------------------------------------------------------
#Name: data.getlen
#-----------------------------------------------------------------------
#Type: Function
#Ret: Integer
#Doc: Returns the length of a data vector.
#Par: field	= 'f(x)'	- Coordinate axis ('Pos','f(x)',...) or variable to
#                         to retrieve.
#Par: index	= 0			- Index (i.e. Antennas number) of data set to be
#					      retrieved. (index=0: use default antenna ind data.defant()).
#
#Ref: data.get
#-----------------------------------------------------------------------
    rec.getlen:=function(field='f(x)',  index=0) {
      if (index==0) {idx:=rec.defant()} else {idx:=index};
      wider rec;len(rec.get(field,idx))}

#-----------------------------------------------------------------------
#Name: data.getxn
#-----------------------------------------------------------------------
#Type: Function
#Ret: Integer
#Doc: Returns the position of the value x in the data vector 'field'. E.g.,
#     n:=data.getxn(32e6,'Frequency') will tell you that 32 MHz is at the
#     nth position in the Frequency vector; data.getn('Power',index,n) 
#     will then give you the power in that channel.
#Par: x -  value to looks for
#Par: field	= 'f(x)'	- Coordinate axis ('Pos','f(x)',...) or variable to
#                         to retrieve.
#Par: index	= 0			- Index (i.e. Antennas number) of data set to be
#					      retrieved. (index=0: use default antenna ind data.defant()).
#Ref: data.get, data.getn, GetPosFromXCoord, data.defant
#-----------------------------------------------------------------------
    rec.getxn:=function(x,field='f(x)',  index=1) {wider rec;
                  if (index==0) {idx:=rec.defant()} else {idx:=index};
      	          return GetPosFromXCoord(rec,x,field=field,index=idx)}

#-----------------------------------------------------------------------
#Name: data.x2y
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.x2y:=function(x,fieldx='Pos',fieldy='f(x)',index=1) {wider rec;
	          return rec.getn(field=fieldy,index=index,n=rec.getxn(x,field=fieldx,index=index))}

#-----------------------------------------------------------------------
#Name: data.getrec
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.getrec:=function(field) {wider rec; return FieldToRecord(rec,field)}

#-----------------------------------------------------------------------
#Name: data.getary
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.getary:=function(field) {wider rec; return FieldToArray(rec,field)}

#-----------------------------------------------------------------------
#Name: data.del
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.del:=function(index=1) {wider rec;
        DeleteData(ref rec,index=index) }

#-----------------------------------------------------------------------
#Name: data.modified
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.modified:=function(f,i=1)
	{return (rec.version(f,i) != rec.versionref(f,i))};

#-----------------------------------------------------------------------
#Name: data.touchref
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.touchref:=function(f='Dummy',index=-1) {
      if (len(index)==1 && index<0) {idx:=seq(rec.len)} else {idx:=index}
      for (i in idx) {
       if (has_field(rec.coordinfo(f,i),'CrossRef') && rec.coordinfo(f,i).CrossRef!='-') {
        rec.ref.touchref(rec.coordinfo(f,i).CrossRef,i);
#        rec.ref.touch(rec.coordinfo(f,i).CrossRef,i);
       } else if (has_field(rec.coordinfo(f,i),'Ref') && rec.coordinfo(f,i).Ref!='-') {
        rec.touchref(rec.coordinfo(f,i)['Ref'],i);
#        rec.touch(rec.coordinfo(f,i)['Ref'],i);
       } else rec.touch(f,i);
      }
    }

#-----------------------------------------------------------------------
#Name: data.touch
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.touch:=function(             field='Dummy'   ,index=-1    ,version=-1){wider rec;
        return ChangeVersion(ref rec,field=field,index=index,version=version)}


#-----------------------------------------------------------------------
#Name: data.getval
#-----------------------------------------------------------------------
#Type: Function
#Ret: Scalar
#Doc: Sames as data.getn
#Ref: data.getn
#-----------------------------------------------------------------------
    rec.getval:=rec.getn;

#-----------------------------------------------------------------------
#Name: data.fill
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.fill:=function(        fields=''    ,index=0    ,fill=0){wider rec;
        return FillDataRec(rec,fields=fields,index=index,fill=fill)}

#-----------------------------------------------------------------------
#Name: data.copy
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.copy:=function(        from=1   ,torec=0) {wider rec;
        return CopyDataRec(rec,from=from,torec=torec)}

#-----------------------------------------------------------------------
#Name: data.head
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.head:=function(          field=''   ,index=1) {wider rec;
        return GetDataHeader(rec,field=field,index=index)}

#-----------------------------------------------------------------------
#Name: data.puthead
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.puthead:=function(const ary, field=''   ,index=1) {wider rec;
       if (len(index)==1 && index<0) {idx:=seq(rec.len)} else {idx:=index}
        return PutDataHeader(rec,ary,field=field,index=idx)}

#-----------------------------------------------------------------------
#Name: data.headf
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.headf:=function(                  field=''   ,index=1) {wider rec;
        return as_float(GetDataHeader(rec,field=field,index=index))}

#-----------------------------------------------------------------------
#Name: data.headarr
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.headarr:=function(field='',header=T) { FieldToArray(rec,field,header=header)};

#-----------------------------------------------------------------------
#Name: data.summary
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.summary:=function(        index=0) {wider rec;
      if (index==0) {idx:=rec.defant()} else {idx:=index};
        return ListDataHeader(rec,index=idx)}

#-----------------------------------------------------------------------
#Name: data.getmeta
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.getmeta:=function(        field=''   ,index=0) {wider rec;
      if (index==0) {idx:=rec.defant()} else {idx:=index};
              return GetMetaFromMem(rec,field=field,index=idx)}

#-----------------------------------------------------------------------
#Name: data.putmeta
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.putmeta:=function(const inp    ,field=''   ,index=0) {wider rec;
      if (index==0) {idx:=rec.defant()} else {idx:=index};
	  return PutMetaToMem(rec,inp=inp,field=field,index=idx)}

#------------------------------------------------------------------------
#Name: data.select
#------------------------------------------------------------------------
#Type: function
#Ret: bool array or F
#Doc: If called without paramters it returns a boolean array 
#     indicating whether a data set was selected or
#     deselected. The array is ordered by data set number. If, e.g., the
#     first element is F, data set number one should be ignored in processing.
#     If a specific index is given, the data set will be selected or deselcted
#     according to the second parameter.
#
#Par: index=0   - the atenna/dataset number (-1 = set all, 0=return all) 
#Par: flag=T    - select (T) or deselect(F) dataset
#------------------------------------------------------------------------
    rec.select:=function(index=0,flag=T) {wider rec; 
      if (index==0) return FieldToArray(rec,'Select')
      rec.put(flag,'Select',index) 
    }

#------------------------------------------------------------------------
#Name: data.selected
#------------------------------------------------------------------------
#Type: function
#Ret: T/F
#Doc: Returns whether a data set was selected or not.
#
#Par: index   - the antenna/dataset number 
#------------------------------------------------------------------------
    rec.selected:=function(i) {wider rec; return rec.get('Select',i)}


#-----------------------------------------------------------------------
#Name: data.setregion
#-----------------------------------------------------------------------
#Type: Function
#Ret:  T
#Doc:  undocumented!!
#Ref:  SetRegion
#-----------------------------------------------------------------------
    rec.setregion:=function( region=[-1,-1],domain='T'   ,index=1    ,rno=1) {wider rec;
	  return SetRegion(rec,region=region ,domain=domain,index=index,rno=rno)}

#-----------------------------------------------------------------------
#Name: data.getregion
#-----------------------------------------------------------------------
#Type: Function
#Ret:  T
#Doc:  undocumented!!
#Ref:  GetRegion
#-----------------------------------------------------------------------
    rec.getregion:=function( domain='T'   ,index=1    ,rno=1) {wider rec;
	  return GetRegion(rec,domain=domain,index=index,rno=rno)}

#------------------------------------------------------------------------
#Name: data.setblock
#------------------------------------------------------------------------
#Type: Glish function
#Doc:  Sets the current block of data to be read from file or memory.
#      Can also set the blocksize. Will recalculate regions and other
#      meta information.
#Par:  n         - Number the datablock to be displayed in the
#                  PlotControl window.
#Par:  blocksize - Size of a datablock in samples.
#Par:  offset    - Offset from the start of the dataset, in samples.
#Par:  index     - Index of the datasets to which to apply this command.
#Ret:  ok - Boolean indicating status of SetBlock
#Ref:  SetBlock
#------------------------------------------------------------------------

rec.setblock:=function(n=-1, blocksize=-1,  offset=-1, index=-1)
{
  wider rec;
  return SetBlock(rec,n=n,blocksize=blocksize,offset=offset,index=index)
};

#------------------------------------------------------------------------
#Name: data.nextblock
#------------------------------------------------------------------------
#Type: function
#Ret: F
#Doc: Advances to the next data block in the file (default)
#     or jump by n blocks (pos or negative directions possible).
#     This will just set the block description parameters. 
#     You will have to call 'get' to actually access the block.
#Par: n=1       - the number of blocks to advance
#Par: index=-1  - the atenna/dataset number (-1 = all) 
#------------------------------------------------------------------------
    rec.nextblock:=function(n=1, index=-1) {wider rec;
        if ( index == -1) index:=[1:rec.len][rec.select()];
#print 'data-kern.g: rec.nextblock: Blocknumber:',rec.getmeta('Blocknum')+n,(rec.getmeta('Blocknum')+n)*rec.getmeta('Blocksize')
        for (i in index) {
          EOF:=SetBlock(rec,n=rec.getmeta('Blocknum',i)+n,index=i);
          EOF:=EOF || EOF;
        }
    return EOF
  };

#------------------------------------------------------------------------
#Name: data.doframe
#Doc: Sets the observers frame to the values specified in the data set.
#------------------------------------------------------------------------
    rec.doframe:=function(refant=1){wider rec
      dm.doframe(dm.epoch('utc',qnt.quantity(rec.head('Date',refant))))
      dm.doframe(dm.observatory(rec.head('Observatory',refant)));
    } 

#------------------------------------------------------------------------
#Name: data.setdate
#Doc: Sets the observers time and date, stores the original value, and recall it
#     if called without parameters.
#------------------------------------------------------------------------
    rec.setdate:=function(inp=-1){
      wider rec
      if (as_integer(inp)==-1) {date:=F} else {date:=as_time(inp)};
      if (date) {
        if (has_field(rec.get('Header'),'OriginalDate')) {
          rec.puthead(date,'Date',-1);
          rec.doframe();
        } else {
          rec.puthead(rec.head('Date'),'OriginalDate',-1);
          rec.puthead(date,'Date',-1);
          rec.doframe()} 
        }
      else {
        if (has_field(rec.get('Header'),'OriginalDate')) {
          rec.puthead(date:=rec.head('OriginalDate'),'Date',-1);
          rec.doframe();
        } else return T
      }
    print 'Setting new Date and Time to ',rec.head('Date');
    return T
  }

#------------------------------------------------------------------------
#Name: data.resetdir
#------------------------------------------------------------------------
#Type: Glish function
#Doc:  Sets the direction information to standard information
#      (AZEl->zenith, Distance to infinity, etc.)
#Ref: data.setphase, 'AZEL', etc.
#------------------------------------------------------------------------

rec.resetdir := function(index=-1,resetdistance=T,resetxy=T)
{
  wider rec
  if ( index == -1) index:=[1:rec.len]
 
  for (i in index) {
    rec.put([0,90],'AZEL',i);
    if (resetdistance) {rec.put(0,'Distance',i)};
    if (resetxy) {rec.put([0,0],'XY',i)};
    rec.put(1,'RefAnt',i);
    rec.touchref('FFT',i);
    rec.setphase(index=i);
  }
if (has_field(rec,'plotgui') && has_field(rec.plotgui,'refreshgui')) {rec.plotgui.refreshgui()}
} 

#------------------------------------------------------------------------
#Name: data.resetblockinfo
#------------------------------------------------------------------------
#Type: function
#Doc:  Sets the block information to standard information
#Ref:  data.saveblockinfo,data.restoreblockinfo
#------------------------------------------------------------------------

rec.resetblockinfo := function(index=-1)
{
  wider rec
  if ( index == -1) index:=[1:rec.len]
  for (i in index) {
    rec.setblock(n=1,blocksize=min(2^17,rec.getmeta('FileBlocksize',i)),offset=0,index=i)
  }
if (has_field(rec,'plotgui') && has_field(rec.plotgui,'refreshgui')) {rec.plotgui.refreshgui()}
} 


#------------------------------------------------------------------------
#Name: data.defant
#Doc: Returns sefault antenna data set to be used for getting basic parameters in guis or other programs.
#------------------------------------------------------------------------
    rec.defant:=function(){return min([1:rec.len][rec.select()])
};

#-----------------------------------------------------------------------
#Name: data.fieldexists
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.fieldexists:=function(field,index=1) {wider rec; return fieldexists(rec,field=field,index=index)}
###NEW

#-----------------------------------------------------------------------
#Name: data.coordinfos
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.coordinfos:=function(i=1) {rec.get('Coordinfo',i)}


#-----------------------------------------------------------------------
#Name: data.coordinfo
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.coordinfo:=function(f,i=1) {rec.get('Coordinfo',i)[f]}

#-----------------------------------------------------------------------
#Name: data.coord
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.coord:=function(i=1) {return field_names(rec.get('Coordinfo',i))}

#-----------------------------------------------------------------------
#Name: data.fields
#-----------------------------------------------------------------------
#Type: Function
#Doc:  List the fields in the data structure/record, that can be
#      accessed via the 'data.get()' method.
#Par:  i = 1 - Index of the dataset, for which to obtain the list.
#Ret:  T
#Ref:  data.get
#-----------------------------------------------------------------------

rec.fields := function(i=1) {wider rec; return field_names(rec.get('',i))}

#-----------------------------------------------------------------------
#Name: data.metafields
#-----------------------------------------------------------------------
#Type: Function
#Doc:  List the fields in the data structure/record, that can be
#      accessed via the 'data.getmeta()' method.
#Par:  i = 1 - Index of the dataset, for which to obtain the list.
#Ret:  T
#Ref:  data.fields, data.getmeta, data.putmeta
#-----------------------------------------------------------------------
rec.metafields := function(i=1) {wider rec; return field_names(rec.data[i].Metadata)}

#-------------------------------------------------------------
#Name: data.dirvariables
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Gives a list of important parameters which are relevant for calculating
# delays and directions. They can be stored with data.savedirection.
#
#Ret:  T
#Ref:  data.get, data.restoredirection,data.resetdir, data.savedirection, data.fields
#-----------------------------------------------------------------------
rec.dirvariables := function() {return ['RefAnt','AZEL','DoZ','Center','Distance','XY','XCYC']}

#-------------------------------------------------------------
#Name: data.blockvariables
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Gives a list of important parameters which are relevant for setting
# block information. They can be stored with data.saveblockinfo
#
#Ret:  record
#Ref:  data.getmeta, data.restoreblockinfo, data.saveblockinfo, data.getblockinfo
#-----------------------------------------------------------------------
rec.blockvariables := function() {return ['Blocknum','Blocksize','ZeroOffset','FFTSize','FFTlen','FreqBin','FrequencyLow','FrequencyHigh','PowerNormFaktor']}


#-------------------------------------------------------------
#Name: data.getdirection
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Retrieves a record with important parameters that are relevant for calculating
# delays and directions. They can be stored with data.savedirection.
#
#Ret:  record
#Ref:  data.get, data.dirvariables, data.restoredirection,data.resetdir, data.savedirection, data.fields
#-----------------------------------------------------------------------
rec.getdirection := function(index=1,fields='All') {
  if (index == -1) index:=[1:rec.len]
  if (fields == 'All') fields:=rec.dirvariables()
  save:=[=];
  for (f in fields) {save[f]:=rec.get(f,index)}
  return save
}

#-------------------------------------------------------------
#Name: data.getblockinfo
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Retrieves a record with important parameters that are relevant for setting
# block information. They can be stored with data.saveblockinfo.
#
#Ret:  T
#Ref:  data.getmeta, data.restoreblockinfo, data.saveblockinfo, data.getblockinfo
#-----------------------------------------------------------------------
rec.getblockinfo := function(index=1,fields='All') {
  if (index == -1) index:=[1:rec.len]
  if (fields == 'All') fields:=rec.blockvariables()
  save:=[=];
  for (f in fields) {save[f]:=rec.getmeta(f,index)}
  return save
}


#-------------------------------------------------------------
#Name: data.savedirection
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Saves some important parameters which are relevant for calculating
# delays and directions. They can be recalled with data.restoredirection.
#
#Par:  i = -1 - Index of the dataset, for which to save the values. -1 indicates
#          all antennas.
#
#Par:  fields = 'All' - fields to save. 
# If fields='All' all relevant fields will be stored. Otherwise only the fields
# given in the list are saved. Fields that can be used are those that are
# accessible through data.get adn which are listed by data.fields.
# Currently 'All' means ['RefAnt','AZEL','DoZ','Center','Distance','XY','XCYC']
#
#Par: storagearea = '' - You can have an arbitrary number of individual storage areas which 
#                         you can give different names (arbitrary combination of letters and digits). 
#                         Thus you can store and retrieve multiple sets of parameters. The default
#                         value is unnamed. This should contain the basic infofrmation associated 
#                         with the current data set.
#Ret:  T
#Ref:  data.get, data.restoredirection,data.fields,data.dirvariables,data.getdirection
#-----------------------------------------------------------------------

rec.savedirection := function(index = -1,fields='All',storagearea='') {
 wider rec
 local save
  if (index == -1) index:=[1:rec.len]
  for (i in index) {
    rec.put(rec.getdirection(i,fields),spaste('SaveDir',storagearea),i)
  }
return T
}

#-------------------------------------------------------------
#Name: data.saveblockinfo
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Saves some important parameters which are relevant for setting
# block information. They can be restored with data.restoreblockinfo
#
#Par:  i = -1 - Index of the dataset, for which to save the values. -1 indicates
#          all antennas.
#
#Par:  fields = 'All' - fields to save. 
# If fields='All' all relevant fields will be stored. Otherwise only the fields
# given in the list are saved. Fields that can be used are those that are
# accessible through data.getmeta and which are listed by data.metafields.
# Currently 'All' means ['Blocknum','BlockSize','Offset','FFTSize','FFTlen','FreqBin','FrequencyLow','FrequencyHigh','PowerNormFaktor']
#Par: storagearea = '' - You can have an arbitrary number of individual storage areas which 
#                         you can give different names (arbitrary combination of letters and digits). 
#                         Thus you can store and retrieve multiple sets of parameters. The default
#                         value is unnamed. This should contain the basic infofrmation associated 
#                         with the current data set.
#
#Ret:  T
#Ref:  data.getmeta, data.restoreblockinfo, data.getblockinfo
#-----------------------------------------------------------------------

rec.saveblockinfo := function(index = -1,fields='All',storagearea='') {
 wider rec
 local save
  if (index == -1) index:=[1:rec.len]
  for (i in index) {
    rec.putmeta(rec.getblockinfo(i,fields),spaste('SaveBlock',storagearea),i)
  }
return T
}

#-------------------------------------------------------------
#Name: data.restoredirection
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Restores some important parameters which are relevant for calculating
# delays and directions. They can be stored with data.savedirection.
#
#Par:  i = -1 - Index of the dataset, for which to restore values. -1 indicates
#          all antennas.
#
#Par:  fields = 'All' - fields to save. 
# If fields='All' all relevant fields will be restored. Otherwise only the fields
# given in the list are restored. Fields that can be restored are those that have been 
# stored by data.savedirection
#Par: refresh = T - Automatically refresh the GUI and recalculate phases. Can be set to
#                   F if data.restoreblockinfo is called thereafter.
#Par: replot = F - If set to T the plotpanel will be automatically replotted if present.
#
#Par: storagearea = '' - You can have an arbitrary number of individual storage areas which 
#                         you can give different names (arbitrary combination of letters and digits). 
#                         Thus you can store and retrieve multiple sets of parameters. The default
#                         value is unnamed. This should contain the basic infofrmation associated 
#                         with the current data set.
#
#Ret:  T
#Ref:  data.get, data.savedirection,data.fields,data.dirvariables,data.getdirection
#-----------------------------------------------------------------------

rec.restoredirection := function(index = -1,fields='All',refresh=T,replot=F,storagearea='') {
 wider rec
 local save
  if ( index == -1) index:=[1:rec.len]
  for (i in index) {
    save:=rec.get(spaste('SaveDir',storagearea),i);
    if ( fields == 'All') {fields:=field_names(save)}
    else {fields:=intersection(field_names(save),fields)};
    for (f in fields) {rec.put(save[f],f,i)}
    if (refresh) {
       rec.touchref('FFT',i);
       rec.setphase(index=i);
    }
  }
  if (refresh) {if (replot) {rec.replot()} else {rec.refreshgui()}}
  return T
}

#-------------------------------------------------------------
#Name: data.restoreblockinfo
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Restores some important parameters which are relevant for setting
# blocks. They can be stored with data.saveblockinfo.
#
#Par:  i = -1 - Index of the dataset, for which to restore values. -1 indicates
#          all antennas.
#
#Par:  fields = 'All' - fields to save. 
# If fields='All' all relevant fields will be restored. Otherwise only the fields
# given in the list are restored. Fields that can be restored are those that have been 
# stored by data.saveblockinfo
#Par: replot = F - If set to T the plotpanel will be automatically replotted if present.
#
#Par: storagearea = '' - You can have an arbitrary number of individual storage areas which 
#                         you can give different names (arbitrary combination of letters and digits). 
#                         Thus you can store and retrieve multiple sets of parameters. The default
#                         value is unnamed. This should contain the basic infofrmation associated 
#                         with the current data set.
#Ret:  T
#Ref:  data.getmeta, data.saveblockinfo,data.blockvariables,data.getblockinfo
#-----------------------------------------------------------------------

rec.restoreblockinfo := function(index = -1,fields='All',replot=F,storagearea='') {
 wider rec
 local save
  if (index == -1) index:=[1:rec.len]
  for (i in index) {
    save:=rec.getmeta(spaste('SaveBlock',storagearea),i);
    if (fields == 'All') {fields:=field_names(save)}
    else {fields:=intersection(field_names(save),fields)};
    for (f in fields) {rec.putmeta(save[f],f,i)}
    rec.setblock(n=save.Blocknum,blocksize=save.Blocksize,offset=save.ZeroOffset,index=i)
  }
if (replot) {rec.replot()} else {rec.refreshgui()}
return T
}

#-----------------------------------------------------------------------
#Name: data.replot
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Replot plotgui if present
#Par:  all = F  - If T recursively go trhough all dependent plotpanels and
#                 replot them as well. 
#Ret:  T
#Ref:  plotguif
#-----------------------------------------------------------------------
rec.replot:=function(all=F){wider rec;
   if (has_field(rec,'plotgui') && has_field(rec.plotgui,'replot')) {
      if (has_field(rec,'aux') && all) {rec.aux.replot(all=all)}
      rec.plotgui.replot()
   }
}

#-----------------------------------------------------------------------
#Name: data.refreshgui
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Refresh data in GUI if plot panel is present
#Par:  all = F  - If T recursively go through all dependent plotpanels and
#                 refresh them as well. 
#Ret:  T
#Ref:  plotguif
#-----------------------------------------------------------------------
rec.refreshgui:=function(all=F){wider rec;
  if (has_field(rec,'plotgui') && has_field(rec.plotgui,'refreshgui')) {
      if (has_field(rec,'aux') && all) {rec.aux.refreshgui(all=all)}
     rec.plotgui.refreshgui()
  }
}

#-----------------------------------------------------------------------
#Name: data.readcalfile
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Read in a file with calibration information.
#Ret:  T
#Ref:  readcalfile
#-----------------------------------------------------------------------

rec.readcalfile:=function(file='') { return readcalfile(rec, file); };

#-----------------------------------------------------------------------
#Name: data.versionref
#-----------------------------------------------------------------------
#Type: Function
#Doc:  undocumented!!
#Ret:  T
#-----------------------------------------------------------------------

rec.versionref:=function(f='',i=1) {
  vers:=rec.version(f,i);
  if (has_field(rec.coordinfo(f,i),'CrossRef')) {
    return max(rec.ref.versionref(rec.coordinfo(f,i).CrossRef,i),vers)
  }
  if (rec.coordinfo(f,i)['Ref']=='-') return max(rec.version(f,i),vers)
  return max(rec.versionref(rec.coordinfo(f,i)['Ref'],i),vers)
}


#-----------------------------------------------------------------------
#Name: data.version
#-----------------------------------------------------------------------
#Type: Function
#Doc: This function returns the version number of a field. If the field
#     is not stored, the version of the field is derived from is returned.
#     The function is used to check whether a field needs to be recalculated,
#Ret: T
#-----------------------------------------------------------------------

#This is not good since derived fields always have 0 version...
#return rec.get('Version',i)[field]};

    rec.version:=function(field,i=1) {
	if (rec.coordinfo(field,i)['Stored'] || rec.coordinfo(field,i)['Ref']=='-') {
       if (has_field(rec.coordinfo(field,i),'CrossRef')) {return rec.ref.version(rec.coordinfo(field,i).CrossRef,i) }
       else return GetDataFromMem(rec,'Version',i)[field];
	} else {
	  return rec.version(rec.coordinfo(field,i)['Ref'],i)
	};
}


#-----------------------------------------------------------------------
#Name: data.flag
#-----------------------------------------------------------------------
#Type: Function
#Ret: T
#Doc: undocumented!!
#-----------------------------------------------------------------------
    rec.flag:=function() {return ants_check(rec)}

#-----------------------------------------------------------------------
#Name: data.ant
#-----------------------------------------------------------------------
#Type: Function
#Ret: Integer Array of antenna numbers
#Doc: Returns a list of all antenna numbers
#-----------------------------------------------------------------------
    rec.ant:=function() {return seq(rec.len)}

#-----------------------------------------------------------------------
#Name: data.selant
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns a list of all selected antenna numbers
#Ret: Integer Array of antenna numbers
#-----------------------------------------------------------------------

rec.selant:=function() {return seq(rec.len)[rec.select()]}

#-----------------------------------------------------------------------
#Name: data.crid
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns a unique ID string for LOPES data (KASCADE ID number).
#Ret: String
#Ref: data.load, data.head, loadcr
#-----------------------------------------------------------------------
    rec.crid:=function(){
     return (split(rec.head('Filename'),'[t-]')[2])
    }

#-----------------------------------------------------------------------
#Name: data.load
#-----------------------------------------------------------------------
#Type: Function
#Doc: Load a cosmic ray event file from disk. A list of events is stored
#     in a table specified by 'tabfiles'. This can contain multiple files
#     and fileno specifies which table to read. The 'ith' event listed in 
#     table is then loaded. AZ/EL direction of the cosmic ray from that 
#     table are stored in the event file and displayed in the GUI.
#     The table is accessible as a globla object 'evtab'.
#Par: i	= 1			- Which of the selected cosmic rays to load
#Par: fileno = -1	- Which of the tabfiles to use: (-1=default,
#                        0=none, >0 position in tabfiles
#Par: doflag = T        - automatically do a 2-pass RFI flag on events
#
#Par: doshifts = T		- shift the data by the guesstimate of 
#				  the trigger delay
#Ret: T
#Ref: globalpar.DefTabfileNo, globalpar.DefTabfiles,input_event,evtab
#Ref: loadcr
#-----------------------------------------------------------------------
   rec.load:=function(i=1,doflag=T,fileno=-1,doshifts=T){
     global evtab; 
     return loadcr(rec,i=i,fileno=fileno,doflag=doflag,doshifts=doshifts)
   }

#-----------------------------------------------------------------------
#Name: data.process
#-----------------------------------------------------------------------
#Type: Function
#Doc: Load and process cosmic ray event file from disk. A list of events is stored
#     in a table specified by 'tabfiles'. This can contain multiple files
#     and fileno specifies which table to read. The 'ith' event listed in 
#     table is then loaded. AZ/EL direction of the cosmic ray from that 
#     table are stored in the event file and displayed in the GUI.
#     The table is accessible as a globla object 'evtab'.
#     The event is then inspected. Spiky antennas are flagged and the distance
#     is determined. The data shifted such that the CR spike is in the
#     expected time range according to a guess of the trigger delays.
#Par: i	= 1			- Which of the selected cosmic rays to process (accroding to position in crtab).
#Ret: record with results
#Ref: guipipeline, crprocess, loadccr, data.load
#-----------------------------------------------------------------------
    rec.process:=function(i=1){global plotgui;
     return crprocess(rec,plotgui,i=i)
    }

#Define general header information describing the data set
#Select	  - Boolean array describing whether data set should be used in operations
#Header	  - Header information from file with constants contains:
#   Filename: Original Filename data is read from
#   ID: an ID grouping files of the same series of measurements into one group
#   DataType: which ID to describe data structure and from which system data was taken (e.g. NOEMI)
#Metadata - Global parameters for the fields in the file


#This sets the default fields that should be commonly availabe in the
#individual data sets - but there is no guarantee. Make obsolete soon!
#     rec.infofields:='Filename ID DataType Select Header Metadata'
#     rec.infofields:='Select Header Metadata'


#Define x- and y-axes coordinate fields
#This sets the top-level fields in rec.Coord ... etc. which are just
#for information and for building the GUI. Should become obsolete.
    emptyrec:=EmptyRecord(type)
    for (f in "Coord Units") {rec[f]:=emptyrec[f]}

#Create empty data record that contains the data sets
    rec.data:=[=];
    rec.calfiles:=[=];
    rec.len:=0
}


#-----------------------------------------------------------------------
#Name: readfilesH
#-----------------------------------------------------------------------
#Type: Function
#Doc: Reads data from files into the internal memory using header information
#
#Par: rec = 				- Data record to identify dataset
#Par: headers = 			- Array of Headers
#Par: blocksize	= -1			- Length of block to read
#                                         blocksize<=0 -> read all
#Par: offset    = 0			- Starting position in file
#Par: pad	= F			- Pad the remaining fields
#					  with padval if file shorter
#					  than blocksize?
#Par: padval	= 0			- Value used for padding file
#-----------------------------------------------------------------------

readfilesH := function (ref rec,
                        headers,
                        blocksize=-1,
                        offset=0,
                        pad=F,
                        padval=0,
                        init=T)
{
  # --------------------------------------------------------
  # global variables 

  global obspar;
  global globalpar

  # --------------------------------------------------------
  # hack for backward compatibility

  if (init) {
    newdata(rec);
  }

  globalpar.DataID +:= 1;
  MinBlocksize      := globalpar.BlocksizeForDisplay
  if (blocksize==-1) {
    blocksize:=MinBlocksize;
  }

  # --------------------------------------------------------
  # check if news-agent is active

  if (is_agent(rec.newsagent)) {
    rec.newsagent->message('[data-kern] Reading Project-File(s) ...');
  }

  if (len(headers)>1){
    bar := progress(1,
                    len(headers),
                    'Reading Project Files',
                    spaste(headers[1].Files,' etc...'));
    bar.activate()
  } else {
    bar := F;
  }
  i:=0;
  obspar.toglobalpar(headers[1].Observatory,headers[1].Date);

  for (h in headers) {
    i+:=1;
    if (is_record(bar)) {
      bar.update(i);
    }
    ## add timezone information
    if (as_integer(h.Timezone)!=0) {
      d:=dq.quantity(h.Date);
      d.value:=d.value-as_float(h.Timezone)/24.;
      h.Date:=dq.time(d,form="ymd utc");
      h.Timezone:='00';
    }
#print h.Files
    statval:=shell(spaste('stat -c %s ',h.Files))
    if (len(statval)!=0&&as_double(statval)>0) { #if (len(stat(h.Files))>0) {
      filesize:=as_double(shell(spaste('stat -c %s ',h.Files)))/2
      #filesize:=as_integer(stat(h.Files).size/2) # one value == 2 Byte
#printvar(h)
      if (has_field(h,'DataFormat')&&h.DataFormat=='float32le')
      {filesize:=as_double(shell(spaste('stat -c %s ',h.Files)))/4}; # one float value = 32bit = 4byte
print h.Files,filesize
#      global system; system.output.trace := T
#      Initializing and storing data
      i:=rec.newentry(h)
      ci := rec.get('Coordinfo',i)
      FileBlocksize:=filesize
      if ((blocksize>0)&(filesize>blocksize)) {FileBlocksize:=blocksize};
      for (ff in rec.get('Input',i)) {
        if (ci[ff].Stored) {
          ary:=readdat(h.Files,blocksize=blocksize,offset=offset,pad=pad,padval=padval)
          FileBlocksize:=len(ary)
          rec.put(ary,ff,i)
        };
      };
      for (ff in rec.coord(i)) {rec.touch(ff,i,version=0)}
      rec.put(globalpar.centeroffset,'Center',i)
      rec.putmeta(offset,'FileOffset',i);
      rec.putmeta(FileBlocksize,'FileBlocksize',i);
      rec.putmeta(filesize,'Filesize',i);
      rec.setblock(n=1,blocksize=min(MinBlocksize,FileBlocksize),offset=0,index=i);
      rec.doframe();
#      rec.setphase();
    } else {print 'File',h.Files,'does not exist is empty or too big!'};
  }

  rec.setphase();

  if (len(globalpar.badantennas)>0) {
    for (j in rec.ant()) {
      antn:=as_integer(rec.head('AntNum',j))
      if (is_member(antn,globalpar.badantennas)) {
        printf ('%s%i%s%i%s', 'De-selecting antenna #',antn,' (Data Set #',j,')\n');
        rec.select(j,F);
      }
    }
  }

  if (is_record(bar)) {
    bar.deactivate()
    bar:=F;
  };

} # end of function

#-----------------------------------------------------------------------
#Name: EmptyRecord
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Generate a new empty data structure for a given data type.
#Par:  DataType - Data type for which a new record is generated.
#Ref:  NOEMIgenEmptyRec,TIM40genEmptyRec,TIM40revmodifyrec,
#      TIM40beammodifyrec,TIM40fgenEmptyRec,TEKgenEmptyRec
#-----------------------------------------------------------------------

EmptyRecord := function(DataType='NOEMI') {
  if (DataType=='NOEMI') {
    return NOEMIgenEmptyRec();
  } else if (DataType=='NOEMI-rec') {
    return NOEMIgenEmptyRec();
  } else if (DataType=='TIM40') {
    return TIM40genEmptyRec();
  } else if (DataType=='TIM40-rec') {
    return TIM40genEmptyRec();
  } else if (DataType=='TIM40rev') {
    return TIM40revmodifyrec(TIM40genEmptyRec());
  } else if (DataType=='TIM40rev_up') {
    return TIM40rev_up_modifyrec(TIM40genEmptyRec());
  } else if (DataType=='TIM40revrev') {
    return TIM40revrevgenEmptyRec();
  } else if (DataType=='TIM40beam') {
    return TIM40beammodifyrec(TIM40genEmptyRec());
  } else if (DataType=='TIM40f') {
    return TIM40fgenEmptyRec();
  } else if (DataType=='TEK') {
    return TEKgenEmptyRec();
  } else if (DataType=='CGaccess') {
    return CGAccessgenEmptyRec();
  } else if (DataType=='NANCAY'|DataType=='NANCAY2') {
    return NANCAYgenEmptyRec();
  } else if (DataType=='BEAM') {
    return BEAMgenEmptyRec();
  } else {
    print 'Unknown DataType:',DataType;
    return F;
  };
};

#-----------------------------------------------------------------------
#Name: getits
#-----------------------------------------------------------------------
#Type: Function
#Doc: Copies data from ITS and converts it to LOPES format
#     It will then open a user gui to select the files in the directory.
#
#Par: s = '' - Name of ITS experiment
#-----------------------------------------------------------------------
getits:=function(s){
  global data;
  global globalpar;
  print '========================================================================'
  print 'Transfering ITS experiment ',s,' from ITS.'
  print '========================================================================'
  idir:=globalpar.DefITSDataDir;
  ldir:=concat_dir(idir,s);
  print 'Files will be copied into directory:',ldir;

  if (!dexist(idir)) {print 'Creating directory:',idir; shell(spaste('mkdir ',idir))};
  if (!dexist(ldir)) {print 'Creating directory:',ldir; shell(spaste('mkdir ',ldir))};
  shell(spaste('cd ',idir,'; ',concat_dir(globalconst.progDir,'getits'),' ',s));
  print '========================================================================'
  print 'Done transfering ITS experiment ',s,' from ITS.'
  print '========================================================================'
  proj:=[=];
  lgfile:=concat_dir(ldir,'lopeshdr.hdr');
  readproject(proj,lgfile,headerfiles=T);
  h:=proj[1];
  if (len(h)>0) {
  for (i in seq(len(h))) {h[i].Files:=concat_dir(ldir,h[i].Files)}
	readfilesH(data,h,blocksize=globalpar.BlocksizeForRead,
           offset=globalpar.OffsetForRead);
        } else {return}
  if (is_agent(data.newsagent)) {data.newsagent->newdata(); data.newsagent->checkdata()};
}



########################################################################
#Section: data-handling Functions
########################################################################

#######################################################################
#----------------------------------------------------------------------
# name: ants_check  check antenna data for realistic values
#----------------------------------------------------------------------
# type: function
# doc:  loads event-file(s) from directory
# par:  rec	data set
# ret:  erg	boolvector with a flag for each antenna
#----------------------------------------------------------------------
# Andreas Nigl @ 30.06.03 @ MPIfR
#----------------------------------------------------------------------
# SPECTRUMFILTER (stddev)
# AHORNEFFERFILTER
# f1: ADCMax/MinChann
# f2: +-200
# f3: +-1000
# f4: +2047 to -2048 (ADCMax/Min)
ants_check:=function(ref rec)
{
  for (ant in seq(rec.len))
  {
    #erg[ant]:=(max(rec.get('f(x)',ant))<=as_integer(rec.head('ADCMaxChann'))&min(rec.get('f(x)',ant))>=as_integer(rec.head('ADCMinChann')));
    erg[ant]:=(max(rec.get('f(x)',ant))<=2048&min(rec.get('f(x)',ant))>=-2049);
  };
  return erg;
}
#######################################################################

#-----------------------------------------------------------------------
#Name: CreateEntry
#-----------------------------------------------------------------------
CreateEntry := function(ref rec, head = [=]) {
	index := rec.len +1;
	rec.len := index;
	rec.data[index]:=[=]
#        rec.puthead(head.DataType,'DataType',index);
    rec.put(T,'Select',index);
	rec.put([=],'Metadata',index);
	rec.put([=],'Version',index);
    rec.put(F,'Doflag',index);
    rec.put(1,'RefAnt',index);
    rec.put(F,'Doshift',index);
    rec.put([0,90],'AZEL',index);
    rec.put(0,'Caldelay',index);
    rec.put(0,'Delay',index);
    rec.put(0,'Corrdelay',index);
    rec.put(T,'DoZ',index);
#The center of the array relative to the reference Antenna 1
    rec.put([0,0,0],'Center',index);
    rec.put(0,'Distance',index);
    rec.put([0,0],'XY',index);
	emptyrec:=EmptyRecord(head.DataType)
	for (ff in field_names(emptyrec)){rec.put(emptyrec[ff],ff,index)}
	if (has_field(head,'Files')) {
	  Filename:=globalconst.FilenameExtractor ~ head.Files;
          head.Filename:=Filename;
	};
	rec.put(head,'Header',index);
        rec.puthead(globalpar.DataID,'ID',index);
	rec.setregion([-1,-1],'T',index)
	rec.setregion([-1,-1],'F',index)
	rec.putmeta(0,'FileOffset',index);
	rec.putmeta(0,'FileBlocksize',index);
        FixDelays(rec,index=index);
	return index;
};

###NEW
#-----------------------------------------------------------------------
#Name: SpawnRecord
#-----------------------------------------------------------------------
#Type: Function
#Doc: Copies an entire data record and spawns a child record which
#     uses the original record as its data source
#Ret: New Data Record
#
#Par: srec	= 			- Name of Record (String), where data was
#                         originally stored
#Ref: TIM40revmodifyrec
#-----------------------------------------------------------------------
SpawnRecord := function (ref rec, ref rec2,totype='TIM40rev') {
newdata(rec2,type=totype);
rec2.ref:=ref rec;
for (i in seq(rec.len)) {
     h:=rec.get('Header',i);
     h.DataType:=totype;
     n:=rec2.newentry(h);
#     type:=rec2.head('DataType',i);
#     if (totype=='TIM40beam') {rec2.data[i]:=TIM40beammodifyrec(rec2.data[i])}
#     else if (totype=='TIM40rev') {
#       rec2.data[i]:=TIM40revmodifyrec(rec2.data[i])
#     }
#Avoid duplicate calibration!
     rec2.put(0,'Corrdelay',i);
     rec2.put(0,'Delay',i);
     rec2.put(0,'Caldelay',i);
     rec2.put(0,'Distance',i);
     rec2.put([0,0],'XY',i);
     rec2.put([0,0,0],'Center',i);
     rec2.put(F,'DoZ',i);
	 for (ff in rec2.coord(i)) {rec2.touch(ff,i,version=0)}
     rec2.data[i].Metadata:=rec.data[i].Metadata
     if (totype=='TIM40rev_up') { TIM40rev_up_modifyheader(rec2,i); };
####UGLY....
       MinBlocksize:=globalpar.BlocksizeForDisplay;
       arylen:=(len(rec.get('FFT',1))-1)*2;
	   rec2.setregion([1,min(MinBlocksize,arylen)],'T',i)
	   rec2.setregion([1,min(MinBlocksize/2+1,arylen/2+1)],'F',i)
       rec2.putmeta(0,'FileOffset',i);
       rec2.putmeta(arylen,'FileBlocksize',i);
#       rec2.setblock(n=1,index=i);
#       rec2.setblock(n=1,offset=0,index=i);
#       rec2.setblock(blocksize=min(MinBlocksize,arylen),index=i);
#       rec2.setblock(n=1,blocksize=min(MinBlocksize,arylen),offset=0,index=i);
   }
  if (totype == 'TIM40rev' || totype == 'TIM40rev_up') { 
    if (len(globalpar.badantennas)>0) {
      print 'De-selecting spawned antennas:',globalpar.badantennas;
      for (j in rec2.ant()) {
        if (is_member(j,globalpar.badantennas)) {
          rec2.select(j,F);
        }
      }
    };
  };
}

###NEW End

########################################################################
#Section: Field-Access Functions
########################################################################

#-----------------------------------------------------------------------
#Name: GetDataDispatch
#-----------------------------------------------------------------------
#Type: Function
#Doc: Dispatches the requests for data to the corresponding functions.
#     eg. to GetDataFromMem if the data is in memory.
#Ret: Array with y-data
#
#Par: rec	= 			- Record, where data is stored
#Par: field	= 'f(x)'			- Coordinate axis ('x','f(x)',...)
#Par: index	= 1			- Index of data set to be
#					  retrieved
#Ref: data.get
#-----------------------------------------------------------------------
GetDataDispatch := function(ref rec,field='f(x)',index=1,start=-1,end=-1) {
  ci := [=];
  if (field != 'Coordinfo') ci:=rec.get('Coordinfo',index);
  if (has_field(ci,field)) {

##This is only for data sets which are stored in memory
    if(ci[field].Stored) {
      region0:=rec.getregion(ci[field].Domain,index)
      if (start<0 && end <0) region:=region0
      else region:=[start,end];
 
## This is for data that are derived and stored and the parent has been modified
    if ( (ci[field].Ref != '-') && (rec.modified(field,index) || rec.version(field,index)==0 || (!has_field(rec.data[index],field)))) {
#debug# print 'R0a:',index,field,ci[field].Ref,region,region0,!has_field(rec.data[i],field)
	 rec.put(ci[field].ConvFunc(rec.get(ci[field].Ref,index), rec,index,reffield=ci[field].Ref),field,index)
#debug# print  'R0b:',rec.version(ci[field].Ref,index),rec.version(field,index)
	 rec.touch(field,index,version=rec.versionref(ci[field].Ref,index))
#debug# print  'R0c:',rec.version(ci[field].Ref,index),rec.version(field,index)
	 if (ci[field].Domain == ci[(ci[field].Ref)].Domain) {
		rec.putmeta(region0,spaste(field,'storedregion'),index);
	 } else {
#debug# print 'R2:',index,field,ci[field].Ref,region,region0
#flagdat
     		 return GetDataFromMem(rec, field, index,start=region[1],end=region[2])
	 };
      }
## End of modified and stored

      if ((region[1] !=-1) && (region[2] !=-1)) {
        storegion := rec.getmeta(spaste(field,'storedregion'),index);
        sstart := region[1]-storegion[1]+1; sstop := region[2]-storegion[1]+1;  
        l := storegion[2]-storegion[1]+1;
        region[1] := max(min(sstart,l),1);  region[2] := max(min(sstop,l),1);
      };
#debug# print 'R3:',index,field,ci[field].Ref,region,region0
#flagdat

#return for unmodified and for modified and stored where ref is in same domain
      return  GetDataFromMem(rec, field, index,start=region[1],end=region[2])

## end of stored
    } else {
      if (start<0 && end <0) region:=rec.getregion(ci[field].Domain,index)
      else region:=[start,end]

## Begin not stored
##  Begin Data that is generated and not derived
      if (ci[field].Ref=='-') {
#debug# print 'Gen:',index,field,region,ci[field].GenFunc
        if (has_field(ci[field],'CrossRef')) {fld:=ci[field].CrossRef} else {fld:=field}
        return ci[field].GenFunc(fld,rec,index,start=region[1],end=region[2])
        }
##  End Data that is generated and not derived
##  Begin Data that is not stored and  derived
      else {
	if (ci[field].Domain == ci[(ci[field].Ref)].Domain) {
	  ary:=ci[field].ConvFunc(rec.get(ci[field].Ref,index,
			          start=region[1],end=region[2]),rec,index);
#debug# print 'R4:',index,field,ci[field].Ref,region,len(ary)
	  return ary;
	} else {
	  ary:=ci[field].ConvFunc(rec.get(ci[field].Ref,index),rec,index);
	  l:=len(ary)
	  if (region[1]<1 || region[1]>l) region[1]:=1;
	  if (region[2]<1 || region[2]>l) region[2]:=l;
#debug# print 'R5:',index,field,region,len(ary)
	  return ary[region[1]:region[2]];
	};
      }
    }
  } else {  
#debug# if (field != 'Coordinfo' && field != 'Select' && field != 'Header' && field != 'Input') print 'R6:',index,field,start,end
    return GetDataFromMem(rec, field, index,start=start,end=end);
  }
};

#-----------------------------------------------------------------------
#Name: GetDataFromMem
#-----------------------------------------------------------------------
#Type: Function
#Doc: Obtains the y-coordinate data (e.g. time series data) from the 
#     memory. Using the index one can access multiple time series data 
#     sets.
#Ret: Array with y-data
#
#Par: rec	= 			- Record, where data is stored
#Par: field	= ''			- Coordinate axis ('x','f(x)',...)
#Par: index	= 1			- Index of data set to be
#					  retrieved
#Ref: PutDataToMem,  readfilesH, writefiles, data.get
#-----------------------------------------------------------------------
GetDataFromMem:=function(ref rec,field='',index=1,start=-1,end=-1) {
  if (index<1 || index>rec.len) return F
  if (field=='') {
    return rec.data[index]
  } else {
    if (has_field(rec.data[index],field)) {
      l:=len(rec.data[index][field])
      if (start<1 || start>l) start:=1
      if (end<1 || end>l) end:=l
      return  rec.data[index][field][start:end]
    } else return F;
  }
}


#-----------------------------------------------------------------------
#Name: GetDataVal
#-----------------------------------------------------------------------
#Type: Function
#Doc: Obtains one element of y-coordinate data (e.g. time series data) 
#     from the memory. 
#Ret: Value of one element 
#
#Par: rec	= 			- Record, where data is stored
#Par: field	= 'f(x)'			- Coordinate axis ('x','f(x)',...)
#Par: index	= 1			- Index of data set to be
#					  retrieved
#Par: n		= 1			- nth element
#Ref: PutDataToMem, GetDataFromMem, InitializeData, readfiles, writefiles
#-----------------------------------------------------------------------
GetDataVal:=function(ref rec,field='f(x)',index=1,n=1) {
  if (index<1 || index>rec.len) return F
  ary := rec.get(field,index)[n]
  return ary
}

#-----------------------------------------------------------------------
#Name: GetDataHeader
#-----------------------------------------------------------------------
#Type: Function
#Doc: Obtains header information from a data set stored in memory.
#Ret: Contents of header field
#
#Par: rec	= 			- Record, where data is stored
#Par: field	= ''			- Header field
#Par: index	= 1			- Index of data set to be
#					  retrieved
#Ref: PutDataToMem, InitializeData, readfiles, writefiles
#-----------------------------------------------------------------------
GetDataHeader:=function(ref rec,field='',index=1) {
  #if (index<1 || index>rec.len) {
  #print 'PutDataHeader: antenna index=',i,'out of range.'; return F}
  h := rec.get('Header',index);
  if (has_field(h,field)) { return h[field] };
  return F;
}

#-----------------------------------------------------------------------
#Name: PutDataHeader
#-----------------------------------------------------------------------
#Type: Function
#Doc: Puts header information into the header field
#Ret: T/F
#
#Par: rec	= 		- Record, where data is stored
#Par: ary	= 		- New value of header field
#Par: field	= ''		- Header field
#Par: index	= 1		- Index of data set to be
#				  retrieved
#Ref: PutDataHeader
#-----------------------------------------------------------------------
PutDataHeader:=function(ref rec,const ary, field='',index=1) {
  for (i in index) {
  if (i<1 || i>rec.len) {
    #print 'PutDataHeader: antenna index=',i,'out of range.'
  } else {
    header:=rec.get('Header',i);
    header[field]:=ary;
    rec.put(header,'Header',i);
   }
 }
}

#-----------------------------------------------------------------------
#Name: ListDataHeader
#-----------------------------------------------------------------------
#Type: Function
#Doc: List the header information from a data set stored in memory.
#Ret: T
#
#Par: rec	= 			- Record, where data is stored
#Par: index	= 1			- Index of data set to be
#					  retrieved
#Ref: PutDataToMem, GetDataHeader
#-----------------------------------------------------------------------
ListDataHeader:=function(ref rec,index=1) {
  for (i in index) {
    if (i<1 || i>rec.len) print spaste('Index ',i,' not valid for data record.')
    else for (f in field_names(rec.get('Header',index))) print spaste(f,'=',rec.get('Header',index)[f])
  }
return T
}

#-----------------------------------------------------------------------
#Name: SaveData
#-----------------------------------------------------------------------
#Type: Function
#Doc: Saves the data header in a text file
#Ret: T
#
#Par: rec	= 			- Record, where data is stored
#Par: index	= 1			- Index of data set to be
#					  edited
#Ref: ListDataHeader, EditDataHeader, GetDataHeader
#-----------------------------------------------------------------------
SaveData:=function(ref rec,index=1) {
  for (i in index) {
    if (i<1 || i>rec.len) print spaste('Index ',i,' not valid for data record.')
    else {
    filename:=spaste(globalpar.DefDataDir,globalconst.DirSeparator,rec.head('Filename',i));
    filename1:=spaste(filename,'.hdr');
    filename2:=spaste(filename,'.dat');
    print 'Writing ',filename;
    file:=open(spaste('> ',filename1));
    rec.puthead(filename2,'Files',i)
    head:=rec.get('Header',i);
    for (f in field_names(head)) {write(file,sprintf('#%s: %s',f,as_string(head[f])))}
    file:=F; 
    #t.data:=as_short(rec.get('f(x)',i));
    t.data:=as_short(rec.data[i]['Dummy']);
    t.filename:=filename2;
    t.type:='int16';
    print globalpar.readdat_client->writefile(t)
  }
 }
print 'Done!'
}

#-----------------------------------------------------------------------
#Name: GetMetaFromMem
#-----------------------------------------------------------------------
#Type: Function
#Doc: Obtains Metadata information from a data set stored in memory.
#Ret: Contents of header field
#
#Par: rec	= 			- Record, where data is stored
#Par: field	= ''			- Metadata field
#Par: index	= 1			- Index of data set to be
#					  retrieved
#Ref: PutDataToMem, InitializeData, readfiles, writefiles
#-----------------------------------------------------------------------
GetMetaFromMem:=function(ref rec,field='',index=0) {
  if ( index == -1) index:= min([1:rec.len][rec.select()]);
  if (len(index)>1 || index<1 || index>rec.len) return F;
#  if ( all(field_names(rec.get('Metadata',index)) != field) ) return F;
#  return rec.get('Metadata',index)[field]
  if ( all(field_names(rec.data[index]['Metadata']) != field) ) return F;
  return rec.data[index]['Metadata'][field]
}

#-----------------------------------------------------------------------
#Name: ChangeVersion
#-----------------------------------------------------------------------
#Type: Function
#Doc: Increase the version number of a field. This is used to check 
#     whether dependent fields in a record need to be recalculated.
#Ret: Contents none
#
#Par: rec	= 			- Record, where data is stored
#Par: field	= ''			- Field for which version is changed
#Par: index	= 1			- Index of record to be stored
#Par: version	= -1			- Set version No. 
#Ref: data.touch, data.touchref, data.modified
#-----------------------------------------------------------------------
ChangeVersion:=function(ref rec,field='',index=1,version=-1) {
    if (len(index)==1 && index < 0) {idx:=seq(rec.len)} else {idx:=index}
    for (i in idx) {
      if (version>=0) {
	rec.data[i]['Version'][field]:=version
      } else {  #no new version given -> set version to new highest value
	maxvers := 0;
	for (ff in field_names(rec.data[i]['Version']) )
	  maxvers := max(maxvers,rec.data[i]['Version'][ff]);
	rec.data[i]['Version'][field]:=maxvers+1
      };
      if (rec.data[i]['Version'][field]>1e9) {
	print '[ChangeVersion]: Version number too high - integer �erlauf!!!!';
        rec.data[i]['Version'][field]:=0 
      };
    };
}


#-----------------------------------------------------------------------
#Name: PutMetaToMem
#-----------------------------------------------------------------------
#Type: Function
#Doc: Store Metadata information in a data set in memory.
#Ret: Contents none
#
#Par: rec	= 			- Record, where data is stored
#Par: inp	= 			- Data to store
#Par: field	= ''			- Metadata field
#Par: index	= 1			- Index of data set to be stored
#Ref:
#-----------------------------------------------------------------------
PutMetaToMem:=function(ref rec,const inp,field='',index=0) {
  if ( index == 0) index:= min([1:rec.len][rec.select()]);
    for (i in index) {
  rec.data[max(min(i,rec.len),1)]['Metadata'][field] := inp;
 }
}

#-----------------------------------------------------------------------
#Name: SetRegion
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Store the 
#Ret:  Contents none
#
#Par:  rec	= 			- Record, where data is stored
#Par:  region	= [-1,-1]		- Region
#Par:  domain	= 'T'			- Domain (e.g. FFT, Time)
#Par:  index	= 1			- Index of data set to be stored
#Par:  rno	= 1			- Region Number
#Ref:  readfilesH,GetDataDispatch,GetRegion
#-----------------------------------------------------------------------
SetRegion := function(ref rec,region=[-1,-1],domain='T',index=1,rno=1) {
    if (rno>1) {
      for (i in index) {
	rec.data[max(min(i,rec.len),1)]['Region'][domain][(rno-1)*2+1] := region[1];
	rec.data[max(min(i,rec.len),1)]['Region'][domain][(rno-1)*2+2] := region[2];  }
    }
    else    
    for (i in index) rec.data[max(min(i,rec.len),1)]['Region'][domain] := region;
    for (c in rec.coord(index)) {
      if ((rec.coordinfo(c,index).Domain==domain) && 
           (rec.coordinfo(c,index).Ref=='-')) rec.touch(c,index);
    };
}

#-----------------------------------------------------------------------
#Name: GetRegion
#-----------------------------------------------------------------------
#Type: Function
#Doc: Store the
#Ret: Region Vector
#
#Par: rec	= 			- Record, where data is stored
#Par: domain	= 'T'			- Domain (e.g. FFT, Time)
#Par: index	= 1			- Index of data set to be stored
#Par: rno	= 1			- Region Number
#Ref: readfilesH,SetRegion,GetDataDispatch
#-----------------------------------------------------------------------
GetRegion:=function(ref rec,domain='T',index=1,rno=1) {
    return rec.data[max(min(index,rec.len),1)]['Region'][domain][[(rno-1)*2+1,(rno-1)*2+2]]
}


#-----------------------------------------------------------------------
#Name: SetBlock
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Set the current block to be read and used. It will update block
#      offset, length, FFTlen, regions etc. 
#Par: n         = -1  - Block number (1=first,n=-1 unchanged)
#Par: blocksize	= -1  - Block size (-1=unchanged)
#Par: index	= -1  - Index of data sets to be updated (-1=all)
#Par: offset    = -1  - a single offset of arbitrary length to be added 
#                       (no. of samples) from the start of file (-1=unchanged)
#Ref: plotpanel, CreateEntry, readfilesH, OffsetRecalc
#-----------------------------------------------------------------------

SetBlock := function(ref rec, n=-1, blocksize=-1, offset=-1, index=-1) 
{
  if ( index == -1) index:= [1:rec.len][rec.select()];
  EOF:=F;
  for (i in index) {
    # 'offset' is here only used for initialization to zero, it will be
    # overwritten by offsetrecalc
    if (offset>=0) {rec.putmeta(offset,'ZeroOffset',i)};
    if (n>=0) {
      rec.putmeta(0,'Offset',i);
      rec.putmeta(n,'Blocknum',i);
    }
    if (blocksize>=0 && blocksize != rec.getmeta('Blocksize',i)) {
      if (n<=0) {rec.putmeta(1,'Blocknum',i)};
#      rec.putmeta(1,'Blocknum',i);
      rec.putmeta(blocksize,'Blocksize',i);
      rec.putmeta(blocksize,'FFTSize',i);
      rec.putmeta((as_integer(blocksize/2)+1),'FFTlen',i);

## Now calculate a few useful meta informations
#      rec.putmeta(rec.headf('Samplerate',i)*rec.headf('SamplerateUnit',i)/(blocksize-1),'FreqBin',i);
      rec.putmeta(rec.headf('Bandwidth',i)*rec.headf('FrequencyUnit')/(blocksize/2),'FreqBin',i);
      rec.putmeta((rec.headf('NyquistZone',i)-1)*rec.headf('Bandwidth',i)*rec.headf('FrequencyUnit'),'FrequencyLow',i);
      rec.putmeta((rec.headf('NyquistZone',i))*rec.headf('Bandwidth',i)*rec.headf('FrequencyUnit'),'FrequencyHigh',i);

      rec.putmeta(blocksize,'PowerNormFaktor',i);
      rec.put(F,'Doflag',i);
    }

    if (n>=0 || blocksize>=0 || offset>=0) {
      EOF:=EOF | OffsetRecalc(rec,n=rec.getmeta('Blocknum',i),zerooffset=offset,index=i);
    }
    rec.touch(rec.get('Input',i),i);
#######################################
#Error:
#This leads to an infinite loop if spawned ...
#    rec.setphase(index=i);
#######################################
#    rec.setphase(index=index);
  }
  return EOF;
}


#-----------------------------------------------------------------------
#Name: OffsetRecalc
#-----------------------------------------------------------------------
#Type: Function
#Doc: Recalculates the offset from the beginning of the file for reading the
# next block. This is the basic routine for the 'block' mechanism.
#Ret:
#
#Par: rec  		- data record for which to determine the offset
#Par: n = -1 		- Block number (1=first,n=-1 unchanged)
#Par: index	 = -1	- Index of data sets to be updated (-1=all)
#Par: zerooffset = -1 	- a single offset of arbitrary length to be added
# 			  (no. of samples) from the start of file (-1=unchanged)
#Ref: readfilesH, OffsetRecalc, SetBlock, data.setblock, data.nextblock
#-----------------------------------------------------------------------
OffsetRecalc := function(ref rec, n=1,index=-1,zerooffset=-1) {
  if ( index == -1) index:= min([1:rec.len][rec.select()]);
  n := n-1;
  EOF := F;
  if (n <0) {EOF:=T; return EOF};
  for (i in index ) {
    if (zerooffset>=0) rec.putmeta(zerooffset,'ZeroOffset',i);
    blocksize := rec.getmeta('Blocksize',i);
    offset:=rec.getmeta('ZeroOffset',i)+blocksize*n;
    if ((offset+blocksize) > rec.getmeta('Filesize',i)) {
      print 'data-kern.g:OffsetRecalc: (offset+blocksize) > rec.getmeta(\'Filesize\',i) This should not happen!';
      offset:=offset-blocksize;
      EOF:=T;
    }
    rec.putmeta(offset,'Offset',i);
    rec.setregion([(offset+1),(offset+blocksize)],'T',i);
    rec.setregion([1,blocksize/2+1],'F',i);
    ff := rec.get('Input',i);
    if (rec.coordinfo(ff,i).Domain == 'T') {rec.touchref(ff,i); };
  };
return EOF;
};


#-----------------------------------------------------------------------
#Name: PutDataToMem
#-----------------------------------------------------------------------
#Type: Function
#Doc: Stores the y-coordinate data (e.g. time series data) in
#     memory. A parent record is specified to hold the data.
#
#Ret: None
#
#Par: rec	= 			- Record, where data is stored
#Par: ary	= 			- Array to store
#Par: field	= 'f(x)'			- Coordinate axis ('x','f(x)',...)
#Par: index	= 1			- Index of data set to be retrieved
#Ref: GetDataFromMem, InitializeData, readfiles, writefiles
#-----------------------------------------------------------------------
PutDataToMem:=function(ref rec,const ary,field='f(x)',index=1) {
  while (len(rec.data)+1<index) rec.data[len(rec.data)+1]:=[=];
  if (index>rec.len) rec.len:=index;
#  end:=min(max(index,end),rec.len)
  if ( any(index < 1) || any( index > rec.len) ) {
	if (is_agent(rec.newsagent)) {
	    rec.newsagent->message('Cannot put data into nonexisting storage (index>len). Warum dass denn nicht??');
	} else {
	    print 'Cannot put data into nonexisting storage (index>len)';
	};
	return F;
  };
  for (i in index) {
    if (field=='') {
      rec.data[i]:=ary
    } else {
      rec.data[i][field]:=ary;
      rec.putmeta([1,len(ary)],spaste(field,'storedregion'),i);
      if (has_field(rec.data[i],'Version') && has_field(rec.data[i].Version,field) ) {
	rec.touch(field,index);
	if (rec.get('Input',i) == field){
	   region := rec.getregion('T',i);
	   region[1] := min(region[1],len(ary));
	   region[2] := min(region[2],len(ary));
	   rec.setregion(region,'T',i)
     rec.putmeta((region[2]-region[1]+1),'FileBlocksize',i);
	   rec.putmeta(region[1],'Offset',i);
	   rec.putmeta((region[2]-region[1]+1),'Blocksize',i);
	};
      }
    };
  };
  return T;
}

#-----------------------------------------------------------------------
#Name: DeleteData
#-----------------------------------------------------------------------
#Type: Function
#Doc: Deletes one data entry from a dataset and from memory
#
#Par: rec	= 			- Record, where data is stored
#Par: index	= 1			- Index of data set to be deleted
#
#Ref: PutDataToMem,  readfilesH
#-----------------------------------------------------------------------
DeleteData:=function(ref rec,field='',index=1) {
  if (index<1 || index>rec.len) return F
  rec.data[index]:=F
  if (index<rec.len) for (i in seq(index+1,rec.len)) rec.copy(i,i-1)
  rec.len-:=1
}

#-----------------------------------------------------------------------
#Name: FieldToArray
#-----------------------------------------------------------------------
#Type: Function
#Doc: Collects all entries for a field in a dataset and creates an array
#
#Par: rec	= 			- Record, where data is stored
#Par: field	= 'f(x)'			- Field/Coordinate axis to be deleted
#Par: index	= 1			- Index of data set to be deleted
#Par: header    = F			- Read the Field from the Header?
#
#Ref: readfilesH
#-----------------------------------------------------------------------
FieldToArray:=function(ref rec,field='',header=F) {
  if (header) {
    for (index in seq(rec.len)) {
      ary[index]:=rec.head(field,index)
    }
  } else {
    for (index in seq(rec.len)) {
      ary[index]:=rec.get(field,index)
    }
  };
  return ary
}

#-----------------------------------------------------------------------
#Name: FieldToRecord
#-----------------------------------------------------------------------
#Type: Function
#Doc: Collects all entries for a field in a dataset and creates a record
#
#Par: rec	= 			- Record, where data is stored
#Par: field	= 'f(x)'			- Field/Coordinate axis to be deleted
#Par: index	= 1			- Index of data set to be deleted
#Par: header    = F			- Read the Field from the Header?
#
#Ref: readfilesH
#-----------------------------------------------------------------------
FieldToRecord:=function(ref rec,field='f(x)',header=F) {
  ary:=[=];
  if (header) {
    for (index in seq(rec.len)) {
      ary[index]:=rec.head(field,index)
    }
  } else {
    for (index in seq(rec.len)) {
      ary[index]:=rec.get(field,index)
    }
  };
  return ary
}


#-----------------------------------------------------------------------
#Name: GetPosFromXCoord
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns the sample number (position) of an arbitray x coordinate.
#     This assumes the data to be monotonically rising or falling. Use
#     the function to convert between different coordinate x-axis.
#
#Par: rec	= 			- Record, where data is stored
#Par: x	    = 	        - Value of the x-cooridnate to be converted.
#Par: field	= 'f(x)'	- Field of the x-cooridnate to be converted.
#Par: index	= 1		    - Index of data set to be deleted
#
#-----------------------------------------------------------------------

GetPosFromXCoord:=function(ref rec,x,field='f(x)',index=1) {
           ary:=rec.get(field,index)
	       l:=len(ary);
           x1:=ary[1];
	       dx:=(ary[l]-x1)/(l-1.)
           sg:=sign(dx)
	       xnmin1:=max(min(l-1,as_integer((x-x1)/dx+1)),1); xnmin2:=xnmin1+sg;
           while (xnmin1>1 && ary[xnmin1]>=x) {xnmin1-:=sg; xnmin2:=xnmin1+sg}
           while (xnmin1<(l-1) &&  ary[xnmin1+1]<=x) {xnmin1+:=sg; xnmin2:=xnmin1-sg}
           if (abs(ary[xnmin1]-x)<=abs(ary[xnmin2]-x)) {return xnmin1} else {return xnmin2};
#           y1:=ary[xnmin1]; y2:=ary[xnmin2];
#           return y1+(y2-y1)/(xnmin2-xnmin1)*(x-xnmin1)
	       }

########################################################################
# I/O- Functions
########################################################################
#-----------------------------------------------------------------------
#Name: directory,ldirectory
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns the contents of a directory
#Ret: String, List of Files,  Attributes: status
#Par: dir = ''        - Directory
#-----------------------------------------------------------------------
directory:=function(dir) {return shell('ls -1 ',dir)}
ldirectory:=function(dir) {return shell('ls -lh1 ',dir,'| gawk \'{print substr($0,57),substr($0,31,26)}\'')};

#-----------------------------------------------------------------------
#Name: readproject
#-----------------------------------------------------------------------
#Type: Function
#Doc: Reads a logfile defining the entire project
#Par: proj        =		- Data set to store project data
#Par: infile	  = 		- Filename to read from
#Par: headerfiles = F		- 
#-----------------------------------------------------------------------

readproject:=function(ref proj,
                      infile,
                      headerfiles=F)
{
  if (headerfiles) {
    file := open(paste(program.catheaderprocess(infile),
                       program.logprocess,
                       '|'));
  } else {
   if (!fexist(infile)) {
      print 'ERROR opening',infile,'- file does not exist!';
      return F
   }
   else {
     print 'Reading Projectfile',infile;
   }
   file := open(paste(program.logprocess,'<',infile,'|'));
 }
 print '[data-kern::readproject] Acessing ',infile,'...';
 dummy:=[=]
 meas[1]:='0';
 nmeas:=0
 while ((line := read(file))!='') {
   if (line ~ globalconst.ProjectParNameFinder) {
    parname := (line ~ globalconst.ProjectParNameExtractor)
    pars := (line ~ globalconst.ProjectParExtractor)
    dummy[parname]:=pars
#	if (parname == 'Project') {
#	   dummy:=[=];
#	   dummy['Project']:=pars;
#	}
      if ((parname == 'Measurement') && (len(meas[meas==pars])==0)) {
         nfil:=0;
         nmeas+:=1;
         meas[nmeas]:=pars
         proj[dummy.Measurement]:=[=];
      }
      if (parname == 'Files') {
         nfil:=nfil+1;
         proj[dummy.Measurement][nfil]:=dummy;
      }
    }
  }
file:=F
}


#-----------------------------------------------------------------------
#Name: readdat
#-----------------------------------------------------------------------
#Type: Function
#Doc: Reads a section of a file of Integers of length integer one per
#     line from a given position.
#Ret: Array of integers
#Par: infile	= 			- Filename to read from
#Par: blocksize	= -1			- Length of block to read,
#                                         blocksize<=0 -> read all
#Par: offset    = 0			- Starting position in file
#Par: pad	= F			- Pad the remaining fields
#					  with padval if file shorter
#					  than blocksize?
#Par: padval	= 0			- Value used for padding file
#Par: type	= int16b		- type of file to read
#Ref: readfilesH
#-----------------------------------------------------------------------

readdat := function(infile,blocksize=-1,offset=0,pad=F,padval=0,type='int16b') {
	global globalpar;

	feld := [];
	if (!fexist(infile)) {
	   print 'ERROR opening',infile,'- file does not exist!';
	   return F}
	#else print 'Reading',infile;

	file.filename := infile;
	file.block := blocksize;
	file.offset := offset
	file.type := type;


	feld := globalpar.readdat_client->readfile(file);

	file := F;
	if (len(feld)<blocksize && pad) {
	   feld:=[feld,array(padval,1:blocksize-len(feld))]}
	#print 'Read file:',infile,' shape:',shape(feld);
	return feld;
};

#-----------------------------------------------------------------------
#Name: readcalfile
#-----------------------------------------------------------------------
#Type: Function
#Doc: Reads in a calibration file.
#
#Ret: Array of two dimensions
#Par: rec = 		- Record, where data is stored
#Par: file = ''		- name of the file with calibration data
#-----------------------------------------------------------------------

readcalfile := function(ref rec, file) {
     file:=add_dir(file);
     global erg
      if (has_field(rec.calfiles,file)) {
	return rec.calfiles[file];
      } else {
    	if (!fexist(file)) {
     	  print 'ERROR opening',file,'- file does not exist!';
    	  return F;
	};
	fd := open(paste('< ',file));
	tmp := split(<fd>);
	if (len(tmp) >= 2) {
		erg := as_float(tmp[1:2]);
	};
	while ( len(tmp:=split(<fd>)) >= 2  ) {
		if (split(tmp[1],'')!='#')
		   erg := cbind(erg,as_float(tmp[1:2]));
	};
	shap := shape(erg)
	erg := erg[1:2,order(erg[1,(1:shap[2])])]
	rec.calfiles[file] := erg;
	return erg;
      };
};


#-----------------------------------------------------------------------
#Name: writetab
#-----------------------------------------------------------------------
#Type: Function
#Doc: Write a section of a vector to a file
#Ret: Status of file operation (T/F)
#Par: outfile	= 			- Filename to write to
#Par: vec	=			- Vector to write
#Par: blocksize	= -1			- Length of block to read
#                                         blocksize<=0 -> write all
#Par: offset    = 0			- Starting position in file
#Par: pad	= F			- Pad the remaining fields
#					  with padval if file shorter
#					  than blocksize?
#Par: padval	= 0			- Value used for padding file
#-----------------------------------------------------------------------

writetab := function(outfile,vec,blocksize=-1,offset=0,pad=F,padval=0) {
	file := open(paste('>',outfile));
	print outfile;
	if (blocksize<=0) blocksize:=len(vec)
	for (x in vec) write(file,x)
        if (len(vec)<blocksize && pad) {
	   for (i in [1:blocksize-len(vec)]) write(file,padval)}
	file := F;
};
