.. _developer-guide:

*********************
Developer information
*********************

This section describes the internal structure of the pipeline framework. It is
intended for developers who plan to work on the framework code itself.

.. _code-structure:

Structure of the code
=====================

The pipeline code can be obtained from `USG Subversion
<http://usg.lofar.org/svn/code/trunk/src/pipeline/>`_. There are five
top-level directories:

``deploy``
    IPython system deployment scripts. See the section on
    :ref:`parallelisation with IPython <parallelisation-ip>` for more details,
    but note that the use of IPython within the pipeline is *deprecated*.

``docs``
    Documentation and examples. See the section on :ref:`available
    documentation <documentation>` for details.

``mac``
    MAC/SAS EventPort interface code. See the :ref:`mac-interface` section for
    details.

``framework``
    The framework code itself. This is implemented as the Python module
    :mod:`lofarpipe`; see its documentation for details. A ``distutils`` based
    setup script, ``setup.py``, is included for easy installation: see the
    section on :ref:`installing the framework <framework-installation>`.

``recipes``
    A collection of :ref:`recipes <recipe-docs>`, intended to both demonstrate
    the operation of the framework and serve as useful pipeline components.
    Recipes intended for different pipelines may be stored in separate
    directories: for example, the ``sip`` directory contains recipes for
    useful to the :ref:`standard imaging pipeline <sip>`.

External components
===================

.. _ipython-deprecated:

IPython
-------

The IPython system was extensively used by earlier versions of this framework,
but is now *deprecated*. Both recipe and framework developers are urged to
avoid using it wherever possible. However, until all existing recipes
(including those not distributed with the framework) have been converted to
use another system, the IPython support in the framework should be maintained.
That includes:

* :class:`lofarpipe.support.clusterhandler.ClusterHandler`
* :func:`lofarpipe.support.clusterhandler.ipython_cluster`
* :class:`lofarpipe.support.ipython.LOFARTask`
* :class:`lofarpipe.support.ipython.IPythonRecipeMixIn`
* :func:`lofarpipe.support.utilities.build_available_list`
* :func:`lofarpipe.support.utilities.clear_available_list`
* :func:`lofarpipe.support.utilities.check_for_path`

Conversely, once all IPython-based recipes in active use have been replaced,
the IPython support code should be removed from the framework.

.. _documentation:

Available documentation
=======================

.. todo::

   Describe the available documentation in the docs directory: what the
   examples are, how to build the Sphinx documenation.

.. _mac-interface:

MAC/SAS interface
=================

.. todo::

   Describe current status of MAC/SAS interface.
