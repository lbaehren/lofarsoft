
"""Module gaul2srl

This will group gaussians in an island into sources. Will code callgaul2srl.f here, though
it could probably be made more efficient.

img.sources is a list of source objects, which are instances of the class Source 
(with attributes the same as in .srl of fbdsm). 
img.sources[n] is a source.
source.gaussians is the list of component gaussian objects.
source.island_id is the island id of that source.
source.source_id is the source id of that source, the index of source in img.sources. 
Each gaussian object gaus has gaus.source_id, the source id.

Also, each island object of img.islands list has the source object island.source 
"""

from image import *
from islands import *
import mylogger
import output_fbdsm_files as opf

nsrc = Int(doc="Number of sources in the image")
source_id = Int(doc="Source number of a gaussian")


class Op_gaul2srl(Op):
    """  
    Slightly modified from fortran. 
    """

    def __call__(self, img):
                        #  for each island, get the gaussians into a list and then send them to process
                        #  src_index is source number, starting from 0
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Gaul2Srl  ")
        src_index = -1
        sources = []
        for iisl, isl in enumerate(img.islands):
            isl_sources = []
            g_list = []
            for g in isl.gaul:
                if g.flag==0: g_list.append(g)

            if len(g_list) >0:
              if len(g_list) == 1: 
                src_index, source = self.process_single_gaussian(img, g_list, src_index, code = 'S')
                sources.append(source)
                isl_sources.append(source)
              else:
                src_index, source = self.process_CM(img, g_list, isl, src_index)
                sources.extend(source)
                isl_sources.extend(source)

            isl.source = isl_sources

        img.source = sources
        img.nsrc = src_index+1
        mylog.info("Grouped " + str(img.ngaus) + " gaussians into " + str(img.nsrc) + " sources")

        if img.opts.output_fbdsm: opf.write_fbdsm_gaul(img)

##################################################################################################

    def process_single_gaussian(self, img, g_list, src_index, code):
        """ Process single gaussian into a source, for both S and C type sources. g is just one
            Gaussian object (not a list)."""

        g = g_list[0] 

        total_flux = [g.total_flux, g.total_fluxE]
        peak_flux_centroid = peak_flux_max = [g.peak_flux, g.peak_fluxE]
        posn_sky_centroid = posn_sky_max = [g.centre_sky, g.centre_skyE]
        size_sky = [g.size_sky, g.size_skyE]
        #deconv_size_sky = [g.deconv_size_sky, g.deconv_size_skyE]
        #rms_isl = 
        #imrms = 
        bbox = img.islands[g.island_id].bbox
        ngaus = 1
        island_id = g.island_id
        gaussians = list([g])
        
        source_prop = list([code, total_flux, peak_flux_centroid, peak_flux_max, posn_sky_centroid, \
             posn_sky_max, size_sky, bbox, ngaus, island_id, gaussians])
        source = Source(img, source_prop)

        src_index += 1
        g.source_id = src_index
        source.source_id = src_index

        return src_index, source

##################################################################################################

    def process_CM(self, img, g_list, isl, src_index):
        """ 
        Bundle errors with the quantities. 
        ngau = number of gaussians in island
        src_id = the source index array for every gaussian in island
        nsrc = final number of distinct sources in the island
        """
        
        ngau = len(g_list)  # same as cisl in callgaul2srl.f
        nsrc = ngau         # same as islct; initially make each gaussian as a source
        src_id = N.arange(nsrc)  # same as islnum in callgaul2srl.f

        boxx, boxy = isl.bbox
        subn = boxx.stop-boxx.start; subm = boxy.stop-boxy.start
        delc = [boxx.start, boxy.start]
        subim = self.make_subim(subn, subm, g_list, delc)

        index = [(i,j) for i in range(ngau) for j in range(ngau) if j > i]
        for pair in index:
            same_island = self.in_same_island(pair, img, g_list, isl, subim, subn, subm, delc)
            if same_island:
              nsrc -= 1
              mmax, mmin = max(src_id[pair[0]],src_id[pair[1]]), min(src_id[pair[0]],src_id[pair[1]])
              arr = N.where(src_id == mmax)[0]; src_id[arr] = mmin
                            # now reorder src_id so that it is contiguous
        for i in range(ngau):
            ind1 = N.where(src_id==i)[0]
            if len(ind1) == 0:
                arr = N.where(src_id > i)[0]
                if len(arr) > 0:
                  decr =  N.min(src_id[arr])-i
                  for j in arr: src_id[j] -= decr
        nsrc = N.max(src_id)+1
                                # now do whats in sub_calc_para_source
                                # make mask and subim. Invalid mask value is -1 since 0 is valid srcid
        mask = self.make_mask(isl, subn, subm, nsrc, src_id, g_list, delc)

        source_list = []
        for isrc in range(nsrc):
          posn = N.where(src_id == isrc)[0]
          g_sublist=[]
          for i in posn:
              g_sublist.append(g_list[i])
          ngau_insrc = len(posn)
                                # Do source type C
          if ngau_insrc == 1:
              src_index, source = self.process_single_gaussian(img, g_sublist, src_index, code = 'C')
          else:
              src_index, source = self.process_Multiple(img, g_sublist, mask, src_index, isrc, subim, \
                                  isl, delc, subn, subm)
          source_list.append(source)

        return src_index, source_list

##################################################################################################

    def in_same_island(self, pair, img, g_list, isl, subim, subn, subm, delc):
        """ Whether two gaussians belong to the same source or not. """
        import functions as func

        def same_island_min(pair, g_list, subim, delc):
            """ If the minimum of the reconstructed fluxes along the line joining the peak positions 
                is greater than thresh_isl times the rms_clip, they belong to different islands. """

            g1 = g_list[pair[0]]
            g2 = g_list[pair[1]]
            pix1 = N.array(g1.centre_pix)
            pix2 = N.array(g2.centre_pix)

            x1, y1 = N.floor(pix1)-delc; x2, y2 = N.floor(pix2)-delc
            pix1 = N.array(N.unravel_index(N.argmax(subim[x1:x1+2,y1:y1+2]), (2,2)))+[x1,y1]
            pix2 = N.array(N.unravel_index(N.argmax(subim[x2:x2+2,y2:y2+2]), (2,2)))+[x2,y2]
            if pix1[1] >= subn: pix1[1] = pix1[1]-1
            if pix2[1] >= subm: pix2[1] = pix2[1]-1
            
            maxline = int(round(N.max(N.abs(pix1-pix2)+1)))
            flux1 = g1.peak_flux
            flux2 = g2.peak_flux
                                                # get pix values of the line
            pixdif = pix2 - pix1
            same_island_min = False
            same_island_cont = False
            if maxline == 1: 
              same_island_min = True
              same_island_cont = True
            else:
              if abs(pixdif[0]) > abs(pixdif[1]):
                xline = N.round(min(pix1[0],pix2[0])+N.arange(maxline))
                yline = N.round((pix1[1]-pix2[1])/(pix1[0]-pix2[0])* \
                       (min(pix1[0],pix2[0])+N.arange(maxline)-pix1[0])+pix1[1])
              else:
                yline = N.round(min(pix1[1],pix2[1])+N.arange(maxline))
                xline = N.round((pix1[0]-pix2[0])/(pix1[1]-pix2[1])* \
                       (min(pix1[1],pix2[1])+N.arange(maxline)-pix1[1])+pix1[0])

              rpixval = N.zeros(maxline)
              for i in range(maxline):
                pixval = subim[xline[i],yline[i]]
                rpixval[i] = pixval 
              min_pixval = N.min(rpixval)
              minind_p = N.argmin(rpixval)
              maxind_p = N.argmax(rpixval)
  
              if minind_p in (0, maxline-1) and maxind_p in (0, maxline-1): 
                same_island_cont = True

              if abs(min_pixval-min(flux1,flux2)) <= isl.rms*img.opts.thresh_isl:
                same_island_min = True

            return same_island_min, same_island_cont

        def same_island_dist(pair, g_list, hdr):
            """ If the centres are seperated by a distance less than half the sum of their 
                fwhms along the PA of the line joining them, they belong to the same island. """
            from math import sqrt
  
            g1 = g_list[pair[0]]
            g2 = g_list[pair[1]]
            pix1 = N.array(g1.centre_pix)
            pix2 = N.array(g2.centre_pix)
            gsize1 = g1.size_pix
            gsize2 = g2.size_pix
            
            fwhm1 = func.gdist_pa(pix1, pix2, gsize1)
            fwhm2 = func.gdist_pa(pix1, pix2, gsize2)
            dx = pix2[0]-pix1[0]; dy = pix2[1]-pix1[1]
            dist = sqrt(dy*dy + dx*dx)

            if dist <= 0.5*(fwhm1+fwhm2):
              same_island = True
            else:
              same_island = False

            return same_island

        same_isl1_min, same_isl1_cont = same_island_min(pair, g_list, subim, delc)
        same_isl2 = same_island_dist(pair, g_list, img.header)

        g1 = g_list[pair[0]]

        same_island = (same_isl1_min and same_isl2) or same_isl1_cont
        
        return same_island

##################################################################################################

    def process_Multiple(self, img, g_sublist, mask, src_index, isrc, subim, isl, delc, subn, subm):
        """ Same as gaul_to_source.f. isrc is same as k in the fortran version. """
        from math import pi, sqrt
        from const import fwsig
        import functions as func

        dum = img.opts.beam[0]*img.opts.beam[1]
        cdeltsq = abs(img.header['CDELT1']*img.header['CDELT2'])
        bmar_p = 2.0*pi*dum/(cdeltsq*fwsig*fwsig)

                                        # try
        subim_src = self.make_subim(subn, subm, g_sublist, delc)

        mompara = func.momanalmask_gaus(subim_src, mask, isrc, bmar_p, True)
                                        # initial peak posn and value
        maxv = N.max(subim_src)
        maxx, maxy = N.unravel_index(N.argmax(subim_src), subim_src.shape)
                                        # fit gaussian around this posn
        blc = N.zeros(2); trc = N.zeros(2)
        n, m = subim_src.shape[0:2]
        bm_pix = N.array([img.pixel_beam[0]*fwsig, img.pixel_beam[1]*fwsig, img.pixel_beam[2]])
        ssubimsize = max(N.int(N.round(N.max(bm_pix[0:2])*2))+1, 5)
        blc[0] = max(0, maxx-(ssubimsize-1)/2); blc[1] = max(0, maxy-(ssubimsize-1)/2)
        trc[0] = min(n, maxx+(ssubimsize-1)/2); trc[1] = min(m, maxy+(ssubimsize-1)/2)
        s_imsize = trc - blc + 1

        p_ini = [maxv, (s_imsize[0]-1)/2.0*1.1, (s_imsize[1]-1)/2.0*1.1, bm_pix[0]/fwsig*1.3, \
                 bm_pix[1]/fwsig*1.1, bm_pix[2]*2]
        data = subim_src[blc[0]:blc[0]+s_imsize[0], blc[1]:blc[1]+s_imsize[1]]
        smask = mask[blc[0]:blc[0]+s_imsize[0], blc[1]:blc[1]+s_imsize[1]]
        rmask = N.where(smask==isrc, False, True)
        x_ax, y_ax = N.indices(data.shape) 

        para, ierr = func.fit_gaus2d(data, p_ini, x_ax, y_ax, rmask)
                                        # stats
        if (0.0<para[1]<s_imsize[0]) and (0.0<para[2]<s_imsize[1]) and \
            para[3]<s_imsize[0] and para[4]<s_imsize[1]:
            maxpeak = para[0]
        else:
            maxpeak = maxv
        posn = para[1:3]-(0.5*N.sum(s_imsize)-1)/2.0+N.array([maxx, maxy])-1+delc
                                        # calculate peak by bilinear interpolation around centroid
        x1 = N.int(N.round(mompara[1])); y1 = N.int(N.round(mompara[2]))
        xind = slice(x1, x1+2, 1); yind = slice(y1, y1+2, 1)
        if N.sum(mask[xind, yind]==N.ones((2,2))*isrc) != 4:
            raise ValueError("Island number not in mask for gaus->source")
        t=(mompara[1]-x1)/(x1+1-x1)  # in case u change it later
        u=(mompara[2]-y1)/(y1+1-y1)
        s_peak=(1.0-t)*(1.0-u)*subim_src[x1,y1]+t*(1.0-u)*subim_src[x1+1,y1]+ \
               t*u*subim_src[x1+1,y1+1]+(1.0-t)*u*subim_src[x1,y1+1]
                                        # convert pixels to coords
        sra, sdec = img.pix2sky([mompara[1]+delc[0], mompara[2]+delc[1]])
        mra, mdec = img.pix2sky(posn)
                                        # "deconvolve" the sizes
        gaus_c = [mompara[3]/sqrt(cdeltsq)/fwsig, mompara[4]/sqrt(cdeltsq)/fwsig, mompara[5]]
        gaus_bm = [bm_pix[0]/fwsig, bm_pix[1]/fwsig, bm_pix[2]]
                ##############  ERRORS Not done yet
        gaus_dc = func.deconv(gaus_c, gaus_bm)

                                        # update all objects etc
        tot= 0.0
        for g in g_sublist:
            tot += g.total_flux
        size_sky = [mompara[3]*sqrt(cdeltsq), mompara[4]*sqrt(cdeltsq), mompara[5]+90.0]
        isl_id = isl.island_id
        source_prop = list(['M', [tot, 0.0], [s_peak, 0.0], [maxpeak, 0.0], [[sra, sdec], \
                      [0.0,0.0]], [[mra, mdec],[0,0]], [size_sky,[0,0,0]], isl.bbox,  \
                      len(g_sublist), isl_id, g_sublist])
        source = Source(img, source_prop)

        src_index += 1
        for g in g_sublist:
            g.source_id = src_index
        source.source_id = src_index

        return src_index, source

##################################################################################################

    def make_subim(self, subn, subm, g_list, delc):
        import functions as func

        subim = N.zeros((subn, subm))
        x, y = N.indices((subn, subm))
        for g in g_list:
            params = func.g2param(g)
            params[1] -= delc[0]; params[2] -= delc[1]
            gau = func.gaus_2d(params, x, y)
            subim = subim + gau

        return subim

##################################################################################################

    def make_mask(self, isl, subn, subm, nsrc, src_id, g_list, delc):
        import functions as func
                                        # define stuff for calculating gaussian
        boxx, boxy = isl.bbox
        subn = boxx.stop-boxx.start; subm = boxy.stop-boxy.start
        x, y = N.indices((subn, subm))
                                        # construct image of each source in the island
        src_image = N.zeros((subn, subm, nsrc))
        nn = 1
        for isrc in range(nsrc):
            posn = N.where(src_id == isrc)[0]
            g_sublist=[]
            for i in posn:
                g_sublist.append(g_list[i])
            for g in g_sublist:
                params = func.g2param(g)
                params[1] -= delc[0]; params[2] -= delc[1]
                gau = func.gaus_2d(params, x, y)
                src_image[:,:,isrc] = src_image[:,:,isrc] + gau
                                        # mark each pixel as belonging to one source 
                                        # just compare value, should compare with sigma later
        mask = N.argmax(src_image, axis=2)
        orig_mask = isl.mask_active
        mask[N.where(orig_mask)] = -1
                                        # add src_images for use in moments
        return mask


##################################################################################################
#  Define class Source
##################################################################################################

from image import *
from gausfit import Gaussian
from islands import Island

class Source(object):
    """ Instances of this class store sources made from grouped gaussians. """

    code                = String(doc='Source code S, C, or M')
    total_flux          = Float(doc="Total flux (Jy)")
    total_fluxE         = Float(doc="Error in total flux (Jy)")
    peak_flux_centroid  = Float(doc="Peak flux at centroid of emission (Jy/beam)")
    peak_flux_centroidE = Float(doc="Error in peak flux at centroid of emission (Jy/beam)")
    peak_flux_max       = Float(doc="Peak flux at posn of maximum emission (Jy/beam)")
    peak_flux_maxE      = Float(doc="Error in peak flux at posn of max emission (Jy/beam)")
    posn_sky_centroid   = List(Float(), doc="Posn (RA, Dec in deg) of centroid of source")
    posn_sky_centroidE  = List(Float(), doc="Error in posn (RA, Dec in deg) of centroid of source")
    posn_sky_max        = List(Float(), doc="Posn (RA, Dec in deg) of maximum emission of source")
    posn_sky_maxE       = List(Float(), doc="Error in posn (deg) of maximum emission of source")
    size_sky            = List(Float(), doc="")
    size_skyE           = List(Float(), doc="")
    deconv_size_sky     = List(Float(), doc="")
    deconv_size_skyE    = List(Float(), doc="")
    rms_isl             = Float(doc="")
    ngaus               = Int(doc='Number of gaussians in the source')
    island_id           = Int(doc="")
    gaussians           = List(tInstance(Gaussian), doc="")
    bbox                = List(Instance(slice(0), or_none=False), doc = "")

    def __init__(self, img, sourceprop):
    
        code, total_flux, peak_flux_centroid, peak_flux_max, posn_sky_centroid, \
                     posn_sky_max, size_sky,bbox, ngaus, island_id, gaussians = sourceprop
        self.code = code
        self.total_flux, self.total_fluxE = total_flux 
        self.peak_flux_centroid, self.peak_flux_centroidE = peak_flux_centroid 
        self.peak_flux_max, self.peak_flux_maxE = peak_flux_max 
        self.posn_sky_centroid, self.posn_sky_centroidE = posn_sky_centroid 
        self.posn_sky_max, self.posn_sky_maxE = posn_sky_max 
        self.size_sky, self.size_skyE = size_sky
        self.bbox = bbox
        self.ngaus = ngaus 
        self.island_id = island_id
        self.gaussians = gaussians
        self.rms_isl = img.islands[island_id].rms


Image.source = List(tInstance(Source), doc="List of Sources")
Island.source = List(tInstance(Source), doc="List of Sources")



