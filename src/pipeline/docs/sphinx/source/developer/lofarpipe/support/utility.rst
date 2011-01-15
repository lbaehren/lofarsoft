.. _lofarpipe-utility:

*********
Utilities
*********

The framework provides a number of convenience and utility functions. These
are not fundamental to the operation of the framework itself, but rathr
provude functionality which is commonly needed in pipeline recipes.

.. _parset-handling:

Parset handling
---------------

Parsets ("parameter sets") are files containing key-value pairs commonly used
for the configuration of LOFAR tools. Many pipeline recipes will, at heart,
run a standard tool over a number of datasets in parallel by substituting
per-dataset values into a template parset. These routines are designed to
simplify that process.

.. autoclass:: lofarpipe.support.parset.Parset
   :show-inheritance:

.. autofunction:: lofarpipe.support.parset.get_parset

.. autofunction:: lofarpipe.support.parset.patch_parset

.. autofunction:: lofarpipe.support.parset.patched_parset

Cluster descriptions (clusterdesc) handling
-------------------------------------------

Clusterdesc files (see :ref:`distproc-blurb`) describe the layout of the
compute cluster. They can be used within the pipeline to help choose nodes to
which jobs may be dispatched.

.. autoclass:: lofarpipe.support.clusterdesc.ClusterDesc

.. autofunction:: lofarpipe.support.clusterdesc.get_compute_nodes

.. autofunction:: lofarpipe.support.clusterdesc.get_head_node

Grouping input data
-------------------

Often, a long list of input datasets are grouped according to some criteria
for processing. These routines provide some useful ways of batching-up data.

.. autofunction:: lofarpipe.support.group_data.group_files

.. autofunction:: lofarpipe.support.group_data.gvds_iterator

.. autofunction:: lofarpipe.support.group_data.load_data_map

Process control
---------------

Many pipeline recipes spawn an external executable and wait for it to
complete. These routines can assist the recipe author by simplifying this
process and automatically recovering from transient errors.

.. autofunction:: lofarpipe.support.utilities.spawn_process

.. autofunction:: lofarpipe.support.utilities.catch_segfaults

File and directory maniupulaton
-------------------------------

.. autofunction:: lofarpipe.support.utilities.get_mountpoint

.. autofunction:: lofarpipe.support.utilities.create_directory

Iterators and generators
------------------------

.. autofunction:: lofarpipe.support.utilities.is_iterable

.. autofunction:: lofarpipe.support.utilities.izip_longest

.. autofunction:: lofarpipe.support.utilities.group_iterable

Miscellaneous
-------------

.. autofunction:: lofarpipe.support.utilities.read_initscript

.. autofunction:: lofarpipe.support.utilities.string_to_list

Exceptions
----------

The follow exceptions may be raised by pipeline components.

.. automodule:: lofarpipe.support.lofarexceptions
   :members:
   :undoc-members:

