.. _plotting:

**************
Basic plotting
**************

In order to plot the data we use the :mod:`matplotlib` package. This
can be loaded by using the following command::

    >>> import matplotlib.pyplot as plt

however, this should have already been done when loading the
:mod:`pycrtools`. Besides the plotting commands provided by
:mod:`matplotlib`, this plotting functionality is also available from
the :class:`hArrays`.


Matplotlib
==========

.. [under construction]

Depending on the system you are working on you may have to create a
window in which the plots are shown. This is done by::

    >>> plt.show()

Now we can use the following plotting commands to show the average
spectra for two antennas::

    >>> frequencies = datafile["FREQUENCY_DATA"].setUnit("M","")
    >>> for i in range(2):
    >>>     plt.subplot(1,2,i)
    >>>     plt.title("Average spectrum for two antennas")
    >>>     plt.semilogy(frequencies.vec(), avspectrum[0].vec())  #plt.semilogy(frequencies.vec(), avspectrum[i].vec())
    >>>     plt.xlabel(frequencies.getKey("name")+" ["+frequencies.getUnit()+"]")
    >>>     plt.ylabel(avspectrum.getKey("name")+" ["+avspectrum.getUnit()+"]")

Note that the array entries need to be converted to vectors (using the
:func:`vec` method) when they are given as arguments to the plotting
command.

To plot the time series of the entire dataset, we first read in all
samples from all antennas::

    >>> datafile["BLOCK"] = 0
    >>> datafile["BLOCKSIZE"] = maxblocksize
    >>> timeall = datafile["TIME_DATA"]
    >>> fxall = datafile["TIMESERIES_DATA"]

and then we plot it::

    >>> plt.subplot(1,2,2)
    >>> plt.title("Time series of antenna 0")
    >>> plt.plot(timeall.vec(), fxall[0].vec())
    >>> plt.xlabel("Time [$\\mu$s]")
    >>> plt.ylabel("Electric Field [ADC counts]")

.. [resulting plot of the code above]

If you want to plot a linear plot use :func:`plt.plot`, for a loglog
plot you use :func:`plt.loglog` and for a log-linear plot you can use
:func:`plt.semilogx` or :func:`plt.semilogy`, depending on which axis
needs to be logarithmic.


Plotting using the :class:`hArray` plotting methods
===================================================

Another way of producing plots is to use the plot method of the
:class:`hArray` class::

    >>> avspectrum.par.xvalues = frequencies
    >>> avspectrum.par.title = "Average spectrum"
    >>> avspectrum[0].plot(logplot="y")

.. [resulting plot of the code above]

This creates a semilog-plot with appropriate labels and units (if
provided beforehand).

You can either provide the parameters directly (has precedence), or
set the plotting parameters and attributes to the :class:`par` class
of the array, e.g.::

   >>> array.par.xvalues = x_vector
   >>> array.plot()

If the array is in looping mode, multiple curves are plotted in a
single window. Hence,::

   >>> avspectrum.par.logplot = "y"
   >>> avspectrum[...].plot(legend=datafile.antennas)

.. [resulting plot of the code above]

will simply plot all spectra of all antennas (= highest array index)
in the array.

The available parameters, used in the :class:`hArray.par` class, are:

* ``xvalues``: An array with corresponding x values. If ``None``, integer numbers from 0 to the length of the array are used.
* ``xlabel``: The x-axis label. If not specified, use the ``name`` keyword of the array. Units are added automatically.
* ``ylabel``: The y-axis label. If not specified, use the ``name`` keyword of the array. Units are added automatically.
* ``xlim``: Tuple with minimum and maximum values for the x-axis.
* ``ylim``: Tuple with minimum and maximum values for the y-axis.
* ``title``: A title for the plot
* ``clf``: If ``True`` (default) clear the screen before plotting. If
  ``False`` plots are composed with multiple lines from different
  arrays.
* ``logplot``: Can be used to make log-log or semi-log plots:

  "x" -> semilog plot in x

  "y" -> semilog plot in y

  "xy" -> loglog plot


You can either provide the parameters directly (has precedence), or
set the plotting parameters and attributes to the :class:`par` class
of the array, e.g.::

   >>> array.par.xvalues = x_vector
   >>> array.plot()

If the array is in looping mode, multiple curves are plotted in a
single window. Hence,::

   >>> avspectrum.par.logplot = "y"
   >>> avspectrum[...].plot(legend=datafile.antennas)

.. [resulting plot of the code above]

will simply plot all spectra of all antennas (= highest array index)
in the array.

The available parameters, used in the :class:`hArray.par` class, are:

  =============== ==================================================
  ``xvalues``     An array with corresponding x values.
                  If ``None``, integer numbers from 0 to the length of the array are used.
  ``xlabel``      The x-axis label.
                  If not specified, use the ``name`` keyword of the array.
                  Units are added automatically.
  ``ylabel``      The y-axis label.
                  If not specified, use the ``name`` keyword of the array.
                  Units are added automatically.
  ``xlim``        Tuple with minimum and maximum values for the x-axis.
  ``ylim``        Tuple with minimum and maximum values for the y-axis.
  ``title``       A title for the plot
  ``clf``         If ``True`` (default) clear the screen before plotting.
                  If ``False`` plots are composed with multiple lines from different arrays.
  ``logplot``     Can be used to make log-log or semi-log plots:

                  * "x" -> semilog plot in x
                  * "y" -> semilog plot in y
                  * "xy" -> loglog plot
  =============== ==================================================
