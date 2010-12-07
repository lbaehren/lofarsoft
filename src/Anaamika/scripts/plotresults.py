
""" Plot stuff """
from image import *
import pylab as pl
from math import log10
import matplotlib.cm as cm
import functions as func
import os

def _isl2border(img, isl):
    """ From all valid islan pixels, generate the border. """
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

def plotresults(img):
        im_mean = img.clipped_mean
        im_rms = img.clipped_rms
        low = N.max(1.1*abs(img.min_value),1.1*abs(img.resid_gaus.min()))
        vmin_est = im_mean - im_rms*5.0 + low
        if vmin_est <= 0.0:
            vmin = N.log10(low)
        else:
            vmin = N.log10(vmin_est)
        vmax = N.log10(im_mean + im_rms*30.0 + low)
        origin='lower'
        colours = ['r', 'g', 'b', 'c', 'm', 'y', 'k']
        styles = ['-', '-.', '--']
        tit = ['Original Image\n(arbitrary logarithmic scale)', 'Islands (hatched boundaries) and\nBest-fit Gaussians (ellipses)', 'Model Image', 'Residual Image']

        images = [img.ch0, img.ch0, img.model_gaus, img.resid_gaus]
        fig = pl.figure(figsize=(10.0,img.ch0.shape[1]/img.ch0.shape[0]*10.0))
        gray_palette = cm.gray
        gray_palette.set_bad('k')
        for i, image in enumerate(images):
          if i == 0:
              ax1 = pl.subplot(2, 2, 1)
          if i == 1:
              ax2 = pl.subplot(2, 2, 2, sharex=ax1, sharey=ax1)
          if i == 2:
              ax3 = pl.subplot(2, 2, 3, sharex=ax1, sharey=ax1)
          if i == 3:
              ax4 = pl.subplot(2, 2, 4, sharex=ax1, sharey=ax1)
          im = N.log10(image + low)
          if i == 0:
              ax1.imshow(N.transpose(im), origin=origin, interpolation='nearest',vmin=vmin, vmax=vmax, \
                    cmap=gray_palette)
          if i == 1:
              for iisl, isl in enumerate(img.islands):
                                           
                xb, yb = _isl2border(img, isl) 
                ax2.plot(xb, yb, 'x', color='#afeeee', markersize=8) # mark the island number
                ax2.text(N.max(xb)+2, N.max(yb), str(isl.island_id), color='#afeeee', clip_on=True) # draw the border
                # draw the gaussians with one colour per source or island (if gaul2srl was not run)
                if hasattr(img, 'nsrc'): # check whether gaul2srl was run
                    nsrc = len(isl.source)
                    for isrc in range(nsrc):
                        col = colours[isrc % 7]
                        style = styles[isrc/7 % 3]
                        src = isl.source[isrc]
                        for g in src.gaussians:
                            ellx, elly = func.drawellipse(g)
                            ax2.plot(ellx, elly, color = col, linestyle = style)
                else: # just plot one color per island
                    isrc = 0
                    col = colours[isrc % 7]
                    style = styles[isrc/7 % 3]
                    for g in isl.gaul:
                        ellx, elly = func.drawellipse(g)
                        ax2.plot(ellx, elly, color = col, linestyle = style)
   
          if i == 1:
              ax2.imshow(N.transpose(im), origin=origin, interpolation='nearest',vmin=vmin, vmax=vmax, \
                    cmap=gray_palette)
          if i == 2:
              ax3.imshow(N.transpose(im), origin=origin, interpolation='nearest',vmin=vmin, vmax=vmax, \
                    cmap=gray_palette)
          if i == 3:
              ax4.imshow(N.transpose(im), origin=origin, interpolation='nearest',vmin=vmin, vmax=vmax, \
                    cmap=gray_palette)
          pl.title(tit[i])
        pl.show()
        if os.environ.get("REMOTEHOST") != 'lfe001.offline.lofar':
            pl.close()

def showrms(img):
    """Show original and background rms images or print rms if constant."""
    if img.opts.rms_map is False:
        # rms map is a constant, so don't show image
        print 'Background rms map is set to constant value of '+str(img.clipped_rms)+' Jy/beam'
    else:
        # show rms map
        im_mean = img.clipped_mean
        im_rms = img.clipped_rms
        low = N.max(1.1*abs(img.min_value),1.1*abs(img.resid_gaus.min()))
        vmin = N.log10(img.rms.min()+low)
        vmax = N.log10(img.rms.max()+low)
        origin='lower'
        tit = ['Original Image with Islands\n(arbitrary logarithmic scale)', 'Background rms Image']

        images = [img.ch0, img.rms]
        fig = pl.figure(figsize=(10.0,img.ch0.shape[1]/img.ch0.shape[0]*5.0))
        gray_palette = cm.gray
        gray_palette.set_bad('k')
        for i, image in enumerate(images):
          if i == 0:
              ax1 = pl.subplot(1, 2, 1)
          if i == 1:
              ax2 = pl.subplot(1, 2, 2, sharex=ax1, sharey=ax1)
          im = N.log10(image + low)
          if i == 0:
              ax1.imshow(N.transpose(im), origin=origin, interpolation='nearest',vmin=vmin, vmax=vmax, \
                    cmap=gray_palette)
          if i == 0:
              for iisl, isl in enumerate(img.islands):
                xb, yb = _isl2border(img, isl) 
                ax1.plot(xb, yb, 'x', color='#afeeee', markersize=8)  # mark the island number
                ax1.text(N.max(xb)+2, N.max(yb), str(isl.island_id), color='#afeeee', clip_on=True)  # draw the border
          if i == 1:
              ax2.imshow(N.transpose(im), origin=origin, interpolation='nearest',vmin=vmin, vmax=vmax, \
                    cmap=gray_palette)
          pl.title(tit[i])
        pl.show()
        if os.environ.get("REMOTEHOST") != 'lfe001.offline.lofar':
            pl.close()
