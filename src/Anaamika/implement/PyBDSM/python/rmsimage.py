"""Module rmsimage.

Defines operation Op_rmsimage which calculates noise and
rms maps. Depending on the value of parameter rms_noise_map
the generated map will contain either real map or a constant
value.

The current implementation will handle both 2D and 3D images,
where for 3D case it will calculate maps for each plane.

Masked images aren't handled properly yet.
"""

import numpy as N
import scipy.ndimage as nd
import _cbdsm
from image import Op, Image, NArray, List
import const
import mylogger
import os
import functions as func

### insert into Image tc-variables for mean & rms maps
Image.mean = NArray(doc="Mean map, Stokes I")
Image.rms  = NArray(doc="RMS map, Stokes I")
Image.mean_QUV = List(NArray(), doc="Mean map, Stokes Q")
Image.rms_QUV  = List(NArray(), doc="RMS map, Stokes Q")

class Op_rmsimage(Op):
    """Calculate rms & noise maps

    Prerequisites: Module preprocess should be run first.
    """
    def __call__(self, img):
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"RMSimage  ")
        mylog.info("Calculating background rms and mean images")
        if img.opts.polarisation_do:
            pols = ['I', 'Q', 'U', 'V']
        else:
            pols = ['I'] # assume I is always present
            
        if hasattr(img, 'rms_mask'):
            mask = img.rms_mask
        else:
            mask = img.mask
        opts = img.opts
        map_opts = (opts.kappa_clip, opts.rms_box, opts.spline_rank)
        ch0_images = [img.ch0, img.ch0_Q, img.ch0_U, img.ch0_V]
        cmeans = [img.clipped_mean] + img.clipped_mean_QUV
        crmss = [img.clipped_rms] + img.clipped_rms_QUV
        for ipol, pol in enumerate(pols):
          data = ch0_images[ipol]
          mean = N.zeros(data.shape, dtype=N.float32)
          rms  = N.zeros(data.shape, dtype=N.float32)
          if len(pols) > 1:
              pol_txt = ' (' + pol + ')'
          else:
              pol_txt = ''
              
          ## calculate rms/mean maps if needed
          if (opts.rms_map is not False) or (opts.mean_map not in ['zero', 'const']):
            if len(data.shape) == 2:   ## 2d case
              self.map_2d(data, mean, rms, mask, *map_opts)
            elif len(data.shape) == 3: ## 3d case
              if not isinstance(mask, N.ndarray):
                mask = N.zeros(data.shape[0], dtype=bool)
              for i in range(data.shape[0]):
                  ## iterate each plane
                  self.map_2d(data[i], mean[i], rms[i], mask[i], *map_opts)
            else:
              mylog.critical('Image shape not handleable' + pol_txt)
              raise RuntimeError("Can't handle array of this shape" + pol_txt)
            mylog.info('Background rms and mean images computed' + pol_txt)

          ## check if variation of rms/mean maps is significant enough
          if pol == 'I':
              if opts.rms_map is None:
                  # check_rmsmap() sets img.use_rms_map
                  self.check_rmsmap(img, rms)
              else:
                  img.use_rms_map = opts.rms_map
              if img.use_rms_map is False:
                  mylogger.userinfo(mylog, 'Using constant background rms')
              else:
                  mylogger.userinfo(mylog, 'Using 2D map for background rms')
                                  
              if opts.mean_map == 'default':
                  # check_meanmap() sets img.mean_map_type
                  self.check_meanmap(img, rms)
              else:
                  img.mean_map_type = opts.mean_map
              if img.mean_map_type != 'map':
                  mylogger.userinfo(mylog, 'Using constant background mean')
              else:
                  mylogger.userinfo(mylog, 'Using 2D map for background mean')

          ## if rms map is insignificant, or rms_map==False use const value
          if img.use_rms_map is False:
            if opts.rms_value == None:
              rms[:]  = crmss[ipol]
            else:
              rms[:]  = opts.rms_value
            mylogger.userinfo(mylog, 'Value of background rms' + pol_txt,
                              '%.5f Jy/beam' % rms[0][0])
          else:
            rms_min = N.nanmin(rms)
            rms_max = N.nanmax(rms)
            mylogger.userinfo(mylog, 'Min/max values of background rms map' + pol_txt,
                              '(%.5f, %.5f) Jy/beam' % (rms_min, rms_max))

          if img.mean_map_type != 'map':
            val = 0.0
            if opts.mean_map == 'const': val = img.clipped_mean
            mean[:] = val
            mylogger.userinfo(mylog, 'Value of background mean' + pol_txt,
                              str(round(val,5))+' Jy/beam')
          else:
            mean_min = N.nanmin(mean)
            mean_max = N.nanmax(mean)
            mylogger.userinfo(mylog, 'Min/max values of background mean map' + pol_txt,
                              '(%.5f, %.5f) Jy/beam' % (mean_min, mean_max))

          if pol == 'I':
            # Apply mask to mean_map and rms_map by setting masked values to NaN
            if isinstance(mask, N.ndarray):
                pix_masked = N.where(mask == True)
                mean[pix_masked] = N.nan
                rms[pix_masked] = N.nan
            img.mean = mean; img.rms  = rms
            if opts.savefits_rmsim or opts.output_all:
              dir = img.basedir + '/background/'
              if not os.path.exists(dir): os.mkdir(dir)
              func.write_image_to_file(img.use_io, img.imagename + '.rmsd_I.fits', N.transpose(rms), img, dir)
              mylog.info('%s %s' % ('Writing ', dir+img.imagename+'.rmsd_I.fits'))
            if opts.savefits_meanim or opts.output_all: 
              dir = img.basedir + '/background/'
              if not os.path.exists(dir): os.mkdir(dir)
              func.write_image_to_file(img.use_io, img.imagename + '.mean_I.fits', N.transpose(mean), img, dir)
              mylog.info('%s %s' % ('Writing ', dir+img.imagename+'.mean_I.fits'))
            if opts.savefits_normim or opts.output_all: 
              dir = img.basedir + '/background/'
              if not os.path.exists(dir): os.mkdir(dir)
              func.write_image_to_file(img.use_io, img.imagename + '.norm_I.fits', N.transpose((img.ch0-mean)/rms), img, dir)
              mylog.info('%s %s' % ('Writing ', dir+img.imagename+'.norm_I.fits'))
          else:
            img.mean_QUV.append(mean); img.rms_QUV.append(rms)

        return img

    def check_rmsmap(self, img, rms):
        """Calculates the statistics of the rms map and decides, when 
        rms_map=None, whether to take the map (if variance 
        is significant) or a constant value
        """
    	from math import sqrt

        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Rmsimage.Checkrms  ")
        cdelt = img.wcs_obj.acdelt[:2]
    	bm = (img.beam[0], img.beam[1])
    	fw_pix = sqrt(N.product(bm)/abs(N.product(cdelt)))
    	stdsub = N.std(rms)

    	rms_expect = img.clipped_rms/sqrt(2)/img.rms_box[0]*fw_pix
        mylog.debug('%s %10.6f %s' % ('Standard deviation of rms image = ', stdsub*1000.0, 'mJy'))
        mylog.debug('%s %10.6f %s' % ('Expected standard deviation = ', rms_expect*1000.0, 'mJy'))
    	if stdsub > 1.1*rms_expect:
            img.use_rms_map = True
            mylogger.userinfo(mylog, 'Variation in rms image significant')
        else:
            img.use_rms_map = False
            mylogger.userinfo(mylog, 'Variation in rms image not significant')

        return img

    def check_meanmap(self, img, mean):
        """Calculates the statistics of the mean map and decides, when 
        mean_map=None, whether to take the map (if variance 
        is significant) or a constant value
        """
    	from math import sqrt
    	
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Rmsimage.Checkmean ")
        cdelt = img.wcs_obj.acdelt[:2]
        bm = (img.beam[0], img.beam[1])
        fw_pix = sqrt(N.product(bm)/abs(N.product(cdelt)))
        stdsub = N.std(mean)
        
        rms_expect = img.clipped_rms/img.rms_box[0]*fw_pix
        mylog.debug('%s %10.6f %s' % ('Standard deviation of mean image = ', stdsub*1000.0, 'mJy'))
        mylog.debug('%s %10.6f %s' % ('Expected standard deviation = ', rms_expect*1000.0, 'mJy'))
        if stdsub > 1.1*rms_expect:
          img.mean_map_type = 'map'
          mylogger.userinfo(mylog, 'Variation in mean image significant')
        else:
          if img.confused:
            img.mean_map_type = 'zero'
          else:
            img.mean_map_type = 'const'
          mylogger.userinfo(mylog, 'Variation in mean image not significant')

        return img

    def map_2d(self, arr, out_mean, out_rms, mask=False,
               kappa=3, box=None, interp=1):
        """Calculate mean&rms maps and store them into provided arrays

        Parameters:
        arr: 2D array with data
        out_mean, out_rms: 2D arrays where to store calculated maps
        mask: mask
        kappa: clipping value for rms/mean calculations
        box: tuple of (box_size, box_step) for calculating map
        interp: order of interpolating spline used to interpolate 
                calculated map
        """
        axes, mean_map, rms_map = self.rms_mean_map(arr, mask, kappa, box)
        ax = map(self.remap_axis, arr.shape, axes)
        ax = N.meshgrid(*ax[-1::-1])
        nd.map_coordinates(mean_map, ax[-1::-1], order=interp, output=out_mean)
        nd.map_coordinates(rms_map,  ax[-1::-1], order=interp, output=out_rms)
      
        # Apply mask to mean_map and rms_map by setting masked values to NaN
        if isinstance(mask, N.ndarray):
            pix_masked = N.where(mask == True)
            out_mean[pix_masked] = N.nan
            out_rms[pix_masked] = N.nan

    def rms_mean_map(self, arr, mask=False, kappa=3, box=None):
        """Calculate map of the mean/rms values

        Parameters:
        arr:  2D array with data
        mask: mask
        kappa: clipping for calculating rms/mean within each box
        box: box parameters (box_size, box_step)

        Returns:
        axes: list of 2 arrays with coordinates of boxes alongside each axis
        mean_map: map of mean values
        rms_map: map of rms values

        Description:
        This function calculates clipped mean and rms maps for the array.
        The algorithm is a moving-window algorithm, where mean&rms are 
        calculated within a window of a size (box_size * box_size), and the
        window is stepped withing the image by steps of box_steps.

        Special care is taken for the borders of the image -- outer borders
        (where box doesn't fit properly) are given one extra round with a box
        applied to the border of the image. Additionally outer values are 
        extrapolated to cover whole image size, to simplify further processing.

        See also routine 'remap_axes' for 'inverting' axes array

        Example:
        for an input image of 100x100 pixels calling rms_mean_map with default
        box parameters (50, 25) will result in the following:

        axes = [array([  0. ,  24.5,  49.5,  74.5,  99. ]),
                array([  0. ,  24.5,  49.5,  74.5,  99. ])]

        mean_map = <5x5 array>
        rms_map  = <5x5 array>
 
        rms_map[1,1] is calculated for  arr[0:50, 0:50]
        rms_map[2,1] is calculated for  arr[25:75, 0:50]
        ...etc...
        rms_map[0,0] is extrapolated as .5*(rms_map[0,1] + rms_map[1,0])
        rms_map[0,1] is extrapolated as rms_map[1,1]
        """
        mylog = mylogger.logging.getLogger("PyBDSM.RmsMean")
        if box is None:
            box = (50, 25)
        if box[0] < box[1]:
            raise RuntimeError('Box size is less than step size.')

        # Some math first: boxcount is number of boxes alongsize each axis,
        # bounds is non-zero for axes which have extra pixels beyond last box
        BS, SS = box
        imgshape = N.array(arr.shape)

        # If boxize is less than 10% of image, use simple extrapolation to
        # derive the edges of the mean and rms maps; otherwise, use padded
        # versions of arr and mask to derive the mean and rms maps
        if float(BS)/float(imgshape[0]) < 0.1 and \
                float(BS)/float(imgshape[1]) < 0.1:
            use_extrapolation = True
            mylog.info('Using simple extrapolation for edges of mean '\
                           'and rms maps.')
        else:
            use_extrapolation = False
            mylog.info('Using padded array for edges of mean '\
                           'and rms maps.')

        if use_extrapolation:
            boxcount = 1 + (imgshape - BS)/SS
            bounds   = N.asarray((boxcount-1)*SS + BS < imgshape, dtype=int)
            mapshape = 2 + boxcount + bounds    
        else:                
            boxcount = 1 + imgshape/SS
            bounds   = N.asarray((boxcount-1)*SS < imgshape, dtype=int)
            mapshape = boxcount + bounds
            pad_border_size = int(BS/2.0)
            new_shape = (arr.shape[0] + 2*pad_border_size, arr.shape[1]
                         + 2*pad_border_size)
            arr_pad = self.pad_array(arr, new_shape)
            if mask == None:
                mask_pad = None
            else:
                mask_pad = self.pad_array(mask, new_shape)

        # Make arrays for calculated data
        mean_map = N.zeros(mapshape, dtype=float)
        rms_map  = N.zeros(mapshape, dtype=float)
        axes     = [N.zeros(len, dtype=float) for len in mapshape]          

        # Step 1: internal area of the image
        for i in range(boxcount[0]):
            for j in range(boxcount[1]):
                ind = [i*SS, i*SS+BS, j*SS, j*SS+BS]
                if use_extrapolation:
                    self.for_masked(mean_map, rms_map, mask, arr, ind,
                                    kappa, [i+1, j+1])
                else:
                    self.for_masked(mean_map, rms_map, mask_pad, arr_pad, ind,
                                    kappa, [i, j])

        # Check if all regions have too few unmasked pixels
        if mask != None and N.size(N.where(mean_map != N.inf)) == 0:
            raise RuntimeError("No unmasked regions from which to determine "\
                         "mean and rms maps.")

        # Step 2: borders of the image
        if bounds[0]:
            for j in range(boxcount[1]):
                if use_extrapolation:
                    ind = [-BS, arr.shape[0], j*SS,j*SS+BS]
                    self.for_masked(mean_map, rms_map, mask, arr, ind,
                                    kappa, [-2, j+1])
                else:
                    ind = [-BS, arr_pad.shape[0], j*SS,j*SS+BS]
                    self.for_masked(mean_map, rms_map, mask_pad, arr_pad, ind,
                                    kappa, [-1, j])

        if bounds[1]:
            for i in range(boxcount[0]):
                if use_extrapolation:
                    ind = [i*SS,i*SS+BS, -BS,arr.shape[1]]
                    self.for_masked(mean_map, rms_map, mask, arr, ind,
                                    kappa, [i+1, -2])
                else:
                    ind = [i*SS,i*SS+BS, -BS,arr_pad.shape[1]]
                    self.for_masked(mean_map, rms_map, mask_pad, arr_pad, ind,
                                    kappa, [i, -1])

        if bounds.all():
                if use_extrapolation:
                    ind = [-BS,arr.shape[0], -BS,arr.shape[1]]
                    self.for_masked(mean_map, rms_map, mask, arr, ind,
                                    kappa, [-2, -2])
                else:
                    ind = [-BS,arr_pad.shape[0], -BS,arr_pad.shape[1]]
                    self.for_masked(mean_map, rms_map, mask_pad, arr_pad, ind,
                                    kappa, [-1, -1])

        # Step 3: correct(extrapolate) borders of the image
        def correct_borders(map):
            map[0, :] = map[1, :]
            map[:, 0] = map[:, 1]
            map[-1, :] = map[-2, :]
            map[:, -1] = map[:, -2]

            map[0,0] = (map[1,0] + map[0, 1])/2.
            map[-1,0] = (map[-2, 0] + map[-1, 1])/2.
            map[0, -1] = (map[0, -2] + map[1, -1])/2.
            map[-1,-1] = (map[-2, -1] + map[-1, -2])/2.

        if use_extrapolation:
            correct_borders(mean_map)
            correct_borders(rms_map)

        # Step 4: fill in coordinate axes
        for i in range(2):
            if use_extrapolation:
                axes[i][1:boxcount[i]+1] = (N.arange(boxcount[i])*SS
                                            + BS/2. - .5)
                if bounds[i]:
                    axes[i][-2] = imgshape[i] - BS/2. - .5
            else:
                axes[i][0:boxcount[i]] = N.arange(boxcount[i])*SS - .5
                if bounds[i]:
                    axes[i][-2] = imgshape[i] - .5
            axes[i][-1] = imgshape[i] - 1

        # Step 5: fill in boxes with < 5 unmasked pixels (set to values of
        # N.inf)
        unmasked_boxes = N.where(mean_map != N.inf)
        if N.size(unmasked_boxes,1) < mapshape[0]*mapshape[1]:
            mean_map = self.fill_masked_regions(mean_map)
            rms_map = self.fill_masked_regions(rms_map)

        return axes, mean_map, rms_map


    def fill_masked_regions(self, themap, magic=N.inf):
        """Fill masked regions (defined where values == magic) in themap.
        """
        masked_boxes = N.where(themap == magic) # locations of masked regions
        for i in range(N.size(masked_boxes,1)):
            num_unmasked = 0
            x, y = masked_boxes[0][i], masked_boxes[1][i]
            delx = dely = 1
            while num_unmasked == 0:
                x1 = x - delx
                if x1 < 0: x1 = 0
                x2 = x + 1 + delx
                if x2 > themap.shape[0]: x2 = themap.shape[0]
                y1 = y - dely
                if y1 < 0: y1 = 0
                y2 = y + 1 + dely
                if y2 > themap.shape[1]: y2 = themap.shape[1]
         
                cutout = themap[x1:x2, y1:y2].ravel()
                goodcutout = cutout[cutout != magic]
                num_unmasked = N.alen(goodcutout)
                if num_unmasked > 0:
                    themap[x, y] = N.nansum(goodcutout)/float(len(goodcutout))
                delx += 1
                dely += 1
        themap[N.where(N.isnan(themap))] = 0.0
        return themap

    def pad_array(self, arr, new_shape):
        """Returns a padded array by mirroring around the edges."""
        # Assume that padding is the same for both axes and is equal
        # around all edges.
        half_size = (new_shape[0] - arr.shape[0]) / 2
        arr_pad = N.zeros( (new_shape), dtype=arr.dtype )

        # left band
        band = arr[:half_size, :]
        arr_pad[:half_size, half_size:-half_size] =  N.flipud( band )

        # right band
        band = arr[-half_size:, :]
        arr_pad[-half_size:, half_size:-half_size] = N.flipud( band )

        # bottom band
        band = arr[:, :half_size]
        arr_pad[half_size:-half_size, :half_size] = N.fliplr( band )

        # top band
        band = arr[:, -half_size:]
        arr_pad[half_size:-half_size, -half_size:] =  N.fliplr( band )

        # central band
        arr_pad[half_size:-half_size, half_size:-half_size] = arr

        # bottom left corner
        band = arr[:half_size,:half_size]
        arr_pad[:half_size,:half_size] = N.flipud(N.fliplr(band))

        # top right corner
        band = arr[-half_size:,-half_size:]
        arr_pad[-half_size:,-half_size:] = N.flipud(N.fliplr(band))

        # top left corner
        band = arr[:half_size,-half_size:]
        arr_pad[:half_size,-half_size:] = N.flipud(N.fliplr(band))

        # bottom right corner
        band = arr[-half_size:,:half_size]
        arr_pad[-half_size:,:half_size] = N.flipud(N.fliplr(band))

        return arr_pad

    def for_masked(self, mean_map, rms_map, mask, arr, ind, kappa, co):

        bstat = _cbdsm.bstat
        a, b, c, d = ind; i, j = co
        if mask == None:
          m, r, cm, cr, cnt = bstat(arr[a:b, c:d], mask, kappa)
          if cnt > 198: cm = m; cr = r
          mean_map[i, j], rms_map[i, j] = cm, cr
        else:                  
          pix_unmasked = N.where(mask[a:b, c:d] == False)
          npix_unmasked = N.size(pix_unmasked,1)
          if npix_unmasked > 20: # find clipped mean/rms
            m, r, cm, cr, cnt = bstat(arr[a:b, c:d], mask[a:b, c:d], kappa)
            if cnt > 198: cm = m; cr = r
            mean_map[i, j], rms_map[i, j] = cm, cr
          else:
            if npix_unmasked > 5: # just find simple mean/rms
              cm = N.mean(arr[pix_unmasked])
              cr = N.std(arr[pix_unmasked])
              mean_map[i, j], rms_map[i, j] = cm, cr
            else: # too few unmasked pixels --> set mean/rms to inf
              mean_map[i, j], rms_map[i, j] = N.inf, N.inf

          #return mean_map, rms_map
                
    def remap_axis(self, size, arr):
        """Invert axis mapping done by rms_mean_map

        rms_mean_map 'compresses' axes by returning short arrays with
        coordinades of the boxes. This routine 'inverts' this compression
        by calculating coordinates of each pixel of the original array
        within compressed array.

        Parameters:
        size: size of the original (and resulting) array
        arr : 'compressed' axis array from rms_mean_map

        Example:
        the following 'compressed' axis (see example in rms_mean_map):

           ax = array([  0. ,  24.5,  49.5,  74.5,  99. ])

        will be remapped as:

        print remap_axis(100, ax)
        [ 0.          0.04081633  0.08163265  0.12244898 ....
           ...............................................
          3.91836735  3.95918367  4.        ]

        which means that pixel 0 in the original image corresponds to pixels
        0 in the rms/mean_map array (which is 5x5 array).
        pixel 1 of the original image has coordinate of 0.04081633 in the
        compressed image (e.g. it has no exact counterpart, and it's value
        should be obtained by interpolation)
        """
        from math import floor, ceil
        res = N.zeros(size, dtype=float)

        for i in range(len(arr) - 1):
            i1 = arr[i]
            i2 = arr[i+1]
            t = N.arange(ceil(i1), floor(i2)+1, dtype=float)
            res[ceil(i1):floor(i2)+1] = i + (t-i1)/(i2-i1)

        return res
