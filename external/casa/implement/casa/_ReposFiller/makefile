# $Id: makefile,v 1.1 2004/08/24 20:14:37 gvandiep Exp $

# Use the generic AIPS++ class implementation makefile.
#------------------------------------------------------
include $(word 1, $(AIPSPATH))/code/install/makefile.imp

# Set up a parallel make if PARALLEL_MAKE is defined in makedefs.
ifdef PARALLEL_MAKE
  MAKE := $(PARALLEL_MAKE)
endif
