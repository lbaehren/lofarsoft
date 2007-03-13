
## 'prefix' used for the installed software; this will be used along with the '--prefix' parameter
## option during the configuration of the individual packages
## Use this to adjust the reference point of your software installation

var_prefix=`pwd`

## External software required for AIPS++/CASA -----------------------------------

atlas:
	./atlas.install $(var_prefix)

bison:
	./bison.install $(var_prefix)

boost:
	./boost.install $(var_prefix)

fftw:
	./fftw.install $(var_prefix)

cfitsio:
	./cfitsio.install $(var_prefix)

ccfits: cfitsio
	./ccfits.install $(var_prefix)

lapack:
	./lapack.install $(var_prefix)

m4:
	./m4.install $(var_prefix)

rivet:
	./rivet.install $(var_prefix)

pgplot:
	./pgplot.install $(var_prefix)

python:
	./python.install $(var_prefix)

qt:
	./qt.install $(var_prefix)

rpfits:
	./rpfits.install $(var_prefix)

tcltk:
	./tcltk.install $(var_prefix)

all: tcltk ccfits fftw boost atlas

## Code management ------------------------------------------------------------------

installation_dirs=bin include info lib man share
installation_archive=casa-local.tgz

## 'update' will fetch the latest version of the installation scripts from the download
## source and put them into the current directory

update:
	svn update

tar:
	tar -cvzf $(installation_archive) *.install *.patch makefile

## 'clean' will remove backup files from e.g. (x)Emaccs and any still remaining archives from
## earlier downloads (e.g. from incomplete installations)

clean:
	rm -f *~ *.tgz *.gz *.zip

instclean: clean
	rm -rf $(installation_dirs)

distclean: clean
	rm -rf *.install *.patch

cleanall: instclean distclean

show:
	-@ echo "Prefix  = $(var_prefix)"
