.. _sip-quickstart:

==============
CEP Quickstart
==============

This brief guide should take you from scratch to an operational imaging
pipeline running on LOFAR CEP. It is still under development, and corrections
(preferably in the form of patches) are very welcome.

This guide assumes you already have a working installation of the pipeline
framework; please see the :ref:`framework documentation
<framework-quickstart>` for help with that.

If you will be using the LOFAR offline cluster, it's worth taking a moment to
ensure you have an SSH keychain that will enable you to log into all the
cluster nodes you need (lfe001, lfe002, lce0XX, lse0XX) without needing to
type a password: see the `LOFAR wiki
<http://www.lofar.org/operations/doku.php?id=public:ssh-usage>`_ for help.

Note that examples of many of the steps described here are available as part
of the example data provided with the pipeline framework (`available from
Subversion <http://usg.lofar.org/svn/code/trunk/src/pipeline/docs/examples/>`_ if
required).

For the sake of example, we will consider the ``L2009_16007`` dataset,
available on LOFAR subcluster 3. 

Set up your environment
-----------------------
Before starting, you shoud ensure that all
the ``LofIm`` packages are available in your environment.  The typical way
to add this package to ones' start up environment is to type the following
in the command line:

.. code-block:: bash

    $ use LofIm
     
or add this to the .bashrc or .cshrc files so that it is automatically 
added at login.  Note, there are some issues with paths when the daily build
fails.  To ensure that there are no problems accessing the LOFAR imaging software,
you may want to skip the ``use LofIm`` step and add ``/opt/LofIm/daily/lofar`` the 
paths explicitly to your environment. You will also need to add the
appropriate environment to the cluster setup in your configuration file: see
the :ref:`framework quickstart <pipeline-config>` for details.

Make a job directory
--------------------

This is dedicated to the output of a specific pipeline "job" -- that is, a run
with a given set of input data, pipeline configuration and so on. Note that
each job is given as specific, user-defined name, and the job directory should
be named after that. The name is entirely up to the user, but I generally use
the name of the input dataset with an appended number to differentiate
configurations. Hence, something like:

.. code-block:: bash

    $ mkdir -p ~/pipeline_runtime/jobs/L2009_16007_1

Prepare parsets describing how to run the pipeline components
-------------------------------------------------------------

Any individual pipeline component you plan to run that needs a parset -- such
as DPPP, BBS or the MWImager -- will need one provided by the pipeline
framework. Drop them into ``~/pipeline_runtime/jobs/L2009_16007_1/parsets``.

Note that some parameters will be added to the parset by the framework as part
of the run. At present, these are:

* NDPPP: ``msin``, ``msout``
* MWImager: ``dataset``

Prepare your task definitions
-----------------------------

Refer to the :ref:`framework overview <framework-overview>` and :ref:`recipe
documentation <task-definition>` for more on tasks; recall that they are
basically the combination of a recipe and a set of parameters. We will use the
:ref:`vdsreader-recipe` and :ref:`dppp-recipe` recipes, and define the following task:

.. code-block:: none

   [vdsreader]
   recipe = vdsreader
   gvds = %(runtime_directory)s/jobs/%(job_name)s/vds/%(job_name)s.gvds

   [ndppp]
   recipe = dppp
   executable = %(lofarroot)s/bin/NDPPP
   initscript = %(lofarroot)s/lofarinit.sh
   working_directory = %(default_working_directory)s
   parset = %(runtime_directory)s/jobs/%(job_name)s/parsets/ndppp.parset
   dry_run = False


Prepare a pipeline definition
-----------------------------

The pipeline definition specifies how data should flow through the pipeline.
It is a Python script, so you can use whatever logic you like to determine the
flow. For now, I suggest you keep it simple!

The :meth:`pipeline.master.control.run_task()` method is a shortcut to run the
specific recipe configurations specified in the configuration file; it takes a
configuration stanza and a list of datafiles as its input, and returns a list
of processed datafiles. More complex configurations are also possible, but
you'll have to define these by hand (ie, specifying the inputs and outputs of
the underlying recipe manually).

A very simple definition might be:

.. code-block:: python

    class sip(control):
       def pipeline_logic(self):
           with log_time(self.logger):
               datafiles = self.run_task("vdsreader")['data']
               datafiles = self.run_task("ndppp", datafiles)['data']

Here, the ``vdsreader`` task reads a list of filenames to be processed from a
VDS file, and then hands them to ``ndppp``. Note that the ``log_time``
context simply writes an entry to the log recording how long it all took.

Prepare a VDS file describing your data
---------------------------------------

A VDS file describes the location of all the datasets/measurement sets.  
Preparing the VDS file actually not strictly necessary: you can use the vdsreader task to
obtain a list of filenames to process (as above in sip.py run_task("vdsreader")), 
or you can specify them by hand -- just writing a list in a text file is fine, then parsing that and
feeding it to the DPPP task is fine. You need to specify the full path to each
measurementset, but don't need to worry about the specific hosts it's
accessible on. Note, you with the current cross-mount arrangement of the 
cluster compute and storage notes, you need to be on the **lce0XX** nodes in order 
to see the paths to the MS files.  A list that looks like

.. code-block:: python

    ['/net/sub3/lse007/data2/L2009_16007/SB1.MS', '/net/sub3/lse007/data2/L2009_16007/SB2.MS', ...]

is fine.  This method allows you the test the pipeline with a fewer set of
files than the typical set in its entirety.  In order to **run on a list of
files instead of running vsdreader**, the list would go into the sip.py file
as such (otherwise, use the above setting for datafiles of
run_task("vdsreader")):

.. code-block:: python

    class sip(control):
       def pipeline_logic(self):
           with log_time(self.logger):
               datafiles = ['/net/sub3/lse007/data2/L2009_16007/SB1.MS', '/net/sub3/lse007/data2/L2009_16007/SB2.MS']
               datafiles = self.run_task("ndppp", datafiles)


Anyway, assuming you want to go the VDS route, something like

For bash (on any imaging lce0XX node machine):

.. code-block:: bash

    $ ssh lce019
    $ mkdir /tmp/16007
    $ mkdir ~/pipeline_runtime/jobs/L2009_16007_1/vds/
    $ for storage in `seq 7 9`; do for file in /net/sub3/lse00$storage/data2/L2009_16007/\*MS; do /opt/LofIm/daily/lofar/bin/makevds ~/Work/pipeline_runtime/sub3.clusterdesc $file /tmp/16007/`basename $file`.vds; done; done
    $ /opt/LofIm/daily/lofar/bin/combinevds ~/pipeline_runtime/jobs/L2009_16007_1/vds/L2009_16007_1.gvds /tmp/16007/\*vds

For tcsh (on any imaging lce0XX node machine):

.. code-block:: tcsh

    $ ssh lce019
    $ mkdir /tmp/16007
    $ echo "for storage in "\`"seq 7 9"\`"; do for file in /net/sub3/lse00"\$"storage/data2/L2009_16007/\*MS; do /opt/LofIm/daily/lofar/bin/makevds ~/Work/pipeline_runtime/sub3.clusterdesc "\$"file /tmp/16007/"\`"basename "\$"file"\`".vds; done; done" > run.sh
    $ chmod 755 run.sh
    $ ./run.sh
    $ mkdir ~/pipeline_runtime/jobs/L2009_16007_1/vds/
    $ /opt/LofIm/daily/lofar/bin/combinevds ~/pipeline_runtime/jobs/L2009_16007_1/vds/L2009_16007_1.gvds /tmp/16007/\*vds


will do the trick.  Check to be sure that your global vds file was created
(``~/pipeline_runtime/jobs/L2009_16007_1/vds/L2009_16007_1.gvds``).  Clean up
the temporary location.

Run the pipeline
----------------

The pipeline can take a long time to process all subbands, especially if you
are running multiple passes of DPPP.  Since your loggin session with the head
node is likely to be cut off by an auto-logout, it is recommended that you use a
`screen <http://www.gnu.org/software/screen/manual/screen.html>`_ session when
running the pipeline, so that you can re-attach to the the session if you log
out before the pipeline is finished.
 
.. code-block:: bash

    $ cd ~/pipeline_runtime/jobs/L2009_16007_1/
    $ python sip.py -j L2009_16007_1 -d

The ``-d`` flag specifies debugging mode (ie, more logging). The ``-j``
argument just specifies the job we're running.  Intermediate pipeline files
are placed in your default_working_directory (in ``pipeline.cfg``);  results
are placed in the ``~/pipeline_runtime/jobs/L2009_16007_1/results`` directory;
logs are placed in the ``~/pipeline_runtime/jobs/L2009_16007_1/logs``
directory. ``DPPP`` leaves all the results in the default_working_directory;
if you do not run any additional pipeline tasks after ``DPPP``, there will be
no results directory created.  The pipeline log will indicate whether the
pipeline completed successfully.
