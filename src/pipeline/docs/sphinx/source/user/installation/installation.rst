.. _framework-installation:

**********************
Framework installation
**********************

**Note**: These instructions were developed and tested using Ubuntu 10.04
Server. They assume the user is using the ``bash`` shell.  Adjustment for
other systems should be straightforward.

Before starting, you will need to install the ``lofar.parameterset`` Python
module. This must be available on your ``$PYTHONPATH``. Check by ensuring that
you can replicate the following command:

.. code-block:: bash

  $ python -c 'import lofar.parameterset ; print "ok"'
  ok

The latest version of the framework is available by Subversion from the `USG
repository <http://usg.lofar.org/>`_. Obtain a snapshot as follows:

.. code-block:: bash

  $ svn export http://usg.lofar.org/svn/code/trunk/src/pipeline/ ~/pipeline_framework

This will create a ``pipeline_framwork`` directory within your current working
directory. That directory contains a number of subdirectories. Note first
``docs``: this contains the source for the pipeline documentation. If you have
`Sphinx <http://sphinx.pocoo.org/>`_ installed, you can run ``make`` in that
directory to generate the documentation tree.

The framework itself is a Python package named ``lofarpipe``. It is found in
the ``framework`` directory, and may be installed using the setup.py script
included. The output directory can be specified using the ``--prefix``
option; in the example below, we install to ``/opt/pipeline/framework``.

.. code-block:: bash

  $ cd ~/pipeline_framework/framework
  $ sudo python setup.py install --prefix=/opt/pipeline/framework
  running install
  running build
  running build_py
  [... many lines elided ...] 
  running install_egg_info
  Writing /opt/pipeline/framework/lib/python2.6/site-packages/Pipeline-0.1.dev-py2.6.egg-info

After installation, ensure that the relevant ``site-packages`` directory
appears on your ``$PYTHONPATH`` environment variable:

.. code-block:: bash

  $ export PYTHONPATH=$PYTHONPATH:/opt/pipeline/framework/lib/python2.6/site-packages/

You may wish to add this to your shell startup sequence.

The pipeline also comes with a collection of recipes in the
``pipeline_framework/recipes`` directory. These are not required by the
framework itself, but will be useful for building pipelines. Ensure the
contents of this directory are conveniently accessible:

.. code-block:: bash

  $ sudo cp -r ~/pipeline_framework/recipes /opt/pipeline

At this point, the basic framework code should be installed. The next step is
to start running simple recipes: see the :ref:`running-basic` section for
details.
