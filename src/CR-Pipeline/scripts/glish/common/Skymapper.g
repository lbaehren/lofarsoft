
#-------------------------------------------------------------------------------
# $Id: Skymapper.g,v 1.3 2006/08/09 10:01:43 bahren Exp $
#-------------------------------------------------------------------------------
#
# The Skymapper provides the functionality to create (multidimensional) skymaps
# from either raw digitized time-series data (LOPES, ITS) or preprocessed 
# cross-correlated data (ITS). For a detailed information consult the 
# <a href="http://www.astron.nl/~bahren/research/coding/LOPESTools_skymapper.html">online documentation</a>.
# <p>
# Contents of the local documentation:
# <ol>
#   <li><a href="#skymapper">The Skymapper main routine</a>
#   <li><a href="#wcs">World Coordinate Systems (WCS) related operations</a>
#   <li><a href="#freqDomain">Processing of frequency domain information</a>
#   <li><a href="#timeDomain">Processing of time domain information</a>
#   <li><a href="#image">Image tool related operations</a>
#   <li><a href="#misc">Misc. routines for dealing with files, etc.</a>
# </ol>
# A collection of tests is available in <a href="tSkymapper.doc.html">tSkymapper</a>
#
#-------------------------------------------------------------------------------



################################################################################
#
#Section: 1. <a name="skymapper">The Skymapper main routine</a>
#
################################################################################


#-------------------------------------------------------------------------------
#Name: skymapper
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Glish function interfacing between the GUI taking user input on the
#      characteristics of a skymap and the C/C++ routines performing the actual
#      computations. The communication between the Glish function and the C++
#      routines is carried utilizing the Glish event wrapper as provided by the
#      GlishSysEvent class (for direct links to the AIPS++ documentation see
#      Skymapper.h).
#
#Par: ref rec          - Reference to the data record.
#Par: fromDir          - Take input from diretory? If false, then the currently
#                        selected file will be processed.
#Par: mapTitle         - User defined title line to be displayed with the image.
#Par: index = -1       - Index of the selected antennae; if 'index=-1' the
#                        selection made in the GUI is adopted.
#Par: unit             - units [m] for the antenna positions
#Par: mapCenterWCS     - Reference code of the celestial coordinate system, in
#                        which the center of the map is defined.
#Par: mapCenterCoords  - Coordinates of the centeral point of the map
#Par: mapWCS           - Reference code for the celestial coordinates
#Par: mapProjection    - Spherical-to-cartesian projection
#Par: mapDirectionUp   - Orientation of the map: which direction (North/South)
#                        does the upper edge of the image point to.
#Par: mapDirectionLeft - Orientation of the map: which direction (East/West)
#                        does the left edge of the image point to.
#Par: minCSC           - CSC bounding box, lower left corner
#Par: maxCSC           - CSC bounding box, upper right corner
#Par: incrCSC          - angular resolution in celestial coordinates
#Par: mapPixels        - number of pixels of the generated sky map
#Par: mapDistance      - Use distance information for beamforming? To enable a
#                        record containing the following fields must be
#                        provided:
#                        <ul type="square">
#                          <li>rmin   = Minimum distance, [m]
#                          <li>incr   = Distance increment, [m]
#                          <li>slices = Number of distance slices/spheres
#                          <li>rmax   = Maximum distance, [m]; this field is
#                              optional, since the value can be derived from
#                              the other record fields.
#                        </ul>
#Par: nofIntegrations  - Number of integrations, i.e. the number of timeframes
#                        for which a map data cube is generated.
#Par: nofBlocks        - number of datablocks to process
#Par: nofSamples       - Number of samples per datablock.
#Par: firstBlock       - Number of the datablock, from which on the computation
#                        is started.
#Par: channels         - the number of output channels per pixel;
#                        if -1 all channels in the selected frequency
#                        band are returned
#Par: SignalQuantity   - Electrical quantity for which the map is computed; can
#                        be either "field" or "power". Keep in mind that a
#                        particular choice here will also put constraints on the
#                        method used for beamforming. 
#Par: SignalDomain     - Domain for which the electrical quantity is computed;
#                        can be either "time" or "freq".
#Par: exportFITS   = F - export the final image to FITS?
#Par: backupsNumber    - Number of backups to be made during processing a larger
#                        set of data blocks; at each instance a complete image
#                        is generated and written to disk. Furthermore progress 
#                        information are written to the logfile.
#Par: bfMethod         - Method used for beamforming.
#Par: dataType         - Type of processed data, i.e. the type of data (raw FFT,
#                        calibrated FFT, electrical voltage) passed to the
#                        Skymapper CLI.
#Par: pbeam            - Create a map of the array primary beam.
#Par: autoflagging = F - Automatic flagging of RFI contaminated frequency
#                        channels.
#Par: selfcal          - Add a single self-cal cycle if desired.
#Par: ViewMap          - Display image tool for each integration cycle?
#Par: LaunchViewer     - Launch viewer and display image tool at the end?
#Par: viewerIsDS9 = F  - Use DS9 as external viewer?
#                        If enabled, the created image tool is converted to FITS
#                        and afterwards loaded into DS9.
#Par: directDataIO = F - Direct data I/O by the CLI; this bypasses sending FFT
#                        data blocks over the Glish bus.
#Ref: VerifyFrequencyBand, FrequencyPassBand, FilenameFromHeader,
#     mapIntegrationTime, getAntennaPositions, FreqReferenceValue, FreqIncrement,
#     data.resetdir,setupCoordSysTool,verifyProcessingParameters
#Created: 2004/05/12 (Lars Baehren)
#Example: skymapper (data, channels=1)
#-------------------------------------------------------------------------------

skymapper := function (ref rec,
                       fromDir=F,
                       mapTitle='My new skymap',
                       band=[43.6,76.1],
                       paramAntenna=[index=-1,refant=-1,doz=T,xy=F],
                       unit=1,
                       mapCenterCoords=[0,90],
                       mapCenterWCS='AZEL',
                       mapWCS='AZEL',
                       mapProjection='STG',
                       mapGridding='projection',
                       mapDirectionUp='North',
                       mapDirectionLeft='East',
                       minCSC=[0,0],
                       maxCSC=[360,90],
                       incrCSC=[2,2],
                       rangeElevation=[0,90],
                       mapPixels=[120,120],
                       mapDistance=[rmin=-1,incr=100,slices=1,rmax=-1],
                       nofIntegrations=1,
                       nofBlocks=1,
                       nofSamples=2048,
                       firstBlock=1,
                       channels=-1,
                       SignalQuantity='power',
                       SignalDomain='freq',
                       exportFITS=F,
                       backupsNumber=1,
                       pbeam=F,
                       autoflagging=F,
                       bfMethod='addSignals',
                       dataType='CalFFT',
                       selfcal=F,
                       ViewMap=F,
                       LaunchViewer=T,
                       viewerIsDS9=F,
                       directDataIO=F
                      ) 
{
  #-------------------------------------------------------------------
  # Set up variables used throughout the function

  global globalpar;
  verboseON:=F;

  #-------------------------------------------------------------------
  # Print full list of function input parameters

  if (verboseON) {
    print '[Glish::skymapper] Input parameters';
    print fromDir,F;
    print mapTitle,'My new skymap';
    print band,[43.6,76.1];
    print paramAntenna,[index=-1,refant=-1,doz=T,xy=F];
    print unit,1;
    print mapCenterCoords,[0,90];
    print mapCenterWCS,'AZEL';
    print mapWCS,'AZEL';
    print mapProjection,'STG';
    print mapDirectionUp,'North';
    print mapDirectionLeft,'East';
    print minCSC,[0,0];
    print maxCSC,[360,90];
    print incrCSC,[1,1];
    print rangeElevation,[0,90];
    print mapPixels,[359,89];
    print mapDistance,[rmin=-1,incr=100,slices=1,rmax=-1];
    print nofIntegrations,1;
    print nofBlocks,1;
    print nofSamples,2048;
    print firstBlock,1;
    print channels,-1;
    print SignalQuantity,'power';
    print SignalDomain,'freq';
    print exportFITS,F;
    print backupsNumber,1;
    print pbeam,F;
    print autoflagging,F;
    print bfMethod,'addSignals';
    print dataType,'CalFFT';
    print selfcal,False;
    print ViewMap,F;
  }

  rec.savedirection(storagearea='1')
  rec.saveblockinfo(storagearea='1')

  # ------------------------------------------------------------------
  # Make sure that the global variable 'imageTool' is not used;
  # otherwise conflicts concerning AIPS++' internal data tables may
  # arise.

  if (is_image(imageTool)) {
    imageTool.done();
  }

  # ------------------------------------------------------------------
  # If the option to process files from a directory has been enabled,
  # we first need to extract a list of files.

  eventdir    := globalpar.cur_event_dir;
  filenames   := rec.get('Header').Filename;

  if (fromDir) {
    filenames   := shell(spaste('ls ', eventdir));
  } 
  
  # ------------------------------------------------------------------
  # Antenna related parameters: selection of the antenna set and choice
  # for the reference antenna

  if (paramAntenna.index != -1 ) {
    rec.select(1:rec.len,F);
    rec.select(paramAntenna.index,T);
  }
  if (bfMethod != 'ccMatrix') {
    paramAntenna.index := [1:rec.len][rec.select()];
  }
  nant := len(paramAntenna.index);

  if (paramAntenna.refant == -1) {
    paramAntenna.refant := rec.get('RefAnt');
  }

  # ------------------------------------------------------------------
  # Adjust the variables to control the progression through the
  # dataset, i.e. the number of integrations, the number of datablocks
  # per integration, the number of samples per datablock and the number
  # of the first block to start the computation with.

  ok := verifyProcessingParameters (rec, verboseON,
                                    firstBlock, nofIntegrations,
                                    nofBlocks, nofSamples,
                                    band, channels,
                                    SignalDomain, bfMethod);

  if (!ok) {
    print '[Glish::skymapper] verifyProcessingParameters returned non-zero error status!';
  }

  # ------------------------------------------------------------------
  # Clear all directional delays.

  ok := rec.setblock(n=firstBlock,blocksize=nofSamples);

  bar := progress (0, nant,'Clearing all directional delays ...',)
  bar.activate();

  antenna := 1;
  for (ant in paramAntenna.index) {
#    ok := rec.setblock(n=firstBlock, blocksize=nofSamples, index=ant);
    ok := rec.put(F,'DoZ',ant);
    ok := rec.resetdir(ant,resetdistance=T,resetxy=F);
#    ok := rec.touchref(rec.get('Input',ant),ant);
#    ok := rec.touch(rec.get('Input',ant),ant);
    #
    bar.update(antenna);
    antenna +:= 1;
  }

  rec.setphase();
  rec.refreshgui();
  rec.replot();

  # ------------------------------------------------------------------
  # Preparation to enable incremental backups in case of longer 
  # computation; each given number of steps an image tool is created
  # from the current skymap, as in possession of the Skymapper.  

  if (backupsNumber < 0) backupsNumber := 0;

  backupIncr    := nofIntegrations/(backupsNumber+1);
  backupCounter := 1;
  backupBlock   := as_integer(backupCounter*backupIncr);

  # ------------------------------------------------------------------
  # Automatic flagging of RFI contaminated frequency channels.

  if (autoflagging) autoflag(rec);

  # ------------------------------------------------------------------
  # Get epoch and location of the observation

  epoch   := qnt.quantity(rec.head('Date',1));
  obsName := rec.head('Observatory');

  # ------------------------------------------------------------------
  # Set up the records send to/received from the client and report its
  # contents

  if (is_agent(rec.newsagent)) {
    rec.newsagent->message('[Glish::skymapper] Preparing record passed to CLI ...');
  }

  recCLI := [=];

  # Data input
  recCLI.fromDir         := fromDir;
  recCLI.eventdir        := eventdir;
  recCLI.Filename        := FilenameFromHeader(rec);
  recCLI.antennaSet      := paramAntenna.index;
  recCLI.Integrations    := nofIntegrations;
  recCLI.blocks          := nofBlocks;
  recCLI.firstBlock      := firstBlock;
  recCLI.IntegrationTime := mapIntegrationTime (rec,nofBlocks,SignalDomain);
  recCLI.bfMethod        := bfMethod;
  recCLI.dataType        := dataType;
  
  recCLI.directDataIO := directDataIO;
  if (directDataIO) {
    recCLI.fx2Voltage := calibrationValues (rec,'Voltage');
    recCLI.fft2calfft := calibrationValues (rec,'CalFFT');
    FilesToRecord (rec,recCLI);
  }

  recCLI.mapTitle         := mapTitle;
  recCLI.epoch            := epoch;
  recCLI.epochValue       := epoch.value;
  recCLI.epochUnit        := epoch.unit;
  recCLI.ObservatoryName  := obsName;
  recCLI.CoordinateSystem := mapWCS;
  recCLI.Projection       := mapProjection;
  recCLI.mapCenterWCS     := mapCenterWCS;
  recCLI.mapCenterCoords  := mapCenterCoords;
  recCLI.minCSC           := mapCenterCoords;
  recCLI.maxCSC           := mapCenterCoords;
  recCLI.incrCSC          := incrCSC;
  recCLI.mapGridding      := mapGridding;

  recCLI.orientation      := [mapDirectionLeft,mapDirectionUp]

  if (mapDirectionUp == 'North') {
    recCLI.NorthUp := T;
  } else {
    recCLI.NorthUp := F;
  }

  if (mapDirectionLeft == 'East') {
    recCLI.EastLeft := T;
  } else {
    recCLI.EastLeft := F;
  }

  recCLI.rangeElevation   := rangeElevation;
  recCLI.pixels           := mapPixels;
  recCLI.SignalQuantity   := SignalQuantity;
  recCLI.SignalDomain     := SignalDomain;

  recCLI.AntennaMask      := rec.select();
  recCLI.antennaFile      := spaste(globalconst.progDir,"/antpos-its.dat");
  recCLI.AntennaPositions := getAntennaPositions (rec,
                                                  refant=paramAntenna.refant,
                                                  doz=paramAntenna.doz,
                                                  xy=paramAntenna.xy,
                                                  verboseON=verboseON);

  # Frequency domain
  DataToRecord (rec,recCLI,'Frequency');
  if (SignalDomain == 'time') {
    recCLI.FrequencyBand := band;
  }
  if (SignalDomain == 'freq') {
    recCLI.FrequencyBand := band*as_double(rec.head('FrequencyUnit'));
  }
  recCLI.Blocksize := as_integer(rec.getmeta('Blocksize'));

  # ----------------------------------------------------------------------------
  # Event 'initSkymapper' [Glish->CLI]
  #
  #   Initialize the internal member data of the Skymapper CLI. After this is
  #   done, the CLI will return the WCS parameters required for setting up a
  #   coordinate system tool, later to be attached to the image tool.
  # ----------------------------------------------------------------------------

  if (is_agent(rec.newsagent)) {
    rec.newsagent->message('[Glish::skymapper] Starting CLI ...');
  }

  SkymapperCLI := client(spaste(globalconst.LOPESBIND,'/skymapper'));

  if (is_agent(rec.newsagent)) {
    rec.newsagent->message('[Glish::skymapper] Initializing CLI ...');
  }

  recCoordsys := SkymapperCLI->initSkymapper(recCLI);

  #-------------------------------------------------------------------
  # Get the frequency mask

  passband := recCoordsys.frequencyMask;
  nfreq    := sum(passband);

  if (channels == -1 || channels > nfreq) {
    if (SignalDomain == 'freq') {
      channels := nfreq;
    }
    else if (SignalDomain == 'time') {
      channels := rec.getmeta('FFTSize');
    }
    else {}
  }

  # ----------------------------------------------------------------------------
  # Create a coordinate system tool for the image
  # ----------------------------------------------------------------------------

  recCoordsys.epoch      := epoch;
  recCoordsys.telescope  := obsName;

  recCoordsys.dir.crpix  := recCoordsys.referencePixel;
  recCoordsys.dir.crval  := recCoordsys.referenceValue;
  recCoordsys.dir.cdelt  := recCoordsys.increment;
  recCoordsys.dir.pc     := recCoordsys.xform;
  recCoordsys.dir.unit   := recCoordsys.worldAxisUnits;

  recCoordsys.dist.crpix  := 1;
  recCoordsys.dist.crval  := mapDistance.rmin;
  recCoordsys.dist.cdelt  := mapDistance.incr;
  recCoordsys.dist.unit   := 'm';

  recCoordsys.freq.crpix  := 1;
  recCoordsys.freq.crval  := FreqReferenceValue (band,channels)*10^6;
  recCoordsys.freq.cdelt  := FreqIncrement (band,channels)*10^6;
  recCoordsys.freq.unit   := 'Hz';

  recCoordsys.time.crpix  := 1;
  recCoordsys.time.crval  := rec.get('Time')[1];
  recCoordsys.time.cdelt  := mapIntegrationTime(rec,nofBlocks,SignalDomain);
  recCoordsys.time.unit   := 's';

  if (is_agent(rec.newsagent)) {
    rec.newsagent->message('[Glish::skymapper] Setting up coordinate system tool ...');
  }

  csxxx := setupCoordSysTool (rec,
                              inrec=recCoordsys,
                              mapWCS=mapWCS,
                              mapProjection=mapProjection,
                              SignalDomain=SignalDomain);

  if (!is_coordsys(csxxx)) {
    print "[Glish::skymapper] Warning, no valid coordinate system tool generated.";
    fail;
  } else {
    csxxx.summary();
  }

  # ----------------------------------------------------------------------------
  #
  #  NEW: rewrite of the looping through distances, timeframes & integrations
  # 
  # ----------------------------------------------------------------------------

  # [0] Preparation ------------------------------------------------ #
  #                                                                  #
  #     - additional data to the record passed to the CLI            #
  #     - set up name and shape for the final skymap                 #
  #     - tracking of time axis                                      #

  recCLI.nofChannels := channels;

  if (SignalDomain == 'freq') {
    nofFrequencies := channels;
    nofTimesteps   := 1;
  }
  else if (SignalDomain == 'time') {
    nofFrequencies := 1;
    nofTimesteps   := as_integer(rec.getmeta('Blocksize'));
  }

  naxis := [mapPixels[1],mapPixels[2],mapDistance.slices,nofTimesteps,
            nofFrequencies];

  imagefileBase := spaste(FilenameFromHeader(rec),'-',
                          csxxx.referencecode(type='direction'),'-',
                          csxxx.projection().type);

  nofLoopes := naxis[3]*nofIntegrations*nofBlocks;
  numLoop   := 0;

  # ----------------------------------------------------------------------------
  # Provide some feedback

  if (verboseON) {
    print '======================================================================'
    print ' [Glish::skymapper] Variable values for the CLI'
    print '======================================================================'
    print ' - Number of channels ........... :',recCLI.nofChannels;
    print ' - Number of frequencies ........ :',nofFrequencies;
    print ' - Number of integrations ....... :',nofIntegrations;
    print ' - Blocks per integration ....... :',nofBlocks;
    print ' - Samples per block ............ :',nofSamples;
    print ' - Shape of image pixel array ... :',naxis;
    print ' - Base of output image file name :',imagefileBase;
    print ' - Number of data block loops ... :',nofLoopes;

    print ' - Selected antennae ............ :',paramAntenna.index;
    print ' - Reference antenna ............ :',paramAntenna.refant;
    print ' - Shift to shower xy-position .. :',paramAntenna.xy;

    print ' - Selected frequency band   [Hz] :',recCLI.FrequencyBand;
    print ' - Size of the frequency mask ... :',len(passband);
    print ' - Number of frequency channels . :',nfreq;
    print '======================================================================'

    DataPrintBlockinfo(rec);

    DataPrintDelays(rec);
  }

  # [1] Computation of the skymap from the input data -------------- #
  #                                                                  #
  #     - Set up and activate the progress bar                       #

  bar := progress (0, nofLoopes,'Computing Skymap(s) ...',)
  bar.activate();

  for (numDistance in 1:mapDistance.slices) {

    # ----------------------------------------------------------------
    # Get the next value on the distance ladder 

    distance := mapDistance.rmin + mapDistance.incr*(numDistance-1);

    # ----------------------------------------------------------------
    # Append distance loop identifier to file basename

    filename := spaste(imagefileBase,'-',
                       padLoopIndex(numDistance,naxis[3]));
    filesDistance[numDistance] := filename;

    # ----------------------------------------------------------------
    # Correct the reference value for associated image axis

    crval := csxxx.referencevalue()
    crval[3] := distance;
    csxxx.setreferencevalue(crval);

    # ----------------------------------------------------------------
    # Compute the weighting factors for the Beamformer

    if (is_agent(rec.newsagent)) {
      rec.newsagent->message('[Glish::skymapper] Computing BF weighting factors ...');
    }

    recCLI.Distance := distance;
    ok := SkymapperCLI->setBeamformer(recCLI);

    if (!ok) {
      print '[Glish::skymapper] Initialization of the Beamformer has failed!';
      fail;
    }

    # ----------------------------------------------------------------
    # Positioning in the data volume to be processed; this is only
    # required if we actually move through the input data volume - if
    # we are just interested in a series of maps at different distances
    # we work on the same datablock all the time.

    if (nofIntegrations*nofBlocks != 1) {
      ok := rec.setblock(n=firstBlock, blocksize=nofSamples);
    }

    for (numIntegration in 1:nofIntegrations) {

      # --------------------------------------------------------------
      # Append integration loop identifier to file basename

      filename := spaste(filesDistance[numDistance],'-',
                         padLoopIndex(numIntegration,nofIntegrations));
      filesIntegration[numIntegration] := filename;

      # --------------------------------------------------------------
      # Initialize tracking of time values inside the iteration loops.

      timeBlock     := rec.get('Time');
      timeIteration := [timeBlock[1],timeBlock[len(nofSamples)]];

      # --------------------------------------------------------------
      # Start blockwise processing of the data; the CLI can either
      # receive data via the Glish bus (using data.get on the Glish
      # side), or directly read data from disk. In the latter case
      # the previously extracted gain tables are used to convert
      # f(x) -> Voltage -> FFT -> CalFFT.

      if (!directDataIO) {

        for (numBlock in 1:nofBlocks) {

          # ----------------------------------------------------------
          # Tracking of time values

          timeIteration[2] := rec.get('Time')[nofSamples];

          # ----------------------------------------------------------
          # Extract next block of data

          if (bfMethod != 'ccMatrix') {

            if (is_agent(rec.newsagent)) {
              rec.newsagent->message('[Glish::skymapper] Extracting data for CLI ...');
            }
            antenna := 1;

            if (dataType == 'Voltage') {
               local dataVoltage := array(0,as_integer(rec.getmeta('Blocksize')),nant);
	      for (ant in paramAntenna.index) {
	        dataVoltage[,antenna] := rec.get(dataType,ant);
	        antenna +:=1;
              }
              recCLI.antennaSignals := dataVoltage;
              } else {
                local dataFFT := array(1+0i,nfreq,nant);
	        for (ant in paramAntenna.index) {
	        # copy the data to the array passed to the CLI
	        dataFFT[,antenna] := rec.get(dataType,ant)[passband];
	        antenna +:=1;
              }
              recCLI.antennaSignals := dataFFT;
            }

            if (nofIntegrations*nofBlocks != 1) {
              rec.nextblock();
            }

          }

          # ----------------------------------------------------------
          # Pass datablock to CLI for processing

	  if (is_agent(rec.newsagent)) {
            rec.newsagent->message(spaste('[Glish::skymapper] Processing: ',
                                          'distance=',distance,'m, ',
                                          'integration=',numIntegration,
                                          '/',nofIntegrations,
                                          ', block=',numBlock,'/',nofBlocks));
          }

          ok := SkymapperCLI->data2Map(recCLI);

          # ----------------------------------------------------------
          # Show progress

          numLoop +:= 1;
          bar.update(numLoop);

        } ## end loop : numBlock

      } ## end '!directDataIO'
      else {
        print '[Glish::skymapper] Direct data I/O by CLI not yet implemented!'
      }

      # avoid sending unnecessary data over the Glish bus
      val recCLI.antennaSignals := F;

      # --------------------------------------------------------------
      # Request the image pixel values from the CLI; afterwards reset
      # the internal pixel storage array

      if (is_agent(rec.newsagent)) {
        rec.newsagent->message('[Glish::skymapper] Requesting skymap data ...');
      }
      recSkymap := SkymapperCLI->returnMap(recCLI);

      ok := SkymapperCLI->clearMap();

      if (!ok) {
        print '[Glish::skymapper] Error clearing CLI pixel storage array!';
        fail;
      }

      # --------------------------------------------------------------
      # Correct the reference value for associated image axis: time
      
      if (SignalDomain == 'freq' && bfMethod != 'ccMatrix') {
        crval := csxxx.referencevalue()
        crval[4] := mean(timeIteration);
        csxxx.setreferencevalue(crval);
      }

      # --------------------------------------------------------------

      recSkymap.naxis := naxis;

      if (is_agent(rec.newsagent)) {
        rec.newsagent->message('[Glish::skymapper] Creating image tool ...');
      }

      createImageTool (recSkymap,
                       csxxx,
                       filename=filename,
                       SignalDomain=SignalDomain,
                       viewmap=ViewMap);

    } ## end loop : numIntegration

    #-----------------------------------------------------------------
    # Concatenation of images for the current distance

    if (is_agent(rec.newsagent)) {
      rec.newsagent->message('[Glish::skymapper] Concatenating images ...');
    }

    concatImages (outfile=filesDistance[numDistance],
                  infiles=filesIntegration,axis=4,keep=F);

  } ## end loop : distance

  #-----------------------------------------------------------------
  # Concatenation of images after stepping through all distancies

  if (is_agent(rec.newsagent)) {
    rec.newsagent->message('[Glish::skymapper] Concatenating images ...');
  }

  concatImages (outfile=spaste(imagefileBase,'.img'),
                infiles=filesDistance,axis=3,keep=F);

  # [3] End of computation: wrap everything up --------------------- #

  if (is_image(imageTool)) {
    # Export image tool to FITS? 
    # (This is required when using an external viewer as DS9)
    if (exportFITS) {
      imageTool.tofits(spaste(imagefileBase,'.fits'));
    }
    # display of the image
    if (viewerIsDS9) {
      shell (spaste('ds9 ',imagefileBase,'.fits &'));
    }
    else if (LaunchViewer && !viewerIsDS9) {
      imageTool.view(axislabels=T);
    }
  } else {
    print '[Glish::skymapper] No valid image tool created!'
  }

  if (is_agent(rec.newsagent)) {
    rec.newsagent->message('[Glish::skymapper] Resetting Direction and Block Sizes ...');
  }

  rec.restoredirection(refresh=F,storagearea='1');
  rec.restoreblockinfo(storagearea='1',replot=T);

  SkymapperCLI := F;
  if (is_agent(rec.newsagent)) {
    rec.newsagent->message('[Glish::skymapper] All computations completed.');
  }
  globalpar.skymapwatch->done(T);

}


#-------------------------------------------------------------------------------
#Name: verifyProcessingParameters
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Check the processing parameters of the Skymapper for consistence.
#Par:  rec              - Reference to the data structure.
#Par:  firstBlock       - First block of data to be processed.
#Par:  nofIntegrations  - Number of integrations.
#Par:  nofBlocks        - Number of datablocks added per integration step.
#Par:  nofSamples       - Number of samples per datablock.
#Par:  freqBand         - Selected frequency band, in [MHz].
#Par:  freqChannels     - Number of output frequency channels.
#Par:  signalDomain     - Signal domain for the skymap.
#Par:  bfMethod         - Beamforming method.
#Par:  verboseON        - Enable/disable verbose mode.
#Ret:  ok               - Exit status; returns 'T' if successful
#Created: 2004/12/20 (Lars Baehren)
#Status: experimental
#Todo: Make 'VerifyTimeSteps' obsolete by including checks performed there!
#-------------------------------------------------------------------------------

verifyProcessingParameters := function (ref rec, verboseON=T,
                                        ref firstBlock, ref nofIntegrations,
                                        ref nofBlocks, ref nofSamples,
                                        ref freqBand, ref freqChannels,
                                        ref signalDomain, ref bfMethod)
{

  # ------------------------------------------------------------------
  # (a) If Skymapper CLI returns time domain data, we are not to add
  #     up multiple blocks. Furthermore we use the full available
  #     frequency bandwidth.

  if (signalDomain == 'time') {
    nofBlocks    := 1;
    freqChannels := -1;
    freqBand[1]  := rec.getmeta('FrequencyLow');
    freqBand[2]  := rec.getmeta('FrequencyHigh');
    freqBand /:= as_double(rec.head('FrequencyUnit'));
    bfMethod  := 'addSignals';
  } 
 
  # ------------------------------------------------------------------
  # (b) +++

  integrationsOrig := nofIntegrations;
  Filesize  := rec.getmeta('Filesize');
  Blocksize := rec.getmeta('Blocksize');

  numSamples := Blocksize*(firstBlock + (nofIntegrations-1)*nofBlocks);

  while (numSamples >= Filesize && nofIntegrations > 0) {
    nofIntegrations := nofIntegrations-1;
    numSamples := Blocksize*(firstBlock + (nofIntegrations-1)*nofBlocks);
  }

  # ------------------------------------------------------------------
  # (c) Check the selected frequency band.

#  freqBand := VerifyFrequencyBand (rec, freqBand);

  if (verboseON) {
    print '[Skymapper::verifyProcessingParameters]';
    print ' - First data block .................. :',firstBlock;
    print ' - Number of integrations ............ :',integrationsOrig,'->',
                                                     nofIntegrations;
    print ' - Datablocks per integration ........ :',nofBlocks;
    print ' - Samples per datablock ............. :',nofSamples;
    print ' - Selected frequency band ......[MHz] :',freqBand;
    print ' - Number of output frequency channels :',freqChannels;
    print ' - Signal domain for the skymap ...... :',signalDomain;
    print ' - Beamforming method ................ :',bfMethod;
  }

  return T;
}


#-------------------------------------------------------------------------------
#Name: locateTransmitters
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Locate the position of RFI sources mapping a small stripe of the sky 
#      just above the horizon. 
#Par:  rec                    - Reference to the data structure.
#Par:  dataset                - The dataset to load for performing the test; the
#                               variables thus should hold the fully specified 
#                               path to the directory holding the data.
#Par:  datatype  = 'project'  - Type of the dataset to be loaded; intthe present
#                               implementation only <i>Project Files</i> are 
#                               supported, but this might be generalized at some
#                               point.
#Par:  nofBands  = 15         - Number of frequency bands into which the
#                               frequency range is subdivided.
#Par:  blocksize = 2048       - Number of samples per datablock.
#Par:  threshold = 500        - Threshold above which the detected source will
#                               be accepted.
#Par:  projection = 'STG'     - Type of spherical-to-cartesian projection used
#                               for the created skymap. Depending on the chosen
#                               projection the coordinate of the map center and
#                               the shape of the pixel grid will be set.
#Example: locateTransmitters (data, '/data/ITS/exampledata.its')
#-------------------------------------------------------------------------------

locateTransmitters := function (ref rec,
                                dataset,
                                datatype='project',
                                nofBands=15,
                                blocksize=2048,
                                threshold=500,
                                projection='STG')
{
  global imageTool;

  local results := [=];

  if (datatype == 'project') {
    readprfiles_cbfunc(dataset);  
  } else {
    print '[locateTransmitters] Sorry, operation on datatype',datatype,
          'not yet supported.'
  }

  # ------------------------------------------------------------------
  # Settings for the different spherical-to-cartesian projections

  if (projection == 'STG') {
    mapCenter  := [0,90];
    nofPixels  := [240,240];
  }
  else if (projection == 'CAR') {
    mapCenter  := [0,5];
    nofPixels  := [358,10];
  }
  else if (projection == 'CSC') {
    mapCenter  := [0,5];
    nofPixels  := [358,10];
  }
  else if (projection == 'CEA') {
    mapCenter  := [0,5];
    nofPixels  := [358,10];
  }
  else if (projection == 'MER') {
    mapCenter  := [0,5];
    nofPixels  := [358,10];
  } else {
    print '[locateTransmitters] Projection',projection,'not yet supported.';
    fail;
  }
 
  # ------------------------------------------------------------------
  # Frequency settings: extract frequency limits from data record and
  # set up the frequency bands, within which we search for RFI
  # sources.

  freqUnit := as_double(rec.headf('FrequencyUnit'));
  freqLow  := as_double(rec.getmeta('FrequencyLow'));
  freqHigh := as_double(rec.getmeta('FrequencyHigh'));

  freqBandwidth := freqHigh-freqLow;

  freqLow  +:= 0.1*freqBandwidth;
  freqHigh -:= 0.1*freqBandwidth;

  bands:=freqLow*10^(log(freqHigh/freqLow)/nofBands*seq(0,nofBands))/freqUnit;

  # ------------------------------------------------------------------
  # Check the provided blocksize 

  Filesize := rec.getmeta('Filesize');

  if (blocksize > Filesize) {
    print '[locateTransmitters] Adjusting blocksize :',blocksize,'->',Filesize;
    blocksize := Filesize;
  }

  # ------------------------------------------------------------------
  # Run the Skymapper to create an image tool for further analysis

  dq.setformat('long','deg');
  dq.setformat('lat','deg');

  nofSources := 0;

  bar := progress (0, nofBands, 'Scanning frequency bands ...',)
  bar.activate();

  for (numBand in seq(nofBands)) {

    freqBand := [bands[numBand],bands[numBand+1]];

    skymapper (rec,
               band=freqBand,
               channels=1,
               mapCenterWCS='AZEL',
               mapWCS='AZEL',
               mapCenterCoords=mapCenter,
               mapProjection=projection,
               mapPixels=nofPixels,
               mapGridding='projection',
               rangeElevation=[0,10],
               incrCSC=[1,1],
               nofSamples=blocksize);

    if (is_image(imageTool)) {
      cl := imageTool.maxfit(point=F, width=5, negfind=F);
      #
      if (is_fail(cl)) {
        print '[locateTransmitters] No source found in band',freqBand;
      } else {
        # check if flux is above treshhold
        imageTool.statistics(imgrec);
        direction := dm.dirshow(cl.getrefdir(1))[1:2];
        flux := cl.getfluxvalue(1);
        rms  := imgrec.rms;
        if (flux[1] > threshold*rms) {
          nofSources +:= 1;
          #
          results[nofSources].freqBand  := freqBand;
          results[nofSources].direction := direction;
          results[nofSources].flux      := flux;
          results[nofSources].rms       := rms;
          #
          print '[locateTransmitters] Adding source to list.';
        } else {
          print '[locateTransmitters] Source found, but flux too low.';
        }
        print ' - Frequencies :',freqBand;
        print ' - Direction   :',direction;
        print ' - Flux/rms    :',flux[1],'/',rms,'=',flux[1]/rms;
      }
      imageTool.done();
    } else {
      print '[locateTransmitters] Sorry, no valid image tool created.';
    }

    bar.update(numBand);
  }

  # ------------------------------------------------------------------
  # Show the results of the computation; if sources were detected,
  # present a list with the characteristics.

  print '+-----------------------------------------------------------+';
  print '[locateTransmitters] Results                                 ';
  print '+-----------------------------------------------------------+';

  print ' - Detection threshold ...... :',threshold;
  print ' - Number of accepted sources :',nofSources;
  print ' - Number of frequency bands  :',nofBands;

  if (nofSources > 0) {
    print ' - List of detected/accepted sources:';
    for (n in seq(nofSources)) {
      print '   ',n,
            results[n].freqBand,
            results[n].direction,
            results[n].flux[1],
            results[n].rms;
    }
    print '+-----------------------------------------------------------+';
  }

}


################################################################################
#
#Section: 2. <a name="wcs">World Coordinate Systems (WCS) related operations</a>
#
################################################################################


#-------------------------------------------------------------------------------
#Name: setCoordinateSystemTool
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Create a coordinate system tool to be attached to a pixel data cube to
#      form an AIPS++ image tool.<br>
#      (a) Depending on wether the image has time or frequency as third image
#      axis, we use 'Spectral' or 'Linear' as axis type.<br>
#      (b) For the frequency/time axis we assume, that the frequencies
#      returned by "rec.get('Frequency')" are the center frequencies of a
#      frequency channel and not the lower boundary. Thus in case where we
#      produce a single output channel, we have to ensure, that we in fact use
#      the center of the selected frequency band.
#Par:  rec           - Reference to the data structure
#Par:  inrec         - Record with a description of the coordinate axes
#Par:  mapWCS        - reference code for the celestial coordinates
#Par:  mapProjection - spherical-to-cartesian projection
#Par:  band          - selected frequency band; [43.6,76.1] (3dB)
#Ret:  cs            - an AIPS++ Coordinate system tool
#Created: 2004/05/18 (Lars Baehren)
#-------------------------------------------------------------------------------

setupCoordSysTool := function (ref rec,
                               ref inrec,
			       mapWCS='AZEL',
			       mapProjection='CAR',
			       SignalDomain='freq',
                               verbose=F)
{
  #-------------------------------------------------------------------
  # [0] Construct a coordinate system tool

  cs := coordsys(direction=T,linear=2,spectral=T);

  if (!is_coordsys(cs)) {
    print "[setupCoordSysTool] Failure at step [0]";
  }

  #-------------------------------------------------------------------
  # [1] Reorder the coordinate axes and assign names

  cs.reorder([1,3,2]);
  cs.setnames(value="Longitude Latitude Distance Time Frequency");

  if (!is_coordsys(cs)) {
    print "[setupCoordSysTool] Failure at step [1]";
  }

  #-------------------------------------------------------------------
  # [2] Add information on observation: time, location & observer

  cs.setepoch(dm.epoch('utc',inrec.epoch));
  cs.settelescope(as_string(inrec.telescope));
  cs.setobserver(username());

  if (!is_coordsys(cs)) {
    print "[setupCoordSysTool] Failure at step [2]";
  }

  # ------------------------------------------------------------------
  # [3] Add coordinate system reference code and type of spherical
  # projection; if the projection requires specification of additional
  # parameters, they can be set using the information gathered from
  # WCSLIB.

  cs.setprojection(type=as_string(mapProjection),
                   parameters=inrec.projectionParameters);

  cs.setreferencecode(type='Direction', value=as_string(mapWCS));

  if (!is_coordsys(cs)) {
    print "[setupCoordSysTool] Failure at step [3]";
  }

  #-------------------------------------------------------------------
  # [4] Set up the world axis units of the axes; for separate handling
  # of distance and time axis we have to export the coordinate system 
  # tool to a record.

  cs.setunits(type='Direction',
              value=[as_string(inrec.dir.unit[1]),as_string(inrec.dir.unit[2])],  
              overwrite=T);

  cs.setunits(type='spectral',
              value=as_string(inrec.freq.unit),  
              overwrite=T);

  rec := cs.torecord();
  
  rec.linear1.units := [as_string(inrec.dist.unit),
                        as_string(inrec.time.unit)]
  
  # ------------------------------------------------------------------
  # [5] Create a second coordinate system tool from the original one
  #     using the record interface.
  
  cs2:=coordsys()
  cs2.fromrecord(rec)

  if (!is_coordsys(cs2)) {
    print "[setupCoordSysTool] Failure at step [4]";
  }
  else {
    cs.done();
    rec:=F; 
  }

  # ------------------------------------------------------------------
  # [6] Coordinate increment (CDELT)

  cdelt := [inrec.dir.cdelt[1],            # lon
            inrec.dir.cdelt[2],            # lat
            inrec.dist.cdelt,              # dist
            inrec.time.cdelt,              # time
            inrec.freq.cdelt];             # freq

  cs2.setincrement(value=cdelt);

  if (!is_coordsys(cs2)) {
    print "[setupCoordSysTool] Failure at step [6]";
  }

  # ------------------------------------------------------------------
  # [7] Reference value (crval)

  crval := [inrec.dir.crval[1],            # lon
            inrec.dir.crval[2],            # lat
            inrec.dist.crval,              # dist
            inrec.time.crval,              # time
            inrec.freq.crval];             # freq

  cs2.setreferencevalue(crval);

  if (!is_coordsys(cs2)) {
    print "[setupCoordSysTool] Failure at step [7]";
  }

  # ------------------------------------------------------------------
  # [8] Reference pixel (crpix)

  crpix := [inrec.dir.crpix[1],            # lon
            inrec.dir.crpix[2],            # lat
            inrec.dist.crpix,              # dist
            inrec.time.crpix,              # time
            inrec.freq.crpix]              # freq

  cs2.setreferencepixel(crpix);

  if (!is_coordsys(cs2)) {
    print "[setupCoordSysTool] Failure at step [8]";
  }

  #-------------------------------------------------------------------
  # [9] Check if a valid coordinate system tool has been constructed; if
  # this is not the case, at least provide a default coordinate sytem
  # tool matching the arrangement of the coordinate axes.

  if (!is_coordsys(cs2)) {
    print "[Skymapper::setupCoordSysTool]";
    print "Warning, corrupt coordinate system tool - returning default.";
    cs:=coordsys(direction=T,linear=2,spectral=T);
    rec := cs.torecord();
  } else {
    if (verbose) {
      print '[setupCoordSysTool] Summary of coordinate system tool:'
      print ' - Names ......... : ',cs2.names();
      print ' - Units ......... : ',cs2.units();
      print ' - Reference pixel : ',cs2.referencepixel();
      print ' - Reference value : ',cs2.referencevalue();
      print ' - Increment ..... : ',cs2.increment();
    }
    #
    rec := cs2.torecord();
  }

#  return rec;
  return cs2;
}


#-------------------------------------------------------------------------------
#Name: checkCoordsysTool
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Run a number of checks on a coordinate system tool (either being provided
#      directly or as part of an image tool).
#Par:  inputvar - Either a valid coordinate system tool or image tool; in the 
#                 later case the coordinate information then will be extracted
#                 from the image.
#Created: 2004/07/28 (Lars Baehren)
#-------------------------------------------------------------------------------

checkCoordsysTool := function (inputvar) {

  local cs;
  local inputIsImage;

  if (is_image(inputvar)) {
    inputIsImage := T;
    print "[checkCoordsysTool] Extracting coordinate system tool from image ..."
    cs := inputvar.coordsys();
  }
  else if (is_coordsys(inputvar)) cs := coordsys(inputvar);
  else fail;

  csrec := cs.torecord();

  #-------------------------------------------------------------------
  # [1] Provide a summary of the coordinate system tool

  print ""
  print "[1] Summary of the coordinate system tool"

  cs.summary();

  #-------------------------------------------------------------------
  # [2] Values of the FITS keywords for the direction axes

  print ""
  print "[2] Values of the FITS keywords for the direction axes";
  print ""

  crpix := cs.referencepixel('dir');
  crval := cs.referencevalue('s','dir');
  cdelt := cs.increment('s','dir');

  print "> crpix = ",crpix;
  print "> crval = ",crval;
  print "> CDELT = ",cdelt;

  #-------------------------------------------------------------------
  # [3] Check coordinate transformation: pixel -> world -> pixel

  print ""
  print "[3] Check coordinate transformation: pixel -> world -> pixel";
  print ""

  print "Center of the map";
  w := cs.toworld (crpix,'n');
  p := cs.topixel (w);
  print "pixel ...... :",crpix;
  print "-> world ... :",cs.toworld (crpix,'s');
  print "--> pixel .. :",p;
  print ""

  if (inputIsImage) {
    imshape  := inputvar.shape();
    imcenter := (imshape+1.0)/2.0;
    w := cs.toworld (imcenter,'n');
    p := cs.topixel (w);
    print "Center of map from image shape"
    print "pixel ...... :",imcenter;
    print "-> world ... :",cs.toworld (imcenter,'s');
    print "--> pixel .. :",p;
    print ""
  }

}


#-------------------------------------------------------------------------------
#Name: transformSphericalCoordinates
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Transformation between two spherical coordinate systems; this function
#      is the Glish counterpart of the 'convertSphericalCoordinates' method
#      provided in 'CoordinatesWCS'.
#Par:  ref rec - reference to the data record
#Par:  dirFROM        - position [deg] in the coordinates of the input
#                       coordinate system
#Par:  refcodeFROM    - reference code of the input coordinate system
#Par:  refcodeTO      - reference code of the output coordinate system
#Ret:  dirTO          - position [deg] in the coordinates of the output
#                       coordinate system
#-------------------------------------------------------------------------------

transformSphericalCoordinates := function (ref rec,
                                           dirFROM=[0,90],
                                           refcodeFROM='AZEL',
                                           refcodeTO='J2000') {

  todegree := 180.0/pi;
  toarc := pi/180.0;

  # Extract the observation time from the data record

  epoch := dm.epoch('utc',qnt.quantity(rec.head('Date',1)));
  if (is_fail(epoch)) {
    print "transformSphericalCoordinates() : ";
    print " -> error reading Epoch from metadata.";
    fail;
  }

  # Extract the name of the observatory from the data record

  observatory := dm.observatory(rec.head('Observatory'));
  if (is_fail(observatory)) {
    print "transformSphericalCoordinates() : ";
    print " -> error reading Observatory from metadata.";
    fail;
  }

  # Create a reference frame based on observation time and location

  dm.doframe(epoch);
  dm.doframe(observatory);

  # set a direction within this frame and convert it to celestial coordinates

  directionFROM := dm.direction(as_string(refcodeFROM),spaste(dirFROM[1],'deg'),
                                                       spaste(dirFROM[2],'deg'));
  if (is_fail(directionFROM)) {
    print "transformSphericalCoordinates() : ";
    print " -> error setting measure for direction in",refcodeFROM,".";
    fail;
  }

  directionTO := dm.measure(directionFROM,as_string(refcodeTO));
  if (is_fail(directionTO)) {
    print "transformSphericalCoordinates() : ";
    print " -> conversion to",refcodeTO,"coordinates failed.";
    fail;
  }

  # extract the position from the measure and return it as angle (deg)

  dirTO := [directionTO.m0.value,directionTO.m1.value]*todegree;

#  print dirFROM,"(",refcodeFROM,") ->",dirTO,"(",refcodeTO,")";

  return dirTO;

}


#-------------------------------------------------------------------------------
#Name: setRotationMatrix
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Set the linear transformation matrix used for the conversion from pixel
#      to projection plain coordinates; section 6.1 of WCS paper II explaines 
#      how to construct the matrix (FITS keyword PCi_j).
#-------------------------------------------------------------------------------

setRotationMatrix := function (rho=0,cdelt=[1,1])
{
  pc := array(0,2,2);

  lambda := cdelt[2]/cdelt[1];
 
  pc[1,1] := cos(rho);
  pc[1,2] := -sin(rho)*lambda;
  pc[2,1] := sin(rho)/lambda;
  pc[2,2] := cos(rho);

  return pc;
}


#-------------------------------------------------------------------------------
#Name: trackObject
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Plot the trajectory of an object ovver the hemisphere above the telescope
#      site.
#Par:  object   - Name of the object to track.
#Par:  position - Position of the object to track, provided as measure.
#Par:  timeinfo - Time interval during which the track is to be computed.
#Ret:  track    - Record containing the positions of the object for the
#                 specified time interval.
#Created: 2004/08/12 (Lars Baehren)
#Status: under construction
#-------------------------------------------------------------------------------

trackObject := function (object='sun',position,timeinfo) {}


################################################################################
#
#Section: 3. <a name="freqDomain">Processing of frequency domain information</a>
#
################################################################################

#-------------------------------------------------------------------------------
#Name: VerifyFrequencyBand
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Check if the selected frequency band is valid in the current exprimental
#      setup.
#Par:  ref recData   - Record where the data is stored (passed by reference)
#Par:  band          - Limits of the selected frequency band [MHz]
#Par:  margin = 0.05 - Margin kept between the maximum borders of the frequency
#                      band and the borders of the maximal available frequency 
#                      range; this parameter can be used to take into account 
#                      the filter profile at the edges of the frequency band.
#-------------------------------------------------------------------------------

VerifyFrequencyBand := function (ref recData,band,margin=0.05) {

  freqRange[1] := recData.getmeta('FrequencyLow',1);
  freqRange[2] := recData.getmeta('FrequencyHigh',1);

  freqRange /:= as_double(recData.head('FrequencyUnit'));

  bandwidth := freqRange[2]-freqRange[1];

  # Check the ordering of the frequencies

  if (band[2] < band[1]) band := [-1,-1];

  if (band[1] < freqRange[1] || band[1] < 0) band[1] := freqRange[1]+margin*bandwidth;
  if (band[2] > freqRange[2] || band[2] < 0) band[2] := freqRange[2]-margin*bandwidth;

  return band;

}

#-------------------------------------------------------------------------------
#Name: FrequenciesInBand
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Get the frequency values within a selected frequency band.
#Par:  ref recData - Record where the data is stored (passed by reference)
#Par:  band        - Limits of the selected frequency band. [MHz]
#Ret:  frequencies - Array with the frequency values within the selected band.
#Ref:  FrequencyPassBand
#Created: 2004/08/06 (Lars Baehren)
#-------------------------------------------------------------------------------

FrequenciesInBand := function (ref recData, band) {

  passband     := FrequencyPassBand (recData,band);
  frequencies  := recData.get('Frequency');

  return frequencies[passband];
}

#-------------------------------------------------------------------------------
#Name: FrequencyPassBand
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Set up a boolean array which, if applied to the set of available 
#      frequencies, acts as a bandpass filter.
#Par:  recData          - Data record.
#Par:  band             - The limits of the selected frequency band (in MHz).
#Par:  plotPassBand = F - Produce a plot of the bandpass filter.
#Par:  verboseON = T    - Run in verbose mode?
#Ret:  passband - Boolean array.
#Ref:  VerifyFrequencyBand
#Created: 2004/08/06 (Lars Baehren)
#Todo: Allow for different bandpass schemes, not only rectangular, but also e.g.
#      gaussian, cosine, etc.
#-------------------------------------------------------------------------------

FrequencyPassBand := function (ref recData, band, plotPassBand=F,verboseON=T) {

  band         := VerifyFrequencyBand(recData,band);
  frequencies  := recData.get('Frequency');
  nfreq        := len(frequencies);
  passband     := frequencies>=band[1] & frequencies<=band[2]

  if (verboseON) {
    print "[Skymapper::FrequencyPassBand]";
    print " - Frequency band .... :",band;
    print " - Frequency range ... :",frequencies[1],"..",frequencies[nfreq];
  }

  if (plotPassBand) {
    pg := pgplotter();
    pg.env(min(frequencies),max(frequencies),0,1.5, 0, 0);
    pg.setplottitle('Frequency bandpass applied to data');
    pg.setcolor(2);
    pg.line(frequencies,passband);
  }

  return passband;

}

#-------------------------------------------------------------------------------
#Name: FreqReferenceValue
#-------------------------------------------------------------------------------
#Type: Glish function
#Par:  band     - User selected frequency band, [MHz].
#Par:  channels - Number of frequency channels at output
#Ret:  refvalue - Reference value for the frequency axis, [MHz].
#-------------------------------------------------------------------------------

FreqReferenceValue := function (band,channels=1)
{
  refvalue := band[1];

  if (channels==1) refvalue := (band[1]+band[2])/2;
  else refvalue := band[1];

  return refvalue;
}

#-------------------------------------------------------------------------------
#Name: FreqIncrement
#-------------------------------------------------------------------------------
#Type: Glish function
#Par:  band     - User selected frequency band, [MHz].
#Par:  channels - Number of frequency channels at output
#Ret:  incr     - Increment along the frequency axis, [MHz].
#-------------------------------------------------------------------------------

FreqIncrement := function (band,channels=1)
 {
  incr := 1;

  if (channels==1) incr := band[2]-band[1];
  else incr := (band[2]-band[1])/(channels-1);

  return incr;
}


################################################################################
#
#Section: 4. <a name="timeDomain">Processing of time domain information</a>
#
################################################################################

#-------------------------------------------------------------------------------
#Name: mapIntegrationTime
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Given a number of M datablocks of N samples, compute the integration time
#      resulting from adding the blocks/frames.
#Par:  rec          - Reference to the data record
#Par:  nofBlocks    - the number of blocks per timestep
#Par:  signalDomain - Domain for which the electrical quantity is computed;
#Ret:  dt           - the integration time
#-------------------------------------------------------------------------------

mapIntegrationTime := function (ref rec, nofBlocks=1, signalDomain='freq')
{
  dt := 0.0;

  sample := 1/(rec.headf('Samplerate')*rec.headf('SamplerateUnit'));

  if (signalDomain == 'freq') {
    Blocksize  := rec.getmeta('Blocksize');
    dt := nofBlocks*Blocksize*sample;
  }
  else if (signalDomain == 'time') {
    dt := sample;  
  }
  else {
    print "[mapIntegrationTime] No method for handling domain",signalDomain;
  }

  return dt;  
}


#-------------------------------------------------------------------------------
#Name: TimeSampleQnt
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Extract an entry from a vector of Quanta
#Par:  ref QArray - reference to the array made up of N values and a single unit
#Par:  index      - index of the element which to convert into a quantity.
#Ret:  QElement   - the index-th element as quantity
#-------------------------------------------------------------------------------

TimeSampleQnt := function (ref QArray, index=1) {
  QElement := qnt.quantity(QArray.value[1],QArray.unit[1]);
  return QElement;
}


#-------------------------------------------------------------------------------
#Name: getTimeAxis
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Get the time values to correctly set up the time information on the 
#      tabular axis.
#Par:  rec   - The data record passed by reference
#Par:  blockinfo - Array containing settings for blockwise processing of the 
#                  data; the contents of the individual fields is
#                  <ul>
#                    <li>blockinfo[1] - First block to process
#                    <li>blockinfo[2] - Number of blocks to process
#                    <li>blockinfo[3] - Size of a block (in samples)
#                  </ul>
#Par:  type   - Type of the time inforation: relative (rel) or absolute (abs) ?
#Par:  method - Method by which to determine the time values; using 'scan' the 
#               previously prcessed datablocks are scanned for time information
#               (which is efficient in terms of computing time required). Using
#               'calc' the values along the time axis are calculated from the 
#               average separation of the first few blocks, assuming an
#               equidistant distribution along the time axis.
#Ret:  TimeBlockcenter - Array with the center time of the processed datablocks.
#Todo: type='abs' still needs to be implemented; but - does this relly make
#      sense?
#-------------------------------------------------------------------------------

getTimeAxis := function (ref rec,blockinfo,type='rel',method='calc')
{
  
  rec.setblock(n=blockinfo[1],blocksize=blockinfo[3]);

  LastBlock := blockinfo[1] + blockinfo[2] - 1;
  TimeBlockcenter := array (0,blockinfo[2]);

  epoch     := qnt.quantity(rec.head('Date',1));

  bar := progress (0,blockinfo[2], 'Extracting values for time axis',)
  bar.activate()   

  #---------------------------------------------------------
  # Give some basic feedback on the input parameters

  print "getTimeAxis: extracting block time information for tabular axis ...";
#  print "# First block to process . : ",blockinfo[1];
#  print "# Last block to process .. : ",LastBlock;
#  print "# Samples per block ...... : ",blockinfo[3];
#  print "# Type of time information : ",type;
#  print "# Extraction method ...... : ",method;

  #-------------------------------------------------------------------
  # Method 1 [scan] : (re)load the datablocks and extract time
  #                   information from the data header

  if (method == 'scan') {
    block  := blockinfo[1];
    num    := 1;
    while (block <= LastBlock)
    {
      # get the time offset from the trigger time
      TimeBlockcenter[num] := mean(rec.get('Time'));

      bar.update(num);

      # advance to the next block
      rec.nextblock();
      num +:= 1;
      block +:= 1;
    }
  }

  #-------------------------------------------------------------------
  # Method 2 [calc] : 

  if (method == 'calc') {

    rec.setblock(n=blockinfo[1],blocksize=blockinfo[3]);
    OffsetsMean[1] := mean(rec.get('Time'));

    rec.setblock(n=LastBlock,blocksize=blockinfo[3]);
    OffsetsMean[2] := mean(rec.get('Time'));

    OffsetIncr := (OffsetsMean[2] - OffsetsMean[1])/(blockinfo[2]-1);

    # set up the values on the time axis
    num   := 1;
    block := blockinfo[1];
    while (block <= LastBlock)
    {
      TimeBlockcenter[num] := OffsetsMean[1] + (num-1)*OffsetIncr;
      bar.update(num);
      num   +:= 1;
      block +:= 1;
    }    
    
  }

  rec.setblock(n=blockinfo[1],blocksize=blockinfo[3]);

  return TimeBlockcenter;

}


################################################################################
#
#Section: <a name="image">Image tool related operations</a>
#
################################################################################

#-------------------------------------------------------------------------------
#Name: createImageTool
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Create an AIPS++ image tool based on the results returned by the
#      Skymapper client (pixel values, pixel mask) and the coordinate system
#      tool created earlier.
#Par:  ref recImage - Record returned by the Skymapper client (passed via
#                     reference)
#Par:  csysTool     - Coordinate system tool (passed via reference)
#Par:  filename     - Output filename for the image tool
#Par:  SignalDomain - Signal domain of the image axis not degenerated.
#Par:  exportFITS = F - Enable/Disable export of the generated skymap to FITS
#Par:  viewmap    = T - Display the newly created image?
#Created: 2004/06/09 (L. Baehren)
#-------------------------------------------------------------------------------

createImageTool := function (ref recImage,
                             ref csysTool,
                             filename='skymap.img',
                             SignalQuantity='power',
                             SignalDomain='freq',
                             exportFITS=F,
                             viewmap=T,
                             verbose=F)
{
  global imageTool;

  #-------------------------------------------------------------------
  # Check if 'img' already is a valid image tool; if this is the case,
  # destroy the old image tool, to avoid conflicts with the Glish
  # image routines.

  if (is_image(imageTool)) imageTool.done();

  #-------------------------------------------------------------------
  # Check the provided coordinate system tool

  cs  := coordsys();

  if (verbose) {
    print '[createImageTool] Summary of coordinate system tool:'
    print ' - Names ......... : ',csysTool.names();
    print ' - Units ......... : ',csysTool.units();
    print ' - Reference pixel : ',csysTool.referencepixel();
    print ' - Reference value : ',csysTool.referencevalue();
    print ' - Increment ..... : ',csysTool.increment();
  }

  if (!is_coordsys(csysTool)) {
    print '[createImageTool] No valid coordinate system tool provided.';
  }

  local naxis := recImage.naxis;

  if ((len(naxis)) != csysTool.naxes()) {
    print "The number of axes of the coordinate system tool and image do not match!"
    print "-- Image axes in cs tool .. :",csysTool.naxes();
    print "-- Image axes in image tool : ",len(naxis);
    fail;
  }

  #-------------------------------------------------------------------
  # set up the brightness units 

  if (SignalQuantity == 'power') brightnessunit := 'V2';
  else if (SignalQuantity == 'field') brightnessunit := 'V';
  else { brightnessunit := 'Jy'; }

  #-------------------------------------------------------------------
  # set up the pixel array for the image tool

  if (SignalDomain == 'freq') {
    naxis[3] := 1;
    naxis[4] := 1;
  } 
  else if (SignalDomain == 'time') {
    naxis[3] := 1;
    naxis[5] := 1;
  }

  #                     lon      lat      dist     time     freq
  local tmp := array (0,naxis[1],naxis[2],naxis[3],naxis[4],naxis[5]);

  #-------------------------------------------------------------------
  # Create the AIPS++ image tool

  print "[createImageTool] Copying pixel values to image tool";

  if (SignalDomain == 'freq') {
#    for (lon in 1:naxis[1]) {
#      for (lat in 1:naxis[2]) {
#        for (freq in 1:naxis[5]) {
#          tmp[lon,lat,1,1,freq] := recImage.skymapValues[lon,lat,freq];
#        }
#      }
#    }
    tmp[,,1,1,] := recImage.skymapValues;
    imageTool := imagefromarray(outfile=filename,
                                overwrite=T,
                                pixels=tmp,
                                csys=csysTool);

#    for (lon in 1:naxis[1]) {
#      for (lat in 1:naxis[2]) {
#        for (freq in 1:naxis[5]) {
#          tmp[lon,lat,1,1,freq] := recImage.skymapMask[lon,lat,freq];
#        }
#      }
#    }
    tmp[,,1,1,] := recImage.skymapMask;
    imageTool.putregion(pixelmask=as_boolean(tmp));
  }
  if (SignalDomain == 'time') {
    tmp[,,1,,1] := recImage.skymapValues;
    imageTool := imagefromarray(outfile=filename,
                                overwrite=T,
                                pixels=tmp,
                                csys=csysTool);

    tmp[,,1,,1] := recImage.skymapMask;
    imageTool.putregion(pixelmask=tmp);
  }

  #-------------------------------------------------------------------

  imageTool.setcoordsys(csysTool); 

  if (is_image(imageTool)) {
    if (verbose) {
      print '[createImageTool] Summary of image tool coordinate system:'
      print ' - Names ......... : ',imageTool.coordsys().names();
      print ' - Units ......... : ',imageTool.coordsys().units();
      print ' - Reference pixel : ',imageTool.coordsys().referencepixel();
      print ' - Reference value : ',imageTool.coordsys().referencevalue();
      print ' - Increment ..... : ',imageTool.coordsys().increment();
    }
    #
    if (viewmap) {
      print '[createImageTool] Displaying create image tool'
      imageTool.view(axislabels=T);
    }
  }

}


################################################################################
#
#Section: <a name="misc">Misc. routines for dealing with files, etc.</a>
#
################################################################################


#-------------------------------------------------------------------------------
#Name: VerifyTimeframes
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Check if the variables used to control the progressing through the data
#      volume are consistent with the total size of the data. Given the fact,
#      the adjustment can be performed via four different variables, we chose
#      to adjust the number of timeframes, i.e. the number of data cubes
#      aligned along the temporal axis.
#Par:  rec         - Record where the data is stored.
#Par:  nofIntegrations - The number of integrations (aka. timeframes).
#Par:  nofBlocks       - The number of blocks integrated per timeframe.
#Par:  firstBlock      - The number of the block from which the computation is
#                        to start.
#Created: 2004/08/18 (Lars Baehren)
#Status:  experimental+++
#-------------------------------------------------------------------------------

VerifyTimeframes := function (ref rec, nofIntegrations, nofBlocks, 
                              firstBlock,verboseON=F) {

  integrationsOrig := nofIntegrations;
  Filesize  := rec.getmeta('Filesize');
  Blocksize := rec.getmeta('Blocksize');

  numSamples := Blocksize*(firstBlock + (nofIntegrations-1)*nofBlocks);

  while (numSamples >= Filesize && nofIntegrations > 0) {
    nofIntegrations := nofIntegrations-1;
    numSamples := Blocksize*(firstBlock + (nofIntegrations-1)*nofBlocks);
  }

  if (integrationsOrig != nofIntegrations) {
    print "[VerifyTimeframes]";
    print "Adjusted number of integrations :",integrationsOrig,"->",nofIntegrations;
  }

  if (verboseON) {
    print "[VerifyTimeframes]";
    print "- File size [samples] ...... :",Filesize;
    print "- Number of integrations ... :",nofIntegrations;
    print "- Blocks per integration ... :",nofBlocks;
    print "- Samples per block ........ :",Blocksize;
    print "- First block processed .... :",firstBlock;
    print "- Samples to be processed .. :",numSamples;
  }

  return nofIntegrations;

}


#-------------------------------------------------------------------------------
#Name: getAntennaPositions
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Obtain the antenna positions, relative to the position of the phase
#      center (which usually will be chosen as one the antenna positions).
#Par:  rec              - Reference to the data structure.
#Par:  index       =    - Index of data sets which are to be included.
#Par:  refant      = -1 - Reference antenna for which delay is by definition zero
#Par:  unit        = 1  - Input unit of length in meters
#Par:  phasecenter = F  - Shift reference antenna positions to phase center
#Par:  refpos      = F  - A 3-dim vector. This position is used as a reference
#                         position relative to the reference antennas.
#                         Logically, the position is relative to the array
#                         (phase) center if phasecenter=T.
#Par:  doz         = T  - Use the z-coordinate of the antenna position?
#Par:  xy          = F  - 
#Ret:  antennaPositions - Array with the 3D positions of the selected antennae.
#Ref:  CalcAntPos
#-------------------------------------------------------------------------------

getAntennaPositions := function (rec,index=-1,refant=-1,unit=1,phasecenter=F,
                                 refpos=F, doz=T, xy=F, verboseON=F) {

  if (index==-1) index := [1:rec.len][rec.select()];
  nant := len(index);

  if (refant == -1) refant := rec.get('RefAnt');

  if (xy) {
    refpos := [rec.get('XY')[2],rec.get('XY')[1],0];
    phasecenter := T;
  }

  # ------------------------------------------------------------------
  # Debugging output

  if (verboseON) {
    print '[Skymapper::getAntennaPositions]';
    print '- Reference antenna ......... :',refant;
    print '- Selected set of antennae    :',index;
    print '- Number of selected antennae :',sum(data.select());
    print '                              :',nant;
    print '- Reference position ........ :',refpos;
    print '- Shift to phase center ..... :',phasecenter;
    print '- Use position z-coordinate . :',doz;
    print '- Shift to shower core ...... :',xy;
  }

  # ------------------------------------------------------------------
  # Get the record with the antenna positions

  antpos := CalcAntPos (rec, index=index, refant=refant, unit=unit,
                        phasecenter=phasecenter, refpos=refpos,
                        verboseON=verboseON);
 
  if (is_fail(antpos)) {
    print '[Skymapper::getAntennaPositions] No valid results returned by CalcAntPos'
  }

  # ------------------------------------------------------------------
  # Extract the antenna position from the record into an array

  antennaPositions := array(0,len(antpos),3);
  for (ant in 1:len(antpos)) {
    if (verboseON) print ant,antpos[ant].antenna,antpos[ant].position;
    antennaPositions[ant,] := antpos[ant].position;
    if (!doz) antennaPositions[ant,3] := 0;
  }

  return antennaPositions;
}


#-------------------------------------------------------------------------------
#Name: FilenameForIntegration
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Set up the file name for an image file in a series of image files.
#Ref:  FilenameFromHeader
#-------------------------------------------------------------------------------

FilenameForIntegration := function (ref rec,nofIntegrations,NumIntegration)
{
  charsTotal   := as_integer(log(nofIntegrations));
  charsCurrent := as_integer(log(NumIntegration));
  diff := charsTotal - charsCurrent;

  filenameBase := FilenameFromHeader (ref rec);
  filename := spaste(filenameBase,'-');
  if (diff > 0) {
    for (i in 1:diff) filename := spaste(filename,'0');
  }
  filename := spaste(filename,NumIntegration);

  return filename;

}


#-------------------------------------------------------------------------------
#Name: replaceBlanks
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Replace blanks in a string by a user defined character
#Par:  inputString  - Input string in which the blanks are to be replaced
#Par:  replacement  - The Character used as replacement for the blanks
#Ret:  outputString - The input string with the blacks replaced by the set
#                     replacement character.
#-------------------------------------------------------------------------------

replaceBlanks := function (inputString='My Skymap',replacement='_') {

  # [1] Decompose the string into an array of strings
  inputString =~ s/\s+/$$/g
  words := len(inputString);

  # [2] set up the new title string substituting blanks
  outputString := inputString[1];
  for (i in 2:words) {
    outputString := spaste(outputString,replacement,inputString[i]);
  }

  return outputString;
  
}


#-------------------------------------------------------------------------------
#Name: vector2string
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Convert a vector to a string which cann be pasted into a GUI entry field
#Par:  x  - A vector of numbers.
#Ret:  vs - The vector as string.
#-------------------------------------------------------------------------------

vector2string := function (x=[1,2,3,4])
{
  vs := spaste('[',((spaste(x)~s/\s+(?!$)/,/g)~s/^\[//)~s/\].*$//,']');
  return vs;
}


#-------------------------------------------------------------------------------
#Name: viewImageWithDS9
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Use DS9 as external viewer to display created images.
#Par:  filenameBase  - Common filename base for image tool and FITS file.
#-------------------------------------------------------------------------------

viewImageWithDS9 := function (filenameBase='') {

  global globalpar;
  global imageTool;

  filename := imageTool.name();

  if (!globalpar.CASA) {
    print '[Glish::skymapper] Root of CASA installation unset - cannot continue!'
  }
  else {

    #-----------------------------------------------------------------
    # 1. Create shell script
    
    # 1.a Source aipsinit script of the CASA installation

    shell (spaste('echo . ',globalpar.CASA,'/aipsinit.sh > viewImageWithDS9'));
    shell (spaste('echo "echo \$""AIPSROOT" >> viewImageWithDS9'));
    shell ('echo >> viewImageWithDS9');

    # 1.b : Start Glish to convert image to FITS

    shell (spaste('echo glish -l viewImageWithDS9.g >> viewImageWithDS9'));
    shell ('echo >> viewImageWithDS9');

    # 1.c : Start ds9 to display FITS image

    shell (spaste('echo ds9 ',filenameBase,'.fits >> viewImageWithDS9'));

    #-----------------------------------------------------------------
    # 2. Glish script

    shell (spaste('echo "include \'image.g\'" > viewImageWithDS9.g'));
    shell (spaste('echo "img := image(\'"',filename,'"\');" >> viewImageWithDS9.g'));
    shell (spaste('echo "img.tofits(\'"',filenameBase,'".fits\');" >> viewImageWithDS9.g'));
    shell (spaste('echo "exit;" >> viewImageWithDS9.g'));

    #-----------------------------------------------------------------
    # 3. Run the shell script

    bin_bash := shell ('which bash');

    shell (spaste(bin_bash,' viewImageWithDS9'));

  }

}
