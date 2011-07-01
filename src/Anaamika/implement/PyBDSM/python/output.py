
"""Module output.

Writes results of source detection in a variety of formats.
"""

from image import Op
import os
import pyfits
import fbdsm_fitstable_stuff as stuff
import mylogger

class Op_outlist(Op):
    """Write out list of gaussians

    Currently 5 output formats are supported:
    - BBS list
    - fbdsm gaussian list
    - star list
    - kvis annotations
    - ascii

    All output lists are generated atm.
    """
    def __call__(self, img):
        if img.opts.output_all:
            dir = img.basedir + '/catalogues/'
            if not os.path.exists(dir): os.mkdir(dir)
            self.write_bbs(img, dir)
            self.write_gaul(img, dir)
            self.write_star(img, dir)
            self.write_kvis_ann(img, dir)
            self.write_opts(img, img.basedir + '/misc/')
            self.write_gaul_FITS(img, dir)


    def write_bbs(self, img, dir):
        """ Writes the gaussian list as a bbs-readable file"""
        import numpy as N
        from const import fwsig
        import functions as func

        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Output")

        prefix = ''
        if img.extraparams.has_key('bbsprefix'): prefix = img.extraparams['bbsprefix']+'_'
        if img.extraparams.has_key('bbsname'):
            name = img.extraparams['bbsname']
        else:
            name = img.imagename
        fnames = [dir + name + '.sky_in']
        if img.extraparams.has_key('bbsprefix'): 
          fnames.append(dir + img.parentname + '.pybdsm' + '.total.sky_in')
        else:
          fnames.append(dir + name + '.total.sky_in')
            
        if img.opts.spectralindex_do: freq = "%.5e " % img.freq0
        else: freq = "%.5e" % img.cfreq
        bbs_patches = img.opts.bbs_patches
        if bbs_patches == None: 
          patch_s = ''
          patchname = ''
        else:
          patch_s = 'Patch, '
        type = 'GAUSSIAN'
        sep = ', '
        if img.opts.srcroot == None:
            sname = img.imagename.split('.')[0]
        else:
            sname = img.opts.srcroot
        ### sort them in descending order of flux
        max = [] 
        for src in img.source: max.append(src.total_flux)
        ind = list(N.argsort(max)); ind.reverse()
        p_nums = N.arange(len(ind)) + img.bbspatchnum + 1
        img.bbspatchnum = img.bbspatchnum + len(ind)
        type = 'GAUSSIAN'  # if you include POINT you need to change the code which writes *.total.sky_in

        for fno, fname in enumerate(fnames):
          if not img.extraparams.has_key('bbsappend'): img.extraparams['bbsappend'] = False
          old = os.path.exists(fname)
          if fno == 1: 
            f = open(fname, 'a')
          else:
            if img.extraparams['bbsappend']:
              f = open(fname, 'a')
            else:
              f = open(fname, 'w')
              mylog.info('Writing ' + fname)

          if not old:
            str1 = "# (Name, Type, "+patch_s+"Ra, Dec, I, Q, U, V, ReferenceFrequency='"+freq+"', SpectralIndexDegree='0', " \
                  + "SpectralIndex:0='0.0', MajorAxis, MinorAxis, Orientation) = format\n"
            f.write(str1)

          if bbs_patches == 'single': 
            patchname = 'patch_'+img.parentname+', '
            str1 =  ', , patch_'+img.parentname+', 00:00:00, +00.00.00 \n' 
            if (prefix == '' and fno == 0) or prefix == 'w1_':
              f.write(str1)

          for iii, ii in enumerate(ind):
            src = img.source[ii]
            if bbs_patches == 'separate' and src.ngaus > 0: 
              str1 = ', , patch_'+img.parentname+'_'+str(p_nums[iii])+', 00:00:00, +00.00.00 \n'; 
              f.write(str1)
              patchname = 'patch_'+img.parentname+'_'+str(p_nums[iii])+', '
            for g in src.gaussians:
              src = sname + '_' + prefix + str(g.gaus_num)
              ra, dec = g.centre_sky
              ra = ra2hhmmss(ra)
              sra = str(int(ra[0]))+':'+str(int(ra[1]))+':'+str("%.3f" % (ra[2]))
              dec= dec2ddmmss(dec)
              decsign = ('-' if dec[3] < 0 else '+')
              sdec = decsign+str(int(dec[0]))+'.'+str(int(dec[1]))+'.'+str("%.3f" % (dec[2]))
              total = str("%.3e" % (g.total_flux))
              pol = '0.0, 0.0, 0.0, '
              deconv = g.deconv_size_sky
              deconv1 = str("%.5e" % (deconv[0]*3600.0))   # in arcsec
              deconv2 = str("%.5e" % (deconv[1]*3600.0)) 
              deconv3 = str("%.5e" % (deconv[2])) 
              deconv = deconv1 + ', ' + deconv2 + ', ' + deconv3
              specin = '-0.8'
              if img.opts.spectralindex_do: 
                specin = "%.2f" % g.spin1[1]
                total = str("%.3e" % (g.spin1[0]))
                
              str1 = src+ sep+ type+ sep+ patchname + sra+ sep+ sdec+sep+ total+ sep+ pol+ \
                              freq+ sep + '0'+ sep+ specin + sep+ deconv + '\n'
              f.write(str1)
          f.close()
  

    def write_gaul(self, img, dir):
            
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Output    ")
        fname = img.imagename + '.gaul'
        f = open(dir+fname, "w")
        mylog.info('Writing '+dir+fname)

        for g in img.gaussians:
            gidx = g.gaus_num-1  # python numbering
            iidx = g.island_id
            A = g.total_flux
            ra, dec = g.centre_sky
            x, y = g.centre_pix
            shape = g.size_sky
            eA = g.total_fluxE
            era, edec = g.centre_skyE
            ex, ey = g.centre_pixE
            eshape = g.size_skyE
            rms = g.rms

            str = "%3d  %4d  %d    %10g %5g   %10g %5g   " \
                  "%10g %5g   %10g %5g   %10g %5g   %10g %5g   " \
                  "%10g %5g   %10g %5g   %10g %5g  %10g\n" % \
                  (gidx, iidx, 0,    0, 0,     A,  eA, \
                   ra, era,     dec, edec,    x, ex,  y, ey, \
                   shape[0], eshape[0], shape[1], eshape[1],  shape[2], eshape[2], rms)
            f.write(str)

        f.close()

    def write_star(self, img, dir):

        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Output    ")
        fname = img.imagename + '.star'
        f = open(dir+fname, 'w')
        mylog.info('Writing '+dir+fname)

        for g in img.gaussians:#():
            A = g.peak_flux
            ra, dec = g.centre_sky
            shape = g.size_sky
            ### convert to canonical representation
            ra = ra2hhmmss(ra)
            dec= dec2ddmmss(dec)
            decsign = ('-' if dec[3] < 0 else '+')

            str = '%2i %2i %6.3f ' \
                  '%c%2i %2i %6.3f ' \
                  '%9.4f %9.4f %7.2f ' \
                  '%2i %13.7f %10s\n' % \
                   (ra[0], ra[1], ra[2], 
                    decsign, dec[0], dec[1], dec[2],
                    shape[0]*3600, shape[1]*3600, shape[2],
                    4, A, '')

            f.write(str)

        f.close()

    def write_kvis_ann(self, img, dir):

        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Output    ")
        fname = img.imagename + '.kvis.ann'
        f = open(dir+fname, 'w')
        mylog.info('Writing '+dir+fname)
        f.write("### KVis annotation file\n\n")
	f.write("color green\n\n")

        for g in img.gaussians:#():
            iidx = g.island_id
            A = g.peak_flux
            ra, dec = g.centre_sky
            shape = g.size_sky
            cross = (3*img.wcs_obj.acdelt[0], 3*img.wcs_obj.acdelt[1])

            str = 'text   %10.5f %10.5f   %d\n' % \
                (ra, dec, iidx)
            f.write(str)
            str = 'cross   %10.5f %10.5f   %10.7f %10.7f\n' % \
                (ra, dec, abs(cross[0]), abs(cross[1]))
            #f.write(str)
            str = 'ellipse %10.5f %10.5f   %10.7f %10.7f %10.4f\n' % \
                (ra, dec, shape[0], shape[1], shape[2])
            f.write(str)

        f.close()

  
    def write_gaul_FITS(self, img, dir, incl_wavelet=True):
        """ Write as FITS binary table. """
        cnames, cunit, cformat = stuff.cnames, stuff.cunit, stuff.cformat
        fbdsm_list = pybdsm2fbdsm(img)
        col_list = []
        for ind, col in enumerate(fbdsm_list):
          list1 = pyfits.Column(name=cnames[ind], format=cformat[ind], unit=cunit[ind], array=fbdsm_list[ind])
          col_list.append(list1)
        tbhdu = pyfits.new_table(col_list)
        tbhdu.writeto(dir + img.imagename+'.gaul.FITS', clobber=True)

    def write_opts(self, img, dir):
        """ Write input parameters """
        import inspect
        import types
        import numpy as N

        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Output    ")
        if not os.path.exists(dir): os.mkdir(dir)
        fname = 'parameters_used'
        f = open(dir+fname, 'w')
        mylog.info('Writing '+dir+fname)
        for attr in inspect.getmembers(img.opts):
          if attr[0][0] != '_':
            if isinstance(attr[1], (int, str, bool, float, types.NoneType, tuple, list)):
              f.write('%-40s' % attr[0])
              f.write(repr(attr[1])+'\n')
              
              # Also print the values derived internally. They are all stored
              # in img with the same name (e.g., img.opts.beam --> img.beam)
              if hasattr(img, attr[0]):
                  used = img.__getattribute__(attr[0])
                  if used != attr[1] and isinstance(used, (int, str, bool, float,
                                                           types.NoneType, tuple,
                                                           list)):
                      f.write('%-40s' % '    Value used')
                      f.write(repr(used)+'\n')

        f.close()
              

def ra2hhmmss(deg):
    """Convert RA coordinate (in degrees) to HH MM SS"""

    from math import modf
    if deg < 0:
        raise RuntimeError("Negative RA")
    x, hh = modf(deg/15.)
    x, mm = modf(x*60)
    ss = x*60

    return (int(hh), int(mm), ss)

def dec2ddmmss(deg):
    """Convert DEC coordinate (in degrees) to DD MM SS"""

    from math import modf
    sign = (-1 if deg < 0 else 1)
    x, dd = modf(abs(deg))
    x, ma = modf(x*60)
    sa = x*60

    return (int(dd), int(ma), sa, sign)

def pybdsm2fbdsm(img, incl_wavelet=True):
    import functions as func

    fbdsm = []
    g_list = img.gaussians
    if incl_wavelet and hasattr(img, 'atrous_gaussians'):
        for ag in img.atrous_gaussians:
            g_list += ag
    for g in g_list:
        gidx = g.gaus_num
        iidx = g.island_id+1
        widx = g.wavelet_j
        A = g.peak_flux
        T = g.total_flux
        ra, dec = g.centre_sky
        x, y = g.centre_pix
        shape = g.size_sky
        deconv_shape = g.deconv_size_sky
        eA = g.peak_fluxE
        eT = g.total_fluxE
        era, edec = g.centre_skyE
        ex, ey = g.centre_pixE
        eshape = g.size_skyE
        deconv_eshape = g.deconv_size_skyE
        isl_idx = g.island_id
        isl = img.islands[isl_idx]
        isl_rms = isl.rms
        isl_av = isl.mean
        src_idx = g.source_id
        src = img.source[src_idx]
        src_rms = src.rms_isl
        src_av = isl.mean
        flag = g.flag
        grms = g.rms
        x, y = g.centre_pix
        xsize, ysize, ang = g.size_pix # FWHM
        ellx, elly = func.drawellipse(g)
        blc = [int(min(ellx)), int(min(elly))]
        trc = [int(max(ellx)), int(max(elly))]
    
        specin = 0.0
        especin = 0.0
        if img.opts.spectralindex_do:
            spin1 = g.spin1
            espin1 = g.espin1
            if spin1 == None:
                specin = 0.0
                especin = 0.0
            else:                       
                specin = spin1[1]
                especin = espin1[1]

        list1 = [gidx, iidx, widx, flag, T, eT, A, eA, ra, era, dec, edec, x, ex, y,
                 ey, shape[0], eshape[0], shape[1], eshape[1], shape[2],
                 eshape[2], deconv_shape[0], deconv_eshape[0],
                 deconv_shape[1], deconv_eshape[1], deconv_shape[2],
                 deconv_eshape[2], src_rms, src_av, isl_rms, isl_av,
                 specin, especin, src_idx, blc[0], blc[1], trc[0], trc[1], grms]
        fbdsm.append(list1)
    fbdsm = func.trans_gaul(fbdsm)

    return fbdsm


def write_islands(img):
    import numpy as N

    ### write out island properties for reference since achaar doesnt work.
    filename = img.basedir + '/misc/'
    if not os.path.exists(filename): os.mkdir(filename)
    filename = filename + 'island_file'

    if img.j == 0:
      f = open(filename, 'w')
      f.write('Wavelet# Island_id  bbox origin shape mask_active mask_noisy size_active mean rms max_value ngaul gresid_mean '+\
              'gresid_rms resid_rms resid_mean nsource \n')
    else:
      f = open(filename, 'a')

    for isl in img.islands:
      f.write('%5i %5i %5i %5i %5i %5i %5i %5i %5i %5i %10i %10i %10i %.3e %.3e %.3e %5i %.3e %.3e %5i \n' \
              % (img.j, isl.island_id, isl.bbox[0].start, isl.bbox[0].stop, isl.bbox[1].start, isl.bbox[1].stop, \
              isl.origin[0], isl.origin[1], isl.shape[0], isl.shape[1], N.sum(~isl.mask_active), N.sum(~isl.mask_noisy), \
              isl.size_active, isl.mean, isl.rms, isl.max_value, len(isl.gaul), isl.gresid_mean, isl.gresid_rms, \
              len(isl.source)))

    f.close()


