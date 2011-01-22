*****
Tasks
*****

Declaring the full inputs and outputs for a recipe every time it is run can be
a chore, expecially when the same set of parameters are used frequently. This
not only complicates the pipeline definition, it inelegantly mixes
configuration parameters into the code defining the pipeline.  Therefore, we
introduce the concept of a "task": the combination of a recipe and a set of
standard paramters.

First, we define a task file in the :ref:`configuration file <config-file>`:

.. code-block:: none

  task_files = [/path/to/tasks.cfg]

Within that file, tasks are delimited by blocks headed by the task name in
square brackets. There then follows the recipe name and the parameters to be
provided. For example:

.. code-block:: none

  [example_task]
  recipe = example
  parameter1 = value1
  parameter2 = value2

Within the pipeline definition, this task can then be used by invoking the
:meth:`~lofarpipe.support.baserecipe.BaseRecipe.run_task` method:

.. code-block:: python

   self.run_task("example_task")

If required, parameters can be overridden in the arguments to
:meth:`~lofarpipe.support.baserecipe.BaseRecipe.run_task`. For example, we
might over-ride the value of ``parameter2`` above, while leaving
``parameter1`` intact:

.. code-block:: python

   self.run_task("example_task", parameter2="value3")
