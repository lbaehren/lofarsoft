**********************
Recipe design tutorial
**********************

The pipeline framework provides a simple system for dispatching
compute-intensive jobs to remote hosts. This includes sending and
receiving complex data as part of the job. This section provides a tutorial
introduction to writing a recipe which takes advantage of this capability.

Problem specification
=====================

This tutorial addresses a simple real-world use case. This example was
suggested by Anastasia Alexov, and addresses a requirement of the LOFAR Pulsar
Pipeline.

The pulsar pipeline runs across multiple compute nodes, generating a series of
thumbnail plots on the local storage of each one -- that is, the plots are
only accessible by a process running on the compute node, and are not exported
via NFS or similar. The aim is to combine all the thumbnails on a given host
into a single image, using the ``montage`` command provided by `ImageMagick
<http://www.imagemagick.org/>`_. It is assumed that the thumbnails reside in
the same path on each node.

An initial implementation of the code which runs on each node was provided as
a ``ksh`` script.

.. code-block:: ksh
   :linenos:

   #!/bin/ksh

   #find all the th.png files and convert them into a list to paste together using "montage".
   
   if [ -f combined.th.png ]
   then
      echo "WARNING: deleting previous version of results: combined.th.png"
      rm combined.th.png
   fi
   
   find ./ -name "*.th.png" -print  > /tmp/$$_combine_col1.txt
   find ./ -name "*.th.png" -print  | sed -e 's/\// /g' -e 's/^.* //g' -e 's/.*_RSP/RSP/g' -e 's/\..*//g'  -e 's/_PSR//g' > /tmp/$$_combine_col2.txt
   paste /tmp/$$_combine_col1.txt /tmp/$$_combine_col2.txt | awk '{print "-label "$2" "$1" "}' | tr -d '\n' | awk '{print "montage -background none "$0" combined.th.png"}' > combine_png.sh
   rm /tmp/$$_combine_col1.txt /tmp/$$_combine_col2.txt
   wc_convert=`wc -l combine_png.sh | awk '{print $1}'`
   
   if [[ $wc_convert > 0 ]]
   then
      chmod 777 combine_png.sh
      echo "Executing the following comamnd: "
      cat combine_png.sh
      ./combine_png.sh
      echo ""
      echo "Results:  combined.th.png"
      echo ""
   else
      echo ""
      echo "No thumbnail (\*.th.png) files were found to combine."
      echo ""
   fi
   
   exit 0

Per-node script
===============

First, we will consider the processing that must be done on each of the remote
hosts. We start by converting the ``ksh`` script to a native Python version,
then refining it to best take advantage of the framework capabilities.

It may be worth emphasising that the conversion to Python is optional: an
alternative approach would be to run code each node which simply spawned a
copy of ``ksh`` and executed the script directly. In general, though,
minimising forking is a wise approach -- and the Python code provides better
opportunity to demosntrate the framework capabilities.

First Python implementation
---------------------------

A simple Python implementation of functionality similar to that provided by
the ``ksh`` script is shown below.

.. code-block:: python
   :linenos:

   import glob
   import subprocess
   import os
   
   def run(file_pattern, input_dir, output_file, clobber):
       # Returns 0 for success, 1 for faliure
   
       # Sanity checking
       if not os.path.exists(input_dir):
           return 1
       if os.path.exists(output_file):
           if clobber:
               os.unlink(output_file)
           else:
               return 1
   
       # Build list of input files
       input_files = glob.glob(os.path.join(input_dir, file_pattern))
   
       try:
           # Run "montage" command
           subprocess.check_call(['/usr/bin/montage'] + input_files + [output_file])
       except Exception, e:
           return 1
   
       return 0
   
Note the following:

- The Python version has been implemented as a function (``run``).

- Success or failure is indicated by the return value of the function: in true
  Unix fashion, ``0`` represents success.

- We allow the user to specify whether the output should be overwritten using
  the ``clobber`` argument.

- The user can also specify the pattern of filenames to be searched for (so
  this code can be more generic than the simple ``*.th.png`` in the ``ksh``
  version).

- Arguments also enable the user to specify both the directory to search for
  thumbnail files, and the directory into which the output file should be
  written.

- For simplicity, we have not implemented the logic used to add titles to the
  images (but extending the code to do so would be trivial).

- Standard Python code is used to implement all the required functionality,
  with no added complexity. In particular, Python's `subprocess
  <http://docs.python.org/library/subprocess.html>`_ module is used to spawn the
  ``montage`` command.

Using the :class:`~lofarpipe.support.lofarnode.LOFARnodeTCP` class
------------------------------------------------------------------

To integrate the Python code developed above into the framework, some minimal
changes are required. First, we take our ``run()`` function, and make it a
method of a class derived from
:class:`lofarpipe.support.lofarnode.LOFARnodeTCP`. Secondly, we add some
boilerplate such that when the script is run from the command line, it takes
three arguments, then instantiates the class we have defined and executes its
:meth:`~lofarpipe.support.lofarnode.LOFARnodeTCP.run_with_stored_arguments`
method. Note that the script then exits with the value returned by that
method. The result is shown below.

.. code-block:: python
   :linenos:

   import sys
   import subprocess
   import glob
   import os
   
   from lofarpipe.support.lofarnode import LOFARnodeTCP
   
   class thumbnail_combine(LOFARnodeTCP):
       def run(self, file_pattern, input_dir, output_file, clobber):
           # Returns 0 for success, 1 for faliure
   
           # Sanity checking checking
           if not os.path.exists(input_dir):
               return 1
           if os.path.exists(output_file):
               if clobber:
                   os.unlink(output_file)
               else:
                   return 1
   
           # Build list of input files
           input_files = glob.glob(os.path.join(input_dir, file_pattern))
   
           try:
               # Run "montage" command
               subprocess.check_call(['/usr/bin/montage'] + input_files + [output_file])
           except Exception, e:
               return 1
   
           return 0
   
   if __name__ == "__main__":
       jobid, jobhost, jobport = sys.argv[1:4]
       sys.exit(thumbnail_combine(jobid, jobhost, jobport).run_with_stored_arguments())

Logging
-------

Within the :class:`lofarpipe.support.lofarnode.LOFARnode` environment, we
now have access to some other framework-provided services. Chief amont these
is logging. The script is therefore updated to be more robust against failures
and to report progress to the logger.

.. code-block:: python
   :linenos:

   import sys
   import subprocess
   import glob
   import os
   
   from lofarpipe.support.lofarnode import LOFARnodeTCP
   
   class thumbnail_combine(LOFARnodeTCP):
       def run(self, file_pattern, input_dir, output_file, clobber):
           if not os.path.exists(input_dir):
               self.logger.error("Input directory (%s) not found" % input_dir)
               return 1
   
           self.logger.info("Processing %s" % input_dir)
   
           if os.path.exists(output_file):
               if clobber:
                   self.logger.warn(
                       "Deleting previous version of results: %s" % output_file
                   )
                   os.unlink(output_file)
               else:
                   self.logger.error(
                       "Refusing to overwrite existing file %s" % output_file
                   )
                   return 1
   
           input_files = glob.glob(os.path.join(input_dir, file_pattern))
   
           try:
               # Run "montage" command
               subprocess.check_call(['/usr/bin/montage'] + input_files + [output_file])
           except Exception, e:
               self.logger.error(str(e))
               return 1
   
           if not os.path.exists(output_file):
               self.logger.error(
                   "Output file %s not created by montage exectuable" % output_file
               )
               return 1
   
           return 0
   
   if __name__ == "__main__":
       jobid, jobhost, jobport = sys.argv[1:4]
       sys.exit(thumbnail_combine(jobid, jobhost, jobport).run_with_stored_arguments())


Note that ``self.logger`` in the above is an instance of
:class:`logging.logger` from the `Python standard library
<http://docs.python.org/library/logging.html>`_, with all the features that
implies. Any messages sent to the logger will be automatically integrated with
the overall pipeline logging system.

Helper functions
----------------

The pipeline framework provides some (entirely optional!) convenience
functions which can help the recipe author address common use cases.

The :func:`~lofarpipe.support.utilites.catch_segfaults` function, for example,
can automatically recover and re-run an external command in the event that it
results in a segmentation fault. This can be integrated into our existing
script as follows.

.. code-block:: python
   :linenos:

   import sys
   import glob
   import os
   
   from lofarpipe.support.lofarnode import LOFARnodeTCP
   from lofarpipe.support.utilities import catch_segfaults
   
   class thumbnail_combine(LOFARnodeTCP):
       def run(self, executable, file_pattern, input_dir, output_file, clobber):
           if not os.path.exists(input_dir):
               self.logger.error("Input directory (%s) not found" % input_dir)
               return 1
   
           self.logger.info("Processing %s" % input_dir)
   
           if os.path.exists(output_file):
               if clobber:
                   self.logger.warn(
                       "Deleting previous version of results: %s" % output_file
                   )
                   os.unlink(output_file)
               else:
                   self.logger.error(
                       "Refusing to overwrite existing file %s" % output_file
                   )
                   return 1
   
           input_files = glob.glob(os.path.join(input_dir, file_pattern))
   
           command_line = [executable] + input_files + [output_file]
           try:
               catch_segfaults(command_line, None, None, self.logger)
           except Exception, e:
               self.logger.error(str(e))
               return 1
   
           if not os.path.exists(output_file):
               self.logger.error(
                   "Output file %s not created by montage exectuable" % output_file
               )
               return 1
   
           return 0
   
   if __name__ == "__main__":
       jobid, jobhost, jobport = sys.argv[1:4]
       sys.exit(thumbnail_combine(jobid, jobhost, jobport).run_with_stored_arguments())

Note that we have also added the ``executable`` argument to define which
external command should actually be run. There is no reason to avoid making
the code as generic and reusable as possible!

At this point, our node script is complete (at least in this simple form). To
be useful, though, it needs to be executed across many different nodes as part
of a pipeline. This is where the *recipe* needs to be defined.

Defining the recipe
===================

As described in the :ref:`overview <framework-overview>`, a recipe is the
basic building block of pipelines: they describe how to perform an individual
unit of pipeline processing. In this case, our recipe will specify the inputs
for the node script we have written above, dispatch the jobs to a number
of compute nodes, and finally collect the results.

A basic recipe
--------------

All pipeline recipes ultimately derive from
:class:`lofarpipe.support.baserecipe.BaseRecipe`. A trivial example is shown
below.

.. code-block:: python
   :linenos:

   import sys
   from lofarpipe.support.baserecipe import BaseRecipe
   
   class thumbnail_combine(BaseRecipe):
       def go(self):
           self.logger.info("Starting thumbnail_combine run")
           super(thumbnail_combine, self).go()
           self.logger.info("This recipe does nothing")
   
   
   if __name__ == '__main__':
       sys.exit(thumbnail_combine().main())

This recipe does nothing except print a couple of lines to the log. However,
note the following key features:

- The control code for the recipe is all implemented within the ``go()``
  method of a class derived from
  :class:`lofarpipe.support.baserecipe.BaseRecipe`.

- Within that environment, we have access to a logger, which works in exactly
  the same way as it does on the node. (Enthusiasts may wish to note that this
  is actually an instance of
  :class:`lofarpipe.support.pipelinelogging.SearchingLogger`, but the practical
  difference is minimal).

- It is important to call the ``go()`` method of the superclass (as shown at
  line 7) to ensure all the necessary initialisation is performed.

- If called from the command line, we instantiate the object, call its
  ``main()`` method, and exit with its return value.

Dispatching remote jobs
-----------------------

One of the most fundamental aspects of the framework is its ability to
dispatch jobs to remote hosts, and this is absolutely necessary for the
problem under discussion. We can add this to the recipe as follows.

.. code-block:: python
   :linenos:

   import sys
   
   from lofarpipe.support.baserecipe import BaseRecipe
   from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
   from lofarpipe.support.remotecommand import ComputeJob
   
   class thumbnail_combine(BaseRecipe, RemoteCommandRecipeMixIn):
       def go(self):
           self.logger.info("Starting thumbnail_combine run")
           super(thumbnail_combine, self).go()
   
           # Hosts on which to execute
           hosts = ['lce019']
   
           # Path to node script
           command = "python %s" % (self.__file__.replace('master', 'nodes'))
   
           # Build a list of jobs
           jobs = []
           for host in hosts:
               jobs.append(
                   ComputeJob(
                       host, command,
                       arguments=[
                           "/usr/bin/montage",     # executable
                           "\*.th.png",            # file_pattern
                           "/path/to/png/files",   # input_dir
                           "/path/to/output.png",  # output_file
                           True                    # clobber
                       ]
                   )
               )
   
           # And run them
           self._schedule_jobs(jobs)
   
           # The error flag is set if a job failed
           if self.error.isSet():
               self.logger.warn("Failed compute job process detected")
               return 1
           else:
               return 0
   
   if __name__ == '__main__':
       sys.exit(thumbnail_combine().main())

This raises a number of relevant points to note.

- The distribution system is activated for a given recipe by "mixin-in" the
  :class:`~lofarpipe.support.remotecommand.RemoteCommandRecipeMixIn` class to
  its definition.

- In this case, we execute on only one remote host (``lce019``, as defined at
  line 13). However, as many as necessary could be defined.

- Each remote processing job is defined as an instance of
  :class:`~lofarpipe.support.remotecommand.ComputeJob`. It takes three
  arguments: the name of the host on which to execute, the name of the command
  to be run, and any arguments which should be passed to that command. These
  are provided in lines 23 to 30.

- The command to run can be any Python script. By convention, node scripts are
  named such that the name can be derived from the recipe name as shown at line
  16, but this is entirely up to the author.

- The arguments provided to
  :class:`~lofarpipe.support.remotecommand.ComputeJob` correspond exactly to
  those defined in the node script, above.

- After all the jobs have been defined, they are passed (as a list) to
  :meth:`~lofarpipe.support.remotecommand.RemoteCommandRecipeMixIn._schedule_jobs`.
  This blocks until all jobs have finished.

- If a job fails, the ``error`` attribute (an instance of
  :class:`threading.Event` from `Python's standard library
  <http://docs.python.org/library/threading.html>`_ is set.  The recipe should
  check for this and act appropriately.

Ingredients
-----------

The recipe shown in the previous section contains many hard-coded elements:
all the arguments to the compute job, the host on which to run, and so on.
This is obviously inflexible and undesireable. We can overcome this using the
*ingredients* system provided by the framework. An example is shown below.

.. code-block:: python
   :linenos:

   import sys
   
   import lofarpipe.support.lofaringredient as ingredient
   from lofarpipe.support.baserecipe import BaseRecipe
   from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
   from lofarpipe.support.remotecommand import ComputeJob
   
   class thumbnail_combine(BaseRecipe, RemoteCommandRecipeMixIn):
       inputs = {
           'executable': ingredient.ExecField(
               '--executable',
               default="/usr/bin/montage",
               help="montage executable"
           ),
           'file_pattern': ingredient.StringField(
               '--file-pattern',
               default="\*.th.png",
               help="File search pattern (glob)",
           ),
           'input_dir': ingredient.StringField(
               '--input-dir',
               help="Directory containing input files"
           ),
           'output_file': ingredient.StringField(
               '--output-file',
               help="Output filename"
           ),
           'clobber': ingredient.BoolField(
               '--clobber',
               default=False,
               help="Clobber pre-existing output files"
           ),
           'target_hosts': ingredient.ListField(
               '--target-hosts',
               help="Remote hosts on which to execute"
           )
       }
   
       def go(self):
           self.logger.info("Starting thumbnail_combine run")
           super(thumbnail_combine, self).go()
   
           hosts = self.inputs['target_hosts']
           command = "python %s" % (self.__file__.replace('master', 'nodes'))
           jobs = []
           for host in hosts:
               jobs.append(
                   ComputeJob(
                       host, command,
                       arguments=[
                           self.inputs['executable'],
                           self.inputs['file_pattern'],
                           self.inputs['input_dir'],
                           self.inputs['output_file'],
                           self.inputs['clobber']
                       ]
                   )
               )
           self._schedule_jobs(jobs)
   
           if self.error.isSet():
               self.logger.warn("Failed compute job process detected")
               return 1
           else:
               return 0
   
   if __name__ == '__main__':
       sys.exit(thumbnail_combine().main())

Using this system, the recipe author defines a list of inputs to the recipe.
Each input is an instance of a class descended from
:class:`lofarpipe.support.lofaringredients.Field`: the various sub-types of
field enable the user to perform sanity-checking of inputs. For example, in
the above, we can check that the executable provided really is an executable
by making the relevant field an instance of
:class:`~lofarpipe.support.lofaringredients.ExecField`, and that the
``clobber`` value is really a bool by making its field
:class:`~~lofarpipe.support.lofaringredients.BoolField`.  The
:ref:`developer's guide <lofarpipe-ingredients>` provides a lot more
information about the types of field available.

Each of the ingredients is associated with a name in the ``inputs`` dict.
Within the recipe, the values of the inputs are available as
``self.inputs[FIELDNAME]``, as seen (for example) at line 43.

The various inputs can take their values from a number of sources. For
example, as we will see, inputs can be read from the command line, provided in
a configuration file, or take the default value specified in their definition.
Whatever the source, though, they are always made available to the recipe in a
consistent way: a :class:`~~lofarpipe.support.lofaringredients.BoolField`
*always* contains a bool, and so on.

User-defined ingredients
------------------------

The ingredients system is designed to take care of as much error & sanity
checking for the developer as is possible. It is therefore extensible: as well
as checking for basic types as shown above, we can construct specialist fields
to (for example) check that a given input falls within a particular range.

In this case, we know that ``target_hosts`` should be a list of hostnames of
machines to which jobs may be dispatched. Above, we used
:class:`~lofarpipe.support.lofaringredients.ListField` to simply check that it
is a list. However, with a little imagination, we can define a list that is
guaranteed to contain only resolvable hostnames. For example:

.. code-block:: python
   :linenos:

   import lofarpipe.support.lofaringredient as ingredient

   class HostNameList(ingredient.ListField):
       @classmethod
       def is_valid(value):
           import socket
           for hostname in value:
               try:
                   socket.gethostbyname(hostname)
               except:
                   return False
           return True
  
This checks that every element within the list is resolveable (using Python's
standard :func:`socket.gethostbyname` function). We could incorporate it into
the above recipe by simply changing line 33 to:

.. code-block:: python

   'target_hosts': HostNameList(

Configuration file access
-------------------------

In the above, we have expected the user to supply a list of hosts to run jobs
on directly. However, in general the cluster layout is already known: this
can, therefore, be determined automatically.

As part of the :ref:`pipeline configuration <config-file>`, the user is able
to specify a ``clusterdesc`` parameter. This contains the full path to a file
which describes the cluster layout (see :ref:`the note on distproc
<distproc-blurb>` for details). The recipe can access the pipeline
configuration and extract the information from this file directly. We can
simply drop the ``target_hosts`` input from our recipe, and replace line 43
with:

.. code-block:: python

   from lofarpipe.support.clusterdesc import ClusterDesc, get_compute_nodes
   hosts = get_compute_nodes(
       ClusterDesc(
           self.config.get('cluster', "clusterdesc")
       )
   )

There are a number of points to note here.

The pipeline configuration file is available as the ``self.config``
attribute in the recipe. This is an instance of
:class:`ConfigParser.SafeConfigParser` from the `standard library
<http://docs.python.org/library/configparser.html>`_, and can be accessed
exactly as described in the Python documentation. Here, we simply extract the
value of ``clusterdesc`` from the ``cluster`` section.

The framework provides some convenience routines from working with clusterdesc
file. Here, we use :class:`lofarpipe.support.clusterdesc.ClusterDesc` and
:func:`~lofarpipe.support.clusterdesc.get_compute_nodes` to extract a list of
all the compute nodes defined in the cluster, and then proceed to use the list
of hosts in the recipe exactly as before.

Additional notes
================

Some important aspects of recipe design were not covered in the above
discussion.

Assigning jobs to specific hosts
--------------------------------

The example we have considered above is, in one important respect, simpler
than many pipeline recipes: it runs exactly the same code on each of the
remote hosts. A more general situation is processing a large number of
similar, but not identical, datasets (such as independent subbands of an
observation). Due to limited storage capacities on the remote hosts, it is
usually the case that each host only stores a subset of the total number of
datasets locally. Therefore, when dispatching jobs to the host, the recipe
author must be careful only to send jobs which refer to data it can reasonably
process.

From the recipe point of view, this procedure is straightforward. The recipe
developer earlier contains code like:

.. code-block:: python

   jobs = []
   for host in hosts:
       jobs.append(
           ComputeJob(
               host, command,
               arguments=[
                   ...
               ]
           )

When specifying a job which must run on a specific host, the pipeline author
can use a mapping of the form:

.. code-block:: python

   job_list = [
       ("hostname1", [arguments for job 1]),
       ("hostname2", [arguments for job 2]),
       ...
   ]

And our earlier code can then simply be modified to:

.. code-block:: python

   jobs = []
   for host, arguments in job_list:
       jobs.append(
           ComputeJob(
               host, command, arguments=arguments
           )

In general, the recipe author must define the mapping between hostnames and
job arguments themselves: this will depend on the details of the problem the
recipe is addressing. Often, it is conventient to use one recipe to generate
the mapping, then save it to disk for use by several recipes in the pipeline.
This is the approach taken in LOFAR's standard imaging pipeline. Here, the
:ref:`recipe-datamapper` recipe determines which filenames are accessible from
which hosts, and stores them to disk in a :ref:`parset file <parset-handling>`
formatted as follows:

.. code-block:: none

   hostname1 = [ /path/to/filename1, /path/to/filename2 ]
   hostname2 = [ /path/to/filename3, /path/to/filename3 ]
   ...

The :func:`lofarpipe.support.group_data.load_data_map` function makes it easy
to read back this parset from disk and iterate over the values to dispatch
compute jobs: see the imaging pipeline's :ref:`dppp-recipe` recipe for an
example.

.. todo::

   Recipe outputs

.. todo::

   Combining recipes into a pipeline

.. todo::

   Testing this recipe by running it
