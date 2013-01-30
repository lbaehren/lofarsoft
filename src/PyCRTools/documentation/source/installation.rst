.. _installation:

************
Installation
************

This chapter describes how to install the PyCRTools on the following operating systems:

* :ref:`installation_mac`
* :ref:`installation_ubuntu`

  * :ref:`installation_ubuntu_12`
  * :ref:`installation_ubuntu_10`

.. _installation_mac:

Installation on Mac OS X 10.6
=============================

Introduction
------------

The following are instructions for installing the PyCRTools from
source on Mac OS X 10.6 commonly known as Snow Leopard. While the
system kernel itself does not run in 64 bit mode by default, all
libraries are compiled 64 bit and the compiler is set to 64 bit
architecture by default. Therefore these instructions also compile all
the USG software in 64 bit, this may or may not give a performance
improvement.

We assume a clean system (factory default install of Snow Leopard and
nothing else), if this is not the case and you run into architecture
mismatch problems please make sure the compiler can find the right
libraries.


Prerequisites
-------------

Before installing the USG software you need to install some other
tools and libraries.

Warning Do not use fink or macports to install the prerequisites, this
is known to give architecture mismatch problems during linking!


C/C++ compiler and tools
^^^^^^^^^^^^^^^^^^^^^^^^

First install the GNU C/C++ compiler optimized for OSX provided by
Apple as part of the X Code Integrated Development Environment.

You can either use the OSX Installation DVD which came with your mac
or `download <http://developer.apple.com/technologies/xcode.html>`_ it
from the Apple website.

Double click the package and follow the installation
instructions. Then run Software Update from the Apple menu to ensure
you have the latest version of all libraries.


Fortran compiler
^^^^^^^^^^^^^^^^

Install the 64 bit optimized version of the GNU fortran compiler from
the `High Performance Computing <http://hpc.sourceforge.net/>`_
project. Make sure you select the gfortran only binaries which can be
found `here
<http://prdownloads.sourceforge.net/hpc/gfortran-snwleo-intel-bin.tar.gz?download>`_.

Then (if your browser didn't do so already)::

  gunzip gcc-bin.tar.gz

and::

  sudo tar -xvf gcc-bin.tar -C /.

It installs everything in ``/usr/local``.  You can test if this worked by typing::

  gfortran


CMake
^^^^^

CR-Tools uses the CMake build system. This tool needs to be installed
separately. Simply download the binary from `here
<http://www.cmake.org/files/v2.8/cmake-2.8.1-Darwin-universal.dmg>`_,
double click to mount the disk image and follow the installation
instructions.


Python
^^^^^^

There are three ways to install Python. You may use the `Enthought
Python Distribution <http://www.enthought.com/products/epd.php>`_
binary installer (EPD 6.3 64 bit) DMG (recommended), fink or you may
compile Python and all required packages from source manually.


64 bit installation using Fink
""""""""""""""""""""""""""""""

First download the source code from `here <http://downloads.sourceforge.net/fink/fink-0.29.21.tar.gz>`_.

Unpack the tar.gz archive if this hasn't been done automatically,
e.g. via::

  tar -xvzf fink-0.29.21.tar.gz

if it has already been partially unpacked, in a terminal window. Then,
in a terminal window, change to the resulting ``fink-0.29.21`` directory,
and use::

  ./bootstrap

to start the boostrapping operation, which will install the Fink base
setup. When you get the following question: *“Your hardware is a
64bit-compatible intel processor, so you have the option of running
Fink in 64bit-only mode. This is not recommended for most users, since
many more packages are available for the default mode (which is mostly
32bit but includes some 64bit packages). Which mode would you like to
use?"*::

  (1)     Default (mostly 32bit)
  (2)     64bit-only

Be sure to select option 2 to build the 64 bit version::

  Choose a mode: [1] 2

After the installation is completed, running the command::

  /sw/bin/pathsetup.sh

will set up your environment for Fink (assuming you have installed
Fink under /sw). If you open a new terminal window, the session will
use these environment settings. Once you have installed fink and the
other base packages, the command sequences::

  fink selfupdate-rsync
  fink index -f

If you have not done so during the bootstrap, switch your fink
installation to use the unstable tree by running::

  fink configure

and select the appropriate option. Then run the following commands to
update the index and rebuild all packages::

  fink selfupdate
  fink selfupdate-rsync
  fink index -f
  fink scanpackages

Finally install the required packages using the following command::

  fink install numpy-py26 scipy-py26 matplotlib-py26 pyfits-py26 ipython-py26

Unfortunately Fink also automatically installs a basic python 2.7
which does not work with the current version of boost
python. Therefore remove this installation using::

  sudo fink remove -r python27 python27-shlib

Also make sure that python points to the default python by creating
the correct symlink::

  sudo ln -s /sw/bin/python2.6 /sw/bin/python

And check if your path is set correctly::

  which python

should give you::

  /sw/bin/python


Manually compile from source
""""""""""""""""""""""""""""

If you want to build the Python Pypeline you will need to install the
latest 2.6 series version of Python. Unfortunately the binaries for
OSX Snow Leopard are compiled as 32 bit. So we need to download the
source code and compile/install it ourselves. Download the source code
from `here
<http://www.python.org/ftp/python/2.6.5/Python-2.6.5.tgz>`_. Then
extract the tarball with::

  tar xvf Python-2.6.5.tgz

The compilation steps are a bit non standard. First we need to build
Python as a framework and tell the compiler to compile both the 32 and
64 bit versions::

  ./configure --enable-framework MACOSX_DEPLOYMENT_TARGET=10.6 --with-universal-archs=intel --enable-universalsdk=/Developer/SDKs/MacOSX10.6.sdk

To compile run::

  make

or to compile with debug symbols::

  make OPT=-g

and finally install::

  sudo make install

Then we need to make sure the 64 bit binary is loaded by default. Open
the python bin directory
``/Library/Frameworks/Python.framework/Versions/2.6/bin/``. In this
directory, you'll find both 32-bit and 64-bit intel binary symbolic
links. Relink the 32-bit with the 64-bit binaries::

  sudo ln -sf python2.6-64 python
  sudo ln -sf python2.6-64 python2.6
  sudo ln -sf pythonw2.6-64 pythonw
  sudo ln -sf python2.6-64 pythonw2.6

You can test if this worked by starting python and printing the
maximum value of an int using::

  import sys
  print sys.maxint

If you are running 64-bit, it will be 9223372036854775807, for 32 bit
it will print 2147483647. If the latter is the case please check what
went wrong before continuing.


Numpy
^^^^^

Download the source code for the latest stable version of numpy `here
<http://sourceforge.net/projects/numpy/files/NumPy/1.4.1/numpy-1.4.1.tar.gz/download>`_. Then
extract and install using::

  tar xvf numpy-1.4.1.tar.gz
  cd numpy-1.4.1
  sudo python setup.py install


Scipy
^^^^^

Download the source code for the latest stable version of scipy `here
<http://sourceforge.net/projects/scipy/files/scipy/0.7.2/scipy-0.7.2.tar.gz/download>`_. Extract
the tar-archive::

  tar xvf scipy-0.7.2.tar.gz

Build and install Scipy::

  cd scipy-0.7.2
  sudo python setup.py install


Matplotlib
^^^^^^^^^^

To compile a 64bit version of matplotlib we need the latest SVN
checkout of the matplotlib source code::

  svn co https://matplotlib.svn.sourceforge.net/svnroot/matplotlib/trunk/matplotlib matplotlib

(it has been known to work with revision 8337). Then we need to compile and install matplotlib::

  cd matplotlib
  PREFIX=/usr/local sudo make -f make.osx fetch deps mpl_build mpl_install
  sudo python setup.py install

You can test if this worked by starting python and plotting an image as follows::

  python
  >>> import matplotlib.pyplot as plt
  >>> import numpy as np
  >>> x=np.linspace(0,2*np.pi,100)
  >>> y=np.sin(x)
  >>> plt.plot(x,y)
  [<matplotlib.lines.Line2D object at 0x1021d9bd0>]

at this point you should have an image of a sine on your screen.


Installing the PyCRTools.
-------------------------

.. include:: installation_pycrtools_build.rst

.. note::

   .bashrc is not loaded by default on Mac OS X. Either .bash_profile
   or .profile is loaded by default, but only one of these. Therefore,
   make sure you have only one of the two.

Grab a good lunch and if all is well, the build of PyCRTools and its
dependencies completes in about an hour.


Verification
------------

Afterwards, check if Numpy has been built by the build process. This
is unwanted as we (normally) use Numpy from EPD. To prevent version
conflicts (especially a segfault on importing PyCRTools), remove it::

  cd $LOFARSOFT/release/lib64/python
  rm -rf numpy
  rm numpy*

Test by running::

  python
  import pycrtools



.. _installation_ubuntu:


Installation on Ubuntu
======================

.. _installation_ubuntu_12:

Installation on Ubuntu 12.04
----------------------------

The following shows you how to install PyCRTools on Ubuntu 12.04
(Precise Pangolin) for 64 bit systems.

Prerequisites
^^^^^^^^^^^^^

The following Ubuntu packages need to be installed:

* g++
* gfortran
* subversion
* python-dev
* zlib1g-dev
* flex
* libatlas-base-dev
* liblapack-dev
* swig
* bison
* libncurses5-dev
* libfreetype6-dev
* libpng12-dev
* python-tk
* python-pyfits
* tk8.5-dev
* fftw3-dev
* git
* valgrind

This can be done by using following commands::

  sudo apt-get install g++ gfortran flex swig bison subversion \
                       zlib1g-dev libatlas-base-dev liblapack-dev \
                       libncurses5-dev libfreetype6-dev libpng12-dev \
                       python-dev python-tk python-pyfits tk8.5-dev fftw3-dev \
                       libbz2-dev libghc-readline-dev \
                       git git git-core git-doc git-man git-svn \
                       valgrind


Additionally you can install the following packages directly from the
Ubuntu repository to reduce the amount of packages that need to be
build during the build process of the PyCRTools::

  sudo apt-get install libboost1.48-all-dev \
                       wcslib-dev \
                       cmake cmake-doc cmake-curses-gui \
                       libgsl0-dev \
                       python-matplotlib \
                       python-sphinx \
                       libcfitsio3-dev \
                       python-numpy \
                       num-utils \
                       python-scipy \
                       libblas-dev \
                       python-sip-dev \
                       openmpi-bin openmpi-common \
                       ipython



Building the Pycrtools
^^^^^^^^^^^^^^^^^^^^^^

.. include:: installation_pycrtools_build.rst

Grab a cup of coffee and if all is well, in about 15 minutes you
should have your very own working installation of the PyCRTools.


.. _installation_ubuntu_10:

Installation on Ubuntu 10.04 and 10.10
--------------------------------------

The following shows you how to install PyCRTools on Ubuntu 10.04 (Lucid
Lynx) and Ubuntu 10.10 (Maverick Meerkat) in either 32 or 64 bit.

Prerequisites
^^^^^^^^^^^^^

The following Ubuntu packages need to be installed:

* g++
* gfortran
* subversion
* python-dev
* zlib1g-dev
* flex
* libatlas-base-dev
* liblapack-dev
* swig
* bison
* libncurses5-dev
* libfreetype6-dev
* libpng12-dev
* python-tk
* python-pyfits
* tk8.5-dev
* fftw3-dev

For this you can just use the package manager::

  sudo apt-get install g++ gfortran flex swig bison subversion \
                       zlib1g-dev libatlas-base-dev liblapack-dev \
                       libncurses5-dev libfreetype6-dev libpng12-dev \
                       python-dev python-tk python-pyfits tk8.5-dev fftw3-dev


Building the PyCRTools
^^^^^^^^^^^^^^^^^^^^^^

.. note::

  **Building PyCRTools on CEP1**

  Before building the :mod:`PyCRTools` on CEP1, you need to make the following
  adjustments to the build settings:

  * Change the DAL build environment such that it uses the same HDF5 libraries as the rest of the LUS;
  * Force the build of :mod:`matplotlib`, :mod:`numpy` and :mod:`pyfits` since the available version of :mod:`matplotlib` at CEP is not compatible with :mod:`PyCRTools`;
  * Since this version of :mod:`matplotlib` is not compatible with the available :mod:`python-tk`, force :mod:`pyplot` to use :mod:`pyqt4` instead by changing the ``matplotlibrc``;
  * In order to be able to use :mod:`pyrap` and :mod:`PyCRTools` at the same time, give the default location of the casacore library (``/opt/cep/casacore``) by hand, again both for LUS and DAL separately, since the DAL has its own cmake environment.


.. include:: installation_pycrtools_build.rst

Grab a cup of coffee and if all is well, in about 30 minutes you
should have your very own working installation of the PyCRTools.


