*****************
Pipeline Overview
*****************

Before plunging into the nitty-gritty of all the various components that make
up a pipeline, let's first take a bird's-eye overview of the concepts
involved.

The figure shows a schmatic pipeline layout, illustrating most of the relevant
concepts. We will consider each of these in turn below.

.. image:: ../pipeline-flowchart.png

Cluster Layout
==============

The pipeline framework makes the assumption that it will run on a cluster
comprised of a "head node" and a number of "compute nodes". The pipeline
control logic runs on the head node, and may perform less compute-intensive
jobs here. When required, jobs are dispatched over the network to the compute
nodes: the head node may then wait and receive the results before continuing.
Job definition and parameters may be pushed to the compute nodes directly from
the head using :ref:`ipython-blurb`; however, for bandwidth and latency
reasons, large data files should be written to and read from shared NFS
mounts.

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

Control
=======

The *control* recipe is a specialist type of a normal recipe. The fundamentals
are the same; however, it contains some additional "housekeeping" logic which
may be useful for starting a pipeline. For instance, the control recipe can
configure a logging system for the pipeline, and may be used to interface with
LOFAR's MAC/SAS control system.

Distributed Processing
======================

Tasks and Configuration
=======================




