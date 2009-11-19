# This will install .py files into $(bindir) with their execute bit set.
bin_SCRIPTS	= dppp_node.py \
		  DPPP.py \
		  MakeVDS.py \
		  MWImager.py \
		  StandardImagingPipeline.py

# This will install .py, .pyc, and .pyo files into the site-packages directory.
pypkgdir	= $(pythondir)/lofar/pipeline
pypkg_PYTHON	= __init__.py \
		  Observation.py \
		  sysconfig.py

include $(top_srcdir)/Makefile.common
