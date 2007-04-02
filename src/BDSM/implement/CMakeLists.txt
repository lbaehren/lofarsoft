PROJECT (BDSM Fortran)

## ------------------------------------------------------------------------------
## Find external libraries

if (NOT HAVE_CFITSIO)
  include (../../../devel_common/cmake/FindCFITSIO.cmake)
endif (NOT HAVE_CFITSIO)

include (../../../devel_common/cmake/FindCPGPLOT.cmake)

## ------------------------------------------------------------------------------
## Build libraries

ADD_LIBRARY (bdsm_nr
	covsrt.f
	erf.f
	fgauss.f
	fgauss2d.f
	fgauss2de.f
	func2.f
	gammln.f
	gammp.f
	gasdev.f
	gaussj.f
	gcf.f
	gser.f
	mrqcof.f
	mrqcof2d.f
	mrqcof2dm.f
	mrqmin.f
	mrqmin2d.f
	mrqmin2dm.f
	poly2.f
	poly3.f
	ran1.f
	sort3.f
	splint.f
	spline.f
	splie2.f
	splin2.f
)

ADD_LIBRARY (bdsm_cr8
	cr8constantim.f
	cr8modulationimage.f
	cr8noisemap.f
	cr8sky.f
	cr8sky_ilse.f
	cr8sources.f
	sim_cr8noise.f
	sub_cr8sky.f
#	sub_prog.o
#	ran1.o
#	get_subim_xsrc.o
#	sourcemeasure.o
#	xy2radec.o
#	fit_island.o
#	sub_cr8sky.o
#	writearray.o
#	conv2.o
#	conv2_get_wid_fac.o
#	writefitshead.o
#	sigclip.o
#	put_keyword.o
)

ADD_LIBRARY (bdsm_experiment
	arrstat.f
	basicgrey.f
	experiment.f
	fourn.f
	get_keyword.f
	getsurr_8pix.f
	mulsrc.f
	pgsub.f
	plotisland.f
	rlft3.f
	readhead_srclist.f
	readarray.f
	sigclip.f
	sourcelistheaders.f
	sub_prog.f
)

ADD_LIBRARY (bdsm_call
	callfitgaussian.f
	callgaul2srl.f
	callgetpixel.f
	callmoment.f
	callputpixel.f
	callreadfits.f
	callrmsd.f
	calltest.f
	callwritegaulfits.f
	callwritefits.f
	callwritesrlfits.f
#	readhead_srclist.o
#	sourcelistheaders.o
#	gaul_to_source.o
#	sub_calc_para_source.o
#	sourcelistheaders.o
#	sub_prog.o
#	printerror.o
#	deletefile.o
#	get_keyword.o
#	getininp.o
#	readininp.o
#	testmomentanal.o
)

## ------------------------------------------------------------------------------
## Build the executables

ADD_EXECUTABLE (srccomp1
	srccomp1.f
	makelists1.f
	sub_prog.f
	sourcelistheaders.f
	readhead_srclist.f
)

ADD_EXECUTABLE (simulation
	arrstat.f
	check.f
	conv2_get_wid_fac.f
	covsrt.f
	fitinparts.f
	get_fitskeywords.f
	get_keyword.f
	get_mosaicpara.f
	get_subim_dubsrc.f
	get_subim_ptsrc.f
	get_subim_xsrc.f
	set_varypsf.f
	getcdf.f
	getsurr_8pix.f
	primarybeam3D_many.f
	readarray.f
	readhead_srclist.f
	read_simparadefine.f
	sigclip.f
	simulation.f
	sourcelistheaders.f
	sub_simulation.f
	sub_prog.f
	subim3D.f
	sim_addnoisrc.f
	writearray.f
	writefits3D.f
)
TARGET_LINK_LIBRARIES (simulation ${CFITSIO_LIBRARIES} bdsm_nr bdsm_cr8)

ADD_EXECUTABLE (simulation_ilse
	arrstat.f
	check.f
	conv2_get_wid_fac.f
	covsrt.f
	cr8sky_ilse.f
	fgauss.f
	fitinparts.f
	get_keyword.f
	get_fitskeywords.f
	get_mosaicpara.f
	get_subim_dubsrc.f
	get_subim_ptsrc.f
	get_subim_xsrc.f
	getcdf.f
	getsurr_8pix.f
	primarybeam3D_many.f
	read_simparadefine.f
	readarray.f
	readhead_srclist.f
	set_varypsf.f
	sigclip.f
	sim_addnoisrc_ilse.f
	sim_cr8noise.f
	simulation_ilse.f
	sourcelistheaders.f
	sub_cr8sky.f
	sub_prog.f
	sub_simulation.f
	subim3D.f
	writearray.f
	writefits3D.f
)
TARGET_LINK_LIBRARIES (simulation_ilse ${CFITSIO_LIBRARIES} bdsm_nr)

ADD_EXECUTABLE (noise
	acf.f
	arrstat.f
	arrstatmask.f
	associatesrl.f
	basicgrey.f
	bdsm.f
	bdsm_boxnoiseimage.f
	boxf.f
	boxnoiseimage.f
	calcmedian.f
	check.f
	clip.f
	combineimage.f
	conv2.f
	conv2_get_gaus.f
	conv2_get_wid_fac.f
	convolveimage.f
	copyheader.f
	covsrt.f
	deconv.f
	deletefile.f
	detectdiscont.f
	displayim.f
	displaytwoim.f
	fgauss2de.f
	fgauss2dne.f
	fit_island.f
	fit_mulgauss.f
	fitinparts.f
	formisland_rank.f
	formislands.f
	fourn.f
	gaussj.f
	fileformat.f
	filling.f
	fitcdf.f
	gaul_to_source.f
	get_gaus1_iniparm.f
	get_keyword.f
	get_numpix_val.f
	get_rmsmap.f
	get_subim_xsrc.f
	getcdf.f
	getco_ringwidth.f
	getininp.f
	getlistnsigpt.f
	getsurr_8pix.f
	iland_mat2list.f
	imagestats.f
	imstat.f
	islandedit.f
	make_associatelist1.f
	make_mask_gausid.f
	make_src_residim.f
	medianfilter.f
	modulate.f
	mulsrc.f
	nbit.f
	noise.f
	nsigmacut.f
	operateimage.f
	plot_associatelist1.f
	plotisland.f
	pgsub.f
	populateinparts.f
	populatesrcs.f
	preprocess.f
	primarybeam.f
	printerror.f
	put_keyword.f
	puthead.f
	putpixel.f
	read_head_coord.f
	read_paradefine.f
	readarray.f
	readfitshead.f
	readgaul.f
	readhead_srclist.f
	readheader.f
	readinimage.f
	readininp.f
	selectnr.f
	sigclip.f
	slice.f
	soln_quality.f
	sort2_4.f
	sourcelistheaders.f
	sourcemeasure.f
	splintmod.f
	structfn.f
	sub_calc_para_source.f
	sub_prog.f
	sub_readislpix.f
	sub_sourcemeasure_getblctrc.f
	subim.f
	testmomentanal.f
	transposeim.f
	write_associatelist1.f
	writeaipsfiles.f
	writearray.f
	writefits.f
	writefitshead.f
	writeheader.f
	xy2radec.f
# -- for some reason these files need to be included here; check "bdsm_nr"
	gasdev.f
	mrqmin2d.f
)
TARGET_LINK_LIBRARIES (noise ${CFITSIO_LIBRARIES} bdsm_nr bdsm_cr8 bdsm_experiment bdsm_call)

ADD_EXECUTABLE (bdsm-awe
	bdsm-awe.f
# -- rest is just like for bdsm
	acf.f
	arrstat.f
	arrstatmask.f
	associatesrl.f
	basicgrey.f
	bdsm.f
	bdsm_boxnoiseimage.f
	boxf.f
	boxnoiseimage.f
	calcmedian.f
	check.f
	clip.f
	combineimage.f
	conv2.f
	conv2_get_gaus.f
	conv2_get_wid_fac.f
	convolveimage.f
	copyheader.f
	covsrt.f
	deconv.f
	deletefile.f
	detectdiscont.f
	displayim.f
	displaytwoim.f
	fgauss2de.f
	fgauss2dne.f
	fit_island.f
	fit_mulgauss.f
	fitinparts.f
	formisland_rank.f
	formislands.f
	fourn.f
	gaussj.f
	fileformat.f
	filling.f
	fitcdf.f
	gaul_to_source.f
	get_gaus1_iniparm.f
	get_keyword.f
	get_numpix_val.f
	get_rmsmap.f
	get_subim_xsrc.f
	getcdf.f
	getco_ringwidth.f
	getininp.f
	getlistnsigpt.f
	getsurr_8pix.f
	iland_mat2list.f
	imagestats.f
	imstat.f
	islandedit.f
	make_associatelist1.f
	make_mask_gausid.f
	make_src_residim.f
	medianfilter.f
	modulate.f
	mulsrc.f
	nbit.f
	nsigmacut.f
	operateimage.f
	plot_associatelist1.f
	plotisland.f
	pgsub.f
	populateinparts.f
	populatesrcs.f
	preprocess.f
	primarybeam.f
	printerror.f
	put_keyword.f
	puthead.f
	putpixel.f
	read_head_coord.f
	read_paradefine.f
	readarray.f
	readfitshead.f
	readgaul.f
	readhead_srclist.f
	readheader.f
	readinimage.f
	readininp.f
	selectnr.f
	sigclip.f
	slice.f
	soln_quality.f
	sort2_4.f
	sourcelistheaders.f
	sourcemeasure.f
	splintmod.f
	structfn.f
	sub_calc_para_source.f
	sub_prog.f
	sub_readislpix.f
	sub_sourcemeasure_getblctrc.f
	subim.f
	testmomentanal.f
	transposeim.f
	write_associatelist1.f
	writeaipsfiles.f
	writearray.f
	writefits.f
	writefitshead.f
	writeheader.f
	xy2radec.f
# -- for some reason these files need to be included here; check "bdsm_nr"
	gasdev.f
	mrqmin2d.f
)
TARGET_LINK_LIBRARIES (bdsm-awe ${CFITSIO_LIBRARIES} bdsm_nr bdsm_cr8 bdsm_experiment bdsm_call)


#ADD_EXECUTABLE (a
#	a.f
#	combineimage.f
#	readarray.f
#	writearray.f
#	check.f
#	get_keyword.f
#	sub_prog.f
#)
