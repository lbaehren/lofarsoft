.. _running-basic:

***********************
Running basic pipelines
***********************

This section will describe the minimal configuration needed to run a simple
pipeline. Every pipeline recipe is independently runnable as a stand-alone
module, so we start with that. We will then build a larger pipeline connecting
multiple modules.

A single recipe
---------------

We will start by running the :ref:`simple recipe <basic-recipe>` described in
the :ref:`recipe-docs` section. This is included in the standard framework
distribution: it will be available as
``/opt/pipeline/recipes/master/example.py`` if you have followed the
installation guide. When called in its simplest mode, this recipe will simply
return the results of running ``/bin/ls`` in the current working directory.

Before running, it is necessary to create a pipeline configuration file. The
simplest possible configuration file defines two options:
``runtime_directory``, which the pipeline framework uses for storing various
information about the framework state, and ``job_directory``, which contains
information relevant to a specific pipeline job. The former must exist; the
latter will be created if required, and may contain a reference to the runtime
directory. For example:

.. code-block:: none

  [DEFAULT]
  runtime_directory = /home/username/pipeline_runtime
  
  [layout]
  job_directory = %(runtime_directory)s/jobs/%(job_name)s

On startup, the framework will first search for a file named ``pipeline.cfg``
in your current working directory, before falling back to ``~/.pipeline.cfg``.
The user can also specify a configuration file on the command line.

With the configuration file in place, it is now possible to run the example
recipe. **Note** that all pipeline runs must be supplied with a job identifier
on startup: see the section on :ref:`pipeline-jobs` for more. The recipe can
then be executed as follows:

.. code-block:: bash

  $ python /opt/pipeline/recipes/master/example.py --job-name EXAMPLE_JOB -d
  $ python example.py -j EXAMPLE_JOB -d
  2010-10-26 18:38:31 INFO    example: This is a log message
  2010-10-26 18:38:31 DEBUG   example: /bin/ls stdout: bbs.py
  [ls output elided]
  2010-10-26 18:38:31 INFO    example: recipe example completed
  Results:
  stdout = [ls output elided]

Congratulations: you have run your first LOFAR pipeline!
