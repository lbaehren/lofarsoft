****************************
The :mod:`lofarpipe` package
****************************

.. module:: lofarpipe
   :synopsis: The framework package.

The :mod:`lofarpipe` package contains all the Python modules included with the
core framework code. This includes the :mod:`lofarpipe.cuisine` pacakge
(inherited from the :ref:`cuisine <notes-on-cuisine>` framework for WSRT), the
:mod:`lofarpipe.support` pacakge (containing all the pipeline framework code),
and the :mod:`lofarpipe.tests` package (which contains a limited selection of
test).

.. module:: lofarpipe.cuisine
   :synopsis: The Cuisine system.

Cuisine
=======

The LOFAR pipeline system developed partly from the `WSRT Cuisine
<http://www.astron.nl/~renting/pipeline_frame.html>`_, developed by Adriaan
Renting for use at the Westerbork Synthesis Radio Telescope. Many of the basic
concepts (the recipe, with associated inputs and outputs, for example)
originated in the Cuisine system, and the user is encouraged to refer to its
documentation where necessary.

A slightly modified version of the "original" Cuisine is distributed as part
of the :mod:`lofarpipe` package. The modifications include:

* Use of `new style
  <http://www.python.org/download/releases/2.2.3/descrintro/>`_ Python classes
  throughout.

* Reworked option handling code to use the ``optparse``
  module from the `Python standard library
  <http://docs.python.org/library/optparse.html>`_. (While this is an
  improvement on the original implementation, it would be desireable to
  produce a more "bulletproof" configuration handling system for the
  framework, which handles configuration files and command line options in a
  uniform way.)

* Reworked logging system using the ``logging`` module from the `Python
  standard library <http://docs.python.org/library/optparse.html>`_. This
  provides a flexible way of configuring logging formats and destinations,
  included logging to files or TCP sockets.

* Assorted bug-fixes and tweaks.

It is hoped that these changes will eventually be merged upstream.

Very little of the original Cuisine code is currently used in the LOFAR
framework, although the :class:`lofarpipe.cuisine.WSRTrecipe.WSRTrecipe` is
still used as the basis for all LOFAR recipes. The recipe author, however, is
*never* expected to directly interact with Cuisine code: all LOFAR pipeline
recipes inherit from :class:`lofarpipe.support.baserecipe.BaseRecipe`, which
entirely wraps all relevant Cuisine functionality. The
:mod:`lofarpipe.support` inheritance diagrams show exactly how these packages
are related. The following API documentation covers only those routines
directly used by the rest of the pipeline system, not Cuisine as a whole.

.. module:: lofarpipe.support.cuisine.WSRTrecipe
   :synopsis: Base module for all Cuisine recipe functionality.

:mod:`lofarpipe.support.cuisine.WSRTrecipe`
-------------------------------------------

.. autoclass:: lofarpipe.cuisine.WSRTrecipe.WSRTrecipe
   :members: help, main_init, main, run, go, main_result, cook_recipe

.. module:: lofarpipe.support.cuisine.cook
   :synopsis: Cuisine cooks.

:mod:`lofarpipe.support.cuisine.cook`
-------------------------------------------

.. autoclass:: lofarpipe.cuisine.cook.PipelineCook
   :members: copy_inputs, copy_outputs, spawn, try_running

.. todo::

   Note on WSRTrecipe functionality.

.. module:: lofarpipe.support
   :synopsis: Core framework code.

.. todo::

   lofarpipe.support documentation goes here.

.. module:: lofarpipe.tests
   :synopsis: Pipeline framework unit tests.

.. todo::

   lofarpipe.tests documentation goes here.
