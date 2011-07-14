
""" Plot stuff """
from image import *
import matplotlib.pyplot as pl
from math import log10
import matplotlib.cm as cm
import matplotlib.patches as mpatches
import functions as func
import os

def _isl2border(img, isl):
    """From all valid island pixels, generate the border. """
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


def plotresults(img, ch0_image=True, rms_image=True, mean_image=True,
                ch0_islands=True, gresid_image=True, sresid_image=False,
                gmodel_image=True, smodel_image=False, pyramid_srcs=False,
                source_seds=False, ch0_flagged=False):
    """Show the results of a fit.

    Should be done as follows:

    make fig --> add pix2sky, vmin, vmax, etc.
    make_subplots(img, fig, **kwargs)
      image_list = make_image_lists(img, **kwargs) --> make list of images, names, titles, sharexy
      make axes --> add image names 
      make_plot(fig, image_name)
        ax = find axis with right name
        if island/gaussians:
           gline, = ax.plot(...)
           gline.gaussian_idx = ...
           etc.
           same for islands
        if image:
           add_custom_coord_to_axis(fig) --> add coord listers
        if sed:
           fitline, = ax.plot(...)
           fitline.alpha = ...
           fitline.chisq, etc.
           legend?
        if pyramidsrc:
           pyrline, = ax.plot(...)
           pyrline.?

    --> only fig needs to be global (for picker/keypress handlers)
      
    """
    import numpy as N
    global img_ch0, img_rms, img_mean, img_gaus_mod, img_shap_mod
    global img_gaus_resid, img_shap_resid, pixels_per_beam, pix2sky
    global vmin, vmax, vmin_cur, vmax_cur, ch0min, ch0max
    global low, fig, images, src_list, srcid_cur, sky2pix, markers

    # Define the images. The images are used both by imshow and by the
    # on_press() and coord_format event handlers
    pix2sky = img.pix2sky
    sky2pix = img.sky2pix
    gfactor = 2.0 * N.sqrt(2.0 * N.log(2.0))
    pixels_per_beam = 2.0 * N.pi * (img.beam2pix(img.beam)[0]
                                    * img.beam2pix(img.beam)[1]) / gfactor**2
    if hasattr(img, 'atrous_gaussians'):
        img_gaus_mod = img.ch0 - img.resid_wavelets
        img_gaus_resid = img.resid_wavelets
    else:
        img_gaus_mod = img.model_gaus
        img_gaus_resid = img.resid_gaus
    img_ch0 = img.ch0
    img_rms = img.rms
    img_mean = img.mean
    if img.opts.shapelet_do:
        img_shap_mod = img.model_shap
        img_shap_resid = img.ch0 - img.model_shap
    else:
        img_shap_mod = None
        img_shap_resid = None

    # Construct lists of images, titles, etc.
    images = []
    titles = []
    names = []
    markers = []
    if ch0_image:
        images.append(img_ch0)
        titles.append('Original Image\n(arbitrary logarithmic scale)')
        names.append('ch0')
    if ch0_islands:
        images.append(img_ch0)
        if hasattr(img, 'ngaus'):
            if hasattr(img, 'atrous_gaussians'):
                titles.append('Islands (hatched boundaries) and\nGaussians (red = wavelet)')
            else:
                titles.append('Islands (hatched boundaries) and\nGaussians')
        else:
            titles.append('Islands (hatched boundaries)')
        names.append('ch0')
    if ch0_flagged:
        if not hasattr(img, 'ngaus'):
            print 'Image was not fit with Gaussians. Skipping display of flagged Gaussians.'
        else:
            images.append(img_ch0)
            titles.append('Flagged Gaussians')
        names.append('ch0')
    if rms_image:
        images.append(img_rms)
        titles.append('Background rms Image')
        names.append('rms')
    if gresid_image:
        if not hasattr(img, 'ngaus'):
            print 'Image was not fit with Gaussians. Skipping residual Gaussian image.'
        else:
            images.append(img_gaus_resid)
            titles.append('Gaussian Residual Image')
            names.append('gaus_resid')
    if gmodel_image:
        if not hasattr(img, 'ngaus'):
            print 'Image was not fit with Gaussians. Skipping model Gaussian image.'
        else:
            images.append(img_gaus_mod)
            titles.append('Gaussian Model Image')
            names.append('gaus_mod')
    if mean_image:
        images.append(img_mean)
        titles.append('Background mean Image')
        names.append('mean')
    if sresid_image:
        if img.opts.shapelet_do == False:
            print 'Image was not decomposed into shapelets. Skipping residual shapelet image.'
        else:
            images.append(img_shap_resid)
            titles.append('Shapelet Residual Image')
            names.append('shap_resid')
    if smodel_image:
        if img.opts.shapelet_do == False:
            print 'Image was not decomposed into shapelets. Skipping model shapelet image.'
        else:
            images.append(img_shap_mod)
            titles.append('Shapelet Model Image')
            names.append('shap_mod')
    if source_seds:
        if img.opts.spectralindex_do == False:
            print 'Source SEDs were not fit. Skipping source SED plots.'
        else:
            src_list = img.source
            sed_src = get_src(src_list, 0)
            if sed_src == None:
                print 'No sources found. Skipping source SED plots.'
            else:
                images.append('seds')
                titles.append('')
                names.append('seds')
                srcid_cur = 0
    if pyramid_srcs:
        if img.opts.atrous_do == False:
            print 'Image was not decomposed into wavelets. Skipping wavelet images.'
        else:
            # Get the unique j levels and store them. Only make subplots for
            # occupied j levels
            j_list = []
            for p in img.pyrsrcs:
                for l in p.jlevels:
                    j_list.append(l)
            j_set = set(j_list)
            j_with_gaus = list(j_set)
            index_first_waveplot = len(images) 
            for i in range(len(j_with_gaus)):
                images.append('wavelets')
                names.append('pyrsrc'+str(i))
    if images == []:
        print 'No images to display.'
        return
    
    im_mean = img.clipped_mean
    im_rms = img.clipped_rms
    if img.resid_gaus == None:
        low = 1.1*abs(img.min_value)
    else:
        low = N.max([1.1*abs(img.min_value),1.1*abs(N.nanmin(img.resid_gaus))])
    vmin_est = im_mean - im_rms*5.0 + low
    if vmin_est <= 0.0:
        vmin = N.log10(low)
    else:
        vmin = N.log10(vmin_est)
    vmax = N.log10(im_mean + im_rms*30.0 + low)
    ch0min = vmin
    ch0max = N.log10(img.max_value + low)
    vmin_cur = vmin
    vmax_cur = vmax
    origin = 'lower'
    colours = ['m', 'b', 'c', 'g', 'y', 'k'] # reserve red ('r') for wavelets
    styles = ['-', '-.', '--']
    print '=' * 72
    print 'NOTE -- With the mouse pointer in plot window:'
    print '  Press "i" ........ : Get integrated fluxes and mean rms values'
    print '                       for the visible portion of the image'
    print '  Press "m" ........ : Change min and max scaling values'
    print '  Press "n" ........ : Show / hide island IDs'
    print '  Press "0" ........ : Reset scaling to default'
    if 'seds' in images:
        print '  Press "c" ........ : Change source for SED plot'
    if ch0_islands and hasattr(img, 'ngaus'):
        print '  Click Gaussian ... : Print Gaussian and source IDs (zoom_rect mode, '
        print '                       toggled with the "zoom" button and indicated in '
        print '                       the lower right corner, must be off)'
        if 'seds' in images:
            print '                       The SED plot will also show the chosen source.'
    print '_' * 72

    numx = 2
    numy = int(N.ceil(float(len(images))/float(numx)))
    fig = pl.figure(figsize=(max(15, 10.0*float(numy)/float(numx)), 10.0))
    fig.canvas.set_window_title('PyBDSM Fit Results for '+ img.filename)
    gray_palette = cm.gray
    gray_palette.set_bad('k')

    for i, image in enumerate(images):
        if image != 'wavelets' and image != 'seds':
            if i == 0:
                cmd = 'ax' + str(i+1) + ' = pl.subplot(' + str(numx) + \
                    ', ' + str(numy) + ', ' + str(i+1) + ')'
            else:
                cmd = 'ax' + str(i+1) + ' = pl.subplot(' + str(numx) + \
                    ', ' + str(numy) + ', ' + str(i+1) + ', sharex=ax1' + \
                    ', sharey=ax1)'
            exec cmd
            im = N.log10(image + low)
            if 'Islands' in titles[i]:
                for iisl, isl in enumerate(img.islands):
                    xb, yb = _isl2border(img, isl)
                    cmd = "ax" + str(i+1) + ".plot(xb, yb, 'x', color='#afeeee', "\
                        "markersize=8)"
                    exec cmd
                    ax = pl.gca()
                    marker = ax.text(N.max(xb)+2, N.max(yb), str(isl.island_id),
                                      color='#afeeee', clip_on=True)
                    marker.set_visible(not marker.get_visible())
                    markers.append(marker)
                    # draw the gaussians with one colour per source or island
                    # (if gaul2srl was not run)
                    if hasattr(img, 'nsrc'):
                        nsrc = len(isl.source)
                        for isrc in range(nsrc):
                            col = colours[isrc % 6]
                            style = styles[isrc/6 % 3]
                            src = isl.source[isrc]
                            for g in src.gaussians:
                                gidx = g.gaus_num - 1
                                ellx, elly = func.drawellipse(g)
                                gline, = ax.plot(ellx, elly, color = col,
                                                 linestyle = style, picker=3)
                                gline.gaus_id = gidx
                                gline.src_id = src.source_id
                                gline.jlevel = g.jlevel
                                gline.isl_id = g.island_id
                                gline.tflux = g.total_flux
                                gline.pflux = g.peak_flux
                    else: # just plot one color per island
                        if hasattr(img, 'ngaus'):
                            isrc = 0
                            col = colours[isrc % 6]
                            style = styles[isrc/6 % 3]
                            for g in isl.gaul:
                                gidx = g.gaus_num - 1
                                ellx, elly = func.drawellipse(g)
                                gline, = ax.plot(ellx, elly, color = col,
                                                 linestyle = style, picker=3)
                                gline.gaus_id = gidx
                                gline.src_id = src.source_id
                                gline.jlevel = g.jlevel
                                gline.isl_id = g.island_id
                                gline.tflux = g.total_flux
                                gline.pflux = g.peak_flux
                if hasattr(img, 'atrous_gaussians'):
                    for jindx, atrgaus in enumerate(img.atrous_gaussians):
                        for atrg in atrgaus:
                            gidx = atrg.gaus_num - 1
                            col = 'r'
                            style = '-'
                            ellx, elly = func.drawellipse(atrg)
                            gline, = ax.plot(ellx, elly, color = col,
                                             linestyle = style, picker=3)
                            gline.gaus_id = gidx
                            gline.src_id = atrg.source_id
                            gline.jlevel = atrg.jlevel
                            gline.isl_id = g.island_id
                            gline.tflux = g.total_flux
                            gline.pflux = g.peak_flux
            if 'Flagged' in titles[i]:
                for iisl, isl in enumerate(img.islands):
                    xb, yb = _isl2border(img, isl)
                    cmd = "ax" + str(i+1) + ".plot(xb, yb, 'x', color='#afeeee', "\
                        "markersize=8)"
                    exec cmd
                    ax = pl.gca()
                    style = '-'
                    for ig, g in enumerate(isl.fgaul):
                        col = colours[ig % 6]
                        ellx, elly = func.drawellipse(g)
                        gline, = ax.plot(ellx, elly, color = col,
                                         linestyle = style, picker=3)
                        gline.flag = g.flag

            cmd = 'ax' + str(i+1) + ".imshow(N.transpose(im), origin=origin, "\
                    "interpolation='bilinear',vmin=vmin, vmax=vmax, cmap=gray_palette)"
            exec cmd
            cmd = 'ax' + str(i+1) + '.format_coord = format_coord_'+names[i]
            exec cmd
            pl.title(titles[i])
        elif image == 'seds':
            cmd = 'ax' + str(i+1) + ' = pl.subplot(' + str(numx) + \
                ', ' + str(numy) + ', ' + str(i+1) + ')'
            exec cmd
            ax = pl.gca()
            plot_sed(sed_src, ax)

        elif image == 'wavelets':
            if i == index_first_waveplot:
                for j in range(len(j_with_gaus)):
                    cmd = 'ax' + str(j+i+1) + ' = pl.subplot(' + str(numx) + \
                        ', ' + str(numy) + ', ' + str(j+i+1) + ', sharex=ax1, '+\
                        'sharey=ax1)'
                    exec cmd
                    pl.title('Pyramidal Sources for\nWavelet Scale J = ' +
                             str(j_with_gaus[j]))
            for pyr in img.pyrsrcs:
              for iisl, isl in enumerate(pyr.islands):
                jj = pyr.jlevels[iisl]
                jindx = j_with_gaus.index(jj)
                col = colours[pyr.pyr_id % 6]
                ind = N.where(~isl.mask_active)
                cmd = "ax" + str(jindx + index_first_waveplot + 1) + \
                    ".plot(ind[0]+isl.origin[0], "\
                    "ind[1]+isl.origin[1], '.', color=col)"
                exec cmd
           
    pl.connect('key_press_event', on_press)
    pl.connect('pick_event', on_pick)
    pl.show()

def on_pick(event):
    global images
    global srcid_cur
    g = event.artist
    if hasattr(g, 'gaus_id'):
        gaus_id = g.gaus_id
        src_id = g.src_id
        isl_id = g.isl_id
        tflux = g.tflux
        pflux = g.pflux
        wav_j = g.jlevel
        if wav_j == 0:
            print 'Gaussian #' + str(gaus_id) + ' (in src #' + str(src_id) + \
                ', isl #' + str(isl_id) + '): F_tot = ' + str(round(tflux,4)) + \
                ' Jy, F_peak = ' + str(round(pflux,4)) + ' Jy/beam'
        else:
            print 'Gaussian #' + str(gaus_id) + ' (in src #' + str(src_id) + \
                ', isl #' + str(isl_id) + ', wav #' + str(wav_j) + \
                '): Ftot = ' + str(round(tflux,3)) + ' Jy, F_peak = ' + \
                str(round(pflux,4)) + ' Jy/beam'
                
        # Change source SED
        # First check that SEDs are being plotted and that the selected Gaussian
        # is from the zeroth wavelet image
        has_sed = False
        if 'seds' in images and wav_j == 0:
            has_sed = True
        if not has_sed:
            return
        ax_indx = images.index('seds')
        sed_src = get_src(src_list, src_id)
        if srcid_cur == src_id:
            return
        srcid_cur = src_id
        axes_list = fig.get_axes()
        for axindx, ax in enumerate(axes_list):
            if images[axindx] == 'seds':
                plot_sed(sed_src, ax)
    else:
        print 'Flagged Gaussian (flag = ' + str(g.flag) + '; use "' + \
            "help 'flagging_opts'" + '" for flag meanings)'
 
    pl.draw()
       
    
def on_press(event):
    """Handle keypresses"""
    from interface import raw_input_no_history
    import numpy
    
    global img_ch0, img_rms, img_mean, img_gaus_mod, img_shap_mod
    global pixels_per_beam, vmin, vmax, vmin_cur, vmax_cur
    global ch0min, ch0max, low, fig, images, src_list, srcid_cur
    global markers
    if event.key == '0':
        print 'Resetting limits to defaults (%.4f -- %.4f Jy/beam)' \
            % (pow(10, vmin)-low,
               pow(10, vmax)-low)
        axes_list = fig.get_axes()
        for axindx, ax in enumerate(axes_list):
            if images[axindx] != 'wavelets' and images[axindx] != 'seds':
                im = ax.get_images()[0]
                im.set_clim(vmin, vmax)
        vmin_cur = vmin
        vmax_cur = vmax
        pl.draw()
    if event.key == 'm':
        # Modify scaling
        # First check that there are images to modify
        has_image = False
        for im in images:
            if isinstance(im, numpy.ndarray):
                has_image = True
        if not has_image:
            return
        minscl = 'a'
        while isinstance(minscl, str):
            try:
                if minscl == '':
                    minscl = pow(10, vmin_cur) - low
                    break
                minscl = float(minscl)
            except ValueError:
                prompt = "Enter min value (current = %.4f Jy/beam) : " % (pow(10, vmin_cur)-low,)
                try:
                    minscl = raw_input_no_history(prompt)
                except RuntimeError:
                    print 'Sorry, unable to change scaling.'
                    return
        minscl = N.log10(minscl + low)
        maxscl = 'a'
        while isinstance(maxscl, str):
            try:
                if maxscl == '':
                    maxscl = pow(10, vmax_cur) - low
                    break
                maxscl = float(maxscl)
            except ValueError:
                prompt = "Enter max value (current = %.4f Jy/beam) : " % (pow(10, vmax_cur)-low,)
                try:
                    maxscl = raw_input_no_history(prompt)
                except RuntimeError:
                    print 'Sorry, unable to change scaling.'
                    return
        maxscl = N.log10(maxscl + low)
        if maxscl <= minscl:
            print 'Max value must be greater than min value!'
            return
        axes_list = fig.get_axes()
        for axindx, ax in enumerate(axes_list):
            if images[axindx] != 'wavelets' and images[axindx] != 'seds':
                im = ax.get_images()[0]
                im.set_clim(minscl, maxscl)
        vmin_cur = minscl
        vmax_cur = maxscl
        pl.draw()  
    if event.key == 'c':
        # Change source SED
        # First check that SEDs are being plotted
        has_sed = False
        if 'seds' in images:
            has_sed = True
        if not has_sed:
            return
        srcid = 'a'
        while isinstance(srcid, str):
            try:
                if srcid == '':
                    srcid = srcid_cur
                    break
                srcid = int(srcid)
            except ValueError:
                prompt = "Enter source ID (current = %i) : " % (srcid_cur,)
                try:
                    srcid = raw_input_no_history(prompt)
                except RuntimeError:
                    print 'Sorry, unable to change source.'
                    return
        ax_indx = images.index('seds')
        sed_src = get_src(src_list, srcid)
        if sed_src == None:
            print 'Source not found!'
            return
        srcid_cur = srcid
        axes_list = fig.get_axes()
        for axindx, ax in enumerate(axes_list):
            if images[axindx] == 'seds':
                plot_sed(sed_src, ax)
        pl.draw()
    if event.key == 'i':
        # Print info about visible region
        has_image = False
        axes_list = fig.get_axes()
        # Get limits of visible region
        for axindx, ax in enumerate(axes_list):
            if images[axindx] != 'wavelets' and images[axindx] != 'seds':
                xmin, xmax = ax.get_xlim()
                ymin, ymax = ax.get_ylim()
                has_image = True
                break
        if not has_image:
            return
        if xmin < 0:
            xmin = 0
        if xmax > img_ch0.shape[0]:
            xmax = img_ch0.shape[0]
        if ymin < 0:
            ymin = 0
        if ymax > img_ch0.shape[1]:
            ymax = img_ch0.shape[1]
        flux = N.nansum(img_ch0[xmin:xmax, ymin:ymax])/pixels_per_beam
        mask = N.isnan(img_ch0[xmin:xmax, ymin:ymax])
        num_pix_unmasked = float(N.size(N.where(mask == False), 1))
        mean_rms = N.nansum(img_rms[xmin:xmax, ymin:ymax])/num_pix_unmasked
        mean_map_flux = N.nansum(img_mean[xmin:xmax, ymin:ymax])/pixels_per_beam
        if img_gaus_mod == None:
            gaus_mod_flux = 0.0
        else:
            gaus_mod_flux = N.nansum(img_gaus_mod[xmin:xmax, ymin:ymax])/pixels_per_beam
        print 'Visible region (%i:%i, %i:%i) :' % (xmin, xmax, ymin, ymax)
        print '  ch0 flux from sum of pixels ........... : %f Jy'\
            % (flux,)
        print '  Background mean map flux .............. : %f Jy'\
            % (mean_map_flux,)
        print '  Gaussian model flux ................... : %f Jy'\
            % (gaus_mod_flux,)
        if img_shap_mod != None:
            shap_mod_flux = N.nansum(img_shap_mod[xmin:xmax, ymin:ymax])/pixels_per_beam
            print '  Shapelet model flux ................... : %f Jy'\
                % (shap_mod_flux,)
        print '  Mean rms (from rms map) ............... : %f Jy/beam'\
            % (mean_rms,)
    if event.key == 'n':
        # Show/Hide island numbers
        if markers:
            for marker in markers:
                marker.set_visible(not marker.get_visible())
            pl.draw()

# The following functions add ra, dec and flux density to the
# coordinates in the lower-right-hand corner of the figure window.
# Since each axis needs its own function (to return its particular
# flux), we need a separate function for each subplot.
def format_coord_ch0(x, y):
    """Custom coordinate format for ch0 image"""
    global img_ch0
    im = img_ch0
    coord_str = make_coord_str(x, y, im)
    return coord_str

def format_coord_rms(x, y):
    """Custom coordinate format for rms image"""
    global img_rms
    im = img_rms
    coord_str = make_coord_str(x, y, im)
    return coord_str

def format_coord_mean(x, y):
    """Custom coordinate format for mean image"""
    global img_mean
    im = img_mean
    coord_str = make_coord_str(x, y, im)
    return coord_str

def format_coord_gaus_mod(x, y):
    """Custom coordinate format for Gaussian model image"""
    global img_gaus_mod
    im = img_gaus_mod
    coord_str = make_coord_str(x, y, im)
    return coord_str

def format_coord_shap_mod(x, y):
    """Custom coordinate format for shapelet model image"""
    global img_shap_mod
    im = img_shap_mod
    coord_str = make_coord_str(x, y, im)
    return coord_str

def format_coord_gaus_resid(x, y):
    """Custom coordinate format for Gaussian residual image"""
    global img_gaus_resid
    im = img_gaus_resid
    coord_str = make_coord_str(x, y, im)
    return coord_str

def format_coord_shap_resid(x, y):
    """Custom coordinate format for shapelet residual image"""
    global img_shap_resid
    im = img_shap_resid
    coord_str = make_coord_str(x, y, im)
    return coord_str

    
def xy_to_radec_str(x, y):
    """Converts x, y in image coords to a sexigesimal string"""
    from output import ra2hhmmss, dec2ddmmss
    global pix2sky
    ra, dec = pix2sky([x, y])
    
    ra = ra2hhmmss(ra)
    sra = str(ra[0]).zfill(2)+':'+str(ra[1]).zfill(2)+':'+str("%.1f" % (ra[2])).zfill(3)
    dec = dec2ddmmss(dec)
    decsign = ('-' if dec[3] < 0 else '+')
    sdec = decsign+str(dec[0]).zfill(2)+':'+str(dec[1]).zfill(2)+':'+str("%.1f" % (dec[2])).zfill(3)
    return sra, sdec


def make_coord_str(x, y, im):
    """Makes the x, y, ra, dec, flux string"""
    rastr, decstr = xy_to_radec_str(x, y)
    col = int(x + 0.5)
    row = int(y + 0.5)
    numcols, numrows = im.shape
    if col >= 0 and col < numcols\
            and row >= 0 and row < numrows:
        z = im[col, row]
        return 'x=%1.1f, y=%1.1f, RA=%s, Dec=%s, F=%+1.4f Jy/beam' % (x, y, rastr, decstr, z)
    else:
        return 'x=%1.1f, y=%1.1f' % (x, y)

def plot_sed(src, ax):
    """Plots the SED for source 'src' to axis 'ax'"""
    global sky2pix
    global fig
    ax.cla()
    spin = src.spin1
    espin = src.espin1
    spin1 = src.spin2
    espin1 = src.espin2
    take2nd = src.take2nd
    y = src.specin_flux
    ey = src.specin_fluxE
    x = src.specin_freq
    ax.errorbar(N.log10(x/1e6), N.log10(y), yerr=ey/y, fmt='bo')
    ax.plot(N.log10(x/1e6), N.log10(spin[0])+N.log10(x/src.specin_freq0)*spin[1], 
            '-g', label="alpha = %.2f" % (spin[0],))
    ax.plot(N.log10(x/1e6), N.log10(spin1[0])+N.log10(x/src.specin_freq0)*
            spin1[1]+N.log10(x/src.specin_freq0)*
            N.log10(x/src.specin_freq0)*spin1[2], '-m',
            label="alpha1,2 = %.2f, %.2f" % (spin1[0], spin1[1]))
    pos = sky2pix(src.posn_sky_centroid)
    xpos = int(pos[0])
    ypos = int(pos[1])
    pl.title('SED of source #'+str(src.source_id)+'\n'
             +'(x = '+str(xpos)+', y = '+str(ypos)+')')
    pl.xlabel('log Frequency (MHz)')
    pl.ylabel('log Flux (Jy)')
    pl.legend()


def get_src(src_list, srcid):
    """Returns the source for srcid or None if not found"""
    for src in src_list:
        if src.source_id == srcid:
            return src
    return None
