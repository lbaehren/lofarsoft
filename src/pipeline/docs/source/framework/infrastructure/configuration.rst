.. _config-file:

*************
Configuration
*************

Various default parameters for pipeline operation are stored in a
configuration file. The contents of this file are available to recipes as
``self.config``, which is an instance of ``SafeConfigParser`` from the `Python
Standard Library <http://docs.python.org/library/configparser.html>`_.

Accepted parameters
===================

Section ``DEFAULT``
-------------------

This section contains global pipeline configuration parameters. They can be
referred to in other sections of the configuration file using ``%()s`` syntax:
see the Python documentation (lined above) for details.

``runtime_directory``
    Overall pipeline framework runtime directory. Pipeline framework metadata
    which is shared between multiple jobs will be written here.

    This parameter is **required**.

``recipe_directories``
    List of directories in which to search for recipes. Multiple directories
    can be specified using a Python list-like syntax: ``[/path/to/dir/1,
    /path/to/dir/2]``.

``task_files``
    List of task definition files. Multiple entries may be specified in the
    same way as above.

Section ``layout``
------------------

This section contains paths which may be referenced by individual pipeline
recipes, for example to save state, locate parset files or write logs.

``job_directory``
    This will contain configuration information (parsets, etc) for a given
    pipeline job. Metadata referring to an ongoing run may be written into
    this directory (and will normally be removed when the run finishes),
    and logs and output files are colelcted here.

    This directory should be available to (eg, NFS mounted) to **every** node
    that is involved in the pipeline run.

    This parameter is **required**.

Section ``cluster``
-------------------

This section describes the layout of a cluster which can be used for
distributed processing.

``clusterdesc``
    The full path to a ``clusterdesc`` file (see :ref:`distproc-blurb`)
    which describes the cluster configuration to be used by the pipeline.

    This parameter is **required** if remote jobs are being used.

``task_furl`` and ``multiengine_furl``
    Filenames which will be used for the FURL files needed for collection to an
    :ref:`ipython-blurb` cluster.

    These parameters are only required if IPython is being used within the
    pipeline.

Section ``deploy``
------------------

This section describes the environment used for starting up jobs on remote
hosts.

``engine_lpath`` and ``engine_ppath``
    The values of ``$LD_LIBRARY_PATH`` and ``$PYTHONPATH`` which will be used
    for all remote commands. Note that these are **not** inherited from the
    environment on the pipeline head node.

    These parameter are **required** if remote jobs are being used.

``controller_ppath``
    The value of ``$PYTHONPATH`` which will be used for an IPython controller
    started on the head node. Note that this is not used (or required) if
    IPython is not being used.

``script_path``
    The location of scripts needed for starting IPython engines on remote
    hosts. This is not used (or required) if IPython is not being used, or if
    a non-pipeline method is used for starting the engines.

Section ``logging``
-------------------

This section enables the user to customise the pipeline logging output. Note
that it is entirely optional: a log file with default settings will be written
to the ``job_directory`` if this section is omitted.

``log_file``
    Output filename for logging.

``format`` and ``datefmt``
    Format for log entries and dates, respectively. These are used exactly as
    per the Python logging system; see `its documentation
    <http://docs.python.org/library/logging.html#formatters>`_ for details.

Section ``remote``
------------------

This section contains parameters for configuring the remote command execution
strategy. It is intended for expert use only.
