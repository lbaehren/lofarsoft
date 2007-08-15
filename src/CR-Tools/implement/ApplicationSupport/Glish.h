//# Glish.h: AIPS++ wrapper classes for Glish values and events.
//# Copyright (C) 1994,1995,1999
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id: Glish.h,v 19.4 2004/11/30 17:51:10 ddebonis Exp $

#ifndef TASKING_GLISH_H
#define TASKING_GLISH_H

#include <ApplicationSupport/GlishArray.h>
#include <ApplicationSupport/GlishRecord.h>
#include <ApplicationSupport/GlishEvent.h>

namespace casa { //# NAMESPACE CASA - BEGIN

  /*!
    \file Glish.h

    \ingroup ApplicationSupport

    \brief AIPS++ wrapper classes for Glish values and events.

    <h3>Synopsis</h3>
    
    The classes in the Glish module were created to make it easy to construct
    AIPS++ applications which interact with each other (via interprocess
    communications). This involves constructing values which are sent back and
    forth between the processes, sending and receiving these values, and managing
    all of the events (including X Windows events).
    
    This module is built upon Glish. Glish is a language (and supporting
    infrastructure) for constructing loosely coupled distributed systems.
    The values which Glish and this module deal with can either be arrays or
    records. Arrays correspond to the arrays found in C and other languages,
    and records correspond to the heterogeneous structures found in most languages,
    e.g. the <em>C struct</em>.
    
    The major external classes in this module are:
    <ul>
      <li>GlishValue is the class for all Glish value objects in this module.
      GlishValues follow copy-on-write semantics, so assignment and passing
      and return by value are cheap.
      <li>GlishArray is a (possibly n-dimensional) array of glish values, all
      of which have the same type. This is also the class which is used to
      hold a single value.
      <li>GlishRecord is a "structure" of values, made up of named arrays and
      records (the latter makes it hierarchical).
      <li>GlishSysEvent is a named value which has been sent from an external
      source.
      <li>GlishSysEventSource the object inside the program which emits the
      GlishSysEvent objects. It is also the object to which events from this
      executable are posted. This object can also be used to register callbacks
      and and handle incoming events in an event driven manner.
      <li>XSysEventSource allows the user to handle both Glish and X events
      using the same event loop. See XSysEventSource for an example.
    </ul>
    These classes insulate the user from the underlying Glish classes by hiding
    event management complications and translating Glish values into AIPS++
    classes. It is very easy to construct an application which can stand alone
    or be started from the Glish CLI with these classes. In addition, the
    application could easily be a windowing application.
 
    For information about the Glish system in general, a
    <a href="http://info.cv.nrao.edu/aips++/glish_docs/USENIX-93.ps">general 
    introduction <em>(75K)</em></a> to Glish is available as well as a complete
    <a href="http://info.cv.nrao.edu/aips++/glish_docs/USER-DOC.ps"> users
    manual <em>(1Meg)</em></a>. These documents as well as the Glish source are
    available from the <a href="ftp://ee.lbl.gov/glish/">Lawrence Berkeley
    Laboratory</a>.
    
    <h3>Motivation</h3>
    
    AIPS++ is using Glish as a user command line interpreter and as the 
    communications infrastructure. Functionality is bound to the CLI by creating
    new clients which pass events back and forth between the client and the
    CLI. This module supports writing these clients in C++. While it would have
    been possible to use the native Glish classes, it is useful to have an
    independent interface which converts Glish classes into the classes used
    by AIPS++ programmers, e.g. GlishArray.
  */

// <a name="procedural_example">
// <h3>Procedural Example</h3>
// The following example implements a simple statistics server for arrays which
// have been sent from some other process (for example, a user interactively
// manipulating a user interface). In this example, the Glish events are
// handled serially with the process blocked waiting for the next event. This
// is a procedural style where as the <a href=#event_example>next example</a>
// is an event driven style.
// 
// <srcblock>
// #include <tasking/Glish.h>                                           //  1
// #include <casa/Arrays/ArrayMath.h>                                //  2
// #include <casa/Arrays/IPosition.h>                              //  3
//                                                                   //  4
// int main(int argc, char **argv)                                   //  5
// {                                                                 //  6
//     GlishSysEventSource eventStream(argc, argv);                  //  7
//     GlishSysEvent event;                                          //  8
//     while (eventStream.connected()) {                             //  9
//         event = eventStream.nextGlishEvent();                     // 10
//         if (event.type() == "statistics") {                       // 11
//             GlishArray ga = event.val();                          // 12
//             Array<Double> ad(ga.shape());                         // 13
//             ga.get(ad);                                           // 14
//             GlishRecord response;                                 // 15
//             response.add("mean", mean(ad));                       // 16
//             response.add("median", median(ad));                   // 17
//             eventStream.postEvent("statistics_result", response); // 18
//         } else if (event.type() != "shutdown") {                  // 19
//             eventStream.unrecognized();                           // 20
//         }                                                         // 21
//     }                                                             // 22
//     return 0;                                                     // 23
// }                                                                 // 24
// </srcblock>
// <ol>
//    <li> Include the Glish module header.
//    <li> Where the statistics functions (mean and median here) are defined.
//    <li> IPosition is needed when dealing with shapes.
//    <li>
//    <li> When Glish starts up a server, it uses special command line 
//           arguments. Thus we need the command line arguments to get passed
//           into main.
//    <li>
//    <li> The events from the "remote" Glish will appear to come from this
//           object, which requires the command line arguments for construction.
//    <li> "event" will be used to hold the current event from eventStream.
//    <li> Run for as long as we are connected to the remote process.
//    <li> Get the event. This blocks until there's an event to be had.
//    <li> If the event type (name) is statistics
//    <li> Convert the event into a GlishArray. A more serious application 
//           would check that the event is a <src>GlishValue::ARRAY</src> before
//           assigning it to one. In this case, an exception would be thrown if
//           it wasn't.
//    <li> Create an AIPS++ array to hold the array (so we can compute upon
//           it).
//    <li> Put the value into the AIPS++ array. We should really check that
//           the elemental type is compatible with double, e.g. not String.
//           If it was incompatible, an exception would be thrown.
//    <li> Create a record which will be used to hold the statistics we
//           compute.
//    <li> Create a field in the record called "mean", holding the mean value
//           of the array.
//    <li> Similarly for the median.
//    <li> Send the response back, naming the event "statistics_result".
//    <li> If the event is not "statistics", and it is not "shutdown" (this
//           latter being system generated before the connection is "normally"
//           severed) then it is unrecognized.
//    <li> Call the "unrecognized" member whenever this happens.
// </ol>
// </a>
//
// <a name="event_example">
// <h3>Event Driven Example</h3>
// The previous example could also be written in an event driven style. This
// is the style of programming familiar to developers of graphical user
// interfaces. Here callbacks are set up for the Glish events and then
// control is given up until the callbacks are called. This implements the
// same functionality as the <a href=#procedural_example>previous example</a>.
// See the <linkto class=XSysEvent#event_example>event driven example</linkto>
// in the <src>XSysEvent</src> header file for an example of how X Windows
// events can be combined with Glish events.
// 
// <srcblock>
// #include <tasking/Glish.h>                                           //  1
// #include <casa/Arrays/ArrayMath.h>                                //  2
// #include <casa/Arrays/IPosition.h>                              //  3
//                                                                   //  4
// Bool statCallbk(GlishSysEvent &e, void *) {                       //  5
//     GlishSysEventSource *es = e.glishSource();                    //  6
//     GlishArray ga = e.val();                                      //  7
//     Array<Double> ad(ga.shape());                                 //  8
//     ga.get(ad);                                                   //  9
//     GlishRecord response;                                         // 10
//     response.add("mean", mean(ad));                               // 11
//     response.add("median", median(ad));                           // 12
//     es->postEvent("statistics_result", response);                 // 13
//     return True;                                                  // 14
// }                                                                 // 15
//                                                                   // 16
// int main(int argc, char **argv)                                   // 17
// {                                                                 // 18
//     GlishSysEventSource eventStream(argc, argv);                  // 19
//     eventStream.addTarget(statCallbk,"^statistics$");             // 20
//     eventStream.loop();                                           // 21
// }                                                                 // 22
// </srcblock>
// <ol>
//    <li> Include the Glish module header.
//    <li> Where the statistics functions (mean and median here) are defined.
//    <li> IPosition is needed when dealing with shapes.
//    <li>
//    <li> To handle Glish events in an event driven manner, callbacks must
//           be created for each <em>type</em> of event. The callback parameters
//           are the event, and a <src>void*</src> which can contain any
//           user data. The user data is an optional parameter to
//           <src>GlishSysEventSource::addTarget</src>.
//    <li> To post events, the event source must be retrieved from the event.
//    <li> Convert the event into a GlishArray. A more serious application 
//           would check that the event is a <src>GlishValue::ARRAY</src> before
//           assigning it to one. In this case, an exception would be thrown if
//           it wasn't.
//    <li> Create an AIPS++ array to hold the array (so we can compute upon
//           it).
//    <li> Put the value into the AIPS++ array. We should really check that
//           the elemental type is compatible with double, e.g. not String.
//           If it was incompatible, an exception would be thrown.
//    <li> Create a record which will be used to hold the statistics we
//           compute.
//    <li> Create a field in the record called "mean", holding the mean value
//           of the array.
//    <li> Similarly for the median.
//    <li> Send the response back, naming the event "statistics_result".
//    <li> If the handler successfully handles the event, it should return
//           <src>True</src>, otherwise another handler will be sought.
//    <li>
//    <li>
//    <li> When Glish starts up a server, it uses special command line 
//           arguments. Thus we need the command line arguments to get passed
//           into main.
//    <li>
//    <li> This object will orchestrate the event handling. It requires the
//           command line arguments for initialization.
//    <li> To make callback functions known, it must be registered with
//           the <src>GlishSysEventSource::addTarget</src>. The string
//           (the second parameter) is a regular expression which indicates
//           which group of Glish events the function will accept. In this
//           case, <em>"^statistics$"</em> indicates that this function
//           will only accept events whose name equals <em>statistics</em>.
//    <li> This member function will cause the events to be processed and
//           passed to a callback function (if possible) until all events
//           have been processed.
// </ol>
// </a>
// </module>


} //# NAMESPACE CASA - END

#endif
