==============
The SIP recipe
==============

The SIP recipe -- ``sip.py`` -- demonstrates the basic functions of an
imaging pipeline. In turn, it runs ``DPPP`` (data compression and flagging),
``BBS`` (calibration), ``MWimager`` (imaging) and a custom-developed source
finding step. The logs of all these steps are collected and stored centrally,
images (in ``CASA`` format) are made available, and results of the source
finding are stored in a central database.

This should form a model for how a pipeline can be constructed. However, it
does not contain logic for routines such as the "major cycle" (whereby
``BBS``, ``MWimager`` and the source finder will iterate to find an optimum
calibration). Such logic should be straightforward to add based on this
framework.

``sip.py``
----------

.. literalinclude:: ../../../pipeline/master/sip.py
