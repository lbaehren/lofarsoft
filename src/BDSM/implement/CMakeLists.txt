PROJECT (BDSM Fortran)

#INCLUDE (../../../devel_common/cmake/FindCFITSIO.cmake)
#INCLUDE (../../../devel_common/cmake/FindCPGPLOT.cmake)

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
	cr8sky.f
	fgauss.f
	fitinparts.f
	func2.f
	gasdev.f
	gaussj.f
	get_fitskeywords.f
	get_keyword.f
	get_mosaicpara.f
	get_subim_dubsrc.f
	get_subim_ptsrc.f
	get_subim_xsrc.f
	set_varypsf.f
	getcdf.f
	getsurr_8pix.f
	mrqcof.f
	mrqmin.f
	poly2.f
	poly3.f
	primarybeam3D_many.f
	ran1.f
	readarray.f
	readhead_srclist.f
	read_simparadefine.f
	sigclip.f
	simulation.f
	sim_cr8noise.f
	sourcelistheaders.f
	sub_simulation.f
	sub_cr8sky.f
	sub_prog.f
	subim3D.f
	sim_addnoisrc.f
	writearray.f
	writefits3D.f
)
TARGET_LINK_LIBRARIES (simulation CFITSIO)
#$(FC) simulation.f $(OBJNOISE2) $(CFITS) -o simulation


## this no longer exists

#ADD_EXECUTABLE (a
#	a.f
#	combineimage.f
#	readarray.f
#	writearray.f
#	check.f
#	get_keyword.f
#	sub_prog.f
#)
