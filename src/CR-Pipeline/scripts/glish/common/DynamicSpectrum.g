
#-------------------------------------------------------------------------------
# $Id: DynamicSpectrum.g,v 1.3 2006/07/12 11:59:14 bahren Exp $
#-------------------------------------------------------------------------------
# Alternative set of functions for the generation and analysis of dynamic 
# spectra. In contrast to the routines provided by
# <a href="dynspec.doc.html">dynspec.g</a> the present
# implementation does no rely on the Skymapper, but instead only utilizes the
# functionality provided by the
# <a href="http://www.astron.nl/~bahren/research/coding/classBeamformer.html">Beamformer</a>
# class.
# For a detailed description of the functionality consult the online
# documentation on the <a href="http://www.astron.nl/~bahren/research/coding/dynspec.html">Generation of dynamic spectra</a>
# <ol>
#   <li><a href="#gui">Graphical User Interface (GUI)</a>
#   <li><a href="#generation">Master routines for computing dynamic spectra</a>
#   <li><a href="#analysis">Analysis of dynamic spectra</a>
#   <li><a href="#events">Detection of events</a>
#   <li><a href="#helpers">Helper functions</a>
# </ol>
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Section: <a name="gui">1.</a> Graphical User Interface (GUI)
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: DynamicSpectrumGUI
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Provides a Graphical User Interface (GUI) to the
#      <a href="#generation">routines </a> for computing dynamic spectra.
#Par:  rec - Data record; passed by reference.
#Created: 2004/11/01 (Lars Baehren)
#Status: production
#-------------------------------------------------------------------------------

DynamicSpectrumGUI := function (ref rec) {

  tk_hold();

  DSGUI:=[=];

  # creation of outer GUI frame with title and dimensions
  DSGUI.frame := frame(title='Dynamic spectrum Generator',
                       width=1000,height=350,tpos='c');

  #-------------------------------------------------------------------
  # Data input 

  filename := rec.get('Header').Filename;
  date     := rec.get('Header').Date;

  DSGUI.data := SectionFrame (DSGUI.frame,'Dynamic spectrum: Data I/O',T);

  DSGUI.file := SectionEntry (DSGUI.data,'Input dataset : ',filename);

  DSGUI.date := SectionEntry (DSGUI.data,'Observation date : ',date);

  #-------------------------------------------------------------------
  # Flagging

  DSGUI.flagging := SectionFrame (DSGUI.frame,
                                  'Dynamic spectrum: Antenna flagger',
                                  F);

  DSGUI.flagBlocks := SectionEntry (DSGUI.flagging,
                                    '# of blocks to inspect : ',
                                    '1');

  DSGUI.flagClipping := SectionEntry (DSGUI.flagging,
                                     'Clipping fraction : ',
                                     '0.01');

  DSGUI.flagThreshold := SectionEntry (DSGUI.flagging,
                                       'Flagging threshold : ',
                                       '2.5');

  #-------------------------------------------------------------------
  # Basic control parameters for the generation of a dynamic spectrum

  antennae      := vector2string(seq(rec.len)[rec.select()]);
  Filesize      := rec.getmeta('Filesize');
  nofSamples    := as_integer(rec.getmeta('Blocksize'));
  nofBlocks     := as_integer(Filesize/nofSamples);
  freqBand      := vector2string(VerifyFrequencyBand(rec,[-1,-1]));
  direction     := vector2string([0,90]);
  refcodes      := getCoordinateSystems();

  DSGUI.processing := SectionFrame (DSGUI.frame,'Dynamic spectrum: Generation',T);

  DSGUI.antennae := SectionEntry (DSGUI.processing,'Antenna selection : ',antennae);

  whenever DSGUI.antennae.Entry->return do {
    antennae := eval(DSGUI.antennae.Entry->get());
    DSGUI.antennae.Entry->delete('start','end');
    DSGUI.antennae.Entry->insert(vector2string(antennae));
  }  

  DSGUI.StartBlock := SectionEntry (DSGUI.processing,'Start block : ','1');

  whenever DSGUI.StartBlock.Entry->return do {
    ### extract variable values from GUI
    start := eval(DSGUI.StartBlock.Entry->get());
    samples := eval(DSGUI.nofSamples.Entry->get());
    ### update DSGUI entry field
    DSGUI.StartBlock.Entry->delete('start','end');
    DSGUI.StartBlock.Entry->insert(as_string(start));
    ### Update the PlotControl window
    rec.setblock(n=start,blocksize=samples);
    plotgui.replot();
  }  

  DSGUI.nofBlocks := SectionEntry (DSGUI.processing,'Number of blocks : ',nofBlocks);

  whenever DSGUI.nofBlocks.Entry->return do {
    blocks := eval(DSGUI.nofBlocks.Entry->get());
    DSGUI.nofBlocks.Entry->delete('start','end');
    DSGUI.nofBlocks.Entry->insert(as_string(blocks));
  }  

  DSGUI.nofSamples := SectionEntry (DSGUI.processing,'Samples per block : ',nofSamples);

  whenever DSGUI.nofSamples.Entry->return do {
    ### extract variable values from GUI
    startBlock := eval(DSGUI.StartBlock.Entry->get());
    nofBlocks  := eval(DSGUI.nofBlocks.Entry->get());
    samples := eval(DSGUI.nofSamples.Entry->get());
    ### update DSGUI entry field
    DSGUI.nofSamples.Entry->delete('start','end');
    DSGUI.nofSamples.Entry->insert(as_string(samples));
    #
    if (startBlock ==1) nofBlocks := as_integer(Filesize/samples);
    #
    while (((startBlock+nofBlocks-1)*nofBlocks) > Filesize) {
      nofBlocks -:= 1;
    }
    ### Update the DSGUI
    DSGUI.nofBlocks.Entry->delete('start','end');
    DSGUI.nofBlocks.Entry->insert(as_string(nofBlocks));
    ### Update the plotgui
    rec.setblock(n=startBlock,blocksize=samples);
    plotgui.replot();
  }

  DSGUI.freqBand := SectionEntry (DSGUI.processing,'Frequency band [MHz] : ',freqBand);

  DSGUI.dsMethod := SectionButton (DSGUI.processing,' Computation method: ','menu',
                                   'power')

  recMethod    := [=];
  for (i in "beam power") {
    recMethod[i] := button (DSGUI.dsMethod.Button,i,value=i);
    whenever recMethod[i]->press do {
      DSGUI.dsMethod.Button->text($value);
    }
  }

  DSGUI.direction := SectionEntry (DSGUI.processing,'Beam direction : ',direction);
	
  DSGUI.wcs := SectionButton (DSGUI.processing,'Beam direction WCS : ','menu','AZEL')

  recWCSrefcode := [=];
  for (i in refcodes) {
    recWCSrefcode[i] := button (DSGUI.wcs.Button,i,value=i);
    whenever recWCSrefcode[i]->press do {
      DSGUI.wcs.Button->text($value);
    }
  }

  #-------------------------------------------------------------------
  # Analysis of the generated dynamic spectrum

  DSGUI.analysis := SectionFrame (DSGUI.frame,'Dynamic spectrum: Analysis',F);

  DSGUI.average := SectionButton (DSGUI.analysis,'','check','Compute averaged spectra')
  DSGUI.average.Button->state(T);

  DSGUI.differential := SectionButton (DSGUI.analysis,'','check',
                                       'Compute differential spectra')
  DSGUI.differential.Button->state(F);

  DSGUI.antcc := SectionButton (DSGUI.analysis,'','check',
                                'Cross-correlation of antenna signals');
  
  #-------------------------------------------------------------------
  # Annotation of the generated plots

  DSGUI.plotting := SectionFrame (DSGUI.frame,
                                  'Dynamic spectrum: Plot annotation',F);

  #-------------------------------------------------------------------

  tk_release();

  #-------------------------------------------------------------------
  # Control buttons

  DSGUI.control := frame(DSGUI.frame,side='left',expand='x',relief='groove');

  DSGUI.start := button (DSGUI.control,'Start computation',background='green');

  DSGUI.save := button (DSGUI.control,'Save settings',background='LemonChiffon');
  
  whenever DSGUI.save -> press do { DynamicSpectrumGUIsave (DSGUI); }

  DSGUI.load := button (DSGUI.control,'Load settings',background='LemonChiffon');
  
  whenever DSGUI.load -> press do {
    DynamicSpectrumGUIload (rec, DSGUI);
  }

  DSGUI.dismiss := button (DSGUI.control,'Dismiss',background='orange');

  whenever DSGUI.dismiss -> press do {
    val DSGUI.frame:=F;
    val DSGUI:=F;
  }

  whenever DSGUI.start -> press do {
    # provide some feedback
    rec.newsagent->message('[DynamicSpectrumGUI] Extracting parameters from GUI ...');
    # get variable values
    recParam := [=];
    recParam.blockinfo      := array(0,3);
    recParam.blockinfo[1]   := as_integer(DSGUI.StartBlock.Entry->get());
    recParam.blockinfo[2]   := as_integer(DSGUI.nofBlocks.Entry->get());
    recParam.blockinfo[3]   := as_integer(DSGUI.nofSamples.Entry->get());
    recParam.index          := eval(DSGUI.antennae.Entry->get());
    recParam.dsMethod       := as_string(DSGUI.dsMethod.Button->text($value));
    recParam.direction      := eval(DSGUI.direction.Entry->get());
    recParam.wcs            := as_string(DSGUI.wcs.Button->text($value));
    recParam.band           := eval(DSGUI.freqBand.Entry->get());
    recParam.calcAverage    := as_boolean(DSGUI.average.Button->state());
    recParam.calcDifferential := as_boolean(DSGUI.differential.Button->state());
    # start conputation of dynamic spectrum
    print '[DynamicSpectrumGUI] Calling computation function ...';
    DynamicSpectrum (rec,
                     blockinfo=recParam.blockinfo,
                     index=recParam.index,
                     dsMethod=recParam.dsMethod,
                     direction=recParam.direction,
                     WCS=recParam.wcs,
                     band=recParam.band,
                     calcDifferential=recParam.calcDifferential,
                     calcAverage=recParam.calcAverage);
  }

}

#-------------------------------------------------------------------------------
#Name: DynamicSpectrumGUIsave
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Store the settings of the graphical interface to disk; the data are
#      written to a file 'DynamicSpectrum.sos'
#Par:  gui - Record where the GUI settings are stored
#Created: 2004/11/12 (Lars Baehren)
#Status: experimental
#Todo: Allow storage to user defined file.
#-------------------------------------------------------------------------------

DynamicSpectrumGUIsave := function (ref gui) {

  filename := "DynamicSpectrum.sos";

  if (!is_record(gui)) {
    print "[DynamicSpectrumGUIsave] Input is no record!";
    fail;
  } else {
    settings := [=];

    settings.file       := gui.file.Entry->get();
    settings.date       := gui.date.Entry->get();

    settings.antennae   := eval(gui.antennae.Entry->get());
    settings.StartBlock := eval(gui.StartBlock.Entry->get());
    settings.nofBlocks  := eval(gui.nofBlocks.Entry->get());
    settings.nofSamples := eval(gui.nofSamples.Entry->get());
    settings.freqBand   := eval(gui.freqBand.Entry->get());
    settings.dsMethod   := as_boolean(gui.dsMethod.Button->text($value));
    settings.direction  := eval(gui.direction.Entry->get());
    settings.wcs        := gui.wcs.Button->text($value);
    settings.band       := eval(gui.freqBand.Entry->get());

    settings.average    := gui.average.Button->state();
    settings.antcc      := gui.antcc.Button->state();

    write_value(settings,filename);
    print "[DynamicSpectrumGUIsave] Saved settings to file",filename;
  }
}

#-------------------------------------------------------------------------------
#Name: DynamicSpectrumGUIload
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Reload the settings for the dynamic spectrum generator from disk.
#Par:  rec - Data record; passed by reference.
#Par:  gui - Record where the GUI settings are stored
#Created: 2004/11/12 (Lars Baehren)
#Status: experimental
#Todo: Allow loading from user defined file (catalog tool).
#-------------------------------------------------------------------------------

DynamicSpectrumGUIload := function (ref rec, ref gui) {

  filename := "DynamicSpectrum.sos";

  if (!is_record(gui)) {
    print "[DynamicSpectrumGUIload] Input is no record!";
    fail;
  } else {
    #-------------------------------------------------------
    # load settings from disk

    print "[DynamicSpectrumGUIsave] Reading settings from file",filename;
    tmp := [=];
    tmp := read_value(filename);

    #-------------------------------------------------------
    # copy the settings to the GUI

    gui.file.Entry->delete('start','end');
    gui.file.Entry->insert(as_string(tmp.file));
#    gui.date.Entry->delete('start','end');
#    gui.date.Entry->insert(as_string(tmp.date));

    gui.antennae.Entry->delete('start','end');
    gui.antennae.Entry->insert(vector2string(tmp.antennae));
    gui.StartBlock.Entry->delete('start','end');
    gui.StartBlock.Entry->insert(as_string(tmp.StartBlock));
    gui.nofBlocks.Entry->delete('start','end');
    gui.nofBlocks.Entry->insert(as_string(tmp.nofBlocks));
    gui.nofSamples.Entry->delete('start','end');
    gui.nofSamples.Entry->insert(as_string(tmp.nofSamples));
    gui.freqBand.Entry->delete('start','end');
    gui.freqBand.Entry->insert(vector2string(tmp.freqBand));
    gui.dsMethod.Entry->delete('start','end');
    gui.dsMethod.Entry->insert(as_string(tmp.dsMethod));

    gui.direction.Entry->delete('start','end');
    gui.direction.Entry->insert(vector2string(tmp.direction));    

#    gui..Entry->delete('start','end');
#    gui..Entry->insert(as_string(tmp.));

    #-------------------------------------------------------
    # Update internal buffers

    rec.setblock(n=as_integer(tmp.StartBlock),
                 blocksize=as_integer(tmp.nofSamples));
    rec.replot();

  }

}

#-------------------------------------------------------------------------------
#Section: <a name="generation">2.</a> Master routines for computing dynamic spectra
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: DynamicSpectra
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Batch processing of data set located within a given directory, 'dirname'.
#      In the first step the specified directory is check for valid datasets,
#      i.e. it is checked if the required LOPES header file exists in the
#      subdirectories; if this is the case, the dataset is added to the list
#      of datasets to process.
#Par:  ref rec - Record where the data is stored (passed as reference)
#Par:  dirname - Directory containing the data files to process; if an empty
#                parameter value if passed the data file currently loaded into 
#                memory is processed.
#Par:  WCS     - Reference code of the world coordinate system, in which the
#                direction of the beam is defined.
#Par:  direction - Angular coordinates, (alpha,delta), of the direction
#                    towards which the beam is formed.
#Par:  band        - Limits of the frequency band
#Par:  refant      - Index of the reference antenna used as array phase center
#Par:  index       - Indicies of the antennae selected for the computation.
#Par:  blockinfo   - Setup of the processing of the datablocks, where 
#                    <ul>
#                    <li>blockinfo[1] is the number of the first block to process,
#                    <li>blockinfo[2] the number of blocks to process and
#                    <li>blockinfo[3] is the blocksize in samples.
#                    </ul>
#Par:  viewImage = T     - Display the created image tool containing the
#                          spectrum.
#Par:  logStatistics = F - Log some basic statistical properties of the spectra
#                          per datablock (min,max,mean,variance)
#Ref:  getFilelist, readprfiles_cbfunc, DynamicSpectrum
#Example: DynamicSpectra (data,index=[1:4,13:16,25:28,37:40,49:52],blockinfo=[128^2])
#Created: 2004/07/30 (Lars Baehren)
#Status: experimental
#-------------------------------------------------------------------------------

DynamicSpectra := function (ref rec,
                            dirname='',
                            WCS='AZEL',
                            Projection='CAR',
                            direction=[0,90],
                            band=[-1,-1],
                            index=-1,
                            refant=1,
                            blockinfo=[2048],
                            viewImage=F,
                            logStatistics=F
                           )
{

  #-------------------------------------------------------------------
  # Check wether to run on multiple data files in a directory or a
  # data file already loaded into memory.

  if (dirname == '') {
    print "[DynamicSpectra] Processing data file already loaded into memory.";
  }
  else {

    #-----------------------------------------------------------------
    # Get the list of valid input files 

    files := getFilelist (dirBase=dirname);
    print "[DynamicSpectra] Found ",len(files),"datasets in",dirname;

    #-----------------------------------------------------------------
    # batch processing of the files

    for (file in files ) {

      #---------------------------------------------------------------
      # Load the dataset into memory

      print "[DynamicSpectra] Loading dataset",file;
      readprfiles_cbfunc (file);

      #---------------------------------------------------------------
      # generate dynamic spectrum for dataset

      DynamicSpectrum (rec,
                       WCS=WCS,
                       Projection=Projection,
                       direction=direction,
                       band=band,
                       index=index,
                       refant=refant,
                       blockinfo=blockinfo,
                       logStatistics=logStatistics,
                       viewImage=viewImage,
                       calcCrossCorr=F
                      );

    }

  }

  print "[DynamicSpectra] Processing completed.";

}

#-------------------------------------------------------------------------------
#Name: DynamicSpectrum
#-------------------------------------------------------------------------------
#Type: Glish Function
#Doc:  Glish part of the routines to generate a dynamic spectrum. This routine
#      serves as interface between the recorded data and the main computation
#      routines, which are implemented in a C++ CLI.<br>
#      As demonstrated by the example below, it is sufficient for the data
#      processing to provide the number of samples per datablock; the remaining
#      control variables then will be adjusted by the function 'VerifyBlockinfo'
#Par:  ref rec - Record where the data is stored (passed as reference)
#Par:  WCS     - Reference code of the world coordinate system, in which the
#                direction of the beam is defined.
#Par:  direction - Angular coordinates, (alpha,delta), of the direction
#                    towards which the beam is formed.
#Par:  band        - Limits of the frequency band
#Par:  index       - Indicies of the antennae selected for the computation; if
#                    'index=-1' the settings made in the PlotGUI are adopted.
#Par:  refant      - Index of the reference antenna used as array phase center
#Par:  blockinfo   - Setup of the processing of the datablocks, where 
#                    <ul>
#                    <li>blockinfo[1] is the number of the first block to process,
#                    <li>blockinfo[2] the number of blocks to process and
#                    <li>blockinfo[3] is the blocksize in samples.
#                    </ul>
#Par:  dsMethod      =   - Computation method; 'beam', 'meanPower'
#Par:  inputData     =   - Of what type are the input data? this can be either
#                          <li><i>rawTime</i>
#                          <li><i>ccCube</i>
#Par:  logStatistics = F - Log some basic statistical properties of the spectra
#                          per datablock (min,max,mean,variance)
#Par:  calcDifferential  - Compute differential spectra, i.e. calculate the
#                          difference between the spectra of two subsequent
#                          processed datablocks, <tt>D[n,k] = S[n,k] - S[n-1,k]</tt>
#                          where <tt>n</tt> is the blocknumber and <tt>k</tt>
#                          the number of the frequency channel.
#Par:  calcAverage   = T - Compute averaged spectra
#Par:  calcAntPow    = T - Display the total power as function of time for each
#                          individual antenna.
#Par:  calcCrossCorr = F - Compute 2-dim cross-correlation function on the
#                          generated dynamic spectrum. (This is still
#                          experimental and therefore disabled by default).
#Par:  calcTPC       = F - Calculate the Two-Point-Correlation function on the
#                          Dynamic spectrum; this parameter should be handled 
#                          with care, since running this process is extremely
#                          compute intensive (it's therefore disabled by
#                          default). For detailed information see the AIPS++
#                          online documentation.
#Par:  viewImage = T     - Display the created image tool containing the
#                          spectrum.
#Ref:  VerifyBlockinfo, FrequenciesInBand, getAntennaPositions,
#      VerifyFrequencyBand, FilenameFromHeader, DynspecCrossCorr, DynspecAverages
#Example: DynamicSpectrum (data,blockinfo=[128^2],index=[4,16,28,40,52])
#Example: DynamicSpectrum (data,blockinfo=[128^2])
#Created: 2004/07/30 (Lars Baehren)
#-------------------------------------------------------------------------------

DynamicSpectrum := function (ref rec,
                             WCS='AZEL',
                             Projection='CAR',
                             direction=[0,90],
                             band=[-1,-1],
                             index=-1,
                             refant=1,
                             blockinfo=[2048],
                             dsMethod='power',
                             inputData='rawTime',
                             logStatistics=F,
                             calcDifferential=T,
                             calcAverage=T,
                             calcAntPow=T,
                             calcCrossCorr=F,
                             calcTPC=F,
                             viewImage=T
                            )
{
  rec.newsagent->message('[DynamicSpectrum] Welcome');

  verbose := T;

  global binCLI := client(spaste(globalconst.LOPESBIND,'/dynamicspectrum'));
  recClient := [=];

  #-------------------------------------------------------------------
  # Set the range of selected antennae

  if (index != -1 ) {
    rec.select(1:rec.len,F);
    rec.select(index,T);
  }
  index  := [1:rec.len][rec.select()];
  nant   := len(index);

  #-------------------------------------------------------------------
  # If enabled, run the auto flagger/checker on the data

  stats := checkData (rec,
                      index=index,
                      clipping=0.01, 
                      autoflag=T);

  index  := [1:rec.len][rec.select()];
  nant   := len(index);

  #-------------------------------------------------------------------
  # Check the range of the frequency band

  band := VerifyFrequencyBand(rec,band)*rec.headf('FrequencyUnit');

  #-------------------------------------------------------------------
  # Go to the starting point in the data volume

  blockinfo := VerifyBlockinfo (rec,blockinfo);

  SetBlock (rec, n=blockinfo[1],blocksize=blockinfo[3]);
  rec.setblock(n=blockinfo[1],blocksize=blockinfo[3],index=index);
  ok := plotgui.replot();
  
  if (ok) {
    print "[DynamicSpectrum::DynamicSpectrum] Error while replotting main GUI!";
  }

  #-------------------------------------------------------------------
  # Set up the record passed to the CLI

  rec.newsagent->message('[DynamicSpectrum] Preparing record passed to CLI ...');

  epoch        := qnt.quantity(rec.head('Date'));
  filenameBase := FilenameFromHeader(rec);

  DataToRecord (rec, recClient, 'ObservationInfo');
  recClient.epoch            := epoch;
  recClient.antennaSet       := index;

  recClient.refcode          := WCS;
  recClient.Projection       := Projection;

  recClient.antennaPositions := getAntennaPositions (rec,refant=refant);
  recClient.direction        := direction;

  DataToRecord (rec,recClient,'Frequency');
  recClient.FrequencyBand   := band;
  recClient.Blocksize       := as_integer(rec.getmeta('Blocksize'));

  recClient.dsMethod     := dsMethod;
  recClient.inputData    := inputData;
  recClient.statistics   := logStatistics;
  recClient.differential := calcDifferential;

  #-------------------------------------------------------------------
  # Call the CLI to initialize the beamformer

  rec.newsagent->message('[DynamicSpectrum] Initializing the Beamformer ...');

  replyCLI := binCLI->initBeamformer (recClient);

  #-------------------------------------------------------------------
  # Time domain information

  crvalTime := mean(rec.get('Time'));
  cdeltTime  := blockinfo[3]/(rec.headf('Samplerate')*rec.headf('SamplerateUnit'));

  #-------------------------------------------------------------------
  # Frequency domain information

  frequencies := rec.get('Frequency');
  passband    := frequencies>=band[1] & frequencies<=band[2];
  crvalFreq   := replyCLI.frequencies[1];
  cdeltFreq   := replyCLI.frequencies[2]-replyCLI.frequencies[1];
  nfreq       := len(replyCLI.frequencies);
  nant        := len(index);  

  # ------------------------------------------------------------------
  # Create a coordinate system tool to be attached to the image tool

  cs  := coordsys (spectral=1,linear=1);

  cs.reorder([2,1]);

  cs.setepoch(dm.epoch('utc',qnt.quantity(rec.head('Date'))));
  cs.settelescope(rec.head('Observatory'));
  cs.setobserver(username());

  # Set up the relation between pixel and world coordinates

  { 
    local crpix;
    local crval;
    local cdelt;
    local units;

    cs.setnames(value="Time Frequency");

    cs.setunits(type='linear', value="s", overwrite=T);
    cs.setunits(type='spectral', value="Hz", overwrite=T);

    crpix := [1,1];
    cs.setreferencepixel(crpix);

    crval := [crvalTime,crvalFreq];
    cs.setreferencevalue(crval);

    cdelt := [cdeltTime,cdeltFreq];
    cs.setincrement(cdelt);
  }

  if (is_coordsys(cs)) {
    cs.summary();
  } else {
    print 'Error creating valid coordinate system tool!';
  }

  #-------------------------------------------------------------------
  # Create logfile and add processing parameters

  logfileName := spaste(FilenameFromHeader(rec),'-dynspec.log')

  logfile := open(spaste('> ',logfileName));

  write (logfile, spaste('[DynamicSpectrum] ',shell("date")));

  write (logfile,' ');

  write (logfile,
         spaste('Filename base string ....................... : ',
                FilenameFromHeader(rec)));

  write (logfile,' ');

  write (logfile,
         spaste('Direction of the beam (AZEL) ............... : ',
                replyCLI.beamDirection));
  write (logfile,
         spaste('Selected antennae .......................... : ',index));
  write (logfile,
         spaste('                                             : ',
                recClient.AntName));
  write (logfile,
         spaste('Reference antenna .......................... : ',refant));
  write (logfile,
         spaste('Selected frequency band ............... [Hz] : ',band));
  write (logfile,
         spaste('Number of frequency channels ............... : ',nfreq));
  write (logfile,
         spaste('Range of frequency array .............. [Hz] : ',
                min(replyCLI.frequencies),
                ' .. ',
                max(replyCLI.frequencies)));
  write (logfile,
         spaste('Frequency resolution .................. [Hz] : ',cdeltFreq));
  write (logfile,
         spaste('Processing blockinfo ....................... : ',blockinfo));
  write (logfile,
         spaste('Computation method of dyn. spectrum ........ : ',dsMethod));
  write (logfile,
         spaste('Compute differential spectra ............... : ',calcDifferential));
  write (logfile,
         spaste('Compute averaged spectra ................... : ',calcAverage));
  write (logfile,
         spaste('Compute cross-correlation of antenna signals : ',calcCrossCorr));
  write (logfile,
         spaste('Compute Two-Point-Correlation function ..... : ',calcTPC));
   
  # ------------------------------------------------------------------
  #  Loop over the data blocks and generate the dynamic spectrum
  # ------------------------------------------------------------------

  spectrum  := array (0,blockinfo[2],nfreq);
  if (calcDifferential) {
    diffSpectrum := array (0,blockinfo[2]-1,nfreq);
  }
  powerAnt  := array (0,blockinfo[2],nant);
  TimeBlockcenter := array(0,blockinfo[2]);

  # ----------------------------------------------------------------
  # Fill the record for passing parameters to the Client

  recDataReader := [=];

  recDataReader.method := dsMethod;

  RecordForDataReader (rec,
                       recDataReader,
                       frequencyBand=band);

  recDataReader.StartBlock := blockinfo[1];
  recDataReader.nofBlocks  := blockinfo[2];

  recDataReader.filenameBase := spaste(filenameBase,'-dynspec');
  recDataReader.epoch        := cs.epoch();
  recDataReader.telescope    := cs.telescope();
  recDataReader.observer     := cs.observer();
  recDataReader.crval        := cs.referencevalue();
  recDataReader.cdelt        := cs.increment();

  # ----------------------------------------------------------------
  # Run the Client to carry out the computation
 
  results := binCLI->computeSpectra (recDataReader);

  recDataReader := F;
  binCLI        := F;

  # ----------------------------------------------------------------
  # Get the computed dynamic spectrum from the FITS file written by
  # the client application.

  filenameFITS := spaste(results.filenameBase,".fits");

  print "Loading image ",filenameFITS,"...";

  img := imagefromfits (infile=filenameFITS);

  if (!is_image(img)) {
    img.done();
    print "No valid image tool created.";
    fail;
  }

  spectrum := img.getchunk();

  if (!is_image(img)) {
    print "[DynamicSpectrum] No valid image tool created from spectrum data!"
  }
  else {
    if (viewImage) {
      vid := dv.loaddata(img,'raster');
      vid.setoptions([axislabelswitch=T,
		      axislabels=T,
		      titletext=spaste('Dynamic Spectrum, ',filenameBase),
		      xaxistext='Time [s]',
		      yaxistext='Frequency [MHz]',
		      xgridtype=['Tick marks'],
		      ygridtype=['Tick marks'],
		      xgridcolor='green',
		      ygridcolor='green',
		      plotoutlinecolor='green',
		      axislabelspectralunit=['MHz'],
		      colormap=['Hot Metal 1']]
		    );
      vdp := dv.newdisplaypanel();
      vdp.register(vid);
    }
  }

  # ----------------------------------------------------------------
  #  Fill the returned data into the array used for construction of
  #  of the image tool.

  bar := progress (0,blockinfo[2], 'Arranging computation results ...')
  bar.activate()

  for (num in 1:blockinfo[2]) {
    powerAnt[num,] := results.antennaPower[,num];
    bar.update(num);
  }

  results := F;

  # revert position in data

  rec.setblock(n=block,blocksize=blockinfo[3]);

  # ------------------------------------------------------------------
  # Add some basic statistics to the logfile

  write (logfile, '');
  write (logfile,'Statistical properties of the generated dynamic spectrum');
  write (logfile,
         spaste(' - Shape of the data array ...... : ',shape(spectrum)));
  write (logfile,
         spaste(' - Coordinate increment .. [s Hz] : ',cs.increment()));
  write (logfile,
         spaste(' - Range of values in spectrum .. : ',
                min(spectrum),' .. ',max(spectrum)));
  write (logfile,
         spaste(' - Mean value of the spectrum ... : ',mean(spectrum)));

  #-------------------------------------------------------------------
  # Present a time-averaged plot of the spectrum

  if (calcAverage && is_image(img)) {
    rec.newsagent->message('[DynamicSpectrum] Computing averaged spectra ...');
    DynspecAverages (rec, recClient, img, viewImage=viewImage);
  }

  # ------------------------------------------------------------------
  # Construct an image tool to display the total power per antenna

  if (calcAntPow) {
    plotPowerPerAntenna (rec,
                         powerAnt,
                         valuesAlongImageAxis(img,axis=1),
                         viewImage=viewImage);
  }

  #-------------------------------------------------------------------
  # Create an image tool for the differential spectrum

  if (calcDifferential) {
    rec.newsagent->message('[DynamicSpectrum] Computing differential spectrum ...');
    # Set the filename for the newly created image
    filename := spaste(filenameBase,'-dynspec-diff.img');
    # Adjust the coordinate system tool
    img := imagefromarray(outfile=filename,pixels=diffSpectrum,csys=cs,overwrite=T);
    
    # Custom display tool for the image

    if (!is_image(img)) {
      print "[DynamicSpectrum] No valid image tool created from spectrum data!"
    }
    else {
      if (viewImage) {
        vid := dv.loaddata(img,'raster');
        vid.setoptions([axislabelswitch=T,
	 	        axislabels=T,
		        titletext=spaste('Dynamic Spectrum (diff), ',filenameBase),
		        xaxistext='Time [s]',
		        yaxistext='Frequency [MHz]',
		        xgridtype=['Tick marks'],
		        ygridtype=['Tick marks'],
		        xgridcolor='green',
		        ygridcolor='green',
		        plotoutlinecolor='green',
		        axislabelspectralunit=['MHz'],
		        colormap=['Hot Metal 1']]
		       );
        vdp := dv.newdisplaypanel();
        vdp.register(vid);
      }
    }
  }

  #-------------------------------------------------------------------
  # 2-dimensional cross-correlation of the dynamic spectrum

  if (calcCrossCorr) {
    rec.newsagent->message('[DynamicSpectrum] Computing 2D cross-correlation ...');
    DynspecCrossCorr (rec,recClient,img);
  }

  #-------------------------------------------------------------------
  # Compute the two-point correlation function on the spectrum

  if (calcTPC) {
    rec.newsagent->message('[DynamicSpectrum] Computing correlation function ...');
    DynspecTPC (rec,img);
  }

  #-------------------------------------------------------------------
  # Clean up

  recSpectrum := F;
  spectrum := F;
  powerAnt := F;
  if (!viewImage) {
    img.done();
  }

  rec.newsagent->message('[DynamicSpectrum] Done.');
}


#-------------------------------------------------------------------------------
#Section: <a name="analysis">3.</a> Analysis of dynamic spectra
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: DynspecCrossCorr
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  [still to be written]
#Par:  recData      - Data record
#Par:  imgDynspec   - AIPS++ image tool created for the dynamic spectrum.
#Par:  viewImage    - Display the created image tool.
#Ref:  FilenameFromHeader
#Created: 2004/08/06 (Lars Baehren)
#Status: experimental
#-------------------------------------------------------------------------------

DynspecCrossCorr := function (ref recData, ref recClient, ref imgDynspec,
                              viewImage=T) {

  binCLI := client(spaste(globalconst.LOPESBIND,'/dynamicspectrum'));

  #---------------------------------------------------------
  # Set up variables for further processing.

  if (!is_image(imgDynspec)) fail;

  filenameBase := FilenameFromHeader(recData);
  filename  := spaste(filenameBase,'-dynspec-cc.img');
  pixels    := imgDynspec.getchunk();
  cs        := imgDynspec.coordsys();

  #---------------------------------------------------------
  # Pass the array with the pixels values to the CLI to compute
  # the cross-correlation.

  recClient.dynspec := pixels;

  cc := binCLI->crosscorrDynspec (recClient);

  #---------------------------------------------------------
  # Create an image tool to display the computation result.

  img_cc := imagefromarray(outfile=filename,pixels=cc.crosscorr,
                           csys=cs,overwrite=T);

  #---------------------------------------------------------
  # Custom display tool for the image.

  if (viewImage) {
    vid := dv.loaddata(img_cc,'raster');
    vid.setoptions([axislabelswitch=T,
                    axislabels=T,
                    titletext=spaste('Cross-Correlation Spectrum, ',filenameBase),
                    xgridtype=['Tick marks'],
                    xgridcolor='white',
                    ygridtype=['Tick marks'],
                    ygridcolor='foreground',
                    axislabelspectralunit=['MHz'],
                    colormap=['Rainbow 2']]
                  );
    vdp := dv.newdisplaypanel();
    vdp.register(vid);
  } else {
    img_cc.done();
  }
}

#-------------------------------------------------------------------------------
#Name: DynspecTPC
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Compute the two-point correlation function on the dynamic spectrum
#Par:  recData      - Data record
#Par:  imgDynspec   - AIPS++ image tool created for the dynamic spectrum.
#Par:  viewImage    - Display the created image tool.
#Ref:  FilenameFromHeader
#Created: 2004/08/10 (Lars Baehren)
#Status: experimental
#-------------------------------------------------------------------------------

DynspecTPC := function (ref recData, ref imgDynspec, viewImage=T) {

  #---------------------------------------------------------
  # Set up variables for further processing.

  if (!is_image(imgDynspec)) fail;

  filenameBase := FilenameFromHeader(recData);
  filename  := spaste(filenameBase,'-dynspec-tpc.img');

  tpc := imgDynspec.twopointcorrelation(outfile=filename,overwrite=T);

  if (viewImage) {
    vid:=dv.loaddata(tpc,'raster');
    vid.setoptions([axislabelswitch=F,
		    axislabels=T,
	            titletext=spaste('Two-Point Correlation, ',filenameBase),
		    axislabelspectralunit=['MHz'],
		    colormap=['Rainbow 2']]
		  );
    vdp := dv.newdisplaypanel();
    vdp.register(vid);
  } else {
    tpc.done();
  }

}

#-------------------------------------------------------------------------------
#Name: DynspecAverages
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Produce plots of (a) total power as function of time and (b) the
#      time-averaged spectrum.
#Par:  recData    - Data record
#Par:  recClient  - Record sent to the CLI.
#Par:  imgDynspec - AIPS++ image tool created for the dynamic spectrum.
#Par:  viewImage  - Display the created image tool.
#Ref:  find_brightest_transmitters,valuesAlongImageAxis
#Created: 2004/08/06 (Lars Baehren)
#-------------------------------------------------------------------------------

DynspecAverages := function (ref recData,
                             ref recClient,
                             ref imgDynspec,
                             viewImage=T,
                             verbose=F)
{
  if (!is_image(imgDynspec)) {
    print '[DynspecAverages] No valid image tool with dynamic spectrum provided!';
    fail;
  }

  #---------------------------------------------------------
  # Set up variables for further processing.

  global binCLI := client(spaste(globalconst.LOPESBIND,'/dynamicspectrum'));

  selection    := [1:recData.len][recData.select()];
  nant         := len(selection);
  freqUnit     := as_double(recData.head('FrequencyUnit'));
  filenameBase := FilenameFromHeader(recData);
  dynspecOrig  := imgDynspec.getchunk();
  cs           := imgDynspec.coordsys();
  imgshape     := imgDynspec.shape();
  axisTime     := valuesAlongImageAxis(imgDynspec,axis=1);
  axisFreq     := valuesAlongImageAxis(imgDynspec,axis=2);

  #-------------------------------------------------------------------
  # Provide some feedback on the control variables

  if (verbose) {
    print "[DynspecAverages]";
    print " - Antenna selection .... : ",selection;
    print " - Number of antennae ... : ",nant;
    print " - Frequency unit ....... : ",freqUnit;
    print " - Filename base string . : ",filenameBase;
  }

  #-------------------------------------------------------------------
  # [1] Compute the time-averaged spectral distribution from the
  #     dynamic spectrum.

  print "[DynamicSpectrum::DynspecAverages] Get averaged spectrum ...";

  timeav := CollapseAxis (dynspecOrig,
                          plotdata=F,
                          axis=1,
                          xaxis=axisFreq);

  freqRFI := find_brightest_transmitters(recData,
                                         index=as_integer(selection[1]),
                                         freqlow=(min(axisFreq)/freqUnit),
                                         freqhigh=(max(axisFreq)/freqUnit));

  pg := pgplotter();
  pg.env(min(axisFreq),max(axisFreq),min(log(timeav)),max(log(timeav)), 0, 0);
  pg.lab('Frequency [Hz]','Power',spaste('Time-averaged spectrum, ',filenameBase));
  pg.setcolor(2);
  pg.line(axisFreq,log(timeav));
  if (len(freqRFI) > 0) {
    ypos := 0.75*max(log(timeav));
    pg.setcolor(8);
    for (n in 1:len(freqRFI)) {
      pg.ptxt(as_float(freqRFI[n]),ypos,90,0,spaste("[",freqRFI[n]/freqUnit,"]"));
    }
  }
  pg.plotfile(spaste(filenameBase,"-dynspec-timeav.pg"));

  #-------------------------------------------------------------------
  # [2] Normalize the dynamic spectrum by the time-averaged spectral
  #     distribution.

  dynspecNorm := array(0,imgshape[1],imgshape[2]);
  for (f in 1:imgshape[2]) {
    dynspecNorm[,f] := dynspecOrig[,f]/timeav[f];
  }

  filename  := spaste(filenameBase,'-dynspec-norm.img');

  imgNorm := imagefromarray(outfile=filename,pixels=dynspecNorm,
			    csys=cs,overwrite=T);

  #-------------------------------------------------------------------
  # [3] Total power as function of time; compute for both raw and
  #     normalized dynamic spectrum. Create plot containing both 
  #     curves for better comparison.

  print "[DynamicSpectrum::DynspecAverages] Get power vs. time ...";

  powervstimeOrig := CollapseAxis (dynspecOrig,plotdata=F,axis=2,
                                   xaxis=axisTime);

  powervstimeNorm := CollapseAxis (dynspecNorm,plotdata=F,axis=2,
                                   xaxis=axisTime);

  pg2 := pgplotter(size=[700,750]);
  pg2.subp(1,2);
  # raw data
  pg2.setcolor(1);
  pg2.env(min(axisTime),max(axisTime),min(powervstimeOrig),max(powervstimeOrig),0,0);
  pg2.lab('Time [s]','Power',spaste('Total power vs. time, ',filenameBase));
  pg2.setcolor(2);
  pg2.line(axisTime,powervstimeOrig);
  # normalized data
  pg2.setcolor(1);
  pg2.env(min(axisTime),max(axisTime),min(powervstimeNorm),max(powervstimeNorm),0,0);
  pg2.lab('Time [s]','Power',spaste('Normalized total power vs. time, ',filenameBase));
  pg2.setcolor(3);
  pg2.line(axisTime,powervstimeNorm);
  # store plot on disk
  pg2.plotfile(spaste(filenameBase,"-dynspec-power.pg"));

  #-------------------------------------------------------------------
  # [3.b] FFT of the total power as function of time.

  ffts:=fftserver();
  powervstimeFFT := ffts.realtocomplexfft(powervstimeOrig);
  lenFFT := len(powervstimeFFT);

  pgTotalPowerFFT := pgplotter();
  pgTotalPowerFFT.setcolor(1);
  pgTotalPowerFFT.env(1,lenFFT,min(real(powervstimeFFT)),max(real(powervstimeFFT)),0,0);
  pgTotalPowerFFT.lab('Eigenfrequency',
                      'Power',
                      spaste('Eigenfrequencies of total power, ',filenameBase));
  pgTotalPowerFFT.setcolor(2);
  pgTotalPowerFFT.line([1:lenFFT],real(powervstimeFFT));

  ffts.done();

  #-------------------------------------------------------------------
  # [4] 'Center of mass' equivalent in frequency space; gain from both
  #     raw and normalized dynamic spectrum.

  recClient.DynSpec     := dynspecOrig;
  cms := binCLI->cmsFrequency(recClient);
  cmsFreqOrig := cms.cmsFreq;

  recClient.DynSpec     := dynspecNorm;
  cms := binCLI->cmsFrequency(recClient);
  cmsFreqNorm := cms.cmsFreq;

  minval := min([min(cmsFreqOrig),min(cmsFreqNorm)])
  maxval := max([max(cmsFreqOrig),max(cmsFreqNorm)])

  pg3 := pgplotter();
  pg3.env(min(axisTime),max(axisTime),minval,maxval,0,0);
  pg3.lab('Time [s]','Frequency [Hz]',spaste('Weighted center frequency, ',filenameBase));
  # sub-plot 1
  pg3.setcolor(2);
  pg3.line(axisTime,cmsFreqOrig);
  pg3.ptxt(LabelPositionPGPlot(axisTime,0.05),
           LabelPositionPGPlot([minval,maxval],0.95),
           0,0,"Raw dynamic spectrum");
  # sub-plot 2
  pg3.setcolor(3);
  pg3.line(axisTime,cmsFreqNorm);
  pg3.ptxt(LabelPositionPGPlot(axisTime,0.05),
           LabelPositionPGPlot([minval,maxval],0.90),
           0,0,"Normalized dynamic spectrum");
  # Store the plot to disk
  pg3.plotfile(spaste(filenameBase,"-dynspec-centerfreq.pg"));

  #-------------------------------------------------------------------
  # [5] After all computations have been: close opened plot-displays,
  #     if no displayed is desired. Otherwise create custom display
  #     tool for the normalized dynamic spectrum.

  if (viewImage) {
    vid:=dv.loaddata(imgNorm,'raster');
    vid.setoptions([axislabelswitch=T,
		    axislabels=T,
		    titletext=spaste('Dynamic Spectrum (norm), ',filenameBase),
		    xaxistext='Time [s]',
		    yaxistext='Frequency [MHz]',
		    xgridtype=['Tick marks'],
		    ygridtype=['Tick marks'],
		    xgridcolor='green',
		    ygridcolor='green',
		    plotoutlinecolor='green',
		    axislabelspectralunit=['MHz'],
		    colormap=['Hot Metal 1']]
		  );
    vdp := dv.newdisplaypanel();
    vdp.register(vid);
  } else {
    imgNorm.done();
    pg.done();
    pg2.done();
    pg3.done();
  }
}


#-------------------------------------------------------------------------------
#Name: CollapseAxis
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Compute a time-average of the dynamic spectrum
#Par:  ref dynspec       - Array with the dynamic spectrum; passed via reference.
#Par:  plotdata     = T  - Plot the averaged spectrum.
#Par:  plottitle         - Title text used for the plot.
#Par:  filename          - Disk filename
#Par:  axis         = 1  - Index of the axis along which to collapse.
#Par:  xaxis        = -1 - Array with values along the x-axis of the plot; if
#                          xaxis=-1, a simple linear axis of integer numbers is
#                          generated.
#Created: 2004/08/05 (Lars Baehren)
#-------------------------------------------------------------------------------

CollapseAxis := function (ref dynspec,
                          plotdata=T,
                          plottitle='',
                          filename='pgplot.pg',
                          axis=1,
                          xaxis=[-1])
{
  #-------------------------------------------------------------------
  # Allocate memory for the averaged spectrum

  specshape := shape(dynspec);

  #-------------------------------------------------------------------

  if (axis == 1) {
    nofx := specshape[2];
  }
  else if (axis == 2) {
    nofx := specshape[1];
  }
  else fail;

  meanspec := array (0,nofx);
  xary := array (0,nofx);

  #-------------------------------------------------------------------
  # Set up the values along the x-axis

  if (len(xaxis) == 1 && xaxis[1] == -1) {
    print "[CollapseAxis] Constructing linear axis";
    xary := [1:nofx];
  }
  else {
    print "[CollapseAxis] Constructing tabular axis from input array";
    xary := xaxis;
  }

  #-------------------------------------------------------------------
  # Average over the specified array axis.
  
  if (axis == 1) {
    for (i in 1:nofx) {
      meanspec[i] := mean(dynspec[,i]);
    }
  }
  else if (axis == 2) {
    for (i in 1:nofx) {
      meanspec[i] := mean(dynspec[i,]);
    }
  }

  if (plotdata) {
    pg := pgplotter();
    pg.env(min(xary),max(xary),min(meanspec),max(meanspec), 0, 0);
    pg.lab('x-axis','y-axis',plottitle);
    pg.setcolor(2);
    pg.line(xary,meanspec);
    pg.plotfile(filename);
  }

  return meanspec;

}

#-------------------------------------------------------------------------------
#Name: SubstractAveragedSpectrum
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Compute a time averaged spectrum from a dynamic spectrum.
#Par:  ref dynspec - Array or image tool containing the dynamic spectrum; if the
#      latter is the case, the pixel array is extracted in the first step.
#Ret:  Array with the dynamic spectrum after substraction of the time averaged 
#      spectrum
#Created: 2004/08/02 (Lars Baehren)
#-------------------------------------------------------------------------------

SubstractAveragedSpectrum := function (ref dynspec) {

  if (is_image(dynspec)) {
    pixels := dynspec.getchunk();
  } else {
    pixels := dynspec;
  }

  meanSpectrum := CollapseAxis (pixels,plotdata=T,
                                plottitle='Time averaged spectrum');

  specshape := shape(pixels);
  pixelsSubstracted := array(0,specshape[1],specshape[2]);

  for (freq in 1:specshape[2]) {
    pixelsSubstracted[,freq] := pixels[,freq] - meanSpectrum[freq];
  }

  return pixelsSubstracted;

}

#-------------------------------------------------------------------------------
#Section: <a name="events">4.</a> Detection of events
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: analyseDynamicSpectrum
#-------------------------------------------------------------------------------
#Type: Glish function 
#Example: analyseDynamicSpectrum (filename='2004-08-11-07:17:33.23-dynspec-norm.img')
#-------------------------------------------------------------------------------

analyseDynamicSpectrum := function (filename='')
{
  include "mathematics.g"

  if (filename == '') {
    print '[analyseDynamicSpectrum] No filename provided.';
    fail;
  }

  img := image(infile=filename);

  if (!is_image(img)) {
    print '[analyseDynamicSpectrum] Input is no valid image.';
    fail;
  } else {
    img.statistics();
  }

  imageCoordsys := img.coordsys();
  imagePixels   := img.getchunk();
  imageShape    := img.shape();
  imageMean     := mean(imagePixels);
  imageMeadian  := median(imagePixels);

  print '[analyseDynamicSpectrum]';
  print ' - Input image  :',filename;
  print ' - Image shape  :',imageShape;   
  print ' - Mean value   :',imageMean;
  print ' - Median value :',imageMeadian;

  img.done();

  # --------------------------------------------------------
  # Create new image
  
  threshold := 2*imageMedian;

  print 'Scanning image pixels ...';

  for (nAxis1 in 1:imageShape[1]) {
    for (nAxis2 in 1:imageShape[2]) {
      if (imagePixels[nAxis1,nAxis2] < threshold) {
        imagePixels[nAxis1,nAxis2] := 0.0;
      }
    }
  }

  print 'Creating new image ...';

  img := imagefromarray(outfile='calc.img',
                        pixels=imagePixels,
                        csys=imageCoordsys,
                        overwrite=T)
  img.view();
  img.statistics();
}

#-------------------------------------------------------------------------------
#Name: SearchEventInPower
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  -/-
#Par:  rec  - Data record
#Par:  blockinfo   - Setup of the processing of the datablocks, where 
#                    <ul>
#                    <li>blockinfo[1] is the number of the first block to process,
#                    <li>blockinfo[2] the number of blocks to process and
#                    <li>blockinfo[3] is the blocksize in samples.
#                    </ul>
#Ref:  VerifyBlockinfo,plotPowerPerAntenna
#Example: SearchEventInPower (data,blockinfo=[4096])
#Created: 2004/08/11 (Lars Baehren)
#-------------------------------------------------------------------------------

SearchEventInPower := function (ref rec,
                                blockinfo=[128^2])
{

  #-------------------------------------------------------------------
  # Go to the starting point in the data volume

  blockinfo := VerifyBlockinfo (rec,blockinfo);

  rec.setblock(n=blockinfo[1],blocksize=blockinfo[3]);
  plotgui.replot();

  #-------------------------------------------------------------------
  # Set up the array for internal data storage

  index     := [1:rec.len][rec.select()];
  nant      := len(index);

  power  := array (0,blockinfo[2],nant);

  #-------------------------------------------------------------------

  bar := progress (0,blockinfo[2], 'Getting power as function of time ...',)
  bar.activate()   

  for (num in 1:blockinfo[2]) {

    # get the time offset from the trigger time
    TimeBlockcenter[num] := mean(rec.get('Time'));

    #-----------------------------------------------------------------
    # Extract the data from the record; afterwards advance the pointer
    # to the next datablock;

    ant := 1;
    for (i in index) {
      power[num,ant] := sum(rec.get('Power',i));
      ant +:=1;
    }

    rec.nextblock();
    bar.update(num);
  }  
  bar.done();

  #-------------------------------------------------------------------
  # Construct an image tool to display the data

  plotPowerPerAntenna (rec, power, TimeBlockcenter);

}

#-------------------------------------------------------------------------------
#Name: plotPowerPerAntenna
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Construct an image tool to display the total power per antenna.
#Par:  rec            - The data record.
#Par:  pixelvalues    - The array with the image pixel values.
#Par:  axisTimeWorld  - World coordinates for the tabular time axis.
#Par:  viewImage  = T - Custom display of the created image tool.
#Par:  plotcurves = F - Make 2-dim plots for each individual antenna.
#Created: 2004/08/11 (Lars Baehren)
#-------------------------------------------------------------------------------

plotPowerPerAntenna := function (ref rec,
                                 ref pixelvalues,
                                 axisTimeWorld,
                                 viewImage=T,
                                 plotcurves=F)
{
  nofTimesteps  := len(axisTimeWorld);
  axisTimePixel := 1:nofTimesteps;
  nant          := shape(pixelvalues)[2];
  index         := [1:rec.len][rec.select()];
  nant          := len(index);

  print "[DynamicSpectrum::plotPowerPerAntenna]";
  print " - Number of antennae ... :",nant;
  print " - Number of time steps . :",nofTimesteps;
  print " - Range of time steps .. :",min(axisTimeWorld),"..",max(axisTimeWorld);
  print " - Shape of pixel array . :",shape(pixelvalues);
  print " - Plot individual curves :",plotcurves;
  print " - Range of values (orig) :",min(pixelvalues),"..",max(pixelvalues);

  #-------------------------------------------------------------------
  # Set up the coordinate system tool for the image tool

  cs := coordsys (tabular=1,linear=1);

  cs.setepoch(dm.epoch('utc',qnt.quantity(rec.head('Date'))));
  cs.settelescope(rec.head('Observatory'));

  # Set up the time axis
  
  cs.settabular(pixel=axisTimePixel, world=axisTimeWorld);
  cs.setunits('s','tabular',overwrite=T);

  # Set up the axis of the antenna numbers

  cs.setreferencepixel(1,'linear');
  cs.setreferencevalue(1,'linear');
  cs.setincrement(1,'linear');

  # Make sure that time is along the x-axis

  cs.reorder([2,1]);

  #-------------------------------------------------------------------
  # Normalized antenna power.

  powerNorm := pixelvalues;

  for (ant in 1:nant) {
    powerNorm[,ant] := pixelvalues[,ant]/(sum(pixelvalues[,ant])/nofTimesteps);
  }

  print "- Range of values (norm) :",min(powerNorm),"..",max(powerNorm);

  #-------------------------------------------------------------------
  # Create an image tool and display it

  filenameBase    := FilenameFromHeader(rec);
  filenameOrig := spaste(filenameBase,'-dynspec-antpow.img');
  filenameNorm := spaste(filenameBase,'-dynspec-antpowNorm.img');

  antpowerOrig := imagefromarray(outfile=filenameOrig,
                                 pixels=pixelvalues,
                                 csys=cs,
                                 overwrite=T);

  antpowerNorm := imagefromarray(outfile=filenameNorm,
                                 pixels=powerNorm,
                                 csys=cs,
                                 overwrite=T);

  if (viewImage) {
    vdp := dv.newdisplaypanel(nx=1,ny=2);
    mdd1:=dv.loaddata(antpowerOrig,'raster');
    mdd2:=dv.loaddata(antpowerNorm,'raster');
    dv.hold();
    mdd1.setoptions([axislabelswitch=T,
		    axislabels=T,
		    titletext=spaste('Power per antenna, ',filenameBase),
                    nxpanels=1,
                    nypanels=2,
		    colormap=['Rainbow 2']]
		   );
    mdd2.setoptions([axislabelswitch=T,
		    axislabels=T,
		    titletext=spaste('Normalized power per antenna, ',filenameBase),
                    nxpanels=1,
                    nypanels=2,
		    colormap=['Rainbow 2']]
		   );
    vdp.register(mdd1);  
    vdp.register(mdd2);  
    dv.release();
  } else {
    antpowerOrig.done();
    antpowerNorm.done();
  }

  #-------------------------------------------------------------------
  # Plot the power for each individual antenna
    
  if (plotcurves) {
    labelXpos  := max(timeaxis)+0.01*(max(timeaxis)-min(timeaxis));
    labelYstep := 0.8*(max(pixelvalues)-min(pixelvalues))/(nant-1);
    pg := pgplotter();
    pg.env(min(timeaxis),max(timeaxis),min(pixelvalues),max(pixelvalues), 0, 0);
    pg.setplottitle(spaste('Power per antenna, ',filenameBase));
    for (ant in 1:nant) {
      labelYpos := 0.99*max(pixelvalues)-(ant-1)*labelYstep;
      pg.setcolor(ant);
      pg.ptxt(labelXpos,labelYpos,0,0,spaste('[',index[ant],']'));
      pg.line(timeaxis,pixelvalues[,ant]);
    }
    pg.plotfile(spaste(filenameBase,"-dynspec-antpow.pg"));
    if (!viewImage) {
      pg.done();
    }
  }

}


#-------------------------------------------------------------------------------
#Section: <a name="helpers">5.</a> Helper functions
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: VerifyBlockinfo
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Check the 'blockinfo' variable passed to 'DynamicSpectrum'. In the
#      simplest case the user will only need to provide the Blocksize, i.e. the
#      number of samples per datablock; the settings then will be adjusted such, 
#      that the complete contents of the loaded datafile is processed.
#Par:  recData     - Record where the data is stored (passed as reference)
#Par:  Blockinfo   - Setup of the processing of the datablocks, where 
#                    Blockinfo[1] is the number of the first block to process,
#                    Blockinfo[2] the number of blocks to process and
#                    Blockinfo[3] is the blocksize in samples.
#                    As this routine is intended to actually verify the contents
#                    of this variable, it is sufficient that the 'Blockinfo'
#                    has a single field only, which hold the blocksize in
#                    samples -- the full set of parameters then will be set up
#                    internally.
#Example: VerifyBlockinfo (data,Blockinfo=[10,2048])
#-------------------------------------------------------------------------------

VerifyBlockinfo := function (ref recData,Blockinfo=[2048]) {

  blockinfo  := array(0,3);
  nelem      := len(Blockinfo);
  Blocksize  := Blockinfo[nelem];
  Filesize   := recData.getmeta('Filesize');

  if (nelem == 1) {
    blockinfo[1] := 1;
    blockinfo[2] := as_integer(Filesize/Blocksize);
    blockinfo[3] := Blocksize;
  }
  else if (nelem == 2) {
    blockinfo[1] := Blockinfo[1];
    blockinfo[2] := as_integer(Filesize/Blocksize)-Blockinfo[1]+1;
    blockinfo[3] := Blocksize;    
  }
  else if (nelem == 3) {
    blockinfo := Blockinfo;
  } 
  else {
    Blocksize := sqrt(recData.getmeta('Blocksize'));
    blockinfo := VerifyBlockinfo (recData,Blockinfo=[Blocksize]);
  }

  # Check against going over the upper end of the dataset

  maxData := (blockinfo[1]+blockinfo[2]-1)*blockinfo[3];
  
  while (maxData > Filesize) {
    print "[VerifyBlockinfo] Adjusting blockinfo[2]";
    blockinfo[2] -:= 1;
    maxData := (blockinfo[1]+blockinfo[2]-1)*blockinfo[3];
  }

  print "[VerifyBlockinfo] Adjusted \'blockinfo\' to ",blockinfo;

  return blockinfo;

}


#-------------------------------------------------------------------------------
#Name: getFilelist
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  
#Par:  dirBase = $LOPESDATADIR - Path to the directory which to scan for files
#Ret:  filenames - List of files, which seem to be proper Project/ITS files
#-------------------------------------------------------------------------------

getFilelist := function (dirBase) {

  dirNames :=  shell(spaste('ls ',dirBase));

  num := 1;
  for (dir in dirNames) {

    dir := spaste (dirBase,'/',dir);
    metafile := concat_dir (dir,'lopeshdr.hdr');

    if (fexist(metafile)) {
      filelist[num] := as_string(dir);
      num +:= 1;
    }

  }

  return filelist;

}


#-------------------------------------------------------------------------------
#Name: PeakFrequencies
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Search for peaks within a power spectrum.
#Par:  rec      - The data record.
#Par:  valFreq  - Array with the frequency values
#Par:  valPower - Array with the power at each provided frequency.
#Created: 2004/08/15 (Lars Baehren)
#Status: experimental
#-------------------------------------------------------------------------------

PeakFrequencies := function(ref rec,valFreq,valPower,
                            nbands=10,threshold=100) {

  band := [min(valFreq),max(valFreq)]/rec.headf("FrequencyUnit");

  bands := band[1]*rec.headf("FrequencyUnit")*10^(log(band[2]/band[1])/nbands*seq(0,nbands));

  print "Frequency band .... [MHz] :",band;
  print "Frequency sub-bands [MHz] :",bands;

  n10:=1;
  n20:=len(valFreq);
  mn:=median(valPower[as_integer(n10+(n20-n10)*0.5):n20]);
  print 'Cutoff Level =',threshold*mn,threshold

  peaks:=array(-1,nbands);
  nf:=0;
  for (i in seq(nbands)) {  
    f1:=bands[i];  
    f2:=bands[i+1];
    n1:=ArrayIndexFromValue(valFreq,f1);
    n2:=ArrayIndexFromValue(valFreq,f2);
    mx:=max_with_location(valPower[n1:n2],pkn);
    pkn:=n1+pkn-1;
    if (mx>threshold*mn) {
      nf+:=1;
      peaks[nf]:=valFreq[pkn];
      print "Subband",f1,"...",f2,"median=",mn,"peak=",peaks[nf];
      }
    }
  print "Peaks           [MHz]:",roundn(peaks/1e6,4)
  return peaks[1:nf]
}


#-------------------------------------------------------------------------------
#Name: ArrayIndexFromValue
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Given an array of values, find the array index 'n' at which the value
#      'y' is stored.
#Par:  ary - The array of data values.
#Par:  y   - A value stored in the array.
#Ret:  n   - The array index 'n' at which the value 'y' is stored; if 'y' could
#            not be found in the array, 'n=-1'.
#-------------------------------------------------------------------------------

ArrayIndexFromValue := function (ref ary, y) {

  nelem := len(ary);
  mindx := nelem;
  mindy := max(ary)-min(ary);

  for (n in 1:nelem) {
    dy := abs(ary[n] - y);
    if (dy < mindy) {
      mindy := dy;
      mindx := n;
    }
  }

  return mindx;

}


#-------------------------------------------------------------------------------
#Name: LabelPositionPGPlot
#-------------------------------------------------------------------------------
#Type: Glish function
#Par:  ary - Array withh the data values along a certain axis.
#Par:  x   - Fractional position along the axis (where e.g. a label is to be
#            placed.
#Ret:  Absolute position along the axis
#-------------------------------------------------------------------------------

LabelPositionPGPlot := function (ary,x) {

  position := x*max(ary)+(1-x)*min(ary);

  return position;

}

#-------------------------------------------------------------------------------
#Name: paramExtraction
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Test transport of structures across the GlishBus, such as Quanta 
#Par:  rec      - The data record.
#-------------------------------------------------------------------------------

paramExtraction := function (ref rec)
{
  record := [=];

  record.time    := qnt.quantity (10,"h");
  record.speed   := qnt.quantity (100,"km/s");
  record.date    := dm.epoch('utc',qnt.quantity(rec.head('Date')));

  print 'input  = ',record;

  dsClient := client(spaste(globalconst.LOPESBIND,'/dynamicspectrum'));
  result   := dsClient->paramExtraction (record);

  print 'result = ',result;
}