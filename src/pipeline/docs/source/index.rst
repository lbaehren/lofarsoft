.. LOFAR Standard Imaging Pipeline documentation master file, created by sphinx-quickstart on Wed Jun 10 17:09:31 2009.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

LOFAR Standard Imaging Pipeline documentation
=============================================

This document provides a brief overview of the so-called "pipeline framework"
developed for supporting LOFAR pipelines, and describes how it has been used
to assemble a prototype "standard imaging pipeline".

The framework aims to make it possible to manage a variety of different
processing steps in a flexible yet consistent way, while running them in
parallel across the LOFAR offline cluster.

The standard imaging pipeline will accept raw data from the LOFAR correlator,
pre-process it, calibrate it, image it, and update the sky model with the
sources detected in the data.

.. toctree::
   :maxdepth: 2

   framework/index.rst
   recipe/index.rst
   parallel/index.rst
   pipeline/index.rst
   sip/index.rst

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

