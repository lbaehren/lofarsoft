#! /usr/bin/env python
import pyfits
import numpy as np

def get_fluxcal_params(f):
    t = f['FLUX_CAL']
    nchan = t.header['NCHAN']
    nrcvr = t.header['NRCVR']
    data_cal = t.data.field('S_CAL').reshape((nrcvr,nchan))
    err_cal = t.data.field('S_CALERR').reshape((nrcvr,nchan))
    data_sys = t.data.field('S_SYS').reshape((nrcvr,nchan))
    err_sys = t.data.field('S_SYSERR').reshape((nrcvr,nchan))
    wts = t.data.field('DAT_WTS').reshape((nchan,))
    freq = t.data.field('DAT_FREQ').reshape((nchan,))
    return(freq,data_cal.T,err_cal.T,data_sys.T,err_sys.T,wts)

# First two fns work on pcm.fits

def get_cal_params(f):
    t = f['CAL_POLN']
    nchan = t.header['NCHAN']
    data = t.data.field('DATA').reshape((nchan,3))
    err  = t.data.field('DATAERR').reshape((nchan,3))
    wts  = t.data.field('DAT_WTS').reshape((nchan,))
    data = np.where(np.isfinite(data),data,0.0)
    return (data,err,wts)

def unpack_cov(raw,nchan,npar):
    cov = np.zeros((nchan,npar,npar))
    for c in range(nchan):
        ic = 0
        for i in range(npar):
            for j in range(i,npar):
                cov[c,i,j] = raw[c,ic]
                ic += 1
    return cov

def pack_cov(cov,nchan,npar):
    ncov = npar*(npar+1)/2
    raw = np.zeros((nchan,ncov))
    for c in range(nchan):
        ic = 0;
        for i in range(npar):
            for j in range(i,npar):
                raw[c,ic] = cov[c,i,j]
                ic += 1
    return raw
    
def get_feed_params(f):
    t = f['FEEDPAR']
    nchan = t.header['NCHAN']
    npar = t.header['NCPAR']
    ncovar = t.header['NCOVAR']
    names = [''] * npar
    for i in range(npar):
        names[i] = t.header['PAR_%4.4d' % i]
    data = t.data.field('DATA').reshape((nchan,npar))
    covraw = t.data.field('COVAR').reshape((nchan,ncovar))
    cov = unpack_cov(covraw,nchan,npar)
    freq = t.data.field('DAT_FREQ').reshape((nchan,))
    wts = t.data.field('DAT_WTS').reshape((nchan,))
    data = np.where(np.isfinite(data),data,0.0)
    return(names,freq,data,cov,wts)

from scipy.interpolate import UnivariateSpline, LSQUnivariateSpline

from optparse import OptionParser
usage = """%prog [options] file1 (file2 ...)

%prog smooths and interpolates PSRCHIVE calibration solutions 
(pcm and fluxcal output files)"""
par = OptionParser(usage=usage)
par.add_option("-e", "--ext", dest="ext",
        help="Extension to append to output file (default .int)",
        action="store", type="string", default="int")
par.add_option("-F", dest="freq_zap", 
        help="Specify extra freq range to zap (MHz)",
        action="append", type="string", default=None)
par.add_option("-k", "--order", dest="k",
        help="Spline polynomial order (default 3)",
        action="store", type="int", default=3)
par.add_option("-s", "--scale", dest="scale",
        help="Freq scale for spline boundaries (default auto)",
        action="store", type="float", default=None)
(opt,args) = par.parse_args()

for fname in args:

    print "Processing '%s'..." % fname
    f = pyfits.open(fname)

    # check if this is a fluxcal or pcm file
    fluxcal = False
    pcm = False
    try:
        (freq,scal,scal_err,ssys,ssys_err,wts) = get_fluxcal_params(f)
        fluxcal = True
    except KeyError:
        pass
    try:
        (cal_stokes,cal_stokes_err,cal_stokes_wts) = get_cal_params(f)
        (param_names,freq,feed_params,feed_params_cov,wts) = \
            get_feed_params(f)
        pcm = True
    except KeyError:
        pass

    nchan = len(freq)

    if pcm:
        nstokes = 3
        npar = len(param_names)
        
    if fluxcal:
        nrcvr = scal.shape[1]

    # Extra zap range
    if opt.freq_zap != None:
        for r in opt.freq_zap:
            zap_range = r.split(' ')
            f0 = float(zap_range[0])
            f1 = float(zap_range[1])
            for i in range(nchan):
                if freq[i] > f0 and freq[i] < f1:
                    wts[i] = 0
                    if pcm:
                        cal_stokes_wts[i] = 0

    # Pick out any other fluxcal bad chans
    if fluxcal:
        for i in range(nchan):
            if np.any(scal_err[i,:]==0.0): wts[i] = 0
            if np.any(ssys_err[i,:]==0.0): wts[i] = 0


    # Set up range that interpolation will cover
    idx = np.where(wts > 0.0)
    freq0 = freq[idx]
    idx_int = np.where(np.logical_and(freq>=freq0.min(), freq<=freq0.max()))

    # Explicit spline knots
    if opt.scale != None:
        t0 = np.arange(freq0.min() + opt.scale/2.0, freq0.max(), opt.scale)
        t = np.array([])
        # Remove any that occur in blanked regions
        for tt in t0:
            cidx = (np.abs(freq-tt)).argmin()
            if wts[cidx]!=0: 
                t = np.append(t,tt)

    if fluxcal:
        wts_int = 0.0 * wts
        wts_int[idx_int] = 1.0
        scal_int = 0.0 * scal
        scal_err_int = 0.0 * scal_err
        ssys_int = 0.0 * ssys
        ssys_err_int = 0.0 * ssys_err
        for ir in range(nrcvr):

            w0 = wts[idx] / scal_err[idx,ir]
            d0 = scal[idx,ir]
            if opt.scale != None:
                sp = LSQUnivariateSpline(freq0,d0,t,w0,k=opt.k)
            else:
                sp = UnivariateSpline(freq0,d0,w0,k=opt.k)
            scal_int[idx_int,ir] = sp(freq[idx_int])
            scal_err_int[idx_int,ir] = 0.1*np.median(scal_err[idx,ir])

            w0 = wts[idx] / ssys_err[idx,ir]
            d0 = ssys[idx,ir]
            if opt.scale != None:
                sp = LSQUnivariateSpline(freq0,d0,t,w0,k=opt.k)
            else:
                sp = UnivariateSpline(freq0,d0,w0,k=opt.k)
            ssys_int[idx_int,ir] = sp(freq[idx_int])
            ssys_err_int[idx_int,ir] = 0.1*np.median(ssys_err[idx,ir])
        
        # Fill in output arrays
        f['FLUX_CAL'].data.field('S_CAL')[0,:] = scal_int.T.flatten()
        f['FLUX_CAL'].data.field('S_CALERR')[0,:] = scal_err_int.T.flatten()
        f['FLUX_CAL'].data.field('S_SYS')[0,:] = ssys_int.T.flatten()
        f['FLUX_CAL'].data.field('S_SYSERR')[0,:] = ssys_err_int.T.flatten()
        f['FLUX_CAL'].data.field('DAT_WTS')[0,:] = wts_int.flatten()

    if pcm:

        # Interpolate feed params
        feed_wts_int = 0.0 * wts
        feed_wts_int[idx_int] = 1.0
        feed_params_int = 0.0 * feed_params
        feed_params_cov_int = 0.0 * feed_params_cov
        for ip in range(npar):
            if param_names[ip] == 'or0':
                feed_params_int[idx_int,ip] = feed_params[idx_int,ip]
                feed_params_cov_int[idx_int,ip,ip] = feed_params_cov[idx_int,ip,ip]
            else:
                w0 = wts[idx] * np.sqrt(1.0/feed_params_cov[idx,ip,ip])
                d0 = feed_params[idx,ip]
                if opt.scale != None:
                    sp = LSQUnivariateSpline(freq0,d0,t,w0,k=opt.k)
                else:
                    sp = UnivariateSpline(freq0,d0,w0,k=opt.k)
                feed_params_int[idx_int,ip] = sp(freq[idx_int])
                feed_params_cov_int[idx_int,ip,ip] = 0.1*np.median(feed_params_cov[idx,ip,ip])

        # Fill it into file
        covout = pack_cov(feed_params_cov_int,nchan,npar)
        f['FEEDPAR'].data.field('DATA')[0,:] = feed_params_int.flatten()
        f['FEEDPAR'].data.field('DAT_WTS')[0,:] = feed_wts_int.flatten()
        f['FEEDPAR'].data.field('COVAR')[0,:] = covout.flatten()

        # Interpolate cal stokes
        cal_stokes_int = 0.0 * cal_stokes
        idx = np.where(cal_stokes_wts > 0.0)
        freq0 = freq[idx]
        idx_int = np.where(np.logical_and(freq>=freq0.min(), freq<=freq0.max()))
        cal_stokes_wts_int = 0.0 * cal_stokes_wts
        cal_stokes_wts_int[idx_int] = 1.0
        cal_stokes_err_int = 0.0 * cal_stokes_err
        for ip in range(nstokes):
            w0 = cal_stokes_wts[idx] / cal_stokes_err[idx,ip]
            d0 = cal_stokes[idx,ip]
            if opt.scale != None:
                sp = LSQUnivariateSpline(freq0,d0,t,w0,k=opt.k)
            else:
                sp = UnivariateSpline(freq0,d0,w0,k=opt.k)
            cal_stokes_int[idx_int,ip] = sp(freq[idx_int])
            cal_stokes_err_int[idx_int,ip] = 0.1*np.median(cal_stokes_err[idx,ip])

        # Fill into file
        f['CAL_POLN'].data.field('DATA')[0,:] = cal_stokes_int.flatten()
        f['CAL_POLN'].data.field('DAT_WTS')[0,:] = cal_stokes_wts_int.flatten()
        f['CAL_POLN'].data.field('DATAERR')[0,:] = cal_stokes_err_int.flatten()

    # Write out new file
    f.writeto(fname + "." + opt.ext)

