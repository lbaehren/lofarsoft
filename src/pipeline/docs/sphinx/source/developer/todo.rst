********************************
Potential framework enhancements
********************************

There are a number of areas in which the current version of the pipeline
framework could be enhanced. In no particular order, these include:

- An ingredients system for :class:`~lofarpipe.support.lofarnode.LOFARnode`,
  so that inputs are checked as they are supplied to nodes.

  - A consistent interface with
    :class:`~lofarpipe.support.baserecipe.BaseRecipe`, ie relying on an
    ``inputs`` dictionary, should also be supplied.

  - Unfortunately, this can't be used to validate inputs before starting the
    recipe, for obvious reasons.

- Ingredients probably shouldn't be provided with default values. At least,
  not default values for things like paths to executables which may sometimes
  disappear.

- Error handling throughout should be cleaned up; more exceptions should be
  caught and better feedback provided to the user.

- Configuration of the :ref:`logging system <lofarpipe-logging>` should be
  made more flexible.

- Rather than checking exit status, the framework should use a more-Pythonic
  error handling system based on exceptions.

- For consistency with the rest of the LOFAR system, :ref:`parsets
  <parset-handling>` should be used for all configuration information.

  - That, in turn, means that the parset format should be standardised.

- A global job queue per node should be implemented, so that multiple
  simultaneous recipes can submit jobs without overloading nodes.

- :meth:`lofarpipe.support.remotecommand.RemoteCommandRecipeMixIn.__schedule_jobs`
  should be non-blocking.

- :meth:`lofarpipe.support.baserecipe.BaseRecipe.go` and
  :meth:`lofarpipe.support.lofarnode.LOFARnodeTCP.run` should be made
  consistent, in terms of both name and semantics.

- The logging system should interface more directly with ``log4cplus``, rather
  than just reading log information from a file.

- More detailed feedback and quality-check information should be sent to the
  rest of the LOFAR system.

  - This is an issue which requires changes throughout the LOFAR system; it
    can't be addressed by the framework alone.

- The ``recipe_directories`` option should be removed from the
  :ref:`configuration file`, and the regular Python import mechanism, with
  appropriate namespacing, should be used to locate recipes.

- The dependency on :mod:`lofar.parameterset` should be removed, so that the
  framework is a stand-alone codebase and can more easily be ported to other
  systems.
