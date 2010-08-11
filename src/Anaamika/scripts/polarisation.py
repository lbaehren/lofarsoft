"""Module polarisation.

This module finds the Q, U, and V fluxes, the total, linear, and circular
polarisation fractions and the linear polarisation angle of each source identified
by gaul2srl. The position angle is defined from North, with positive angles
towards East.

"""

from image import *
from islands import *
from gaul2srl import *
import mylogger
import numpy as N
import functions as func

### Insert attributes into Source class
Source.total_flux_Q        = Float(doc="Total flux(Jy), Stokes Q")
Source.total_fluxE_Q       = Float(doc="Error in total flux (Jy), Stokes Q")
Source.total_flux_U        = Float(doc="Total flux (Jy), Stokes U")
Source.total_fluxE_U       = Float(doc="Error in total flux (Jy), Stokes U")
Source.total_flux_V        = Float(doc="Total flux (Jy), Stokes V")
Source.total_fluxE_V       = Float(doc="Error in total flux (Jy), Stokes V")
Source.lpol_fraction       = Float(doc="Linear polarisation fraction")
Source.lpol_fraction_err   = Float(doc="Linear polarisation fraction error")
Source.cpol_fraction       = Float(doc="Circular polarisation fraction")
Source.cpol_fraction_err   = Float(doc="Circular polarisation fraction error")
Source.tpol_fraction       = Float(doc="Total polarisation fraction")
Source.tpol_fraction_err   = Float(doc="Total polarisation fraction error")
Source.lpol_angle          = Float(doc="Polarisation angle (deg from North towards East)")
Source.lpol_angle_err      = Float(doc="Polarisation angle error (deg)")

class Op_polarisation(Op):
    """ Finds the flux in each Stokes and calculates the polarisation fraction and angle.

    Fluxes are calculated by summing all nonmasked pixels assigned to the source.
    If a pixel contains contributions from two or more sources, its flux is assigned to
    the source with the largest contribution. Errors on the fluxes are derived
    by summing the same pixels in the rms maps in quadrature. The results are stored
    in the Source structure.

    For linearly polarised emission, the signal and noise add vectorially, giving a
    Rice distribution (Vinokur 1965) instead of a Gaussian one. To correct for this, a bias 
    is estimated and removed from the polarisation fraction using the same method used for the
    NVSS catalog (see ftp://ftp.cv.nrao.edu/pub/nvss/catalog.ps). Errors on the linear and total
    polarisation fractions and polarisation angle are estimated using the debiased polarised flux
    and standard error propagation. See Sparks & Axon (1999) for a more detailed treatment.

    Prerequisites: module gaul2srl should be run first."""

    def __call__(self, img):
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Polarisatn")
        if img.opts.polarisation_do:
          # Get number of pixels per beam from img.beam (gives FWHM in deg) and img.beam2pix (deg->pix)
          gfactor = 2.0 * N.sqrt(2.0 * N.log(2.0))
          pixels_per_beam = 2.0 * N.pi * (img.beam2pix(img.beam)[0] * img.beam2pix(img.beam)[1]) / gfactor**2

          for isl in img.islands:
            nsrc_in_island = len(isl.source)
            # Cut out images for each island and find mean rms
            ch0_Q = img.ch0_Q[isl.bbox]
            ch0_U = img.ch0_U[isl.bbox]
            ch0_V = img.ch0_V[isl.bbox]
           
            for i, src in enumerate(isl.source):
              if nsrc_in_island == 1:
                # Just one source in the island: add up all unmasked pixels
                ind = N.where(~isl.mask_active)
                pixels_in_source = N.size(ind) # number of unmasked pixels assigned to current source
                flux_Q = N.sum(ch0_Q[ind])/pixels_per_beam # Jy
                flux_Q_err = N.mean(img.rms_QUV[0][isl.bbox]) * N.sqrt(pixels_in_source/pixels_per_beam) # Jy
                flux_U = N.sum(ch0_U[ind])/pixels_per_beam # Jy
                flux_U_err = N.mean(img.rms_QUV[1][isl.bbox]) * N.sqrt(pixels_in_source/pixels_per_beam) # Jy
                flux_V = N.sum(ch0_V[ind])/pixels_per_beam # Jy
                flux_V_err = N.mean(img.rms_QUV[2][isl.bbox]) * N.sqrt(pixels_in_source/pixels_per_beam) # Jy
              else:
                # More than one source in the island: assign every pixel to a unique source.
                # First, reconstruct sources from the gaussians
                if i == 0: # only need to do this once per island
                  x1, x2 = N.mgrid[isl.bbox]
                  s_im = N.zeros((nsrc_in_island,isl.shape[0],isl.shape[1]), dtype=float)
                  for j, src_in_isl in enumerate(isl.source):
                    for g in src_in_isl.gaussians:
                      s_im[j,:,:] = s_im[j,:,:] + func.gaussian_fcn(g, x1, x2)

                # Now, compare each source image (s_im) pixel by pixel in the bbox, 
                # and only sum those that are max for current source and unmasked.
                #
                # First, make an array giving index (starting from 1) of source that is
                # maximum in a given pixel, with zeros where mask = True:
                index_of_max = (s_im.argmax(axis=0) + 1) * ~isl.mask_active 
                in_current_src = N.where(index_of_max == (i + 1)) # indices of pixels assigned to current source
                pixels_in_source = N.size(in_current_src) # number of umasked pixels assigned to current source

                # Sum pixels in the temp images that are assigned to current source
                flux_Q = N.sum(ch0_Q[in_current_src])/pixels_per_beam # Jy
                flux_Q_err = N.mean(img.rms_QUV[0][isl.bbox]) * N.sqrt(pixels_in_source/pixels_per_beam) # Jy
                flux_U = N.sum(ch0_U[in_current_src])/pixels_per_beam # Jy
                flux_U_err = N.mean(img.rms_QUV[1][isl.bbox]) * N.sqrt(pixels_in_source/pixels_per_beam) # Jy
                flux_V = N.sum(ch0_V[in_current_src])/pixels_per_beam # Jy
                flux_V_err = N.mean(img.rms_QUV[2][isl.bbox]) * N.sqrt(pixels_in_source/pixels_per_beam) # Jy

              # Store fluxes and errors for each source in the island
              src.total_flux_Q = flux_Q
              src.total_flux_U = flux_U
              src.total_flux_V = flux_V
              src.total_fluxE_Q = flux_Q_err
              src.total_fluxE_U = flux_U_err
              src.total_fluxE_V = flux_V_err

              # Calculate and store polarisation fractions and angle for each source in the island
              # For this we need the I flux, which we can just take from src.total_flux
              flux_I = src.total_flux
              flux_I_err = src.total_fluxE
              stokes = [flux_I, flux_Q, flux_U, flux_V]
              stokes_err = [flux_I_err, flux_Q_err, flux_U_err, flux_V_err]

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
              

  ####################################################################################
    def calc_lpol_fraction(self, stokes, err):
        """ Calculate linear polarisation fraction and error from:
            stokes = [I, Q, U, V] and err = [Ierr, Qerr, Uerr, Verr]
        
        """
        I, Q, U, V = stokes
        Ierr, Qerr, Uerr, Verr = err
        QUerr = N.mean([Qerr, Uerr])
        
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
