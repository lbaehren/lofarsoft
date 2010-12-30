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
describes the aims of the framework, the structure of a pipeline, and gives an
overview of how the system fits together, as well as information on how to run
pre-defined pipelines. :ref:`The second <section-author-guide>` covers how
users can use the system to design their own pipelines or pipeline components.
:ref:`The third <section-developer-reference>` describes the framework
codebase in more detail, and is intended primarily for those directly working
on the framework, rather than simply developing pipelines. :ref:`The fourth
<section-pipeline-specific>` provides a guide to the imaging pipeline itself.

.. ifconfig:: todo_include_todos

   This documentation is still a work in progress. See the :ref:`to-do list
   <todo>` for upcoming improvements.

.. _section-overview:

Overview & Getting Started
==========================

.. toctree::
   :maxdepth: 2

   overview/overview/index.rst
   overview/infrastructure/index.rst
   overview/dependencies/index.rst

.. _section-author-guide:

Recipe & Pipeline Author's Guide
================================

.. toctree::
   :maxdepth: 2

   author/recipe/index.rst
   author/parallel/index.rst

.. _section-developer-reference:

Developer's Reference
=====================

.. toctree::
   :maxdepth: 2

   developer/index.rst
   developer/lofarpipe.rst

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
