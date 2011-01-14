.. _lofarpipe-remotecommand:

************
Distribution
************

Before proceeding with this section, the reader should ensure they are
familiar with the :ref:`recipe author's perspective` on how tasks can be
distributed within the pipeline framework. This section will describe in
detail what goes on behind the scenes and how it can be extended.

.. todo::

   Details!

Node scripts
------------

.. autoclass:: lofarpipe.support.lofarnode.LOFARnode
   :members:

.. autoclass:: lofarpipe.support.lofarnode.LOFARnodeTCP
   :members:

Compute jobs
------------

.. autoclass:: lofarpipe.support.remotecommand.ComputeJob
   :members:

Scheduling and rate limiting
----------------------------

See :class:`~lofarpipe.support.remotecommand.RemoteCommandRecipeMixIn`.

.. autoclass:: lofarpipe.support.remotecommand.ProcessLimiter

Dispatch mechanisms
-------------------

.. todo::

   Extenting this system.

Dispatch mechanism can be specified in the ``remote`` section of the
:ref:`pipeline configuration <config-file>`

.. autofunction:: lofarpipe.support.remotecommand.run_remote_command

.. autofunction:: lofarpipe.support.remotecommand.run_via_mpirun

.. autofunction:: lofarpipe.support.remotecommand.run_via_paramiko

.. autoclass:: lofarpipe.support.remotecommand.ParamikoWrapper

Exchanging data between master and nodes
----------------------------------------

.. autofunction:: lofarpipe.support.jobserver.job_server

.. autoclass:: lofarpipe.support.jobserver.JobSocketReceiver
   :show-inheritance:

.. autoclass:: lofarpipe.support.jobserver.JobStreamHandler
   :show-inheritance:

Clean shut-down and recovery
----------------------------

.. autofunction:: lofarpipe.support.remotecommand.threadwatcher
