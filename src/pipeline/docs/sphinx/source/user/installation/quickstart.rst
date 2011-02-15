.. _framework-quickstart:

CEP quickstart
==============

.. todo::

   Bring this quickstart guide in-line with the current situation.

This section provides some quick notes on getting started with the pipeline
system. More details are available in subsequent sections of this chapter.

This section describes the basic requirements for setting up the pipeline
framework. You may also need further configuration to run specific tools in
your pipeline: see, for example, the Standard Imaging Pipeline
:ref:`sip-quickstart` section.

Locate the pipeline dependencies
--------------------------------

There are a number of Python packages which are required for the framework to
operate: see :ref:`framework-dependencies`. On the LOFAR cluster, these are
available under ``/opt/pipeline/dependencies``. Ensure the appropriate
directories are available in the environment variables ``$PATH`` (should
contain ``/opt/pipeline/dependencies/bin``)
and ``$PYTHONPATH``
(``/opt/pipeline/dependencies/lib/python2.5/site-packages``). To avoid any
possible conflicts with system installations, it is best to list these paths
early in the relevant variables.

Ensure the framework modules are available
------------------------------------------

There are two Python packages which comprise the pipeline framework: :mod:`ep`
and :mod:`lofarpipe`. These must both be available on your ``$PYTHONPATH``.
The easiest way to achieve this is to use the system installations in
``/opt/pipeline/framework``: add
``/opt/pipeline/framework/lib/python2.5/site-packages`` to your
``$PYTHONPATH``. Alternatively, you may wish to build and install your own
copies for development purposes: see :ref:`building-modules` for details.

Decide on a basic layout
------------------------

The pipeline will store all its logs, results, configuration data, etc in a
centralised location or "runtime directory". This should be accessible from
all nodes you will be using -- both the head node, and any compute nodes --
and should be writable (at least) by the userid under which the pipeline will
run. You should create this directory now.

If you will be using the compute nodes to store data on their local disks, you
will also need to create a "working directory" in a standard location on each
of them. On the LOFAR cluster, ``/data/scratch/[username]`` is a good choice.
This can be easily achieved using ``cexec``; for instance:

.. code-block:: bash

   $ cexec sub3:0-8 mkdir -p /data/scratch/swinbank

Produce a ``clusterdesc`` file
------------------------------

The ``clusterdesc`` file describes the layout of the cluster -- the names of
the various nodes, what disks they have access to, and so on. Some are already
available in LOFAR Subversion. A minimal file for subcluster three could be:

.. code-block:: bash

   Head.Nodes = [ lfe001..2 ]
   Compute.Nodes = [ lce019..027 ]

It doesn't matter where you save this, but you might find it convenient to
leave it in the runtime directory.

.. _pipeline-config:

Produce a pipeline configuration file
-------------------------------------

This file will contain all the standard information the pipeline framework
needs to get going. For a basic pipeline, running only on the head node, you
should have something like:

.. literalinclude:: ../../../../../docs/examples/definition/dummy/pipeline.cfg

Ensure that the ``runtime_directory`` and ``default_working_directory``
directives match the directories you created above. The others can mostly be
ignored for now, unless you know you need to change them.

If you also want to use the cluster, you need to add another two stanzas:

.. code-block:: none

  [cluster]
  clusterdesc = %(runtime_directory)s/sub3.clusterdesc
  task_furl = %(runtime_directory)s/task.furl
  multiengine_furl = %(runtime_directory)s/multiengine.furl

  [deploy]
  script_path = /opt/pipeline/framework/bin
  controller_ppath = /opt/pipeline/dependencies/lib/python2.5/site-packages:/opt/pipeline/framework/lib/python2.5/site-packages
  engine_ppath = /opt/pipeline/dependencies/lib/python2.5/site-packages/:/opt/pipeline/framework/lib/python2.5/site-packages
  engine_lpath = /opt/pipeline/dependencies/lib

You should ensure the ``clusterdesc`` directive points at the clusterdesc
file you are using. Note that ``%(runtime_directory)s`` will be expanded to
the path you've specified for the runtime directory.

``engine_lpath`` and ``engine_ppath`` specify (respectively) the
``$LD_LIBRARY_PATH`` and ``$PYTHONPATH`` that will be set for jobs on the
compute nodes. These should (at least) point to the dependencies and the
framework, as above, but should also include any necessary paths for code
which you will be running on the engines (imaging tools, data processing,
etc).

The other variables can be left at the default settings for now, unless you
know they need to be changed.

When looking for a configuration file, the framework will look first in its
current working directory for ``pipeline.cfg`` and, if nothing is there, look
under ``~/.pipeline.cfg``. Save yours somewhere appropriate.

At this point, the framework should be ready to run on the head node. If
required, continue to :ref:`launch-cluster`.

.. _launch-cluster:

Setting up the IPython cluster
------------------------------

The IPython system consists of a controller, which runs on the head node, and
engines, which run on the compute nodes. See the sections on :ref:`IPython
<ipython-blurb>` and the :ref:`cluster layout <cluster-layout>` for details.
Simple Python scripts make it easy to start and stop the cluster. This can be
done independently of an individual pipeline run: one can start the engines
once, run multiple piplines using the same engines, and then shut it down.

The relevant scripts are available in ``/opt/pipeline/framework/bin``, named
``start_cluster.py`` and ``stop_cluster.py``. Each accepts the name of a
pipeline configuration file as an optional argument: if one is not provided,
it defaults to ``~/.pipeline.cfg``.

Usage is very straightforward:

.. code-block:: bash

  $ /opt/pipeline/framework/bin/start_cluster.py --config /path/to/pipeline.cfg

After the script has finished executing, you can continue to set up and run
your pipeline. When finished, shut down the cluster:

.. code-block:: bash

  $ /opt/pipeline/framework/bin/stop_cluster.py --config /path/to/pipeline.cfg

