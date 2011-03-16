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
    """Show the results of a fit with model and residual images."""
    # This function should be rewritten similarly to Joris's parmdbplot.py
    # Then, we can use check boxes to show/hide island boundaries, Gaussians,
    # and sources on all images. Maybe also show scale bars or pixel values?
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
    if img.opts.atrous_do:
        model_img = img.model_gaus_atrous
        resid_img = img.resid_gaus_atrous
    else:
        model_img = img.model_gaus
        resid_img = img.resid_gaus
    if img.opts.shapelet_do:
        tit = ['Original Image\n(arbitrary logarithmic scale)', 'Islands (hatched boundaries) and\nBest-fit Gaussians (ellipses)', 'Gaussian Model Image', 'Gaussian Residual Image', 'Shapelet Model Image', 'Shapelet Residual Image']  
        images = [img.ch0, img.ch0, model_img, resid_img, img.model_shap, img.resid_shap]
        numy = 3
    else:
        tit = ['Original Image\n(arbitrary logarithmic scale)', 'Islands (hatched boundaries) and\nBest-fit Gaussians (ellipses)', 'Gaussian Model Image', 'Gaussian Residual Image']
        images = [img.ch0, img.ch0, model_img, resid_img]
        numy = 2

    fig = pl.figure(figsize=(10.0,10.0))
    #fig = pl.figure(figsize=(10.0,img.ch0.shape[1]/img.ch0.shape[0]*10.0))
    gray_palette = cm.gray
    gray_palette.set_bad('k')

    for i, image in enumerate(images):
      if i == 0:
          ax1 = pl.subplot(2, numy, 1)
      if i == 1:
          ax2 = pl.subplot(2, numy, 2, sharex=ax1, sharey=ax1)
      if i == 2:
          ax3 = pl.subplot(2, numy, 3, sharex=ax1, sharey=ax1)
      if i == 3:
          ax4 = pl.subplot(2, numy, 4, sharex=ax1, sharey=ax1)
      if i == 4:
          ax5 = pl.subplot(2, numy, 5, sharex=ax1, sharey=ax1)
      if i==5:
          ax6 = pl.subplot(2, numy, 6, sharex=ax1, sharey=ax1)

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
          if hasattr(img, 'atrous_gaussians'):
              for atrgaus in img.atrous_gaussians:
                  for atrg in atrgaus():
                      col = 'r'
                      style = '-'
                      ellx, elly = func.drawellipse(atrg)
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
      if i == 4:
          ax5.imshow(N.transpose(im), origin=origin, interpolation='nearest',vmin=vmin, vmax=vmax, \
                cmap=gray_palette)
      if i == 5:
          ax6.imshow(N.transpose(im), origin=origin, interpolation='nearest',vmin=vmin, vmax=vmax, \
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


def showsrc(img):
    """Show original image and overlay source boundaries and indices."""
    if hasattr(img, 'nsrc') is False:
        # gaul2srl was not run
        print 'Gaussians were not grouped into sources.'
    else:
        # show ch0 map
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
        tit = 'Original Image with Sources\n(arbitrary logarithmic scale)'

        image = img.ch0
        fig = pl.figure(figsize=(10.0,10.0))
        gray_palette = cm.gray
        gray_palette.set_bad('k')
        ax1 = pl.subplot(1, 1, 1)
        im = N.log10(image + low)
        ax1.imshow(N.transpose(im), origin=origin, interpolation='nearest',vmin=vmin, vmax=vmax, \
                       cmap=gray_palette)
        # draw the gaussians with one colour per source
        for isrc, src in enumerate(img.source):
            col = colours[isrc % 7]
            style = styles[isrc/7 % 3]
            for g in src.gaussians:
                ellx, elly = func.drawellipse(g)
                ax1.plot(ellx, elly, color = col, linestyle = style)
            text_xpos = img.sky2pix([src.posn_sky_centroid[0] + src.size_sky[0]/N.cos(src.posn_sky_centroid[1]), src.posn_sky_centroid[1]])[0]
            text_ypos = img.sky2pix(src.posn_sky_centroid)[1]
            ax1.text(text_xpos, text_ypos, str(src.source_id), color=col, clip_on=True) # label source

        pl.title(tit)
        pl.show()
        if os.environ.get("REMOTEHOST") != 'lfe001.offline.lofar':
            pl.close()
