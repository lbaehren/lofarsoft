.. _startup:

Starting up
===========

To work with the PyCRTools package it first has to be loaded. This can
be done by starting Python or iPython and type the following at the
prompt (``>>>``)::

   >>> import pycrtools as cr

which makes all PyCRTools functionality available under the namespace
:mod:`cr`. If you don't want to type the :mod:`cr` prefix for each
PyCRTools function, and have all functionality available in the
default namespace, type::

   >>> from pycrtools import *

.. note::

   The latter approach is not recommended as all PyCRTools
   functions get cluttered up in the current namespace possibly
   overwriting already existing functions.

.. note::

   The previous note is only true if you are a computer specialist who cares
   about code purity. However, if you are a scientist, who wants to
   use the package interactively and you are interested in getting
   things done quickly, don't hesitate to do it the easy way ...


For interactive use ipython is an alternative.  Before the first use
copy the respective profile file into your home directory::

   mkdir ~/.ipython/
   cp $LOFARSOFT/src/PyCRTools/extras/ipython_config_pycr.py   ~/.ipython/

then start ipython (if you installed it)::

   ipython -p pycr

To make life easy you can (should) also define some aliases, e.g., in
your .profile or .bashrc file.::

   alias ipycr="ipython -p pycr"
   alias pycr='python -i $LOFARSOFT/src/PyCRTools/scripts/pycrinit.py'

and then start with ``pycr`` or ``ipypcr``.

