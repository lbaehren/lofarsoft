.. _parallelisation-docs:

============================
Parallelisation with IPython
============================

The pipeline system contains various mechanisms for streamlining the
deployment and execution of parallelised code across the cluster using
``IPython``. These are described in this section.

A brief introduction to IPython
-------------------------------

The ``IPython system`` works by running multiple "engine" processes, each of
which registers with a "controller" process. Each engine provides a fully
functional Python interpreter; the controller can send both data and Python
statements for execution on the engines, and then retrieve results in the form
of pickleable Python objects.

The communication between the engines and the controller is
network-transparent: it can take place across a cluster, or, indeed, across
the internet. Of course, transferring significant quantites of data between
the controller and engine is slow: usually, it is convenient to make data
files available to the engines using shared storage (NFS, etc) rather than
sending them directly.

There are various different patterns by which commands can be allocated to
engines. In the simplest "multiengine" scheme, the user selects one or more of
the registered engines and directly sends a command to them, then blocks until
the job completes. Equally useful is the "task" system, whereby the user can
submit a variety of tasks to the controller, each of which is queued and then
submitted to the next engine which becomes free (by default—other queue types
are available).

Starting the IPython system
---------------------------

The simplest way to start the IPython cluster for a single pipeline run is to
use the :func:`lofarpipe.support.clusterhandler.ipython_cluster` context
manager. This will automatically set up and tear down the IPython system based
on the current cluster configuration. For example:

.. code-block:: python

    from __future__ import with_statement
    from lofarpipe.support.control import control
    from lofarpipe.support.clusterhandler import clusterhandler

    class ExamplePipeline(control):
        def pipeline_logic(self):
            with ipython_cluster():
                # include pipeline logic

An optional argument, ``nproc``, to :func:`ipython_cluster` specifies how many
engines to start per cluster node; by default, the number of engines is set
equal to the number of CPU cores on the node.

If several pipeline runs on the same cluster are anticipated, it is more
efficient not to restart the cluster on every run. This is possible due to the
scripts ``start_cluster.py`` and ``stop_cluster.py``. Each accepts the name of a
pipeline configuration file as an optional argument: if one is not provided,
it defaults to ``~/.pipeline.cfg``.

Usage is very straightforward:

.. code-block:: bash

  $ /opt/pipeline/framework/bin/start_cluster.py --config /path/to/pipeline.cfg

After the script has finished executing, you can continue to set up and run
your pipeline. When finished, shut down the cluster:

.. code-block:: bash

  $ /opt/pipeline/framework/bin/stop_cluster.py --config /path/to/pipeline.cfg

The ``--num-engines`` option to ``start_cluster.py`` may be used to specify
how many engines to start per node:

.. code-block:: bash

  $ /opt/pipeline/framework/bin/start_cluster.py --num-engines 4

