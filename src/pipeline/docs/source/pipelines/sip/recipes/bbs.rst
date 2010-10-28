.. _recipe-bbs:

===
BBS
===

The ``bbs`` recipe coordinates running BBS on a group of MeasurementSets. It
runs both ``GlobalControl`` and ``KernelControl``; as yet, ``SolverControl``
has not been integrated.

The recipe will also run the :ref:`recipe-sourcedb` and :ref:`recipe-parmdb`
recipes on each of the input MeasuementSets.

Inputs
------

The BBS recipe expects to receive a mapfile describing the data to be
processed as its argument.

The following options are accepted:

``control_exec`` (:class:`ExecField`)
    The full path to the BBS GlobalControl executable.

``kernel_exec`` (:class:`ExecField`)
    The full path to the BBS KernelControl executable.

``initscript`` (:class:`FileField`)
    The full path to an (Bourne) shell script which will intialise the
    environment (ie, ``lofarinit.sh``).

``parset`` (:class:`FileField`)
    The full path to the BBS configuration parset.

``key`` (:class:`StringField`)

``db_host`` (:class:`StringField`)

``db_user`` (:class:`StringField`)

``db_name`` (:class:`StringField`)
    Standard BBS database configuration parameters (see BBS documentation).

``combinevds`` (:class:`ExecField`, default: ``"/opt/LofIm/daily/lofar/bin/combinevds"``)
    Full path to ``combinevds`` executable.

``nproc`` (:class:`IntField`, default: ``8``)
    The maximum number of simultaneous KernelControl processes per processing node.

Outputs
-------

None.

Master
------

.. literalinclude:: ../../../../../recipes/master/bbs.py

Nodes
-----

.. literalinclude:: ../../../../../recipes/nodes/bbs.py
