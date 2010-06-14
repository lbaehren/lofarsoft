
"""Module Spectral index.
   See detailed notes at http://www/LofarWiki/doku.php?id=wiki:pybdsm_spin 
   for the implementation model.

   img.opts.beam_spectrum is None for now, assumed to be constant. Else is list of beam per channel,
   beam being list of 3.
"""

import numpy as N
from image import *
import mylogger
import debug_figs as df
#import test
import pyfits
from gaul2srl import Source
from copy import deepcopy as cp
import _cbdsm
import pylab as pl
import collapse 

specQC_phot = NArray(doc="Array of abs(fitted peak-expected value for str8 line fit excl data)/rms excl data")
raw_rms = Float(doc=" ")
phot_mask = NArray(doc = "")
beam_spec_av = Option(None, List(Tuple(Float(), Float(), Float())), doc = "syn. beam per averaged channel")
avimage = NArray(doc = "Averaged cube")
freq = NArray(doc = "Freqs of unaveraged cube")
case = String(doc = "Case of source")
freq_av = NArray(doc = "Freqs of averaged cube")
freq0 = Float(doc = "Fiducial frequency for calculating spectral indices")
spin1 = NArray(doc = "spectral index (1st order in log)")
espin1 = NArray(doc = "error in spectral index (1st order in log)")
spin2 = NArray(doc = "spectral index (2nd order in log)")
espin2 = NArray(doc = "error in spectral index (2nd order in log)")
take2nd = Bool(doc = "spectral index fit with 2nd order in log makes sense or not")
spec_descr = String(doc = "Description of source, if multiple")
specind_win_size = Int(doc = "Size of averaging window for spectral index of M sources")
specin_freq = NArray(doc = 'Frequency array used to calculate spectral index')
specin_flux = NArray(doc = 'Flux density array used to calculate spectral index')


        #check size vs freq wrt beam and see if consistent

class Op_spectralindex(Op):
    """Computes spectral index of every gaussian and every source """

    def __call__(self, img):

     mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"SpectIndex")
     if img.opts.spectralindex_do:
      shp = img.image.shape
      if len(shp) == 3 and shp[0] > 1:
                                                # calc freq, beam_spectrum for nchan channels
        self.freq_beamsp_unav(img)
                                                # calc initial channel flags if needed
        iniflags = self.iniflag(img)    
        img.iniflags = iniflags
                                                # preaverage to ~nchan0 channels; get modified beam sp
        if shp[0] >= 2*img.opts.spin_nchan0:  
            avimage, avimage_flags = self.pre_av(img, shp, iniflags)
            img.avimage_flags = avimage_flags
        else:   
            avimage = img.image
            img.opts.beam_spec_av = img.opts.beam_spectrum
            img.freq_av = img.freq
            img.freq0 = N.median(img.freq)
            img.avimage_flags = iniflags
            mylog.info('%s %i %s' % ('Kept all ',shp[0]," channels "))
        img.avimage = avimage
        pyfits.writeto(img.imagename + '.avimage.fits', N.transpose(img.avimage, (0,2,1)), img.header, clobber=True)
                                                # calculate the rms of each channel

        nchan = avimage.shape[0]
        rms_spec = self.rms_spectrum(img, avimage)

        for src in img.source:
          if src.code in ['S', 'C']:
            isl = img.islands[src.island_id]
            case, casepara = self.findcase(img, src, nchan, avimage, rms_spec)
            src.case = str(case)
            if case == 1: para, epara, q_spec, spin_para = self.sc_case1(img, src, avimage, isl.bbox, nchan, True, \
                               img.freq_av, img.opts.beam_spec_av, src.rms_chan)
            if case == 2 or case == 3: para, epara, q_spec, spin_para = self.sc_case23(img, src, casepara, avimage, nchan)
            src.spec_descr = " "; src.specind_win_size = 0
            gaus = src.gaussians[0]
            spin, espin, spin1, espin1, take2nd = spin_para
            gaus.spin1 = spin; gaus.espin1 = espin; gaus.spin2 = spin1; gaus.espin2 = espin1; gaus.take2nd = take2nd
        #self.globalastrometry(img)
        
        for src in img.source:
          if src.code == 'M':
            isl = img.islands[src.island_id]
            case, casepara = self.findcase(img, src, nchan, avimage, rms_spec)
            src.case = str(case)
            para, epara, q_spec, spin_para = self.msource(img, src, rms_spec, avimage, casepara, nchan)

####################################################################################
    def flagchans_rmschan(self, crms, zeroflags, iniflags, cutoff):
        """ Calculate clipped rms (r1) of the rms as fn of channel, crms, with zeroflags 
        applied and kappa=cutoff. Then exclude crms=0 (for NaN mages etc) and get ch.s
        which are more than cutoff*r1 away from median of rms. If this is less than 10 %
        of all channels, flag them. 
        
        #Also, if you include channels which are more than 
        #cutoff2*r1 away AND right next to a zeroflags (NaN image) channel,  and you get <10%
        #flags, flag that too. Obviously cutoff2 < cutoff to make sense. 
        
        """

                                                                # crms_rms and median dont include rms=0 channels
        nchan = len(crms)
        mean, rms, cmean, crms_rms = _cbdsm.bstat(crms, zeroflags, cutoff)
        zeroind = N.where(crms==0)[0]
        median = N.median(N.delete(crms, zeroind))
        badind = N.where(N.abs(N.delete(crms, zeroind) - median)/crms_rms >=cutoff)[0]
        frac = len(badind)/(nchan - len(zeroind))
        #for i in range(nchan):
            
        if frac <= 0.1: 
          badind = N.where(N.abs(crms - median)/crms_rms >=cutoff)[0]
          iniflags[badind] = True

        return iniflags

####################################################################################
    def iniflag(self, img):
        """ Calculate clipped rms of every channel, and then median and clipped rms of this rms distribution.
        Exclude channels where rms=0 (all pixels 0 or blanked) and of the remaining, if outliers beyond 5 sigma
        are less then 10 % of number of channels, flag them. This is done only when flagchan_rms = True. 
        If False, only rms=0 (meaning, entire channel image is zero or blanked) is flagged."""

        image = img.image
        nchan = image.shape[0]
        iniflags = N.zeros(nchan, bool)
        zeroflags = N.zeros(nchan, bool)
        crms = img.channel_clippedrms

        for ichan in range(nchan):
          if crms[ichan] == 0: zeroflags[ichan] = True
        iniflags = cp(zeroflags)

        if img.opts.flagchan_rms:
          iniflags = self.flagchans_rmschan(crms, zeroflags, iniflags, 4.0) 

        return iniflags
        

####################################################################################
    def freq_beamsp_unav(self, img):
        """ Defines img.opts.beam_spectrum and img.freq for the unaveraged cube. """

        shp = img.image.shape
        sbeam = img.opts.beam_spectrum 
        if sbeam != None and len(sbeam) != shp[0]: sbeam = None
        if sbeam == None:
          img.opts.beam_spectrum = [img.opts.beam]*shp[0]

        hdr = img.header
        nax = img.header['naxis']
        img.freq = N.zeros(shp[0])
        found = False
        for i in range(nax):
          s = str(i+1)
          if hdr['ctype'+s][0:4] == 'FREQ':
            crval, cdelt, crpix = hdr['CRVAL'+s], hdr['CDELT'+s], hdr['CRPIX'+s]
            found = True
            for ichan in range(shp[0]):
              ich = ichan+1
              img.freq[ichan] = crval+cdelt*(ich-crpix)
        if not found:
          mylog.critical("CTYPE = FREQ not found in header")
          raise RuntimeError("CTYPE = FREQ not found in header")


####################################################################################
    def pre_av(self, img, shp, iniflags):
        """ Pre-averages the image cube so that it has roughly img.opts.spin_nchan0 number of channels.
            Does this only when nchan > twice this value.
        """
        import math

        nchan0 = img.opts.spin_nchan0
        nchan = shp[0]
        fac = nchan/nchan0
        nn = nchan*1.0/fac
        remain = nn-math.floor(nn)
        if remain <= 0.5:
          new_n = int(math.floor(nn))
        else:
          new_n = int(math.ceil(nn))
        image = N.zeros((new_n, shp[1], shp[2]))

        d = collapse.windowaverage_cube(imagein=img.image, imageout=image, fac=fac, chanrms=img.channel_clippedrms, \
                iniflags=iniflags, c_wts='rms', kappa=img.opts.kappa_clip, sbeam=N.array(img.opts.beam_spectrum), 
                freqin=img.freq, calcrms_fromim=True)
        image, img.opts.beam_spec_av, freq_av, avimage_flags, crms_av = d
        img.freq_av = freq_av
        img.freq0 = N.median(img.freq_av)
        img.avimage_crms = crms_av
                                                   # flag channels based on high rms if asked for, again
        if img.opts.flagchan_rms:
          avimage_flags = self.flagchans_rmschan(crms_av, avimage_flags, avimage_flags, 3.0) 
                                                        # calc beams directly if needed
        if img.opts.beam_sp_derive:
          f0 = img.freq0
          bmaj, bmin, bpa = img.opts.beam
          for ichan in range(new_n):
            f = img.freq_av[ichan]
            img.opts.beam_spec_av[ichan] = tuple([bmaj*f0/f, bmin*f0/f, bpa])
           
        mylog.info('%i %s %i %s' % (nchan," channels first averaged to ",new_n," channels"))
        
        return image, avimage_flags

####################################################################################
    def rms_spectrum(self, img, image):
        from rmsimage import Op_rmsimage

        nchan = image.shape[0]
        rms_map = img.opts.rms_map
        map_opts = (img.opts.kappa_clip, img.opts.rms_box, img.opts.spline_rank)
        chanflag = img.avimage_flags

        if rms_map:
          rms_spec = N.zeros(image.shape)
          mean = N.zeros(image.shape[1:])
          rms = N.zeros(image.shape[1:])
          median_rms = N.zeros(nchan)
          for ichan in range(nchan):
            if not chanflag[ichan]:
              dumi = Op_rmsimage()
              Op_rmsimage.map_2d(dumi, image[ichan], mean, rms, None, *map_opts)
              rms_spec[ichan,:,:] = rms
              median_rms[ichan] = N.median(rms)
        else:
          rms_spec = N.zeros(nchan)
          avimage_crms = img.avimage_crms
          for ichan in range(nchan):
            if not chanflag[ichan]:
              rms_spec[ichan] = avimage_crms[ichan]
          median_rms = rms_spec
          ### plot for debugging
          if img.opts.debug_figs_3:
            pl.figure() 
            pl.subplot(211); pl.plot(img.channel_clippedrms, '*r-'); ind=N.where(img.iniflags)[0]; 
            pl.plot(N.arange(len(img.iniflags))[ind], img.channel_clippedrms[ind], '*b')
            pl.title('All channels (blue=flagged)')
            pl.subplot(212); pl.plot(avimage_crms, '*r-'); ind=N.where(chanflag)[0]; 
            pl.plot(N.arange(len(chanflag))[ind], avimage_crms[ind], '*b')
            pl.title('Averaged channels (blue=flagged)')

        str1 = " ".join(["%9.4e" % n for n in median_rms])
        if rms_map:        
          mylog.debug('%s %s ' % ('Median rms of channels : ', str1))
          mylog.info('RMS image made for each channel')
        else:
          mylog.debug('%s %s ' % ('RMS of channels : ', str1))
          mylog.info('Clipped rms calculated for each channel')

        return rms_spec

####################################################################################
    def findcase(self, img, src, nchan, image, rms_spec):
        """ Case I, II or III.

        S0 = ch0 flux; K = threshold
        rms_i = rms for channel i for island isl; S_i = maxflux in channel i.
        
        Case I  : S0 >= K * rms_i for all  i and S_i >= K * rms_i for all  i
        Case II : S0 >= K * rms_i for all  i and S_i >= K * rms_i for some i
        Case III: S0 >= K * rms_i for some i 
        """

        S0 = src.peak_flux_max
        K = img.opts.specind_kappa
        n,m = image.shape[1:]
        if img.opts.rms_map:
          rms_i = N.zeros(nchan)
          for ichan in range(nchan):
            rms_i[ichan] = rms_spec[ichan][src.bbox].mean()
        else:
          rms_i = rms_spec
        src.rms_chan = rms_i

        posn = img.sky2pix(src.posn_sky_max)
        x, y = N.array(posn).round()
        S_i = N.zeros(nchan)
        for ichan in range(nchan):
          S_i[ichan] = N.max(image[ichan,max(0,x-1):min(n,x+2),max(0,y-1):min(m,y+2)])
          
        cond1 = S0 >= K * rms_i 
        cond2 = S_i >= K * rms_i 
        flags = img.avimage_flags
        ind = N.where(~flags)[0]
        cond1 = cond1[ind]; cond2 = cond2[ind]

        case = 0
        if cond1.all() and cond2.all(): case = 1
        if cond1.all() and not cond2.all(): case = 2
        if not cond1.all(): case = 3

        return case, [S0, S_i, K, rms_i]

####################################################################################
    def findcase_1(self, fl, noise, mask):

        cond = fl >= noise
        ind = N.where(~mask)[0]
        cond = cond[ind]

        if cond.all(): 
          case = 1
        else:
          if cond.any(): 
            case = 2
          else:
            case = 3

        return case
####################################################################################
    def sc_case1(self, img, src, avimage, bbox, nchan, direct, freqarr, beams, src_rms_chan, chanmask = None):

      import functions as func
      from const import fwsig
      import math

      isl = img.islands[src.island_id]
      para = []; epara = []; mompara = []; 
      cdeltsq = abs(img.header['CDELT1']*img.header['CDELT2'])
      cbmaj, cbmin, cbpa = N.transpose(beams) 
      if chanmask == None: chanmask = img.avimage_flags
                                                      # get the region to fit, for each source. rank val=gausnum+1
      reg_rank, reg_bbox = self.get_case1_region(img, src)
      gaus = src.gaussians[0]
      g_id = gaus.gaussian_idx+1
      rmask = N.where(reg_rank == g_id, False, True)
      p_ini = func.g2param(gaus)
      p_ini[1] = p_ini[1]-isl.origin[0]; p_ini[2] = p_ini[2]-isl.origin[1]
      for ichan in range(nchan):
                                                      # fit a 6-para gaussian using scipy (not yet MGFunction)
        if not chanmask[ichan]: 
          chimage = avimage[ichan]
          data = chimage[bbox]
          x_ax, y_ax = N.indices(data.shape) 
          if N.isnan(data[N.where(isl.mask_active==False)]).any():
            p_fit = [float("NaN")]*7
            errors = [float("NaN")]*7
            chanmask[ichan] = True
          else:
            p_fit, ierr = func.fit_gaus2d(data, p_ini, x_ax, y_ax, rmask)
                                                        # calc total flux as well, for that channel
            p_fit = list(p_fit) 
            total = p_fit[0]*p_fit[3]*p_fit[4]/(cbmaj[ichan]*cbmin[ichan])*cdeltsq*fwsig*fwsig
            p_fit.append(total)
            errors = func.get_errors(img, p_fit, src_rms_chan[ichan])
          if img.opts.debug_figs_1: 
            df.test_spectralindex_chfit1(img, isl, data, rmask, x_ax, y_ax, p_ini, p_fit, ichan)
        else:
          p_fit = [float("NaN")]*7
          errors = [float("NaN")]*7
                                                        # add to list
        para.append(list(p_fit))
        epara.append(list(errors))

      if img.opts.debug_figs_2: 
        df.test_spectralindex_chfit2(para, epara, nchan, p_ini, src.island_id)
                                                        # get 6-moments
                                                        # check quality for astrometry and photometry
      q_spec = self.spectralquality(para, epara, src.island_id, freqarr, chanmask, img.opts.specind_dolog)

                                                        # quality control and flagging
      self.spectral_qc_flag(src, q_spec, chanmask) 
      if N.any(src.phot_mask): mylog.debug('%s %i %s %i %s ' % ('Island ',isl.island_id, ' : flagged ', \
         N.sum(src.phot_mask), ' channels for fitting spectral index'))

                                                        #fit sp.in., calc quality flag
      d = self.fit_spectralindex(img, src, para, epara, freqarr, img.opts.specind_dolog)
                                                        # spin for src itself
      srcslice = [slice(0, nchan, None)]+bbox
      im = avimage[srcslice]
      self.calc_src_spin(img, src, im, rmask, freqarr)
      ind1 = N.where(src.phot_mask == False)[0]
      gaus.specin_freq = freqarr[ind1]
      if img.opts.spin_flux =='peak': ind = 0
      else: ind = 6
      gaus.specin_flux = N.array(para)[:,ind][ind1]

      ### plots for debug_figs
      if img.opts.debug_figs_4: 
        if direct: 
          pl.figure()
          pl.title('Case I: source no. '+str(src.island_id))
        spin, espin, spin1, espin1, take2nd = d
        y = N.array(para)[:,6]; ey = N.array(epara)[:,6]; x = freqarr/img.freq0
        ind = N.where(~chanmask)[0]; ind1 = N.where(chanmask)[0]
        pl.plot(N.log10(x[ind]), N.log10(y[ind]), '*-b')
        pl.errorbar(N.log10(x[ind]), N.log10(y[ind]), ey[ind]/y[ind])
        pl.plot(N.log10(x[ind1]), N.log10([N.median(y[ind])]*len(ind1)), '*r')
        pl.plot(N.log10(x), N.log10(spin[0])+N.log10(x)*spin[1], '-g')
        pl.plot(N.log10(x), N.log10(spin1[0])+N.log10(x)*spin1[1]+N.log10(x)*N.log10(x)*spin1[2], '-m')

      return para, epara, q_spec, d
          

####################################################################################
    def sc_case23(self, img, src, casepara, avimage, nchan):
        """ First calculate expected flux at caseII channels. 
        
        If the end channels are caseII, just flag them. If just one channel is case II or, 
        apart from edge, only one channel in the middle is case II, flag it. Now if you dont 
        have any more caseII channels, proceed as in case I.
        If you still have caseII channels, then do the following:
        
        Compute expected spectrum using Case I points alone (case II can be due to curvature but 
        also due to high rms, and bad channels so fit straight line and use this conservatively).

        If all unflagged caseII channels are within 3sigma range of expected value, then its ok, just 
        average channels a bit and proceed since this can be due to curvature but is probably due to 
        high rms instead.

        If some channels are outside 3sigma expected range, then definitely curvature is important 
        (or bad channels but then should be flagged through some other method like really high 
        channel rms - DO THAT) and ASURV should be used.

        Also do case 3 here.

        """

        import functions as func
        from const import fwsig
        from math import sqrt

        minchan = img.opts.specind_minchan
        S0, S_i, K, rms_i = casepara
        freqs = img.freq_av; nf = len(freqs)
        chanmask = cp(img.avimage_flags)
        caseIind = N.where(S_i >= K*rms_i)[0]; caseIIind = N.where(S_i < K*rms_i)[0]
        if 0 in caseIIind: chanmask[0] = True
        if nf-1 in caseIIind: chanmask[-1] = True
        if len(caseIIind) == 1: chanmask[caseIIind[0]] = True
        c = list(cp(caseIIind))
        try: c.remove(0)
        except: pass
        try: c.remove(nf-1)
        except: pass
        if len(c) == 1 and c[0] in range(2,nf-2): chanmask[c[0]] = True
        isl = img.islands[src.island_id]

        sl0 = src.bbox
        srcslice = [slice(0, nchan, None)]+sl0
        gaus = src.gaussians[0]
        g_id = gaus.gaussian_idx+1
        reg_rank, reg_bbox = self.get_case1_region(img, src)
        rmask = N.where(reg_rank == g_id, False, True)
        do_log = img.opts.specind_dolog
                                                # no unflagged case II channels remain, proceed as in case I
        # set up debug plots first
        if img.opts.debug_figs_4:
          pl.figure(); pl.suptitle('Case '+src.case+': source no. '+str(src.island_id))
          y = cp(S_i); ey = rms_i; x = img.freq_av/img.freq0
          y = N.where(N.isnan(y), N.median(y), y)
          ind = N.where(~chanmask)[0]; ind1 = N.where(chanmask)[0]; pl.subplot(2,1,1); 
          pl.plot(N.log10(x), N.log10(y), '*-r'); 
          pl.plot(N.log10(x[ind]), N.log10(y[ind]), '*-b'); 
          pl.plot(N.log10(x[ind1]), N.log10(y[ind1]), '*r'); pl.subplot(2,1,2)

        if N.sum(chanmask[caseIIind]) == len(caseIIind): 
                                                # now fit as Case I
          para, epara, q_spec, spin_para = self.sc_case1(img, src, avimage, isl.bbox, nchan, False, img.freq_av, \
                          img.opts.beam_spec_av, src.rms_chan, chanmask)
          casetype='a'
        else:                                   # calculate 'expected' spectrum assuming linear sp.
          x = N.log10(freqs[caseIind]/img.freq0)
          if len(caseIind) < 3: 
            sp = -0.8; inter = N.mean(N.log10(S_i[caseIind]) - sp*x)
            fitp = N.array([inter, sp])
          else: 
            fitp, efitp = func.fit_mask_1d(x, N.log10(S_i[caseIind]), N.ones(len(caseIind)), \
                          N.zeros(len(caseIind), bool), func.poly, do_err=True, order=1)
          xall = N.log10(freqs/img.freq0)
          expected = N.power(10.0, fitp[0] + fitp[1]*xall)
          gooddata = abs(S_i-expected) < 3.0*rms_i
          unflaggedcaseIIchanind = N.array([i for i in caseIIind if ~chanmask[i]])
          asurv_nolog = False
                                        # if all caseII chan data are as expected then average and case I it or asurv
          if N.all(gooddata[unflaggedcaseIIchanind]):   
            win_size = func.get_windowsize_av(S_i, rms_i, chanmask, K, minchan)
            if win_size == 0:
              casetype='c'
              if do_log:
                mylog.error('Survival analysis for polynomial regression not yet implemented. ')
                mylog.error('Change img.opts.specin_dolog to False')
                mylog.error('Stopping PyBDSM now')
                self.asurv()
                import sys
                sys.exit()
              else:
                asurv_nolog = True
            else:          #av it and send it to case I
              casetype='b'
              new_nchan = nchan/win_size
              ipimage = avimage[srcslice]; opimage = N.zeros((new_nchan, ipimage.shape[1], ipimage.shape[2]))
              d = collapse.windowaverage_cube(imagein=ipimage, imageout=opimage, fac=win_size, chanrms=src.rms_chan, \
                iniflags=chanmask, c_wts='rms', kappa=img.opts.kappa_clip, sbeam=N.array(img.opts.beam_spec_av), 
                freqin=img.freq_av, calcrms_fromim=False)
              opimage, src_beam_spec_av, src_freq_av, src_avimage_flags, src_crms_av = d
              newslice = [slice(0, opimage.shape[i], None) for i in range(1,3)]
              para, epara, q_spec, spin_para = self.sc_case1(img, src, opimage, newslice, new_nchan, False, src_freq_av, \
                          src_beam_spec_av, src_crms_av, src_avimage_flags)
          else:
            casetype='c'
            if do_log:
              mylog.error('Survival analysis for polynomial regression not yet implemented. ')
              mylog.error('Change img.opts.specin_dolog to False')
              mylog.error('Stopping PyBDSM now')
              self.asurv()
              import sys
              sys.exit()
            else:
              asurv_nolog = True
              src.phot_mask = chanmask

          if asurv_nolog:
            n_caseI = len(caseIind); n_caseII = len(caseIIind)
            if n_caseII > 0.2*n_caseI:          # if not many case II chan, just get flux by summing, else average first
              win_size1 = func.get_windowsize_av(S_i, rms_i, chanmask, K=4.0, minchan=4)
              if win_size1 == 0: win_size1 = nchan/3
              new_nchan = nchan/win_size1
              ipimage = avimage[srcslice]; opimage = N.zeros((new_nchan, ipimage.shape[1], ipimage.shape[2]))
              d = collapse.windowaverage_cube(imagein=ipimage, imageout=opimage, fac=win_size1, chanrms=src.rms_chan, \
                iniflags=chanmask, c_wts='rms', kappa=img.opts.kappa_clip, sbeam=N.array(img.opts.beam_spec_av), 
                freqin=img.freq_av, calcrms_fromim=False)
              opimage, src_beam_spec_av, src_freq_av, src_avimage_flags, src_crms_av = d

              s_im = N.zeros(isl.image.shape); x1, x2 = N.mgrid[isl.bbox]
              for g in src.gaussians:
                s_im = s_im + func.gaussian_fcn(g, x1, x2)
              a, b = N.array(N.unravel_index(N.argmax(s_im), s_im.shape))  # stupid way of doing this ... 
              n, m = isl.mask_active.shape
              li = []
              for ia in range(a-2,a+3):
                for ib in range(b-2,b+3):
                  if 0 <= ia < n and 0 <= ib < m and not isl.mask_active[ia, ib]: li.append([ia, ib])
              li = N.array(li).transpose(); ind1 = (li[0], li[1])
              S_max = N.zeros(new_nchan)
              for i in range(new_nchan):
                S_max[i] = N.max(opimage[i][ind1])
            else:
              opimage, src_beam_spec_av, src_freq_av, src_avimage_flags, src_crms_av = avimage[srcslice], \
                       img.opts.beam_spec_av, img.freq_av, chanmask, src.rms_chan
              new_nchan = nchan
              S_max = S_i
            caseIind_1 = N.where(S_max >= 4.0*src_crms_av)[0]; caseIIind_1 = N.where(S_max < 4.0*src_crms_av)[0]
            beamarea = 1.1331*img.pixel_beam[0]*img.pixel_beam[1]*fwsig*fwsig
            cbmaj, cbmin, cbpa = N.transpose(src_beam_spec_av) 
            cdeltsq = abs(img.header['CDELT1']*img.header['CDELT2'])
            para = []; epara = []
            for ichan in range(new_nchan):
              if src_avimage_flags[ichan]:
                p_fit = [float("NaN")]*7
                errors = [float("NaN")]*7
              else:
                data = opimage[ichan]
                if ichan in caseIind_1:
                  x_ax, y_ax = N.indices(data.shape) 
                  p_ini = func.g2param(src.gaussians[0])
                  p_ini[1] = p_ini[1]-isl.origin[0]; p_ini[2] = p_ini[2]-isl.origin[1]
                  p_fit, ierr = func.fit_gaus2d(data, p_ini, x_ax, y_ax, rmask)
                  p_fit = list(p_fit) 
                  total = p_fit[0]*p_fit[3]*p_fit[4]/(cbmaj[ichan]*cbmin[ichan])*cdeltsq*fwsig*fwsig
                  p_fit.append(total)
                  errors = func.get_errors(img, p_fit, src_crms_av[ichan])
                else:
                  ind = N.where(rmask == False); p_fit = [1.]*7; errors = [1.]*7
                  p_fit[0] = p_fit[6] = N.sum(data[ind])/beamarea
                  errors[0] = errors[6] = isl.rms*sqrt(beamarea/len(ind[0]))
              para.append(list(p_fit))
              epara.append(list(errors))

            q_spec = self.spectralquality(para, epara, src.island_id, src_freq_av, src_avimage_flags, False)
            self.spectral_qc_flag(src, q_spec, src_avimage_flags) 
            src.phot_mask = src_avimage_flags
            spin_para = self.fit_spectralindex(img, src, para, epara, src_freq_av, False)
       
            src.phot_mask = chanmask
            self.calc_src_spin(img, src, avimage[srcslice], rmask, img.freq_av)
            ind1 = N.where(src.phot_mask == False)[0]
            gaus.specin_freq = img.freq_av[ind1]
            gaus.specin_flux = None

        src.case = src.case+casetype
        para = None; epara = None; q_spec = None; chanmask = None

        return para, epara, q_spec, spin_para
       
####################################################################################
    def asurv(self):
        """ Either put in an ASURV code here or change all the fitting to freq-flux instead of log-log and then
        you dont need ASURV at all."""

        print "ASURV : Needs to be implemented"

####################################################################################
    def msource(self, img, src, rms_spec, avimage, casepara, nchan):
        """
        Calculate source area in terms of beam_area (take sum of mask_active if all of island has only one source else 
        calc from the individual gaussians). If more than 10 then truly extended and mark as such. dont do sp.in. for
        each gaussian  but just for whole source and also calc sp.in map based on astrometry (centroid) and mark for wavelet.

        Else, do sp.in for gaussians and source as a whole. source as a whole -- just add all flux within mask and calc sp.in.

        Use C and S astrometry as fn of channel to figure out astrometric correction if any, later.

        For each gaussian, fix posn and size of each gaussian and fit only amplitudes and calc sp.in. for each. 
        Also, fix posn alone and let size vary and do for that too. 

        For whole source, it doesnt matter if its case 1 or 2. For each gaussian, need to average in order to fit gaussians. If each
        gaussian in case I, thats simple. Calculate this by just using the ch0 fluxes.
        If not, then take weakest gaussian and strongest gaussian - calc reqd averaging. If for 
        weakest, u need more than 4/8 channels, take that for everything. If u cant do weakest gaussian then take mean of channels
        for max (weakest, 4/8) and for strongest and average and fit. for those gaussians below detectability, fix the size always.

        get case for each gaussian. if all 1 then easy. if not, then calc winsize for weakest and if reasonable take that. if not
        reasonable then exclude gaussians below 1/15th of max gaussian and then redo. if still no good winsize for weakest but ok 
        for strongest then take mean and proceed and fix posn and size for weaker ones. if not even for strongest then extended.

        fitcode : 0=fit all; 1=fit amp; 2=fit amp, posn; 3=fit amp, size

        """
        import functions as func
        from math import pi, sqrt
        from const import fwsig

        isl = img.islands[src.island_id]                        # set up parameters
        S0, S_i, K, rms_i = casepara
        chanmask = cp(img.avimage_flags); nchan = len(S_i)
        srcslice = [slice(0, nchan, None)]+src.bbox
        image = avimage[srcslice]
        nsrc_in_isl = len(isl.source)
        bm_pix = img.beam2pix(src.size_sky); n_bm_pix = 2.0*pi/2.3548/2.3548*bm_pix[0]*bm_pix[1]
        ngaus = len(src.gaussians)
        if nsrc_in_isl == 1:
          srcmask = isl.mask_active
          npix = N.sum(~isl.mask_active)
        else:                                                   # If >1 src in island, compute mask, and area
          s_im = N.zeros(isl.image.shape); x1, x2 = N.mgrid[isl.bbox]
          for g in src.gaussians:
            s_im = s_im + func.gaussian_fcn(g, x1, x2)
          srcmask = N.where(s_im > src.rms_isl*img.opts.thresh_isl, False, True)
          npix = N.sum(srcmask)
        src_in_bms = npix/n_bm_pix                              # number of beams in the source
        extended = False
        if src_in_bms > 10.0:                                   # mark the source as truly extended if > 10 beams big
          extended = True
          src.spec_descr = "Extended source; too big"
        else:                                                   # else process further
          fitfix = N.zeros(ngaus)
          win_size = 1
          g_peaks = N.zeros(ngaus)
          gaucase = N.zeros(ngaus)
          for i, g in enumerate(src.gaussians):                 # find the case for each gaussian first
            g_peaks[i] = g.peak_flux
            gaucase[i] = self.findcase_1([g_peaks[i]]*nchan, K*rms_i, chanmask)
          if N.all(gaucase==1):                                 # If ALL gaussians are case I, no averaging reqd., fitfix=0
            src.spec_descr = "All gaussians are Case I with full spectral resolution"
          else:                                                 ### WIN_SIZE FOR THE SOURCE 
            winsizes = N.ones(ngaus, int)                            # Calc winsize for all gaussians
            for i, g in enumerate(src.gaussians):                 
              if gaucase[i] > 1:
                winsizes[i] = func.get_windowsize_av(N.ones(nchan)*g.peak_flux, rms_i, chanmask, K, img.opts.specind_minchan)
            if N.all(winsizes > 0):                             # all gaussians can be case I with averaging, fitfix=0
              win_size = N.max(winsizes)
              src.spec_descr = "All gaussians are Case I after averaging every "+str(win_size)+" channels"
            else:                                               # exclude gaussians < 1/15th of max and then see
              maxpk = N.max(g_peaks)
              brightgaus_ind = N.where(g_peaks > maxpk/10.0)[0]
              weakgaus_ind  = N.where(g_peaks <= maxpk/10.0)[0]
              dum = winsizes[brightgaus_ind]
              fitfix[weakgaus_ind] = 1                          # gaus < 0.1*max peak : fit only amp
              if N.all(dum > 0):                                # after exclusion, rest are case I
                win_size = N.max(dum)
                src.spec_descr = "Remaining "+str(len(brightgaus_ind))+" gaussians are Case I after excluding "+ \
                                  str(len(weakgaus_ind))+" weak gaussians"
              else:                                             # some bright gaussians also case II/III
                nonzero = [i for i in winsizes if i > 0]
                if len(nonzero) > 0:
                  win_size = 0.5*(min(nonzero)+max(nonzero))      # take mean window size of max and min
                  case2_g = N.where(winsizes > 0)[0]
                  mediumgaus_ind  = [i for i in range(ngaus) if i in brightgaus_ind and winsizes[i] == 0]
                  fitfix[mediumgaus_ind] = 3                        # gaus > 0.1*max peak but still not case I --> fix the posn
                  src.spec_descr = "Not all gaussians are Case I even after averaging"
            if N.all(winsizes ==0):                             # If no gaussian is strong enough even after averaging
              extended = True
              src.spec_descr = "No gaussian is bright enough even after averaging; take as extended"
          src.specind_win_size = win_size

                                                                # Now average the subcube if win_size > 1
        if not extended: 
          if win_size == 1:
            src_beam_spec_av, src_freq_av, src_avimage_flags, src_crms_av = img.opts.beam_spec_av, \
                               img.freq_av, chanmask, src.rms_chan
            new_nchan = nchan
          else:
            new_nchan = int(round(nchan/win_size))
            ipimage = avimage[srcslice]
            image = N.zeros((new_nchan, ipimage.shape[1], ipimage.shape[2]))    # now average
            d = collapse.windowaverage_cube(imagein=ipimage, imageout=image, fac=win_size, chanrms=src.rms_chan, \
                iniflags=chanmask, c_wts='rms', kappa=img.opts.kappa_clip, sbeam=N.array(img.opts.beam_spec_av), 
                freqin=img.freq_av, calcrms_fromim=False)
            image, src_beam_spec_av, src_freq_av, src_avimage_flags, src_crms_av = d    # averaged cube properties

          x, y = N.mgrid[isl.bbox]; para = []; epara = []
                                                        # exclude gaussians < factor*maxpeak from fit
          if img.opts.specind_Msrc_exclude1 > 1.0: img.opts.specind_Msrc_exclude1 = 0.0
          gg = []; ff = []
          fitgaus_ind = []
          for i, g in enumerate(src.gaussians):                 
            goodrms = src_crms_av[N.where(~src_avimage_flags)[0]]
            snr = g.peak_flux/goodrms
            if (g.peak_flux/N.max(g_peaks) >= img.opts.specind_Msrc_exclude1) and \
                N.mean(snr) >= img.opts.specind_Msrc_exclude2:
              gg.append(src.gaussians[i])
              ff.append(fitfix[i])
              fitgaus_ind.append(i)
          fitfix = N.array(ff)

          cbmaj, cbmin, cbpa = N.transpose(src_beam_spec_av) 
          cdeltsq = abs(img.header['CDELT1']*img.header['CDELT2'])
          for ichan in range(new_nchan):
            if not src_avimage_flags[ichan]:
              p, ep = func.fit_mulgaus2d(image[ichan], gg, x, y, srcmask, fitfix)
              total = N.zeros(len(fitfix))
              for ig in range(len(fitfix)):
                total[ig] = p[ig*6]*p[ig*6+3]*p[ig*6+4]/(cbmaj[ichan]*cbmin[ichan])*cdeltsq*fwsig*fwsig
                if p[ig*6] < 0: src_avimage_flags[ichan] = True  # hence doesnt matter that u have a value in para
              p = N.insert(p, N.arange(len(fitfix))*6+6, total)
              errors = func.get_errors(img, p, src_crms_av[ichan])
            else:
              p = [float("NaN")]*7*len(gg)
              errors = [float("NaN")]*7*len(gg)
            para.append(list(p))
            epara.append(list(errors))

          if img.opts.debug_figs_5:
            apara = N.array(para); tit = ['Amp', 'Posn x', 'Posn y', 'Bmaj', 'Bmin', 'Tot']
            inds = N.array([0,1,2,3,4,6])
            for i in range(len(gg)):
              pl.figure(); pl.suptitle('M src : '+str(src.island_id)+' gaus '+str(i))
              for ii in range(6): pl.subplot(2,3,ii+1); pl.plot(apara[:,i*7+inds[ii]], '*-g'); pl.title(tit[ii])

          q_spec = self.spectralquality(para, epara, src.island_id, src_freq_av, src_avimage_flags, img.opts.specind_dolog)
                                                        # quality control and flagging
          self.spectral_qc_flag(src, q_spec, src_avimage_flags)

          if N.any(src.phot_mask): mylog.debug('%s %i %s %i %s ' % ('Island ',isl.island_id, ' : flagged ', \
             N.sum(src.phot_mask), ' channels for fitting spectral index'))
                                                        #fit sp.in., calc quality flag
          pp = N.array(para); epp = N.array(epara)
          ctr = 0
          for i, g in enumerate(src.gaussians):                 
            if i in fitgaus_ind:
              g_par = pp[:,ctr*7:ctr*7+7]
              g_epar = epp[:,ctr*7:ctr*7+7]
              spin_para = self.fit_spectralindex(img, src, g_par, g_epar, src_freq_av, img.opts.specind_dolog)
              spin, espin, spin1, espin1, take2nd = spin_para
              g.spin1 = spin; g.espin1 = espin; g.spin2 = spin1; g.espin2 = espin1; g.take2nd = take2nd
              ctr += 1
              ind1 = N.where(src.phot_mask == False)[0]
              if img.opts.spin_flux =='peak': ind2 = 0
              else: ind2 = 6
              g.specin_freq = src_freq_av[ind1]
              g.specin_flux = g_par[:,ind2][ind1]
            else:
              g.spin1 = None; g.espin1 = None; g.spin2 = None; g.espin2 = None; g.take2nd = None
              g.specin_freq = None
              g.specin_flux = None
        else:                                                   # extended
          para = epara = q_spec = None; spin_para = [None]*5
          src.phot_mask = chanmask
          src_freq_av = img.freq_av
          ind1 = N.where(src.phot_mask == False)[0]
          g.specin_freq = None
          g.specin_flux = None
                                                                # spectral index for src as a whole
        self.calc_src_spin(img, src, image, srcmask, src_freq_av)
                            
        return para, epara, q_spec, spin_para
              
####################################################################################

    def get_case1_region(self, img, src):
      """ Sets mask and bbox for each source. for C sources, first assign each pixel 
          out to 1.0*fwhm of each gaussian to that source. This will be unique because 
          of same_island constraints. Next, out to f*FWHM, a pixel belongs to that 
          gaussian which gives it more flux. After that, the remaining pixels with
          mask_active=True will be assigned to the gaussian nearest to it.
      """
      import functions as func

      if src.island_id >= 0: 
        fac = 2.0
        isl = img.islands[src.island_id]
        if src.code == 'S':
            rank = ~isl.mask_active

        if src.code == 'C':
            rank_im = N.zeros(isl.image.shape)
            mask0 = N.zeros(isl.image.shape, bool)
            gau_ims = N.zeros((len(isl.gaul)+1, isl.image.shape[0], isl.image.shape[1]))
            mask_gauim = N.zeros(isl.image.shape, bool)
            for i,g in enumerate(isl.gaul):
              rank, dum = func.mask_fwhm(g, 0.0, 1.0, isl.origin, isl.image.shape) 
              rank_im = rank_im + rank*(g.gaussian_idx+1)
              mask0 = mask0 + rank
              dum, gau_ims[i+1,:,:] = func.mask_fwhm(g, 1.0, fac, isl.origin, isl.image.shape) 
              mask_gauim = mask_gauim + dum*~rank
            gau_ims = N.argmax(gau_ims, axis=0)

            mask3 = ~(mask0 + mask_gauim + isl.mask_active)
            rank = gau_ims*~mask0*~isl.mask_active + rank_im
            coords = N.transpose(N.where(mask3))
            if len(coords) > 0:
              for ind in coords:
                co = tuple(ind)
                minval = 99e9; minind = -1
                for i,g in enumerate(isl.gaul):
                  d1 = func.dist_2pt(N.array(g.centre_pix)-N.array(isl.origin), ind)
                  if d1 < minval:
                    minval = d1; minind = i+1
                rank[co] = minind

      return rank, isl.bbox

####################################################################################
    def spectralquality(self, para, epara, islid, freqarr, chanmask, do_log):
        """ 
        Check if astrometry, photometry etc is good for the source. para is array(nchan, 7n) 

        Calculate chisq for const=mean, line and 2d poly.
        Calculate chisq of line and poly over that of const, check if < 0.5
        Calculate rms around mean and straight line
        Calculate for each channel the (para-mean(except that para))/actual_std(except that para) and store
        for later use.
        """

        import functions as func
        import math

        def fn(ip, x, ex):
            if ip == 0 and do_log: 
              return N.log10(x), ex/x
            else:
              return x, ex

        aa = [len(g) for g in para]
        params = N.array(para) 
        eparams = N.array(epara)
        nchan = params.shape[0]
        nng = params.shape[1]
        if nng % 7 > 0: 
          mylog.error("Dimension of para not multiple of 7")
        ngaus = nng/7
        chisq = N.zeros((ngaus, 6)); chisq1 = N.zeros((ngaus, 6)); chisq2 = N.zeros((ngaus, 6) )
        dev_para = N.zeros((ngaus, nchan, 6)); dev_para1 = N.zeros((ngaus, nchan, 6))
        rms = N.zeros((ngaus, 6)); rms1 = N.zeros((ngaus, 6)); fitpara = N.zeros((ngaus, 6, 2))
        for ig in range(ngaus):
          pp = params[:, ig*7:ig*7+7]
          epp = eparams[:, ig*7:ig*7+7]

          xc = N.arange(nchan); xf = N.log10(freqarr)
          ind = N.where(N.array(chanmask) == False)[0]
          for ip in range(6):
            if do_log and ip == 0:
              funct = func.sp_in
            else:
              funct = func.poly
            if ip == 0:
              x = xf
            else:
              x = xc
            p, ep = fn(ip, pp[:, ip], epp[:, ip])
            mask = cp(chanmask)
            chisq[ig, ip]  = func.fit_chisq(x, p, ep, mask, func.nanmean, order=0)     # fit the mean
            chisq1[ig, ip] = func.fit_chisq(x, p, ep, mask, funct, order=1) # fit straight line
            chisq2[ig, ip] = func.fit_chisq(x, p, ep, mask, funct, order=2) # fit 2nd order
            rms[ig, ip] = N.std(p[ind])    # rms around mean
            fpara, errors = func.fit_mask_1d(x, p, ep, mask, funct, do_err=True, order=1)
            fit = funct(fpara, x)
            fitpara[ig, ip, :]=fpara
            tmp = p-fit
            rms1[ig, ip] = math.sqrt(N.sum(tmp[ind]*tmp[ind])/(len(ind)-1))   # rms around straight line fit
                                                        # deviation for each element excl itself
            dl = N.delete
            xf = x[ind]; pf = p[ind]; epf = ep[ind]; maskf = N.array(mask)[ind]; 
            for i in range(len(p)):
              if not chanmask[i]:
                                                        # deviation of ele from fit of mean excl it
                xcl = N.where(ind==i)[0][0]
                dev_para[ig, i,ip] = abs(p[i]-N.mean(dl(pf,xcl)))/N.std(dl(pf, xcl)) 
                                                        # deviation of ele from fit of line excl it
                fitpara1, errors = func.fit_mask_1d(dl(xf,xcl), dl(pf,xcl), dl(epf,xcl), dl(maskf,xcl), funct, \
                                do_err=True, order=1)
                fit = funct(fitpara1, dl(xf,xcl))
                rms_dl = math.sqrt(N.sum((dl(pf,xcl)-fit)*(dl(pf,xcl)-fit))/(len(pf)-1))   # rms around str8 line 
                fit = funct(fitpara1, N.array([x[i]]))
                dev_para1[ig, i,ip] = abs(p[i]-fit)/rms_dl
              else:
                dev_para[ig, i,ip] = -999; dev_para1[ig, i,ip] = -999

        q_spec = chisq, chisq1, chisq2, rms, rms1, fitpara, dev_para, dev_para1
        
        return q_spec
####################################################################################

    def spectral_qc_flag(self, src, q_spec, chanmask): 
        """ Store Quality control info on photometry and astrometry. 
        Flagging of channels based on statistics q_spec. """

        chisq, chisq1, chisq2, rms, rms1, fitpara, dev_para, dev_para1 = q_spec
        nsrc = len(chisq)
        nchan, npara = dev_para[0].shape
                                                        # for photometry
        phot_mask = cp(chanmask)
        for ichan in range(nchan):
          if not chanmask[ichan]:
            if N.all(dev_para1[:, ichan, 0] > 8.0):
              phot_mask[ichan] = True
        src.phot_mask = phot_mask

        para = [dev[:,0] for dev in dev_para1]
        src.specQC_photo = specQC_photo(para)
                                                        # for astrometry
        para = rms[:,1:3], [chisq1[:,1]/chisq[:,1], chisq1[:,2]/chisq[:,2]], [chisq2[:,1]/chisq[:,1], chisq2[:,2]/chisq[:,2]], \
               [fitpara[:,1,1], fitpara[:,2,1]], [dev_para[:,:,1], dev_para[:,:,2]], [dev_para1[:,:,1], dev_para1[:,:,2]]
        src.specQC_astro = specQC_astro(para)

####################################################################################
    def fit_spectralindex(self, img, src, para, epara, freqarr, do_log):
        """ Fits spectral index to data, with a mask, for 1st and 2nd order. For 2nd order, solution
        diverges very fast and hence is disabled for now. cflux=peak/total for sp.in of that flux.
        do_log is True/False implies you fit spectral index in logFlux vs logFreq space or not."""
        import functions as func
        import math
        from scipy.optimize import leastsq

        print 'Need to flag gaussians'
        cflux = img.opts.spin_flux
        if cflux=='peak': 
          ind = 0
        else:
          ind = 6
        x = freqarr
        f0 = img.freq0
        flux = N.array(para)[:,ind]
        eflux = N.array(epara)[:,ind]
        mask = src.phot_mask

        if do_log:
          x = N.log10(x/f0); y = N.log10(flux); sig = N.abs(eflux/flux)/2.303
          funct = func.poly
        else:
          x = x/f0; y = flux; sig = eflux
          funct = func.sp_in
        spin, espin = func.fit_mask_1d(x, y, sig, mask, funct, do_err=True, order=1)
        spin1, espin1 = func.fit_mask_1d(x, y, sig, mask, funct, do_err=True, order=2)
        chisq1 = func.calc_chisq(x, y, sig, spin, mask, funct, order=1)
        chisq2 = func.calc_chisq(x, y, sig, spin1, mask, funct, order=2)

        if abs(spin[1]-spin1[1]) > 0.1 and chisq2/chisq1 > 0.5: 
          take2nd = False 
        else:
          take2nd = True

        if do_log:
          spin[0] = math.pow(10.0, spin[0])
          espin[0] = spin[0]*math.log(10.0)*espin[0]
          spin1[0] = math.pow(10.0, spin1[0])
          espin1[0] = spin1[0]*math.log(10.0)*espin1[0]

        return spin, espin, spin1, espin1, take2nd

####################################################################################
    def calc_src_spin(self, img, src, image, imagemask, freqarr): 
        """ Fits spectral index to src itself. Since this is the only estimation method for weak source,
        dont do it in log space."""
        from const import fwsig
        from math import sqrt

        chanmask = src.phot_mask
        ind = N.where(imagemask == False)
        npix = len(ind[0])
        nchan = image.shape[0]
        flux = N.zeros(nchan); eflux = N.zeros(nchan)
        beamarea = 1.1331*img.pixel_beam[0]*img.pixel_beam[1]*fwsig*fwsig
        para = N.zeros((nchan, 7)); epara = N.ones((nchan, 7))

        isl = img.islands[src.island_id]
        for ichan in range(nchan):
          if not chanmask[ichan]:
            im = image[ichan]
            flux[ichan] = N.sum(im[ind])/beamarea
            para[ichan, 0] = para[ichan, 6] = flux[ichan]
            epara[ichan, 0] = epara[ichan,0] = isl.rms*sqrt(beamarea/npix)

        d = self.fit_spectralindex(img, src, para, epara, freqarr, False)
        spin, espin, spin1, espin1, take2nd = d
        src.spin1 = spin; src.espin1 = espin; src.spin2 = spin1; src.espin2 = espin1; src.take2nd = take2nd
        ind = N.where(src.phot_mask == False)[0]
        src.specin_freq = freqarr[ind]
        src.specin_flux = flux[ind]


####################################################################################
#def globalastrometry(self, img):
#
#    pass
#
#####################################################################################
                                                      
class specQC_astro(object):
    raw_rms = NArray(doc=" ")
    chisq_10 = List(NArray(), doc=" ")
    chisq_20 = List(NArray(), doc=" ")
    slope = List(NArray(), doc=" ")
    dev_para = List(NArray(), doc=" ")
    dev_para1 = List(NArray(), doc=" ")

    def __init__(self, para):
      raw_rms, chisq_10, chisq_20, slope, dev_para, dev_para1 = para
      self.raw_rms = raw_rms
      self.chisq_10 = chisq_10
      self.chisq_20 = chisq_20
      self.slope = slope
      self.dev_para = dev_para
      self.dev_para1 = dev_para1

####################################################################################
class specQC_photo(object):
    dev_para1 = List(NArray(), doc=" ")

    def __init__(self, para):
      dev_para1 = para
      self.dev_para1 = dev_para1
            
            
####################################################################################
Source.specQC_astro = tInstance(specQC_astro)
Source.specQC_photo = tInstance(specQC_photo)


