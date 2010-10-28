.. _dppp-recipe:

====
DPPP
====

The ``new_dppp`` recipe runs DPPP (either ``NDPPP`` or -- in the unlikely
event it's required ``IDPPP``) on a number of MeasurementSets. It is suitable
for compressing and/or flagging data.

Inputs
------

One mapfile is accept as an argument.

The following options are accepted:

``executable`` (:class:`ExecField`)
    The full path to the relevant DPPP executable.

``initscript`` (:class:`FileField`)
    The full path to an (Bourne) shell script which will intialise the
    environment (ie, ``lofarinit.sh``).

``parset`` (:class:`FileField`)
    The full path to a DPPP configuration parset. The ``msin`` and ``msout``
    keys will be added by this recipe.

``suffix`` (:class:`StringField`, default: ``".dppp"``)
    A suffix added to the input filename to generate the output filename.

``working_directory`` (:class:`StringField`)
    The working directory to be used on the output nodes. Results are written
    here.

``data_start_time`` (:class:`StringField`, default: ``"None"``)

``data_end_time`` (:class:`StringField`, default: ``"None"``)
    Start and end times for the data being processed. If these are specified,
    the data will be padded to fill the time range.

``nproc`` (:class:`IntField`, default: ``8``)
    The maximum number of simultaneous DPPP processes per processing node.

``nthreads`` (:class:`IntField`, default: ``2``)
    The number of threads per DPPP process.

``mapfile`` (:class:`StringField`)
    Filename into which a mapfile describing the output data will be written.

``clobber`` (:class:`BoolField`, default: ``False``)
    If ``True``, pre-existing output files will be removed before processing
    starts. If ``False``, the pipeline will abort if files already exist with
    the appropriate output filenames.

Outputs
-------

``mapfile`` (:class:`FileField`)
    The full path to a mapfile describing the processed data.

``fullyflagged`` (:class:`ListField`)
    A list of all baselines which were completely flagged in any of the
    MeasurementSets.

Master
------

.. literalinclude:: ../../../../../recipes/master/new_dppp.py

Nodes
-----

.. literalinclude:: ../../../../../recipes/nodes/dppp.py
