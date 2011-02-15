###################################
LOFAR Pipeline System Documentation
###################################

.. toctree::
   :hidden:

   todo

This document provides an overview of the LOFAR pipeline system. This system
has largely been developed to support the LOFAR imaging pipeline, but is now
being deployed for a variety of science pipelines on the LOFAR cluster. This
document is split into a number of sections: :ref:`the first
<section-overview>` describes the aims of the framework, the structure of a
pipeline, and gives an overview of how the system fits together. :ref:`The
second <section-user-guide>` provides details on how to run pre-defined
pipelines. :ref:`The third <section-author-guide>` provides a tutorial
introduction to writing pipelines and pipeline components. :ref:`The fourth
<section-developer-reference>` describes the framework codebase in more
detail, and is intended for pipeline authors who wish to dig a little deeper,
as well as those interested in developing the framework itself. :ref:`The
final section <section-pipeline-specific>` provides a guide to the imaging
pipeline itself.

.. ifconfig:: todo_include_todos

   This documentation is still a work in progress. See the :ref:`to-do list
   <todo>` for upcoming improvements.

.. _section-overview:

The pipeline system was developed by John Swinbank (University of Amsterdam)
in 2009 & 2010. Since 2011, the primary maintainer is Marcel Loose (ASTRON).

Overview & Getting Started
==========================

.. toctree::
   :maxdepth: 2

   overview/overview/index.rst
   overview/dependencies/index.rst

.. _section-user-guide:

User's Guide
============

.. toctree::
   :maxdepth: 2

   user/installation/index.rst
   user/usage/index.rst

.. _section-author-guide:

Recipe & Pipeline Author's Guide
================================

.. toctree::
   :maxdepth: 2

   author/index.rst

.. _section-developer-reference:

Developer's Reference
=====================

.. toctree::
   :maxdepth: 2

   developer/index.rst
   developer/lofarpipe.rst
   developer/todo.rst

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
