PROJECT (VLT)

SUBDIRS (test)

INCLUDE_DIRECTORIES (. .. ${VLT_SOURCE_DIR}/implement)

ADD_LIBRARY (aomath
	aomath.cc
	svd.cc
	vector.cc
	matrix.cc
	mathBase.cc
	memory.cc
	MAFilter.cc
	MAStdFilter.cc
)
