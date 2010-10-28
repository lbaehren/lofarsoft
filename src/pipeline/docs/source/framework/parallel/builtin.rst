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

  class parallel_example(LOFARnode):
      def run(self, *args):
          for arg in args:
              self.logger.info("Received %s as argument" % arg)
          return 0

  if __name__ == "__main__":
      sys.exit(parallel_example(sys.argv[1], sys.argv[2]).run_with_logging(*sys.argv[3:]))

Note that you can continue to access the pipeline logger (as ``self.logger``)
within the node script, as shown.

Parallel recipes
================

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

Thus, a simple parallel recipe could be:

.. code-block:: python

  import sys
  from lofarpipe.support.baserecipe import BaseRecipe
  from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
  from lofarpipe.support.remotecommand import ComputeJob

  class parallel_example(BaseRecipe, RemoteCommandRecipeMixIn):
      def go(self):
          super(parallel_example, self).go()
          node_command = "python %s" % (self.__file__.replace("master", "nodes"))
          job = ComputeJob(hostname, node_command, arguments=[])
          self._schedule_jobs([job])
          if self.error.isSet():
              return 1
          else
              return 0

