.. _crdatabase:

***********
CR Database
***********

The :mod:`crdatabase` module is used to read and/or update event information.
It can be imported by the following statement:

  >>> from pycrtools import crdatabase

The event information is organized in various nested information objects.

* Event information (:class:`Event`)

  This contains the basic event information, such as the timestamp and the
  processing status of the event. It also includes the event information found
  by LORA.  Each event is linked to at least one datafile information
  object.

* Datafile information (:class:`Datafile`)

  This contains the information of a single data file, such as the file name and
  processing status, that is linked to an event. Each datafile is linked to one
  station information file.

* Station information (:class:`Station`)

  The station information object contains information about the station, such as
  the station name and processing status. Each station contains links to various
  polarization information objects.

* Polarization information (:class:`Polarization`)

  The polarization information object contains event information for a single
  polarization direction for a station in an event. It contains an extensive
  list of polarization specific parameters of the event.


Opening the database
====================

Opening of a CR database is done by initiating a :class:`CRDatabase` object

    >>> dbm = crdatabase.CRDatabase('dbfilename.db')

which returns an object with which the database can be managed.

By default it is assumed that the CR database that is to be opened already
exists, but it can be explicitly checked and created by adding the
``create=True`` attribute at the instantiation of the :class:`CRDatabase`
object, i.e.

    >>> dbm = crdatabase.CRDatabase('dbfilename.db', create=True)

In the examples below we assume we have such a ``dbm`` object.


Navigating through the database
===============================

To obtain a list of ``eventIDs`` of events that satisfy particular criteria you
can use the :meth:`getEventIDs` method, e.g.:

    >>> eventIDs = dbm.getEventIDs(status='NEW')

An event information object can then be obtained via:

    >>> event = dbm.getEvent(id=eventIDs[0])

This ``event`` object will contain a list with datafile objects that can be
looked up via:

    >>> for datafile in event.datafiles:
    ...     datafile.summary()

where we used the :meth:`summary` method to show in a human readable format what
is inside the information object.


Modifying the database
======================

Updating an information object
------------------------------

To store the modifications you have made to an information object to the
database you need to write it explicitly to the database by using the
:meth:`write` command.  By default this will not only save the information in
the particular information object, but also its parameters and underlying
information objects. E.g. the command:

    >>> station.write()

will store the station information, the station parameters and all polarization
information.  You can suppress the writing of parameters and underlying
information objects by applying the ``parameters=False`` and ``recursive=False``
attribute settings, respectively, e.g.:

    >>> station.write(parameters=False, recursive=False)


Removing an event from the database
-----------------------------------

.. warning:: This is a powerful command, use it with caution!

If you want to remove an information item from the database, this means that the
underlying also needs to be removed from the database. For this there are the
:meth:`deleteEvent`, :meth:`deleteDatafile`, :meth:`deleteStation` and
:meth:`deletePolarization` methods in :class:`CRDatabase`.

    >>> dbm.deleteEvent(id=1)

This will remove the event with ``eventID=1``, all event parameters and all
underlying information objects that are linked to it.

