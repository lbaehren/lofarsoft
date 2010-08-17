
bin_SCRIPTS = psrdada_ldflags psrdada_cflags

libs: fix_prefix

install-exec-hook: fix_prefix

#
# The following lines ensure that the psrdada_cflags and psrdada_ldflags
# scripts are up-to-date with respect to where the software was installed.
#

FIX_PREFIX := ${prefix}/bin/psrdada_ldflags ${prefix}/bin/psrdada_cflags

fix_prefix: install-binSCRIPTS
	perl -p -i -e "s|PREFIX|${prefix}|" ${FIX_PREFIX}

