PROJECT (BDSM Fortran)

#INCLUDE (../../../devel_common/cmake/FindCFITSIO.cmake)
#INCLUDE (../../../devel_common/cmake/FindCPGPLOT.cmake)

ADD_EXECUTABLE (srccomp1
	srccomp1.f
	makelists1.f
	sub_prog.f
	sourcelistheaders.f
	readhead_srclist.f
)
#	g77 -fbounds-check srccomp1.f $(OBJNOISE3)   -o srccomp1
