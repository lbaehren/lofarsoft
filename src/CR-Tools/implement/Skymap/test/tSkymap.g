# ------------------------------------------------------------------------------
# Create a test image from an array shape.
# ------------------------------------------------------------------------------
# $Id: tSkymap.g,v 1.2 2005/08/03 10:14:15 bahren Exp $

include 'image.g';
include 'misc.g';
include 'viewer.g';

imageShape := [100,100,10,20,10];

units := ['deg','deg','m','s','Hz'];
cdelt := [0.5,0.5,100.0,0.1,1.0];

# ------------------------------------------------------------------------------

print ''
print '[1] Create initial coordinate system tool.';
print ''

cs := coordsys(direction=T,linear=2,spectral=T);

cs.summary();

# ------------------------------------------------------------------------------

print ''
print '[2] Reorder the image axes';
print ''

cs.reorder([1,3,2]);
cs.setnames(value="Longitude Latitude Distance Time Frequency");

cs.summary();

# ------------------------------------------------------------------------------

print ''
print '[3] Set axis world units';
print ''

cs.setunits(type='Direction',
            value=[as_string(units[1]),as_string(units[2])],  
            overwrite=T);

cs.setunits(type='Linear',
            value=[as_string(units[3]),as_string(units[4])],  
            overwrite=T);

cs.setunits(type='Spectral',
            value=as_string(units[5]),  
            overwrite=T);

cs.summary();

# ------------------------------------------------------------------------------

print ''
print '[4] Set coordinate increment';
print ''

cs.setincrement(value=cdelt);

cs.summary();

# ------------------------------------------------------------------------------

print ''
print '[5] Set reference pixel';
print ''

cs.setreferencepixel (value=(imageShape/2));

cs.summary();

# ------------------------------------------------------------------------------

print ''
print '[6] Create image';
print ''

img := imagefromshape('testimage.img',
                      shape=imageShape, 
                      csys=cs,
                      overwrite=T);

img.summary();

img.view(axislabels=T);

# ------------------------------------------------------------------------------

print ''
print '[7] Creating custom display for image';
print ''

# ------------------------------------------------------------------------------
# Start off with the testimage facility of the image module; extract pixel
# values and the coordinate system from the test image and use both to create a
# new image

useTestimage := function ()
{
  im := imagemaketestimage();

  pixels := im.getchunk();
  cs     := im.coordsys();

  im.done();

  im := imagefromarray ('testimage.img',
                        pixels=pixels,
                        csys=cs,
                        overwrite=T);

  im.view(axislabels=T);

  dd := dv.loaddata(im,'raster');
  dd.setoptions([axislabelswitch=T,
                 axislabels=T,
                 titletext='Test image',
                 xgridtype=['Tick marks'],
                 ygridtype=['Tick marks'],
                 xgridcolor='green',
                 ygridcolor='green',
                 plotoutlinecolor='green',
                 axislabelspectralunit=['MHz'],
                 colormap=['Hot Metal 2']]
                );
  dp := dv.newdisplaypanel();
  dp.register(dd);
}
