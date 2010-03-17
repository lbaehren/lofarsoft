==========
Quickstart
==========

This brief guide should take you from scratch to an operational **imaging
pipeline**. It is still under development, and corrections (preferably in the
form of patches) are very welcome.

Before starting, you should ensure that your ssh keychain password has been 
set up so that you are able to log into all the cluster head nodes (lfe001, 
lfe002), compute nodes (lce0XX) and storage nodes (lse0XX) without having to 
type your password.  Please see the 
`LOFAR Imaging Cookbook <http://http://www.mpa-garching.mpg.de/~fdg/LOFAR_cookbook/>`_ 
for instructions on how to set up your ssh keychain passwords.

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
you may want to skip the "use LofIm" step and add "/opt/LofIm/daily/lofar" the 
paths explicitly to your environment.

Additionally, the following environment variables need to be set in order to run 
the pipeline.

For bash (**without LofIm**):

.. code-block:: bash

    export PYTHONPATH=/opt/pipeline/dependencies/lib/python2.5/site-packages:/opt/pipeline/pipeline_snapshot/lib/python2.5/site-packages:$PYTHONPATH:.
    export LD_LIBRARY_PATH=/opt/pipeline/dependencies/lib:/data/sys/opt/lofar/external/log4cplus/lib:$LD_LIBRARY_PATH
    export PATH=/opt/pipeline/dependencies/bin:$PATH
    
For bash (**with LofIm**):

.. code-block:: bash

    export PYTHONPATH=/opt/LofIm/daily/lofar/lib/python2.5/site-packages:/opt/LofIm/daily/pyrap/lib:/opt/pipeline/dependencies/lib/python2.5/site-packages:/opt/pipeline/pipeline_snapshot/lib/python2.5/site-packages:$PYTHONPATH:.
    export LD_LIBRARY_PATH=/opt/LofIm/daily/lofar/lib:/opt/LofIm/daily/pyrap/lib:/opt/LofIm/daily/casarest/lib:/opt/hdf5/lib:/opt/LofIm/daily/casacore/lib:/opt/pipeline/dependencies/lib:/data/sys/opt/lofar/external/log4cplus/lib:$LD_LIBRARY_PATH
    export PATH=/opt/LofIm/daily/lofar/bin:/opt/LofIm/daily/askapsoft/bin:/opt/LofIm/daily/casarest/bin:/opt/LofIm/daily/casacore/bin:/opt/pipeline/dependencies/bin:$PATH
    

For tcsh (**without LofIm**):

.. code-block:: tcsh

    setenv PYTHONPATH /opt/pipeline/dependencies/lib/python2.5/site-packages:/opt/pipeline/pipeline_snapshot/lib/python2.5/site-packages:${PYTHONPATH}:.
    setenv LD_LIBRARY_PATH /opt/pipeline/dependencies/lib:/data/sys/opt/lofar/external/log4cplus/lib:${LD_LIBRARY_PATH}
    setenv PATH /opt/pipeline/dependencies/bin:${PATH}

For tcsh (**with LofIm**):

.. code-block:: tcsh

    setenv PYTHONPATH /opt/LofIm/daily/lofar/lib/python2.5/site-packages:/opt/LofIm/daily/pyrap/lib:/opt/pipeline/dependencies/lib/python2.5/site-packages:/opt/pipeline/pipeline_snapshot/lib/python2.5/site-packages:${PYTHONPATH}:.
    setenv LD_LIBRARY_PATH /opt/LofIm/daily/lofar/lib:/opt/LofIm/daily/pyrap/lib:/opt/LofIm/daily/casarest/lib:/opt/hdf5/lib:/opt/LofIm/daily/casacore/lib:/opt/pipeline/dependencies/lib:/data/sys/opt/lofar/external/log4cplus/lib:${LD_LIBRARY_PATH}
    setenv PATH /opt/LofIm/daily/lofar/bin:/opt/LofIm/daily/askapsoft/bin:/opt/LofIm/daily/casarest/bin:/opt/LofIm/daily/casacore/bin:/opt/pipeline/dependencies/bin:${PATH}
    

Make a runtime directory
------------------------

This will hold pipeline configuration, logs, results, etc. It should be
writeable to by all the nodes. Your home directory is probably a good place.

.. code-block:: bash

    $ mkdir  ~/pipeline_runtime

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

Prepare a clusterdesc file
--------------------------

This must specifically list the compute nodes and head nodes for the cluster. Something like:

.. code-block:: none

    $ more ~swinbank/Work/pipeline_runtime/sub3.clusterdesc 
    ClusterName = sub3
    HeadNode = lfe001
    ComputeNodes = [ lce019, lce020, lce021, lce022, lce023, lce024, lce025, lce026, lce027 ]

Save this in your runtime directory.

.. code-block:: bash

    $ mv sub3.clusterdesc ~/pipeline_runtime

Prepare parsets describing how to run the pipeline components
-------------------------------------------------------------

Any individual pipeline component you plan to run that needs a parset -- such
as DPPP, BBS or the MWImager -- will need one provided by the pipeline
framework. Drop them into ``~/pipeline_runtime/jobs/L2009_16007_1/parsets``.

Note that some parameters will be added to the parset by the framework as part
of the run. At present, these are:

* IDPPP: ``msin``, ``msout``
* MWImager: ``dataset``

Prepare a pipeline configuration file
-------------------------------------

The template in ``/opt/pipeline/pipeline.cfg`` is a good place to start. You
will need to customise:

- ``runtime_directory``: this refers to the location you have just created to store the pipeline information (~/pipeline_runtime)
- ``default_working_directory``: this is where temporary files, intermediate data products, etc are written on the compute nodes. It will be created for you, but obviously needs to be in a location you have write access to. An example location is /data/scratch/[username].
- ``clusterdesc``: this should point to your newly created and edited cluster description file (i.e. ~/pipeline_runtime/sub3.clusterdesc).
- If you plan to run BBS, you will also need to edit the ``[bbs]`` stanza to reflect the correct database name etc.  You can refer to the `LOFAR Imaging Cookbook <http://http://www.mpa-garching.mpg.de/~fdg/LOFAR_cookbook/>`_ for instructions on setting up the database to run BBS. 

Note that the various stanzas in the configuration file reflect a particular
set of configuration parameters for the various pipeline recipes. For
instance, the stanza ``[dppp_pass1]`` describes how to run the DPPP recipe with
one set of parameters; ``[dppp_pass2]`` describes how to run the *same* recipe
with *different* parameters.

You can save this as ``~/.pipeline.cfg``, and it will be used as the default for
all pipeline runs. Alternatively, you can drop it into the job directory for a
specific run. Let's do that here:

.. code-block:: bash

    $ mv pipeline.cfg ~/pipeline_runtime/jobs/L2009_16007_1

Prepare a pipeline definition
-----------------------------

The pipeline definition specifies how data should flow through the pipeline.
It is a Python script, so you can use whatever logic you like to determine the
flow. For now, I suggest you keep it simple!

A model pipe is available at ``/opt/pipeline/scripts/sip.py``. Your 
sip.py scripts should be placed in the job directory for a specific run:

.. code-block:: bash

    $ cp /opt/pipeline/scripts/sip.py ~/pipeline_runtime/jobs/L2009_16007_1

The
:meth:`pipeline.master.control.run_task()` method is a
shortcut to run the specific recipe configurations specified in the
configuration file; it takes a configuration stanza and a list of datafiles as
its input, and returns a list of processed datafiles. More complex
configurations are also possible, but you'll have to define these by hand (ie,
specifying the inputs and outputs of the underlying recipe manually).

A very simple definition would be:


.. code-block:: python

    class sip(control):
       def pipeline_logic(self):
           with log_time(self.logger):
               datafiles = self.run_task("vdsreader")
               datafiles = self.run_task("dppp_pass1", datafiles)

Here, the ``vdsreader`` task reads a list of filenames to be processed from a
VDS file, and then hands them to ``dppp_pass1``. Note that the ``log_time``
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

is fine.  This method allows you the test the pipeline with a fewer set of files than the typical set in its entirety.  In order to **run on a list of files instead of running vsdreader**, the list would go into the sip.py file as such (otherwise, use the above setting for datafiles of run_task("vdsreader")):

.. code-block:: python

    class sip(control):
       def pipeline_logic(self):
           with log_time(self.logger):
               datafiles = ['/net/sub3/lse007/data2/L2009_16007/SB1.MS', '/net/sub3/lse007/data2/L2009_16007/SB2.MS']
               datafiles = self.run_task("dppp_pass1", datafiles)


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


will do the trick.  Check to be sure that your global vds file was created (~/pipeline_runtime/jobs/L2009_16007_1/vds/L2009_16007_1.gvds).  Clean up the temporary location.

Start your engines
------------------

Various recipes use the IPython system for distributing jobs over the cluster.
We need to start IPython engines on all the compute machines, and an IPython
controller on the front end. This is done using a tool called Fabric. It, in
turn, reads instructions from /opt/pipeline/scripts/fabfile.py (or you can write your
own, if you prefer).

Unfortunately, the syntax here is a little unwieldy. Both these commands should 
be run on the head node lfe001, even though the commands start up with the head 
and compute nodes.  Something like

.. code-block:: bash

    $ cd /opt/pipeline/scripts
    $ fab head_node:~/pipeline_runtime/jobs/L2009_16007_1/pipeline.cfg start_controller:~/pipeline_runtime/jobs/L2009_16007_1/pipeline.cfg
    $ fab compute_nodes:~/pipeline_runtime/jobs/L2009_16007_1/pipeline.cfg start_engine:~/pipeline_runtime/jobs/L2009_16007_1/pipeline.cfg

should do the trick. Yes, repeating the name of the configuration file four
times is annoying -- patches welcome!  There maybe errors regarding "log4cplus"
when starting the engines, which can be ignored;  in general, the STDOUT is as 
follows for the head_node engine start up:

.. code-block:: none

	Debug: registered context Global=0
	log4cplus:ERROR No appenders could be found for logger (LCS.Common.EXCEPTION).
	log4cplus:ERROR Please initialize the log4cplus system properly.
	[lfe001] run: bash /opt/pipeline/pipeline_snapshot/bin/ipcontroller.sh ~/pipeline_runtime start /opt/pipeline/dependencies/lib/python2.5/site-packages:/opt/pipeline/pipeline_snapshot/lib/python2.5/site-packages

	Done.
	Disconnecting from lfe001... done.



Wait a moment
-------------

If the cluster is heavily loaded, it can take a minute or two for all the
engines to connect to the controller. Pause to give them chance to catch up.
Using "top", you should be able to see the process called "ipcontoller" on the 
head node and compute nodes;  this indicates that Fabric has started all the
pipeline controllers correctly.

Run the pipeline
----------------

The pipeline can take a long time to process all subbands, especially if you are
running multiple passes of DPPP.  Since your loggin session with the head node is
likely to be cut off by an auto-logout, we recommend that you use a VNC or 
`screen <http://www.gnu.org/software/screen/manual/screen.html>`_
session when running the pipeline, so that you can re-attach to the the session if
you log out before the pipeline is finished.
 
.. code-block:: bash

    $ cd ~/pipeline_runtime/jobs/L2009_16007_1/
    $ python sip.py -j L2009_16007_1 -d

The ``-d`` flag specifies debugging mode (ie, more logging). The ``-j``
argument just specifies the job we're running.  Intermediate pipeline files are placed in your default_working_directory (in pipeline.cfg);  results are placed in the ~/pipeline_runtime/jobs/L2009_16007_1/results directory;  logs are placed in the ~/pipeline_runtime/jobs/L2009_16007_1/logs directory.  DPPP leaves all the results in the default_working_directory;  if you do not run any additional pipeline tasks after DPPP, there will be no results directory created.  The pipeline log will indicate whether the pipeline completed successfully.

When you're done, clean up your engines
---------------------------------------

.. code-block:: bash

    $ cd /opt/pipeline/scripts
    $ fab compute_nodes:~/pipeline_runtime/jobs/L2009_16007_1/pipeline.cfg stop_engine:~/pipeline_runtime/jobs/L2009_16007_1/pipeline.cfg
    $ fab head_node:~/pipeline_runtime/jobs/L2009_16007_1/pipeline.cfg stop_controller:~/pipeline_runtime/jobs/L2009_16007_1/pipeline.cfg

(Of course, you can use the same engines for multiple pipeline runs, so you
might not want to do this right away).

