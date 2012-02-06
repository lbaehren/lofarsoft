"""Module polarisation.

This module finds the Q, U, and V fluxes, the total, linear, and circular
polarisation fractions and the linear polarisation angle of each source identified
by gaul2srl. The position angle is defined from North, with positive angles
towards East.

"""

from image import *
from islands import *
from gausfit import Gaussian
from gaul2srl import *
import mylogger
import numpy as N
import functions as func
import statusbar

### Insert polarization attributes into Gaussian and Source classes
Gaussian.total_flux_Q        = Float(doc="Total flux(Jy), Stokes Q", colname='Total_Q',
                                   units='Jy')
Gaussian.total_fluxE_Q       = Float(doc="Error in total flux (Jy), Stokes Q", colname='E_Total_Q',
                                   units='Jy')
Gaussian.total_flux_U        = Float(doc="Total flux (Jy), Stokes U", colname='Total_U',
                                   units='Jy')
Gaussian.total_fluxE_U       = Float(doc="Error in total flux (Jy), Stokes U", colname='E_Total_U',
                                   units='Jy')
Gaussian.total_flux_V        = Float(doc="Total flux (Jy), Stokes V", colname='Total_V',
                                   units='Jy')
Gaussian.total_fluxE_V       = Float(doc="Error in total flux (Jy), Stokes V", colname='E_Total_V',
                                   units='Jy')
Gaussian.lpol_fraction       = Float(doc="Linear polarisation fraction", 
                                   colname='Linear_Pol_frac', units=None)
Gaussian.lpol_fraction_err   = Float(doc="Linear polarisation fraction error", 
                                   colname='E_Linear_Pol_frac', units=None)
Gaussian.cpol_fraction       = Float(doc="Circular polarisation fraction", 
                                   colname='Circ_Pol_Frac', units=None)
Gaussian.cpol_fraction_err   = Float(doc="Circular polarisation fraction error", 
                                   colname='E_Circ_Pol_Frac', units=None)
Gaussian.tpol_fraction       = Float(doc="Total polarisation fraction", 
                                   colname='Total_Pol_Frac', units=None)
Gaussian.tpol_fraction_err   = Float(doc="Total polarisation fraction error", 
                                   colname='E_Total_Pol_Frac', units=None)
Gaussian.lpol_angle          = Float(doc="Polarisation angle (deg from North towards East)", 
                                   colname='Linear_Pol_Ang', units='deg')
Gaussian.lpol_angle_err      = Float(doc="Polarisation angle error (deg)",
                                   colname='E_Linear_Pol_Ang', units='deg')

Source.total_flux_Q        = Float(doc="Total flux(Jy), Stokes Q", colname='Total_Q',
                                   units='Jy')
Source.total_fluxE_Q       = Float(doc="Error in total flux (Jy), Stokes Q", colname='E_Total_Q',
                                   units='Jy')
Source.total_flux_U        = Float(doc="Total flux (Jy), Stokes U", colname='Total_U',
                                   units='Jy')
Source.total_fluxE_U       = Float(doc="Error in total flux (Jy), Stokes U", colname='E_Total_U',
                                   units='Jy')
Source.total_flux_V        = Float(doc="Total flux (Jy), Stokes V", colname='Total_V',
                                   units='Jy')
Source.total_fluxE_V       = Float(doc="Error in total flux (Jy), Stokes V", colname='E_Total_V',
                                   units='Jy')
Source.lpol_fraction       = Float(doc="Linear polarisation fraction", 
                                   colname='Linear_Pol_frac', units=None)
Source.lpol_fraction_err   = Float(doc="Linear polarisation fraction error", 
                                   colname='E_Linear_Pol_frac', units=None)
Source.cpol_fraction       = Float(doc="Circular polarisation fraction", 
                                   colname='Circ_Pol_Frac', units=None)
Source.cpol_fraction_err   = Float(doc="Circular polarisation fraction error", 
                                   colname='E_Circ_Pol_Frac', units=None)
Source.tpol_fraction       = Float(doc="Total polarisation fraction", 
                                   colname='Total_Pol_Frac', units=None)
Source.tpol_fraction_err   = Float(doc="Total polarisation fraction error", 
                                   colname='E_Total_Pol_Frac', units=None)
Source.lpol_angle          = Float(doc="Polarisation angle (deg from North towards East)", 
                                   colname='Linear_Pol_Ang', units='deg')
Source.lpol_angle_err      = Float(doc="Polarisation angle error (deg)",
                                   colname='E_Linear_Pol_Ang', units='deg')

class Op_polarisation(Op):
    """ Finds the flux in each Stokes and calculates the polarisation fraction 
    and angle.

    Fluxes are calculated by summing all nonmasked pixels assigned to
    the Gaussian. If a pixel contains contributions from two or more
    Gaussians, its flux is divided between the Gaussians by the ratio of
    fluxes that they contribute to the pixel. Errors on the fluxes are
    derived by summing the same pixels in the rms maps in quadrature.
    The results are stored in the Gaussian and Source structure.

    Fits are also done to Q, U, and V images to determine if there are
    any islands of emission that lie outside those found in the I image.
    If there are, they are fit and the process above is done for them
    too.

    For linearly polarised emission, the signal and noise add
    vectorially, giving a Rice distribution (Vinokur 1965) instead of a
    Gaussian one. To correct for this, a bias is estimated and removed
    from the polarisation fraction using the same method used for the
    NVSS catalog (see ftp://ftp.cv.nrao.edu/pub/nvss/catalog.ps). Errors
    on the linear and total polarisation fractions and polarisation
    angle are estimated using the debiased polarised flux and standard
    error propagation. See Sparks & Axon (1999) for a more detailed
    treatment.

    Prerequisites: module gaul2srl should be run first."""

    def __call__(self, img):
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Polarisatn")
        if img.opts.polarisation_do:
          mylog.info('Extracting polarisation properties for all sources')
          
          # Run gausfit and gual2srl on Q, U, and V ch0 images to look for sources
          # undetected in I
          fit_QUV = False
          if fit_QUV:
              ch0_list = [img.ch0_Q, img.ch0_U, img.ch0_V]
              thresh_isl = img.opts.thresh_isl
              mask = img.mask
              for i, ch0_im in enumerate(ch0_list):
                  # For each polarization, find islands and check if any are new
                  rms = img.rms_QUV[i]
                  mean = img.mean_QUV[i]
                  
                  # act_pixels is true if significant emission
                  act_pixels = (ch0_im-mean)/thresh_isl >= rms
                  if isinstance(mask, N.ndarray):
                      act_pixels[mask] = False
    
                  rank = len(ch0_im.shape)
                                  # generates matrix for connectivity, in this case, 8-conn
                  connectivity = nd.generate_binary_structure(rank, rank)
                                  # labels = matrix with value = (initial) island number
                  labels, count = nd.label(act_pixels, connectivity)
                                  # slices has limits of bounding box of each such island
                  slices = nd.find_objects(labels)
          
                  ### apply cuts on island size and peak value
                  pyrank = N.zeros(ch0_im.shape, dtype=bool)
                  res = []
                  islid = 0
                  for idx, s in enumerate(slices):
                      idx += 1 # nd.labels indices are counted from 1
                                  # number of pixels inside bounding box which are in island
                      isl_size = (labels[s] == idx).sum()
                      isl_peak = nd.maximum(image[s], labels[s], idx)
                      isl_maxposn = tuple(N.array(N.unravel_index(N.argmax(image[s]), image[s].shape))+\
                                    N.array((s[0].start, s[1].start)))
                      if (isl_size >= minsize) and (isl_peak - mean[isl_maxposn])/thresh_pix > rms[isl_maxposn]:
                          isl = Island(image, mask, mean, rms, labels, s, idx)
                          res.append(isl)
                          pyrank[isl.bbox] += N.invert(isl.mask_active)
    
                  # Loop over island and check if they are also found in I
                  # for isl in res:
                      
              
                  pimg = Image(wopts)
                  pimg.pixel_beam = (wopts['beam'][0]/fwsig/cdelt[0], wopts['beam'][1]/fwsig/cdelt[1], wopts['beam'][2])
                  pimg.pixel_beamarea = 1.1331*wimg.pixel_beam[0]*wimg.pixel_beam[1]*fwsig*fwsig
                  pimg.pixel_restbeam = img.pixel_restbeam
                  pimg.ch0 = ch0_im
          
          # Get number of pixels per beam from img.beam (gives FWHM in deg) and img.beam2pix (deg->pix)
          gfactor = 2.0 * N.sqrt(2.0 * N.log(2.0))
          pixels_per_beam = 2.0 * N.pi * (img.beam2pix(img.beam)[0] * img.beam2pix(img.beam)[1]) / gfactor**2
          bar = statusbar.StatusBar('Calculating polarisation properties ....  : ', 0, img.nsrc)
          if img.opts.quiet == False:
              bar.start()

          for isl in img.islands:
            nsrc_in_island = len(isl.sources)
            ngaus_in_island = 0
            for src in isl.sources:
                  ngaus_in_island += src.ngaus
            # Cut out images for each island, subtract mean
            ch0_Q = img.ch0_Q[isl.bbox] #- img.mean_QUV[0][isl.bbox]
            ch0_U = img.ch0_U[isl.bbox] #- img.mean_QUV[1][isl.bbox]
            ch0_V = img.ch0_V[isl.bbox] #- img.mean_QUV[2][isl.bbox]
           
            for i, src in enumerate(isl.sources):
              # First, reconstruct sources from the Gaussians:
              if i == 0: # only need to do this once per island
                x1, x2 = N.mgrid[isl.bbox]
                tot_im = N.zeros((isl.shape[0], isl.shape[1]), dtype=float)
                for src_in_isl in isl.sources:
                  for gaus in src_in_isl.gaussians:
                    tot_im += func.gaussian_fcn(gaus, x1, x2)

              # Now, compare each Gaussian image pixel by pixel in the bbox, 
              # and assign a fractional flux to each.
              src_flux_Q = 0
              src_flux_U = 0
              src_flux_V = 0
              src_flux_Q_err_sq = 0
              src_flux_U_err_sq = 0
              src_flux_V_err_sq = 0

              for g, gaussian in enumerate(src.gaussians):
                  # First, make an array giving fractional contribution to the flux of the 
                  # current Gaussian:
                  g_im = func.gaussian_fcn(gaussian, x1, x2)
                  low_vals = N.where(g_im/N.max(g_im) < 0.1)
                  g_im[low_vals] = 0.0  
                  frac_flux = g_im / tot_im * ~isl.mask_active 
                  
                  in_current_gaus = N.where(frac_flux > 0.0) # indices of pixels assigned to current Gaussian
                  pixels_in_source = N.size(in_current_gaus) # number of umasked pixels assigned to current Gaussian
                  
                  # Sum pixels in the cutout ch0 images that are assigned to current Gaussian
                  flux_Q = N.sum(ch0_Q[in_current_gaus] * frac_flux[in_current_gaus])/pixels_per_beam # Jy
                  flux_Q_err = N.mean(img.rms_QUV[0][isl.bbox][in_current_gaus] * frac_flux[in_current_gaus]) * N.sqrt(pixels_in_source/pixels_per_beam) # Jy
                  flux_U = N.sum(ch0_U[in_current_gaus] * frac_flux[in_current_gaus])/pixels_per_beam # Jy
                  flux_U_err = N.mean(img.rms_QUV[1][isl.bbox][in_current_gaus] * frac_flux[in_current_gaus]) * N.sqrt(pixels_in_source/pixels_per_beam) # Jy
                  flux_V = N.sum(ch0_V[in_current_gaus] * frac_flux[in_current_gaus])/pixels_per_beam # Jy
                  flux_V_err = N.mean(img.rms_QUV[2][isl.bbox][in_current_gaus] * frac_flux[in_current_gaus]) * N.sqrt(pixels_in_source/pixels_per_beam) # Jy

#                   if gaussian.gaus_num == 48 or gaussian.gaus_num == 50:
#                     0/0
#                   # Check whether fluxes are significant (>= thresh_pix * error)
#                   if abs(flux_Q) < img.opts.thresh_pix * flux_Q_err:
#                       flux_Q = 0.0
#                   if abs(flux_U) < img.opts.thresh_pix * flux_U_err:
#                       flux_U = 0.0
#                   if abs(flux_V) < img.opts.thresh_pix * flux_V_err:
#                       flux_V = 0.0
                  
                  # Store fluxes and errors for each Gaussian in the source
                  gaussian.total_flux_Q = flux_Q
                  gaussian.total_flux_U = flux_U
                  gaussian.total_flux_V = flux_V
                  gaussian.total_fluxE_Q = flux_Q_err
                  gaussian.total_fluxE_U = flux_U_err
                  gaussian.total_fluxE_V = flux_V_err
                  
                  src_flux_Q += flux_Q
                  src_flux_U += flux_U
                  src_flux_V += flux_V
                  src_flux_Q_err_sq += flux_Q_err**2
                  src_flux_U_err_sq += flux_U_err**2
                  src_flux_V_err_sq += flux_V_err**2
                  
                  # Calculate and store polarisation fractions and angle for each Gaussian in the island
                  # For this we need the I flux, which we can just take from g.total_flux and src.total_flux
                  flux_I = gaussian.total_flux
                  flux_I_err = gaussian.total_fluxE
                  stokes = [flux_I, flux_Q, flux_U, flux_V]
                  stokes_err = [flux_I_err, flux_Q_err, flux_U_err, flux_V_err]
    
                  lpol_frac, lpol_frac_err = self.calc_lpol_fraction(stokes, stokes_err) # linear pol fraction
                  lpol_ang, lpol_ang_err = self.calc_lpol_angle(stokes, stokes_err) # linear pol angle
                  cpol_frac, cpol_frac_err = self.calc_cpol_fraction(stokes, stokes_err) # circular pol fraction
                  tpol_frac, tpol_frac_err = self.calc_tpol_fraction(stokes, stokes_err) # total pol fraction
    
                  gaussian.lpol_fraction = lpol_frac
                  gaussian.lpol_fraction_err = lpol_frac_err
                  gaussian.cpol_fraction = cpol_frac
                  gaussian.cpol_fraction_err = cpol_frac_err
                  gaussian.tpol_fraction = tpol_frac
                  gaussian.tpol_fraction_err = tpol_frac_err
                  gaussian.lpol_angle = lpol_ang
                  gaussian.lpol_angle_err = lpol_ang_err
       
              # Store fluxes for each source in the island
              src.total_flux_Q = src_flux_Q
              src.total_flux_U = src_flux_U
              src.total_flux_V = src_flux_V
              src.total_fluxE_Q = N.sqrt(src_flux_Q_err_sq)
              src.total_fluxE_U = N.sqrt(src_flux_U_err_sq)
              src.total_fluxE_V = N.sqrt(src_flux_V_err_sq)

              # Calculate and store polarisation fractions and angle for each source in the island
              # For this we need the I flux, which we can just take from g.total_flux and src.total_flux
              src_flux_I = src.total_flux
              src_flux_I_err = src.total_fluxE
              stokes = [src_flux_I, src_flux_Q, src_flux_U, src_flux_V]
              stokes_err = [src_flux_I_err, N.sqrt(src_flux_Q_err_sq), N.sqrt(src_flux_U_err_sq), N.sqrt(src_flux_V_err_sq)]

              lpol_frac, lpol_frac_err = self.calc_lpol_fraction(stokes, stokes_err) # linear pol fraction
              lpol_ang, lpol_ang_err = self.calc_lpol_angle(stokes, stokes_err) # linear pol angle
              cpol_frac, cpol_frac_err = self.calc_cpol_fraction(stokes, stokes_err) # circular pol fraction
              tpol_frac, tpol_frac_err = self.calc_tpol_fraction(stokes, stokes_err) # total pol fraction

              src.lpol_fraction = lpol_frac
              src.lpol_fraction_err = lpol_frac_err
              src.cpol_fraction = cpol_frac
              src.cpol_fraction_err = cpol_frac_err
              src.tpol_fraction = tpol_frac
              src.tpol_fraction_err = tpol_frac_err
              src.lpol_angle = lpol_ang
              src.lpol_angle_err = lpol_ang_err
              if bar.started:
                  bar.increment()


  ####################################################################################
    def calc_lpol_fraction(self, stokes, err):
        """ Calculate linear polarisation fraction and error from:
            stokes = [I, Q, U, V] and err = [Ierr, Qerr, Uerr, Verr]
        
        """
        I, Q, U, V = stokes
        Ierr, Qerr, Uerr, Verr = err
        QUerr = N.mean([Qerr, Uerr])
        if Q - Qerr < 0.0 and U - Uerr < 0.0:
            return 0.0, 0.0
        
        lpol = N.sqrt(Q**2 + U**2)
        lpol_debiased = self.debias(lpol, QUerr) # debias (to first order)
        if lpol_debiased > 0.0:
            lfrac = lpol_debiased / I
            dlfrac = lfrac * N.sqrt((Ierr/I)**2 + (Q*Qerr/lpol_debiased**2)**2 + (U*Uerr/lpol_debiased**2)**2)
        else:
            # if debiased fraction is consistent with zero, estimate a ballpark error with biased value
            lfrac = 0.0
            lpolsq = Q**2 + U**2
            dlfrac = N.sqrt(lpolsq) / I * N.sqrt((Ierr/I)**2 + (Q*Qerr/lpolsq)**2 + (U*Uerr/lpolsq)**2)

        return lfrac, dlfrac


  ####################################################################################
    def calc_cpol_fraction(self, stokes, err):
        """ Calculate circular polarisation fraction and error from:
            stokes = [I, Q, U, V] and err = [Ierr, Qerr, Uerr, Verr]
        
        """
        I, Q, U, V = stokes
        Ierr, Qerr, Uerr, Verr = err

        if I - Ierr < 0.0 or V - Verr < 0.0:
            return 0.0, 0.0
        cfrac = abs(V) / I
        dcfrac = cfrac * N.sqrt((Ierr/I)**2 + (Verr/V)**2)

        return cfrac, dcfrac


  ####################################################################################
    def calc_tpol_fraction(self, stokes, err):
        """ Calculate total polarisation fraction and error from:
            stokes = [I, Q, U, V] and err = [Ierr, Qerr, Uerr, Verr]
        
        """
        I, Q, U, V = stokes
        Ierr, Qerr, Uerr, Verr = err
        QUerr = N.mean([Qerr, Uerr])
        if Q - Qerr < 0.0 and U - Uerr < 0.0 and V - Verr < 0.0:
            return 0.0, 0.0

        lpol = N.sqrt(Q**2 + U**2)
        lpol_debiased = self.debias(lpol, QUerr)
        tpol_debiased = N.sqrt(Q**2 + U**2 + V**2) - (lpol - lpol_debiased) # debias (to first order)
        if tpol_debiased > 0.0:
            tfrac = tpol_debiased / I
            dtfrac = tfrac * N.sqrt((Ierr/I)**2 + (Q*Qerr/tpol_debiased**2)**2 + (U*Uerr/tpol_debiased**2)**2 + (V*Verr/tpol_debiased**2)**2)
        else:
            # if debiased fraction is consistent with zero, estimate a ballpark error with biased value
            tfrac = 0.0
            tpolsq = Q**2 + U**2 + V**2
            dtfrac = N.sqrt(tpolsq) / I * N.sqrt((Ierr/I)**2 + (Q*Qerr/tpolsq)**2 + (U*Uerr/tpolsq)**2 + (V*Verr/tpolsq)**2)

        return tfrac, dtfrac


  ####################################################################################
    def calc_lpol_angle(self, stokes, err):
        """ Calculate linear polarisation angle and error (in degrees) from:
            stokes = [I, Q, U, V] and err = [Ierr, Qerr, Uerr, Verr]
        
        """
        I, Q, U, V = stokes
        Ierr, Qerr, Uerr, Verr = err
        if Q - Qerr < 0.0 and U - Uerr < 0.0:
            return 0.0, 0.0

        ang = 0.5 * N.arctan2(U, Q) * 180.0 / N.pi
        dang = 0.5 / (1.0 + (U/Q)**2) * N.sqrt((Uerr/Q)**2 + (U*Qerr/Q**2)**2) * 180.0 / N.pi

        return ang, dang


  ####################################################################################
    def debias(self, pflux, QUerr):
        """ Debiases the linearly polarised flux using the same method
            used for the NVSS catalog (see ftp://ftp.cv.nrao.edu/pub/nvss/catalog.ps).
        
        """
        data_table=N.array([[1.253,1.2530], [1.256,1.1560], [1.266,1.0660], [1.281,0.9814],
                            [1.303,0.9030], [1.330,0.8304], [1.364,0.7636], [1.402,0.7023],
                            [1.446,0.6462], [1.495,0.5951], [1.549,0.5486], [1.606,0.5064],
                            [1.668,0.4683], [1.734,0.4339], [1.803,0.4028], [1.875,0.3749],
                            [1.950,0.3498], [2.027,0.3273], [2.107,0.3070], [2.189,0.2888],
                            [2.272,0.2724], [2.358,0.2576], [2.444,0.2442], [2.532,0.2321],
                            [2.621,0.2212], [2.711,0.2112], [2.802,0.2021], [2.894,0.1938],
                            [2.986,0.1861], [3.079,0.1791], [3.173,0.1726], [3.267,0.1666],
                            [3.361,0.1610], [3.456,0.1557], [3.551,0.1509], [3.646,0.1463],
                            [3.742,0.1420], [3.838,0.1380], [3.934,0.1342], [4.031,0.1306]])

        pnorm = pflux / QUerr
        if pnorm <= data_table[0,0]:
            bias = data_table[0,1]
        else:
            if pnorm >= data_table[-1,0]:
                bias = 1.0 / (2.0 * pnorm) + 1.0 / (8.0 * pnorm**3)
                pnorm = pnorm - bias
                bias = 1.0 / (2.0 * pnorm) + 1.0 / (8.0 * pnorm**3)
            else:
                bias = N.interp(pnorm, data_table[:,0], data_table[:,1])
                
        pflux_debiased = pflux - bias * QUerr
        
        return pflux_debiased
