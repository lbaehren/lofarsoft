########################################################################
#
#Functions that shouldn't be here! (cause they belong to the gui)
#
########################################################################

#-----------------------------------------------------------------------
#Name: readfiles_gui
#-----------------------------------------------------------------------
#Type: Function
#Doc: Opens a window, listing the files in a directory and allows
#     the user to select mutliple files
#Ret: String, List of Files

#Par: dir = ''	      - Directory to start with
#Par: filter = ''     - Filefilter to start 
#Par: header = F     - Default header to add to the file
#-----------------------------------------------------------------------

readfiles_gui := function(dir='',filter='',header=F){
    global globalconst, globalpar 
    selfiles:="";     selheaders:=[=];  
    f := frame(title='File Selection')  
    if (filter == '') {filter:=globalpar.DefFileFilter}
    if (dir == '') dir:=globalpar.DefDataDir
    if (last_char(dir)!=globalconst.DirSeparator) {
       dir:=paste(dir,globalconst.DirSeparator,sep='')}
    cdir:=paste(dir,filter,sep='')
    ef := frame(f,side='left',expand='x')
    b := button(ef,'dir')
    en := entry(ef)
    read := button(ef,'Read')
    dismiss := button(ef,'Dismiss')
    en -> insert(cdir)
    mf := frame(f,side='left',expand='x')             
    m := message(mf,'Enter directory and file filter.')
    bsizelabel:=label(mf,'No. of Samples:');
    bsize:=entry(mf,width=6,justify='right'); 
    bsize->insert(as_string(globalpar.BlocksizeForRead))
    offsetlabel:=label(mf,'Offset:');
    offset:=entry(mf,width=6,justify='right'); 
    offset->insert(as_string(globalpar.OffsetForRead))
    lbf := frame(f,side='left',expand='both')             
    lb := listbox(lbf,width=50,height=20,fill='both') 
    sb := scrollbar(lbf)             
    lb->mode('extended')
    files := directory(cdir)       
    lb->insert(files)
    whenever lb->select do {selfiles:=lb->get($value)}
    whenever b->press, en->return do
        {
        lb->delete("start","end")
        cdir := en->get()
	files := directory(cdir)       
        if ( ! files::status )
            lb->insert(files)
        else
            m->text('No files - choose new directory or filter.')
        }
    whenever sb->scroll do 
        lb->view($value)   
    whenever lb->yscroll do
        sb->view($value)   

    whenever dismiss->press do
           {selfiles:=''; ignore:=T}

    ignore:=F	   
    await read->press, f->killed, dismiss->press
    if (!ignore) {
        globalpar.BlocksizeForRead:=as_integer(bsize->get())
	globalpar.OffsetForRead:=as_integer(offset->get())
	globalpar.DefDataDir:=(cdir ~ globalconst.DirExtractor)
	globalpar.DefFileFilter:=(cdir ~ globalconst.FilenameExtractor)
    }
    f:=F
    if (is_record(header) && len(selfiles)>0){
       selheaders:=[=]
       for (i in seq(len(selfiles))) {
          selheaders[i]:=header
	  selheaders[i]["Files"]:=selfiles[i]
	  }
       if ((!is_record(selheaders[1])) && len(selheaders)>0) {
            tmp:=[=]; tmp["1"]:=selheaders; return tmp}
       else {return selheaders}
       } 
       else {return selfiles}
};

#-----------------------------------------------------------------------
#Name: readprojectfiles_gui
#-----------------------------------------------------------------------
#Type: Function
#Doc: Opens a window, listing the projecs in a directory and allows
#     the user to select project files
#Ret: String-Array, List of Files

#Par: proj =	      - record containing information about the
#			project data files   
#Par: dir = ''	      - Directory to start with
#Par: logfile = ''     - Logfile to start with
#-----------------------------------------------------------------------

readprojectfiles_gui:=function(ref proj,dir='',logfile=''){
    global globalconst, globalpar
    selfiles:=""
    f := frame(title='File Selection')
    if (logfile == '') {logfile:=globalpar.DefLogfile}
    if (dir == '') dir:=globalpar.DefDataDir
    if (last_char(dir)!=globalconst.DirSeparator) {
       dir:=spaste(dir,globalconst.DirSeparator)}
    lgfile:=spaste(dir,logfile)
    ef := frame(f,side='left',expand='x')
    b := button(ef,'Load Logfiles')
    b2 := button(ef,'Load Headers')
    en := entry(ef)
    read := button(ef,'Read')
    dismiss := button(ef,'Dismiss')
    en -> insert(lgfile)
    mf := frame(f,side='left',expand='x')             
    m := message(mf,'Enter logfile.',width=360)

    bsizelabel:=label(mf,'No. of Samples:');
    bsize:=entry(mf,width=6,justify='right'); 
    bsize->insert(as_string(globalpar.BlocksizeForRead))
    offsetlabel:=label(mf,'Offset:');
    offset:=entry(mf,width=6,justify='right'); 
    offset->insert(as_string(globalpar.OffsetForRead))


    selprojs:=[=]
    selheaders:=[=]

    lbf := frame(f,side='left',expand='both')             
    sb := scrollbar(lbf)             
    lb := listbox(lbf,width=20,height=20,fill='both') 
    lb->mode('extended')
    lb->bind('<Button-3>','button3')
    lb->bind('<ButtonRelease-3>','button3release')
    info_f:=T

    lproj:=function(proj){
	wider lb,m
	wider selprojs
	s:=""; i:=0 
        lb->delete("start","end")
	for (mm in field_names(proj)) {
	    i+:=1; 
	    s[i]:=spaste('[',as_string(selprojs[i]),'] ','Measurement ',mm)
	};
	m->text('Left mouse button to select/de-select. Right mouse button for info');
	lb->insert(s)
    };

    whenever b->press do {
	lgfile:=en->get();   
        if (fexist(lgfile)) { readproject(proj,lgfile) } 
	 else { print 'File not found:',lgfile ; }; 
	selprojs:=rep(F,len(proj))
	lproj(proj)
    }   

    whenever b2->press, en->return do {
	lgfile:=en->get();   
        readproject(proj,lgfile,headerfiles=T) 
	selprojs:=rep(F,len(proj))
	lproj(proj)
    }   

    whenever lb->button3release do {wider info_f; info_f:=F}
    whenever lb->button3 do {
	     wider info_f,proj
	     field:=field_names(proj)[(lb->nearest($value.device[2]))+1]
	     info_f:=frame(tlead=lb,tpos='ne',background='blue')             
	     if (len(proj[field])>0) {
	     fields:=field_names(proj[field][1]);
	     for (ff in fields) {
		 mf[ff]:=message(info_f,text=spaste(ff,'=',proj[field][1][ff]),
				 justify='left',pady=0,background='blue')}
	     }
	     else ff:=message(info_f,'Empty Measurement.',background='blue')
    }

    sb2 := scrollbar(lbf)             
    lb2 := listbox(lbf,width=60,height=20,fill='both') 
    lb2->mode('extended')
    filelist:=[=]
    headerlist:=[=]

    l2proj:=function(proj){
	wider lb2,filelist,headerlist
	wider selprojs
	s:=""; i:=0 
	dir:=lgfile ~ globalconst.DirExtractor

        lb2->delete("start","end")
	for (mm in field_names(proj)[selprojs]) {
	    for (j in seq(len(proj[mm]))) {
		i+:=1; 
		filedir:=proj[mm][j]["Files"] ~ globalconst.DirExtractor
		filename:=proj[mm][j]["Files"] ~ globalconst.FilenameExtractor
		if (filedir=='') {filelist[i]:=spaste(dir,filename)}
		else {filelist[i]:=spaste(filedir,filename)}
		headerlist[i]:=proj[mm][j]
		headerlist[i]["Files"]:=filelist[i]
		s[i]:=ldirectory(filelist[i])}
	}
	lb2->insert(s)
    };

    whenever lb->select do {
    	    for (i in $value) selprojs[i+1]:=!selprojs[i+1]
	    lproj(proj); l2proj(proj)
    };

    whenever lb2->select do {
    	    selfiles:=filelist[$value+1]
    	    selheaders:=headerlist[$value+1]
    };

    whenever sb->scroll do 
        lb->view($value)   
    whenever lb->yscroll do
        sb->view($value)   

    whenever sb2->scroll do 
        lb2->view($value)   
    whenever lb2->yscroll do
        sb2->view($value)   

    whenever dismiss->press do
           {selprojs:=''; selfiles:=''; ignore:=T}



    ignore:=F	   
    await read->press, f->killed, dismiss->press
    if (!ignore) {
        globalpar.BlocksizeForRead:=as_integer(bsize->get())
	globalpar.OffsetForRead:=as_integer(offset->get())
	globalpar.DefDataDir:=(lgfile ~ globalconst.DirExtractor)
	globalpar.DefLogfile:=(lgfile ~ globalconst.FilenameExtractor)
    }
    f:=F
#    Check if only one file selected. In this case fake a one-element
#    array. Otherwise a scalar would be returned!
    if ((!is_record(selheaders[1])) && len(selheaders)>0) {
     tmp:=[=]; tmp["1"]:=selheaders; return tmp}
    else {return selheaders}
}

#-----------------------------------------------------------------------
#Name: EditDataHeader
#-----------------------------------------------------------------------
#Type: Function
#Doc: Edit the header information from a data set stored in memory.
#Ret: T
#
#Par: rec	= 			- Record, where data is stored
#Par: index	= 1			- Index of data set to be
#					  edited
#Ref: ListDataHeader, PutDataToMem, GetDataHeader
#-----------------------------------------------------------------------
EditDataHeader:=function(ref rec,index=1) {
    header:=[=];
    i:=index[1];

    chooseset:=function(const j){
      wider header,rec,i;
      if (j<1) j:=1;
      if (j>rec.len) j:=rec.len;
      header:=rec.get("Header",j);
      i:=j;
    }

    chooseset(1);    
    tk_hold();
    frame0 := frame(title='Header Editor');
    linef:=[=];
    topf.topframe:=frame(frame0,side='left',expand='x');
    topf.label:=label(topf.topframe,'Data set',anchor='w',fill='x');
    topf.topsplitter:=frame(frame0,side='left',expand='x',borderwidth=1,relief='sunken',height=2);
    topf.scale:=scale(topf.topframe,1,len(index),1,borderwidth=1, hlthickness=0, width=6, fill='none');    

    for (f in field_names(header)) {
	linef[f].frame:=frame(frame0,side='left',expand='x',borderwidth=0);
        linef[f].text:=label(linef[f].frame,f,width=20,justify='left',anchor='w');
        linef[f].entry:=entry(linef[f].frame,width=40,borderwidth=1,background='white');
        linef[f].copy:=button(linef[f].frame,'Copy',bitmap=spaste(globalconst.ImageDir,'copy.xbm'),
                              padx=1,pady=1,value=f,borderwidth=1);
        popuphelp(linef[f].copy,'Copy header field');
	whenever linef[f].copy -> press do {
          header[$value]:=(linef[$value].entry->get());
          for (j in index) {rec.puthead(header[$value],$value,j)}
	}
      }
    topf.botsplitter:=frame(frame0,side='left',expand='x',borderwidth=1,relief='sunken',height=2);
    topf.botframe:=frame(frame0,side='left',expand='x');
    topf.save:=button(topf.botframe,'Save',width=7,fill='x',borderwidth=1);
    topf.reset:=button(topf.botframe,'Reset',width=7,fill='x',borderwidth=1);
    topf.botspacer:=frame(topf.botframe,expand='x',height=1,width=10);
    topf.done:=button(topf.botframe,'Done',width=7,fill='x',borderwidth=1,background='limegreen');
    topf.dismiss:=button(topf.botframe,'Dismiss',width=7,fill='x',borderwidth=1,background='orange');
    tk_release();

    listf:=function() {
        for (f in field_names(header)) {
        linef[f].entry->delete("start","end")
        linef[f].entry->insert(as_string(header[f]))
	}}

     savef:=function(){
        wider header,rec,i;
        for (f in field_names(header)) {
          header[f]:=(linef[f].entry->get());
	}
	rec.put(header,"Header",i)
    }

    listf();

    whenever topf.scale -> value do {
      chooseset($value); listf()    
    }

    whenever topf.done-> press do {
        savef();	frame0:=F
     }
    whenever topf.save-> press do {savef()}

    whenever topf.reset-> press do {
      header:=rec.get("Header",i);
      listf();
    }

    whenever topf.dismiss-> press do {frame0:=F}
    return T
}


