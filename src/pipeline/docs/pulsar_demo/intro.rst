=======================
Pipeline Framework Demo
=======================
17 December 2010
================

What is a pipeline?
-------------------

- A standardised way of interacting with the rest of LOFAR; logging, MAC/SAS
  integration, etc.
- Should make the developer's job *easier* (after an initial learning curve),
  by providing a bunch of helper routines.
- Provides sanity checking of inputs, outputs, etc.
- Does not dictate exactly how you write your code: designed to enable the
  developer as much freedom as necessary to get the job done. We have to
  interface with a wide variety of external executables, scripts, Python
  modules, ...

Basic concepts
--------------

- Framework is pure Python, with minimal dependencies on external libraries.
- Originated in the WSRT "Cuisine" framework, although almost no Cuisine code
  is now actually used.
- Also hardly used is IPython (although it's very hard to convince people it's
  not the "IPython framework"!).
- An inidividual "pipeline component" is wrapped in *recipe*: a Python
  script which describes how to carry out that tasks.
- Recipes take *inputs* and produce *outputs*. We do type & sanity checking
  on the inputs and outputs.
- Inputs will include some indication of the data to process (eg, a series
  of filenames) and any configuration parameters needed (eg, tool
  configuration parsets).
- A *task* is a recipe + a set of configuration parameters. Separate
  configuration from code; provide a shortcut for common processing jobs.
- Recipes can be nested: for example, in the imaging pipeline, the *bbs*
  recipe calls the *vdsmaker*, *parmdb* and *sourcedb* recipes as part of
  its run.
- A *pipeline* is just a recipe which calls a series of subsidiary recipes
  in order. (Often with a little extra boilerplate to receive messages from
  MAC/SAS etc.)

Job management
--------------

- Each pipeline "job" (ie, a combination of pipeline definition & a
  particular dataset) is given a job identifier (a free form string, but
  generally based on the obsid).
- Each job may be run multiple times; logs, results, etc are filed by a
  combination of job identifier and pipeline start time

Configuration
-------------

- pipeline.cfg file (in Python ConfigParser format) is used to configure the
  pipeline system.
- Includes things like where to search for recipes, what log format to use,
  etc.

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
- Basic integration with log4cplus/log4cxx as used by the LofarLogger.
