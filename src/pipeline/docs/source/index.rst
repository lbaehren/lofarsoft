###################################
LOFAR Pipeline System Documentation
###################################

.. toctree::
   :hidden:

   todo

This document provides an overview of the LOFAR pipeline system. This system
has largely been developed to support the LOFAR imaging pipeline, but is now
being deployed for a variety of science pipelines on the LOFAR cluster. This
document is split into three sections: :ref:`the first <section-overview>`
provides an overview of the framework, and describes how the user can start
building their own pipelines. :ref:`The second <section-developer-reference>`
describes the framework codebase in more detail, and is intended primarily for
those directly working on the framework, rather than simply developing
pipelines. :ref:`The third <section-pipeline-specific>` provides a guide to the
imaging pipeline itself.

.. ifconfig:: todo_include_todos

   This documentation is still a work in progress. See the :ref:`to-do list
   <todo>` for upcoming improvements.

.. _section-overview:

The LOFAR Pipeline Framework
============================

The framework aims to make it possible to manage a variety of different
processing steps in a flexible yet consistent way, while running them in
parallel across a cluster. The following chapters describe the various
components of the framework and how they fit together.

.. toctree::
   :maxdepth: 2

   framework/overview/index.rst
   framework/infrastructure/index.rst
   framework/recipe/index.rst
   framework/parallel/index.rst
   framework/dependencies/index.rst

.. _section-developer-reference:

Developer's Reference
=====================

.. toctree::
   :maxdepth: 2

   developer/index.rst

.. _section-pipeline-specific:

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
