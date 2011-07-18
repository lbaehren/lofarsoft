.. _template:

Template recipe
===============

This is a template recipe.
A recipe should be a **complete** description of a given task [#f1]_.
An example could be how to make an all-sky image.

A recipe should always start with an introduction describing briefly what
the recipe is for and a rough sketch of the approach.
The rest of the recipe can be divided into

Sections
--------

Subsections
^^^^^^^^^^^

And paragraphs.
"""""""""""""""

Appendix
--------

The recipe may have an appendix.

You can make *inline* references to *Python* modules, classes, methods and functions using the cross referencing directives as described in :ref:`documenting`.
There you will also find how to include :math:`\text{\LaTeX}` math and images.

To nicely refer to other documents you may use the ``.. seealso::`` directive.

.. seealso::
    
    For more information about markup see :ref:`documenting`

.. rubric:: Footnotes

.. [#f1] Note that this may or may not correspond to an actual :class:`~pycrtools.tasks.Task`. These should always be documented in the module source code and **not** in a recipe.

