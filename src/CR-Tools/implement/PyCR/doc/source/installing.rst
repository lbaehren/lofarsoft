**********
Installing
**********
To install PyCR first follow the instructions at `the USG website <http://usg.lofar.org/wiki/doku.php?id=development:getting_started>`_ to install CR-Tools.
Then use::

    ccmake .

in the ``$LOFARSOFT/build/cr`` directory and enable ``CR_WITH_PYCR``.

Finally execute::

    cd $LOFARSOFT/build/cr
    make && make_install

