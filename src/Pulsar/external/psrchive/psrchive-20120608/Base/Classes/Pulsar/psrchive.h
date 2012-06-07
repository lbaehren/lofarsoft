//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2002, 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/psrchive.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/07 14:07:13 $
   $Author: straten $ */

#ifndef __Pulsar_psrchive_h
#define __Pulsar_psrchive_h

/*! \mainpage 
 
  \section Introduction
 
  The PSRCHIVE Library implements a set of base classes (in C++) that
  may be used in the storage, manipulation, and analysis of the
  observational data commonly used in pulsar experiments.  The base
  classes implement a wide variety of general purpose, flexible
  routines.
 
  \section profiles Pulse Profiles
 
  The basic quantity observed in most pulsar experiments is the pulse
  profile: a one-dimensional array of some measured quantity as a
  function of pulse phase.  This is represented by the Pulsar::Profile
  class.  The Pulsar::Integration class contains a two-dimensional
  array of Pulsar::Profile objects, each integrated over the same time
  interval.  The Pulsar::Profile objects are typically organized as a
  function of polarization and observing frequency, though other
  measured states are possible.  The Pulsar::Archive class implements
  a one-dimensional array of Pulsar::Integration objects, each with
  similar observational parameters and each in "pulse phase" with each
  other.

  \section container Container Interface

  The Pulsar::Profile, Pulsar::Integration, and Pulsar::Archive
  classes are Pulsar::Container classes; they implement a minimal set
  of operations required to load, store, and perform simple
  manipulations of their contents.  For example, the Pulsar::Profile
  class provides the following member functions:

  <UL>
  <LI> offset - adds offset to each bin of the profile </LI>
  <LI> scale - multiplies each bin of the profile by scale </LI>
  <LI> rotate - rotates the profile in phase </LI>
  <LI> bscrunch - integrates neighbouring phase bins in profile </LI>
  <LI> fold - integrates neighbouring sections of the profile </LI>
  </UL>

  The Pulsar::Integration class implements a minimal set of operations
  required to manipulate a set of Pulsar::Profile objects in the
  polarization and frequency domains.  In addition to simple nested
  calls of the above functions, these include:
  <UL>
  <LI> dedisperse - rotates all profiles to remove dispersion delays 
  between channels </LI>
  <LI> defaraday - transforms all profiles to remove Faraday rotation
  between channels </LI>
  <LI> fscrunch - integrates profiles from neighbouring channels </LI>
  <LI> pscrunch - integrates profiles from two polarizations into 
  one total intensity </LI>
  <LI> invint - forms the polarimetric invariant interval from 
  Stokes (I,Q,U,V) </LI>
  <LI> transform - performs a polarimetric transformation </LI>
  </UL>

  The Pulsar::Archive virtual base class is the interface that will be
  used in most applications.  In addition to providing interfaces to
  all of the above functions, the Pulsar::Archive class implements:
  <UL>
  <LI> tscrunch - integrates profiles from neighbouring Integrations </LI>
  <LI> append - copies (or transfers) the Integrations from one Archive
  to another </LI>
  <LI> set_ephemeris - installs a new ephemeris and polyco, and aligns
  all profiles to it </LI>
  </UL>

  For a complete list of the methods defined in each of these base classes,
  please see the <a href="annotated.html">Compound List</a>.

  More sophisticated operations are generally implemented as separate
  classes that take one of the Pulsar::Container classes as an input.
  These mostly inherit the Pulsar::Algorithm base class.

  \section algorithm Algorithm Interface

  The interface to an Pulsar::Algorithm is arbitrary.  The base class
  exists only as a means of classification.  Pulsar::Algorithm classes
  can be used to calculate statistics, find the best-fit shift between
  a profile and a template, perform scattered power correction, etc.

  \section format Archive File Formats

  The Pulsar::Archive virtual base class specifies only the minimal
  set of information required in order to implement basic data
  reduction algorithms.  Although it specifies the interface to set
  and get the values of various attributes and load and unload data
  from file, no storage or I/O capability is implemented by
  Pulsar::Archive.  These methods, especially those related to file
  I/O, must be implemented by classes that inherit the Pulsar::Archive
  base class.

  Most observatories and research groups use unique file formats and
  associate different pieces of information with their observations.
  The derived classes must therefore implement the storage and
  modification of this auxiliary information.  This may be
  facilitated using the Archive::Extension classes.

  In general, applications need not know anything about the specific
  archive file format with which they are dealing.  New Pulsar::Archive
  instances are created and loaded from file by calling the static
  Pulsar::Archive::load factory.

 */

#define PSRCHIVE_HTTP "http://psrchive.sourceforge.net"

#endif
