.. _lofarpipe-ingredients:

***********
Ingredients
***********

One of the most fragile parts of pipeline definition is ensuring that the
inputs to a recipe are correct. Broadly, there are two failure modes here
which should be avoided. The first is perhaps the more obvious: it is always
desireable to check that the inputs actually make sense before attempting to
use them to operate on some piece of data -- particularly when that data may
be scientifically valuable. By checking inputs when the recipe is started,
awkward errors during the recipe run may be avoided.

The second failure mode concerns the source of the inputs. As we have seen,
recipe inputs may be provided on the command line, read from a configuration
file, calculated by another recipe as part of the pipeline run, etc. It is
important that these inputs are presented to the recipe code in a consistent
way: if, for example, a ``float`` is required, a ``float`` should be provided,
not a string read from the command line.

All LOFAR recipes define a series of ``inputs``, as described in the
:ref:`recipe design section <recipe-docs>`. These inputs are ultimately
derived from the :class:`~lofarpipe.support.lofaringredient.Field` class,
which provides validation and type-conversion (where appropriate) of the
inputs. A number of pre-defined fields are available, covering many basic use
cases (see :ref:`below <pre-defined-fields>`); the recipe author is also
encouraged to defined their own as necessary.

All recipes ultimately derive from
:class:`~lofarpipe.support.lofaringredient.RecipeIngredients` (see :ref:`the
last section <lofarpipe-recipes>`). This provides a number of standard fields,
which are present in all recipes, as well as ensuring that additional,
per-recipe fields are handled appropriately, including type-checking on recipe
instantiation. Within the recipe environment, the contents of fields are
available as ``self.inputs``, an instance of
:class:`~lofarpipe.support.lofaringredient.LOFARingredient`.

.. _pre-defined-fields:

Pre-defined fields
==================
.. automodule:: lofarpipe.support.lofaringredient
   :members: Field, StringField, IntField, FloatField, FileField, ExecField, DirectoryField, BoolField, ListField, DictField, FileList

Infrastructure
==============

.. autoclass:: lofarpipe.support.lofaringredient.RecipeIngredients

.. autoclass:: lofarpipe.support.lofaringredient.RecipeIngredientsMeta

.. autoclass:: lofarpipe.support.lofaringredient.LOFARingredient
