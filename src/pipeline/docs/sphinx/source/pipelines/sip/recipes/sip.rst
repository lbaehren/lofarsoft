==============
The SIP recipe
==============

.. todo::

   Check if this section is up to date.

There is no single SIP recipe: an imaging pipeline should be composed of
components as required. However, various examples are available to help.

``sip.py``
----------

.. todo::

   Provide simpler example SIP.

This recipe demonstrates the basic functionality of an imaging pipeline. In
turn, it runs ``DPPP`` (data compression and flagging), ``BBS`` (calibration),
``MWimager`` (imaging) and a custom-developed source finding step. The logs of
all these steps are collected and stored centrally, images (in ``CASA``
format) are made available.

This should form a model for how a pipeline can be constructed. However, it
does not contain logic for routines such as the "major cycle" (whereby
``BBS``, ``MWimager`` and the source finder will iterate to find an optimum
calibration). Such logic should be straightforward to add based on this
framework.

.. literalinclude:: ../../../../../examples/definition/sip2/sip.py

``tasks.cfg``
-------------

.. todo::

   Check task file for completenes/correctness.

This task file defines the tasks referred to in the above example.

.. literalinclude:: ../../../../../examples/definition/sip2/tasks.cfg
