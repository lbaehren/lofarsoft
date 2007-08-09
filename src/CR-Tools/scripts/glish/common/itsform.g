
#-------------------------------------------------------------------------------
# Basic routines to create a graphical user interface for setting up and 
# submitting experiments on the LOFAR ITS.
#-------------------------------------------------------------------------------



#-----------------------------------------------------------------------
#Section: Routines for constructing a general input form GUI
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
#Name: widthLabel
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Uniform setting of the width of labels.
#-----------------------------------------------------------------------

widthLabel := function (width) { return width/2; }

#-----------------------------------------------------------------------
#Name: inputform.pluginbuttons
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Create the GUI entries for settings up the Plugins used in the
#      experiment.
#-----------------------------------------------------------------------

inputform.pluginbuttons:=function(ref inrec,ref f,choices,plugins,
                                  txt='Plugin',width=50,bg='white'){
    if (!is_record(inrec)) {val inrec:=[=]};
    inrec.choices:=choices;
    inrec.plugins:=plugins;
    inrec.data:=inrec.plugins[1]; 

    inrec.frame:=frame(f,side='left');
    inrec.frameL:=frame(inrec.frame,side='top',borderwidth=0);
    inrec.frameR:=frame(inrec.frame,side='top',borderwidth=0);

    inrec.label:=label(inrec.frameL,spaste(txt," : "),
                       width=widthLabel(width),justify='left',anchor='nw');

    inrec.spc:=frame(inrec.frameR, width=1, height=1, expand='x',borderwidth=0);
    inrec.ent:=button(inrec.spc,inrec.data,type='menu',width=width-1, 
                      borderwidth=0,relief='flat',justify='left',anchor='w');
    inrec.buttons:=[=];
	for (i in 1:len(inrec.choices)) {
  	  inrec.buttons[i]:=button(inrec.ent,text=inrec.plugins[i],type='radio',
                                   value=i, borderwidth=0);
	  whenever inrec.buttons[i]->press do {
	      inrec.data:=inrec.choices[$value];
              inrec.ent->text(inrec.plugins[$value]);
              setPluginParameters (inrec);
	    }
	}
    inrec.opt:=entry(inrec.frameR,relief='sunken',width=width,background=bg);
}

#-----------------------------------------------------------------------
#Name: inputform.choicebuttons
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Create choice buttons in the GUI.
#-----------------------------------------------------------------------

inputform.choicebuttons:=function(ref inrec,ref f,choices,txt='Choice',width=50){
    if (!is_record(inrec)) {val inrec:=[=]};
    inrec.choices:=choices;
    inrec.data:=inrec.choices[1];
    inrec.frame:=frame(f,side='left');
    inrec.label:=label(inrec.frame,spaste(txt," : "),justify='left',anchor='w',
                       width=widthLabel(width));
    inrec.spc:=frame(inrec.frame, width=1, height=1, expand='x');
    inrec.ent:=button(inrec.spc,inrec.data,type='menu', width=width-1, 
                      borderwidth=1,relief='flat',justify='left',anchor='w');
    inrec.buttons:=[=];
	for (i in 1:len(inrec.choices)) {
  	  inrec.buttons[i]:=button(inrec.ent,text=inrec.choices[i],type='radio',
                                   value=i, borderwidth=1);
	  whenever inrec.buttons[i]->press do {
	      inrec.data:=inrec.choices[$value];
              inrec.ent->text(inrec.data);
	    }
	}
}

#-----------------------------------------------------------------------
#Name: inputform.checkbuttons
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Create check buttons in the GUI.
#-----------------------------------------------------------------------

inputform.checkbuttons:=function(ref inrec,ref f,txt='Choice',default=T){
  if (!is_record(inrec)) {val inrec:=[=]};
  inrec.ent:=button(f,txt,type='check',borderwidth=1, relief='flat', hlthickness=0);
  inrec.ent->state(default);
}

#-----------------------------------------------------------------------
#Name: inputform.entryfield
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Create entry fields in the ITS experiment GUI.
#-----------------------------------------------------------------------

inputform.entryfield := function(ref inrec,ref f,txt="Input",default='',
                        width=50,background='white',relief='sunken')
{
  if (!is_record(inrec)) {val inrec:=[=]};
  inrec.frame:=frame(f,side='left');
  inrec.label:=label(inrec.frame,spaste(txt," : "),justify='left',anchor='w',
                     width=widthLabel(width));
  inrec.spc:=frame(inrec.frame, width=1, height=1, expand='x');
  inrec.ent:=entry(inrec.frame,relief=relief,width=width,background=background);
  if (default!='') {inrec.ent -> insert(default)};
  whenever inrec.ent -> return do {
    inrec.data:=inrec.ent->get();
    if (has_field(inrec,"inputfunc")) {
      val inrec.data := as_string(inrec.inputfunc(inrec.data));
      inrec.ent -> delete('start','end');
      inrec.ent -> insert(inrec.data);
    }
  }
}

#-----------------------------------------------------------------------
#Name: inputform.dismissbut
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Create a 'dismiss' button in the ITS experiment GUI.
#-----------------------------------------------------------------------

inputform.dismissbut:=function(ref inrec,f){
  inrec.dismissbut:=button(f, 'Dismiss', background='orange');
  whenever inrec.dismissbut-> press do {
    val inrec.panel:=F;
  }
}

#-------------------------------------------------------------------------------
#Name: inputform.make
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Create the input form for the ITS experiments from the provided record.
#Par:  A record describing the structure of the input form to be build.
#-------------------------------------------------------------------------------

inputform.make:=function(ref inrec){
  for (f in field_names(inrec.entries)){
    val inrec.entries[f].gui:=[=];
    if (has_field(inrec.entries[f],"inputfunc")) {val inrec.entries[f].gui.inputfunc:=inrec.entries[f].inputfunc};
    if (inrec.entries[f].type=='entry') {
      if (has_field(inrec.entries[f],"default")) {default:=inrec.entries[f].default} else {default:=''};
      inputform.entryfield(inrec.entries[f].gui,inrec.frame,inrec.entries[f].description,default=default);
    }
    if (inrec.entries[f].type=='menu') {
      inputform.choicebuttons(inrec.entries[f].gui,inrec.frame,choices=inrec.entries[f].choices,txt=inrec.entries[f].description);
    }
    if (inrec.entries[f].type=='check') {
      if (has_field(inrec.entries[f],"default")) {default:=inrec.entries[f].default} else {default:=F};
      inputform.checkbuttons(inrec.entries[f].gui,inrec.frame,txt=inrec.entries[f].description,default=default);
    }
    if (inrec.entries[f].type=='plugin') {
      inputform.pluginbuttons(inrec.entries[f].gui,inrec.frame,
                              choices=inrec.entries[f].choices,
                              plugins=inrec.entries[f].plugins,
                              txt=inrec.entries[f].description);
    }
  }
}

#-------------------------------------------------------------------------------
#Name: inputform.read
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  [still needs to be written]
#-------------------------------------------------------------------------------

inputform.read:=function(ref inrec){
  for (f in field_names(inrec.entries)){
    if (inrec.entries[f].type=='entry') {
      val inrec.entries[f].gui.data:=inrec.entries[f].gui.ent->get()
    }
    if (inrec.entries[f].type=='check') {
      if (inrec.entries[f].gui.ent->state()) {val inrec.entries[f].gui.data:="true"}
      else {val inrec.entries[f].gui.data:="false"}
    }
  }
}

#-------------------------------------------------------------------------------
#Name: inputform.writeits
#-------------------------------------------------------------------------------
#Type: Function
#Doc: Constructs a GUI for producing and submitting an experiment file for ITS.
#Ret: T/F
#Par: rec =	  			- Data record
#Par: fields =				- Which coordinate to crosscorrelate
#Par: maxlag =				- Maximum number of lags in Pixel
#Par: maxord =				- Maximum order??
#Ref: readfiles,plotpanels
#-------------------------------------------------------------------------------

inputform.writeits:=function(ref inrec,filename='submit-experiment.meta'){
  inrec.filename:=filename;
  inrec.file:=open(spaste('>',inrec.filename));
  write(inrec.file,'[OBSERVATION]');
  for (f in field_names(inrec.entries)) { 
    write(inrec.file,spaste(inrec.entries[f].label,'=',inrec.entries[f].gui.data));
  }
  write(inrec.file,'#EOF');
  write(inrec.file);
  val inrec.file:=F;

  # make sure that no plugin parameters are submitted when in capturing mode

  experimentMode := shell('grep capturing ',filename);         

  if (mode != '') {
    tmpfile := spaste (filename,'.tmp');
    shell ('cat ',filename,' | grep -v plugin > ',tmpfile,' && mv ',tmpfile,' ',filename);
  }

}

#-----------------------------------------------------------------------
#Section: Input form for ITS
#-----------------------------------------------------------------------

itsform:=[=];

#-----------------------------------------------------------------------
#Name: itsform.constructor
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Constructs a GUI for producing and submitting an experiment file for ITS.
#Ret:  T/F
#Par:  rec 	  			- Data record
#Par:  fields 				- Which coordinate to crosscorrelate
#Par:  maxlag 				- Maximum number of lags in Pixel
#Par:  maxord 				- Maximum order??
#Ref:  readfiles,plotpanels
#-----------------------------------------------------------------------

itsform.constructor:=function(){

  global itsformgui;

  # set Tk on hold until all widgets are defined
  tk_hold();

  itsformgui.panel:=frame(title=spaste('ITS Experiment Form'));
  itsformgui.frame:=frame(itsformgui.panel,relief='ridge',height=100,width=60);

  # ------------------------------------------------------------------
  # Prepare to include information on the AVIARY plugins

  pluginsLocal := getPlugin(index='local');
  names := field_names(pluginsLocal);
  pluginLocal := array('a',len(names));
  pluginLocalNames := array('a',len(names));
  for (i in 1:len(names)) pluginLocal[i] := pluginsLocal[names[i]].doc;
  for (i in 1:len(names)) pluginLocalNames[i] := field_names(pluginsLocal)[i];

  pluginsCentral := getPlugin(index='central');
  names := field_names(pluginsCentral);
  pluginCentral := array('a',len(names));
  pluginCentralNames := array('a',len(names));
  for (i in 1:len(names)) pluginCentral[i] := pluginsCentral[names[i]].doc;
  for (i in 1:len(names)) pluginCentralNames[i] := field_names(pluginsCentral)[i];

  # ------------------------------------------------------------------
  # Description of form record

  itsformgui.entries:=[=];

  f:='observation_owner'; r:=[=]; r.label:=f;
  r.description:='Observer'
  r.type:='entry'
  r.default:=username();
  itsformgui.entries[f]:=r;

  f:='basename'; r:=[=]; r.label:=f;
  r.description:='Filename'
  r.type:='entry';
#  r.default:=spaste(shell('whoami'),'-',dq.time(dm.getvalue(dm.epoch('utc', 'today'))[1], form="ymd time") ~ s/\/[^\/]+$// ~ s/\//-/g);
  r.default:=spaste(shell('date +%Y%m%d'),'-',shell('whoami'));
  itsformgui.entries[f]:=r;

  f:='starttime'; r:=[=]; r.label:=f;
  r.description:='Starttime (LT)';
  r.type:='entry';
  r.default:=dq.time(dm.getvalue(dm.epoch('utc', 'today'))[1], form="ymd time") ~ s/([0-9]+\/[0-9]+\/[0-9]+)\//$1 / ~ s/\.[0-9]*$//
  itsformgui.entries[f]:=r;

  f:='description'; r:=[=]; r.label:=f;
  r.description:='Description'
  r.type:='entry'
  itsformgui.entries[f]:=r;

  f:='antennas'; r:=[=]; r.label:=f;
  r.description:='Antennas ([0:59])'
  r.type:='entry'; 
  r.default:=paste(as_string([0:59]));
  r.inputfunc:=function(s){return paste(as_string(eval(s)) ~ s/^ *\[// ~ s/^\] *$//)}
  itsformgui.entries[f]:=r;

  f:='iterations'; r:=[=]; r.label:=f;
  r.description:='Repeats'
  r.type:='entry'
  r.default:='1';
  itsformgui.entries[f]:=r;

  f:='interval'; r:=[=]; r.label:=f;
  r.description:='Interval (sec)'
  r.type:='entry'
  r.default:='0';
  itsformgui.entries[f]:=r;

  f:='capturemode';
  r:=[=];
  r.label:=f;
  r.description:='Capture Mode'
  r.type:='menu'
  r.choices:=['SYNCSTART','SYNCSTOP'];
  itsformgui.entries[f]:=r;

  f:='capturesize'; r:=[=]; r.label:=f;
  r.description:='Capture Size'
  r.type:='menu'
  r.choices:=['S16_512K','S16_1M','S16_2M','S16_16M','S16_32M','S16_128M','S16_256M','S16_512M'];
  itsformgui.entries[f]:=r;

  f:='experiment_type';
  r:=[=];
  r.label:=f;
  r.description:='Experiment Type'
  r.type:='menu'
  r.choices:=['capturing','processing'];
  itsformgui.entries[f]:=r;

  f:='plugin1'
  r:=[=];
  r.label:=f;
  r.description:='Plugin 1 (Capture PC)'
  r.type:='plugin'
  r.plugins:=[pluginLocal];      # Abbrev. plugin names (e.g. fft)
  r.choices:=[pluginLocalNames]; # Plugin description   (e.g. Fourier Transform)
  itsformgui.entries[f]:=r;

  f:='plugin2'
  r:=[=];
  r.label:=f;
  r.description:='Plugin 2 (Concentrator)'
  r.type:='plugin'
  r.plugins:=[pluginCentral];
  r.choices:=[pluginCentralNames];
  itsformgui.entries[f]:=r;

  f:='signextension'; r:=[=]; r.label:=f;
  r.description:='Sign Extension'
  r.type:='check'
  r.default:=T;
  itsformgui.entries[f]:=r;

  f:='skipcapture'; r:=[=]; r.label:=f;
  r.description:='Skip Capture'
  r.type:='check'
  itsformgui.entries[f]:=r;

  # -----------------------------------------------------------------------
  # Initializing GUI 

  inputform.make(itsformgui);
  
  itsformgui.bottomf1:=frame(itsformgui.panel,relief='ridge',padx=2,pady=10,height=100,width=60,side='left');
  itsformgui.sendbut := button(itsformgui.bottomf1, 'Send', background='orange');
  triggergui.bootmfsp1 :=frame(itsformgui.bottomf1, height=1, width=20, expand='x'); 
  itsformgui.triggerbut := button(itsformgui.bottomf1, 'Trigger GUI', background='orange');
  triggergui.bootmfsp2 :=frame(itsformgui.bottomf1, height=1, width=20, expand='x'); 
  itsformgui.retrievebut := button(itsformgui.bottomf1, 'Retrieve', background='orange');
  triggergui.bootmfsp3 :=frame(itsformgui.bottomf1, height=1, width=20, expand='x'); 
  inputform.dismissbut(itsformgui,itsformgui.bottomf1);

  tk_release();

  # ------------------------------------------------------------------
  # Events associated with the buttons in the GUI

  whenever itsformgui.sendbut-> press do {
    inputform.read(itsformgui);
    inputform.writeits(itsformgui);
    # Submit the experiment specification via network
    print shell('cat submit-experiment.meta | netcat astron-buinen.nema.rug.nl 8890');
  }  

  whenever itsformgui.triggerbut-> press do {
       trigger.gui();
  }  

  whenever itsformgui.retrievebut-> press do {
   getits(itsformgui.entries.basename.gui.data)
  }  

}
  
