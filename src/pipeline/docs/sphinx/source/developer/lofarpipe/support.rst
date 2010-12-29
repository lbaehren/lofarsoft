.. module:: lofarpipe.support
   :synopsis: Core framework code.

************************************
The :mod:`lofarpipe.support` package
************************************

This package contains effectively all the core framework code that comprises
the LOFAR pipeline system. Broadly speaking, it address four distinct problem
areas:

* :ref:`The construction of recipes <lofarpipe-recipes>`;
* :ref:`Checking of recipe inputs and outputs ("ingredients") <lofarpipe-ingredients>`;
* :ref:`Distribution of pipeline jobs <lofarpipe-remotecommand>`;
* :ref:`Utility and convenience functions <lofarpipe-utility>`.

.. _lofarpipe-recipes:

Recipes
=======

All LOFAR pipeline recipes are ultimately derived from
:class:`lofarpipe.cuisine.WSRTrecipe.WSRTrecipe`. However, all the
functionality it provides is encapsulated and enhanced by the
:class:`~lofarpipe.support.baserecipe.BaseRecipe` class, and it is from this
that all pipeline recipes should be derived. This class also includes the
:ref:`"ingredients" system <lofarpipe-ingredients>`, which controls recipe
inputs and outputs.

A number of "mix-in" classes may be added to
:class:`~lofarpipe.support.baserecipe.BaseRecipe` to provide additional
functionality, such as the ability to dispatch jobs to remote hosts
(:class:`~lofarpipe.support.remotecommand.RemoteCommandRecipeMixIn`). Recipe
authors may mix-in whatever functionality is required to achieve their aims.

The :class:`~lofarpipe.suppport.control.control` class provides a recipe with a
little extra functionality to help it act as an overall pipeline. This can
include interfacing with an external control system, for example, or keeping
track of the pipeline progress
(:class:`~lofarpipe.support.stateful.StatefulRecipe`).

The relationship between all these classes is illustrated below.

.. inheritance-diagram:: lofarpipe.support.control.control lofarpipe.support.lofarrecipe.LOFARrecipe
   :parts: 3

.. autoclass:: lofarpipe.support.baserecipe.BaseRecipe
   :members:

.. autoclass:: lofarpipe.support.stateful.StatefulRecipe

.. autoclass:: lofarpipe.support.control.control
   :members: pipeline_logic

.. autoclass:: lofarpipe.support.remotecommand.RemoteCommandRecipeMixIn

   See the :ref:`distribution <lofarpipe-remotecommand>` section for details.

.. autoclass:: lofarpipe.support.ipython.IPythonRecipeMixIn

   The use of IPython within the pipeline framework is :ref:`deprecated
   <ipython-deprecated>`.

.. _lofarpipe-ingredients:

Ingredients
===========

.. todo::

   lofarpipe ingredients description

.. _lofarpipe-remotecommand:

Distribution
============

.. todo::

   lofarpipe distribution description

.. _lofarpipe-utility:

Utilities
=========

.. todo::

   lofarpipe utilities description
