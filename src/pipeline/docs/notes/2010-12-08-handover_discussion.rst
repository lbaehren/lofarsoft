==================
Pipeline Framework
==================
Points for Discussion
=====================
8 December 2010
===============

Job management
--------------

- Each pipeline "job" (ie, a combination of pipeline definition & a
  particular dataset) is given a job identifier (a free form string, but
  generally based on the obsid)
- Each job may be run multiple times; logs, results, etc are filed by a
  combination of job identifier and pipeline start time

Configuration
-------------

- pipeline.cfg file (in Python ConfigParser format) is used to configure the
  pipeline system.
- Includes things like where to search for recipes, what log format to use,
  etc.

Recipes & Tasks
---------------

- Continue to use "recipes" to describe individual components, as in Cuisine
  (indeed, ultimately derives from Cuisine's WSRTrecipe).
- We add the concept of a "task", which is a short-cut to running a
  recipe with a particular set of parameters; access via self.run_task()
  method in recipe.
- Tasks are defined through another configuration file (tasks.cfg).
- Entirely optional: everything can be done the "old-fashioned" way of
  specifying inputs directly in the recipe. But tasks can make the recipe
  code cleaner, and help separate configuration details from code.

Inputs, outputs, type checking
------------------------------

- Input and output dicts for recipes will be checked for completeness and
  data types.
- This enables the pipeline to check inputs for correctness before running a
  recipe, rather than failing part through.
- A recipe must declare any inputs it expects as part of its definition.
- Acceptable types are based on simple classes. Basics (strings, ints,
  floats...) are already defined as part of the framework; the recipe author
  is encouraged to specify (eg "this must be a float with value between 0.3
  and 1.5").
- Default and/or optional inputs can also be declared.
- Uniform parser and the same type-checking is applied reading inputs from 
  command line options or from the tasks definitions.

Distribution
------------

- The pipeline includes its own distribution system, whereby recipes (running
  on the head node) can call node scripts (running on other machines).
- Node scripts are dispatched using either SSH or mpirun (depending on
  pipeline.cfg; SSH by default).
- Pickleable Python objects can be sent to and retrieved from the nodes, so
  complex configuration data or results can be exchanged.
- The pipeline can detect a failure on one node, and shut down the rest of the
  system if required.
- The recipes for rficonsole, NDPPP, BBS and cimager all use this system.
  None of the current recipes use other systems (eg "startdistproc").
- Helper functions make this system very easy for independent tasks (eg
  running multiple instances of rficonsole, NDPPP). For more involved
  workflows (eg BBS, where KernelControl and GlobalContol must be run
  simultaneously), a more elaborate recipe is required.

Parsets and tool configuration
------------------------------

- Most imaging pipeline tasks are configured by means of a parset.
- Typically, the pipeline will take a template parset missing names of eg
  input and output files, and fill in the blanks for each file to be
  processed.
- There are helper routines to do this in the pipeline framework
  (utilities.patch_parset() et al).

Logging
-------

- Pervasive use of Python logging module throughout.
- Every recipe has a logger (self.logger), which can be used directly and
  passed to (most) pipeline functions.
- Logger is also made available on compute nodes when running in a
  distributed way.

Users
-----

- There is currently not one "standard" imaging pipeline definition. The
  system is designed to be modular enough that processing chains can be
  quickly in response to requests from commissioners.
- As well as the imaging pipeline, the Pulsar and Transients groups have also
  defined and are actively using pipelines in this framework.
- The Pulsar pipeline is still dependent on the IPython system.
