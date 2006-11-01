PROJECT (BDSM)

INCLUDE (${BDSM_SOURCE_DIR}/../../devel_common/cmake/FindCFITSIO.cmake)
INCLUDE (${BDSM_SOURCE_DIR}/../../devel_common/cmake/FindCPGPLOT.cmake)

BUILD_COMMAND (BUILD_BDSM "g77 -fbounds-check")

ADD_EXECUTABLE (srccomp1
	srccomp1.f
	makelists1.f
	sub_prog.f
)
#	g77 -fbounds-check srccomp1.f $(OBJNOISE3)   -o srccomp1
