PROJECT (LOPES-Tools)

INCLUDE_DIRECTORIES (
	.
	../..
	${Blitz_INCLUDE_DIR}
)

ADD_EXECUTABLE (tDynamicSpectrum tDynamicSpectrum.cc)
TARGET_LINK_LIBRARIES (tDynamicSpectrum
		       lopestools
		       ${Blitz_LIBRARY}
		       ${CFITSIO_LIBRARY})

ADD_EXECUTABLE (tUnitPrefix tUnitPrefix.cc)
TARGET_LINK_LIBRARIES (tUnitPrefix
		       lopestools
		       ${Blitz_LIBRARY}
		       ${CFITSIO_LIBRARY})
