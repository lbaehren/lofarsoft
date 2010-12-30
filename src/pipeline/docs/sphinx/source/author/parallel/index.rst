.. _parallelisation-docs:

================================
Parallelisation and distribution
================================

The pipeline system contains a simple system for dispatching compute-intensive
jobs to designated nodes. In addition, it is possible to integrate with the
job distribution system included with `IPython <http://ipython.scipy.org>`_,
for a possibly more comprehensive (and certainly more complex!) approach.

.. toctree::
   :maxdepth: 1

   builtin.rst
   ipython.rst
