.. _builtin-parallel:

=================================
Pipeline-native task distribution
=================================

The pipeline system provides a built-in method for distributing tasks over a
network. This provides the facility to dispatch a job (effectively consisting
of a Python script plus a collection of arguments) to a remote host, then
collect the return code. The remote job can log directly to the standard
pipeline logger.

There is no direct facility for "pushing" and "pulling" Python objects to and
from the remote host: it is only possible to strings (which are sent as
command line arguments). If required, though, this is an easy extension:
simply serialise (pickle) your object into a file in the ``job_directory``,
then pass the filename on the command line to the remote host and deserialise
there. Alternatively, you may wish to investigate the
:ref:`parallelisation-ip` section, which provides a rather more complex
mechanism.

Parallel pipeline tasks are split into two parts: the first is the "master
recipe", which runs on the pipeline control node. This is just like any other
recipe (see :ref:`recipe-docs`), but should mix-in the class
:class:`RemoteCommandRecipeMixIn` to its composition, which provides some
additional functionality. The second part is the "node script", which is the
code which is actually executed on the compute machines. Both of these
components are discussed in more detail below.

It is worth adding that we will consider here there most straightforward
application of task distribution: simply firing off a number of parallel jobs,
where the order of execution and so on is not important. In some cases, that
is not an adequate solution. The basic machinery described here can, however,
be used to build rather more elaborate systems: see, for example, the
:ref:`recipe-bbs` recipe, which synchronises multiple processes, or the
:ref:`recipe-cimager` recipe, which pre-processes separate input files for
each job.

Recipes and the corresponding node scripts are saved in one of the
``recipe_directories`` defined in the pipeline :ref:`config-file`. By
convention, they are stored in directories named ``master`` and ``nodes``
respectively, and with the same filename. Thus, ``master/example_parallel.py``
for the recipe name, and ``nodes/example_parallel.py`` for the corresponding
node script. Indeed, the scripts below are available under those names in the
pipeline framework source tree.

Node scripts
============

As described, parallel processing runs a "node script" on each of the compute
machines. A node script should be executable from the command line, and must
define a class derived from :class:`LOFARnode` which defines a :meth:`run`
method which performs all the necessary processing. When executed, the script
should call the class constructor with the first two command line arguments,
then pass all the remainder to the :meth:`run_with_logging` method. For
example:

.. code-block:: python

  import sys
  from lofarpipe.support.lofarnode import LOFARnode

  class example_parallel(LOFARnode):
      def run(self, *args):
          for arg in args:
              self.logger.info("Received %s as argument" % arg)
          return 0

  if __name__ == "__main__":
      sys.exit(example_parallel(sys.argv[1], sys.argv[2]).run_with_logging(*sys.argv[3:]))

Note that you can continue to access the pipeline logger (as ``self.logger``)
within the node script, as shown.

It is important to realise that the node script must actually be accessible on
the node(s) which plan to run it! This is normally achieved by having all
pipeline components NFS mounted under a standard path.

Spawning remote commands
========================

Next, we will explore how to invoke such a node script from a recipe.

First, the recipe must mix-in the :class:`RemoteCommandRecipeMixIn` class.
This provides access to the :meth:`_schedule_jobs` method, which takes a list
of jobs to run and blocks until completion.

Jobs themselves are instances of
:class:`lofarpipe.support.remotecommand.ComputeJob`. This takes two mandatory
arguments: the target host and the command to execute on that host. It also
takes a list of arguments to that command.

If an error is detected during job processing, the :attr:`error` flag is set
on the recipe. This is an instance of :class:`threading.Event` from the Python
standard library. After job executing, the recipe can use this flag to check
for problems.

Thus, a simple recipe could be:

.. code-block:: python

  import sys
  from lofarpipe.support.baserecipe import BaseRecipe
  from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
  from lofarpipe.support.remotecommand import ComputeJob

  class example_parallel(BaseRecipe, RemoteCommandRecipeMixIn):
      def go(self):
          super(example_parallel, self).go()
          node_command = "python %s" % (self.__file__.replace("master", "nodes"))
          job = ComputeJob(hostname, node_command, arguments=["example_argument"])
          self._schedule_jobs([job])
          if self.error.isSet():
              return 1
          else:
              return 0

Note that we have used the convention on file naming described above to derive
the name of the node script to run based on the name of the recipe. Note that
the relevant path is that under which the file will be accessible *on the
node*.

Before running the recipe, check that your ``pipeline.cfg`` contains the
``engine_ppath`` and ``engine_lpath`` directives (see the :ref:`config-file`
section). Then it can be executed as follows:

.. code-block:: bash

  $ python example_parallel.py -j foo -d
  2010-10-28 14:14:18 DEBUG   example_parallel: Logging to 127.0.1.1:52584
  2010-10-28 14:14:18 DEBUG   example_parallel: Dispatching command to localhost with ssh
  2010-10-28 14:14:18 INFO    example_parallel: Waiting for compute threads...
  2010-10-28 14:14:18 INFO    node.heastro1.example_parallel: Received example_argument as argument
  2010-10-28 14:14:21 INFO    example_parallel: recipe example_parallel completed
  Results:

(Note that ``heastro1`` is the name of the remote host in this case).

By default, remote commands are dispatched by SSH. You should ensure that it
is possible for the user running the pipeline to log into the relevant
machines in a non-interactive way (eg, using SSH keys, an agent, etc),
otherwise the pipeline will be unable to proceed.

Distribution
============

The above recipe obviously only sent one job to a remote machine. However, the
:meth:`_schedule_jobs` method can accept a list of many jobs. In addition, it
takes a further argument -- ``max_per_node`` -- which specifies the maximum
number of jobs that will be run on a given host at the same time. This is
useful if each job is memory and/or compute intensive, and there are many to
be run per host: starting them all simultanously will lead to degraded
performance and (in the worst case) may cause the machine to run out of
memory.

Given some list of hosts and the corresponding job parameters, then, we can
schedule them as follows:

.. code-block:: python

  # joblist is a list of (host, job argument) tuples
  jobs = []
  command = 'python /path/to/node/script'
  for host, job_parameters in joblist:
      jobs.append(
          ComputeJob(host, command, arguments=job_parameters)
       )
  self._schedule_jobs(jobs, max_per_node=8)

Of course, this leaves the outstanding quesion of how to generate ``joblist``.
Often, the jobs to be dispatched by a recipe will involve running the same
routine over a number of different input files in parallel. Usually, those
input files are accessible from only some nodes in the cluster. Some
deterministic system (see the next section!) is used to allocate each data file
to an appropriate node, and this is saved on disk as a so-called "mapfile".
For example, a mapfile from a processing run on the LOFAR CEP cluster might
look like:

.. code-block:: none

  lce001=[ /data/scratch/swinbank/L2010_20852_1/L20852_SB000-uv.MS.dppp, /data/scratch/swinbank/L2010_20852_1/L20852_SB009-uv.MS.dppp, /data/scratch/swinban k/L2010_20852_1/L20852_SB018-uv.MS.dppp, /data/scratch/swinbank/L2010_20852_1/L20852_SB027-uv.MS.dppp ] lce002=[ /data/scratch/swinbank/L2010_20852_1/L20852_SB001-uv.MS.dppp, /data/scratch/swinbank/L2010_20852_1/L20852_SB010-uv.MS.dppp, /data/scratch/swinban k/L2010_20852_1/L20852_SB019-uv.MS.dppp ]
  lce003=[ /data/scratch/swinbank/L2010_20852_1/L20852_SB002-uv.MS.dppp, /data/scratch/swinbank/L2010_20852_1/L20852_SB011-uv.MS.dppp, /data/scratch/swinban k/L2010_20852_1/L20852_SB020-uv.MS.dppp ]
  lce004=[ /data/scratch/swinbank/L2010_20852_1/L20852_SB003-uv.MS.dppp, /data/scratch/swinbank/L2010_20852_1/L20852_SB012-uv.MS.dppp, /data/scratch/swinban k/L2010_20852_1/L20852_SB021-uv.MS.dppp ]
  lce005=[ /data/scratch/swinbank/L2010_20852_1/L20852_SB004-uv.MS.dppp, /data/scratch/swinbank/L2010_20852_1/L20852_SB013-uv.MS.dppp, /data/scratch/swinban k/L2010_20852_1/L20852_SB022-uv.MS.dppp ]
  lce006=[ /data/scratch/swinbank/L2010_20852_1/L20852_SB005-uv.MS.dppp, /data/scratch/swinbank/L2010_20852_1/L20852_SB014-uv.MS.dppp, /data/scratch/swinban k/L2010_20852_1/L20852_SB023-uv.MS.dppp ]
  lce007=[ /data/scratch/swinbank/L2010_20852_1/L20852_SB006-uv.MS.dppp, /data/scratch/swinbank/L2010_20852_1/L20852_SB015-uv.MS.dppp, /data/scratch/swinban k/L2010_20852_1/L20852_SB024-uv.MS.dppp ]
  lce008=[ /data/scratch/swinbank/L2010_20852_1/L20852_SB007-uv.MS.dppp, /data/scratch/swinbank/L2010_20852_1/L20852_SB016-uv.MS.dppp, /data/scratch/swinban k/L2010_20852_1/L20852_SB025-uv.MS.dppp ]
  lce009=[ /data/scratch/swinbank/L2010_20852_1/L20852_SB008-uv.MS.dppp, /data/scratch/swinbank/L2010_20852_1/L20852_SB017-uv.MS.dppp, /data/scratch/swinban k/L2010_20852_1/L20852_SB026-uv.MS.dppp ]
  lce010=[ /data/scratch/swinbank/L2010_20852_1/L20852_SB028-uv.MS.dppp ]

This is, conveniently, the in the standard LOFAR parameterset format. The name
of such a file can be supplied to a recipe (often it is given as an argument,
rather than an option. It is then available as ``inputs['args']``).

The function :func:`lofarpipe.support.group_data.load_data_map` can
conveniently read such a file, returning a list of ``(host, filename)``
tuples which can be easily iterated over and combined with a standard set of
arguments to generate a series of :class:`ComputeJob` objects which can be
dispatched as above. For example, see the :ref:`recipe-rficonsole` recipe,
which takes exactly this approach.

The final part of the puzzle is how best to generate an appropriate mapfile
describing your data. Exactly how this can be managed will be specific to your
particular system. The :ref:`recipe-datamapper` and
:ref:`recipe-storagemapper` recipes demonstrate how this can be done in LOFAR
CEP.
