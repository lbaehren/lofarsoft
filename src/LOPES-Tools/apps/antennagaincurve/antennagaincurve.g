#-------------------------------------------------------------------------------
# $Id: antennagaincurve.g,v 1.15 2006/07/05 14:08:59 bahren Exp $
#-------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Section: [1] Graphical user interface to the client program
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: antennagaincurve_GUI
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Graphical interface for antenna gain curve extraction client.
#Ref:  antennagaincurve, vector2string
#-------------------------------------------------------------------------------

antennagaincurve_GUI := function (ref rec)
{
  tk_hold();

  guirec := [=];

  guirec.frame := frame(title='Extraction of antenna gain-curves',
                        width=800,height=350,tpos='c');

  # ------------------------------------------------------------------
  # Information on the loaded data set

  guirec.data := SectionFrame (guirec.frame,
                               'Information on the loaded data set',
                               T);

  guirec.project := SectionEntry (guirec.data,
                                  'Project : ',
                                  rec.head('Project'));

  guirec.observatory := SectionEntry (guirec.data,
                                      'Observatory : ',
                                      rec.head('Observatory'));

  guirec.date := SectionEntry (guirec.data,
                               'Observation date : ',
                               rec.head('Date'));

  guirec.antennas := SectionEntry (guirec.data,
                                    'Set of antennas : ',
                                    vector2string(seq(rec.len)[rec.select()]));

  # ------------------------------------------------------------------
  # Setup of the processing parameters

  guirec.processingTime := SectionFrame (guirec.frame,
                                         'Processing parameters: time domain',
                                         T);

  guirec.blocksize := SectionEntry (guirec.processingTime,
                                    '# Samples / Block : ',
                                    rec.getmeta("Blocksize"));

  whenever guirec.blocksize.Entry->return do {
    ### extract variable values from GUI
    blocksize := eval(guirec.blocksize.Entry->get());
    ### Update the plotgui
    rec.setblock(blocksize=blocksize);
    rec.replot();
    ### update entry field
    guirec.blocksize.Entry->delete('start','end');
    guirec.blocksize.Entry->insert(as_string(blocksize));
  }

  guirec.nofBlocks := SectionEntry (guirec.processingTime,
                                    '# of scanned blocks : ',
                                    '1');

  guirec.scanMethod := SectionButton (guirec.processingTime,
                                      ' Scanning method : ',
                                      'menu',
                                      'median')

  recMethod    := [=];
  for (i in "median mean minimum") {
    recMethod[i] := button (guirec.scanMethod.Button,i,value=i);
    whenever recMethod[i]->press do {
      guirec.scanMethod.Button->text($value);
    }
  }

  # ------------------------------------------------------------------
  # Setup of the processing parameters

  guirec.processingFreq := SectionFrame (guirec.frame,
                                         'Processing parameters: frequency domain',
                                         T);

  guirec.nofSubBands := SectionEntry (guirec.processingFreq,
                                    '# frequency sub-bands : ',
                                    '128');

  guirec.averageSpectra := SectionButton (guirec.processingFreq,
                                          'Average the baselines?',
                                          'check',
                                          '');

  whenever guirec.averageSpectra.Button->press do {
    averageSpectra := as_boolean(guirec.averageSpectra.Button->state());
    if (averageSpectra) {
      guirec.nofGroups.Frame->enable();
      guirec.nofGroups.Entry->background('LemonChiffon');
    } else {
      guirec.nofGroups.Entry->background('grey70');
      guirec.nofGroups.Frame->disable();
    }
  }

  guirec.nofGroups := SectionEntry (guirec.processingFreq,
                                    '# of groups to average : ',
                                    '1');
  guirec.nofGroups.Entry->background('grey70');
  guirec.nofGroups.Frame->disable();

  # ------------------------------------------------------------------
  # Post-Processing

  guirec.postProcessing := SectionFrame (guirec.frame,
                                         'Post-Processing tasks',
                                         T);

  guirec.createImage := SectionButton (guirec.postProcessing,
                                       'Image creation',
                                       'check',
                                       'Image from gain curves?');
  guirec.createImage.Button->state(T);

  # ------------------------------------------------------------------
  # Control bottons for the GUI

  guirec.control := frame(guirec.frame,
                          side='left',
                          padx='5',
                          pady='1',
                          expand='x',
                          relief='groove');

  guirec.start := button (guirec.control,
                          'Extract gain-curves',
                          background='green');

  guirec.control.space := frame(guirec.control);

  guirec.dismiss := button (guirec.control,'Done',background='red');

  ## Event handling: start button ------------------------------------

  whenever guirec.start -> press do {

    recParam := [=];
    recParam.antennas   := eval(guirec.antennas.Entry->get());
    recParam.blocksize  := as_integer(eval(guirec.blocksize.Entry->get()));
    recParam.nofBlocks  := as_integer(eval(guirec.nofBlocks.Entry->get()));
    recParam.scanMethod := as_string(guirec.scanMethod.Button->text($value));
    recParam.nofSubBands   := as_integer(eval(guirec.nofSubBands.Entry->get()));
    recParam.averageSpectra := as_boolean(guirec.averageSpectra.Button->state());
    recParam.nofGroups   := as_integer(eval(guirec.nofGroups.Entry->get()));
    recParam.createImage := as_boolean(guirec.createImage.Button->state());

    antennagaincurve (rec,
                      index=recParam.antennas,
                      blocksize=recParam.blocksize,
                      nofBlocks=recParam.nofBlocks,
                      scanMethod=recParam.scanMethod,
                      nofSubBands=recParam.nofSubBands,
                      averageSpectra=recParam.averageSpectra,
                      nofGroups=recParam.nofGroups,
                      createImage=recParam.createImage
                     );

  }

  ## Event handling: dismiss button ----------------------------------

  whenever guirec.dismiss -> press do {
    val guirec.frame:=F;
    val guirec:=F;
  }

  tk_release();

}


#-------------------------------------------------------------------------------
#Section: [2] Glish interface to client program
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: antennagaincurve
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Extract the antenna gain curve from a data set
#Par:  rec             - Reference to the data record.
#Par:  index     = -1  - Indicies of the antennae selected for the computation.
#Par:  nofSubBands  = 1   - Number of sub-bands, into which the spectrum is
#                        divided. If set to a number larger the FFT length, it
#                        is adjusted to that number.
#Par:  nofBlocks = 1   - Number of blocks to analyze
#Par:  apply     = F   - Automatically apply the flags to the processed data?
#Par:  createImage     - Create an AIPS++ image tool from the extracted antenna
#                        gain curves and display it?
#Example: antennagaincurve (data, nofSubBands=100)
#Created: 2005/07/26 (Lars B&auml;hren)
#-------------------------------------------------------------------------------

antennagaincurve := function (ref rec,
                              index=-1,
                              blocksize=128^2,
                              nofBlocks=1,
                              scanMethod='median',
                              nofSubBands=1,
                              averageSpectra=F,
                              nofGroups=1,
                              apply=F,
                              createImage=T
                             )
{
  myClient := client(spaste(globalconst.LOPESBIND,'/antennagaincurve'));

  # ------------------------------------------------------------------
  # Process function parameters

  if (index != -1 ) {
    rec.select(1:rec.len,F);
    rec.select(index,T);
  } else {
    rec.select(1:rec.len,T);
  }

  # Check if the number of frequency sub-bands exceeds the number of 
  # frequency bins; if this is the case, we adjust to the latter number.

  if (nofSubBands > rec.getmeta('FFTlen')) {
    print '[antennagaincurve] Warning: adjusting number of sub-bands.';
    nofSubBands := rec.getmeta('FFTlen')/2;
  }

  # ------------------------------------------------------------------
  # Extract run-time parameters and add them to the record passed to
  # the client.

  par := [=];

  par.index       := [1:rec.len][rec.select()];
  par.nant        := len(par.index);
  par.offset      := rec.getmeta('Offset');
  par.filesize    := rec.getmeta('Filesize');
  par.frequencies := rec.get('Frequency');

  par.blocksize      := blocksize;
  par.nofBlocks      := nofBlocks;
  par.nofSubBands    := nofSubBands;
  par.scanMethod     := scanMethod;
  par.averageSpectra := averageSpectra;
  par.nofGroups      := nofGroups;

  FilesToRecord (rec,par);
  RecordForDataReader (rec,par);

  # ------------------------------------------------------------------
  # Run the Client

  ok := myClient->init (par);

  if (ok) {
    print "[antennagaincurve] Client successfully initialized.";
  } else {
    print "[antennagaincurve] Error initializing Client.";
    fail;
  }

  result := myClient->extractGaincurves (par);

  # ------------------------------------------------------------------
  # Image creation from the data

  if (createImage) {
    # --- Get observation information ---
    ObservationInfoToRecord (rec,result);
    # --- create coordiate system tool ---
    cs := coordsys(spectral=T,tabular=T);
    cs.setnames(value="Frequency Antenna");
    # Reference frame
    cs.setepoch(dm.epoch('utc',qnt.quantity(rec.head('Date'))));
    cs.settelescope(result.Observatory);
    # --- Axis 1: frequency ---
    cs.setunits(type='spectral',
                value='Hz',  
                overwrite=T);
    cs.setreferencepixel (type='spectral', value=1);
    cs.setreferencevalue (type='spectral', value=spaste(result.frequencies[1],'Hz'));
    cs.setincrement (type='spectral',
                     value=spaste(result.frequencies[2]-result.frequencies[1],'Hz'));
    # --- Axis 2: Antenna number 
    cs.settabular (pixel=[1:len(result.AntName)], world=result.AntName);
    #
    global imageTool;
    filename := spaste(FilenameFromHeader(rec),'-gaincurves.img');
    imageTool := imagefromarray (outfile=filename,
                                 pixels=result.gaincurves,
                                 csys=cs,
                                 overwrite=T);
    if (is_image(imageTool)) {
      vid := dv.loaddata(imageTool,'raster');
      vid.setoptions([axislabelswitch=T,
                      axislabels=T,
                      titletext=spaste(rec.head('Date',1),' - Antenna gain curves'),
                      xgridtype=['Tick marks'],
                      xgridcolor='white',
                      ygridtype=['Tick marks'],
                      ygridcolor='white',
                      axislabelspectralunit=['MHz'],
                      colormap=['Rainbow 3']]
                    );
      vdp := dv.newdisplaypanel();
      vdp.register(vid);
    } else {
      print '[antennagaincurve] No valid image tool created';
    }
  }

  # ------------------------------------------------------------------
  # Apply results

  if (Apply) {
    print '[antennagaincurve] Applying extracted gain curves.';
  }
}

#-------------------------------------------------------------------------------
#Section: [3] Experimental processing routines
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: SpectrumStatistics
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Extract some statistics of a spectrum. This serves for establishing some
#      number on which a decision for the antenna gain extraction algorithms
#      may be based on.
#Par:  rec        - LOPES-Tool data object, passed by reference.
#Par:  index      - Antenna set on which to operate; -1 will force working on
#                   all antennas.
#Par:  blocksize  - Size of a data block, [samples].
#Par:  nofBlocks  - Number of data blocks to analyze.
#Par:  nofBands   - Number of sub-bands into which the spectrum is sub-divided
#                   for analysis.
#Par:  plotCurves - Plot the extracted curves? 
#Example: SpectrumStatistics(data,index=1,nofBands=100,plotCurves=T)
#Created: 2005/07/28 (Lars B&auml;hren)
#-------------------------------------------------------------------------------

SpectrumStatistics := function (ref rec,
                                index=-1,
                                blocksize=128^2,
                                nofBlocks=1,
                                nofBands=128,
                                plotCurves=F)
{

  # ------------------------------------------------------------------
  # Antenna selection

  print '[SpectrumStatistics] Selecting antennas ...';

  if (index != -1 ) {
    rec.select(1:rec.len,F);
    rec.select(index,T);
  } else {
    rec.select(1:rec.len,T);
  }

  # ------------------------------------------------------------------
  # Adjustment of block sizes
 
  print '[SpectrumStatistics] Adjusting data block size ...';

  rec.setblock(blocksize=blocksize,index=-1);
  rec.replot();

  freqValues := rec.get("Frequency")

  step := rec.getmeta('FFTlen')/(nofBands);
  for (n in 0:nofBands) {
    bandLimits[n+1] := as_integer(n*step);
  }

  channelRatio := rec.getmeta('FFTlen',1)/nofBands;

  print ' - Data block size ......... :',blocksize;
  print ' - FFT length / # Bands .... :',rec.getmeta('FFTlen',1),'/',nofBands,'=',
                                         channelRatio;
  print ' - Frequency resolution [Hz] :',freqValues[2]-freqValues[1],'->',
                                         (freqValues[2]-freqValues[1])*channelRatio;
  print ' - Number of data blocks ... :',nofBlocks;
  print ' - Frequencies per band .... :',step;

  # ------------------------------------------------------------------
  # Processing ...

  print '[SpectrumStatistics] Processing data ...';

  val_min  := array(0.0,nofBands);
  val_mean := array(0.0,nofBands);
  val_max  := array(0.0,nofBands);
  val_var  := array(0.0,nofBands);
  freqAxis := array(0.0,nofBands);

  for (nAntenna in index) {

    print '  Antenna',nAntenna,'...';

    rec.setblock(n=1,index=nAntenna);

    bar := progress (0, nofBlocks, 'Analyzing data blocks ...');
    bar.activate()   

    for (nBlock in seq(nofBlocks)) {

      # get the spectrum for the current data block
      fft := rec.get('FFT',nAntenna);

      if (nBlock == 1) {

	for (nBand in 1:nofBands) {
	  bandRange := [bandLimits[nBand]+1,bandLimits[nBand+1]];
	  band := abs(fft[bandRange[1]:bandRange[2]]);
	  #
	  val_min[nBand]  := min(band);
	  val_mean[nBand] := mean(band);
	  val_max[nBand]  := max(band);
	  val_var[nBand]  := variance(band);
	  #
	  freqAxis[nBand] := mean(freqValues[bandRange[1]:bandRange[2]]);
	}
      } else {

	for (nBand in 1:nofBands) {
	  bandRange := [bandLimits[nBand]+1,bandLimits[nBand+1]];
	  band := abs(fft[bandRange[1]:bandRange[2]]);
	  # 
	  val_min[nBand]  := min([val_min[nBand],min(band)]);
	  val_mean[nBand] := min([val_mean[nBand],mean(band)]);
	  val_max[nBand]  := min([val_max[nBand],max(band)]);
	  val_var[nBand]  := min([val_var[nBand],variance(band)]);
	}
      }
  
      bar.update(nBlock);

      rec.nextblock(index=nAntenna);

    } # end [nBlock]

    # ----------------------------------------------------------------
    # Plot of the extracted statistics

    if (plotCurves) {

      pg := pgplotter(size=[1000,750]);
      pg.subp(2,2);

      label := spaste(rec.head('Date'),
                      ', Blocksize = ',blocksize,
                      ', # bands = ',nofBands,
                      ', Antenna =',nAntenna);

      # next sub-plot ------------------------------

      pg.setcolor(1);
      pg.env(min(freqAxis),max(freqAxis),min(log(val_min)),max(log(val_max)), 0, 0);
      pg.lab('Frequency [Hz]','log(Power)',label);

      pg.setcolor(2);
      pg.line(freqAxis,log(val_min));

      pg.setcolor(8);
      pg.line(freqAxis,log(val_mean));

      pg.setcolor(4);
      pg.line(freqAxis,log(val_max));

      # next sub-plot ------------------------------

      pg.setcolor(1);
      pg.env(min(freqAxis),max(freqAxis),min(log(val_var)),max(log(val_var)), 0, 0);
      pg.lab('Frequency [Hz]','log(Variance)',label);

      pg.setcolor(2);
      pg.line(freqAxis,log(val_var));

      # next sub-plot ------------------------------

      y := val_mean/val_var;

      pg.setcolor(1);
      pg.env(min(freqAxis),max(freqAxis),min(y), max(y), 0, 0);
      pg.lab('Frequency [Hz]','Mean/Variance',label);

      pg.setcolor(2);
      pg.line(freqAxis,y);

      # next sub-plot ------------------------------

      y := val_mean-val_var;

      pg.setcolor(1);
      pg.env(min(freqAxis),max(freqAxis),min(y), max(y), 0, 0);
      pg.lab('Frequency [Hz]','Mean-Variance',label);

      pg.setcolor(2);
      pg.line(freqAxis,y);

    } # end [plotCurves]

  } # end [nAntenna]

}