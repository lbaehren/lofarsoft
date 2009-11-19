=====================
Constructing a recipe
=====================

A pipeline is composed of a hierarchical tree of so-called *recipes*. Each
recipe takes a list of inputs, performs some processing on them, and
returns a list of outputs. The processing performed by the recipe may include
calls to further recipes. For example, the ``sip`` recipe, which forms the
basis for LOFAR's Standard Imaging Pipeline, calls other recipes to run
components like DPPP, BBS and the imager.

Recipes are derived from the :class:`LOFARrecipe` class, which provides some
standard functionality common to all recipes. In turn, :class:`LOFARrecipe`
inherits from Cuisine's :class:`WSRTrecipe`. Neither of these should be
instantiated directly [#f1]_. 

In the "original" :mod:`Cuisine` framework, and in the extended LOFAR pipeline
version, the fundamental job of the recipe author is to extend the base class
by overriding the :meth:`go` method.

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
        recipes should be declared in their own ``__init__()`` methods. See
        the :ref:`recipe-example` below.


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
                must have a configuration file, but it may be empty. If one is
                not provided by the user, it is inferred from the location of
                the module::

                  from pipeline import __path__ as config_path

                It is stored in :attr:`~WSRTrecipe.inputs` as
                ``config``.

            -d, --dry-run

                If this option is specfied, :attr:`~WSRTrecipe.inputs` has
                ``dry_run`` set to ``True``. This may optionally be used by
                pipeline components to skip parts of their processing for test
                processes.

    .. method:: go(self)

        :meth:`LOFARrecipe.go` performs some initialisation of the recipe. It
        should therefore not simply be ignored by derived classes, but called
        at the start of their own ``go()`` methods. See the
        :ref:`recipe-example` below.

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

    .. method:: _input_or_default(self, key, section=None)

        If ``self.inputs[key]`` exists but isn't populated, attempt to load it
        from the recipe's configuration file. If ``section`` is specified,
        look in that section of the file; otherwise, the section is given by
        the recipe name.


.. class:: LOFARingredient

    :class:`LOFARingredient` is a specialised form of :class:`WSRTingredient`.
    It ensures that the ingredient always provides the required keys,
    ``job_name``, ``runtime_directory``, ``config`` and ``dry_run``.


.. _recipe-example:

Example
-------

A trivial recipe could be constructed as follows::

    from pipeline.support.lofarrecipe import LOFARrecipe

    class NewRecipe(LOFARrecipe):
        def __init__(self):
            super(NewRecipe, self).__init__()
            self.optionparser.add_option('--dummy')

        def go(self):
            super(NewRecipe, self).go()
            self.outputs['dummy'] = self.inputs['dummy']

.. rubric:: Footnotes

.. [#f1] In an ideal world, these would be implemented as Abstract Base
  Classes, so they *cannot* be instantiated. However, this is precluded by the
  necessity of Python 2.5 compatibility.


