===================
Building a pipeline
===================

One of the major aims in the design of the pipeline system is to require as
little run-specific configuration as possible. In other words, we rely on
convention and sensible defaults wherever possible. This section describes
the standards that our pipeline system will rely on.


.. _pipeline-layout:

Pipeline layout
---------------

The pipeline system is designed to be organised in a standard directory
structure. Insofar as is possible, this contains all information needed to
manage a cluster and assosciated set of pipeline tasks. It is not designed to
contain the actual data that is being processed. It is assumed that
this directory will be available to all the various cluster nodes, presumably
using NFS.

The root directory of this structure is the ``runtime`` directory. This
contains all the information about a given "cluster" -- that is, all the
disks, compute nodes, management node, etc described by a given
``clusterdesc`` file. This top level directory contains that ``clusterdesc``,
and, if appropriate, information about an associated IPython cluster: 

* A PID file (``ipc.pid``) and log files from the IPython controller (named
  according to the pattern ``ipc.log${pid}.log``)

* An ``engines`` directory, which contains PID (``ipengine${N}.pid``, where
  ``N`` simply increments according to the number of engines on the host)
  files and log files (``log${PID}.log``) from the various engines in the
  cluster, organised into subdirectories by hostname.

* The files ``engine.furl``, ``multiengine.furl`` and ``task.furl``, which
  provide access to the IPython engines, multiengine client and task client
  resepectively. See the IPython documentation for details.

Of course, a single cluster (and hence runtime directory) may process many
different jobs. These are stored in the ``jobs`` subdirectory, and are
organised by job name -- an arbitrary user-supplied string.

Within each job directory, three further subdirectories are found:

``logs``
    Processing logs; where appropriate, these are filed by sub-band name.

``parsets``
    Paramaeter sets providing configuration information for the various
    pipeline components. These should provide the static parameters used by
    tasks such as ``DPPP`` and the imager; dynamic parameters, such as the
    name of the files to be processed, can be added by the pipeline at
    runtime.

``vds``
    Contains VDS and GDS files pointing to the location of the data to be
    processed on the cluster.

.. _config-file:

Configuration
-------------

Various default parameters for pipeline operation are stored in a
configuration file. Normally, this lives within the top-level Python
``pipeline`` module at ``pipeline/pipeline.cfg``. This file is designed to be
used in conjuntion with the ``ConfigParser`` module from the `Python Standard
Library <http://docs.python.org/library/configparser.html>`_.

The file should be read using an instance of ``SafeConfigParser``; at this
time, it should be given a job name and optionally a working directory to use
as part of the configuration::

  >>> from ConfigParser import SafeConfigParser as ConfigParser
  >>> from pipeline import __path__ as config_path
  >>> config = ConfigParser({
          "job_name": ....,
          "runtime_directory": ....
      })
  >>> config.read("%s/%s" % (config_path[0], "pipeline.cfg"))


Initialising the cluster
------------------------

The ``IPython`` controller and engine processes ("the IPython cluster") are
started and stopped separately from the rest of the pipeline code, since each
cluster can and should be used for processesing multiple pipeline jobs.
Currently, this is done using :ref:`fabric-blurb` [#f1]_.

An appropriate, if basic, ``fabfile.py`` can be found in the ``deploy``
directory in the pipeline tree. It reads a ``clusterdesc`` file (see the
:ref:`distproc-blurb` section) specified in the default pipeline configuration
file. It then provides the commands ``controller_host`` and ``engine_hosts``
to select the hosts on which to operate, ``start_controller`` and
``start_engines`` to start the engines, and the equivalents to stop. For
example:

.. code-block:: bash

    $ fab controller_host start_controller
    $ fab engine_hosts start_engines
    $ fab engine_hosts stop_engines
    $ fab controller_host stop_controller

FURL files are written as per the :ref:`pipeline-layout` section.


.. [#f1] Actually, it's not clear if Fabric is really significantly better
  than spawning a bunch of SSH processes, and, in it's current stage of
  development, it seems to be more fragile. Thus, it may be replaced in a
  later iteration of the codebase.
