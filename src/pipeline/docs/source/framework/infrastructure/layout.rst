.. _pipeline-layout:

Pipeline layout
===============

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


