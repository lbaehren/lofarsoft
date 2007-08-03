
#-------------------------------------------------------------------------------
# $Id: ImageTools.g,v 1.2 2006/03/29 14:46:13 bahren Exp $
#-------------------------------------------------------------------------------
#
# Methods for loading, analyzing and editing of AIPS++ image tools:
# <ul>
#   <li><a href="#gui">Graphical User Interface (GUI)</a>
#   <li><a href="#io">File I/O</a>
#   <li><a href="#plotting">Plotting</a> (spectra, ...)
#   <li><a href="#analysis">Analysis</a> (statistics, ...)
#   <li><a href="#processing">Processing</a> (concatenation, addition, ...)
#   <li><a href="#coordinates">Coordinates</a>
# </ul>
# Some additional routines operating on image tools are:
# <ul>
#   <li><a href="Skymapper.doc.html#createImageTool">createImageTool</a>
#       (Skymapper.g)
#   <li><a href="Skymapper.doc.html#stackImageTools">stackImageTools</a>
#       (Skymapper.g)
# </ul>
#
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#
#Section: <a name="gui">Graphical User Interface (GUI)</a>
#
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: ImageToolsGUI
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Graphical frontend for various tools to analyze and/or manipulate image
#      tools.
#Par:  rec   - Record where the data is stored.
#Ref:  loadimagetool,removeImages,moviefromimage_gui,find_peak_in_image,
#      calibrate_phases_on_pointsource
#Created: 2004/08/24 (Lars B&auml;hren) 
#Status:  experimental
#-------------------------------------------------------------------------------

ImageToolsGUI := function (ref rec) {

  tk_hold();

  imggui := [=];
  imggui.frame := frame(title='Image tool manipulator',
                        width=1000,height=350,tpos='c');

  #-------------------------------------------------------------------
  # Image I/O

  imggui.io := SectionFrame (imggui.frame,'Image I/O',T);

  imggui.readimage := button (imggui.io.Contents,
                              'Read image tool from disk',
                              anchor='w', fill='x', relief='ridge');

  whenever imggui.readimage -> press do {
    global dcstarted;
    dc.setselectcallback(loadimagetool);
    dc.gui(F,'Image',F);
  }

  imggui.concat := button (imggui.io.Contents,
                           'Concat sequence of images',
                           anchor='w', fill='x', relief='ridge');

  whenever imggui.concat->press do {
    # Define a callback function
    setImageList := function (filename) {
      print "Loading image list from disk ...";
      tmp := [=];
      tmp := read_value(filename);
      concatImages (infiles=tmp);
      if (is_image(imageTool)) {
	    imageTool.view();
	  }
    }
    # start a catalog tool to select an image list stored on disk
    global dcstarted;
    dc.setselectcallback(setImageList);
    dc.gui(F,'Regular File',F);
  }

  imggui.disk:=button(imggui.io.Contents,'Remove list of image files', 
                      anchor='w', fill='x', relief='ridge');

  whenever imggui.disk->press do
  {
    global dcstarted;
    dc.setselectcallback(removeImages);
    dc.gui(F,'Regular File',F);  
  };

  #-------------------------------------------------------------------
  # Image analysis

  imggui.analysis := SectionFrame (imggui.frame,'Image analysis',T);

  imggui.summary := button(imggui.analysis.Contents,
                           'Print image summary and statistics',
                           anchor='w', fill='x', relief='ridge');

  whenever imggui.summary->press do {
    if (is_image(imageTool)) {
      imageTool.summary();
      imageTool.statistics();
    }
    else print "Sorry, you have to load an image first!"
  }

  imggui.findpointsource := button(imggui.analysis.Contents,
                                   'Find Point Source',
                                   anchor='w', fill='x', relief='ridge');

  whenever imggui.findpointsource->press do {
    find_peak_in_image(rec,imageTool) 
    plotgui.replot()
  }

  imggui.calibrate := button(imggui.analysis.Contents,
                             'Calibrate on Point Source',
                             anchor='w', fill='x', relief='ridge');

  whenever imggui.calibrate->press do {
   freqs := [as_float(eval(mapgui.freqRangeMin->get())),
             as_float(eval(mapgui.freqRangeMax->get()))]*1e6
    calibrate_phases_on_pointsource(rec,index=-1,frequency=freqs)
    plotgui.replot()
  }

  imggui.tpc := button(imggui.analysis.Contents,
                       'Two point correlation function',
                       anchor='w', fill='x', relief='ridge');

  whenever imggui.tpc->press do {
    tpc := imageTool.twopointcorrelation();
    tpc.view();
    plotgui.replot()
  }

  imggui.spectrum_f := frame(imggui.analysis.Contents,relief='ridge');

  imggui.spectrum_b := button(imggui.spectrum_f,
                              'Plot spectrum at image pixel',
                              anchor='w', fill='x', relief='flat');

  imggui.spectrum_e := entry(imggui.spectrum_f,background='lightgreen');
  
  if (is_image(imageTool)) {
    imggui.spectrum_e->insert(vector2string(as_integer(imageTool.shape()/2)));
  } else {
    imggui.spectrum_e->insert(vector2string(as_integer([-1,-1])));
  }

  whenever imggui.spectrum_b->press do {
    position  := eval(imggui.spectrum_e->get());
    normalize := as_boolean(imggui.spectrum_b1->state());
    logy      := as_boolean(imggui.spectrum_b2->state());
    clear     := as_boolean(imggui.spectrum_b3->state());
    plotSpectrumAtPixel (imageTool,position=position,normalize=normalize,
                         col=2,clear=clear);
  }

  imggui.spectrum_b1 := button(imggui.spectrum_f,'Normalize to maximum',
                               type='check',relief='flat',fill='x', anchor='w');

  imggui.spectrum_b2 := button(imggui.spectrum_f,'Logarithmic y-axis',
                               type='check',relief='flat',fill='x', anchor='w');

  imggui.spectrum_b3 := button(imggui.spectrum_f,'Clear plot window',
                               type='check',relief='flat',fill='x', anchor='w');

  #-------------------------------------------------------------------
  # Image processing

  imggui.processing := SectionFrame (imggui.frame,'Image processing',T);

  imggui.movie := button(imggui.processing.Contents,
                         'Create movie from image', 
                         anchor='w', fill='x', relief='ridge');

  whenever imggui.movie -> press do {
    global dcstarted;
    dc.setselectcallback(moviefromimage_gui)
    dc.gui(F,'Image',F);
  }

  imggui.add := button (imggui.processing.Contents,
                        'Co-add images',
                        anchor='w', fill='x', relief='ridge');

  whenever imggui.add -> press do {
    dirname := shell ('pwd');
    filelist := [=];
    selectedItemsFromList (filelist, dir=dirname);
    addImages (filelist.selection, display=T);
  }

  #-------------------------------------------------------------------
  # Control buttons for the GUI

  tk_release();

  imggui.control := frame(imggui.frame,side='right',expand='x',relief='groove');

  imggui.dismiss := button (imggui.control,'Dismiss',foreground='orange3',
                            bitmap=spaste(globalconst.ImageDir,'dismiss.xbm'),
                            borderwidth=0, hlthickness=0);

  whenever imggui.dismiss -> press do {
    val imggui.frame:=F;
    val imggui:=F;
  } 

}


#-------------------------------------------------------------------------------
#
#Section: <a name="io">File I/O</a>
#
#-------------------------------------------------------------------------------


#-----------------------------------------------------------------------
#Name: openCatalogGUI
#-----------------------------------------------------------------------
#Type:    Glish function
#Doc:     Open an AIPS++ catalog tool to select a file on disk.
#Par:     callbackFunction          - Name of the function called on
#                                     pressing 'Send&Dismiss'.
#Par:     type             =['all'] - Vector of strings with the file
#                                     type to be listed in the catalog
#                                     GUI
#Created: 2004/08/26 (Lars B&auml;hren) 
#Status:  production
#-----------------------------------------------------------------------

openCatalogGUI := function (callbackFunction,type=['all']) {
  global dcstarted;
  dc.setselectcallback(callbackFunction);
  dc.gui(F,type,F);  
}

#-----------------------------------------------------------------------
#Name: loadimagetool
#-----------------------------------------------------------------------
#Type: Glish function
#Doc:  Load an AIPS++ image tool from disk and display it in a
#      customized viewer tool.
#Par:  filename - Path the image tool stored on disk.
#Created: 2004/08/24 (Lars B&auml;hren) 
#Status: production
#-----------------------------------------------------------------------

loadimagetool := function (filename) {
  global imageTool;
  # check if the imageTool variable is in use already
  if (is_image(imageTool)) imageTool.done();
  # load the image from disk
  imageTool := image (filename);
  # extract the observation epoch
  cs    := imageTool.coordsys();
  epoch := dq.time(dm.getvalue(cs.epoch()),form='ymd');
  # display the loaded image 
  mdd := dv.loaddata(imageTool,'raster');
  mdd.setoptions([axislabelswitch=T,
	          axislabels=T,
		  titletext=epoch,
		  xgridtype=['Tick marks'],
		  xgridcolor='white',
		  ygridtype=['Tick marks'],
		  ygridcolor='foreground',
		  axislabelspectralunit=['MHz'],
		  colormap=['Rainbow 2']]
		 );
  vdp := dv.newdisplaypanel();
  vdp.register(mdd);
}

#-----------------------------------------------------------------------
#Name: removeImages
#-----------------------------------------------------------------------
#Type:    Glish function
#Doc:     As the <a href=Skymapper.doc.html>Skymapper</a> creates one
#         image tool per integration/timeframe, which are combined at
#         the final stage, there will be a lot of image files present in
#         the directory where the computation was run. Since - in order
#         to avoid loss of data - the intermediate image tools are not
#         deleted automatically, some cleaning up needs to be done
#         once the final image tool has been successfully created. A
#         complete list of files is stored on disk by the function
#         'stackImageTools()', such that we can read in this list from
#         disk, extract the name of the related files and delete those.
#Par:     diskfile       - Name of the diskfile in which the Glish record
#                          with the required image information is stored.
#Par:     keepRecord = F - Keep the record file itself after all image
#                          files have been deleted?
#Created: 2004/08/26 (Lars B&auml;hren)
#Status:  production
#-----------------------------------------------------------------------

removeImages := function (diskfile,keepRecord=F) {
  images := read_value(diskfile);
  if (is_record(images)) {
    nofEntries := len(images);
    print "[removeImages] Input record has",nofEntries,"entries.";
  } else {
    print "[removeImages] No valid record provided!";
    print images;
    fail;
  }
  for (i in 1:nofEntries) {
    if (has_field(images[i],'filename')) {
      print "[removeImages] Removing",images[i].filename,"...";
      shell(spaste('rm -rf ',images[i].filename));
    }
  }
  # Remove the record file from disk
  if (!keepRecord) {
    print "[removeImages] Removing",diskfile,"...";
    shell(spaste('rm -rf ',diskfile));
  }
  print "[removeImages] Finished.";
}

#-------------------------------------------------------------------------------
#
#Section: <a name="plotting">Plotting</a>
#
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: plotSpectrumAtPixel
#-------------------------------------------------------------------------------
#Type: GLish function
#Doc:  Plot the power spectrum at a given pixel position in an image tool.
#Par:  img       - A valid AIPS++ image tool, passed by reference.
#Par:  position  - Position at which to extract the spectrum; internal checking
#                  of the provided values is done.
#Par:  normalize - Normalize plot to maximum value.
#Par:  lines     - Option passed to the plotting routine.
#Par:  col       - Option passed to the plotting routine.
#Par:  clear     - Option passed to the plotting routine: clear the plot window 
#                  before drawing a new plot.
#Ref: plot
#Created: 2004/11/23 (Lars B&auml;hren)
#Status:  experimental
#-------------------------------------------------------------------------------

plotSpectrumAtPixel := function (ref img,position=[-1,-1],normalize=F,lines=T,
                                 col=-1,clear=T)
{

  # Check if the input really is a valid image tool

  if (!is_image(img)) {
    print "[plotSpectrumAtPixel] Input is not a valid image tool!";
    fail;
  } else {

    imgshape := img.shape();

    if (position[1] == -1 && position[2] == -1) {
      position := as_integer((imgshape)/2+1);
    }

    spectrum := img.getchunk()[position[1],position[2],,1];

    if (normalize) spectrum /:= max(spectrum);

    plot([1:len(spectrum)],log(spectrum),lines=lines,col=col,clear=clear);

  }

}


#-------------------------------------------------------------------------------
#Name: plotCutThroughImage
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  -/-
#Par:  imagetool - AIPS++ image tool 
#Par:  position  - Record with a set of positions at which cut through the
#                  image are performed [in pixel coordinates].
#Par:  axis      - Image axis along which the cut is produced
#Par:  range     - Range of the cut [in pixel coordinates]; if 'range=[-1,-1]'
#                  the full range along the specified axis is used.
#Par:  filename  - Name of the disk file to which the extracted slices are
#                  stored.
#Ret:  pgtool    - PGPlotter tool
#Created: 2004/08/24 (Lars B&auml;hren) 
#Status: experimental
#-------------------------------------------------------------------------------

plotCutThroughImage := function (ref img, position, axis=1, range=[-1,-1],
                                 filename='') {

  local cs;
  local imgshape;

  #-----------------------------------------------------------------------------
  # Check if 'img' contains a valid AIPS++ image tool; if this is not the case,
  # try to load an image tool from disk (check beforehand if the provided path
  # points to a file!

  if (!is_image(img)) {
    print "[SpectrumFromImage] Reading image tool from disk ...";
    img := image (infile=infile);
  }

  if (!is_image(img)) {
    print "[SpectrumFromImage] Warning, no valid image tool loaded!";
    fail;
  }

  #-----------------------------------------------------------------------------
  # Extract the part of the image data, which are to plotted

  cs := img.coordsys();
  epoch := cs.epoch();
  imgshape := img.shape();
  naxes    := len(imgshape);
  axisType := cs.coordinatetype(axis);
  axisUnit := cs.units()[axis];

  # set up the definition for slicing through the data volume

  nofPositions  := shape(position);

  cutDefinition := [=];
  positionPixel := array(1,naxes);
  positionMask  := array(1,naxes-1);

  # check the provided range for the cut-axis

  if (range[1] > imgshape[axis] || range[1] < 0) {
    range[1] := 1; 
  }
  if (range[2] > imgshape[axis] || range[2] <= range[1]) {
    range[2] := imgshape[axis];
  }

  #-------------------------------------------------------------------
  # Get the values along the x-axis and set the plot label position

  xaxis := valuesAlongImageAxis(img,axis=axis)[range[1]:range[2]];
  xpos  := LabelPositionPGPlot (xaxis,0.8);

  print "[plotCutThroughImage] Setup for x-axis:";
  print "- Length of selected axis ....... :",imgshape[axis];
  print "- Selected axis range (Pixels) .. :",range;
  print "- Selected axis range (World) ... :",min(xaxis),max(xaxis),axisUnit;

  #-------------------------------------------------------------------
  # Set up the settings and values for the individual cuts

  cut  := [=];
  yMin := max(img.getchunk());
  yMax := min(img.getchunk());

  print "[plotCutThroughImage] Extracting cuts through image ...";

  for (numPosition in 1:nofPositions) {

    # Definition for the cut

    m := 1;
    for (n in 1:naxes) {
      if (n != axis) {
	cutDefinition[n] := position[numPosition][m];
	positionPixel[n] := position[numPosition][m];
	positionMask[m]  := n;
	m +:= 1;
      }
      else {
        cutDefinition[n] := [range[1]:range[2]];
      }
    }

    # world coordinates for the location of the cut

    cut[numPosition].world := cs.toworld(positionPixel,'s')[positionMask]

    # Get the values along the cut through the image

    cut[numPosition].values := img.getchunk()[cutDefinition];
    
    # Keep track of the range of values

    if (min(cut[numPosition].values) < yMin) yMin := min(cut[numPosition].values);
    if (max(cut[numPosition].values) > yMax) yMax := max(cut[numPosition].values);

    # feedback to terminal

    print numPosition,cut[numPosition].world,yMin,yMax;

  }

  #-------------------------------------------------------------------
  # Open a PGPlotter tool 

  print "[plotCutThroughImage] Creating PGPlotter tool";

  pg := pgplotter();
  pg.env(min(xaxis),max(xaxis),yMin,yMax,0,0);
  pg.lab(spaste(axisType,' axis [',axisUnit,']'),      # x-axis label
	 'Intensity',                                  # y-axis label
	 '');                                          # top label

  for (numPosition in 1:nofPositions) {
    pg.setcolor(1+numPosition);
    pg.line(xaxis,cut[numPosition].values);
    ypos := LabelPositionPGPlot ([yMin,yMax],1-0.05*numPosition);
    pg.ptxt(xpos,ypos,0,0,cut[numPosition].world);
  }

  #-----------------------------------------------------------------------------
  # Output to file

  if (filename) {
    # loop over the selected axis
    for (axisPos in 1:len(xaxis)) {
      # export axis value to disk
      # loop over the cross-cuts
      for (numCut in 1:nofPositions) {
        # export the value for the individual cuts
      }
    }
  }

  return pg;

}


#-------------------------------------------------------------------------------
#Name: plotImageSequence
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  -/-
#Par:  img    - AIPS++ image tool, passed by reference
#Par:  axis   - Image axis along which the frame sequence is plotted
#Par:  frames - Range of image frames to plot.
#Created: 2004/10/18 (Lars B&auml;hren)
#Status:  under construction
#-------------------------------------------------------------------------------

plotImageSequence := function (ref img, axis=3, frames)
{
  #-------------------------------------------------------------------
  # check if 'img' really is a valid image tool

  if (!is_image(img)) {
    print "[plotImageSequence] Error, no valid image tool provided!";
    fail;
  }

  #-------------------------------------------------------------------
  # Get the characteristics of the image and on this check the
  # additional function parameters

  imgshape := img.shape();
  naxes    := len(imgshape);

  if (axis > naxes) {
    print "[plotImageSequence] Error, input image does not have axis",axis;
    fail;
  }

  if (frames[1] < 1 || frames[1] > imgshape(axis)) {
    print "[plotImageSequence] Warning, start of image sequence outside valid range."
    frame[1] := 1;
  }
  if (frames[2] < 1 || frames[2] > imgshape(axis)) {
    print "[plotImageSequence] Warning, end of image sequence outside valid range."
    frame[2] := imgshape(axis);
  }

  #-------------------------------------------------------------------
  # Se up a new display for the image, such that we can use the animator
  # tool to step through the image frames

  mdp := dv.newdisplaypanel();
  mdd := dv.loaddata(img,'raster');
  
  mdp.register(mdd);
  mda := mdp.animator();
}


#-------------------------------------------------------------------------------
#Name: plotTimeDistancePlane
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  This assumes a least a 4D input image, consisting of two direction axes
#      (lon,lat) and two linear axes (distance,time).
#Par:  img        - Reference to an AIPS++ image tool.
#Par:  direction  - Direction for which to generate the plot; if no direction is
#                   provided, i.e. direction=F, we integrate over the whole
#                   sphere of radius R.
#Par:  outfile    - Output image file name.
#Ret:  imgCut      - AIPS++ image tool
#Created: 2004/12/13 (Lars B&auml;hren)
#Todo: Automatic detection of distance and time axis from coordinate system
#      tool.
#-------------------------------------------------------------------------------

plotTimeDistancePlane := function (ref img, direction=F,
                                   outfile='plotTimeDistancePlane.img')
{
  inputFromFile := F;

  if (!is_image(img)) {
    filename := img;
    if (dms.fileexists(filename)) {
      img := image (filename);
      inputFromFile := T;
    }
    else {
      print "[plotTimeDistancePlane] No imagge tool provided!";
      fail;
    }
  }

  #-------------------------------------------------------------------
  # Extract coordinate information from the input image

  cs   := img.coordsys();
  mask := [F,F,T,T,F];
  
  epoch     := cs.epoch();
  telescope := cs.telescope();
  observer  := cs.observer();
  crpix     := cs.referencepixel()[mask];
  crval     := cs.referencevalue()[mask];
  cdelt     := cs.increment()[mask];
  units     := cs.units()[mask];

  cs.done();

  #-------------------------------------------------------------------
  # Set up a new coordinate system tool

  cs0 := coordsys(linear=2);

  rec := cs0.torecord();
  
  rec.linear0.units := [as_string(units[2]),as_string(units[1])];
  
  cs:=coordsys()
  cs.fromrecord(rec)

  cs.setepoch(epoch);
  cs.settelescope(telescope);
  cs.setobserver(observer);
  cs.setreferencepixel(crpix[2:1]);
  cs.setreferencevalue(crval[2:1]);
  cs.setincrement(cdelt[2:1]);

  #-------------------------------------------------------------------
  # Extract the pixel values from the input image

  imgshape := img.shape();
  local pixels := array(0.0,imgshape[4],imgshape[3]); # time x radius

  bar := progress (0,imgshape[4],'Extracting image pixels ...',)
  bar.activate();

  if (!direction) {
  }
  else {
    for (t in 1:imgshape[4]) {
      pixels[t,] := img.getchunk()[direction[1],direction[2],,t,1];
      bar.update(t);
    }
  }

  if (inputFromFile) img.done();

  imgCut := imagefromarray(outfile=outfile, pixels=pixels, csys=cs,
	                   overwrite=T);

  return imgCut;
}


#-------------------------------------------------------------------------------
#
#Section: <a name="analysis">Analysis</a>
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: findCoordinateAxisType
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Find the number of the image axis of a specific coordinate axis type.
#Par:  img                   - Reference to an AIPS++ image tool.
#Par:  axisType = 'Spectral' - Type of the coordinate axis to search for.
#Ret:  axis                  - The number of the image axis which is of the
#                              specified coordinate type; return 'F' if no image
#                              axis can be identified.
#Created: 2005/02/09 (L. B&auml;hren)
#-------------------------------------------------------------------------------

findCoordinateAxisType := function (ref img, axisType='Spectral')
{
  if (!is_image(img)) {
    print '[ImageTools] Input is no valid image tool!';
    return F;
  } else {
    axis := F;
    cs := img.coordsys();
    axisTypes := cs.axiscoordinatetypes();
    for (i in 1:len(axisTypes)) {
      if (axisType == axisTypes[i]) {
        axis := i;
      }
    }
    return axis;
  }
}

#-------------------------------------------------------------------------------
#Name: allskySpectrum
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  [still to be written]
#Par:  img - Reference to an AIPS++ image tool.
#Created: 2005/02/09 (L. B&auml;hren)
#-------------------------------------------------------------------------------

allskySpectrum := function (ref img, display=T) 
{
  # check the input
  if (!is_image(img)) {
    print '[ImageTools::allskySpectrum] No valid image tool provided!'
    fail;
  } else {
    # get the image shape and the number of the frequency axis
    imgshape := img.shape();
    freqAxis := findCoordinateAxisType (img);
    nofChan := imgshape[freqAxis];
    #
    regionBLC := array(1,len(imgshape));
    regionTRC := imgshape;
    #
    stats := [=];
    rms := array(0.0,nofChan);
    #
    bar := progress (0,nofChan,'[ImageTools::allskySpectrum] Extracting statistics...',)
    bar.activate();
    #
    for (freq in 1:nofChan) {
      # update the region definition
      regionBLC[freqAxis] := freq;
      regionTRC[freqAxis] := freq;
      r1 := drm.box(blc=regionBLC, trc=regionTRC);
      # get the image statistics 
      img.statistics (statsout=stats, region=r1, verbose=F)
      rms[freq] := stats.rms;
      # 
      bar.update(freq);
    }
    nofPoints := stats.npts;
    # get the frequency values themselves
    freqValues := valuesAlongImageAxis (img, axis=freqAxis);
    # plot the extracted spectrum
    plot (freqValues,
          log(rms/nofPoints),
          lines=T,
          col=2,
          clear=T,
          xlabel='Frequency [MHz]',
          ylabel='rms/pixel');
  }
}


#-------------------------------------------------------------------------------
#Name: trackSource
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Track the maximum within an image along a selected axis.<br>
#Par:  ref img - AIPS++ image tool; passed by reference
#Par:  axis = 4 - Image axis over which to track; by default this is done along
#                 the time axis.
#Par:  plotbrightness = T - Plot the maximum brightness against the values on
#                           tracking axis (e.g. max brightness as function of
#                           time).
#Example: trackSource (imageTool)</tt><br>
#         This is suitable for tracking the intensity maximum through a
#         [lon,lat,dist,time] data volume.<p>
#         <tt>trackSource (imageTool, axis=3)</tt><br>
#         Tracking of position (and time) through distance slices.
#-------------------------------------------------------------------------------

trackSource := function (ref img, axis=4, plotbrightness=T)
{
  if (!is_image(img)) {
    print '[ImageTools::trackSource] No valid image tool provided!';
    fail;
  }

  cs := img.coordsys();

  #-------------------------------------------------------------------
  # Basic settings for region within which the tracking is performed

  imgshape  := img.shape();
  regionBLC := array(1,len(imgshape));
  regionTRC := imgshape;

  #-------------------------------------------------------------------
  # 

  bar := progress (0,imgshape[axis],'Tracking maximum ...',)
  bar.activate();

  for (frame in 1:imgshape[axis]) {

    # set up a region within with the source search if confined
    regionBLC[axis] := frame;
    regionTRC[axis] := frame;
    r1 := drm.box(blc=regionBLC, trc=regionTRC)

    img.statistics (statsout=rec, region=r1, list=F);

    max[frame]    := rec.max;
    maxpos[frame] := rec.maxposf;

    bar.update(frame);
  }

  print "------------------------------------------------------------"
  for (frame in 1:imgshape[axis]) { 
    print frame,maxpos[frame],max[frame];
  }
  print "------------------------------------------------------------"

  #-------------------------------------------------------------------
  # Plot maximum brightness as function of the selected image axis

  if (plotbrightness) {
    dx := cs.increment()[axis];
    lx := cs.units()[axis];
    nx := cs.names()[axis];
    plot ([1:len(max)]*dx,max,lines=T,col=2,clear=T,
          xlabel=paste(nx,'[',lx,']'),ylabel='Brightness');
  }
  
}

#-------------------------------------------------------------------------------
#Name: analyseImage
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Perform some analysis on the generated AIPS++ image tool, esp. w.r.t. to
#      the coordinates.
#Created: 2004/08/24 (Lars B&auml;hren) 
#Status: under construction
#-------------------------------------------------------------------------------

analyseImage := function (ref rec, ref myimg) {

  img_shape := myimg.shape();
  img_cs := myimg.coordsys();
  
  # world coordinates of the outer pixels
  img_cs.toworld([1,1],'s');
  img_cs.toworld([1,img_shape[2]],'s');
  img_cs.toworld([img_shape[1],img_shape[2]],'s');
  img_cs.toworld([img_shape[1],1],'s');

  # world coordinates of the reference pixel
  refpixel := img_cs.referencepixel();
  img_cs.toworld(refpixel,'s');

}


#-------------------------------------------------------------------------------
#Name: valuesAlongImageAxis
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Extract world coordinate values along an axis of an image.
#Par:  inputTool - AIPS++ image tool.
#Par:  axis      - Index number of the axis for which to extract the values.
#Created: 2004/08/06 (Lars B&auml;hren)
#-------------------------------------------------------------------------------

valuesAlongImageAxis := function (ref inputTool, axis) {

  if (!is_image(inputTool)) {
    print "[valuesAlongImageAxis] No valid image tool provided!"
    fail;
  }

  # --------------------------------------------------------
  # Extract the required paramerers from the image tool
 
  cs      := inputTool.coordsys();
  naxes   := cs.naxes(T);
  imshape := inputTool.shape();

  p := array (0,naxes);
  w := array (0,imshape[axis]);

  # --------------------------------------------------------
  # Perform the actual conversion from pixel to world coordinates

  for (i in 1:imshape[axis]) {
    p[axis] := i;
    w[i]    := cs.toworld(p)[axis];
  }

  return w;

}

#-------------------------------------------------------------------------------
#Name: getDifferentialValues
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Get the differential values of data stored in an image tool along a
#      selected axis.
#Par:  rec  - Reference to the data structure.
#Par:  img  - Reference to an AIPS++ image tool.
#Par:  axis - Axis along which to proceed; by default this will be the last
#             imag axis.
#Par: verboseON - Be verbose during computation
#Ret:  newimg - Image tool constructed from the coordinate system tool of the 
#               input image and the differential data values along the selected
#               axis; image dimension will be the same as those of the input
#               image, except for the selected axis, where N{out} = N{in}-1.
#Created: 2004/12/01 (Lars B&auml;hren)
#Status: development
#-------------------------------------------------------------------------------

getDifferentialValues := function (ref rec, ref img, axis=-1,verboseON=T)
{
  if (!is_image(img)) {
    print "[ImageTools::getDifferentialValues] No valid image tool provided!";
    fail;
  }

  # Get the shape of the image data cube

  imgShape := img.shape();

  # Set the axis along which to compute the differential values

  if (axis == -1) axis := len(imgShape);

  # Set the shape of the output image

  newShape := imgShape;
  newShape[axis] -:= 1;

  # Set up a record used for slicing the pixel array

  index := [=];
  for (n in 1:len(newShape)) {
    if (n == axis) index[n] := 1;
    else index[n] := [];
  }

  # Extract the image pixel data and compute the changes along the selected axis

  bar := progress (0,newShape[axis],'Extracting differential dynamic spectrum ...',)
  bar.activate()   

  values1 := img.getchunk()[index]
  for (n in 1:newShape[axis]) {
    #
    index[axis] := n+1;
    values2 := img.getchunk()[index]
    #
    diff := values2-values1;
    print n,min(diff),mean(diff),max(diff);
    # 
    values1 := values2;
    #
    bar.update(n);
  }
  
  if (verboseON) {
    print "[ImageTools::getDifferentialValues]";
    print " - Selected image axis ....... :",axis;
    print " - Shape of the input image .. :",imgShape;
    print " - Shape of the output image . :",newShape;
    print " - Pixel array slicer ........ :",index;
  }

}


#-------------------------------------------------------------------------------
#
#Section: <a name="processing">Processing</a>
#
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: concatImages
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Construct an AIPS++ image by concatenating images. This function provides
#      an interface to the AIPS++ function 'imageconcat', while containing a
#      set of checks which are run before actually concatenating. The resulting
#      image tool is available through the global variable 'imageTool'.
#Par:  outfile =    - Output image file name.
#Par:  infiles =    - List of input AIPS++ image files to concatenate (vector of 
#                     strings).
#Par:  axis    = F  - Concatenation pixel axis.
#Par:  keep    = F  - Keep the input images after concatenation? This will only
#                     be carried out actually, if the creation of the
#                     concatenated image was successfull.
#Created: 2004/12/11 (Lars B&auml;hren)
#Status:  production
#-------------------------------------------------------------------------------

concatImages := function (outfile='imageconcat.img', ref infiles,
                          axis=F, keep=T) 
{
  global imageTool;
  local nofImages,mask;
  
  #-------------------------------------------------------------------
  # Is the concatenation axis defined?

  if (!axis) {
    print "[ImageTools::concatImages] No concatenation axis defined!";
    fail;
  }

  #-------------------------------------------------------------------
  # Check the list of input image files: 
  #  - Check if the files in the list indeed exist; if not mask this
  #    entry in the list.

  nofImages := len(infiles);
  mask := array(T,nofImages);

  for (i in 1:nofImages) {
    if (!dms.fileexists(infiles[i])) mask[i] := F;
  }

  infiles := infiles[mask];
  
  #-------------------------------------------------------------------
  # Concatenate the input images

  if (is_image(imageTool)) imageTool.done();

  if (len(infiles) == 1) {
    imageTool := imagefromimage(outfile=outfile,
                                infile=infiles[1],
                                overwrite=T);
  } else {
    imageTool := imageconcat(outfile=outfile, 
                             infiles=infiles, 
                             axis=axis, 
                             relax=T,
                             overwrite=T);
    if (is_image(imageTool)) {
      print "[ImageTools::concatImages] Successfully created new image",outfile;
    } else if (is_fail(imageTool)) {
      print "[ImageTools::concatImages] Failed to create new image.";
    }
  }

  #-------------------------------------------------------------------
  # Remove the input files, given everything else was successfull

  if (!keep && is_image(imageTool)) {
    print "[ImageTools::concatImages] Removing input images."
    for (i in 1:len(infiles)) {
      shell(spaste('rm -rf ',infiles[i]));
    }
  }

}


#-------------------------------------------------------------------------------
#Name: addImages
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Co-add a set of existing image tools.<br>
#      Given a list of image tools, create a co-added image. This assumes that
#      all images are of the same shape.<br>
#      <b>Note:</b> For some reason the function fails at adding the images;
#      direct execution of the function code (after setting the required input
#      variables) itself though performs just fine.
#Created: 2004/11/23 (Lars B&auml;hren)
#
#Par:  files          - A vector with the names of the images to be added.
#Par:  startImage = 1 - Image from the list which loaded first and on which
#                       thereafter all further images are added.
#Par:  freqBand   =   - Frequency band, [MHz].
#Par:  display    = T - Display the co-added image?
#
#Ref:  findCoordinateAxisType, valuesAlongImageAxis, mergeMasks
#
#
#Status:  testing (see note above)
#
#Example: <pre>
#         files := shell('ls | grep img');
#         startImage := floor(len(files)/2);
#         freqBand := [29,30];
#         addImages (files=files, startImage=startImage, freqBand=freqBand);
#         </pre>
#-------------------------------------------------------------------------------

addImages := function (files, startImage=1, freqBand=[0], display=T)
{
  global imageTool;
  imageName := 'add.img';

  print '[ImageTools::addImages]'

  #-------------------------------------------------------------------
  # Prepare loading sequence of the images

  print ' - Preparing loading sequence of the images ...'

  nofImages := len(files);

  fileMask := array(T,nofImages);
  fileMask[startImage] := F;

  fileList := [1:nofImages][fileMask]

  #-------------------------------------------------------------------
  # Set up mask for the frequency axis

  print ' - Setting up mask for the frequency axis ...';

  # load image to extract the information from
  img := image(files[startImage]);
  imgshape := img.shape();

  # determine which is the frequency axis
  freqAxis := findCoordinateAxisType (img);
  nofChan := imgshape[freqAxis];
  freqRange := array (1,2);

  if (len(freqBand) > 1) {
    # determine which channels match the frequency band detection
    freqValues := valuesAlongImageAxis (img, axis=freqAxis);
    freqMask   := freqValues>=freqBand[1] & freqValues<=freqBand[2];
    freqChan   := [1:nofChan][freqMask];
    freqRange  := [min(freqChan),max(freqChan)];
  } else {
    freqRange := [1,nofChan]
  }

  img.done();

  #-------------------------------------------------------------------
  # set of the image pixel region based on the frequency channel
  # selection

  print ' - Creating image region for pixel selection ...';

  regionBLC := array(1,len(imgshape));
  regionTRC := imgshape;
  regionBLC[freqAxis] := freqRange[1];
  regionTRC[freqAxis] := freqRange[2];
  r1 := drm.box(blc=regionBLC, trc=regionTRC);

  #-------------------------------------------------------------------
  # Load the first image from disk

  print ' - Loading first image from disk ...';

  img := imagefromimage (outfile=imageName,
                         infile=files[startImage],
                         region=r1);

  if (!is_image(img) || is_fail(img)) {
    print '[ImageTools::addImages] Failed creating initial image!';
    fail;
  } else {
    print ' -- Successfully loaded starting image.';
  }

  #-------------------------------------------------------------------
  # Load the rest of the images from disk to add them up

  print ' - Start adding images ...'
  
  bar := progress (0,
                   nofImages,
                   '[ImageTools::addImages] Adding images ...',);

  if (nofImages > 1) {
    bar.activate();
    for (i in fileList) {
      print ' Adding',files[i],'...';
      #
      img2 := imagefromimage(infile=files[i], region=r1);
      if (is_fail(img2) || !is_image(img2)) {
        print '[ImageTools::addImages] Error loading image',files[i];
      }
      #
      img.getregion(pixelmask=imgMask);
      img2.getregion(pixelmask=imgMask2);
      mask := mergeMasks (imgMask,imgMask2,'OR');
      print ' - Masks:',sum(imgMask),sum(imgMask2),sum(mask);
      img.putregion(pixelmask=mask);
      img2.putregion(pixelmask=mask);
      #
      img.calc('$img + $img2');
      if (is_fail(img2) || !is_image(img2)) {
        print '[ImageTools::addImages] Failed adding image',files[i];
      }
      #
      img2.done();
      bar.update(i)
    }
  } else {
    print '[ImageTools::addImages] Single image provided - nothing to add!';
  }

  img.done();

  #-------------------------------------------------------------------
  # make the created image tool visible to the outside by loading it
  # into the global 'imageTool' variable.

  if (is_image(imageTool)) {
    imageTool.done();
  }

  imageTool := image(infile=imageName);

  if (display && is_image(imageTool)) {
    imgageTool.view();
  }
}


#-------------------------------------------------------------------------------
#
#Section: <a name="coordinates">Coordinates</a>
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: fixImageCoordinates
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Apply a number of fixes to an existing image tool in order to properly
#      align the image pixels with the overlayed coordinate system grid. It
#      turns out, that depending on the celestial coordinate system used, the 
#      coordinate frame is rotated w.r.t. to the orientation it should have
#      (based on the expectation from the settings for the linear transform 
#      between pixel and intermediate world coordinates):
#      <ul>
#        <li>For J2000 the longitude is rotated by 180 degrees, originating from
#            a sign reversal in the CDELT value for the associated image axis.
#        <li>For GALACTIC the longitude is rotated by 90 degrees.
#      </ul>
#Par:  img          =   - An AIPS++ image tool, passed by reference.
#Par:  outfile      =   - Filename for the image to be stored to disk.
#Par:  keepOriginal = F - Keep the original image tool opened? (This later on
#                         might be expanded for actually replacing the input 
#                         image tool)
#Ret:  img2             - AIPS++ image tool with the coordinate fixes applied.
#Created: 2004/12/09 (L. B&auml;hren)
#Example: img := fixImageCoordinates(imageTool)
#-------------------------------------------------------------------------------

fixImageCoordinates := function (ref img, outfile='skymap-fixedCoordiates.img',
                                 keepOriginal=F)
{

  #-------------------------------------------------------------------
  # Check if the input really is a valid image file.

  if (!is_image(img)) {
    print "[ImageTools::fixImageCoordinates] No valid image tool provided!";
    fail;
  }

  #-------------------------------------------------------------------
  # Get the coordinate system tool to extract the coordinate system
  # reference code and the increment

  cs := img.coordsys();

  refcode := cs.referencecode()[1];
  cdelt   := cs.increment();

  #-------------------------------------------------------------------
  # Apply the fixes on the coordinate system tool, depending on the 
  # type of reference code or celestial coordinates.

  print "[ImageTools::fixImageCoordinates] Applying fix for",refcode;

  if (refcode == 'J2000' || refcode == 'AZEL') {
    cdelt[1] *:= -1;
    cs.setincrement(cdelt);
  }
  else if (refcode == 'GALACTIC') {
    print "[ImageTools::fixImageCoordinates] No fix for GALACTIC yet!";
  }
  else {
    print "[ImageTools::fixImageCoordinates] No fix required for",refcode;
  }

  #-------------------------------------------------------------------
  # set up a new image tool with the corrected coordinate increment

  img2 := imagefromarray(outfile,csys=cs,pixels=img.getchunk(),overwrite=T);

  #-------------------------------------------------------------------
  # Clean up

  if (!keepOriginal && is_image(img2)) {
    outfile := img.name(strippath=T);  # get original filename
    img.delete(F);                     # delete file of old image
    img2.rename(outfile, overwrite=T); # rename the new file
  }

  return img2;
}

