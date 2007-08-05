#-------------------------------------------------------------------------------
# $Id$
#-------------------------------------------------------------------------------
# Graphical User Interface so set up the parameters required for the computation
# of a skymap; see the LOPES Tools
# <a href="http://www.astron.nl/~bahren/research/coding/LOPESTools_skymapper.html">online doucmentation</a> for further details.
# <ol>
#   <li><a href="#data">Extraction of information from the data structure</a>
#   <li><a href="#gui_help">Helper functions for the construction of the GUIs</a>
#   <li><a href="#gui_main">Construction of the GUIs</a>
# </ol>
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Section: <a name="data">[1]</a> Extraction of information from the data structure
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: get_blocksizes
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Helper routine for 'SkymapperGUI.g': Given an input event file with
#       a data block of size Filesize, get the blocksizes by which the
#       input data block can be divided without reminder.
#Par: ref recDataBlock - reference to the data record
#Ret: blocksizes[]     - array with the blocksizes
#-------------------------------------------------------------------------------

get_blocksizes := function (ref recDataBlock) {

  Filesize := recDataBlock.getmeta('Filesize');
  Blocksize     := recDataBlock.getmeta('Blocksize');
  numBlocks     := Filesize/Blocksize;
  numBlocksInt  := as_integer(numBlocks);
  numBlocksRem  := Filesize - Blocksize*numBlocksInt;

  arrayIndex := 1;
  for (size in 1:(Filesize/2)) {
    blocks := Filesize/size;
    if (blocks == as_integer(blocks)){
      blocksizes[arrayIndex] := blocks;
      arrayIndex +:= 1;
    }
  }

  if (is_fail(blocksizes)) {
    print 'get_blocksizes(): no valid array generated as return value';
    fail;
  }

  return blocksizes;

}


#-------------------------------------------------------------------------------
#Name: getProjections
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Generate a list of available map projections. This is done by simply
#      constructing a coordinate system tool and then extraction the map
#      projection options.
#Ret:  projections - a record with the abbr. projection identifiers.
#-------------------------------------------------------------------------------

getProjections := function () {

  cs := coordsys(direction=T, spectral=T);
  projections := sort(cs.projection('all'));
  cs.done();

  return projections;

}


#-------------------------------------------------------------------------------
#Name: getCoordinateSystems
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Extract a list of available coordinate system reference codes.
#      This is done by simply constructing a coordinate system tool
#      and then extraction the reference code options.
#Ret:  referencecodes - record with the reference codes
#

getCoordinateSystems := function () {

  cs := coordsys(direction=T, spectral=T);

  referencecodes := sort(cs.referencecode('dir', T).normal);

  cs.done();

  return referencecodes;

}

#-------------------------------------------------------------------------------
#Name: get_topDirectory
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  
#Par:  dir
#Ret:  topdir
#-------------------------------------------------------------------------------

get_topDirectory := function (dir) 
{  
  topdir := shell(spaste('cd ',dir,' && cd .. && pwd'))

  return topdir;
}

#-------------------------------------------------------------------------------
#Name: readDataCC
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Extract all necessary information for processing a cross-correlated
#      dataset.
#Par:  rec             - Reference to the data structure.
#Par:  mapGUI          - Reference to the Skymapper GUI.
#Par:  plotGUI         - Reference to the PlotControl GUI.
#Par:  loadExampledata - Load the 'exampledata.its' dataset, to set up the 
#                        internal values specifying the telescope array.
#Par:  adjustFrequency - Adjust the frequency settings.
#Created: 2004/11/07 (Lars Baehren)
#-------------------------------------------------------------------------------

readDataCC := function (ref rec, ref mapGUI, ref plotGUI, loadExampledata=T,
                        adjustFrequency=T)
{
  global globalpar;
 
  #---------------------------------------------------------
  # Load an ITS dummy dataset

  if (loadExampledata) {
    readprfiles_cbfunc(spaste(globalpar.DefITSDataDir,"/exampledata.its"));
  }

  #---------------------------------------------------------
  # Run the Skymappe CLI to extract the experiment settings
  # from the metafile

  CLI := client(spaste(globalconst.LOPESBIND,'/Skymapper'));

  recCLI := [=];
  recCLI.metafile    := spaste(mapGUI.file.Entry->get(),"/experiment.meta");
  recCLI.antennafile := spaste(globalconst.progDir,"/antpos-its.dat");

  ccSettings := CLI->getSettingsCC(recCLI);

  if (is_fail(ccSettings)) {
    print '[SkymapperGUI::readDataCC] CLI failed returning settings!'
    fail;
  }

  CLI := F;

  #---------------------------------------------------------
  # Set the data directory

  globalpar.cur_event_dir := as_string(mapGUI.file.Entry->get());

  #---------------------------------------------------------
  # Adjust to antenna selection

  mapGUI.AntSel.Entry->delete('start','end');
  mapGUI.AntSel.Entry->insert(vector2string(ccSettings.antennas));

  #---------------------------------------------------------
  # Time domain size of the FFT; the '-1' fix should be removed later!

  mapGUI.nofSamples.Entry->delete('start','end');
  mapGUI.nofSamples.Entry->insert(as_string(ccSettings.FFTSize-1));

  #---------------------------------------------------------
  # Experiment trigger time

  for (ant in ccSettings.antennas) {
    rec.data[ant].Header.Date := as_string(ccSettings.triggertime);
  }

  #---------------------------------------------------------
  # Adjust the beammforming method 

  mapGUI.bfMethod.Button->text('ccMatrix');

  #---------------------------------------------------------
  # Update the frequency domain settings; this used the
  # information extracted from FFT plugin parameters, since
  # frequency selection can be applied already there.

  if(adjustFrequency) {
    freqRange[1] := min(ccSettings.frequencies);
    freqRange[2] := max(ccSettings.frequencies);
    freqRange /:= as_double(rec.head('FrequencyUnit'));

    mapGUI.freqRangeMin->delete('start','end');
    mapGUI.freqRangeMin->insert(as_string(freqRange[1]));
    mapGUI.freqRangeMax->delete('start','end');
    mapGUI.freqRangeMax->insert(as_string(freqRange[2]));
  }

  #---------------------------------------------------------
  # Update the plotgui

  plotGUI.refreshgui();

}


#-------------------------------------------------------------------------------
#Section: <a name="gui_help">[2]</a> Helper functions for the construction of the GUIs
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: SectionFrame
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Insert a Header Label + Subframe element into a parent frame. The
#      subframe is intended as container to take up a number of GUI elements
#      which are grouped together based on their contents.
#Par:  pFrame   - Parent frame into which the new section frame is inserted.
#Par:  headline - Title put into the headline of the new section.
#Par:  unfold   - Unfold the contents frame to make further subframes visible.
#-------------------------------------------------------------------------------

SectionFrame := function (ref pFrame, headline='New Section', unfold=T) {

  if (unfold) bitmap := 'uparrows.xbm';
  else bitmap := 'downarrows.xbm';

  f := [=];

  f.Frame  := frame (pFrame,side='top',expand='x',relief='groove');

  f.Header := frame (f.Frame,side='left',expand='x');
  f.Title  := label(f.Header, headline, background='grey70',
                    foreground='blue', padx=20, anchor='w', fill='x');
  f.Button := button(f.Header, 'Fold',
	             bitmap=spaste(globalconst.ImageDir,bitmap),
		     background='grey70', borderwidth=0, hlthickness=0,fill='y')

  f.Contents := frame(f.Frame,side='top',expand='x');

  # Folding/Unfolding of the frame holding the main contents

  f.map := unfold;

  if (!unfold) f.Contents->unmap();

  whenever f.Button->press do {
    f.map := !f.map;
    if (f.map) {
      f.Contents->map();
      f.Button->bitmap(spaste(globalconst.ImageDir,'uparrows.xbm'));
    }
    else {
      f.Contents->unmap();
      f.Button->bitmap(spaste(globalconst.ImageDir,'downarrows.xbm'));
    }
  }

  return f;

}

#-------------------------------------------------------------------------------
#Name: SectionEntry
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Insert a Label+Entry field type element into a parent frame.
#Par:  pFrame   - Parent frame into which the new section frame is inserted.
#Par:  defLabel - Label associated with the entry field.
#Par:  defEntry - Contents written to the entry field at construction time.
#-------------------------------------------------------------------------------

SectionEntry := function (ref pFrame, defLabel='secLabel', defEntry='') {

  lwidth := 25;
  rwidth := 40;

  f := [=];

  f.Frame := frame(pFrame.Contents,side='left',expand='x');
  f.Left  := frame(f.Frame,side='top',expand='x',width=lwidth);
  f.Right := frame(f.Frame,side='top',expand='x',width=rwidth);

  f.Label := label(f.Left,text=defLabel, anchor='ne', fill='x', width=lwidth);
  f.Entry := entry(f.Right, fill='x', width=rwidth, background='LemonChiffon');
  f.Entry->insert(as_string(defEntry));

  return f;

}

#-------------------------------------------------------------------------------
#Name: SectionListbox
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Insert a Label+Entry field type element into a parent frame.
#Par:  pFrame   - Parent frame into which the new section frame is inserted.
#Par:  defLabel - Label associated with the entry field.
#Par:  defEntry - Contents written to the entry field at construction time.
#-------------------------------------------------------------------------------

SectionListbox := function (ref pFrame, defLabel='secLabel', defEntry='') {

  lwidth := 25;
  rwidth := 40;

  f := [=];

  f.Frame := frame(pFrame.Contents,side='left',expand='x');
  f.Left  := frame(f.Frame,side='top',expand='x',width=lwidth);
  f.Right := frame(f.Frame,side='left',expand='x',width=rwidth);

  f.Label   := label(f.Left,text=defLabel, fill='x', width=lwidth, anchor='ne');

  f.Listbox   := listbox(f.Right,
                         mode='extended',
                         fill='x', 
                         width=rwidth, 
                         background='LemonChiffon');
  f.Scrollbar := scrollbar(f.Right);  
  f.Listbox->insert(as_string(defEntry));

  # handling of events

  whenever f.Scrollbar->scroll do {
    f.Listbox->view($value);
  }

  whenever f.Listbox->yscroll do {
    f.Scrollbar->view($value);
  }

  return f;

}

#-------------------------------------------------------------------------------
#Name: SectionButton
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Insert a Label+Button type element into a parent frame. Since buttons
#      come along in various flavours, the construction of the GUI elements
#      will depend on wether the button to be inserted is just a simple check
#      button or a pull-down menu.
#Par:  pFrame       - Parent frame into which the new section frame is inserted.
#Par:  sectionLabel - Label in the right frame
#Par:  buttonType   - Type of button.
#Par:  buttonLabel  - Label in the button.
#-------------------------------------------------------------------------------

SectionButton := function (ref pFrame, sectionLabel='Section Label',
                           buttonType='check', buttonLabel='Button Label') {

  lwidth := 25;
  rwidth := 40;

  f := [=];

  f.Frame := frame(pFrame.Contents,side='left',expand='x');
  f.Left  := frame(f.Frame,side='top',expand='x',width=lwidth);
  f.Right := frame(f.Frame,side='top',expand='x',width=rwidth);

  # since the check button symbol takes up space, we need to compensate for
  # this fact
  if (buttonType == 'check') {
    rwidth -:= 5;
  }

  f.Label  := label(f.Left,text=sectionLabel, anchor='ne',width=lwidth,fill='x');
  f.Button := button(f.Right,buttonLabel, type=buttonType, anchor='w', fill='x',
                     relief='flat',justify='left', width=rwidth)

  return f;

}


#-------------------------------------------------------------------------------
#
#Section: <a name="gui_main">[3]</a> Construction of the GUIs
#
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: updateDistanceSettings
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Function to handle changes in the settings for the distance axis of the
#      image to be computed.
#Par:  dist      - Reference to the record containing the settings of the
#                  distance axis.
#Par:  mapgui    - Reference to the GUI holding the entry fields for the
#                  distance parameters.
#Par:  fieldname - Name of the field which has been adjusted; depending in this
#                  an update of the other variables will be performed.
#Created: 2004/12/09 (L. Baehren)
#-------------------------------------------------------------------------------

updateDistanceSettings := function (ref dist, ref mapGUI,fieldname='-1')
{
  if (fieldname != '-1') {
    dist.rmin   := as_double(mapGUI.distMin->get());
    dist.incr   := as_double(mapGUI.distIncr->get());
    dist.slices := as_double(mapGUI.distSlices->get());
    dist.rmax   := as_double(mapGUI.distMax->get());

    if (dist.rmin <= 0) {
      dist.rmin  := -1;
      dist.slices := 1;
      dist.incr  := 100;
      dist.rmax  := dist.rmin;
    }
    else if (fieldname=='rmin' || fieldname=='incr' || fieldname=='slices') {
      dist.rmax := dist.rmin + (dist.slices-1)*dist.incr;
    }
    else if (fieldname=='rmax') {
      if (dist.slices > 1) dist.incr := (dist.rmax+dist.rmin)/(dist.slices-1);
    }
    else {}
  }

  # Update the fields in the GUI

  mapGUI.distMin->delete('start','end');
  mapGUI.distMin->insert(as_string(dist.rmin));

  mapGUI.distIncr->delete('start','end');
  mapGUI.distIncr->insert(as_string(dist.incr));

  mapGUI.distSlices->delete('start','end');
  mapGUI.distSlices->insert(as_string(dist.slices));

  mapGUI.distMax->delete('start','end');
  mapGUI.distMax->insert(as_string(dist.rmax));
}


#-------------------------------------------------------------------------------
#Name: updateBlockSettings
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Function to handle changes in the settings for progression through the 
#      input data volume.
#Par:  rec    - Reference to the data structure.
#Par:  mapGUI - Reference to the Skymapper GUI.
#Created: 2004/12/17 (L. Baehren)
#Status: experimental
#-------------------------------------------------------------------------------

updateBlockSettings := function (ref rec, ref mapGUI)
{
  # Extract parameter values from the GUI

  index           := eval(mapGUI.AntSel.Entry->get());;
  firstBlock      := as_integer(eval(mapGUI.firstBlock.Entry->get()));
  nofIntegrations := as_integer(eval(mapGUI.nofIntegrations.Entry->get()));
  nofBlocks       := as_integer(eval(mapGUI.nofBlocks.Entry->get()));
  nofSamples      := as_integer(eval(mapGUI.nofSamples.Entry->get()));

  # Write the new settings to the data structure

  for (ant in index) {
    ok := rec.setblock(n=firstBlock, blocksize=nofSamples, index=ant);
    ok := rec.touchref(rec.get('Input',ant),ant);
    ok := rec.touch(rec.get('Input',ant),ant);
  }

  # Update the PlotControl GUI

  rec.plotgui.refreshgui();
  rec.plotgui.replot();

  #
  mapGUI.firstBlock.Entry->delete('start','end');
  mapGUI.firstBlock.Entry->insert(as_string(firstBlock));

  #
  mapGUI.nofIntegrations.Entry->delete('start','end');
  mapGUI.nofIntegrations.Entry->insert(as_string(nofIntegrations));

  #
  mapGUI.nofBlocks.Entry->delete('start','end');
  mapGUI.nofBlocks.Entry->insert(as_string(nofBlocks));

  #
  mapGUI.nofSamples.Entry->delete('start','end');
  mapGUI.nofSamples.Entry->insert(as_string(nofSamples));

  # Dependencies

  if (nofBlocks > 1) {
    mapGUI.contentsDomain_freq->state(T);
  }
}

#-------------------------------------------------------------------------------
#Name: skymapGenerator_gui
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Provides the graphical front-end to the routines for the generation of
#      sky maps. After selecting the user's choices, the set parameters are
#      send off to 'skymapper()' which then passes the data to the Skymapper
#      client to carry out the actual computation.
#Par:  rec     - Reference to the data structure.
#Par:  plotgui - Reference to the main LOPES Tools GUI, i.e. the PlotControl
#                window.
#Ref:  updateDistanceSettings
#-------------------------------------------------------------------------------

skymapGenerator_gui:=function(ref rec, ref plotgui)
{
  global img;
  global globalpar;

  verboseON := False;
#  blocksizes := get_blocksizes();
  stdlabel  := 25;
  stdbutton := 35;
  stdEntry  := 35;
  widthFrameLeft  := 25;
  widthFrameRight := 40;
  refcodes := getCoordinateSystems();

  # ------------------------------------------------------------------
  #  Decomposition of the File blocksize
  # ------------------------------------------------------------------

  recClient := [=];
  recClient.Filesize := rec.getmeta('Filesize');

#  rec.newsagent->message('Skymapper: Decomposition of the File blocksize ...');
#  recBlocksizes := globalpar.skymapper_client->dataBlocksizes(recClient);

  #Available blocksizes are powers of two and between 128 samples and 
  #2^26 samples (== 128MB rawdata == 0.6 Hz Frequency resolution)
  recBlocksizes := [=];
  tmpsizes := 2^[7:26]
  recBlocksizes.Blocksizes := tmpsizes[(tmpsizes<=recClient.Filesize)]

  # ------------------------------------------------------------------
  #  Main frame of the GUI
  # ------------------------------------------------------------------

  tk_hold();
 
  mapgui := [=];
  mapgui.frame := frame(title='Skymap Generator',width=1000,height=350,tpos='c');

  # ----------------------------------------------------------------------------
  # Data input 
  # ----------------------------------------------------------------------------

  ### GUI elements ###

  filename := rec.get('Header').Filename;

  mapgui.data := SectionFrame (mapgui.frame,'Source of data',F);

  mapgui.dataSource := SectionButton (mapgui.data,
                                      'Source of data input : ',
                                      'menu',
                                      'Memory');

  mapgui.file := SectionEntry (mapgui.data,'Input dataset : ',filename);

  mapgui.batch := SectionButton (mapgui.data,
                                 'Batch processing : ',
                                 'check',
                                 'Process all datasets in directory');

  mapgui.datadir := SectionEntry (mapgui.data,'Data directory : ','');

  ### event handling ###

  recDataSource := [=];
  for (i in "Memory ccMatrix") {
    recDataSource[i] := button (mapgui.dataSource.Button,i,value=i);
    whenever recDataSource[i]->press do {
      mapgui.dataSource.Button->text($value);
      #
      if ($value == 'ccMatrix') {
        #
        dir:='';
        #
        setDataSource := function (dirname=dir) {
          #
          mapgui.file.Entry->delete('start','end');
          mapgui.file.Entry->insert(dirname);
          #
          mapgui.datadir.Entry->delete('start','end');
          mapgui.datadir.Entry->insert(get_topDirectory(dirname));
          #
          if (dirname != '') {
            readDataCC (rec,mapgui,plotgui);
          }
        }
        #
	if (dir == '') {
          global dcstarted;
	  dc.setselectcallback(setDataSource);
	  dc.gui(F,all,F);
          if (!dcstarted){
            dc.show(globalpar.DefITSDataDir,'All',T)
            dcstarted := T;
          }
        }
        # 
      }
    }
  }

  whenever mapgui.batch.Button->press do {
    # get the state of the button
    buttonState := as_boolean(mapgui.batch.Button->state());
    #
    if (buttonState) {
      # get the directory name
      dirname := mapgui.datadir.Entry->get();
      selectedItemsFromList (mapgui, dir=dirname);
    }

  }

  # ----------------------------------------------------------------------------
  #  Selection of antennae
  # ----------------------------------------------------------------------------

  mapgui.Antennae := SectionFrame (mapgui.frame,'Antenna setup',F);

  antennae := spaste(vector2string(seq(rec.len)[rec.select()]));
  mapgui.AntSel := SectionEntry (mapgui.Antennae,'Selected antennae : ',
                                 antennae);

  whenever mapgui.AntSel.Entry->return do {
    ant := eval(mapgui.AntSel.Entry->get());
    mapgui.AntSel.Entry->delete('start','end');
    mapgui.AntSel.Entry->insert(vector2string(ant));
  }

  mapgui.AntRef := SectionEntry (mapgui.Antennae,'Reference Antenna : ',
                                 as_string(rec.get('RefAnt')));

  mapgui.doz := SectionButton (mapgui.Antennae,'Antenna positions : ','check',
                               'Use z-coordinate');
  mapgui.doz.Button->state(T);

  mapgui.xy := SectionButton (mapgui.Antennae,' ','check',
                              'Shift to shower core position');
  mapgui.xy.Button->state(F);

  # ----------------------------------------------------------------------------
  #  Access to the data:
  #   Set the input source for the map to be generated; as default we're using
  #   the settings from the main plot controls GUI, but we also can chose
  #   generation of maps from all files located in a certain directory.
  # ----------------------------------------------------------------------------

  mapgui.Data := SectionFrame (mapgui.frame,'Processing of the data',T);

  # [1] Samples/Block

  mapgui.BlocksSamples := SectionButton (mapgui.Data,'Process complete dataset : ',
                                         'menu', '-- unused --');

  numberSamples := recBlocksizes.Blocksizes;
  numberBlocks  := as_integer(rec.getmeta('Filesize')/numberSamples);
  recINdatafile := [=];
  for (i in 1:len(numberSamples)) {
    recID := as_string(i);
    buttontext := spaste(numberBlocks[i],
                         ' integrations of ',
                         numberSamples[i],
                         ' samples.');
    recINdatafile[recID] := button (mapgui.BlocksSamples.Button,text=buttontext,
                                    value=numberSamples[i]);
    whenever recINdatafile[recID]->press do {
      # variables values
      nsamples   := eval($value);
      nIntegrations    := rec.getmeta('Filesize')/nsamples;
      buttontext := spaste(as_integer(nIntegrations),
                           ' integrations of ',
                           nsamples,
                           ' samples.');
      # Number of integrations
      mapgui.nofIntegrations.Entry->delete('start','end');
      mapgui.nofIntegrations.Entry->insert(as_string(nIntegrations));
      # Datablocks / Integration
      mapgui.nofBlocks.Entry->delete('start','end');
      mapgui.nofBlocks.Entry->insert('1');
      # Samples / Datablock
      mapgui.nofSamples.Entry->delete('start','end');
      mapgui.nofSamples.Entry->insert(as_string($value));
      # Datablocks / Integration
      mapgui.firstBlock.Entry->delete('start','end');
      mapgui.firstBlock.Entry->insert('1');
      # update button text
      mapgui.BlocksSamples.Button->text(buttontext);
      # settings in the main GUI
      rec.setblock(1,nsamples);
      rec.touch(rec.get('Input'));
      plotgui.replot();
      plotgui.blocksizeentvalue:=as_string(nsamples);
    }
  }

  mapgui.nofSamples := SectionEntry (mapgui.Data,
                                    'Samples / Datablock : ',
                                    rec.getmeta('Blocksize'));

  whenever mapgui.nofSamples.Entry->return do {
    updateBlockSettings (rec, mapgui);
  }

  # [2] Blocks/Integration

  mapgui.nofBlocks := SectionEntry (mapgui.Data,
                                    'Datablocks / Integration : ','1');

  whenever mapgui.nofBlocks.Entry->return do {
    updateBlockSettings (rec, mapgui);
  }

  # [3] Number of integrations, i.e. number of timeframes for which an
  #     individual image tool is created

  mapgui.nofIntegrations := SectionEntry (mapgui.Data,
                                          'Number of Integrations : ','1');

  whenever mapgui.nofIntegrations.Entry->return do {
    updateBlockSettings (rec, mapgui);
  }

  # [4] First block

  mapgui.firstBlock := SectionEntry (mapgui.Data,
                                     'Start from datablock : ','1');

  whenever mapgui.firstBlock.Entry->return do {
    updateBlockSettings (rec, mapgui);
  }
 
  # ----------------------------------------------------------------------------
  # Beamforming
  # ----------------------------------------------------------------------------

  mapgui.Beamforming := SectionFrame (mapgui.frame,'Beamforming',F);

  mapgui.bfMethod := SectionButton (mapgui.Beamforming,'Beamforming method : ',
                                    'menu','addSignals')
  recBF := [=];
  for (i in "addSignals crossCorr ccMatrix") {
    recBF[i] := button (mapgui.bfMethod.Button,i,value=i);
    whenever recBF[i]->press do {
      mapgui.bfMethod.Button->text($value);
    }
  }

  whenever recBF['ccMatrix']->press do {
    print "Beamforming method changed to processing of cross-correlated data";
  }

  # Type of data passed to the Skymapper CLI

  mapgui.dataType := SectionButton (mapgui.Beamforming,'Processed data type : ',
                                     'menu','CalFFT')

  recData := [=];
  for (i in "CalFFT FFT Voltage") {
    recData[i] := button (mapgui.dataType.Button,i,value=i);
    whenever recData[i]->press do {
      mapgui.dataType.Button->text($value);
    }
  }

  # ------------------------------------------------------------------
  #  Setup of the map: Position of center, resolution, number of 
  #  pixels, ...
  # ------------------------------------------------------------------

  mapgui.Map := SectionFrame (mapgui.frame,'Characteristics of the map',T);

  # ------------------------------------------------------------------
  # The center of the map (in celestial sphercial coordinates)

  mapgui.centerFrame := frame(mapgui.Map.Contents,side='left',expand='x');
  mapgui.centerLabel := label(mapgui.centerFrame,'Center of the map : ',
                              width=widthFrameLeft,anchor='e');
  mapgui.centerLonEntry:=entry(mapgui.centerFrame,width=4,background='LemonChiffon');
  mapgui.centerLonEntry->insert('0');
  mapgui.centerLatEntry:=entry(mapgui.centerFrame,width=4,background='LemonChiffon');
  mapgui.centerLatEntry->insert('90');
  mapgui.centerUnit := label(mapgui.centerFrame,'(deg)',width=5);

  # reference code

  mapgui.WCScenter := frame(mapgui.Map.Contents,side='left',expand='x');
  mapgui.WCScenterLabel := label(mapgui.WCScenter,'Reference code : ',
                                  width=widthFrameLeft,anchor='e');
  mapgui.WCScenterButton := button (mapgui.WCScenter,'AZEL',type='menu',
                                     relief='flat',justify='left',
                                     width=widthFrameRight,fill='x',anchor='w');
  recWCScenter := [=];
  for (i in refcodes) {
    recWCScenter[i] := button (mapgui.WCScenterButton,i,value=i);
    whenever recWCScenter[i]->press do {
      mapgui.WCScenterButton->text($value);
    }
  }

  # ----------------------------------------------------------------------------
  #  Number of pixels

  mapgui.pixelsFrame := frame(mapgui.Map.Contents,side='left',expand='x');
  mapgui.pixelsLabel := label(mapgui.pixelsFrame,'Pixels of the map : ',width=25,anchor='e');
  mapgui.pixelsMinEntry:=entry(mapgui.pixelsFrame,width=4,background='LemonChiffon');
  mapgui.pixelsMinEntry->insert('120');
  mapgui.pixelsMaxEntry:=entry(mapgui.pixelsFrame,width=4,background='LemonChiffon');
  mapgui.pixelsMaxEntry->insert('120');
  mapgui.pixelsUnit := label(mapgui.pixelsFrame,' ',width=5);

  # ----------------------------------------------------------------------------
  #  Range in Azimuth/Longitude [deg]

#  mapgui.rangeAZFrame := frame(mapgui.Map.Contents,side='left',expand='x');
#  mapgui.rangeAZLabel := label(mapgui.rangeAZFrame,'Range in Azimuth : ',width=25,anchor='e');
#  mapgui.rangeAZMinEntry:=entry(mapgui.rangeAZFrame,width=4,background='LemonChiffon');
#  mapgui.rangeAZMinEntry->insert('0');
#  mapgui.rangeAZMaxEntry:=entry(mapgui.rangeAZFrame,width=4,background='LemonChiffon');
#  mapgui.rangeAZMaxEntry->insert('360');
#  mapgui.rangeAZUnit := label(mapgui.rangeAZFrame,'(deg)',width=5);

  # ----------------------------------------------------------------------------
  # Range in Elevation/Latitude [deg]

  mapgui.rangeELFrame := frame(mapgui.Map.Contents,side='left',expand='x');
  mapgui.rangeELLabel := label(mapgui.rangeELFrame,'Range in Elevation : ',width=25,anchor='e');
  mapgui.rangeELMinEntry:=entry(mapgui.rangeELFrame,width=4,background='LemonChiffon');
  mapgui.rangeELMinEntry->insert('0');
  mapgui.rangeELMaxEntry:=entry(mapgui.rangeELFrame,width=4,background='LemonChiffon');
  mapgui.rangeELMaxEntry->insert('90');
  mapgui.rangeELUnit := label(mapgui.rangeELFrame,'(deg)',width=5);

  # ----------------------------------------------------------------------------
  #  Angular resolution [deg] in celestial coordinates.

  mapgui.resframe := frame(mapgui.Map.Contents,side='left',expand='x');
  mapgui.reslab   := label(mapgui.resframe,'Angular Resolution : ',
                           width=widthFrameLeft,anchor='e');
  mapgui.resent   := entry(mapgui.resframe,width=10,background='LemonChiffon');
  mapgui.resUnit  := label(mapgui.resframe,'(deg)',width=5);
  mapgui.resent->insert('2')

  # ----------------------------------------------------------------------------
  # Distance scale for near-field beamforming

  mapgui.Dist := SectionFrame (mapgui.frame,
                               'Stepping for near-field beamforming',F);  

  mapgui.distRange_f := frame(mapgui.Dist.Contents,side='left',expand='x');
  
  mapgui.distRange_l   := label(mapgui.distRange_f,'Distance range : ',
                                width=widthFrameLeft,anchor='e');
  
  mapgui.distMin   := entry(mapgui.distRange_f,width=4,background='LemonChiffon');
  mapgui.distMax   := entry(mapgui.distRange_f,width=4,background='LemonChiffon');
  mapgui.distUnit1 := label(mapgui.distRange_f,'(m)',width=5);

  mapgui.distSlices_f := frame(mapgui.Dist.Contents,side='left',expand='x');
  mapgui.distSlices_l := label(mapgui.distSlices_f,'Number of distance slices : ',
                               width=widthFrameLeft,anchor='e');
  mapgui.distSlices   := entry(mapgui.distSlices_f,width=10,
                               background='LemonChiffon');
  mapgui.distSlices_u := label(mapgui.distSlices_f,'',width=5);
  mapgui.distSlices_e->insert('1')

  mapgui.distIncr_f := frame(mapgui.Dist.Contents,side='left',expand='x');
  mapgui.distIncr_l := label(mapgui.distIncr_f,'Increment between slices : ',
                           width=widthFrameLeft,anchor='e');
  mapgui.distIncr   := entry(mapgui.distIncr_f,width=10,background='LemonChiffon');
  mapgui.distIncr_u := label(mapgui.distIncr_f,'(m)',width=5);
  mapgui.distIncr_e->insert('100')

  distance := [=];
  distance.rmin  := -1;
  distance.slices := 1;
  distance.incr  := 100;
  distance.rmax  := -1;

  updateDistanceSettings (distance,mapgui,fieldname="");

  whenever mapgui.distMin->return do updateDistanceSettings (distance,mapgui,'rmin');
  whenever mapgui.distIncr->return do updateDistanceSettings (distance,mapgui,'incr');
  whenever mapgui.distSlices->return do updateDistanceSettings (distance,mapgui,'slices');
  whenever mapgui.distMax->return do updateDistanceSettings (distance,mapgui,'rmax');

  # ----------------------------------------------------------------------------
  #  Setup of the World Coordinate System (WCS)
  # 1) while in the WCS convention for
  #  - zenithal projections : south up, east left
  #  - cylindrical projections : north up, east left
  #  we want to have the flexibility to adjust the orientation to our needs.
  # ----------------------------------------------------------------------------

  # fetch the set of options and set default values for the related variables

  projections   := getProjections();
  mapProjection := 'STG';
  mapRefcode    := 'AZEL';

  # WCS : Main frame

  mapgui.WCS := SectionFrame (mapgui.frame,'World Coordinate Systems (WCS)',F);
  
  # WCS : Reference code of celestial coordinate system

  mapgui.WCSrefcode := SectionButton (mapgui.WCS,'Reference Code : ','menu',mapRefcode)

  recWCSrefcode := [=];
  for (i in refcodes) {
    recWCSrefcode[i] := button (mapgui.WCSrefcode.Button,i,value=i);
    whenever recWCSrefcode[i]->press do {
      mapgui.WCSrefcode.Button->text($value);
    }
  }

  # WCS : Reference code of spherical-to-cartesian projection

  mapgui.WCSprojection := SectionButton (mapgui.WCS,'Coordinate projection : ',
                                         'menu',mapProjection)

  recWCSprojection := [=];
  for (i in projections) {
    recWCSprojection[i] := button (mapgui.WCSprojection.Button,i,value=i);
    whenever recWCSprojection[i]->press do {
      mapgui.WCSprojection.Button->text($value);
    }
  }

  # WCS : Map orientation - direction towards top

  mapgui.WCSmapTop := SectionButton (mapgui.WCS,'Map orientation - top : ',
                                     'menu','North')

  recUp := [=];
  for (i in "North South") {
    recUp[i] := button (mapgui.WCSmapTop.Button,i,value=i);
    whenever recUp[i]->press do {
      mapgui.WCSmapTop.Button->text($value);
    }
  }

  # WCS : Map orientation - direction towards left

  mapgui.WCSmapLeft := SectionButton (mapgui.WCS,'Map orientation - left : ',
                                      'menu','East')

  recLeft := [=];
  for (i in "East West") {
    recLeft[i] := button (mapgui.WCSmapLeft.Button,i,value=i);
    whenever recLeft[i]->press do {
      mapgui.WCSmapLeft.Button->text($value);
    }
  }

  # ----------------------------------------------------------------------------
  #  Settings for the frequency channels
  # ----------------------------------------------------------------------------

  freqRange := VerifyFrequencyBand (rec,band=[-1,-1],margin=0.07);

  mapgui.Frequency := SectionFrame (mapgui.frame,
                                    'Processing in the Frequency domain',T);
 
  # Selection of the frequency band
  mapgui.freqRange_frame := frame(mapgui.Frequency.Contents,side='left',expand='x');
  mapgui.freqRange_label:=label(mapgui.freqRange_frame,'Selected frequency band : ',
                                width=widthFrameLeft,anchor='e');
  mapgui.freqRangeMin:=entry(mapgui.freqRange_frame,width=4,background='LemonChiffon');
  mapgui.freqRangeMax:=entry(mapgui.freqRange_frame,width=4,background='LemonChiffon');
  mapgui.freqRangeUnits:=label(mapgui.freqRange_frame,'(MHz)',anchor='e');
  mapgui.freqselection := button(mapgui.freqRange_frame,'Selection', borderwidth=1);
  mapgui.freqRangeMin->insert(as_string(freqRange[1]));
  mapgui.freqRangeMax->insert(as_string(freqRange[2]));

  whenever mapgui.freqselection->press do {
    if (plotgui.selactive) {
      scale := plotgui.UnitsscaleFactor[1]/1e6;
      mapgui.freqRangeMin->delete('start','end');
      mapgui.freqRangeMax->delete('start','end');
      mapgui.freqRangeMin->insert(as_string(plotgui.selected_region_x1*scale));  
      mapgui.freqRangeMax->insert(as_string(plotgui.selected_region_x2*scale));  
    } else {
      print 'Selection not active in plot panel!'
    }
  }

  # Number of output frequency channels

  mapgui.FreqChannels := SectionEntry (mapgui.Frequency,
                                       '# frequency channels : ','-1');

  whenever mapgui.FreqChannels.Entry->return do {
    nofChannels := as_integer(eval(mapgui.FreqChannels.Entry->get()));
    if (nofChannels == 0) {
      mapgui.FreqChannels.Entry->delete('start','end');
      mapgui.FreqChannels.Entry->insert('-1');
    }
  }

  # Removal of radio interference

  mapgui.autoflag := SectionButton (mapgui.Frequency,'RFI Mitigation : ','check',
                               'Autoflagging RFI contamination');

  # ----------------------------------------------------------------------------
  #  Electric quantity displayed in the map

  mapgui.Quantity := SectionFrame (mapgui.frame,
                                   'Electric quantity displayed in the map',T);
 
  mapgui.contentsQuantity_frame := frame(mapgui.Quantity.Contents, side='left',
                                         expand='x');
  mapgui.contentsQuantity_label := label(mapgui.contentsQuantity_frame,
                                         'Electric ...', anchor='w', fill='x',
                                         width=stdbutton/3.0);
  mapgui.contentsQuantity_power := button(mapgui.contentsQuantity_frame,
                                          '... power', type='radio', anchor='w',
                                          fill='x', justify='left',relief='flat',
                                          width=stdbutton/3.0);
  mapgui.contentsQuantity_field := button(mapgui.contentsQuantity_frame,
                                          '... field', type='radio', anchor='w',
                                          fill='x', justify='left',relief='flat',
                                          width=stdbutton/3.0);
  mapgui.contentsQuantity_power->state(T);

  mapgui.contentsDomain_frame := frame(mapgui.Quantity.Contents, side='left',
                                       expand='x');
  mapgui.contentsDomain_label := label(mapgui.contentsDomain_frame,
                                       'Signal domain', anchor='w', fill='x',
                                       width=stdbutton/3.0);
  mapgui.contentsDomain_freq := button(mapgui.contentsDomain_frame, 'Frequency',
                                       type='radio', anchor='w', fill='x',
                                       relief='flat', justify='left', 
                                       width=stdbutton/3.0);
  mapgui.contentsDomain_time := button(mapgui.contentsDomain_frame, 'Time',
                                       type='radio', anchor='w', fill='x',
                                       relief='flat', justify='left',
                                       width=stdbutton/3.0);
  mapgui.contentsDomain_freq->state(T);

  whenever mapgui.contentsDomain_time->press do {
    # (a) Adjust the number of datablocks per integration
    mapgui.nofBlocks_entry->delete('start','end');
    mapgui.nofBlocks_entry->insert('1');
    # (b) Adjust the number of output frequency channels
    mapgui.FreqChannels.Entry->delete('start','end');
    mapgui.FreqChannels.Entry->insert('-1');
    # (c) Adjust the range of the frequency band
    freqRange[1] := rec.getmeta('FrequencyLow');
    freqRange[2] := rec.getmeta('FrequencyHigh');
    freqRange /:= as_double(rec.head('FrequencyUnit'));
    mapgui.freqRangeMin->delete('start','end');
    mapgui.freqRangeMin->insert(as_string(freqRange[1]));
    mapgui.freqRangeMax->delete('start','end');
    mapgui.freqRangeMax->insert(as_string(freqRange[2]));
    # (d) Adjust the beamforming method
    mapgui.bfMethod.Button->text('addSignals');
  }

  # ----------------------------------------------------------------------------
  #  Calibration procedures
  # ----------------------------------------------------------------------------

  mapgui.Calibration := SectionFrame (mapgui.frame,'Calibration',F);

  mapgui.PBeam := SectionButton (mapgui.Calibration,'','check',
                                 'Map of the array primary beam');

  mapgui.Selfcal := SectionButton (mapgui.Calibration,'','check',
                                   'Selfcalibration on brightest point source');

  # ----------------------------------------------------------------------------
  # Display and export of data
  # ----------------------------------------------------------------------------

  mapgui.Export := SectionFrame (mapgui.frame,
                                 'Display and export of generated data',F);

  # Status snapshots during computation of the skymap

  mapgui.Snapshots := SectionEntry (mapgui.Export,
                                    'Computation snapshots : ',
                                    '0');

  whenever mapgui.Snapshots.Entry -> return do {
    nofSnapshots := eval(mapgui.Snapshots.Entry->get());
    mapgui.Snapshots.Entry->delete('start','end');
    mapgui.Snapshots.Entry->insert(nofSnapshots);
  }

  # Export image to FITS

  mapgui.exportFITS := SectionButton (mapgui.Export,
                                      'Image export : ',
                                      'check',
                                      'Export image to FITS');
  mapgui.exportFITS.Button->state(F);

  # Display of the map at the end of an integration

  mapgui.ViewMap := SectionButton (mapgui.Export,
                                   'Display : ',
                                   'check',
                                   'Display the map for each integration');

  # Use external viewer to display the created image

  mapgui.ViewerDS9 := SectionButton (mapgui.Export,
                                     'External viewer : ',
                                     'check',
                                     'Display image using DS9');

  whenever mapgui.ViewerDS9.Button -> press do {
    viewerIsDS9 := as_boolean(mapgui.ViewerDS9.Button->state());
    #
    if (viewerIsDS9) {
      mapgui.exportFITS.Button->state(T);  
    }
  }

  #
  # --- Button for starting the generation of the map ----------------
  #

  mapgui.fstart:=frame(mapgui.frame,side="left");

  mapgui.mapbut:=button(mapgui.fstart,'Generate Map',background='green',
                        padx='20',fill='x');

  mapgui.analysis:=button(mapgui.fstart,'Analyze image tool',
                          background='LemonChiffon',padx='20',fill='x');

  whenever mapgui.analysis->press do { ImageToolsGUI(rec); };

  mapgui.save := button(mapgui.fstart,
                        'Save settings',
                        background='LemonChiffon',
                        padx='20',
                        fill='x');

  whenever mapgui.save->press do
  {
    write_value(mapgui,"Skymapper.sos");
  };

  mapgui.endbut:=button(mapgui.fstart,'Dismiss',background='orange',
                        padx='20',fill='x');

  whenever mapgui.endbut->press do
  {
    val mapgui.frame:=F;
    val mapgui:=F;
  };

  tk_release();

  # ----------------------------------------------------------------------------
  #  Start of the computation
  # ----------------------------------------------------------------------------

  #
  # Pressing the button 'Gen Map' starts generating the map; the mapping itself
  # is done by routine 'skymaper()' in 'skymaper.g'
  #
  whenever mapgui.mapbut->press do {

    rec.newsagent->message('[SkymapperGUI] Extracting parameters from GUI ...');

    variablesOK := T;

    # ----------------------------------------------------------------
    # Antenna related parameters

    rec.newsagent->message('[SkymapperGUI] Extracting antenna parameters ...');
   
    paramAntenna := [=];

    paramAntenna.index  := eval(mapgui.AntSel.Entry->get());
    paramAntenna.refant := as_integer(mapgui.AntRef.Entry->get());
    paramAntenna.doz    := as_boolean(mapgui.doz.Button->state());
    paramAntenna.xy     := as_boolean(mapgui.xy.Button->state());

    # ----------------------------------------------------------------
    # Check if the values are set reasonably (e.g. upper limit of a
    # range indeed should be larger than the lower limit of the range).

    rec.newsagent->message('[SkymapperGUI] Extracting WCS parameters ...');
   
    mapWCS        := as_string(mapgui.WCSrefcode.Button->text());
    mapProjection := as_string(mapgui.WCSprojection.Button->text());

    mapCenter[1]  := as_double(eval(mapgui.centerLonEntry->get()));
    mapCenter[2]  := as_double(eval(mapgui.centerLatEntry->get()));
    mapCenterWCS  := as_string(mapgui.WCScenterButton->text($value));
    mapPixels[1]  := as_double(eval(mapgui.pixelsMinEntry->get()));
    mapPixels[2]  := as_double(eval(mapgui.pixelsMaxEntry->get()));

#    rangeAZ:=[as_double(mapgui.rangeAZMinEntry->get()),
#              as_double(mapgui.rangeAZMaxEntry->get())];

    rangeEL := [as_double(eval(mapgui.rangeELMinEntry->get())),
                as_double(eval(mapgui.rangeELMaxEntry->get()))];

    incrCSC := [as_double(eval(mapgui.resent->get())),
                as_double(eval(mapgui.resent->get()))];

    updateDistanceSettings (distance,mapgui,'rmin');

    # ----------------------------------------------------------------
    # Frequency domain setings: number of channels, range of selected
    # band, etc.

    rec.newsagent->message('[SkymapperGUI] Extracting frequency parameters ...');
   
    band:=[as_double(eval(mapgui.freqRangeMin->get())),
           as_double(eval(mapgui.freqRangeMax->get()))];

    nofChannels := as_integer(eval(mapgui.FreqChannels.Entry->get()));
    if (nofChannels == 0)  nofChannels := -1;

    # ----------------------------------------------------------------
    # Characterization of the Signal displayed in the map

    rec.newsagent->message('[SkymapperGUI] Extracting signal parameters ...');

    if (mapgui.contentsQuantity_power->state()) SignalQuantity := "power";
    else if (mapgui.contentsQuantity_field->state()) SignalQuantity := "field";
    else SignalQuantity := "power";

    if (mapgui.contentsDomain_time->state()) SignalDomain := "time";
    else if (mapgui.contentsDomain_freq->state()) SignalDomain := "freq";
    else SignalDomain := "freq";

    # ----------------------------------------------------------------
    # Beamforming method

    bfMethod := mapgui.bfMethod.Button->text($value);

    # ----------------------------------------------------------------
    # Export and Display

    exportToFITS := as_boolean(mapgui.exportFITS.Button->state());
    viewerIsDS9  := as_boolean(mapgui.ViewerDS9.Button->state());

    # ----------------------------------------------------------------
    # Set up list of input datasets to be processed.

    rec.newsagent->message('[SkymapperGUI] Checking batchmode parameters ...');

    batchMode := as_boolean(mapgui.batch.Button->state());

    if (batchMode && has_field(mapgui,'selection')) {
      datadir := mapgui.datadir.Entry->get();
      nofDatafiles := len(mapgui.selection);
      #
      for (i in 1:nofDatafiles){
        datafiles[i] := spaste(datadir,'/',mapgui.selection[i]);
      }
    } 
    else {
      datafiles[1] := as_string(mapgui.file.Entry->get());
    }

    nofDatafiles := len(datafiles);
    dataType     := as_string(mapgui.dataType.Button->text($value));

    # ----------------------------------------------------------------
    # Feedback to screen

    if (verboseON) {
      print "[skymapGenerator_gui] Value of variables passed to Skymapper";
      ### Antenna selection 
      print "- Selected antennae ......... :",paramAntenna.index;
      print "- Reference antenna ......... :",paramAntenna.refant;
      print "- Use antenna z-coordinate .. :",paramAntenna.doz;
      print "- Shift to shower core ...... :",paramAntenna.xy;
      ### Data access
      print "- Number of integrations .... :",mapgui.nofIntegrations.Entry->get();
      print "- Blocks per integration .... :",mapgui.nofBlocks.Entry->get();
      print "- Samples per block ......... :",mapgui.nofSamples.Entry->get();
      print "- First block processed ..... :",mapgui.firstBlock.Entry->get();
      ### Data processing 
      print "- Process multiple datasets . :",batchMode;
      print "- Beamforming method ........ :",bfMethod;
      print "- Type of processed data .... :",dataType;
      ### WCS
      print "- WCS reference code ........ :",mapWCS;
      print "- Projection reference code . :",mapProjection;
      print "- Map orientation up ........ :",mapgui.WCSmapTop.Button->text($value);
      print "- Map orientation left ...... :",mapgui.WCSmapLeft.Button->text($value);
      print "- Coordinates of map center . :",mapCenter,mapCenterWCS;
      ### Calibration
      print "- Autoflag RFI .............. :",mapgui.autoflag.Button->state();
      print "- Map of main beam .......... :",mapgui.PBeam.Button->state();
      print "- Calibration on point source :",mapgui.Selfcal.Button->state();
    }

    #
    # If all variables are ok, call the skymaper() routine.
    #
    if (variablesOK) {

      bar := progress (0,
                       nofDatafiles,
                       '[Skymapper] Processing datasets ...',);
      bar.activate();

      for (numDatafile in 1:nofDatafiles) {

        print 'Processing datafile',numDatafile,'/',nofDatafiles,':',
              datafiles[numDatafile];

        # update the GUI
        mapgui.file.Entry->delete('start','end');
        mapgui.file.Entry->insert(datafiles[numDatafile]);

        # get the obervation epoch
        if ((nofDatafiles > 1) && (bfMethod == 'ccMatrix')) {
          readDataCC (rec, mapgui, plotgui, loadExampledata=F, adjustFrequency=F);
        }

        if (is_agent(rec.newsagent)) {
          rec.newsagent->message('[SkymapperGUI] Starting Skymapper ...');
        }

        skymapper (rec,
                   fromDir=F,
                   mapWCS=mapWCS,
                   mapProjection=mapProjection,
                   mapCenterCoords=mapCenter,
                   mapCenterWCS=mapCenterWCS,
                   mapDirectionUp=as_string(mapgui.WCSmapTop.Button->text($value)),
                   mapDirectionLeft=as_string(mapgui.WCSmapLeft.Button->text($value)),
                   incrCSC=incrCSC,
                   rangeElevation=rangeEL,
                   mapPixels=mapPixels,
                   mapDistance=distance,
                   band=band,
                   paramAntenna=paramAntenna,
                   nofIntegrations=as_integer(mapgui.nofIntegrations.Entry->get()),
                   nofBlocks=as_integer(mapgui.nofBlocks.Entry->get()),
                   nofSamples=as_integer(mapgui.nofSamples.Entry->get()),
                   firstBlock=as_integer(mapgui.firstBlock.Entry->get()),
                   channels=nofChannels,
                   SignalQuantity=SignalQuantity,
                   SignalDomain=SignalDomain,
                   exportFITS=exportToFITS,
                   backupsNumber=as_integer(eval(mapgui.Snapshots.Entry->get())),
                   autoflagging=as_boolean(mapgui.autoflag.Button->state()),
                   selfcal=as_boolean(mapgui.Selfcal.Button->state()),
                   pbeam=as_boolean(mapgui.PBeam.Button->state()),
                   bfMethod=bfMethod,
                   dataType=dataType,
                   ViewMap=as_boolean(mapgui.ViewMap.Button->state()),
                   viewerIsDS9=viewerIsDS9
 	          );

        bar.update(numDatafile);

      } ### end numDatafile ###

      # report status to GUI

      if (is_agent(rec.newsagent)) {
        rec.newsagent->message('[SkymapperGUI] Skymapper finished.');
      }

    }
    else {
      if (is_agent(rec.newsagent)) {
        rec.newsagent->message('[SkymapperGUI] Skymapper not started!');
      }
    }

  }

}
