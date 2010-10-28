.. _recipe-rficonsole:

==========
rficonsole
==========

The rficonsole recipe runs the rficonsole executable (flagger) across one or
more MeasurementSets.

Inputs
------

Excepts to a receive a mapfile as its only argument.

``executable`` (``ExecField``, default: ``/opt/LofIm/daily/lofar/bin/rficonsole``)
    The name of the executable to use.

``indirect_read`` (``BoolField``, default: ``False``)
    If ``True``, use rficonsole's ``-indirect-read`` option.

``working_dir`` (``StringField``, default: ``'/tmp'``)
    Temporary rficonsole products are stored under this root on each of the
    remote machines. This directory should therefore be writable on each
    machine, but need not be shared across hosts.

``nthreads`` (``IntField``, default: ``8``)
    Number of threads per rficonsole process.

``nproc`` (``IntField``, default: ``1``)
    Maximum number of simultaneous rficonsole processes per node.

``nmeasurementsets`` (``IntField``, default: ``8``)
    Maximum number of MeasurementSets processed by a single rficonsole process

``strategy`` (``FileField``, Optional)
    rficonsole strategy file.

Outputs
-------

None.

Master
------
.. literalinclude:: ../../../../../recipes/master/rficonsole.py

Nodes
-----
.. literalinclude:: ../../../../../recipes/master/rficonsole.py
