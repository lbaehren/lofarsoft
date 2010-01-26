
aclocaldir = $(datadir)/aclocal

dist_aclocal_DATA = \
	debug.m4 local.m4 compiler.m4 package.m4 \
	x11.m4 pgplot.m4 cfitsio.m4 etr_socket_nsl.m4 getopt_long.m4 mpi.m4 \
	Makefile.root Makefile.local Makefile.include install-intermediate

nodist_aclocal_DATA = Makefile.aclocal

EXTRA_DIST = aclocal.sh
CLEANFILES = Makefile.aclocal

#############################################################################
#
# This Makefile smooths over various bugs and bumps and other features
# that break the other GNU autotools-generated Makefiles
#

all-local: 17-04-2009

libs:

#############################################################################
#
# Rule to generate Makefile.aclocal
#

Makefile.aclocal: $(srcdir)/aclocal.sh
	$(SHELL) $(srcdir)/aclocal.sh $(dist_aclocal_DATA) > Makefile.aclocal


#############################################################################
#
# 17 April 2009 - straten
#
# The packages/ sub-directory contains scripts that can be used to help
# install third-party packages.  By making it independently configurable,
# it can be used before psrchive is configured and compiled.
#
# This turns out to be a bit of a headache, as the automake-generated
# rules that re-run the configure script do not perform the required
# nested configure.  (running ./configure does the required recursion.)
#
# This rule looks out for this problem and runs the nested configure,
# producing the Makefile required for "make dist" to work.

17-04-2009:
	@if test ! -f $(top_builddir)/packages/Makefile; then \
	  cd $(top_builddir)/packages && $(top_srcdir)/packages/configure; \
	fi

