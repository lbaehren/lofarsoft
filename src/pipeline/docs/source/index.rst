#############################
LOFAR Pipelines Documentation
#############################

This document provides a brief overview of the so-called "pipeline framework"
developed for supporting LOFAR pipelines, and describes how it has been used
to assemble a prototype "standard imaging pipeline".


The LOFAR Pipeline Framework
============================

The framework aims to make it possible to manage a variety of different
processing steps in a flexible yet consistent way, while running them in
parallel across the LOFAR offline cluster. The following chapters describe the
various components of the framework and how they fit together.

.. toctree::
   :maxdepth: 2

   framework/overview/index.rst
   framework/recipe/index.rst
   framework/parallel/index.rst
   framework/infrastructure/index.rst
   framework/dependencies/index.rst

The Standard Imaging Pipeline
=============================

The Standard Imaging Pipeline (or SIP) will accept raw data from the LOFAR
correlator, pre-process it, calibrate it, image it, and update the sky model
with the sources detected in the data. This section describes the components
of the SIP and how they fit together.

.. toctree::
   :maxdepth: 2

   sip/quickstart/index.rst
   sip/recipes/index.rst


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

