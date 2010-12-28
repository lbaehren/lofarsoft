.. _sip:

*****************************
The Standard Imaging Pipeline
*****************************

The Standard Imaging Pipeline (or SIP) will accept raw data from the LOFAR
correlator, pre-process it, calibrate it, image it, and update the sky model
with the sources detected in the data. This section describes the components
of the SIP and how they fit together.

It should be emphasised that, at least during development stages, there is not
a single, ideal imaging pipeline. Instead, the user is encouraged to compose a
pipeline which meets their needs by selecting their preferred combination of
individual recipes.

Note that none of the SIP recipes make use of IPython, so configuration and
usage should be relatively simple.

.. toctree::
   :maxdepth: 2

   quickstart/index.rst
   recipes/index.rst
