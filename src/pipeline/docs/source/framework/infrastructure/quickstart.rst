.. _framework-quickstart:

Quickstart
==========

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

.. literalinclude:: ../../../../examples/definition/dummy/pipeline.cfg

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

``engine_ppath`` and ``engine_lpath`` specify (respectively) the
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
:ref:`fabric-blurb` is used to start up and shut down the IPython components;
you are encouraged to have at least a passing acquaintance with the Fabric
system to understand how this works.

Fabric (available in ``/opt/pipeline/dependencies``) is configured by means of
a so-called "fabfile". An example is at ``/opt/pipeline/fabfile.py``. This
fabfile defines four commands: ``start_controller``, ``stop_controller``,
``start_engine`` and ``stop_engine``. It can also parse your pipeline
configuration file, read the clusterdesc file, and hence select the
appropriate machines for ``head_node`` and ``compute_nodes``. 

.. code-block:: bash

   $ cd /opt/pipeline # must be run from directory containing fabfile
   $ fab head_node start_controller  # starts the IPython controller on the head node
   $ fab compute_nodes start_engines # starts engines on the compute nodes

By default, it uses ``~/.pipeline.cfg`` to locate the correct clusterdesc, but
you can specify this in the fab invocation. Note that each part of the Fabric
command line needs this specifying independently [1]_:

.. code-block:: bash

  $ fab head_node:/path/to/pipeline.cfg start_controller:/path/to/pipeline.cfg

Once the controller and engines have been deployed, they can be re-used for
many pipeline runs. However, when you're done, you should shut them down:

.. code-block:: bash

  $ cd /opt/pipeline
  $ fab compute_nodes stop_engine
  $ fab head_node stop_controller

.. rubric:: Footnotes

.. [1] This is cumbersome & can be simplified with a shell alias. In tcsh:

   .. code-block:: tcsh

      alias start_head "fab head_node:\!:1 start_controller:\!:1"
      alias start_compute "fab compute_nodes:\!:1 start_engine:\!:1"
      alias stop_compute "fab compute_nodes:\!:1 stop_engine:\!:1"
      alias stop_head "fab head_node:\!:1 stop_controller:\!:1"
    
      alias start_all "fab head_node:\!:1 start_controller:\!:1 && fab  compute_nodes:\!:1 start_engine:\!:1"
      alias stop_all "fab compute_nodes:\!:1 stop_engine:\!:1 && fab  head_node:\!:1 stop_controller:\!:1"

  Thanks to Anastasia Alexov for this suggestion.
