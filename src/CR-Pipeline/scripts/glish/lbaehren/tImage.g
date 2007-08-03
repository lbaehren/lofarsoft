#-------------------------------------------------------------------------------
# Abstract:
#  Glish script for demonstration of bug in (a) the display in the AIPS++ 
#  viewer and (b) the export to FITS, even though a valid AIPS++ image tool
#  was created
#
# Description:
#  It is found that a previously created AIPS++ image tool can not be
#  (a) displayed correctly using the AIPS++ viewer and (b) exported to FITS, if
#  at least one of the corners of the display widget cannot be mapped onto 
#  valid (celestial) world coordinates. This problem is hardly ever encountered
#  for the case of astronomical images covering only a small area on the 
#  celestial sphere, but frequently run into when aiming at the creation and
#  display of all-sky images.
#  In order to demostrate problem (a) the script below creates images for
#  combinations of pixels sizes and spherical-to-cartesian projections; while
#  for the first example for each projection all pixels can be transformed to
#  valied world coordinates, in the second case this is only possible for a
#  subset of projections.
#  In order to demonstrate problem (b) we use the 'tofits()' method to export
#  the successfully created AIPS++ image to to FITS. This will fail under the
#  same conditions as in (a).
#
# Sidenote:
#  Besides the fact that export to FITS fails under the circumstances described
#  above, furthermore the created FITS contains a wrong value for the CTYPEi 
#  keyword when using 'AZEL' coordinates - as indicated by the warning message
#   'WARN: AIT is not known to standard FITS (it is known to WCS).'
#  This causes a failure to properly import the created FITS files into
#  other astronomical software (e.g. DS9, skycat), since the CTYPEi is not 
#  recognized and therefore a simple linear coordinate system is used.
#
# Author: 
#  Lars Baehren (bahren@astron.nl)
#
# Date:
#  2005/01/14
#-------------------------------------------------------------------------------

refcodes := ['J2000','GALACTIC','HADEC','AZEL'];
projections := ['STG','SIN','CAR','AIT'];
pixels := [10,250];

nTest := 0;
nFail := 0;
ok    := T;

for (refcode in refcodes) {

  for (projection in projections) {

    for (nofPixels in pixels) {

      nTest +:= 1;

      #-----------------------------------------------------
      # Set up an array of pixel values
      #-----------------------------------------------------      

      a := array(0.0,nofPixels,nofPixels);

      for (i in 1:nofPixels) {
	for (j in 1:nofPixels) a[i,j] := i+j; 
      }

      #-----------------------------------------------------      
      # Create a coordinate system tool; the reference pixel
      # is chosen such that the fiducial point is at the
      # center of the image.
      #-----------------------------------------------------      

      cs := coordsys(direction=T);
      cs.setreferencecode(type='Direction', value=refcode);
      cs.setprojection(type=projection,parameters=[]);
      cs.setunits(['deg','deg'],overwrite=T);
      cs.setreferencepixel([nofPixels/2,nofPixels/2]);
      cs.setincrement([-1,1]);
      if (projection == 'STG' | projection == 'SIN') {
        cs.setreferencevalue([0,90]);
      } else {
        cs.setreferencevalue([0,0]);
      }
      cs.summary()

      #-----------------------------------------------------      
      # Create an image tool from the pixel array and the
      # coordinate system tool
      #-----------------------------------------------------      

      filenameBase := spaste('testimage-',refcode,'-',projection,'-',nofPixels);
      filenameImg := spaste(filenameBase,'.img');
      filenameFITS := spaste(filenameBase,'.fits');

      img := imagefromarray(outfile=filenameImg,pixels=a,csys=cs,overwrite=T);

      if (is_image(img)) {
	if (is_fail(img.tofits(filenameFITS,overwrite=T))) {
          print '------------------------------------------------------------'
          print 'Warning: error writing FITS file ',filenameFITS;
          print '------------------------------------------------------------'
          ok:=F;
        } 
	if (is_fail(img.view(axislabels=T))) {
          print '------------------------------------------------------------'
          print 'Warning: unable to correctly display created image';
          print '------------------------------------------------------------'
          ok:=F;
        }
	img.done();
      } else {
	print 'Warning: no valid image tool created';
        ok:=F;
      }

      if (!ok) nFail +:= 1;
      ok := T;

    }

  }

}

print ''
print nFail,'of',nTest,'tests contained failure of some kind.'