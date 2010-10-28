.. _recipe-datamapper:

==========
datamapper
==========

The ``datamapper`` recipe parses a list of filenames and attempts to map them to
appropriate compute nodes (ie, which can access the files) on the LOFAR CEP
cluster. Mapping by filename in this way is fragile, but is the best we can do
for now.

Inputs
------

No arguments.

The following options are accepted:

``mapfile`` (:class:`StringField`)
    The full path (including filename) of the mapfile to produce.

Outputs
-------

``mapfile`` (:class:`FileField`)
    The full path (including filename) of the generated mapfile.

Master
------

.. literalinclude:: ../../../../../recipes/master/datamapper.py
