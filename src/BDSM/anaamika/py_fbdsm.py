""" 
To run fbdsm from python so that plot_associatelist.f can 
be run automatedly for lofar images.

Needs anaamika to compile, f2py, libs.py. _fbdsm.so can be 
compiled from inside anaamika/bdsm-dir using gfortran -fPIC.
Also needs pgplot, cfitsio and wcslib. wcslib should be compiled 
with gfortran -fPIC as well : use latest version; earlier ones 
had a bug for this compiler on 64 bit machines).

                   R. Niruj (sept 2009)
"""

def print_bdsm():
    print '\n\nThis is a python wrapper around much of Anaamika. This first runs '
    print 'the fortran version of Blob Detection and Source Measurement (BDSM).'
    print 'It then associates the extracted gaussians with the WENSS catalogue '
    print 'and generates plots of associated quantities.\n\n'
    print 'All fBDSM parameters taken from the file ./paradefine.\n'

def print_asrl(parms, catalogue):
    imagename = parms['imagename']
    srldir  = parms['srldir']
    exist1 = os.path.exists(srldir+imagename+'.gaul')
    if not exist1: 
        print 'The file '+srldir+imagename+'.gaul doesnt exist! '
    exist2 = os.path.exists(srldir+catalogue)
    if not exist2: 
        print 'The file '+srldir+catalogue+' doesnt exist! '
    if (not exist1) or (not exist2):
        print ' !!! Stopped execution !!!'
        import sys
        sys.exit()
    print 'Will associate the fBDSM gaussians in \n'+srldir+imagename+\
          '.gaul with \n'+srldir+catalogue
    print '\n** If you need another catalogue file instead, to associate \nwith, \
please ask me for a file in the correct format. -- Niruj **'

def pyread_paradefine():

    parms = {}
    os.system("awk '{print $1, $3}' paradefine > op")
    d = libs.readinfile('op')
    for i, val in enumerate(d[1]):
        try:
            d[1][i] = float(val)
        except:
            d[1][i] = val[1:-1]
        parms[d[0][i]] = d[1][i]
    imagename = parms['fitsname']
    if imagename[-5:] in ['.FITS', '.fits']:
        imagename = imagename[:-5]
    imagename = imagename + '.' + parms['solnname']
    parms['imagename'] = imagename
                                        # now add extra from header
    fitsname =  parms['fitsname']
    fitsdir = parms['fitsdir']
    str1 = " | sed 's/=/ = /' | sed \"s/'//g\" | awk '{if ($3!=\"\") print $1, $3}' "
    str1 = str1 + " | grep -v END > op"
    ret = os.system("fitshead "+fitsdir+fitsname+str1)
    d = libs.readinfile('op')
    for i, val in enumerate(d[1]):
        try:
            d[1][i] = float(val)
        except:
            d[1][i] = val
        parms[d[0][i]] = d[1][i]
 
    return parms
            
def parms_to_astrometry(parms):
    
    a, b, c = parms['CTYPE1'], parms['CTYPE2'], parms['CTYPE3']
    ctype = N.array([a, b, c])
    #for i in range(3):
    #  if len(ctype[i]) != 8: ctype[i] = ctype[i]+' '*(8-len(ctype[i]))
    a, b, c = parms['CRVAL1'], parms['CRVAL2'], parms['CRVAL2']
    crval = N.array([a, b, c])
    a, b, c = parms['CDELT1'], parms['CDELT2'], parms['CDELT3']
    cdelt = N.array([a, b, c])
    try: 
      a, b, c = parms['CROTA1'], parms['CROTA2'], parms['CROTA3']
      crota = N.array([a, b, c])
    except: 
      crota = N.zeros(3) 
    a, b, c = parms['CRPIX1'], parms['CRPIX2'], parms['CRPIX3']
    crpix = N.array([a, b, c])
    return ctype, crpix, cdelt, crval, crota 

def setup_var_pasl(var, parms, refcoords=None):
    """ var has everything defined already except for those relating to
    a pixel in the image. parms is dict of paradefine and refcoords is 
    (ra,dec) or centre of image. """
        
    if refcoords == None:    # then take centre of image
      n = parms['NAXIS1'] 
      m = parms['NAXIS2'] 
      cen1, cen2 = n/2, m/2
      ctype, crpix, cdelt, crval, crota = parms_to_astrometry(parms)
      ra1, dec1 = PYASSO.call_wcs_p2c(cen1, cen2, crpix, cdelt, crval, crota, \
                  ctype[0], ctype[1], ctype[2])
    else:
      ra1, dec1 = refcoords
      cen1, cen2 = PYASSO.call_wcs_c2p(ra1, dec1, crpix, cdelt, crval, crota, \
                  ctype[0], ctype[1], ctype[2])
    headfile = parms['imagename']

    for i in range(var.shape[1]):
        var[32,i] = PYASSO.justdist(ra1, var[7,i], dec1, var[8,i])
    PYASSO.sub_pasl_polar(var, headfile, ra1, dec1, parms['scratch'])
    
    PYASSO.sub_pasl_posndiff_magang(var, headfile, parms['scratch'])

    PYASSO.sub_pasl_radazi(var, headfile, parms['scratch'], cen1, cen2)

     

    

#####################################################################

import _fbdsm as FBDSM
import libs
import _pyasso as PYASSO
import os
import numpy as N
import setup_py_asrl as SETUP_PASL
import py_plot_setup as ID

print_bdsm()
                       # Now run fBDSM #
#FBDSM.fbdsm()

# read in the paradefine file for use here, as a dictionary. can change 
#values in the dictionary as well, and also in the paradefine file.
parms = pyread_paradefine()

# Now to associate with wenss (J2000)
catalogue = 'wenss_sc.c2g.gaul'
print_asrl(parms, catalogue)
imagename = parms['imagename']
scratch = parms['scratch']
srldir = parms['srldir']
master = catalogue[:-5]
sec = imagename
rcode = 'q';
res=os.popen('wc -l '+srldir+master+'.gaul').read().split()
nsrcm = int(float(res[0]))
res=os.popen('wc -l '+srldir+sec+'.gaul').read().split()
nsrcs = int(float(res[0]))
PYASSO.sub_associatesrl(master,sec,nsrcm,nsrcs,scratch,srldir,rcode,tol=120.0)

# first read pasl file and then set up array var 

scrat = master+'__'+sec
res=os.popen('wc -l '+scratch+scrat+'.pasf').read().split()
npairs = (int(float(res[0]))-1)/2
var = PYASSO.readin_pasl(scratch+scrat+'.pasf', scrat, scratch, npairs, 100)

ref_coords = None  # in radeg and decdeg as a tuple else assumed to be image centre
setup_var_pasl(var, parms, ref_coords)

chr1 = 'd'
rcode = 'aq'
clip = 'y'
plotdir = parms['plotdir']

id = ID.id
os.system('mkdir -p '+plotdir+scrat)
print 
for val in id:
    xl, yl, xplot, yplot = PYASSO.setup_pasl(var, val)
    label1 = SETUP_PASL.label[val[0]-1]
    label2 = SETUP_PASL.label[val[1]-1]
    fname = plotdir+scrat+'/'+SETUP_PASL.title[val[0]-1]+'_'+SETUP_PASL.title[val[1]-1]+'.png/PNG'
    PYASSO.plot_menu(xplot, yplot, label1, label2, chr1, xl, yl, fname, rcode, scrat, clip)
    print 'Wrote file ' + fname[:-4]
print 
   

