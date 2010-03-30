###################################
LOFAR Pipeline System Documentation
###################################

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

Pipeline Specific Documenation
==============================

.. toctree::
   :maxdepth: 2

   pipelines/sip/index.rst

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

