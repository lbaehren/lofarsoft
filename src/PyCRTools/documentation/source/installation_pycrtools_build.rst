Now we are ready to install PyCRTools itself. First get the latest
source code from the LOFAR USG Subversion repository::

  svn co http://usg.lofar.org/svn/code/trunk lofarsoft

Then add the following line to your ``.bashrc`` or ``.bash_profile``::

  export LOFARSOFT=/path/to/lofarsoft
  . $LOFARSOFT/devel_common/scripts/init.sh

Don't forget to restart/resource your shell or problems will arise along the
way, as your shell environment must be aware of the various configuration
variables.

Bootstrap the build system::

  cd $LOFARSOFT
  ./bootstrap

Then run::

  cd build

The CasaCore package needs to build from the latest SVN
revision. Therefor this needs some extra tweaking of the CMake
settings. To do this run::

  cmake -DCASACORE_FROM_LATEST_SVN_REVISION=ON .
  make rebuild_cache

Compile the PyCRTools::

  make pycrtools
