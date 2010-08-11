
import pyfits
import numpy as N
import scipy
import scipy.signal as S
from time import time
import _fbdsm_sub1 as fb
import compute as comp
import _pytesselate as _pytess
import pytess 
import pylab as pl
import shapelets as sh
from math import sqrt
import functions as func
from scipy.optimize import leastsq
import nat
from socket import gethostname
import sys
import os

scratch = "/data1/anaamika/image/"
fitsdir = "/data1/anaamika/fits/"
plotdir = "/data1/anaamika/plots/"
srldir = "/data1/anaamika/image/"
runcode = "mv"

plot_unresolved = True
plot_edittile = True
plot_psfintile = True
plot_varybeta = True
plot_interpshapcoefs = True
plot_createpsfgrid = True

def get_keyword(f1, extn, keyword, code, scratch=scratch):
    x=fb.get_keyword(f1,extn,keyword,code,scratch)
    if code == 's':
        return x[0]
    else:
        return x[1]

def get_fbdsm_nig(imagename,srldir=srldir):
    " reads nisl, ngau from a fbdsm .gaul file "
    x=fb.sourcelistheaders(imagename, srldir)
    nisl=x[3]
    ngau=x[4]
    return nisl, ngau

def trans_gaul(q):
    " transposes a tuple of .gaul values "
    y=[]
    for i in range(len(q[0])):
        elem=[]
        for j in range(len(q)):
            elem.append(q[j][i])
        y.append(elem)
    return y

def temp_char2str(gaul):
    " joins the code and names fields in .gaul"
    for i in range(len(gaul)):
        gaul[i][-12]="".join(gaul[i][-12])
        gaul[i][-11]="".join(gaul[i][-11])
        gaul[i][-11]=gaul[i][-11].lstrip()
    return gaul

def read_fbdsm_gaul(imagename, ngau, srldir=srldir):
    " Returns gaulid, islid, flag, tot, etot, peak, epeak, ra, era, dec, edec, \n\
  x, ex, y, ey, bmaj, ebmaj, bmin, ebmin, bpa, ebpa, dbmaj, debmaj, dbmin, edbmin, \n\
  dbpa, edbpa, sstd, sav, rstd, rav, chisq, q, code, names, blc1, blc2, trc1, trc2, \n\
  rms, dumr2, dumr3, dumr4, dumr5, dumr6 \n"
    import numpy as N
    x=fb.readgaul(imagename,ngau,ngau,srldir)
    return x

def vec_stats(vec,n1,n,nsig):
    " computes mean, rms, median and nsig-clipped values of a vector"
    (m,cm,r,a,cr,ca)=fb.vec_stats(vec,n1,n,nsig)
    return m,cm,r,cr,a,ca

def bindata(over,num): #ptpbin,nbin,ptplastbin, same as get_bins in fBDSM.
    if num <100: ptpbin=num/5
    if num >100: ptpbin=num/10
    if num > 1000: ptpbin=num/20
    if ptpbin % 2 == 1: ptpbin=ptpbin+1
    if num < 10: ptpbin=num
    nbin=(num-ptpbin)/(ptpbin/over)+1
    ptplastbin=(num-1)-(nbin-1)*ptpbin/over
    nbin=nbin+1
    return ptpbin, nbin, ptplastbin

def bin_and_stats_ny(x,y,over,ptpbin,nbin,ptplastbin,nsig):
    import math
    n1=N.array(range(nbin))+1   # bin number
    n2=N.array([ptpbin]*nbin); n2[nbin-2]=ptplastbin; n2[nbin-1]=ptpbin/over
    n3=N.array([ptpbin]*nbin, dtype=float); n3[nbin-1]=float(over)*(len(x)-ptpbin/2)/(nbin-1)
    xval=N.zeros(nbin); 
    meany=N.zeros(nbin); stdy=N.zeros(nbin); mediany=N.zeros(nbin)
    for i in range(nbin):
        lb=round(1+(n1[i]-1)*n3[i]/over+(1-1))-1 # -1 for python indexing
        ub=round(1+(n1[i]-1)*n3[i]/over+(n2[i]-1))-1 # -1 for python indexing
	x1=x[lb:ub+1]; y1=y[lb:ub+1]

        # do calcmedianclip2vec.f for code=YYN
	nout=100; niter=0
	while nout>0 and niter<6:
	    med1=N.median(y1[:])
	    med2=10.**(N.median(N.log10(x1[:])))
	    medstd=0    				# calcmedianstd.f
	    for j in y1: medstd += (j-med1)*(j-med1) 
	    medstd=math.sqrt(medstd/len(y1))        # 
	    av1=N.mean(y1); std1=comp.std(y1)
	    av2=N.mean(x1); std2=comp.std(x1)
# get_medianclip_vec2
            z=N.transpose([x1, y1])
	    z1=N.transpose([n for n in z if abs(n[1]-med1)<=nsig*medstd])
	    nout=len(x1)-len(z1[0])
	    x1=z1[0]; y1=z1[1]; 
	    niter+=1
	xval[i]=med2; 
	meany[i]=av1; stdy[i]=std1; mediany[i]=med1

    if stdy[nbin-1]/mediany[nbin-1] > stdy[nbin-2]/mediany[nbin-2]:
       stdy[nbin-1]=stdy[nbin-2]/mediany[nbin-2]*mediany[nbin-1]
    return xval, meany, stdy, mediany
    
def LM_fit(n, x, y, err, funct, order=0):
    if funct == func.poly:
       p0=N.array([y[N.argmax(x)]] + [0]*order)
    if funct == func.wenss_fit:
       p0=N.array([y[N.argmax(x)]] + [1.])
    res=lambda p, x, y, err: (y-funct(p, x))/err
    (p, flag)=leastsq(res, p0, args=(x, y, err))
    return p


def fit_bins_func(x,y,over,ptpbin,nbin,ptplastbin,nsig):  # sub_size_ksclip
    import math
    (xval,meany,stdy,medy)=bin_and_stats_ny(x,y,over,ptpbin,nbin,ptplastbin,nsig)
    yfit=stdy/medy
    err=N.array([1.]*nbin)
    err[nbin-2]=err[0]*math.sqrt(1.0*ptpbin/ptplastbin)
    err[nbin-1]=err[0]*math.sqrt(1.0*ptpbin*over/ptplastbin)

    i=0
    while i<nbin-4 and (N.all(N.sort(yfit[i:i+4])[::-1] == yfit[i:i+4]) == False):
       i+=1
    if i==nbin-4: sind=0
    else: sind=i-1
    if sind < 1:
        sind = 0
    if sind > 0.25*nbin:
        sind=int(round(0.25*nbin))-1

    s_c=LM_fit(nbin,xval[sind:],yfit[sind:],err[sind:], func.wenss_fit)

    err[:]=1.
    s_dm=LM_fit(nbin,N.log10(xval),medy,err,func.poly, order=2)
    s_cm=LM_fit(nbin,N.log10(xval),medy,err,func.poly, order=1)

    if ptpbin<75: s_dm=N.append(s_cm[:], [0.])
    return s_c, s_dm

def get_unresolved(g_gauls, beam, nsig, kappa2, plot):
    " gets subset of unresolved sources"
    num=len(g_gauls[15])
    b1=N.asarray(g_gauls[15])/(beam[0]*3600.)
    b2=N.asarray(g_gauls[17])/(beam[1]*3600.)
    s1=N.asarray(g_gauls[5])/N.array(g_gauls[39])
    snr=N.array(s1)
    index=snr.argsort()
    snr=snr[index]
    nmaj=N.array(b1)[index]
    nmin=N.array(b2)[index]

    if plot: pl.figure()
# start loop here
    f_sclip=N.zeros((2,num), dtype=bool)
    for idx, nbeam in enumerate([nmaj, nmin]):
      xarr=N.copy(snr)
      yarr=N.copy(nbeam)
      niter=0; nout=num; noutold=nout*2
      while niter<10 and nout >0.75*num:
        (ptpbin, nbin, ptplastbin)=bindata(over,nout)    # get_bins in fBDSM
        (s_c,s_dm) = fit_bins_func(xarr,yarr,over,ptpbin,nbin,ptplastbin,nsig)  # size_ksclip_wenss in fBDSM
        noutold = len(xarr)
        z = N.transpose([xarr, yarr, s_dm[0]+s_dm[1]*N.log10(xarr)+s_dm[2]*(N.log10(xarr)**2.),  \
            N.sqrt(s_c[0]*s_c[0]+s_c[1]*s_c[1]/(xarr*xarr)) ])
        z1 = N.transpose([n for n in z if abs(n[1]-n[2])/(n[2]*n[3])<kappa2])  # sub_size_wenss_getnum in fBDSM
        nout = len(z1[0]); niter += 1
        xarr = z1[0]; yarr = z1[1];   # end of sub_size_wenss_getnum
        if noutold == nout: break
      print ' Iterations = ',niter,'; Fraction thrown away = ',float(num-nout)/num

# flag in the 'unresolved' sources. returns flag array, True ==> unresolved
      logsnr=N.log10(snr)
      dumr = N.sqrt(s_c[0]*s_c[0]+s_c[1]*s_c[1]/(snr*snr))
      med = s_dm[0]+s_dm[1]*logsnr+s_dm[2]*(logsnr*logsnr)
      f_sclip[idx] = N.abs((nbeam-med)/(med*dumr)) < N.array([kappa2]*num)
      f_s = f_sclip[0]*f_sclip[1]

# now make plots
      if plot:
        bb=[b1, b2]
        pl.subplot(211+idx)
        pl.semilogx(s1, bb[idx], 'og')
        f0=f_sclip[idx][index.argsort()]
        sf=[n for i, n in enumerate(s1) if f0[i]]
        b1f=[n for i, n in enumerate(bb[idx]) if f0[i]]
        pl.semilogx(sf, b1f, 'or')
        pl.semilogx(snr,med,'-')
        pl.semilogx(snr,med+med*dumr*(N.array([kappa2]*num)),'-')
        pl.semilogx(snr,med-med*dumr*(N.array([kappa2]*num)),'-')
        pl.title(' axis ' + str(idx))
	pl.savefig(pname+'unresolved_srcs.png')

    return f_s[index.argsort()]

def av_psf(g_gauls, beam, flag):
    """ calculate how much the SNR-weighted sizes of unresolved sources differs from the 
    synthesized beam. Same as av_psf.f in fBDSM."""
    from math import sqrt
    bmaj = N.asarray(g_gauls[15])
    bmin = N.asarray(g_gauls[17])
    bpa = N.asarray(g_gauls[19])
    wt = N.asarray(g_gauls[5])/N.asarray(g_gauls[39])
    flagwt = wt*flag
    sumwt = N.sum(flagwt)
    w1 = N.sum(flagwt*flagwt)
    wtavbm = N.array([N.sum(bmaj*flagwt), N.sum(bmin*flagwt), N.sum(bpa*flagwt)])/sumwt
    dumrar = N.array([N.sum(bmaj*bmaj*flagwt), N.sum(bmin*bmin*flagwt), N.sum(bpa*bpa*flagwt)])
    dd = sumwt*sumwt-w1
    wtstdbm = N.sqrt((dumrar - wtavbm*wtavbm*sumwt)*sumwt/dd)

    avpa  = N.sum(bpa*flagwt-180.0*flagwt*N.array(bpa >= 90))/sumwt
    stdpa = N.sum(bpa*flagwt+(180.0*180.0-360.0*bpa)*flagwt*N.array(bpa >= 90))
    stdpa = sqrt(abs((stdpa-avpa*avpa*sumwt)*sumwt/dd))
    if stdpa < wtstdbm[2]:
        wtstdbm[2] = stdpa
	wtavbm[2] = avpa

    return (wtavbm - N.array([beam[0]*3600.0, beam[1]*3600.0, beam[2]]))/wtstdbm

def get_voronoi_generators(g_gauls, generators, gencode, snrcut, snrtop, snrbot):
    """This gets the list of all voronoi generators. It is either the centres of the brightest
    sources, or is imported from metadata (in future). generators=calib implies only one source
    per facet, and sources between snrtop and snrmax are primary generators. generators=field 
    implies all sources between snrbot and snrtop are secondary generators. This is the same as
    get_voronoi_generators.f in fBDSM. If calibrators='field' then vorogenS is a list of gen.s else
    is None."""

    from math import sqrt
    num=len(g_gauls[0])
    snr=N.array(N.asarray(g_gauls[5])/N.array(g_gauls[39]))
    index=snr.argsort()
    snr = snr[index]
    x = N.asarray(g_gauls[11])[index]
    y = N.asarray(g_gauls[13])[index]

# choose brightest source even if flagged as resolved since flagging isnt 
# that reliable at high snr.
    cutoff = 0; npts = 0
    if generators == 'calibrators':
        if gencode != 'file': gencode = 'list'
        if gencode == 'list':
            if snrtop == 0.:
                minnum = min(20,num)  # atleast 20 sources
	        maxsnr = 0.1*snr[num] # go downto atleast 0.1*maxsnr
	        minsnr=20.0
	        if minsnr < snrcut: minsnr = snrcut
	        if snr[0] > maxsnr: maxsnr = snr[0]
	        if maxsnr < minsnr: maxsnr = sqrt(minsnr*snr[num])
	        dumi = snr.searchsorted(maxsnr)
	        dumi1 = snr.searchsorted(minsnr)
	        cutoff = min(dumi, num-minnum)
	        if snr[num-minnum] < maxsnr:
                    cutoff = num - minnum
                else:
                    cutoff = dumi
                if snr[cutoff] < minsnr: cutoff = dumi1
    	    else:
                cutoff = int(round(num*(1.0-snrtop)))
	    npts = num - cutoff + 1

    if generators == 'field':
        cutoff = int(round(num*(1.0-snrtop)))
	npts = num - cutoff + 1
	cutoffs = int(round(num*(1.0-snrbot)))
	nptss = cutoff - cutoffs

    if generators == 'calibrators':
        if gencode == 'file':
            raise NotImplementedError, "gencode=file not yet implemented."
# for all generator/gencode combinations
    vorogenP = N.asarray([x[num-1:cutoff-2:-1], y[num-1:cutoff-2:-1], snr[num-1:cutoff-2:-1]])

# for generator=field
    vorogenS = None
    if generators == 'field':
        vorogenS = N.asarray([x[cutoff-2:cutoffs-2:-1], y[cutoff-2:cutoffs-2:-1], snr[cutoff-2:cutoffs-2:-1]])

    return vorogenP, vorogenS

def edit_vorogenlist(vorogenP, frac):
    """ Edit primary voronoi generator list. Each tile has a tile centre and can 
    have more than one generator to be averaged. tile_list is a list of arrays, indexed
    by the tile number and each array is an array of numbers in the ngen list which are
    the generators in that tile. xtile, ytile and snrtile are arrays of length number_of_tiles 
    and have x,y,snr of each tile. The list of tiles is modified later
    using the secondary list in tesselate. For now though, just group together gen.s
    if closer than a fraction of dist to third closest. Same as edit_vorogenlist in fBDSM. """

    xgen, ygen, snrgen = vorogenP
    flag = N.zeros(len(xgen))
    coord=N.array([xgen,ygen]).transpose()
    tile_list = []
    tile_coord = []; tile_snr = []
    for i in range(len(xgen)):
        dist = N.array(map(lambda t: comp.dist_2pt(coord[i], t), coord))
	indi = N.argsort(dist)
	sortdist = dist[indi]
	#if sortdist[1] < frac*sortdist[2]:    # first is the element itself
	if dist[1] < frac*dist[2]:    # first is the element itself
          if flag[indi[1]] + flag[i] == 0:   #  not already deleted from other pair
            tile_list.append([i, indi[1]])
	    tile_coord.append((coord[i]*snrgen[i]+coord[indi[1]]*snrgen[indi[1]])/(snrgen[i]+snrgen[indi[1]]))
	    tile_snr.append(snrgen[i]+snrgen[indi[1]])
	    flag[i] = 1
	    flag[indi[1]] = 1
        else:
          #if sortdist[1]+sortdist[2] < frac*sortdist[3]: # for 3 close-by sources
          if dist[1]+dist[2] < frac*dist[3]: # for 3 close-by sources
            in1=indi[1]
	    in2=indi[2]
            if flag[in1]+flag[in2]+flag[i] == 0: # not already deleted from others
              tile_list.append([i, in1, in2])
	      tile_coord.append((coord[i]*snrgen[i]+coord[in1]*snrgen[in1]+coord[in2]*snrgen[in2]) \
			      /(snrgen[i]+snrgen[in1]+snrgen[in2]))
	      tile_snr.append(snrgen[i]+snrgen[in1]+snrgen[in2])
	      flag[i] = 1
	      flag[in1] = 1
	      flag[in2] = 1
          else:
            tile_list.append([i])
	    tile_coord.append(coord[i])
	    tile_snr.append(snrgen[i])

    return tile_list, tile_coord, tile_snr
	    
def roundness(snrgen):
    return snrgen

def tess_simple(vorogenP, wts, tess_sc, tess_fuzzy, shape):
    """ Simple tesselation """

    xgen, ygen, snrgen = vorogenP
    volrank = _pytess.pytess_simple(shape[0], shape[1], xgen, ygen, snrgen, \
              wts, tess_fuzzy, tess_sc)

    return volrank

def tess_roundness(vorogenP, wts, tess_sc, tess_fuzzy, shape):
    """ Tesselation, modified to make the tiles more round. """

    xgen, ygen, snrgen = vorogenP
    volrank = _pytess.pytess_roundness(shape[0], shape[1], xgen, ygen, snrgen, \
              wts, tess_fuzzy, tess_sc)

    return volrank

def tesselate(vorogenP, vorogenS, tile_prop, tess_method, tess_sc, tess_fuzzy, generators, gencode, shape):
    """ Various ways of tesselating. If generators='calibrator', no need to tesselate, just get 
    modified list based on very nearby sources. If generators='field' then tesselate. The image 
    is tesselated based on tile_prop. """

    wtfn={'unity' : lambda x : N.ones(len(x)), \
          'log10' : N.log10, \
	  'sqrtlog10' : lambda x : N.sqrt(N.log10(x)), \
	  'roundness' : N.array}

    #xgen, ygen, snrgen = vorogenP
    tile_list, tile_coord, tile_snr = tile_prop
    xt = trans_gaul(tile_coord)[0]
    yt = trans_gaul(tile_coord)[1]
    vorogenT = xt, yt, tile_snr

    wt_fn = wtfn[tess_method]
    #wts = wt_fn(snrgen)
    wts = wt_fn(tile_snr)

    if tess_method == 'roundness':
        volrank = tess_roundness(vorogenT, wts, tess_sc, tess_fuzzy, shape)
        #volrank = tess_roundness(vorogenP, wts, tess_sc, tess_fuzzy, shape)
    else:
        volrank = tess_simple(vorogenT, wts, tess_sc, tess_fuzzy, shape)
        #volrank = tess_simple(vorogenP, wts, tess_sc, tess_fuzzy, shape)

    return volrank, wts

def edit_tile(ltnum, g_gauls, flag_unresolved, snrcutstack, volrank, tile_prop, tess_sc, \
   	      tess_fuzzy, wts, plot):
    """ Looks at tiles with no (or one) unresolved source inside it and deletes it and recomputes
       the tiling. For now, does not recompute since we wont use the rank for those pixels anyway."""

    if ltnum > 1: raise NotImplementedError, "NOT YET IMPLEMENTED FOR LTNUM>1"

    tile_list, tile_coord, tile_snr = tile_prop
    tr_gaul = trans_gaul(g_gauls)
    tr=[n for i, n in enumerate(tr_gaul) if flag_unresolved[i] and n[5]/n[39] >= snrcutstack]
    ntile = len(tile_list)
    ngenpertile=N.zeros(ntile)
    for itile in range(ntile):
        tile_gauls = [n for n in tr if volrank[int(round(n[11])),int(round(n[13]))]-1 \
			== itile]
        ngenpertile[itile]=len(tile_gauls)

    print ngenpertile
    new_n = N.sum(ngenpertile >= ltnum)
    print 'new n = ',new_n

# prepare lost of good tiles to pass to pixintile
    goodtiles = N.array(N.where(ngenpertile >= ltnum)[0])
    new_n = len(goodtiles)
    tile_coord_n = [n for i,n in enumerate(tile_coord) if i in goodtiles]
    wts_n = [n for i,n in enumerate(wts) if i in goodtiles]

    r2t = N.zeros(ntile, dtype=int)
    print r2t
    entry = -1
    for itile in range(ntile):
      if ngenpertile[itile] >= ltnum:
          r2t[itile] = itile
      else:
          pixel = tile_coord[itile]
          tilenum = pytess.pixintile(trans_gaul(tile_coord_n), pixel, tess_method, wts_n, tess_sc, tess_fuzzy)
	  r2t[itile] = tilenum
    print '1st pass ',r2t
    for itile in range(ntile):
        num = N.sum(r2t == itile)
	if num == 0:
          arr = N.where(r2t > itile)[0]
	  for i in arr:
              r2t[i]=r2t[i]-1
    print '2nd pass ',r2t
    print 'number matches = ',r2t.max() == new_n-1

      


    sys.exit(0)
    #goodtiles=N.array(N.where(ngenpertile >= ltnum)[0])
    #ntile_list=[n for i,n in enumerate(tile_list) if i in goodtiles]
    #ncoord_tile=[n for i,n in enumerate(coord_tile) if i in goodtiles]

    badtiles=N.array(N.where(ngenpertile < ltnum)[0])   #### assume ltnum = 1 for now
    for ibad in badtiles:
      pixel = tile_coord[ibad]
      tilenum = pytess.pixintile(trans_gaul(ncoord_tile), pixel, tess_method, nwts, tess_sc, tess_fuzzy)
      print 'pix tilenum ', pixel, tilenum
      print ntile_list[tilenum], ncoord_tile[tilenum]
      ntile_list[tilenum] += [ibad]
      snr_o = ntile_snr[tilenum]
      ncoord_tile[tilenum] = (ncoord_tile[tilenum]*snr_o+pixel*snr_tile[ibad])/(snr_o+snr_tile[ibad])
      nsnr_tile[tilenum] = snr_o + snr_tile[ibad]
      volrank_tilenum[volrank[int(round(pixel[0])), int(round(pixel[1]))]] = tilenum
      print '====='
      print ntile_list
      print ncoord_tile
      print '====='
      print volrank[int(round(pixel[0])), int(round(pixel[1]))] 
      print volrank[int(round(coord_tile[tilenum][0])), int(round(coord_tile[tilenum][1]))] 
      print volrank_tilenum

    tile_prop = ntile_list, ncoord_tile, ntile_snr 
    ntile=len(ntile_list)
    ngenpertile=N.zeros(ntile)
    for itile in range(ntile):
        tile_gauls = [n for n in tr if volrank[int(round(n[11])),int(round(n[13]))] \
			== volrank_tilenum[itile]]
        ngenpertile[volrank_tilenum[itile]]=len(tile_gauls)
    print ' NNEEWW = ',ngenpertile
           

    return ngenpertile, tile_prop

def stackpsf(image, beam, g_gauls, wts, cdelt, factor):
    """ Stacks all the images of sources in the gaussian list gauls from image, out to
    a factor times the beam size. Currently the mask is for the whole image but need to 
    modify it for masks for each gaussian. These gaussians are supposed to be relatively 
    isolated unresolved sources. Cut out an image a big bigger than facXbeam and imageshift
    to nearest half pixel and then add."""

    print " Does not handle masks etc well at all. Masks for image for blanks, masks for \
    islands, etc."

    gxcens_pix = g_gauls[11] 
    gycens_pix = g_gauls[13]
    peak = g_gauls[5]

    psfimsize = int(round(max(beam[0], beam[1])/max(cdelt[0], cdelt[1]) * factor))    # fac X fwhm; fac ~ 2
    psfimage = N.zeros((psfimsize, psfimsize))
    cs2=cutoutsize2 = int(round(psfimsize*(1. + 2./factor)/2.))  # size/2. factor => to avoid edge effects etc
    cc = cutoutcen_ind=[cs2, cs2]
    cpsf=cen_psf_ind = N.array([int(round(psfimsize))/2]*2)
    wt=0.
    
#    pl.figure(None)
    num=len(gxcens_pix)
    for isrc in range(num):   #  MASK !!!!!!!!!!!
        wt += wts[isrc]
        gcp=N.array([gxcens_pix[isrc], gycens_pix[isrc]])
	gcen_ind=gcp-1
        rc=rcen_ind = N.asarray(N.round(gcen_ind), dtype=int)
	shift=cc-(gcen_ind-(rc-cs2))
	cutimage = image[rc[0]-cs2:rc[0]+cs2,rc[1]-cs2:rc[1]+cs2]
	if len(cutimage.shape) == 3: cutimage=cutimage[:,:,0]
	if sum(sum(N.isnan(cutimage))) == 0:
	  im_shift = comp.imageshift(cutimage, shift)
	  im_shift = im_shift/peak[isrc]*wts[isrc]
	  psfimage += im_shift[cc[0]-cpsf[0]:cc[0]-cpsf[0]+psfimsize,cc[1]-cpsf[1]:cc[1]-cpsf[1]+psfimsize]
#	  pl.figure(None)
#	  pl.subplot(num/10+1,10,isrc+1)
#	  pl.imshow(im_shift[cc[0]-cpsf[0]:cc[0]-cpsf[0]+psfimsize,cc[1]-cpsf[1]:cc[1]-cpsf[1]+psfimsize]\
#			,interpolation='nearest')
#	  pl.figure(None)
#	  pl.subplot(num/10+1,10,isrc+1)
#	  pl.contour(im_shift[cc[0]-cpsf[0]:cc[0]-cpsf[0]+psfimsize,cc[1]-cpsf[1]:cc[1]-cpsf[1]+psfimsize]\
#			,15)
    psfimage = psfimage/wt

    return psfimage

def psf_in_tile(image, beam, g_gauls, flag_unresolved, cdelt, factor, snrcutstack, volrank, \
		tile_prop, plot):
    """ For each tile given by tile_prop, make a list of all gaussians in the constituent tesselations
    and pass it to stackpsf with a weight for each gaussian, to calculate the average psf per tile. """

    print " Should define weights inside a tile to include closure errors "

    tile_list, tile_coord, tile_snr = tile_prop
    tr_gaul = trans_gaul(g_gauls)
    tr=[n for i, n in enumerate(tr_gaul) if flag_unresolved[i] and n[5]/n[39] >= snrcutstack]
    ntile = len(tile_list)
    psfimages = []
    psfcoords = []
    srcpertile = N.zeros(ntile)
    snrpertile = N.zeros(ntile)

    if plot: 
      pl.figure(None)
      xt, yt = N.transpose(tile_coord)
      colours=['b','g','r','c','m','y','k']*(len(xt)/7+1)
      pl.axis([0.0, image.shape[0], 0.0, image.shape[1]])
      pl.title('Tesselated image with tile centres and unresolved sources')
      pl.imshow(N.transpose(volrank), origin='lower')
      pl.colorbar()
      for i in range(ntile):
        pl.plot([xt[i]], [yt[i]], 'D'+colours[i])
	pl.text(xt[i], yt[i], str(i))

    for itile in range(ntile):
        tile_gauls = [n for n in tr if volrank[int(round(n[11])),int(round(n[13]))]-1 \
			== itile]
	t_gauls = trans_gaul(tile_gauls)
	srcpertile[itile] = len(t_gauls[0])
	if plot: 
  	  pl.plot(t_gauls[11], t_gauls[13], 'x'+'k', mew=1.3)#colours[itile])
	  for i, ig in enumerate(t_gauls[11]):
	   xx=[xt[itile], ig]
	   yy=[yt[itile], t_gauls[13][i]]
	   pl.plot(xx,yy,'-'+colours[itile])
        wts = N.asarray(t_gauls[5])/N.asarray(t_gauls[39])             # wt is SNR
	snrpertile[itile] = sum(wts)
	a = stackpsf(image, beam, t_gauls, wts, cdelt, factor)
        psfimages.append(a)
        psfcoords.append([sum(N.asarray(t_gauls[11])*wts)/sum(wts), sum(N.asarray(t_gauls[13])*wts)/sum(wts)])
    totpsfimage = psfimages[0]*snrpertile[0]
    for itile in range(1,ntile):
        totpsfimage += psfimages[itile]*snrpertile[itile]
    totpsfimage = totpsfimage/sum(snrpertile)

    if plot: pl.savefig(pname + "tesselatedimage.png")

    if plot:
     pl.figure(None)
     pl.clf()
     ax = pl.subplot(1,1,1)
     pax = ax.get_position()
     start = N.array((pax.xmin, pax.ymin))
     stop = N.array((pax.xmax, pax.ymax))
     plaxis = pl.axis([0, image.shape[0], 0, image.shape[1]])
     pl.title('Stacked psf for each tile')
     for itile in range(ntile):
	im=psfimages[itile]
	sz=0.07
	spt = int(round(snrpertile[itile]*10))/10.
	titl='n='+str(int(round(srcpertile[itile])))+'; SNR='+str(spt)
   	posn=[psfcoords[itile][0], psfcoords[itile][1]]
	normposn=N.array(stop-start, dtype=float)/N.array(image.shape[0:2])*posn+start
	a=pl.axes([normposn[0]-sz/2., normposn[1]-sz/2., sz, sz])
	pl.contour(im,15)
	pl.title(titl, fontsize='small')
	pl.setp(a, xticks=[], yticks=[])
     pl.savefig(pname + "stackedpsf_per_tile.png")

    return psfimages, psfcoords, totpsfimage


def interp_shapcoefs(nmax, tr_psf_cf, psfcoords, imshape, compress, plot):
    """ Interpolate in scipy for irregular grids has bugs. Hence fits a second order poly
    to the coefficients. See if it is significant. Do errors for coeff.s properly."""

# interpolating in scipy for irregulr grids has problems.
# fitting a polynomial is also not very nice.
# using natgrid now.

    x, y = N.transpose(psfcoords)
    index = [(i,j) for i in range(nmax+1) for j in range(nmax+1-i)] 
    xi=x
    yi=y
    xo=N.arange(0.0,round(imshape[0]), round(compress))
    yo=N.arange(0.0,round(imshape[1]), round(compress))
    rgrid=nat.Natgrid(xi,yi,xo,yo)
    p={}
    for coord in index:
        z = N.array(tr_psf_cf[coord])    # else natgrid cant deal with noncontiguous memory
        p[coord] = rgrid.rgrd(z)

    if plot:
      pl.figure(None)
      pl.clf()
      pl.title('Interpolated shapelet coefficients')
      for i,coord in enumerate(index):
        pl.subplot(6,5,i+1)
        pl.title(str(coord))
	pl.plot(xi/compress, yi/compress, 'xk')
	pl.imshow(p[coord], interpolation='nearest')
	pl.colorbar()
      pl.savefig(pname + "shapeletcoeff_interp.png")

    return p, xo, yo

def create_psf_grid(psf_coeff_interp, imshape, xgrid, ygrid, skip, nmax, psfshape, basis, beta, 
		cen, totpsfimage, plot):
    """ Creates a image with the gridded interpolated psfs. xgrid and ygrid are 1d numpy arrays
    with the x and y coordinates of the grids. """

    if plot: 
      plnum=N.zeros(2)
      for i in range(2):
        dum=pl.figure(None)
	plnum[i]=dum.number
        pl.clf()
	if i == 0: pl.title('Gridded psfs')
	if i == 1: pl.title('Gridded residual psfs')
        ax = pl.subplot(1,1,1)
        plaxis = pl.axis([0, imshape[0], 0, imshape[1]])
        pax = ax.get_position()
        start = N.array((pax.xmin, pax.ymin))
        stop = N.array((pax.xmax, pax.ymax))
        sz=0.07
    mask=N.zeros(psfshape, dtype=bool)   # right now doesnt matter
    xg=xgrid[::skip+1]
    yg=ygrid[::skip+1]
    index = [(i,j) for i in range(0,len(xgrid),skip+1) for j in range(0,len(ygrid),skip+1)]
    xy = [(i,j) for i in xgrid[::skip+1] for j in ygrid[::skip+1]]
    blah=[]
    for i, coord in enumerate(index):
        cf = N.zeros(psfshape)
	for k in psf_coeff_interp:
            cf[k]=psf_coeff_interp[k][coord]
        psfgridim = sh.reconstruct_shapelets(psfshape, mask, basis, beta, cen, nmax, cf)
	cf[0,0]=0.0
        psfgridim1 = sh.reconstruct_shapelets(psfshape, mask, basis, beta, cen, nmax, cf)
	blah.append(psfgridim)
  
	if plot:
          for j in range(2):
            pl.figure(plnum[j])
            posn = [xy[i][0], xy[i][1]]
	    normposn =N.array(stop-start, dtype=float)/N.array(imshape[0:2])*posn+start
	    a=pl.axes([normposn[0]-sz/2., normposn[1]-sz/2., sz, sz])
	    if j == 0: pl.contour(psfgridim,15)
	    #if j == 1: pl.contour(psfgridim-totpsfimage,15)
	    if j == 1: pl.contour(psfgridim1,15)
	    pl.setp(a, xticks=[], yticks=[])
    if plot: 
      pl.figure(plnum[0])
      pl.savefig(pname + "gridded_image.png")
      pl.figure(plnum[1])
      pl.savefig(pname + "res_gridded_image.png")


    return blah
    




#########################
### MAIN PROGRAM HERE ###
#########################

#fitsfilename=raw_input("  Fits filename  : ")
fitsfilename='A2256.NEWEST.SPAM1B.FITS'
fitsfilename='BOOTES_GMRT_150_PBCOR_COR.FITS'
fitsfilename='WN65341H.fits'
fitsfilename='mi_spam.fits'
fitsfilename='sim_const.1.FITS'
if fitsfilename[-5:] in ['.fits', '.FITS']:
    filename=fitsfilename[0:-5]
else:
    filename=fitsfilename

fitsfile = pyfits.open(fitsdir+fitsfilename)
hdr=fitsfile[0].header
image = fitsfile[0].data
image=image[0]
image=N.array(image.transpose(), order='C',dtype=image.dtype.newbyteorder('='))
fitsfile.close()
#solnname=raw_input("  Solnname : ")
solnname=gethostname()

pdir=plotdir+"/"+ filename + "." + solnname + ".psf"
isdir=os.path.isdir(pdir)
if isdir:
  print " Directory " + pdir + "exists. Will overwrite plots if True."
else:
  cmd="mkdir " + pdir
  os.system(cmd)
pname=pdir+"/"+filename+"."+solnname+"."

imagename=filename+'.'+solnname
(nisl,ngau)=get_fbdsm_nig(imagename)
gauls=read_fbdsm_gaul(imagename,ngau)
tr_gauls=trans_gaul(gauls)
temp_char2str(tr_gauls)    # tr_gauls is transpose of readgaul o/p with char corrected.

# read in psfparadefine file
psfparadefine=fb.read_psfparadefine()
[generators, nsig, over, kappa2, snrcut, snrtop, snrbot, snrcutstack, dum1, dum2, \
      gencode, primarygen, itess_method, tess_sc, tess_fuzzy]=psfparadefine
if snrcut < 10: 
    print 'snrcut=',snrcut,' too low; increasing to 10'
    snrcut=10
generators=generators.strip()
gencode=gencode.strip()
primarygen=primarygen.strip()
# tess_method - do it properly later
wtfns=['unity', 'roundness', 'log10', 'sqrtlog10']
tess_method=wtfns[itess_method]

# takes gaussians with flag=0, code=S and snr > snrcut.
tr=[n for n in tr_gauls if n[2]==0 and n[5]/n[39]>snrcut and n[33].strip()=='S' ]
g_gauls = trans_gaul(tr)

# computes statistics of fitted sizes. Same as psfvary_fullstat.f in fBDSM.
bmaj = g_gauls[15]
bmin = g_gauls[17]
bpa = g_gauls[19]
(bmaj_m,bmaj_cm,bmaj_r,bmaj_cr,bmaj_a,bmaj_ca) = vec_stats(bmaj, 1, len(bmaj), nsig)
(bmin_m,bmin_cm,bmin_r,bmin_cr,bmin_a,bmin_ca) = vec_stats(bmin, 1, len(bmin), nsig)
(bpa_m,bpa_cm,bpa_r,bpa_cr,bpa_a,bpa_ca) = vec_stats(bpa, 1, len(bpa), nsig)

# get subset of sources deemed to be unresolved. Same as size_ksclip_wenss.f in fBDSM.
# change all this to get from image class when inside pybdsm.
beam=[None]*3
beam[0] = get_keyword(imagename,'.header','BMAJ','r',scratch)
beam[1] = get_keyword(imagename,'.header','BMIN','r',scratch)
beam[2] = get_keyword(imagename,'.header','BPA','r',scratch)
flag_unresolved = get_unresolved(g_gauls, beam, nsig, kappa2, plot_unresolved)

# see how far the SNR-weighted sizes of unresolved sources differ from the synthesized beam.
wtsize_beam_snr = av_psf(g_gauls, beam, flag_unresolved)

# get a list of voronoi generators. vorogenS has values (and not None) if generators='field'.
vorogenP, vorogenS = get_voronoi_generators(g_gauls, generators, gencode, snrcut, snrtop, snrbot)
#dumx=vorogenP[0]; dumx=N.insert(dumx,4,800.0); 
#dumy=vorogenP[1]; dumy=N.insert(dumy,4,4700.0); 
#dumz=vorogenP[2]; dumz=N.insert(dumz,4,200.0); 
#vorogenP = N.asarray([dumx, dumy, dumz])

# group generators into tiles
tile_prop = edit_vorogenlist(vorogenP, frac=0.25)

# tesselate the image 
#volrank, volrank_tilenum, wts = tesselate(vorogenP, vorogenS, tile_prop, tess_method, tess_sc, tess_fuzzy, \
volrank, vorowts = tesselate(vorogenP, vorogenS, tile_prop, tess_method, tess_sc, tess_fuzzy, \
	   	    generators, gencode, image.shape)

tile_list, tile_coord, tile_snr = tile_prop
ntile = len(tile_list)
ngenpertile=N.zeros(ntile)
tr_gaul = trans_gaul(g_gauls)
tr=[n for i, n in enumerate(tr_gaul) if flag_unresolved[i] and n[5]/n[39] >= snrcutstack]
for itile in range(ntile):
    tile_gauls = [n for n in tr if volrank[int(round(n[11])),int(round(n[13]))]-1 \
		== itile]
    ngenpertile[itile]=len(tile_gauls)
print 'Number of generators per tile : ',ngenpertile

# filter for (almost) empty tiles with unresolved sources < num, and change volrank accordingly
ltnum=1
#ngenpertile, badtiles = edit_tile(ltnum, g_gauls, flag_unresolved, snrcutstack, volrank, \
#		        tile_prop, tess_sc, tess_fuzzy, vorowts, plot_edittile)

pyfits.writeto(fitsdir+'volrank.fits',N.array(volrank.transpose(), dtype=image.dtype.newbyteorder('=')), hdr, \
               clobber=True)
print ' Written out '+fitsdir+'volrank.fits'

# For each tile, calculate the weighted averaged psf image. Also for all the sources in the image.
cdelt=[None]*2
cdelt[0] = get_keyword(imagename,'.header','CDELT1','r',scratch)
cdelt[1] = get_keyword(imagename,'.header','CDELT2','r',scratch)
factor=3.
psfimages, psfcoords, totpsfimage = psf_in_tile(image, beam, g_gauls, flag_unresolved, \
           cdelt, factor, snrcutstack, volrank, tile_prop, plot_psfintile)

#sys.exit(0)

# use totpsfimage to get beta, centre and nmax for shapelet decomposition. Use nmax=5 or 6
mask=N.zeros(totpsfimage.shape, dtype=bool)
(m1, m2, m3)=comp.moment(totpsfimage, mask)
betainit=sqrt(m3[0]*m3[1])*2.0  * 1.4
tshape = totpsfimage.shape
cen = N.array(N.unravel_index(N.argmax(totpsfimage), tshape))+[1,1]
cen = tuple(cen)
nmax = 6
basis = 'cartesian'
betarange = [0.5,sqrt(betainit*max(tshape))]
print " DONT take varybeta but actual sigma of gaussian"
beta, error  = sh.shape_varybeta(totpsfimage, mask, basis, betainit, cen, nmax, betarange, plot_varybeta)
beta = 3.134
if error == 1: print '  Unable to find minimum in beta'

# decompose all the psf images using the beta from above
nmax=6; psf_cf=[]
npsf = len(psfimages)
for i in range(npsf):
    psfim = psfimages[i]
    cf = sh.decompose_shapelets(psfim, mask, basis, beta, cen, nmax, mode='')
    psf_cf.append(cf)

# transpose the psf image list
tile_list, tile_coord, tile_snr = tile_prop
xt, yt = N.transpose(tile_coord)
tr_psf_cf = N.transpose(N.array(psf_cf))

# interpolate the coefficients across the image. Ok, interpolate in scipy for 
# irregular grids is crap. doesnt even pass through some of the points.
# for now, fit polynomial.
compress = 100.0
psf_coeff_interp, xgrid, ygrid =interp_shapcoefs(nmax, tr_psf_cf, psfcoords, image.shape, \
		compress, plot_interpshapcoefs)

psfshape = psfimages[0].shape
skip = 5
aa=create_psf_grid(psf_coeff_interp, image.shape, xgrid, ygrid, skip, nmax, psfshape, \
		basis, beta, cen, totpsfimage, plot_createpsfgrid)




