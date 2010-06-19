DIR=/home/mohan/anaamika/
CFLAGS = -I$(DIR)/includes

ifeq ($(shell uname -m), x86_64)
 PGLIB = -lpng -lX11 -lpgplot
 CFITS = -L/data/lib64/cfitsio/lib/ -lcfitsio
 #WCSINC = -I/data/lib64/wcslib/wcslibg77/include/wcslib
 #WCSLIB = -L/data/lib64/wcslib/wcslibg77/lib -lwcs -lfftw3 
 WCSINC = -I/data/lib64/wcslib/wcslibgfort_pic/include/wcslib
 WCSLIB = -L/data/lib64/wcslib/wcslibgfort_pic/lib -lwcs -lfftw3 
 #FC=g77  -fbounds-check -fno-second-underscore $(CFLAGS)
 #FC1=g77 -fbounds-check -fno-second-underscore -Wno-globals -fno-globals $(CFLAGS)
 FC=gfortran -fPIC -fno-second-underscore $(CFLAGS)     # fbounds-check gives s_rnge not found error for py
 FC1=gfortran -fPIC -fno-second-underscore $(CFLAGS)  # for python


else   # ! i686
 PGLIB =  -L/usr/lib -L/usr/X11R6/lib -lpng -lX11 -L/usr/lib/pgplot -lpgplot
 CFITS =  -L/disks/strw9/niruj/strw1/fitsio_dir/lib/ -lcfitsio
 WCSLIB = -L/disks/strw9/niruj/strw1/wcslib/lib -lwcs -lfftw3
 WCSINC = -I/disks/strw9/niruj/strw1/wcslib/include/wcslib
 FC=g77 -g -fno-second-underscore -fbounds-check $(CFLAGS)
 FC1=g77 -g -fno-second-underscore -fbounds-check -Wno-globals -fno-globals $(CFLAGS)
endif

#ifeq ($(shell uname -s), Darwin)
# CFITS = -L/sw-me/cfitsio/lib/ -lcfitsio
# WCSLIB = -L/sw-me/wcslib/lib -lwcs 
# WCSINC = -I/sw-me/wcslib/include/wcslib
# PGLIB = -L/usr/X11R6/lib -lX11 -Wl,-framework -Wl,Foundation -L/sw/lib -lpng -lz -laquaterm -L/sw/lib/pgplot -lpgplot
# FC=gfortran -fbounds-check $(CFLAGS)
# FC1=gfortran -fbounds-check $(CFLAGS)
#endif

ifeq ($(shell uname -s), Darwin)
 CFITS = -L/sw-SAVE/lib/ -lcfitsio
 WCSLIB = -L/sw-SAVE/lib -lwcs    
 WCSINC = -I/sw-SAVE/include/wcslib
 PGLIB = -L/usr/X11R6/lib -lX11 -Wl,-framework -Wl,Foundation -L/sw/lib -lpng -lz -laquaterm -L/sw/lib/pgplot -lpgplot
 PGLIB2 = -L/usr/X11R6/lib -lX11 -L/sw/lib -lpng -lz -laquaterm -L/sw/lib/pgplot -lpgplot
 FC=gfortran -fbounds-check $(CFLAGS)
 FC1=gfortran -fbounds-check $(CFLAGS)
endif

FFLAG= -c 
VPATH=shapelets:shapelets/standalone

OBJ = fit_island.o fit_mulgauss1.o sm.o sourcemeasure.o readhead_srclist.o \
 sourcelistheaders.o mulsrc.o plotisland.o sub_prog.o imstat.o covsrt.o pgsub.o splint.o \
 spline.o splie2.o splin2.o getsurr_8pix.o mrqmin.o mrqmin2dm.o mrqcof2dm.o fgauss2dne.o \
 mrqmin2d.o mrqcof.o fourn.o mrqcof2d.o fgauss2d.o fgauss2de.o ran1.o gasdev.o fgauss.o \
 conv2.o conv2_get_wid_fac.o conv2_get_gaus.o cr8noisemap.o cr8constantim.o convolveimage.o \
 make_src_residim.o callfitgaussian.o detectdiscont.o medianfilter.o acf.o structfn.o \
 readarray.o writearray.o fileformat.o iland_mat2list.o islandedit.o \
 formislands.o getlistnsigpt.o readinimage.o arrstat.o arrstatmask.o selectnr.o sort2_4.o \
 sort3.o displaytwoim.o check.o displayim.o boxnoiseimage.o calcmedian.o \
 cr8modulationimage.o modulate.o subim.o transposeim.o get_gaus1_iniparm.o combineimage.o \
 callgetpixel.o callputpixel.o slice.o putpixel.o populatesrcs.o getcdf.o fitcdf.o \
 fitinparts.o imagestats.o nsigmacut.o experiment.o getco_ringwidth.o populateinparts.o \
 readheader.o nbit.o callmoment.o operateimage.o copyheader.o puthead.o func3.o \
 primarybeam.o filling.o func2.o gaussj.o gser.o gcf.o gammln.o gammq.o poly2.o poly3.o \
 getininp.o readininp.o basicgrey.o formisland_rank.o writefits.o printerror.o deletefile.o \
 splintmod.o read_head_coord.o xy2radec.o readfitshead.o cr8sources.o uarg.o \
 get_subim_xsrc.o clip.o associatesrl.o readgaul.o make_associatelist1.o write_associatelist1.o \
 plot_associatelist1.o boxf.o preprocess.o get_keyword.o rlft3.o get_numpix_val.o gammp.o erf.o\
 callrmsd.o bdsm_boxnoiseimage.o put_keyword.o writefitshead.o get_rmsmap.o deconv.o \
 soln_quality.o callwritefits.o read_paradefine.o callwritegaulfits.o callgaul2srl.o sigclip.o \
 sub_sourcemeasure_getblctrc.o sub_readislpix.o sub_calc_para_source.o writemenu_plotasrl.o\
 make_mask_gausid.o gaul_to_source.o calltest.o testmomentanal.o callwritesrlfits.o \
 writeaipsfiles.o blah.o read_dum_head.o callmrqmin2dm.o residgaus.o fit_mulgauss2.o sort3_1.o \
 sigclipmask.o bdsm_boxnoiseimagemask.o callremag.o analysepsf1.o indexx.o sort3_analysepsf1.o \
 erfc.o ksone.o probks.o sort.o ks_gaus.o plane3d.o analysepsf2.o callsort3_2.o sort3_2.o \
 callcalcmedian.o sub_analysepsf2_med.o callfdr.o xcr8noisemap.o readheader4fits.o analyse_asrl.o \
 sub_analyse_aasl_1.o bin_and_stats_ny.o callfit_aasl_1.o getpix11.o star2gaulsrl.o radec2xy.o \
 drawcoords.o isgoodpix.o overplotpasf.o callreadfits.o callavspc.o avspc.o  \
 qc_cc.o fitspix.o combinesrl.o wcs_struct.o wcs_xy2radec.o freewcs.o callcalcspin.o \
 wcs_isgoodpix.o wcs_radec2xy.o get_avmap.o set_colourmap.o mrqmin1dm.o mrqcof1dm.o \
 spinfunc0.o linfitm.o read_qcparadefine.o ranbdsm.o setup_grey.o pgsub_opts.o \
 call_qc_overlaygaussians.o sub_bdsm_sizes.o blah1.o call_qc_associatelists.o \
 readin_pasl.o setup_pasl.o getnlines_pasl.o call_qc_plot_associations.o \
 fillin_gaulist.o get_freq_cube.o calcmedian4.o selectnr4.o spinfunc1.o get_pgnumb.o \
 angle_local_north.o sub_sourcemeasure_writehead.o sub_sourcemeasure_writeheadmore.o \
 putin_srclist.o putsrlhead.o maxit.o check_keyword.o check_beaminheader.o fgauss2dse.o \
 write_shapfiles.o sub_sourcemeasure_writeheadsh.o sub_sourcemeasure_writeheadmoresh.o \
 call_writeresid.o call_getcoords.o callwriteshapfits.o call_psf_analyse.o get_culled_gaullist.o \
 sub_psf_cparray.o vec_stats.o main_psf_analyse.o psfvary_fullstat.o sigclip1d.o \
 calcmedianclip.o size_ksclip_wenss.o get_medianclip_vec.o get_bins.o calcmedianclip2vec.o \
 get_medianclip_vec2.o av_psf.o sort3_3.o get_voronoi_generators.o do_voronoi.o sort3_2i.o \
 tesselate.o read_psfparadefine.o testfft.o getinputarr8.o callfft.o cr8_wcs.o sigclip4.o \
 addimages.o callmwt.o get_atrous_h.o calc_atrous_cj.o call_threshold.o fn_radius.o convfft.o \
 makeshapes.o fftshift2.o plotqty_as_im.o calc_atrous_cj_mask.o writefits2Das4D.o callfullmoment.o \
 write_gaulsrl.o read_catalogue.o reconv.o bdsm_writeofname.o

OBJ2 = sub_simulation.o conv2_get_wid_fac.o cr8sky.o sub_cr8sky.o \
            sim_addnoisrc.o get_subim_ptsrc.o get_subim_xsrc.o get_subim_dubsrc.o \
            get_fitskeywords.o get_mosaicpara.o primarybeam3D_many.o sim_cr8noise.o \
            subim3D.o writearray.o writefits3D.o arrstat.o sigclip.o \
            readarray.o check.o sub_prog.o getcdf.o fitinparts.o \
            gasdev.o mrqmin.o mrqcof.o covsrt.o fgauss.o gaussj.o poly2.o poly3.o \
            func2.o ran1.o getsurr_8pix.o sourcelistheaders.o readhead_srclist.o\
            get_keyword.o read_simparadefine.o set_varypsf.o func3.o spinfunc0.o spinfunc1.o

OBJ3 = makelists1.o sub_prog.o 

OBJ4 = sub_simulation.o conv2_get_wid_fac.o cr8sky_ilse.o sub_cr8sky.o \
            sim_addnoisrc_ilse.o \
            get_fitskeywords.o get_mosaicpara.o primarybeam3D_many.o sim_cr8noise.o \
            subim3D.o writearray.o writefits3D.o arrstat.o sigclip.o \
            readarray.o check.o sub_prog.o getcdf.o fitinparts.o \
            gasdev.o mrqmin.o mrqcof.o covsrt.o fgauss.o gaussj.o poly2.o poly3.o \
            func2.o ran1.o getsurr_8pix.o sourcelistheaders.o readhead_srclist.o\
            get_keyword.o read_simparadefine.o set_varypsf.o

OBJ5 = sub_prog.o read_gsmparadefine.o ranasrl.o associatesrl.o get_asrl_params.o sourcelistheaders.o \
 readgaul.o make_associatelist1.o write_associatelist1.o selectnr.o readin_pasl.o setup_pasl.o \
 get_keyword.o ranbdsm.o getnlines_pasl.o readhead_srclist.o calcmedian.o sigclip1d.o call_multi_asrl.o \
 srllistheaders.o read_dum_head.o read_head_coord.o wcs_struct.o freewcs.o assoc_multiasrl.o \
 call_spin_multiasrl.o get_freq_cube.o mrqmin.o mrqcof.o gaussj.o covsrt.o fgauss.o poly2.o func2.o \
 poly3.o func3.o spinfunc0.o spinfunc1.o linfit.o gammq.o gser.o gcf.o gammln.o write_orgsm.o \
 coord2name.o gsm_plotspectra.o get_pgnumb.o gsm_calcflux_nofit.o gsm_plot_unfitted.o \
 wcs_xy2radec.o calcmedianclip.o get_medianclip_vec.o plotqty_as_im.o calcmedian4.o \
 star2gaulsrl.o readarray.o call_qc_overlaygaussians.o selectnr4.o wcs_radec2xy.o pgsub_opts.o \
 sub_sourcemeasure_writehead.o sub_sourcemeasure_writeheadmore.o writefitshead.o setup_grey.o \
 pgsub.o drawcoords.o overplotpasf.o wcs_isgoodpix.o set_colourmap.o putin_srclist.o putsrlhead.o \
 sigclip.o plotisland.o arrstat.o maxit.o basicgrey.o getsurr_8pix.o gsm_qc.o plot_associatelist1.o \
 calc_gsm.o calcmedian4clip.o ransubbdsm.o sigclip4.o displaytwoim.o call_calc_plot_median.o sort3_2.o \
 indexx.o call_calc_plot_std.o readheader4fits.o writemenu_plotasrl.o

OBJ6 =  getcartim.o recons_cart.o decomp_cart.o locsub.o s_displayshap.o s_findcen.o \
 fitstr8line.o getroot.o sub_prog.o readarray.o writearray.o mrqmin.o mrqcof.o covsrt.o gaussj.o \
 fgauss.o poly2.o func2.o poly3.o func3.o spinfunc0.o spinfunc1.o sigclip.o arrstat.o get_keyword.o \
 check.o 

OBJ7 = makeim_strongsrc.o conv2.o conv2_get_wid_fac.o conv2_get_gaus.o formisland_rank.o  \
 get_totrms_gaus1.o get_totrms_proper.o get_totrms_shape.o arrstatmask.o ran1.o xcr8noisemap.o \
 getsurr_8pix.o get_gaus1_iniparm_mask.o mrqmin2dm.o mrqcof2dm.o fgauss2de.o fgauss2dne.o \
 gammq.o gasdev.o callmoment.o plane3d.o gser.o gcf.o gammln.o 

OBJSH = fit_shapelets.o get_beta_cen_cart.o findcen_cart.o getcartim.o decomp_cart.o fitstr8line.o \
 getroot.o get_centroid_mask.o recons_cart.o varybeta_cart.o decomp_cart_mask.o 

OBJ8 = shapelets.o coeffpower.o locsub.o s_displayshap.o s_findcen.o varybeta.o simul_shapelets.o \
 main_shapelets.o makeim_strongsrc.o get_totrms_proper.o

OBJPG =  pgsub.o pgsub_opts.o setup_grey.o sourcelistheaders.o plotisland.o set_colourmap.o \
 readhead_srclist.o drawcoords.o overplotpasf.o basicgrey.o wcs_isgoodpix.o  wcs_radec2xy.o  wcs_struct.o \
 freewcs.o getsurr_8pix.o read_head_coord.o getroot.o

OBJAI = read_aiparadefine.o sub_prog.o ranbdsm1.o get_keyword.o call_analyse_isl.o readarray.o show_shap.o \
 writefits.o printerror.o deletefile.o read_dum_head.o readheader4fits.o displayshap.o readhead_srclist.o \
 readin_shapcoef.o writefitsshapcoef.o

OBJAT = sub_prog.f xcr8noisemap.f conv2_get_wid_fac.f conv2.f writearray.f get_keyword.f gasdev.f \
 check.f conv2_get_gaus.f ran1.f sigclip.o get_atrous_h.o calc_atrous_cj.o calcmedian.o selectnr.o \
 arrstat.o get_pgnumb.o calc_atrous_cj_mask.o

OBJCON = sub_prog.o ran1.o sigclip.o arrstat.o writearray.o conv2_get_wid_fac.o conv2.o \
 readarray.o get_keyword.o check.o conv2_get_gaus.o

OBJ_pysub1 = sourcelistheaders.o readgaul.o read_psfparadefine.o get_keyword.o sub_prog.o vec_stats.o \
 calcmedianclip.o get_medianclip_vec.o calcmedian.o readhead_srclist.o selectnr.o sigclip1d.o

OBJ_anaamikapyf = cr8noisemap.f sm.f shapelets.f readinimage.f callputpixel.f callgetpixel.f \
 cr8constantim.f nbit.f fileformat.f detectdiscont.f calltest.f callmwt.f fn_radius.f fftshift2.f \
 makeshapes.f convolveimage.f clip.f cr8sources.f experiment.f cr8modulationimage.f callfitgaussian.f \
 modulate.f medianfilter.f boxnoiseimage.f callmoment.f transposeim.f displaytwoim.f displayim.f \
 acf.f structfn.f populatesrcs.f addimages.f combineimage.f operateimage.f subim.f imagestats.f \
 nsigmacut.f primarybeam.f readheader.f copyheader.f puthead.f slice.f callremag.f callfft.f \
 call_getcoords.f callreadfits.f shapelets/varybeta_cart.f shapelets/findcen_cart.f

OBJ_pyasso = associatesrl.o sourcelistheaders.o get_freq_cube.o readgaul.o make_associatelist1.o write_associatelist1.o \
 sub_prog.o readhead_srclist.o selectnr.o get_keyword.o readin_pasl.o call_wcs_c2p.o wcs_struct.o \
 wcs_xy2radec.o call_wcs_p2c.o wcs_radec2xy.o writemenu_plotasrl.o plot_associatelist1.o \
 readheader4fits.o getnlines_pasl.o calcmedian4.o selectnr4.o setup_pasl.o get_pgnumb.o pgsub.o \
 pgsub_opts.o sigclip.o arrstat.o overplotpasf.o drawcoords.o wcs_isgoodpix.o set_colourmap.o \
 plotisland.o readarray.o displaytwoim.o setup_grey.o getsurr_8pix.o basicgrey.o sigclip4.o \
 read_head_coord.o calcmedian.o

OBJ_py2fbdsm = iland_mat2list.o writearray.o gaul2gaulbin.o writefitshead.o sub_prog.o readarray.o get_keyword.o check.o
OBJ_py2fbdsmf = iland_mat2list.f writearray.f gaul2gaulbin.f writefitshead.f 

SRC_pyasso = associatesrl.f readin_pasl.f call_wcs_c2p.f call_wcs_p2c.f plot_associatelist1.f sub_prog.f \
 setup_pasl.f

OBJ_fit = mrqmin.o mrqcof.o ran1.o sub_prog.o covsrt.o gaussj.o poly2.o poly3.o func2.o fgauss.o \
 func3.o spinfunc0.o spinfunc1.o 

testfit : testfit.f $(OBJ_fit)
	$(FC) testfit.f $(OBJ_fit) -o testfit

all: noise

a : a.f $(OBJ) $(OBJ8) $(OBJSH)
	$(FC) a.f $(OBJ) $(OBJ8) $(CFLAGS) $(OBJSH)   $(WCSLIB) $(CFITS) -o a

noise : noise.f $(OBJ) $(OBJ8) $(OBJSH)
	$(FC) noise.f $(OBJ) $(OBJ8) $(CFLAGS) $(OBJSH)   $(WCSLIB) $(CFITS) -o noise

_anaamika.pyf : 
	f2py -include'trhermcoef.inc' $(OBJ_anaamikapyf) -m _anaamika -h _anaamika.pyf

_anaamika.so: _anaamika.pyf $(OBJ) $(OBJ8) $(OBJSH)
	f2py --fcompiler=gfortran -c _anaamika.pyf $(OBJ) $(OBJ8) $(OBJSH)   $(WCSLIB) $(CFITS)

_fbdsm_sub1.pyf : 
	f2py get_keyword.f readgaul.f vec_stats.f read_psfparadefine.f sourcelistheaders.f -m _fbdsm_sub1 -h _fbdsm_sub1.pyf

_fbdsm_sub1.so: _fbdsm_sub1.pyf $(OBJ) $(OBJ8) $(OBJSH) 
	f2py --fcompiler=gnu95 -c _fbdsm_sub1.pyf $(OBJ_pysub1) dummy.f

_py2fbdsm.pyf : 
	f2py $(OBJ_py2fbdsmf) -m _py2fbdsm -h _py2fbdsm.pyf

_py2fbdsm.so: _py2fbdsm.pyf $(OBJ_py2fbdsm)
	f2py --fcompiler=gnu95 -c _py2fbdsm.pyf $(OBJ_py2fbdsm) dummy.f

_pytesselate.pyf : 
	f2py pytess_simple.f pytess_roundness.f -m _pytesselate -h _pytesselate.pyf

_pytesselate.so: _pytesselate.pyf pytess_simple.o pytess_roundness.o
	f2py --fcompiler=gnu95 -c _pytesselate.pyf pytess_simple.o pytess_roundness.o dummy.f 

_pyasso.pyf : 
	f2py -include'constants.inc' $(SRC_pyasso) -m _pyasso -h _pyasso.pyf

_pyasso.so: _pyasso.pyf $(OBJ_pyasso)
	f2py --fcompiler=gnu95 -c _pyasso.pyf $(OBJ_pyasso) $(WCSLIB)   dummy.f

bdsm : bdsm.f $(OBJ) $(OBJSH) 
	$(FC) bdsm.f $(OBJ) $(OBJSH) $(CFLAGS)   $(WCSLIB) $(CFITS) -o bdsm

_fbdsm.pyf : 
	f2py fbdsm.f -m _fbdsm -h _fbdsm.pyf

_fbdsm.so: _fbdsm.pyf fbdsm.o $(OBJ) $(OBJSH)
	f2py --fcompiler=gnu95 -c _fbdsm.pyf fbdsm.o $(OBJ) $(OBJSH) $(WCSLIB) $(CFITS) dummy.f

simulation : simulation.f $(OBJ2)
	$(FC) simulation.f $(OBJ2) $(CFITS) -o simulation

simulation_ilse : simulation_ilse.f $(OBJ4)
	$(FC) simulation_ilse.f $(OBJ4) $(CFITS) -o simulation_ilse

qualitycontrol : qualitycontrol.f $(OBJ) $(OBJ8) $(OBJSH)
	$(FC) qualitycontrol.f $(OBJ) $(OBJ8) $(CFLAGS) $(OBJSH) $(WCSLIB)   $(CFITS) -o qualitycontrol

or_gsm : or_gsm.f $(OBJ5)
	$(FC) or_gsm.f $(OBJ5) $(WCSLIB)   $(CFITS) -o or_gsm

analyse_island : analyse_island.f $(OBJAI) 
	$(FC) analyse_island.f $(OBJAI) $(CFITS)   -o analyse_island

srccomp1 : srccomp1.f $(OBJ3)
	$(FC) srccomp1.f $(OBJ3)   -o srccomp1

testconfusionnoise : testconfusionnoise.f $(OBJCON)
	$(FC) testconfusionnoise.f $(OBJCON)   -o testconfusionnoise

testconfusionfull : testconfusionfull.f $(OBJCON)
	$(FC) testconfusionfull.f $(OBJCON)   -o testconfusionfull

confusion_lofar_synbeam : confusion_lofar_synbeam.f $(OBJCON)
	$(FC) confusion_lofar_synbeam.f $(OBJCON) -o confusion_lofar_synbeam

model : model.f 
	$(FC) model.f -o model

testranbdsm : testranbdsm.f ranbdsm.o read_paradefine.o get_keyword.o sub_prog.o 
	$(FC) testranbdsm.f ranbdsm.o read_paradefine.o get_keyword.o sub_prog.o -o testranbdsm

test_atrous_noise : test_atrous_noise.f $(OBJAT)
	$(FC) test_atrous_noise.f $(OBJAT)   -o test_atrous_noise

#a : a.f combineimage.o readarray.o writearray.o sub_prog.o check.o
#	$(FC) -o a a.f combineimage.o readarray.o writearray.o sub_prog.o check.o
#
wcs_struct.o : wcs_struct.f 
	$(FC1) -c -o wcs_struct.o wcs_struct.f $(WCSINC)
wcs_struct1.o : wcs_struct1.f 
	$(FC1) -c -o wcs_struct1.o wcs_struct1.f $(WCSINC)
wcs_xy2radec.o : wcs_xy2radec.f 
	$(FC1) -c -o wcs_xy2radec.o wcs_xy2radec.f $(WCSINC)
wcs_radec2xy.o : wcs_radec2xy.f 
	$(FC1) -c -o wcs_radec2xy.o wcs_radec2xy.f $(WCSINC)
freewcs.o : freewcs.f 
	$(FC1) -c -o freewcs.o freewcs.f $(WCSINC)
wcs_isgoodpix.o : wcs_isgoodpix.f 
	$(FC1) -c -o wcs_isgoodpix.o wcs_isgoodpix.f $(WCSINC)

makelists1.o : makelists1.f sub_prog.o 
sub_simulation.o : sub_simulation.f 
conv2_get_wid_fac.o : conv2_get_wid_fac.f
cr8sky.o : cr8sky.f sub_cr8sky.o sub_prog.o ran1.o sigclip.o writearray.o
cr8sky_ilse.o : cr8sky_ilse.f sub_cr8sky.o sub_prog.o ran1.o sigclip.o writearray.o
sub_cr8sky.o : sub_cr8sky.f ran1.o 
sim_addnoisrc.o : sim_addnoisrc.f primarybeam3D_many.o subim3D.o writearray.o 
sim_addnoisrc_ilse.o : sim_addnoisrc_ilse.f primarybeam3D_many.o subim3D.o writearray.o 
get_subim_ptsrc.o : get_subim_ptsrc.f
get_subim_xsrc.o : get_subim_xsrc.f 
get_subim_dubsrc.o : get_subim_dubsrc.f 
get_fitskeywords.o : get_fitskeywords.f
get_mosaicpara.o : get_mosaicpara.f 
primarybeam3D_many.o : primarybeam3D_many.f readarray.o writearray.o 
sim_cr8noise.o : sim_cr8noise.f conv2_get_wid_fac.o writearray.o gasdev.o
subim3D.o : subim3D.f
writefits3D.o : writefits3D.f 
printerror.o : printerror.f
deletefile.o : deletefile.f 
arrstat.o : arrstat.f
arrstatmask.o : arrstatmask.f
pgsub.o : pgsub.f sub_prog.o sourcelistheaders.o plotisland.o get_keyword.o drawcoords.o \
 overplotpasf.o set_colourmap.o setup_grey.o pgsub_opts.o
call_qc_overlaygaussians.o : call_qc_overlaygaussians.f sub_prog.o sourcelistheaders.o \
 plotisland.o get_keyword.o drawcoords.o \
 overplotpasf.o set_colourmap.o setup_grey.o pgsub_opts.o
pgsub_opts.o : pgsub_opts.f sub_prog.o
setup_grey.o : setup_grey.f sub_prog.o
set_colourmap.o : set_colourmap.f sub_prog.o
getcdf.o : getcdf.f
fitcdf.o : fitcdf.f
fitinparts.o : fitinparts.f mrqmin.o
gasdev.o : gasdev.f
mrqmin.o : mrqmin.f mrqcof.o covsrt.o gaussj.o
mrqmin2d.o : mrqmin2d.f mrqcof2d.o covsrt.o gaussj.o
mrqmin2dm.o : mrqmin2dm.f mrqcof2dm.o covsrt.o gaussj.o
mrqmin1dm.o : mrqmin1dm.f mrqcof1dm.o covsrt.o gaussj.o
mrqcof.o : mrqcof.f fgauss.o poly2.o func2.o poly3.o func3.o spinfunc0.o spinfunc1.o
mrqcof2d.o : mrqcof2d.f fgauss2de.o fgauss2dse.o
mrqcof2dm.o : mrqcof2dm.f fgauss2de.o fgauss2dne.o
mrqcof1dm.o : mrqcof1dm.f spinfunc0.o
spinfunc0.o : spinfunc0.f
spinfunc1.o : spinfunc1.f
covsrt.o : covsrt.f
splint.o : splint.f
spline.o : spline.f
splintmod.o : splintmod.f
fgauss.o : fgauss.f
gaussj.o : gaussj.f
poly2.o : poly2.f
poly3.o : poly3.f
func2.o : func2.f
func3.o : func3.f
ran1.o: ran1.f
erf.o : erf.f gammp.o
gammp.o : gammp.f gcf.o gser.o 
gcf.o : gcf.f gammln.o 
gser.o : gser.f gammln.o 
gammln.o : gammln.f 

combineimage.o : combineimage.f readarray.o writearray.o 
readarray.o : readarray.f sub_prog.o get_keyword.o 
writearray.o : writearray.f sub_prog.o check.o get_keyword.o
sub_prog.o : sub_prog.f
check.o : check.f

acf.o : acf.f readarray.o writearray.o callfitgaussian.o sub_prog.o sort2_4.o
boxnoiseimage.o : boxnoiseimage.f readarray.o arrstat.o filling.o pgsub.o writearray.o \
      calcmedian.o splie2.o splin2.o sigclip.o

sm.o : sm.f formislands.o islandedit.o getininp.o iland_mat2list.o plotisland.o \
         readininp.o sourcemeasure.o make_src_residim.o associatesrl.o plot_associatelist1.o \
         preprocess.o callrmsd.o get_keyword.o star2gaulsrl.o writemenu_plotasrl.o

plotisland.o : plotisland.f sourcelistheaders.o readarray.o sub_prog.o basicgrey.o \
   getsurr_8pix.o get_keyword.o
basicgrey.o : basicgrey.f sub_prog.o sigclip4.o
sourcelistheaders.o : sourcelistheaders.f readhead_srclist.o
readhead_srclist.o : readhead_srclist.f
sourcemeasure.o : sourcemeasure.f readarray.o pgsub.o fit_island.o sub_prog.o get_keyword.o \
 sub_sourcemeasure_getblctrc.o sub_readislpix.o wcs_struct.o freewcs.o sub_sourcemeasure_writehead.o \
 sub_sourcemeasure_writeheadmore.o sub_sourcemeasure_writeheadsh.o sub_sourcemeasure_writeheadmoresh.o
sub_sourcemeasure_writeheadsh.o : sub_sourcemeasure_writeheadsh.f
sub_sourcemeasure_writehead.o : sub_sourcemeasure_writehead.f sub_prog.o
sub_sourcemeasure_writeheadmore.o : sub_sourcemeasure_writeheadmore.f sub_prog.o
sub_sourcemeasure_writeheadmoresh.o : sub_sourcemeasure_writeheadmoresh.f sub_prog.o
sub_sourcemeasure_getblctrc.o : sub_sourcemeasure_getblctrc.f
sub_readislpix.o : sub_readislpix.f
fit_island.o : fit_island.f sub_prog.o fit_mulgauss1.o xy2radec.o get_keyword.o wcs_xy2radec.o \
 fillin_gaulist.o angle_local_north.o putin_srclist.o maxit.o
putin_srclist.o : putin_srclist.f sub_prog.o wcs_xy2radec.o 
angle_local_north.o : angle_local_north.f wcs_xy2radec.o wcs_radec2xy.o
fillin_gaulist.o : fillin_gaulist.f
fit_mulgauss1.o : fit_mulgauss1.f mrqmin2dm.o sub_prog.o arrstatmask.o deconv.o soln_quality.o
fit_mulgauss2.o : fit_mulgauss2.f mrqmin2dm.o sub_prog.o arrstatmask.o deconv.o soln_quality.o \
 residgaus.o
soln_quality : soln_quality.f
callgetpixel.o : callgetpixel.f readarray.o sub_prog.o basicgrey.o 
callfitgaussian.o : callfitgaussian.f readarray.o pgsub.o sub_prog.o basicgrey.o \
      get_gaus1_iniparm.o mrqmin2d.o writearray.o displaytwoim.o sigclip.o
callmoment.o : callmoment.f readarray.o pgsub.o  sigclip.o
callfullmoment.o : callfullmoment.f readarray.o pgsub.o  sigclip.o sub_prog.o
callputpixel.o : callputpixel.f readarray.o writearray.o putpixel.o
convolveimage.o : convolveimage.f readarray.o pgsub.o conv2.o imstat.o writearray.o \
getininp.o readininp.o boxf.o
box.o : boxf.f conv2.o
copyheader.o : copyheader.f
conv2.o : conv2.f conv2_get_wid_fac.o conv2_get_gaus.o sub_prog.o 
conv2_get_gaus.o : conv2_get_gaus.f
cr8constantim.o : cr8constantim.f writearray.o
cr8modulationimage.o : cr8modulationimage.f ran1.o writearray.o pgsub.o
cr8noisemap.o : cr8noisemap.f writearray.o pgsub.o gasdev.o
detectdiscont.o : detectdiscont.f readarray.o writearray.o
displayim.o : displayim.f readarray.o pgsub.o
displaytwoim.o : displaytwoim.f readarray.o sub_prog.o 
experiment.o : experiment.f rlft3.o fourn.o mulsrc.o testfft.o
rlft3.o : rlft3.f fourn.o
fourn.o : fourn.f
fileformat.o : fileformat.f readarray.o writearray.o writefits.o writefits2Das4D.o
formislands.o : formislands.f readarray.o pgsub.o sub_prog.o get_keyword.o \
       getlistnsigpt.o getsurr_8pix.o writearray.o get_rmsmap.o sigclip.o \
       wcs_struct.o freewcs.o
formisland_rank.o : formisland_rank.f readarray.o pgsub.o sub_prog.o get_keyword.o \
       getlistnsigpt.o getsurr_8pix.o writearray.o get_rmsmap.o sigclip.o
getininp.o : getininp.f
getsurr_8pix.o : getsurr_8pix.f
getlistnsigpt.o : getlistnsigpt.f sort3.o get_keyword.o wcs_isgoodpix.o
iland_mat2list.o : iland_mat2list.f readarray.o sourcemeasure.o
imstat.o : imstat.f sub_prog.o arrstat.o mrqmin.o
imagestats.o : imagestats.f readarray.o arrstat.o pgsub.o sub_prog.o sigclip.o arrstatmask.o \
 sigclipmask.o
islandedit.o : islandedit.f 
medianfilter.o : medianfilter.f readarray.o calcmedian.o filling.o writearray.o pgsub.o
calcmedian.o : calcmedian.f selectnr.o
selectnr.o : selectnr.f
selectnr4.o : selectnr4.f
calcmedian4.o : calcmedian4.f selectnr4.o
modulate.o : modulate.f readarray.o imstat.o writearray.o pgsub.o
make_src_residim.o : make_src_residim.f sourcelistheaders.o readarray.o sub_prog.o \
   writearray.o get_keyword.o sigclip.o
nbit.o : nbit.f readarray.o pgsub.o getininp.o readininp.o writearray.o
nsigmacut.o : nsigmacut.f readarray.o pgsub.o writearray.o sigclip.o
operateimage.o : operateimage.f getininp.o readininp.o readarray.o sub_prog.o \
   writearray.o 

populatesrcs.o : populatesrcs.f populateinparts.o fitcdf.o getcdf.o puthead.o \
   fitinparts.o sigclip.o
populateinparts.o : populateinparts.f sub_prog.o ran1.o arrstat.o pgsub.o writearray.o 
primarybeam.o : primarybeam.f readarray.o writearray.o
putpixel.o : putpixel.f sub_prog.o 
puthead.o : puthead.f 
readheader.o : readheader.f
readinimage.o : readinimage.f 
readininp.o : readininp.f
slice.o : slice.f readarray.o sub_prog.o basicgrey.o 
sort2_4.o : sort2_4.f
sort3.o : sort3.f
sort3_1.o : sort3_1.f
structfn.o : structfn.f readarray.o arrstat.o writearray.o acf.o 
subim.o : subim.f readarray.o imagestats.o writearray.o sub_prog.o 
transposeim.o : transposeim.f readarray.o writearray.o
xy2radec.o : xy2radec.f read_head_coord.o uarg.o
uarg.o : uarg.f
read_head_coord.o : read_head_coord.f get_keyword.o
writefits.o : writefits.f printerror.o deletefile.o sub_prog.o read_dum_head.o readheader4fits.o
writefits2Das4D.o : writefits2Das4D.f printerror.o deletefile.o sub_prog.o read_dum_head.o readheader4fits.o
readheader4fits.o : readheader4fits.f get_keyword.o sub_prog.o
callreadfits.o : callreadfits.f printerror.o deletefile.o writearray.o readfitshead.o check_beaminheader.o
check_beaminheader.o : check_beaminheader.f check_keyword.o get_keyword.o put_keyword.o
check_keyword.o : check_keyword.f
readfitshead.o : readfitshead.f printerror.o deletefile.o
cr8sources.o : cr8sources.f sub_prog.o ran1.o get_subim_xsrc.o sourcemeasure.o read_dum_head.o\
fit_island.o sub_cr8sky.o cr8noisemap.o writearray.o conv2.o conv2_get_wid_fac.o \
writefitshead.o sigclip.o put_keyword.o sort3_1.o callgaul2srl.o freewcs.o wcs_struct.o \
putsrlhead.o
putsrlhead.o : putsrlhead.f
clip.o : clip.f readarray.o imagestats.o writearray.o 
associatesrl.o : associatesrl.f sourcelistheaders.o readgaul.o selectnr.o make_associatelist1.o \
write_associatelist1.o get_keyword.o
readgaul.o : readgaul.f sub_prog.o
make_associatelist1.o : make_associatelist1.f sub_prog.o
write_associatelist1.o : write_associatelist1.f sub_prog.o
plot_associatelist1.o : plot_associatelist1.f sub_prog.o readin_pasl.o setup_pasl.o \
 getnlines_pasl.o calcmedian4.o get_pgnumb.o writemenu_plotasrl.o
getnlines_pasl.o : getnlines_pasl.f sub_prog.o
setup_pasl.o : setup_pasl.f
readin_pasl.o : readin_pasl.f sub_prog.o
preprocess.o : preprocess.f read_dum_head.o read_head_coord.o readarray.o imagestats.o get_keyword.o\
 get_numpix_val.o erf.o put_keyword.o writefitshead.o callfdr.o getpix11.o
callfdr.o : callfdr.f readarray.o sub_prog.o sort.o
writefitshead.o : writefitshead.f sub_prog.o
get_numpix_val.o : get_numpix_val.f readarray.o
callrmsd.o : callrmsd.f readarray.o get_keyword.o bdsm_boxnoiseimage.o pgsub.o put_keyword.o \
 bdsm_boxnoiseimagemask.o
get_keyword.o : get_keyword.f sub_prog.o
bdsm_boxnoise.f : bdsm_boxnoise.o readarray.o arrstat.o filling.o pgsub.o writearray.o \
      calcmedian.o splie2.o splin2.o sigclip.o
bdsm_boxnoisemask.f : bdsm_boxnoisemask.o readarray.o arrstat.o filling.o pgsub.o writearray.o \
      calcmedian.o splie2.o splin2.o sigclip.o
put_keyword.o : put_keyword.f
get_rmsmap.o : get_rmsmap.f get_keyword.o sub_prog.o readarray.o
get_avmap.o : get_avmap.f get_keyword.o sub_prog.o readarray.o
deconv.o : deconv.f
mulsrc.o : mulsrc.f sub_prog.f getsurr_8pix.o readarray.o pgsub.o sigclip.o
callwritefits.o : callwritefits.f sub_prog.o readarray.o get_keyword.o writefits.o
read_paradefine.o : read_paradefine.f sub_prog.o get_keyword.o
read_qcparadefine.o : read_qcparadefine.f sub_prog.o get_keyword.o
read_gsmparadefine.o : read_gsmparadefine.f sub_prog.o get_keyword.o
ranbdsm.o : ranbdsm.f sub_prog.o
ransubbdsm.o : ransubbdsm.f sub_prog.o
read_simparadefine.o : read_simparadefine.f sub_prog.o get_keyword.o
callwritegaulfits.o : callwritegaulfits.f sourcelistheaders.o sub_prog.o printerror.o \
      deletefile.o
callwritesrlfits.o : callwritesrlfits.f sub_prog.o printerror.o \
     deletefile.o readhead_srclist.o
callwriteshapfits.o : callwriteshapfits.f sourcelistheaders.o sub_prog.o printerror.o \
      deletefile.o
callgaul2srl.o : callgaul2srl.f sourcelistheaders.o sub_prog.o gaul_to_source.o \
      sub_calc_para_source.o
gaul_to_source.o : gaul_to_source.f sub_prog.o sourcemeasure.o xy2radec.o
sub_calc_para_source.o : sub_calc_para_source.f sub_prog.o make_mask_gausid.o \
 sub_sourcemeasure_getblctrc.o sub_readislpix.o
make_mask_gausid.o : make_mask_gausid.f 
sigclip.o : sigclip.f arrstat.o
sigclip4.o : sigclip4.f sigclip.o sub_prog.o
sigclip1d.o : sigclip1d.f sub_prog.o
sigclipmask.o : sigclipmask.f arrstatmask.o
calltest.o : calltest.f get_keyword.o sub_prog.o getininp.o readininp.o testmomentanal.o \
 analysepsf1.o analysepsf2.o
testmomentanal.o : testmomentanal.f ran1.o conv2_get_wid_fac.o cr8noisemap.o conv2.o sigclip.o
set_varypsf.o : set_varypsf.f
writeaipsfiles.o : writeaipsfiles.f sub_prog.o readhead_srclist.o
read_dum_head.o : read_dum_head.f
callmrqmin2dm.o : callmrqmin2dm.f mrqmin2dm.o
blah.o : blah.f readarray.o
blah1.o : blah1.f readarray.o
residgaus.o : residgaus.f
callremag.o : callremag.f readarray.o writearray.o
analysepsf1.o : analysepsf1.f sub_prog.o sort3_analysepsf1.o readarray.o writearray.o \
 readhead_srclist.o imagestats.o ksone.o ks_gaus.o plane3d.o
analysepsf2.o : analysepsf2.f sub_prog.o sort3_analysepsf1.o readarray.o writearray.o \
 readhead_srclist.o imagestats.o ksone.o ks_gaus.o plane3d.o sub_analysepsf2_med.o
sub_analysepsf2_med.o : sub_analysepsf2_med.f callsort3_2.o callcalcmedian.o
callcalcmedian.o : callcalcmedian.f calcmedian.o
callsort3_2.o : callsort3_2.f sort3_2.o
sort3_2.o : sort3_2.f
sort3_2i.o : sort3_2i.f
ksone.o : ksone.f probks.o sort.o
probks.o : probks.f
sort.o : sort.f
ks_gaus.o : ks_gaus.f erfc.o
erfc.o : erfc.f gammp.o gammq.o
gammq.o : gammq.f gser.o gcf.o
sort3_analysepsf1.o : sort3_analysepsf1.f indexx.o
indexx.o : indexx.f
plane3d.o : plane3d.f
xcr8noisemap.o : xcr8noisemap.f gasdev.o 
analyse_asrl.o : analyse_asrl.f sub_prog.o sub_analyse_aasl_1.o sort3_2.o callfit_aasl_1.o get_bins.o 
get_bins.o : get_bins.f
callfit_aasl_1.o : callfit_aasl_1.f mrqmin.o
sub_analyse_aasl_1.o : sub_analyse_aasl_1.f sub_prog.o bin_and_stats_ny.o
bin_and_stats_ny.o : bin_and_stats_ny.f sub_prog.o calcmedianclip2vec.o
calcmedianclip2vec.o : calcmedianclip2vec.f calcmedianclip.o sub_prog.o get_medianclip_vec2.o
get_medianclip_vec2.o : get_medianclip_vec2.f 
getpix11.o : getpix11.f get_keyword.o sub_prog.o
star2gaulsrl.o : star2gaulsrl.f sub_prog.o read_head_coord.o sourcelistheaders.o radec2xy.o sourcemeasure.o \
 writefitshead.o fit_island.o cr8sources.o wcs_radec2xy.o
radec2xy.o : radec2xy.f
drawcoords.o : drawcoords.f read_head_coord.o isgoodpix.o sub_prog.o wcs_isgoodpix.o wcs_struct.o \
 freewcs.o wcs_radec2xy.o
isgoodpix.o : isgoodpix.f radec2xy.o xy2radec.o sub_prog.o
overplotpasf.o : overplotpasf.f read_head_coord.o sub_prog.o radec2xy.o wcs_radec2xy.o
callavspc.o : callavspc.f sub_prog.o readarray.o writearray.o get_keyword.o
avspc.o : avspc.f readarray.o writearray.o
qc_cc.o : qc_cc.f imagestats.o
callcalcspin.o : callcalcspin.f sourcelistheaders.o sub_prog.o readhead_srclist.o get_keyword.o \
 readarray.o sub_readislpix.o fitspix.o 
fitspix.o : fitspix.f mrqmin1dm.o linfitm.o
linfitm.o : linfitm.f gammq.o
linfit.o : linfit.f gammq.o
combinesrl.o : combinesrl.f
sub_bdsm_sizes.o : sub_bdsm_sizes.f readarray.o
call_qc_associatelists.o : call_qc_associatelists.f sub_prog.o associatesrl.o get_keyword.o 
call_qc_plot_associations.o : call_qc_plot_associations.f get_keyword.o sub_prog.o readin_pasl.o \
 setup_pasl.o plot_associatelist1.o writemenu_plotasrl.o
call_multi_asrl.o : call_multi_asrl.f sub_prog.o srllistheaders.o get_freq_cube.o call_spin_multiasrl.o \
 write_orgsm.o gsm_plotspectra.o gsm_calcflux_nofit.o gsm_plot_unfitted.o wcs_xy2radec.o gsm_qc.o \
 calc_gsm.o
calc_gsm.o : calc_gsm.f sub_prog.o 
gsm_qc.o : gsm_qc.f sub_prog.o calcmedian.o calcmedianclip.o sigclip1d.o plot_associatelist1.o call_calc_plot_median.o writemenu_plotasrl.o
call_calc_plot_median.o : call_calc_plot_median.f
call_calc_plot_std.o : call_calc_plot_std.f
gsm_plot_unfitted.o : gsm_plot_unfitted.f plotqty_as_im.o
plotqty_as_im.o : plotqty_as_im.f sub_prog.o calcmedian4.o star2gaulsrl.o readarray.o \
 call_qc_overlaygaussians.o
gsm_calcflux_nofit.o : gsm_calcflux_nofit.f
gsm_plotspectra.o : gsm_plotspectra.f coord2name.o 
coord2name.o : coord2name.f sub_prog.o
write_orgsm.o : write_orgsm.f sub_prog.o
call_spin_multiasrl.o : call_spin_multiasrl.f linfit.o mrqmin.o
get_freq_cube.o : get_freq_cube.f get_keyword.o
srllistheaders.o : srllistheaders.f readhead_srclist.o
assoc_multiasrl.o : assoc_multiasrl.f sub_prog.o
get_asrl_params.o : get_asrl_params.f sub_prog.o getnlines_pasl.o calcmedian.o sigclip1d.o calcmedianclip.o
calcmedianclip.o : calcmedianclip.f sub_prog.o selectnr.o get_medianclip_vec.o
calcmedian4clip.o : calcmedian4clip.f sub_prog.o selectnr.o get_medianclip_vec.o
get_medianclip_vec.o : get_medianclip_vec.f 
a.o : a.f
b.o : b.f
getcartim.o : getcartim.f sub_prog.o 
recons_cart.o : recons_cart.f getcartim.o sub_prog.o 
decomp_cart.o : decomp_cart.f getcartim.o 
decomp_cart_mask.o : decomp_cart_mask.f getcartim.o 
locsub.o : locsub.f 
s_displayshap.o : s_displayshap.f readarray.o sub_prog.o 
displayshap.o : displayshap.f readarray.o sub_prog.o 
s_findcen.o : s_findcen.f sub_prog.o getcartim.o decomp_cart.o fitstr8line.o writearray.o
fitstr8line.o : fitstr8line.f mrqmin.o mrqcof.o covsrt.o gaussj.o fgauss.o poly2.o func2.o poly3.o \
 func3.o spinfunc0.o spinfunc1.o 
getroot.o : getroot.f fitstr8line.o 
makeim_strongsrc.o : makeim_strongsrc.f conv2.o sub_prog.o 
get_totrms_gaus1.o : get_totrms_gaus1.f arrstatmask.o writearray.o get_gaus1_iniparm_mask.o \
 mrqmin2dm.o 
get_totrms_proper.o : get_totrms_proper.f arrstatmask.o sigclip.o
get_totrms_shape.o : get_totrms_shape.f sub_prog.o locsub.o get_totrms_gaus1.o sigclip.o s_findcen.o \
 decomp_cart.o recons_cart.o arrstat.o arrstatmask.o s_displayshap.o writearray.o 
get_gaus1_iniparm_mask.o : get_gaus1_iniparm_mask.f sub_prog.o callmoment.o
fit_shapelets.o : fit_shapelets.f sub_prog.o get_beta_cen_cart.o decomp_cart_mask.o recons_cart.o \
 write_shapfiles.o
write_shapfiles.o : write_shapfiles.f
get_beta_cen_cart.o : get_beta_cen_cart.f findcen_cart.o get_centroid_mask.o varybeta_cart.o
varybeta_cart.o : varybeta_cart.f sub_prog.o decomp_cart.o recons_cart.o arrstatmask.o decomp_cart_mask.o
get_centroid_mask.o : get_centroid_mask.f
findcen_cart.o : findcen_cart.f sub_prog.o getcartim.o decomp_cart.o fitstr8line.o writearray.o \
 getroot.o decomp_cart_mask.o callmoment.o
call_writeresid.o : call_writeresid.f callwritefits.o sub_prog.o

call_analyse_isl.o : call_analyse_isl.f readin_shapcoef.o
show_shap.o : show_shap.f sub_prog.o writefits.o writefitsshapcoef.o 
readin_shapcoef.o : readin_shapcoef.f
writefitsshapcoef.o : writefitsshapcoef.f sub_prog.o
call_getcoords.o : call_getcoords.f sub_prog.o callreadfits.o read_dum_head.o read_head_coord.o \
 wcs_struct.o freewcs.o wcs_xy2radec.o
shapelets.o : shapelets.f getininp.o readininp.o sub_prog.o varybeta.o simul_shapelets.o \
 main_shapelets.o
call_psf_analyse.o : call_psf_analyse.f sourcelistheaders.o readgaul.o get_culled_gaullist.o \
 sub_psf_cparray.o sub_prog.o main_psf_analyse.o read_psfparadefine.o 
read_psfparadefine.o : read_psfparadefine.f get_keyword.o
main_psf_analyse.o : main_psf_analyse.f psfvary_fullstat.o size_ksclip_wenss.o \
 get_voronoi_generators.o do_voronoi.o
do_voronoi.o : do_voronoi.f sort3_2i.o tesselate.o
tesselate.o : tesselate.f sub_prog.o
get_voronoi_generators.o : get_voronoi_generators.f sort3_3.o
sort3_3.o : sort3_3.f 
size_ksclip_wenss.o : size_ksclip_wenss.f
psf_varyfullstat.o : psf_varyfullstat.f vec_stats.o
get_culled_gaullist.o : get_culled_gaullist.f
sub_psf_cparray.o : sub_psf_cparray.f
vec_stats.o : vec_stats.f calcmedian.o calcmedianclip.o sub_prog.o sigclip1d.o
av_psf.o : av_psf.f
testfft.o : testfft.f rlft3.o readarray.o writearray.o getinputarr8.o
getinputarr8.o : getinputarr8.f
callfft.o : callfft.f writearray.o readarray.o rlft3.o
cr8_wcs.o : cr8_wcs.f wcs_struct.o freewcs.o read_dum_head.o read_head_coord.o sub_prog.o
addimages.o : addimages.f readarray.o writearray.o sub_prog.o
callmwt.o : callmwt.f sub_prog.o readarray.o writearray.o sigclip.o calc_atrous_cj.o \
 get_atrous_h.o calc_atrous_cj_mask.o
calc_atrous_cj.o : calc_atrous_cj.f calcmedian.o
calc_atrous_cj_mask.o : calc_atrous_cj_mask.f calcmedian.o
get_atrous_h.o : get_atrous_h.f
call_threshold.o : call_threshold.f readarray.o writearray.o callfdr.o get_keyword.o put_keyword.o
fn_radius.o : fn_radius.f sub_prog.o readarray.o
convfft.o : convfft.f readarray.o writearray.o rlft3.o
makeshapes.o : makeshapes.f writearray.o
fftshift.o : fftshift.f readarray.o writearray.o
callfftw.o : callfftw.f writearray.o
write_gaulsrl.o : write_gaulsrl.f
read_catalogue.o : read_catalogue.f
reconv.o : reconv.f
bdsm_writeofname.o : bdsm_writeofname.f
call_wcs_c2p.o : call_wcs_c2p.f wcs_struct.o wcs_xy2radec.o
writemenu_plotasrl.o : writemenu_plotasrl.f

clean :
	rm -fr *.o sm bdsm noise simulation or_gsm analyse_island

# DO NOT DELETE


