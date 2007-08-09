
#-------------------------------------------------------------------------------
# $Id: tSkymapper.g,v 1.1 2006/03/06 15:34:52 horneff Exp $
#-------------------------------------------------------------------------------
#
# Test suite for the <a href="Skymapper.doc.html">Skymapper</a>.<br>
# While a set of low-level tests should be implemented at C/C++ level, this
# collection of tests aims at testing the functionality of the Skymapper as
# a tool available from the Glish side of the LOPES-Tools.
# <ol>
#   <li><a href="#test">Test routines</a>
#   <li><a href="#help">Helper routines</a>
# </ol>


#Section: <a name="test">Test routines</a>


#-------------------------------------------------------------------------------
#Name: testSkymapper
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  A collection of tests to be run on the Skymapper.
#Par:  rec    - Reference to the data structure.
#Par:  dotest - List of tests to perform.
#Created: 2004/12/23 (Lars Baehren)
#Status:  experimental
#Example: testSkymapper (data, dotest=[1,2])
#-------------------------------------------------------------------------------

testSkymapper := function (ref rec, dotest=[1]) 
{

  projections := ['STG','SIN','TAN','ZEA','CAR','CEA','AIT'];
  refcodes    := ['AZEL','J2000','GALACTIC','B1950'];

  print '======================================================================';
  print '[tSkymapper::testSkymapper] Testing projections ...'
  print ' - Projections ......... :',projections;
  print ' - Celestial coordinates :',refcodes;
  print '======================================================================';

  for (index in dotest) {

    if (index == 1) {
      ok := testProjections(rec,projections);
    }

    if (index == 2) {
      print '[tSkymapper::testSkymapper] Running test 2'
    }

    if (index == 3) {
      print '[tSkymapper::testSkymapper] Running test 3'
    }

  } 

}

#-------------------------------------------------------------------------------
#Name: testProjections
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Compute a skymap for various spherical-to-cartesian projections.
#Par:  rec         - Reference to the data structure.
#Par:  projections - List of projections for which to run the tests
#Created: 2004/12/23 (Lars Baehren)
#Status:  developement
#-------------------------------------------------------------------------------

testProjections := function (ref rec,
                             projections=['STG','SIN','CAR','AIT'])
{

  for (proj in projections) {

    if (proj != 'STG' || proj != 'SIN' || proj != 'ZEA') {
      center := [0,45];
    } else {
      center := [0,90];
    }

    for (pixels in [[1:3]*40]) {

      print '[tSkymapper::testProjections]';
      print ' - Projection   :',proj;
      print ' - Image pixels :',pixels,'x',pixels;

      skymapper(rec,
                paramAntenna=[index=-1,refant=-1,doz=F,xy=F],
                mapTitle='Find Distance to Point Source',
                mapCenterCoords=center,
                mapCenterWCS='AZEL',
                mapProjection=proj,
                incrCSC=[2,2],
                rangeElevation=[0,90],
                mapPixels=[pixels,pixels],
                nofIntegrations=1,
                nofBlocks=1,
                nofSamples=1024,
                firstBlock=1,
                SignalQuantity='power',
                SignalDomain='freq',
                channels=1); 
    }

    # check the generated image

    if (is_image(imageTool)) {
      imageTool.view(axislabels=T);
    } else {
      print '[tSkymapper::testProjections] No valid image tool available.';
    }

  }

  return T;

}


#-------------------------------------------------------------------------------
#Name: testExportFITS
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Test the 'tofits()' method of AIPS++ image for various combinations of
#      image sizes, coordinate systems and spherical-to-cartesian projections.
#Par:  rec         - Reference to the data structure.
#Created: 2004/12/23 (Lars Baehren)
#Status:  developement
#-------------------------------------------------------------------------------

testExportFITS := function (ref rec) 
{

  for (nofPixels in [[1:5]*60]) {

    a := array(0.0,nofPixels,nofPixels);

    print '[tSkymapper::testExportFITS] Creating image of shape',shape(a),'...';

    # set up a pixel array

    for (i in 1:nofPixels) {
      for (j in 1:nofPixels) a[i,j] := i+j; 
    }

    # create coordinate system tool

    cs := coordsys(direction=T);
    cs.setreferencecode(type='Direction', value='GALACTIC');
    cs.setprojection(type='TAN',parameters=[]);
    cs.setunits(['deg','deg'],overwrite=T);
    cs.setreferencepixel([nofPixels/2,nofPixels/2]);
    cs.setreferencevalue([0,90]);
    cs.setincrement([-1,1]);
    cs.summary();

    # create image tool from pixel array

    img := imagefromarray(pixels=a,csys=cs,overwrite=T);
   
    # mask the image pixels, for which the cartesian-to-spherical
    # deprojection returns invalid values

    mask := array(T,nofPixels,nofPixels);

    for (i in 1:nofPixels) {
      for (j in 1:nofPixels) {
        if (is_fail(cs.toworld([i,j]))) mask[i,j] := F;
      }
    }    

    img.putregion(pixelmask=mask);

    # display the created image

    if (is_image(img)) {
      print ' - Successfully created image tool!';
      print ' - Shape ............. :',img.shape();
      print ' - Projection ........ :',cs.projection();
      print ' - Direction reference :',cs.referencecode();
      print ' - Axis names ........ :',cs.names();
      print ' - Valid image pixels  :',sum(mask),'/',nofPixels^2;
      img.view(axislabels=T);
      img.tofits(spaste(cs.projection().type,'-',
                        cs.referencecode(),'-',
                        nofPixels,'.fits'),overwrite=T);
    }
    else if (is_fail(img)) print 'Failed creating valid image tool!';
    else {}
    
  }  
}


#-------------------------------------------------------------------------------
#Name: test_locateTransmitters
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Test for the 'locateTransmitters()' function in 'Skymapper.g'
#Par:  rec      - Reference to the data structure.
#Par:  dataset  - The dataset to load for performing the test; the variables
#                 thus should hold the fully specified path to the directory
#                 holding the data.
#Created: 2005/03/30 (Lars B&auml;hren)
#-------------------------------------------------------------------------------

test_locateTransmitters := function (ref rec, dataset='') {

  projections := ["STG","CAR","CSC"];

  if (dataset == '') {
    dataset := '/data/ITS/exampledata.its';
  }

  for (n in seq(len(projections))) {
    locateTransmitters (rec, 
                        dataset,
                        blocksize=1024,
                        projection=projections[n]);
  }

}

#-------------------------------------------------------------------------------
#Name: test_SkymapGrid
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Test script for the functionality to be implemented in the SkymapGrid
#      C++ code. The purpose of this script is to test the steps required to
#      properly set up a coordinate system tool to be used along with an image.
#Par:  telescope        -- Name of the telescope; the position will be looked
#                          up in the data repository.
#Par:  mapReferencecode -- Reference code for the celestial coordinate
#                          reference frame.
#Par:  mapProjection    -- Reference code for the spherical-to-cartesian map
#                          projection.
#Par:  mapPixels        -- The shape of the pixel map.
#Par:  mapCenter        -- The position of the map center, given in coordinates
#                          of the map reference code.
#Par:  mapIncrement     -- Coordinate increment.
#Par:  mapOrientation   -- Orientation of the vertical and horizontal pixel
#                          axes.
#Created: 2005/04/05 (Lars B&auml;hren)
#-------------------------------------------------------------------------------

test_SkymapGrid := function (telescope='WSRT',
                             mapReferencecode='GALACTIC',
                             mapProjection='AIT',
														 mapPixels=[100,50],
														 mapCenter=[45,45],
														 mapIncrement=[1,1],
														 mapOrientation=['North','East']) 
{
  epoch := dm.epoch('utc','today');
  

  centerPixel := mapPixels/2.0+1;

  # ------------------------------------------------------------------
  # Set up the pixel values with a marker at the center

  pixels := array([1:mapPixels[1]*mapPixels[2]],mapPixels[1],mapPixels[2]);

  pixels[centerPixel[1],] := mapPixels[1]*mapPixels[2];
  pixels[,centerPixel[2]] := mapPixels[1]*mapPixels[2];

  pixels[centerPixel[1]+5,centerPixel[2]+5] := mapPixels[1]*mapPixels[2];

  # ------------------------------------------------------------------
  # Set up the coordinate increment according to the chosen map
  # orientation. 

  if (mapProjection == "STG" || mapProjection == "SIN") {
    projectionType := "zenithal";
    crval := [0,90];
  } else if (mapProjection == "CAR" || mapProjection == "MER" || mapProjection == "AIT") {
    projectionType := "cylindrical";
    crval := [0,0];
  }

  if (mapOrientation[1] == "North") {
    northIsUp := T;
  } else {
    northIsUp := F;
  }

  if (mapOrientation[2] == "East") {
    eastIsLeft := T;
  } else {
    eastIsLeft := F;
  }

  cdelt := array(1,2);

  if (eastIsLeft && northIsUp) {        ## North / East ##
    if (projectionType == "zenithal") {
      cdelt := [1,1];
    }
    else if (projectionType == "cylindrical") {
      cdelt := [1,1];
    }
  }
  else if (!eastIsLeft && northIsUp) {  ## North / West ##
    if (projectionType == "zenithal") {
      cdelt := [-1,1];
    }
    else if (projectionType == "cylindrical") {
      cdelt := [-1,1];
    }
  }
  else if (eastIsLeft && !northIsUp) {  ## South / East ##
    if (projectionType == "zenithal") {
      cdelt := [1,-1];
    }
    else if (projectionType == "cylindrical") {
      cdelt := [1,-1];
    }
  }
  else if (!eastIsLeft && !northIsUp) { ## South / West ##
    if (projectionType == "zenithal") {
      cdelt := [-1,-1];
    }
    else if (projectionType == "cylindrical") {
      cdelt := [-1,-1];
    }
  } 

  increment := [spaste(cdelt[1]*mapIncrement[1],'deg'),
                spaste(cdelt[2]*mapIncrement[2],'deg')]

  # ------------------------------------------------------------------
  # Create coordinate system tool

  cs := coordsys (direction=T);
  
  cs.settelescope(telescope);

  cs.setincrement (value=increment, type='direction');
  cs.setprojection (type=as_string(mapProjection));
  cs.setreferencelocation(pixel=centerPixel,
                          world=[spaste(crval[1],'deg'),
                                 spaste(crval[2],'deg')]);
  cs.setreferencecode(as_string(mapReferencecode),'dir');

  # ------------------------------------------------------------------
  # Adjust the value of the reference pixel; this is done by determining
	# the offset between the map center position transformed to pixel 
	# coordinates and the central pixel.

  if (mapCenter == crval ) {
    print '[*] No shift of reference pixel required.';
  }
  else {
    print '[*] Shifting reference pixel ...';

    w := [spaste(mapCenter[1],'deg'),spaste(mapCenter[2],'deg')]
    centerWCS2PIX := cs.topixel(w);
    offset := centerWCS2PIX - centerPixel;

    centerWCS2PIX[1] -:= 2*offset[1];

    if (northIsUp) {
      centerWCS2PIX[2] -:= 2*offset[2];
    } else {
      centerWCS2PIX[2] -:= 2*offset[2];
    }

    cs.setreferencepixel (centerWCS2PIX);
  }

  # ------------------------------------------------------------------
  # Create an image

  filenameBase := spaste (mapReferencecode,'-',
                          mapProjection,'-',
                          mapOrientation[1],'-',
                          mapOrientation[2],'-',
                          cs.epoch().m0.value,'.');

  filenameImage := spaste (filenameBase,'img');
  filenameFits := spaste (filenameBase,'fits');
  filenameData := spaste (filenameBase,'data');

  img := imagefromarray (outfile=filenameImage,
                         pixels=pixels,
                         csys=cs,
                         overwrite=T);
  img.view(axislabels=T);

  img.tofits(outfile=filenameFits,overwrite=T);

  # ------------------------------------------------------------------
  # Export the grid coordinates

  exportGrid (cs, mapPixels=mapPixels, filename=filenameData);

  # ------------------------------------------------------------------
  # Provide some feedback

  print '-------------------------------------------------------------'
  print '[test_SkymapGrid]'
  print '';
  print ' - Map reference code ....... : ',mapReferencecode;
  print ' - Map spherical projection . : ',mapProjection;
  print ' - Map pixel shape .......... : ',mapPixels;
  print ' - Map center coordinates ... : ',mapCenter;
  print ' - Map coordinate increment . : ',mapIncrement;
  print ' - North is up .............. : ',northIsUp;
  print ' - East is left ............. : ',eastIsLeft;
  print ' - Map orientation correction : ',cdelt;
  print '';
  print ' - Coordinate system tool:';
  print ' -- Epoch .................. :',cs.epoch();
  print ' -- Coordinate types ....... :',cs.coordinatetype();
  print ' -- Coordinate axis names .. :',cs.names();
  print ' -- WCS projection code .... :',cs.projection();
  print ' -- WCS coordinate increment :',cs.increment('s');
  print ' -- WCS center coordinates . :',cs.toworld(centerPixel,'s');
  print ' -- WCS center pixel ....... :',cs.topixel(w);
  print '-------------------------------------------------------------'

}


#Section: <a name="help">Helper routines</a>


#-------------------------------------------------------------------------------
#Name: exportGrid
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Export the grid node positions for a combination of pixel array and a
#      coordinate system tool; the regular grid in pixel coordinates will be 
#      transformed into a set of world coordinates, which e.g. could be passed 
#      to a beamforming routine.
#Par:  csys      -- A coordinate system tool.
#Par:  mapPixels -- The shape of the pixel grid.
#Par:  filename  -- The name of the disk file, to which the output will be
#                   written.
#Created: 2005/04/06 (Lars B&auml;hren)
#-------------------------------------------------------------------------------

exportGrid := function (csys, mapPixels=[100,100], filename="") {

  print '';
  print '[exportGrid]';

  nofPoints := mapPixels[1]*mapPixels[2];
  refcode   := csys.referencecode();

  directions := [=];
  mask := array(T,nofPoints);

  referencecode := csys.referencecode();
  projection    := csys.projection();
  epoch         := csys.epoch()
  dm.doframe(epoch);
  dm.doframe(dm.observatory(csys.telescope()));
  dq.setformat('long','deg');

  if (filename == "") {
    filename := spaste(referencecode,'-',projection.type,'-',epoch.m0.value,'.data');
  }

  shell (spaste('echo "" > ',filename));
  numPixel := 0;

  print ' - Exporting grid points ...';
  for (nx in seq(mapPixels[1])) {
    for (ny in seq(mapPixels[2])) {
      numPixel +:= 1;
      w := csys.toworld ([nx,ny],'m').direction;
      if (is_fail(w)) {
        mask[numPixel] := F;
      } else {
        azel := dm.measure(w,'AZEL');
        #
        directions[refcode][1] := dq.unit(dq.angle(dm.getvalue(w)[1])).value;
        directions[refcode][2] := dq.unit(dq.angle(dm.getvalue(w)[2])).value;
        directions['AZEL'][1]   := dq.unit(dq.angle(dm.getvalue(azel)[1])).value;
        directions['AZEL'][2]   := dq.unit(dq.angle(dm.getvalue(azel)[2])).value;
        #
        if (directions['AZEL'][2] < 0) { 
          mask[numPixel] := F;
        } else {
          shell (spaste('echo ',nx,'  ',ny,
                        '  ',directions[refcode][1],'  ',directions[refcode][2],
                        '  ',directions['AZEL'][1],'  ',directions['AZEL'][2],
                        ' >> ',filename));
        }
      }
    }
    shell (spaste('echo "" >> ',filename));
  }
  print ' -- Done.';

  print ' -',sum(mask),'/',nofPoints,'grid points valid.';
}
