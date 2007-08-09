
#-------------------------------------------------------------------------------
# $Id$
#-------------------------------------------------------------------------------
#
# <i>moviefromimage</i>: Create an MPEG movie from an AIPS++ image tool
# 
# <ol>
#  <li><a href="moviefromimage.doc.html#moviefromimage_gui">moviefromimage_gui</a>
#      - The graphical user interface (GUI).
#  <li><a href="moviefromimage.doc.html#write_parfile">write_parfile</a>
#      - Creation of parameter file to drive 'mpeg_encode'.
#  <li><a href="moviefromimage.doc.html#moviefromimage">moviefromimage</a>
#      - Implementation of the conversion tool.
# </ol>
#
# <b>Important notes:</b>
# <ol>
#  <li>The current implementation will work properly only on
#  3-dimensional images, where the variable axis is the third image axis. For 
#  the time being a subimage needs to be created from the images delivered by the
#  Skymapper -- see the <a href="http://www.astron.nl/~bahren/research/coding/lopestools_moviemaker.html">online
#  documentation</a> for detailed description and example code.
#  <li>There is a bug in the 'setoptions()' function of 'viewerdisplaydata.g',
#  which causes failure when trying to set the options of displaydata tool from
#  previously exported options; this bug no longer is present in the recent CASA
#  code. For the time being you might replace
#  <tt>$AIPSARCH/libexec/viewerdisplaydata.g</tt> of the pre-CASA AIPS++ by the
#  CASA version.
# </ol>
#
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Section: Graphical Interface
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: moviefromimage_image
#-------------------------------------------------------------------------------
#Type: 
#Doc:  
#Par:  
#Par:  
#-------------------------------------------------------------------------------

moviefromimage_image := function (ref img, ref mapgui, ref aipsimage) {

  if (!is_image(img)) {
    print '[moviefromimage_image] No valid image tool provided!'
  } else {

    # Update the record storing the image data

    aipsimage.cs := img.coordsys(); 

    aipsimage.shape := img.shape();

    aipsimage.coordinatetypes := img.coordsys().axiscoordinatetypes()

    aipsimage.rangeZ := [1,aipsimage.shape[3]];
    aipsimage.ranges := array(1,3,2);
    aipsimage.ranges[,1] := [1,1,1];
    aipsimage.ranges[,2] := aipsimage.shape;
    aipsimage.flag:=[];

    # Update the GUI

    mapgui.rangesX_min->insert(as_string(aipsimage.ranges[1,1]));
    mapgui.rangesX_max->insert(as_string(aipsimage.ranges[1,2]));
    mapgui.rangesY_min->insert(as_string(aipsimage.ranges[2,1]));
    mapgui.rangesY_max->insert(as_string(aipsimage.ranges[2,2]));
    mapgui.rangesZ_min->insert(as_string(aipsimage.ranges[3,1]));
    mapgui.rangesZ_max->insert(as_string(aipsimage.ranges[3,2]));

  }

}


#-------------------------------------------------------------------------------
#Name: moviefromimage_gui
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Graphical frontend to 'moviefromimage()'; provides access to parameters
#      for selecting regions in the input image tool as well as file formats for
#      export of individual movie frames.
#Par:  filenameImage - Filename of the AIPS++ image tool to process.
#-------------------------------------------------------------------------------

moviefromimage_gui := function (filenameImage) {

  #
  # Records for communicating settings 
  #
  aipsimage := [=];
  addframes := [=];
  files     := [=];

  # ------------------------------------------------------------------
  # Extact some basic information from the image to be processed
  # ------------------------------------------------------------------

  aipsimage.nameImage := filenameImage;

  # load the image
  img := image(as_string(filenameImage));

  # extract and save the coordinate system associated with the image
  aipsimage.cs := img.coordsys(); 

  # get the shape of the image
  aipsimage.shape := img.shape();

  # extract the coordinate type of the 3rd axis
  aipsimage.coordinatetypes := img.coordsys().axiscoordinatetypes()

  # default values of the range of processed image frames
  aipsimage.rangeZ := [1,aipsimage.shape[3]];
  aipsimage.ranges := array(1,3,2);
  aipsimage.ranges[,1] := [1,1,1];
  aipsimage.ranges[,2] := aipsimage.shape;
  aipsimage.flag:=[];

  # ------------------------------------------------------------------
  # create a viewer tool in which we can adjust the mode in which the image
  # is displayed
  # ------------------------------------------------------------------

  mdp := dv.newdisplaypanel();
  mdd := dv.loaddata(img,'raster');
  mdd.setoptions([axislabelswitch=T,
                  titletext='Input AIPS++ image tool',
                  pixeltreatment=['center'],
                  xgridtype=['Tick marks'],
                  ygridtype=['Tick marks'],
                  colormap=['Rainbow 2']
                 ]);
  mdp.register(mdd);
  
  # get the color map names

  ### Start creating the GUI ###

  tk_hold();

  mapgui := [=];

  mapgui.frame := frame (title='Skymap MovieMaker',
                         width=1000,
                         height=350,
                         tpos='c');

  stdlength := 25;

  # ------------------------------------------------------------------
  # Loading of the input image
  # ------------------------------------------------------------------

  mapgui.inputimage := SectionFrame (mapgui.frame,
                                     'Characteristics of the input image',
                                     T);

  # name of the image
#  mapgui.inputimage_frame := frame(mapgui.input_frame,side='left',expand='x');
#  mapgui.inputimage_label := label(mapgui.inputimage_frame,'Input image :',width=20,anchor='e');
#  mapgui.inputimage_name  := label(mapgui.inputimage_frame,as_string(aipsimage.nameImage));

  # shape of the image

  mapgui.imageshape_frame := frame(mapgui.inputimage.Contents,
                                   side='left',
                                   expand='x');
  mapgui.imageshape_label := label(mapgui.imageshape_frame,
                                   text='Image shape :',
                                   width=20,
                                   anchor='e');
  mapgui.imageshape_name  := label(mapgui.imageshape_frame,
                                   spaste(aipsimage.shape));

  # units of the third image axis
  mapgui.thirdaxis_frame := frame(mapgui.inputimage.Contents,
                                  side='left',
                                  expand='x');
  mapgui.thirdaxis_label := label(mapgui.thirdaxis_frame,
                                  text='Coordinate types :',
                                  width=20,
                                  anchor='e');
  mapgui.thirdaxis_name  := label(mapgui.thirdaxis_frame,
                                  paste(aipsimage.coordinatetypes," "));

  #-------------------------------------------------------------------
  # Settings for the output names

  mapgui.names := SectionFrame (mapgui.frame,'Name settings',T);

  # Name of the created movie file

  mapgui.moviename := SectionEntry (mapgui.names,
                                    'Movie filename : ',
                                    'movie.mpg');

  # Titletext in the movie window  

  mapgui.movietitle := SectionEntry (mapgui.names,
                                     'Movie title line : ',
                                     'My great movie');

  whenever mapgui.movietitle.Entry -> return do {
    movietitle := as_string(mapgui.movietitle.Entry->get());
    mdd.setoptions([titletext=movietitle]);
    mdp.register(mdd);
  }

  #-------------------------------------------------------------------
  # Preprocessing of the image (smoothing, etc.)

  mapgui.preprocessing := SectionFrame (mapgui.frame,
                                        'Pre-Processing of the image',
                                        T);

  mapgui.hanning := SectionButton (mapgui.preprocessing,
                                   ' Smoothing : ',
                                   'check',
                                   'Apply Hanning filter')

  whenever mapgui.hanning.Button -> press do {

    # get the state of the button
    applyHanning := as_boolean(mapgui.hanning.Button->state());

    # if true, then apply filter ot the image
    if (applyHanning) { 
      # reset the button in the GUI
      mapgui.hanning.Button->state(F);
      # get information about the image
      imageName := img.name();
      imageHanning := spaste(imageName,'.hann');
      naxis := len(img.shape());
      #
      print '[moviefromimage] Applying Hanning filter to image ...';
      im2 := img.hanning(outfile=imageHanning,
                         axis=naxis, 
                         drop=F,
                         overwrite=T);
      #
      if (is_image(im2)) {
        img.done();
        im2.done();
        print '[moviefromimage] Reloading image ...';
        img := image(imageHanning);
        # reload the image into the viewer panel
        options_mdd  := mdd.getoptions();
        mdd.done();
        mdp.done();
        #
        mdp := dv.newdisplaypanel();
        mdd := dv.loaddata(img,'raster');
        mdd.setoptions(options_mdd);
        mdp.register(mdd);
      }
    }
  }

  #-------------------------------------------------------------------
  # Selection of the subimage turned the movie; for the (x,y) plane this can be
  # defined in terms of a rectangular bounding box, while in the z-direction this
  # is done by setting a range of frames.
 
  mapgui.ranges := SectionFrame (mapgui.frame,
                                 'Image axis ranges',
                                 T);

  mapgui.rangesX_frame := frame(mapgui.ranges.Contents,side='left',expand='x');
  mapgui.rangesX_label := label(mapgui.rangesX_frame,
                                text='Range of the x-axis :',
                                width=stdlength,
                                anchor='e');
  mapgui.rangesX_min   := entry(mapgui.rangesX_frame,width=5,background='LemonChiffon');
  mapgui.rangesX_to    := label(mapgui.rangesX_frame,' ... ');
  mapgui.rangesX_max   := entry(mapgui.rangesX_frame,width=5,background='LemonChiffon');

  mapgui.rangesY_frame := frame(mapgui.ranges.Contents,side='left',expand='x');
  mapgui.rangesY_label := label(mapgui.rangesY_frame,'Range of the y-axis :',width=stdlength,anchor='e');
  mapgui.rangesY_min   := entry(mapgui.rangesY_frame,width=5,background='LemonChiffon');
  mapgui.rangesY_to    := label(mapgui.rangesY_frame,' ... ');
  mapgui.rangesY_max   := entry(mapgui.rangesY_frame,width=5,background='LemonChiffon');

  # range of image frame used
  mapgui.rangesZ_frame := frame(mapgui.ranges.Contents,side='left',expand='x');
  mapgui.rangesZ_label := label(mapgui.rangesZ_frame,'Range of frames used :',width=stdlength,anchor='e');
  mapgui.rangesZ_min   := entry(mapgui.rangesZ_frame,width=5,background='LemonChiffon');
  mapgui.rangesZ_to    := label(mapgui.rangesZ_frame,' ... ');
  mapgui.rangesZ_max   := entry(mapgui.rangesZ_frame,width=5,background='LemonChiffon');
  
  # flag frames for exclusion from movie
  mapgui.flagZ_frame := frame(mapgui.ranges.Contents,side='left',expand='x');
  mapgui.flagZ_label := label(mapgui.flagZ_frame,'Exclude frames:',width=stdlength,anchor='e');
  mapgui.flagZ_range := entry(mapgui.flagZ_frame,width=12,background='LemonChiffon');

  mapgui.reverse_frame := frame(mapgui.ranges.Contents,side='left',expand='x');
  mapgui.reverse_label := label(mapgui.reverse_frame,'Frame order:',width=stdlength,anchor='e');
  mapgui.reverse_but   := button(mapgui.reverse_frame,'Reverse',type='check',anchor='w',fill='x',justify='left',width=5)

  
  mapgui.rangesX_min->insert(as_string(aipsimage.ranges[1,1]));
  mapgui.rangesX_max->insert(as_string(aipsimage.ranges[1,2]));
  mapgui.rangesY_min->insert(as_string(aipsimage.ranges[2,1]));
  mapgui.rangesY_max->insert(as_string(aipsimage.ranges[2,2]));
  mapgui.rangesZ_min->insert(as_string(aipsimage.ranges[3,1]));
  mapgui.rangesZ_max->insert(as_string(aipsimage.ranges[3,2]));
  mapgui.flagZ_range->insert(as_inputvec(aipsimage.flag));
  mapgui.reverse_but->state(F);

  stdlength := 24;

  #-------------------------------------------------------------------
  # Introduce additional frames to obtain smoother transitions in the
  # final movie? If yes, this can be attempted by various methods
  # (linear interpolation of pixel value in two subsequent frames,
  # fading one image into the other, morphing between subsequent frames)

  mapgui.processing := SectionFrame (mapgui.frame,
                                     'Processing of the image',
                                     T);

  mapgui.nofFrames := SectionEntry (mapgui.processing,
                                    'Additional frames : ',
                                    '0');

  # no additional frames
  mapgui.addframes_none := button(mapgui.nofFrames.Right,
                                  'No additional frames',
                                  type='radio',
                                  anchor='w',
                                  fill='x',
                                  justify='left',
                                  width=stdlength);
  mapgui.addframes_none->state(T);
  # interpolation: linear
  mapgui.addframes_linear := button(mapgui.nofFrames.Right,
                                    '... by linear interpolation',
                                    type='radio',
                                    anchor='w',
                                    fill='x',
                                    justify='left',
                                    width=stdlength);
  mapgui.addframes_linear->state(F);
  # interpolation: cosine slope
  mapgui.addframes_cosine := button(mapgui.nofFrames.Right,
                                    '... by cosine slope',
                                    type='radio',
                                    anchor='w',
                                    fill='x',
                                    justify='left',
                                    width=stdlength);
  mapgui.addframes_cosine->state(F);

  #-------------------------------------------------------------------
  # Create still images of the movies frames?

  mapgui.stills       := frame(mapgui.frame,side='left',expand='x',relief='groove');
  mapgui.stills_left  := frame(mapgui.stills,side='top',expand='x');
  mapgui.stills_right := frame(mapgui.stills,side='top',expand='x');
  # left subframe
  mapgui.stills_label := label(mapgui.stills_left,'Create still images',anchor='w',width=30);
  # right subframe
  mapgui.stills_gif := button(mapgui.stills_right,'GIF',type='check',anchor='w',fill='x',justify='left',width=stdlength)
  mapgui.stills_jpg := button(mapgui.stills_right,'JPG',type='check',anchor='w',fill='x',justify='left',width=stdlength)
  mapgui.stills_eps := button(mapgui.stills_right,'EPS',type='check',anchor='w',fill='x',justify='left',width=stdlength)
  mapgui.stills_ppm := button(mapgui.stills_right,'PPM',type='check',anchor='w',fill='x',justify='left',width=stdlength)
  mapgui.stills_tga := button(mapgui.stills_right,'TGA',type='check',anchor='w',fill='x',justify='left',width=stdlength)
  mapgui.stills_xpm := button(mapgui.stills_right,'XPM',type='check',anchor='w',fill='x',justify='left',width=stdlength)

  #
  # Actions to be performed after the conversion process.
  #
  mapgui.post       := frame(mapgui.frame,side='left',expand='x',relief='groove');
  mapgui.post_left  := frame(mapgui.post,side='top',expand='x');
  mapgui.post_right := frame(mapgui.post,side='top',expand='x');
  # left subframe
  mapgui.post_label := label(mapgui.post_left,'Post conversion actions',anchor='w',width=30);
  # right subframe
  mapgui.post_parfile := button(mapgui.post_right,'Keep parameter file',type='check',anchor='w',fill='x',justify='left',width=stdlength)
  mapgui.post_imageMovie := button(mapgui.post_right,'Keep movie image tool',type='check',anchor='w',fill='x',justify='left',width=stdlength)

  # ------------------------------------------------------------------
  # Control buttons

  mapgui.controls_frame := frame(mapgui.frame,side='left',
                                 expand='x',
                                 relief='groove');

  mapgui.controls_start := button(mapgui.controls_frame,
                                  text='Generate Movie',
                                  background='green');

  mapgui.controls_dismiss := button(mapgui.controls_frame,
                                    text='Dismiss',
                                    background='orange');

  # ------------------------------------------------------------------
  # Release the created widget

  tk_release();


  ### functions of the buttons #######################################

  whenever mapgui.rangesX_min -> return do {
    aipsimage.ranges[1,1] := as_integer(eval(mapgui.rangesX_min->get()));
    mdp.setzoom(aipsimage.ranges[1:2,1],aipsimage.ranges[1:2,2]);
  }

  whenever mapgui.rangesX_max -> return do {
    aipsimage.ranges[1,2] := as_integer(eval(mapgui.rangesX_max->get()));
    mdp.setzoom(aipsimage.ranges[1:2,1],aipsimage.ranges[1:2,2]);
  }

  whenever mapgui.rangesY_min -> return do {
    aipsimage.ranges[2,1] := as_integer(eval(mapgui.rangesY_min->get()));
    mdp.setzoom(aipsimage.ranges[1:2,1],aipsimage.ranges[1:2,2]);
  }

  whenever mapgui.rangesY_max -> return do {
    aipsimage.ranges[2,2] := as_integer(eval(mapgui.rangesY_max->get()));
    mdp.setzoom(aipsimage.ranges[1:2,1],aipsimage.ranges[1:2,2]);
  }

  whenever mapgui.rangesZ_min -> return do {
    aipsimage.ranges[3,1] := as_integer(eval(mapgui.rangesZ_min->get()));
  }

  whenever mapgui.rangesZ_max -> return do {
    aipsimage.ranges[3,2] := as_integer(eval(mapgui.rangesZ_max->get()));
  }

  whenever mapgui.flagZ_range -> return do {
    aipsimage.flag := as_integer(eval(mapgui.flagZ_range->get()));
    mapgui.flagZ_range->delete('start','end');
    mapgui.flagZ_range->insert(as_inputvec(aipsimage.flag));
    print "Flagging of frames : ",aipsimage.flag;
  }

  # ------------------------------------------------------------------
  # Start generation of movie from image

  whenever mapgui.controls_start -> press do {

    # name of the generated movie
    aipsimage.nameMovie := as_string(mapgui.moviename.Entry->get());

    # ----------------------------------------------------------------
    # Introduce additional frames for smoother playback?

    addframes.frames := as_integer(eval(mapgui.nofFrames.Entry->get()));
    addframes.none   := as_boolean(mapgui.addframes_none->state());
    addframes.linear := as_boolean(mapgui.addframes_linear->state());
    addframes.cosine := as_boolean(mapgui.addframes_cosine->state());

    # ----------------------------------------------------------------
    # Create still images of the movies frames?

    files.gif := as_boolean(mapgui.stills_gif->state());
    files.jpg := as_boolean(mapgui.stills_jpg->state());
    files.eps := as_boolean(mapgui.stills_eps->state());
    files.ppm := as_boolean(mapgui.stills_ppm->state());
    files.tga := as_boolean(mapgui.stills_tga->state());
    files.xpm := as_boolean(mapgui.stills_xpm->state());

    # parameter file
    files.parfile    := as_boolean(mapgui.post_parfile->state());
    files.imageMovie := as_boolean(mapgui.post_imageMovie->state());

    # Extract data cube from the image
    print "Extracting data cube from image ...";
    aipsimage.ranges[1,1] := as_integer(eval(mapgui.rangesX_min->get()));
    aipsimage.ranges[1,2] := as_integer(eval(mapgui.rangesX_max->get()));
    aipsimage.ranges[2,1] := as_integer(eval(mapgui.rangesY_min->get()));
    aipsimage.ranges[2,2] := as_integer(eval(mapgui.rangesY_max->get()));
    aipsimage.ranges[3,1] := as_integer(eval(mapgui.rangesZ_min->get()));
    aipsimage.ranges[3,2] := as_integer(eval(mapgui.rangesZ_max->get()));

    aipsimage.flag := as_integer(eval(mapgui.flagZ_range->get()));
    aipsimage.reverse :=  mapgui.reverse_but->state();

    if (!addframes.none) {
      addframes.flag:=aipsimage.flag
    }

    # ----------------------------------------------------------------
    # Get the image pixels

    chunkSTART := [aipsimage.ranges[,1]];
    chunkEND   := [aipsimage.ranges[,2]];
    aipsimage.pixels := img.getchunk(chunkSTART,chunkEND);
    aipsimage.shape := shape(aipsimage.pixels);
    aipsimage.cs := img.coordsys(); 

    # ----------------------------------------------------------------
    # save the options of the viewer display

    aipsimage.options_mdp  := mdp.getoptions();
    aipsimage.options_mdd  := mdd.getoptions();
    aipsimage.options_mcm  := mdp.canvasmanager().getoptions();
    aipsimage.paneldisplay := mdp.status().paneldisplay;

    # ----------------------------------------------------------------
    # call the function performing the conversion from image to movie

    moviefromimage (aipsimage, addframes, files);
  }

  #
  # Don't generate a movie and simply dismiss the GUI
  #
  whenever mapgui.controls_dismiss -> press do {

    mdd.done();
    mdp.done();
    img.done();

    val mapgui.frame:=F;
    val mapgui:=F;    

  }

}


#-------------------------------------------------------------------------------
#Section: Processing
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: applyHanningFilter
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Convolve one axis of image with a Hanning kernel 
#Par:  imageName    - Filename of the input image.
#Par:  axis         - Axis to convolve.
#Ret:  imageHanning - Filename of the image resulting from the filtering.
#Created: 2005/02/15 (Lars B&auml;hren)
#-------------------------------------------------------------------------------

applyHanningFilter := function (imageName='', axis=3) {

  if (imageName != '' || !dms.fileexists(imageName)) {

    print '[applyHanningFilter] No valid input image provided!'

  } else {

    imageHanning := spaste(imageName,'.hann');

    img := image(imageName);

    if (!is_image(img)) {
      print '[applyHanningFilter] Failed loading input image!';
    } else {

      im2 := img.hanning(outfile=imageHanning,
                         axis=naxis, 
                         overwrite=T);

      if (!is_image(im2)) {
        print '[applyHanningFilter] Failed creating filtered image!';
      }
    }
  }

  return imageHanning;
}

#-------------------------------------------------------------------------------
#Name: write_parfile
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Create a parameter file which can be used by either 'mpeg_encode' or
#      'ppmtompeg' to create an MPEG movie from a set of images in PPM format.
#Par:  dirname   - Directory containing the movie frames.
#Par:  moviename - Name of the created movie file.
#Par:  frange[]  - The range of frames which are to be included into the movie.
#-------------------------------------------------------------------------------

write_parfile := function (dirname='movie-frames-PPM',
                           moviename='movie.mpg',
                           frange=[1,1])
{

  msh := sh();

  #
  # create a parameter file for ppmtompeg
  #
  msh.command('echo "PATTERN IBBPBBPBBPBBPBBP" > mpeg.par');
  msh.command(spaste('echo "OUTPUT ',moviename,'" >> mpeg.par'));
  msh.command('echo "BASE_FILE_FORMAT PPM" >> mpeg.par');
  msh.command('echo "YUV_SIZE 352x240" >> mpeg.par');
  msh.command('echo "INPUT_CONVERT *" >> mpeg.par');
  msh.command('echo "GOP_SIZE 16" >> mpeg.par');
  msh.command('echo "SLICES_PER_FRAME 1" >> mpeg.par');
  msh.command(spaste('echo "INPUT_DIR ',dirname,'" >> mpeg.par'));
  msh.command('echo "INPUT" >> mpeg.par');
  msh.command(spaste('echo "frame-*.ppm [',frange[1],'-',frange[2],']" >> mpeg.par'));
  msh.command('echo "END_INPUT" >> mpeg.par');
  msh.command('echo "PIXEL HALF" >> mpeg.par');
  msh.command('echo "RANGE 10" >> mpeg.par');
  msh.command('echo "PSEARCH_ALG EXHAUSTIVE" >> mpeg.par');
  msh.command('echo "BSEARCH_ALG CROSS2" >> mpeg.par');
  msh.command('echo "IQSCALE 8" >> mpeg.par');
  msh.command('echo "PQSCALE 10" >> mpeg.par');
  msh.command('echo "BQSCALE 25" >> mpeg.par');
  msh.command('echo "REFERENCE_FRAME ORIGINAL" >> mpeg.par');
  msh.command('echo "BIT_RATE 1000000" >> mpeg.par');
  msh.command('echo "BUFFER_SIZE 327680" >> mpeg.par');
  msh.command('echo "FRAME_RATE 24" >> mpeg.par');

}


#-------------------------------------------------------------------------------
#Name: moviefromimage
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Create a MPEG movie from a sequence of frames extracted from an AIPS++
#      image tool. The conversion is performed in two steps: 
#      (1) using the animator control of the viewer GUI we step through a range
#          of image frame which are written to disk as PPM
#      (2) the sequence of PPM image is merged into a MPEG movie by utilizing
#          'convert'
#Par: aipsimage - reference to the record holding file names, image
#                 characteristics and image pixels
#Par: addframes - record with the settings for introducing additional frames
#                 for smoother playback
#Par: files     - record with the settings for creating/keeping still images
#                 in various formats
#-------------------------------------------------------------------------------

moviefromimage := function(aipsimage,addframes,files){

  # ------------------------------------------------------------------
  # Create directories for storing the frame snapshots in the various
  # output formats

  msh := sh();
  msh.command('mkdir movie-frames-XPM');
  msh.command('mkdir movie-frames-PPM');

  if (files.eps) {
    msh.command('mkdir movie-frames-EPS');
  }
  if (files.gif) {
    msh.command('mkdir movie-frames-GIF');
  }
  if (files.jpg) {
    msh.command('mkdir movie-frames-JPG');
  }
  if (files.tga) {
    msh.command('mkdir movie-frames-TGA');
  }

  ### report settings ##########################################################

  frames_final := (addframes.frames+1)*(aipsimage.ranges[3,2]-aipsimage.ranges[3,1]+1)

  print "----------------------------------------------------------------------";
  print "moviefromimage() : starting up ...";
  print "----------------------------------------------------------------------";
  print "Name of the movie ............. : ",aipsimage.nameMovie;
  print "Shape of the input image ...... : ",aipsimage.shape;
  print "Image frames used for the movie : ",aipsimage.ranges[3,];
  print "Add interpolation frames ...... : ",addframes;
  print "Number of movie frames ........ : ",frames_final;
  print "Flagged image frames .......... : ",aipsimage.flag;
  print "Reverse order of frames ....... : ",aipsimage.reverse;
  print "Still image formats ........... : ",files;
  print "----------------------------------------------------------------------";

  ### create a new image with the additional frames included ###################

  shape1 := aipsimage.shape;
  shape2 := aipsimage.shape;

  if (!addframes.none) {

    # set up the record used for communication with the client program
    recClient := [=];
    recClient.pixels := aipsimage.pixels;
    recClient.shape  := aipsimage.shape;
    recClient.frames := addframes.frames;
    recClient.linear := addframes.linear;
    recClient.cosine := addframes.cosine;
    recClient.flag   := addframes.flag;

    # run the client programm to perform the actual computation of the frames
    MovieMakerClient  := client(spaste(globalconst.LOPESBIND,'/moviemaker'));
    aipsimage.pixels := MovieMakerClient->interpolateFrames(recClient);
    shape2  := shape(aipsimage.pixels);
  }

  ### create a new image tool ##################################################

  print "Creating a new image tool ...";
  cs:=aipsimage.cs
  inc:=cs.increment(); 
  cs.summary();
  print shape1,shape2,(shape1/shape2);
  inc:=inc*(shape1/shape2);
  cs.setincrement(inc);
  newimg := imagefromarray('movie.img',
                           pixels=as_float(aipsimage.pixels),
                           csys=cs,
                           overwrite=T);
  newimg.summary();

  ### create a viewer tool #####################################################
 
  if (is_image(newimg)) {

    # ---------------------------------------------------------------
    # Load the image into a displaypanel and reload the settings
    # adjusted before
 
    mdp := dv.newdisplaypanel();
    mdd := dv.loaddata(newimg,'raster');

    mdd.setoptions(aipsimage.options_mdd);
    mdp.register(mdd);

    mdp.setzoom(blc=aipsimage.paneldisplay.linearblc, 
                trc=aipsimage.paneldisplay.lineartrc)

    mdp.canvasmanager().setoptions(aipsimage.options_mcm);

    # ---------------------------------------------------------------
    # Get access to the animator tool    

    mda := mdp.animator();

    nframe:=0;
    if (!addframes.none) {framelist:=seq(shape2[3])} 
    else {framelist:=exclude(aipsimage.flag,seq(shape2[3]))};
    if (aipsimage.reverse) {framelist:=framelist[seq(len(framelist),1)]};

    bar := progress (0, len(framelist), 'Exporting image frames ...',)
    bar.activate()   

    for (frame in framelist) {
      nframe+:=1;
      mda.goto(frame);
      mdp.canvasprintmanager().writexpm(spaste('movie-frames-XPM/frame-',nframe,'.xpm'));
      # XPM -> PPM
      msh.command(spaste('convert movie-frames-XPM/frame-',nframe,'.xpm movie-frames-PPM/frame-',nframe,'.ppm'));
      # keep track of the additional image formats
      if (files.eps) {
        mdp.canvasprintmanager().writeps(spaste('movie-frames-EPS/frame-',nframe,'.eps'),eps=T,dpi=300);
      }
      if (files.gif) {
        msh.command(spaste('convert movie-frames-XPM/frame-',nframe,'.xpm movie-frames-GIF/frame-',nframe,'.gif'));
      }
      if (files.jpg) msh.command(spaste('convert movie-frames-XPM/frame-',nframe,'.xpm movie-frames-JPG/frame-',nframe,'.jpg'));
      if (files.tga) msh.command(spaste('convert movie-frames-XPM/frame-',nframe,'.xpm movie-frames-TGA/frame-',nframe,'.tga'));
      # show progress
      bar.update(nframe);
    }
    bar.done();

    # ----------------------------------------------------------------
    # Clean up (1): release memory allocated for image tool and viewer
    
    mdd.done()
    mdp.done();
    newimg.done();

  } else {
    print "Sorry, no valid image tool provided to the viewer.";
  }

  ### convert the single images into a movie ###################################

  # check if the required software tools are available on the system
  print "Checking system for conversion software tools ...";
  convert := msh.command('which convert');
  ppmtompeg := msh.command('which ppmtompeg');
  mpeg_encode := msh.command('which mpeg_encode');

  print "Creating the movie from the individual images ...";
  if (!ppmtompeg.status) {
    # create the parameter file 
    write_parfile(dirname='movie-frames-PPM',moviename=aipsimage.nameMovie,
                  frange=[1,nframe]);
    # run ppmtompeg
    print "Running ppmtompeg to convert the images into a movie ...";
    ppmtompeg := msh.command('ppmtompeg mpeg.par');
    # clean up
    if (ppmtompeg) {
      print "ppmtompeg finished with non-zero status: ",ppmtompeg.status;
    } else {
      print "ppmtompeg finished successfully."
    }
  } else if (!mpeg_encode.status) {
    # create the parameter file 
    write_parfile(dirname='movie-frames-PPM',moviename=aipsimage.nameMovie,
                  frange=[1,nframe]);
    # run mpeg_encode
    print "Running mpeg_encode to convert the images into a movie ...";
    mpeg_encode := msh.command('mpeg_encode mpeg.par');
    # clean up
    if (mpeg_encode) {
      print "mpeg_encode finished with non-zero status: ",mpeg_encode.status;
    } else {
      print "mpeg_encode finished successfully."
    }
  }  else if (!convert.status) {
    msh.command(spaste('convert movie-frames-PPM/*.ppm ',aipsimage.nameMovie,' 2> /dev/null'));
  } else {
    print "Sorry, could not find any program to convert frames to movie."
  } 

  # ------------------------------------------------------------------
  # Adjust the permissions to the created files

  msh.command('chmod -R a+r movie.img');
  msh.command(spaste('chmod a+r ',moviename));
  msh.command('chmod a+r mpeg.par');

  # ------------------------------------------------------------------
  # Clean up (2) : Remove disk files of the movie frames.

  print "Cleaning up temporary directories ..."

  if (!files.parfile)  msh.command('rm mpeg.par');
  if (!files.imageMovie)  msh.command('rm -rf movie.img');
  if (!files.ppm) msh.command('rm -rf movie-frames-PPM');
  if (!files.xpm) msh.command('rm -rf movie-frames-XPM');

  ###

  print "moviefromimage() : done.";
  print "----------------------------------------------------------------------";

}


#-------------------------------------------------------------------------------
#Section: Testing area
#-------------------------------------------------------------------------------

tViewerOptions := function () 
{
  include 'image.g'
  include 'viewer.g'

  # ------------------------------------------------------------------
  # Load image tool and display it

  img := image('subim.img');

  # ------------------------------------------------------------------
  # Set up viewer to display the image

  # ------------------------------------------------------------------
  # make some changes

  mdp := dv.newdisplaypanel();
  mdd := dv.loaddata(img,'raster');
  mdd.setoptions([axislabelswitch=T,
                  titletext='Input AIPS++ image tool',
                  pixeltreatment=['center'],
                  xgridtype=['Tick marks'],
                  ygridtype=['Tick marks'],
                  colormap=['Rainbow 2']
                 ]);
  mdp.register(mdd);

  # ------------------------------------------------------------------
  # Store the options

  options := [=];

  options.mdp := mdp.getoptions();
  options.mdd := mdd.getoptions();
  options.mcm := mdp.canvasmanager().getoptions();

  options.paneldisplay:= mdp.status().paneldisplay;

  # ------------------------------------------------------------------
  # destory the display panel tool

  mdp.done();

  # ------------------------------------------------------------------
  # create a new display panel tool

  mdp := dv.newdisplaypanel();
  mdd := dv.loaddata(img,'raster');

  mdd.setoptions(options.mdd);
  mdp.register(mdd);

  mdp.setzoom(blc=options.paneldisplay.linearblc, 
              trc=options.paneldisplay.lineartrc)

  mdp.canvasmanager().setoptions(options.mcm);
}
