
""" Plot stuff """

from image import *
import pylab as pl
from math import log10
import matplotlib.cm as cm
import functions as func

def isl2border(img, isl):
    """ From all valid island pixels, generate the border. """

    fmask = ~isl.mask_active
    n, m = img.ch0.shape
    subn, subm = fmask.shape
    c0 = N.arange(isl.bbox[0].start, isl.bbox[0].stop)
    c1 = N.arange(isl.bbox[1].start, isl.bbox[1].stop)
    coords = [(i,j) for i in range(subn) for j in range(subm)]
    value = [(i,j) for i in c0 for j in c1]

    border = []
    for k, val in enumerate(value):
      if fmask[coords[k]]:
        neigh = [[i,j] for i in range(val[0]-1,val[0]+2) for j in range(val[1]-1,val[1]+2) \
                       if i>=0 and i<n and j>=0 and j<m and [i,j] != val]
        dumi = len(neigh)
        dumi1 = 0
        for nei in neigh:
            if fmask[tuple(N.array(nei)-N.array(isl.origin))]: dumi1 += 1
        if dumi - dumi1 >= 2: border.append(val)

    return N.transpose(N.array(border))

def getisland(img, c):
    """ Return the island object for the island in which pixel c is situated. If in no island, return None."""

    n, m = img.ch0.shape
    x, y = c
    if x<0 or y<0 or x>=n or y>=m:
      print "Pixel ",c, " is not a valid location."
      return None
    else:
      islid = -1
      for isl in img.islands:
        xc, yc = x-isl.origin[0], y-isl.origin[1]
        n1, m1 = isl.shape
        if xc>=0 and yc>=0 and xc<n1 and yc<m1 and (not isl.mask_active[xc, yc]):
          islid = isl.island_id
          print 'Pixel ',repr(c),' is in island_id ',islid,'; returning island object'
          return isl.island_id
      if islid == -1:
        print 'Pixel ',repr(c),' is not in any island'
        return None


def plotresults(img):
        """ Show the greyscale image and the gaussian residual image and overplot the island boundaries with
            their island numbers as well as the sigma-contours of the gaussians, with one colour for each 
            source. """
       
        im_mean = img.clipped_mean
        im_rms = img.clipped_rms
        low = 1.1*abs(img.min_value)
        vmin = log10(im_mean-im_rms*4.0 + low)
        vmax = log10(im_mean+im_rms*10.0 + low)
        origin='lower'
        colours = ['r', 'g', 'b', 'c', 'm', 'y', 'k']
        styles = ['-', '-.', '--']
        tit = [' (arbitrary scale)', ' Residual image']

        images = [img.ch0]
        if hasattr(img, 'resid_gaus') and img.resid_gaus != None: images.append(img.resid_gaus)
        for i, image in enumerate(images):
          pl.figure()
          pl.title('PyBDSM : '+img.imagename + tit[i])

          im = N.log10(image + low)
          for iisl, isl in enumerate(img.islands):
                                        # draw the border
            xb, yb = isl2border(img, isl) 
            pl.plot(xb, yb, 'x', color='#afeeee', markersize=8)
                                        # mark the island number
            pl.text(N.max(xb)+2, N.max(yb), str(isl.island_id), color='#afeeee')

            donegaus = hasattr(img, 'ngaus')
            donesrc = hasattr(img, 'nsrc')

            if donegaus and donesrc:
              nsrc = len(isl.source)
              for isrc in range(nsrc):
                col = colours[isrc % 7]
                style = styles[isrc/7 % 3]
                src = isl.source[isrc]
                for g in src.gaussians:
                  ellx, elly = func.drawellipse(g)
                  pl.plot(ellx, elly, color = col, linestyle = style) 
            if donegaus and not donesrc:
              for g in img.gaussians():
                ellx, elly = func.drawellipse(g)
                pl.plot(ellx, elly, 'r')

          pl.imshow(N.transpose(im), origin=origin, interpolation='nearest',vmin=vmin, vmax=vmax, \
                    cmap=cm.gray)
          pl.colorbar()

def process_morph(image, suptitle, ft, thr, mask, shape, beam):
    import scipy.ndimage as nd
    from copy import deepcopy as cp

    def show(im, title=''):
        pl.imshow(N.transpose(im), origin='lower', interpolation='nearest') 
        pl.title(title)
    
    pl.figure(); pl.suptitle(suptitle)
    pl.subplot(3,3,1); show(image, 'Image')

    if not N.all(N.array(image, bool) == image): 
      grey=True
    else:
      grey=False

    if grey: 
      opd = nd.grey_dilation(image, footprint = ft)
    else:
      opd = nd.binary_dilation(image, ft)
    pl.subplot(3,3,2); show(opd, 'Dil')

    if grey: 
      ope = nd.grey_erosion(image, footprint = ft)
    else:
      ope = nd.binary_erosion(image, ft)
    pl.subplot(3,3,3); show(ope, 'Ero')

    opl = nd.morphological_laplace(image, footprint = ft)
    pl.subplot(3,3,4); show(opl, 'Lap')

    opg = nd.morphological_gradient(image, footprint = ft)
    pl.subplot(3,3,5); show(opg, 'Grad')

    opb = image - ope
    pl.subplot(3,3,6); show(opb, 'Bound')

    if grey: 
      opc = nd.grey_closing(image, footprint = ft)
    else:
      opc = nd.binary_closing(image, ft)
    pl.subplot(3,3,7); show(opc, 'Close')

    if grey: 
      opo = nd.grey_opening(image, footprint = ft)
    else:
      opo = nd.binary_opening(image, ft)
    pl.subplot(3,3,8); show(opo, 'Open')

    if grey:
      inipeak, iniposn, im1 = func.get_maxima(image, mask, thr, shape, beam)
      ng = len(iniposn); markers = N.zeros(shape, int)
      for i in range(ng): markers[iniposn[i]] = i+2
      markers[N.unravel_index(N.argmin(image), image.shape)] = 1
      im1 = cp(image)
      if image.min() < 0.: im1 = image-image.min()
      im1 = 255 - im1/im1.max()*255
      opw = nd.watershed_ift(N.array(im1, N.uint8), markers)
      pl.subplot(3,3,9)
      x1, y1 = N.array(iniposn).transpose(); pl.plot(x1, y1, '*k')
      show(opw, 'Watershed'); pl.colorbar()

def plot_morph_isl(img, isls=None, thr=None):
    import functions as func
    from copy import deepcopy as cp
    import numpy 

    if isls == None:
      isls = []
      for isl in img.islands:
        if isl.ngaus > 1 or isl.size_active > 300: isls.append(isl)

    ft = N.ones((3,3), int)
    if isinstance(isls, numpy.ndarray):
      shape = isls.shape
      mask = N.zeros(isls.shape, bool)
      if thr == None: thr = -1e9
      process_morph(isls, 'Image', ft, thr, mask, shape, img.pixel_beam)
    else:
      for isl in isls:
        im = isl.image
        if thr==None: thr = isl.mean + img.opts.thresh_pix*isl.rms
        shape = isl.shape
        mask = isl.mask_active
        process_morph(im, 'Island '+str(isl.island_id), ft, thr, mask, shape, img.pixel_beam)




