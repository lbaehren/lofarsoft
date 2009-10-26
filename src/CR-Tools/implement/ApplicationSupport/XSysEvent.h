//# XSysEvent.h: Wrappers for X event classes
//# Copyright (C) 1994,1995,1999,2000,2001
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
//# $Id$

#ifndef TASKING_XSYSEVENT_H
#define TASKING_XSYSEVENT_H

#include <casa/OS/SysEvent.h>
#include <casa/Containers/List.h>
#include <casa/Utilities/CountedPtr.h>

typedef union _XEvent XEvent;
typedef struct _WidgetRec *Widget;
typedef struct _XtAppStruct *XtAppContext;

namespace casa { //# NAMESPACE CASA - BEGIN

class XSysEventTarget;

// 
// <summary> X event wrapper </summary>
// <use visibility=export>
// <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
//
// <prerequisite>
//	<li> SysEvent
// </prerequisite>
//
// <etymology>
//	<em>See <linkto class=SysEvent>SysEvent</linkto></em>
// </etymology>
//
// <synopsis> 
//	This class is a wrapper for X events. It provides a standard
//	(minimal) interface to X events. This can be important when
//      several types of events, e.g. GlishEvents and XEvents, must
//      both be handled. This is why this SysEvent hierarchy was
//      developed.
//
//      <em>See <linkto class=XSysEventSource>XSysEventSource</linkto> for
//      more information.</em>
// </synopsis>
//
// <example>
//     <em>See the <a href=#event_example>example in XSysEventSource</a></em>.
// </example>     
//
// <motivation>
//	Often one needs to mix X events with other types of events.
//	This class provides a common interface for X events, and
//	all of the related classes provide a system for handling
//	most asynchronous types of events which occur.
// </motivation>
//
class XSysEvent : public SysEvent {
public: 

    friend class XSysEventSource;
    friend class XSysEventTarget;

    // These copy constructors allow on to copy XSysEvents
    // <group>
    XSysEvent(const XSysEvent &other) : SysEvent(other.src), event(other.event) { }
    XSysEvent(const XSysEvent *other) : SysEvent((*other).src), event((*other).event){ }
    // </group>

    ~XSysEvent();

    // Returns the group to which this event belongs. The 
    // <src>SysEvent::Group</src> is just an enumeration of the possible
    // event types.
    SysEvent::Group group() const;
    //
    // Returns the type this event. The type is a string which represents
    // the type of the event.
    //
    String type() const;
    //
    // Returns the type of the event in X-speak.
    //
    int xtype() const;
    //
    // Dispatches this event to the event target parameter.
    //
    Bool dispatch(SysEventTarget &);
    //
    // Dispatches this event to the event target parameter,
    // and carries along an extra event source in case it
    // is needed. This might be needed if a GUI application
    // wanted to post Glish events as buttons are pressed.
    //
    Bool dispatch(SysEventTarget &, SysEventSource &);

protected:

    XSysEvent();
    XSysEvent(XEvent *v,SysEventSource *xs) : SysEvent(xs), event(v) { }

    //
    // Return the underlying data structure
    //
    XEvent *xevent() {return &(*event);}
    //
    // Return a copy of this event.
    //
    SysEvent *clone() const {
	return new XSysEvent(this);
    }

private:

    CountedPtr<XEvent> event;

};

class XSysEventSourceInfo;
class GlishSysEventSource;
// 
// <summary> X event generation wrapper </summary>
// <use visibility=export>
// <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
//
// <prerequisite>
//	<li> XSysEvent
// </prerequisite>
//
// <etymology>
//	<em>See <linkto class=SysEvent>SysEvent</linkto></em>
// </etymology>
//
// <synopsis> 
//	This class provides a wrapper around the X mechanics of
//	event generation. So that the event source can be queried
//	about waiting events. This is useful because it is part of
//      a general to all types of system events.
//
//      If the event driven interface to these event sources is used,
//      there is no more overhead than the typical X event loop, shown
//      below. Using the <linkto module=Glish#procedural_example>procedural
//      interface</linkto>, however, incurs a performance penalty due to
//      polling
//
//      <note role=tip> This class is most useful when both Glish and
//          X Windows events are used.
//      </note>
//
// </synopsis> 
//
// <a name=event_example>
// <example>
//     This is an example of how these classes might be used. It demonstrates
//     how Glish and X Windows events can be cleanly combined into one event
//     stream. The Glish event stream can also be used by itself in this manner
//     as well (see the <linkto class=GlishSysEvent#event_example>event driven
//     example</linkto> in the GlishEvent header file).
// <srcblock>
// #include <Xm/Xm.h>                                         // --+
// #include <Xm/Form.h>                                       //   |
// #include <Xm/PushB.h>                                      //   +->  1
// #include <iostream>                                      // --+
// #include <ApplicationSupport/Glish.h>                                    //        2
// #include <ApplicationSupport/XSysEvent.h>                          //        3
//                                                            // --+
// Bool helloCallback(GlishSysEvent &e, void *) {             //   |
//     GlishSysEventSource *src =  e.glishSource();           //   |
//     (*src).postEvent("hello_got",e.type());                //   |
//     return True;                                           //   |
// }                                                          //   +->  4
//                                                            //   |
// Bool defaultCallback(GlishSysEvent &e, void *) {           //   |
//     GlishSysEventSource *src =  e.glishSource();           //   |
//     (*src).postEvent("default_got",e.type());              //   |
//     return True;                                           //   |
// }                                                          // --+
//                                                            //
// Bool otherCallback(SysEvent &e, void *) {                  // --+
//     if ( e.group() == SysEvent::GlishGroup ) {             //   |
//      GlishSysEvent &ge = (GlishSysEvent &) e;              //   |
//      GlishSysEventSource *src =  ge.glishSource();         //   |
//      (*src).postEvent("other_got",ge.type());              //   +->  5
//      return True;                                          //   |
//     }                                                      //   |
//     return False;                                          //   |
// }                                                          // --+
//                                                            //
// static void DoXSetup(XSysEventSource &);                   //        6
// main(int argc, char **argv) {                              //        7
//     XSysEventSource xStream(argc,argv);                    //        8
//     GlishSysEventSource glishStream(argc, argv);           //        9
//                                                            //       10
//     glishStream.setDefault(defaultCallback);               // --+
//     glishStream.addTarget(helloCallback,".*hello");        //   +-> 11
//     glishStream.addTarget(otherCallback,"[a-z]+");         // --+
//                                                            //
//     xStream.combine(glishStream);                          //       12
//                                                            //
//     DoXSetup(xStream);                                     //       13
//                                                            //
//     XtRealizeWidget(xStream.toplevel());                   //       14
//                                                            //
//     xStream.loop();                                        //       15
// }
// 
// void QuitCB (Widget, XtPointer,XtPointer) {
//     cout << "Outttaaa here..." << endl;
//     exit(0);
// }
// 
// void DoCB (Widget, XtPointer,XtPointer) {
//     cout << "Do'in it..." << endl;
// }
// 
// static void DoXSetup(XSysEventSource &xStream) {
//     Widget outerContainer, quitButton, doButton;
// 
//     outerContainer = XtVaCreateManagedWidget ("OuterForm", xmFormWidgetClass, 
//                                               xStream.toplevel(), NULL);
//     quitButton = XtVaCreateManagedWidget ("QUIT", xmPushButtonWidgetClass,
//                      outerContainer, XmNtopAttachment, XmATTACH_FORM,
//                      XmNtopOffset, 0, XmNleftAttachment, XmATTACH_FORM,
//                      XmNleftOffset, 0, NULL);
// 
//     doButton = XtVaCreateManagedWidget ("DO", xmPushButtonWidgetClass,
//                      outerContainer, XmNtopAttachment, XmATTACH_FORM,
//                      XmNtopOffset, 25, XmNleftAttachment, XmATTACH_FORM,
//                      XmNleftOffset, 0, NULL);
// 
//     XtAddCallback(quitButton,XmNactivateCallback,QuitCB,(XtPointer) 0);
//     XtAddCallback(doButton,XmNactivateCallback,DoCB,(XtPointer) 0);
// }
// </srcblock>
// <ol>
//     <li> These are the X related includes required by this particular
//          example.
//     <li> Inclusion of the Glish module header file gets everything
//          necessary for manipulation of Glish values.
//     <li> This is also required since we are going to be handling both
//          X and Glish events. This is not included in the Glish module
//          header file because it is only needed when X is involved
//          <em>(perhaps this is wrong)</em>.
//     <li> This is the typical layout for a Glish callback. This is more
//          straight forward than the inherited callback format because
//          no casting is required. True should be returned if the event
//          is handled by the callback.
//     <li> This is the callback format inherited from
//          <linkto class=SysEventSource>SysEventSource</linkto>. It requires
//          that the user verify the type of the event parameter. If the
//          type does not match <src>False</src> is returned to indicate that
//          this callback can't handle the event.
//     <li> This local function is used to hide all of the Motif details.
//     <li> Initialization of Glish and X requires access to the command line
//          parameters so they must be received.
//     <li> This object will be the source of X Windows events for this program.
//     <li> This object will be the source of Glish events for this program.
//     <li> These lines set up the Glish callbacks. The <src>setDefault</src>
//          member function can be used to set the <em>default</em> callback.
//          This callback will then be use if none of the other callbacks
//          can be used. The other callbacks are set with <src>addTarget</src>.
//          The second parameter (a <src>String</src>) specifies which event
//          names this callback is intended to be used for. The first,
//          <em>".*hello"</em>, indicates that the <src>helloCallback</src>
//          should be used for any event whose name begins with anything
//          (including nothing) followed by <em>hello</em>.
//     <li> To allow both Glish and X Windows events to be handled seamlessly
//          the two event streams must be combined. This line does that. A
//          Glish event stream can be combined with an X event stream, but
//          <b>not</b> the reverse.
//     <li> This sets up the underlying X infrastructure for creating the 
//          display and then maps the display.
//     <li> This handles the X Windows events and, because the Glish event
//          stream was combined with this X stream, the Glish events as
//          well.
// </ol>
// </example>
// </a>
class XSysEventSource : public SysEventSource {
public:
    //
    // This initializes the event source. It takes the command line parameters,
    // argc and argv, and the name if any of the source. This will do all of the
    // X initialization. From here on, the <src>XSysEventSource::toplevel()</src>
    // member function should be used to access the top level widget.
    //
    // <group>
    XSysEventSource(int argc, char **argv, const char *name="EventSource");
    //
    // The parameter <src>options</src> should be a pointer to the X structure
    // <src>XrmOptionDescRec</src>. It is <src>void</src> here to avoid inclusion of X
    // header files.
    //
    XSysEventSource(int argc, char **argv, void *options, char **fallback_resources,
		    const char *name="EventSource");
    // </group>

    ~XSysEventSource();

    // Returns the group of the event source. <src>SysEvent::Group</src> is just
    // an enumeration which lists all of the possible types of Events.
    //
    SysEvent::Group group() const;
    //
    // Returns the actual X context for use.
    //
    XtAppContext &context();
    //
    // Returns the actual X top level widget for use.
    //
    Widget &toplevel() {return *toplevel_;}

    // Get the next waiting X event (blocks).
    //
    SysEvent nextEvent();

    // Check to see if any X events are queued up.
    //
    Bool waitingEvent();

    // Indicates if the source is still connected to
    // the event stream or not.
    Bool connected();

    // Used to check to see if the <src>other</src>
    // <src>SysEventSource</src> can be combined with this
    // <src>XSysEventSource</src>.
    // <group>
    Bool canCombine(const SysEventSource &other) const;
    Bool canCombine(const SysEventSource *other) const;
    // </group>

    // This is used to combine another event source, <src>other</src>.
    // The SysEventSource, <src>other</src>, must persist as long as
    // objects of this class persist, its address will be used.
    // <group>
    Bool combine(SysEventSource &other);
    Bool combine(SysEventSource *other);
    // </group>

    // Invoke the target whenever the event matches the regular
    // expression. These do <b>not</b> currently add targets, but
    // rather they always return <src>False</src> indicating that
    // the target couldn't be added.
    // <group>
    Bool addTarget(SysEventTarget &, const String &);
    Bool addTarget(SysEventTarget *, const String &, Bool ownTarget=False);
    Bool addTarget(SysEventTargetProc, const String &, void *userData=0);
    // </group>

    // Specifies the default action which should be applied when
    // no other targets are applicable. These do <b>not</b> currently
    // add targets, but rather they always return <src>False</src>
    // indicating that the target couldn't be added.
    // <group>
    Bool setDefault(SysEventTarget &);
    Bool setDefault(SysEventTarget *, Bool ownTarget=False);
    Bool setDefault(SysEventTargetProc, void *userData=0);
    // </group>

    // <em>loop</em> and deliver events to targets as they arrive.
    // The process will be <em>blocked</em> until an event arrives.
    Bool loop();

    // This is for <b>internal use</b>, and is invoked when an event
    // may be available for processing on this event source.
    void invokeTarget();

protected:

    SimpleCountedPtr<XtAppContext> context_;
    SimpleCountedPtr<Widget> toplevel_;
    XSysEventSource(XtAppContext *v, Bool DeleteIt = True) : context_(v,DeleteIt),
					    srcInfo(0) { }

    Bool addSource(GlishSysEventSource *other);
    // Used to set a file descriptor for monitoring by X and to install
    // (<src>set</src>) or de-install (<src>clr</src>) a particular target
    // for later invocation.
    // <group>
    void setFD(int fd, XSysEventSourceInfo*);
    void clrFD(int fd, XSysEventSourceInfo*);
    // </group>
    ListIter<CountedPtr<XSysEventSourceInfo> > *srcInfo;
};

// 
// <summary> X event handling wrapper </summary>
// <use visibility=export>
// <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
//
// <prerequisite>
//	<li> XSysEventSource
// </prerequisite>
//
// <etymology>
//	<em>See <linkto class=SysEvent>SysEvent</linkto></em>
// </etymology>
//
// <synopsis>
//	This class provides a wrapper around the mechanics of
//	handling an event generated by X. The actions this class
//	can take are very limited. In general, all it can do is
//	dispatch the event through the regular X channels, but
//	this class is, none the less, one way to go when mixing
//	of X and non-X events.
//
//      This <em>default</em> XSysEventTarget simply dispatches the
//      XEvent using the typical X mechanics.
//
//      <em>See <linkto class=XSysEventSource>XSysEventSource</linkto> for
//      more information.</em>
// </synopsis> 
//
// <example>
//     <em>See the <a href=#event_example>example in XSysEventSource</a></em>.
// </example>
//
class XSysEventTarget : public SysEventTarget {
public:

    XSysEventTarget() {}

    ~XSysEventTarget();

    // Get the group of this X event target.
    //
    SysEvent::Group group() const;

    SysEventTarget *clone() const;

    Bool handle(SysEvent);
    //
    // Pass the XSysEvent along to the handler parameter. This
    // typically simply involved dispatching the event to the
    // underlying X dispatch mechanism.
    //
    virtual Bool handle(XSysEvent &);
    //
    // Pass the XSysEvent along to the event handler, but carry
    // along the extra event source. This is useful when handlers
    // of one SysEvent::Group need to post events to a different
    // Group, i.e. X handlers posting Glish events.
    //
    virtual Bool handle(XSysEvent &, SysEventSource &);
};

struct XSysEventSourceInfo_XGUTS;
// <summary><b>Internal</b> class</summary>
// <use visibility=local>
//
// This class is used as a container of information for sources
// which have been <em>combined</em> with <src>XSysEventSource</src>.
// It facilitates invoking targets based on regular expressions.
//
// This class is <b>not</b> intended for external use.
//
// <note role=tip> We would have liked to put this definition in
//        the source (.cc) file, but the compiler SunOS Cfront doesn't
//        seem to deal with it. Instead this class had to be put into
//        this header file, and the X Windows specific information
//        was put into the structure <src>XSysEventSourceInfo_XGUTS</src>
//        to avoid exposing users of <src>XSysEvent.h</src> to X11
//        header files.
// </note>
//
class XSysEventSourceInfo {
public:
    SysEventSource *src;
    fd_set fdset;
    XSysEventSourceInfo_XGUTS *x;
    XSysEventSourceInfo(SysEventSource *s);
    ~XSysEventSourceInfo();
};



} //# NAMESPACE CASA - END

#endif
