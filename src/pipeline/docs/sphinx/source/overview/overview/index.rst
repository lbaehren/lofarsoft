.. _framework-overview:

**********************
Overview of a pipeline
**********************

Before plunging into the nitty-gritty of all the various components that make
up a pipeline, let's first take a bird's-eye overview of the concepts
involved.

The figure shows a schematic pipeline layout, illustrating most of the relevant
concepts. We will consider each of these in turn below.

.. image:: ../pipeline-flowchart.png

Recipes
=======

A *recipe* is a unit of the pipeline. It consists of a a task with a given set
of inputs and outputs. A given recipe may contain calls to subsidiary recipes,
for which it will provide the inputs and use the outputs in its own
processing. Note that the whole pipeline control structure is itself a recipe:
it takes a given set of inputs (visibility data) and produces some outputs
(images and associated metadata) by running through a series of defined steps.
In fact, each of those steps is itself a recipe -- one for flagging, one for
calibration, and so on.

Although some recipes are provided with the pipeline framework, it is
anticipated that users will wish to define their own. A search path for
recipes can be specified, enabling each user to maintain their own private (or
shared) repositories of recipes.

Tasks and Configuration
=======================

A recipe describes the steps that need to be taken to perform some particular
action on the data. For instance, a recipe might describe how to set up and
run an imager process. Often, the recipe will take a series of parameters
describing how it should be run -- what time steps to image, whether to use
the W-projection algorithm, the shape or the restoring beam, and so on. These
are provided as a series of input arguments to the recipe. Some sets of
arguments will be used repeatedly: a set of default configurations for
different modes, say. These can be bundled together as a *task*: a recipe
together with a set of defined parameters, and saved in a configuration file
for easy access.

As with recipes, it is anticipated that users will build up their own
libraries of pre-defined tasks for whatever applications they find necessary.

Control recipes and pipeline definition
=======================================

The *control* recipe is a special type of a normal recipe. The fundamentals
are the same; however, it contains some additional "housekeeping" logic which
may be useful for starting a pipeline. For instance, the control recipe can
configure a logging system for the pipeline, and may be used to interface with
LOFAR's MAC/SAS control system.

Often, a control recipe is referred to as a "pipeline definition".

.. _cluster-layout:

Cluster layout
==============

The control recipe runs on the so called "head node". The head node acts as
the coordination point for all pipeline activity. As and when required, the
head can dispatch compute-intensive jobs to other nodes on a cluster. Various
mechanisms are provided for queueing and dispatching jobs and collecting their
results.

Complex cluster layouts may be described by a "clusterdesc" file, as used by
:ref:`distproc`. The framework understands these files natively, whether or
not the distproc system itself is in use.

.. _pipeline-jobs:

Pipeline jobs
=============

Once a pipeline has been described as above, it will often be used multiple
times -- for example, to process multiple independent datasets. Each pipeline
run is associated with a "job identifier" which can be used to keep track of
these independent pipeline runs. Their results, logs, configuration and so on
can therefore be conveniently kept separate. The job identifier is a free-form
string: the user can choose whatever descriptive name is convenient.

The same job can be run multiple times: this might be convenient when a
previous attempt failed part way through due to a hardware fault, for example.
In this case, data is filed by job identifier combined with the pipeline start
time.
