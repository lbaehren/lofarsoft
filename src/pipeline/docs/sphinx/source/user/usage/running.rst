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
The user can also specify a configuration file on the command line. See the
:ref:`config-file` section for full details on the configuration system.

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

A pipeline
----------

To turn this simple recipe into a fully-functional pipeline is simply a matter
of wrapping it in a pipeline definition derived from the :class:`control`
class. The :meth:`cook_recipe` method can then be used to run the recipe. Note
that it needs to be provided with appropriate input and output objects. An
appropriate pipeline definition might be:

.. code-block:: python

  import sys

  from lofarpipe.support.control import control
  from lofarpipe.support.lofaringredient import LOFARinput, LOFARoutput

  class pipeline(control):
      def pipeline_logic(self):
          recipe_inputs = LOFARinput(self.inputs)
          recipe_outputs = LOFARoutput()
          recipe_inputs['executable'] == '/bin/true'
          self.cook_recipe('example', recipe_inputs, recipe_outputs)

  if __name__ == "__main__":
      sys.exit(pipeline().main())

In order to make it clear where to find the ``example`` recipe, we also need
to edit ``pipeline.cfg``, adding a ``recipe_directories`` directive to the
``DEFAULT`` section:

.. code-block:: none

    recipe_directories = [/opt/pipeline/recipes]

Saving the above definition in ``pipeline.py``, we now have:

.. code-block:: bash

  $ python pipeline.py -j test_job -d
  2010-10-27 18:17:31 INFO    pipeline: LOFAR Pipeline (pipeline) starting.
  2010-10-27 18:17:31 INFO    pipeline.example: recipe example started
  2010-10-27 18:17:31 INFO    pipeline.example: Starting example recipe run
  2010-10-27 18:17:31 DEBUG   pipeline.example: Pipeline start time: 2010-10-27T16:17:31
  2010-10-27 18:17:31 INFO    pipeline.example: This is a log message
  2010-10-27 18:17:31 INFO    pipeline.example: recipe example completed
  2010-10-27 18:17:31 INFO    pipeline: recipe pipeline completed
  Results:

