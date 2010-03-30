.. _recipe-docs:

*******
Recipes
*******

A pipeline is composed of a hierarchical tree of so-called *recipes*. Each
recipe takes a list of inputs, performs some processing on them, and
returns a list of outputs. The processing performed by the recipe may include
calls to further recipes. For example, the ``sip`` recipe, which forms the
basis for LOFAR's Standard Imaging Pipeline, calls other recipes to run
components like DPPP, BBS and the imager.

Recipes are derived from the :class:`LOFARrecipe` class, which provides some
standard functionality common to all recipes. In turn, :class:`LOFARrecipe`
inherits from Cuisine's :class:`WSRTrecipe`. Neither of these should be
instantiated directly.

In the "original" :mod:`Cuisine` framework, and in the extended LOFAR pipeline
version, the fundamental job of the recipe author is to extend the base class
by overriding the :meth:`go` method.


Example
=======

This section is designed to illustrate all the basic concepts required for
constructing a single-threaded recipe. That is, this recipe will run solely on
the cluster head node (as specified in your pipeline configuration; see
:ref:`infrastructure-setup`). The :ref:`parallelisation-docs` section will
show how this recipe can be extended to run in parallel across a cluster.

A basic recipe
--------------

.. literalinclude:: ../../../../recipes/master/dummy_echo.py
   :linenos:


This example recipe simple spawns a simple shell script and collects its
results. When run independently of the pipeline system, the script outputs:

.. code-block:: bash

  $ ./dummy_echo.sh test
  Starting script dummy_echo.sh at  Mon Mar 29 12:50:22 CEST 2010
  The number of files is:  1
  The file names are:  test
  These same file name can be echo'd one by one to STDOUT:
  test
  Ended script dummy_echo.sh successfully at  Mon Mar 29 12:50:22 CEST 2010

We will consider the recipe line by line.

Lines 1 and 2 are basic import statements. :mod:`subprocess` and :mod:`sys` are from
the Python standard library, and are not essential to the framework (but are
used elsewhere in this recipe). :class:`LOFARrecipe` is the superclass of all
pipeline recipes.

Line 4 starts the definition of our recipe. As described, it inherits from
:class:`LOFARrecipe`. Recipes are saved one-per-file, and the file name should
match the recipe name; ie, in this case, :class:`dummy_echo` is saved in
``dummy_echo.py`` in the ``recipes/master`` directory.

:meth:`__init__` (line 5) must first call the superclass constructor
(line 5). It may then perform other initialisation steps for the recipe.
Typically, this will include adding options to
:attr:`LOFARrecipe.optionparser`. All the arguments that a recipe requires
must be defined here, following the model seen in lines 7--10 above. Refer to
the Python standard library documentation for :class:`optparse.OptionParser`
the definition syntax. When the recipe is run, option values will be made
available in :attr:`LOFARrecipe.inputs`, which is accessible with a
dictionary-like syntax; ie, if it is not overriden with the recipe is called,
the above will result in:

  >>> dummy_echo().inputs['executable']
  /home/swinbank/sw/bin/dummy_echo.sh

Optionparser defaults may be overriden from the command line, by the code
which calls the recipe, or by a :ref:`task-definition`.

:meth:`go` (line 14) is the core of the recipe definition: this is
ultimately what is called when the recipe is run, and contains all the logic
determining what happens. It takes no arguments directly, but has access to
the :attr:`LOFARrecipe.inputs` input information, as shown above.

Line 15 illustrates the use of the :attr:`LOFARrecipe.logger`. This is an
instance of :class:`logging.Logger` from the Python standard library, and the
reader should acquaint themselves with the documentation available there for
details. In brief, a number of methods are defined which send messages of
varying degrees of severity to the log: :meth:`debug`, :meth:`info`,
:meth:`warn`, :meth:`critical` and :meth:`error`, in order of increasing
severity. By default, only messages of level ``warning`` and above are logged,
but this can be changed to ``info`` by providing the ``-v`` switch on recipe
invocation or ``debug`` by providing ``-d``. Logs are written to the console,
and also saved to the ``runtime_directory`` (see :ref:`infrastructure-setup`).

Line 16 calls the superclass definition of :meth:`LOFARrecipe.go`: this is
required at the start of all ``go()`` methods (but may occur after logging
etc, as shown).

Lines 18 and 19 construct the command line which will be executed by the
recipe. This is composed of ``self.inputs['executable']``, defined as above,
and ``self.inputs['args']``. This latter contains a list of any commany line
*arguments* (as opposed to options; see the :mod:`optparse` documentation)
supplied to the recipe. Thus, build a command string consisting of the
executable script name and any filenames specified on the command line.

Lines 20 and 21 use the Python :mod:`subprocess` module (refer to the
standard library documenation) to spawn the executable command line, wait for
it to finish, and collect any output it produced in the variables ``sout`` and
``serr``. These are then sent to the log (lines 23 and 24).

Lines 26 to 29 cause the method to finish, checking the return status of the
spawned process and returning 0 or 1 to indicated success or failure.

Lines 32 and 33 make it possible to execute the recipe from the command line;
see the following.

Command line execution
----------------------

All recipes can be independently executed from the command line. They require
one parameter, ``-j`` or ``--job-name``, which should uniquely identify the
pipeline run, in addition to any options or arguments required by the recipe.
For example, the code above can be executed as follows:

.. code-block:: bash

   $ python dummy_echo.py -j test_job -d foo bar
   2010-03-29 14:20:44 INFO    dummy_echo: stdout: Starting script dummy_echo.sh
   at  Mon Mar 29 14:20:44 CEST 2010
   The number of files is:  2
   The file names are:  foo bar
   These same file name can be echo'd one by one to STDOUT:
   foo
   bar
   Ended script dummy_echo.sh successfully at  Mon Mar 29 14:20:44 CEST 2010

   2010-03-29 14:20:44 INFO    dummy_echo: stderr: 
   2010-03-29 14:20:44 INFO    dummy_echo: recipe dummy_echo completed

.. _task-definition:

Task definition
---------------

The above recipe was very simple, with only one option -- and that option had
a sensible default. Often, however, recipes will be much more elaborate, and
have a much richer set of options. In thse cases, it may be useful to define
a *task*, which represents the combination of a recipe and a given set of
options.

As per :ref:`infrastructure-setup`, it is possible to define a ``task_files``
parameter in your pipeline configuration file. Each task you wish to define
corresponds to one stanza in the file. The stanza is headed with the name of
the task. The recipe should then be defined, followed by any required
arguments.

For example, a task definition for our :class:`dummy_echo` recipe could might
look like:

.. code-block:: none

   [dummy_echo_task]
   recipe = dummy_echo
   executable = /a/different/executable

This can then be invoked using the :meth:`LOFARrecipe.run_task` method in
another recipe. For instance:

.. code-block:: python

   def go():
       [.. lines elided ..]
       self.run_task("dummy_echo_task", ['foo', 'bar'])


API Reference
=============

Relevant Cuisine Classes
------------------------

.. class:: WSRTrecipe()

    :class:`WSRTrecipe` is the base class for recipes within the Cuisine
    framework. It provides some standard services which all recipes use. The
    relevant attributes defined are:

    .. attribute:: inputs
    .. attribute:: outputs

        Each :class:`WSRTrecipe` takes a series of inputs, performs some
        processing, and produces a series of outputs. These inputs and outputs are
        stored as instances of :class:`WSRTingredient`.

        The :attr:`inputs` attribute may be populated directly by a calling
        recipe, or, if this recipe is being invoked by the user, by parsing a
        command line. Command line options with are made available with a
        :class:`dict`-like interface; other ("file-name-like" arguments) are
        accessible as a :class:`list` under ``inputs["args"]``. Thus

        .. code-block:: bash

            $ python recipe.py --job-name foo file1 file2

        gives::

            >>> self.inputs['job_name']
            "foo"
            >>> self.inputs['args']
            ["file1", "file2"]

    .. attribute:: logger

        Every recipe has a logger, which is an instance of
        :class:`logging.Logger` from the Python standard library. The logger
        in child recipes is derived from that in the parent. 
        
        By default, a newly instantiated :attr:`WSRTrecipe.logger` sends to
        the console; however, recipes are encouraged to customise this as
        appropriate. Note that the Python :mod:`logging` module supports
        many different output formats, including text files, TCP sockets,
        syslog, SMTP, etc.

        The logger can be written to using the methods defined in the standard
        library. For example::

            self.logger.debug("This is a debug message")
            self.logger.info("This is an informational message")
            self.logger.warn("This is a warning message")

        By default, only warning and error messages are shown; see the
        documentation on :attr:`WSRTrecipe.optionparser`. Recipe authors are
        encouraged to provide copious debugging output!

    .. attribute:: optionparser

        Every recipe has a :attr:`~WSRTrecipe.optionparser`, which is an
        instance of :class:`optparse.OptionParser` from the Python standard
        library. At a minimum, every recipe supports three standard options
        defined in this :class:`WSRTrecipe`:

            -v, --verbose

                Increase the verbosity of the recipe's logger to output at
                level :attr:`logging.INFO`. This uses a callback function to
                adjust :attr:`WSRTrecipe.logger` directly, and is not
                otherwise stored; to find the verbosity level of a running
                pipeline, introspect the :attr:`WSRTrecipe.logger`::

                    if self.logger.level <= logging.DEBUG and self.logger.level != logging.NOTSET:
                        # ... we are in debug mode

            -d, --debug

                Increase the verbosity of the recipe's logger to output at
                level :attr:`logging.DEBUG`.

            -h, --help
                
                Print help and exit.

        The :attr:`~WSRTrecipe.optionparser` is used for validating all inputs
        to the recipe. Therefore, any additional inputs required by derived
        recipes should be declared in their own ``__init__()`` methods.


.. class:: WSRTingredient()

    :class:`WSRTingredient` supports all the features of a standard Python
    :class:`dict`.


LOFAR Extensions
----------------

.. class:: LOFARrecipe()

    :class:`LOFARrecipe` derives from :class:`WSRTrecipe`. It extends the
    set of standard options in a recipe, and adds some utility methods.

    .. attribute:: optionparser

        :attr:`LOFARrecipe.optionparser` provides all the options of
        :attr:`WSRTrecipe.optionparser`. In addition, the following are
        defined:

            -j, --job-name

                A unique identifier for the pipeline job currently being
                processed. This is usually based on e.g. the dataset being
                processed. Every recipe *must* have a job name; it is stored
                in :attr:`~WSRTrecipe.inputs` as ``job_name``.

            -r, --runtime-directory

                The location of the pipeline runtime directory. See
                :ref:`pipeline-layout` for more details. Every job must have a
                runtime directory, although it *may* be provided by a
                configuration file rather than on the command line. It is
                stored in :attr:`~WSRTrecipe.inputs` as ``runtime_directory``.

            -c, --config

                The location of the pipeline configuration file. Every recipe
                must have a configuration file, but it may be empty. If a
                configuration file is not specified in the inputs to a recipe,
                one will be search for in (1) the current working directory
                under the filename ``pipeline.cfg``, (2) the executing user's
                home directory, as ``~/.pipeline.cfg``. If neither of these
                exist, the recipe will raise :class:`PipelineException` and
                exit.

                It is stored in :attr:`~WSRTrecipe.inputs` as
                ``config``. See the :ref:`config-file` section for details on
                how this file can be accessed.

            -n, --dry-run

                If this option is specfied, :attr:`~WSRTrecipe.inputs` has
                ``dry_run`` set to ``True``. This may optionally be used by
                pipeline components to skip parts of their processing for test
                processes.

             --start-time

                *Expert use*. Pipeline logs, results, etc are stored according
                to pipeline start time. This option exists so that the various
                pipeline components can communicate the appropriate
                information between themselves. It may also be invoked on the
                command line, but this should only be done in exceptional
                circumstances.

    .. method:: go(self)

        :meth:`LOFARrecipe.go` performs some initialisation of the recipe. It
        should therefore not simply be ignored by derived classes, but called
        at the start of their own ``go()`` methods.

        :meth:`~LOFARrecipe.go` ensures that a job name is defined, that a
        configuration file is available (and defined in
        :attr:`~LOFARrecipe.inputs`), and that a runtime directory is
        available. It also checks for the existence of ``parset`` and ``gvds``
        keys in the :attr:`~LOFARrecipe.inputs`, and, if they exist but are
        empty, attemps to populate them based on the contents of the
        configuration file.

    .. method:: _get_cluster(self)

        Returns a tuple of (:class:`IPython.kernel.TaskClient`,
        :class:`IPython.kernel.MultiEngineClient`), connected to the IPython
        cluster specified in the recipe's configuration file. This is designed
        for internal use within the recipe.

.. class:: LOFARinput

    :class:`LOFARinput` is a specialised form of :class:`WSRTingredient`.
    It ensures that the ingredient always provides the required keys,
    ``job_name``, ``runtime_directory``, ``config``, ``dry_run`` and
    ``start_time``. These correspond to the options required by
    :class:`LOFARrecipe`, above.

.. class:: LOFARoutput

    :class:`LOFARoutput` matches :class:`LOFARinput` as the container for
    outputs of a recipe.


Invocation and return values
----------------------------

Pipeline recipes may indicate their successful (or otherwise) termination by
returning a value of 0 (for success) or an integer (failure). Beyond this,
standard meanings for return codes have not been defined: they should be
agreed between the callee and the caller.

When invoked from the command line, the result should recipe should be
accessed via its :meth:`main` method. 

When one recipe wishes to call another, it should use it's own
:meth:`cook_recipe` method. For instance::

    self.cook_recipe('recipe_name', inputs, outputs)
