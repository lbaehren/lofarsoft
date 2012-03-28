.. _new:

**********
What's New
**********

Version 1.1 (2012/03/28):

    * Modified calculation of rms_box parameter (when rms_box option is None) to work better with fields composed mainly of point sources when strong artifacts are present. * Modified fitting of large islands to adopt an iterative fitting scheme that limits the number of Gaussians fit simultaneously per iteration to 10. This change speeds up fitting of large islands considerably. * Tweaked settings that affect fitting of Gaussians to improve fitting in general.

    * Added output of shapelet parameters to FITS tables. Fixed issue with resizing of sources in spectral index module.

    * Fixed bugs in polarisation module that caused incorrect polarization fractions.
    
    * Improved plotting speed (by factor of ~ 4) in show_fit when there is a large number of islands.
    
    * Simplified the spectral index module to make it more user friendly and stable.
    
    * Added the option to use a "detection" image for island detection (the detection_image option); source properties are still measured from the main input image.
    
    * Implemented fitting of Gaussians in polarisation module, instead of simple summation of pixel values, to determine polarized fluxes.
    
    * In scripts, process_image() will now accept a dictionary of parameters as input.
    
    * Sources that appear only in Stokes Q or U (and hence not in Stokes I) are now identified and included in the polarisation module. This identification is done using the polarized intensity (PI) image.
    
    * Altered reading of images to correctly handle 4D cubes. Fixed bug in readimage that affected filenames.
    
    * Extended psf_vary module to include fitting of stacked PSFs with Gaussians, interpolation of the resulting parameters across the image, and correction of the de- convolved source sizes using the interpolated PSFs. Changed plotting of Gaussians in show_fit() to use the FWHM instead of sigma. * Added residual rms and mean values to sources and source list catalogs. These values can be compared to background rms and mean values as a quick check of fit quality.


See the changelog (accessible from the interactive shell using ``help changelog``) for details of all changes since the last version.