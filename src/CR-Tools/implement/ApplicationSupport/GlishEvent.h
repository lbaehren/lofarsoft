//# GlishEvent.h: Glish event wrappers 
//# Copyright (C) 1994,1995,1997,1998,1999,2000,2001,2002
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

#ifndef TASKING_GLISHEVENT_H
#define TASKING_GLISHEVENT_H

#include <casa/aips.h>
#include <casa/OS/SysEvent.h>
#include <ApplicationSupport/GlishValue.h>

//# The following are "native" Glish classes.
class Client;
class GlishEvent;
class Value;

struct timeval;

namespace casa { //# NAMESPACE CASA - BEGIN

class String;

class GlishArray;
class GlishSysEventSource;
class GlishValue;

template<class t> class List;
template<class t> class CountedPtr;

extern void throw_invalid_access();
extern void throw_invalid_event();
extern void throw_uninitialzed_glishsysevent();

// 
// <summary>Glish event wrapper</summary>
// <use visibility=export>
// <reviewed reviewer="pshannon" date="1994/10/28" demos="dGlish">
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
//	This class implements a wrapper around Glish events so that
//	they can be processed within a general event loop. A common
//	interface for receiving and transmitting events allows
//	heterogeneous events to be processed within the same loop.
//
//	Typically, the GlishSysEvent objects are either returned by
//      the GlishSysEventSource class or passed as a parameter to
//      a callback function. As a result, this class will rarely
//	be constructed by the user.
// </synopsis>
//
// <a name=procedural_example>
// <example>
//
//     This example simply prints out the events it receives. It does
//     this in a procedural manner.
//     <srcblock>
//     GlishSysEventSource source(argc, argv);
//     GlishSysEvent event;
//     GlishValue value;
//     GlishArray array;
//     GlishRecord record;
//     while (source.connected()) {
//         event = source.nextGlishEvent();
//         cout << "Event " << event.type() << " : ";
//         value = event.val();
//         if (value.type() == GlishValue::ARRAY) {
//             array = value;
//             cout << array.format() << endl;
//         } else {
//             record = value;
//             cout << record.format() << endl;
//         }
//     }
//     </srcblock>
//
//     For another example of this type of processing see
//     <linkto module=Glish#procedural_example>the example</linkto>
//     in the Glish module page. For an example of how this class is
//     used in an event driven setting see the 
//     <linkto module=Glish#event_example>other example</linkto> in the
//     Glish module file or the example <a href=#event_example>below</a>.
// </example>
// </a>
//
// <motivation>
//	For X window based Glish clients, it is often necessary to
//	handle Glish and X events in the same loop. See 
//      <linkto class=XSysEvent#event_example>the example</linkto>
//      in the XSysEvent header file for an example of how X and Glish
//      events can be combined.
// </motivation>
//
class GlishSysEvent: public SysEvent {
public: 

    friend class GlishSysEventSource;

    // Copy constructor which uses reference semantics.
    // <group>
    GlishSysEvent(const GlishSysEvent &other);
    GlishSysEvent(const GlishSysEvent *other);
    // </group>

    // Default constructor which creates an invalid event.
    GlishSysEvent() : event(0), val_(0) {}

    ~GlishSysEvent();

    // Assignment Operator using reference semantics.
    GlishSysEvent &operator=(const GlishSysEvent &other);

    // Returns the group to which this event belongs.
    SysEvent::Group group() const;

    // Returns the GlishValue which is stored in this event. This
    // is the actual raw Glish value.
    GlishValue &val() {
	fillVal();
	if (!val_) throw_uninitialzed_glishsysevent();
	return *val_;
    }

    // Returns the type of the value stored in the event.
    GlishValue::ValueType valType() const {
	fillVal();
	if (!val_) throw_uninitialzed_glishsysevent();
	return val_->type();
    }

    // Returns the event type of this Glish event. This is the
    // <em>name</em> of the event.
    String type() const;

    // Used to deliver this particular event to the <src>SysEventTarget</src>
    // parameter.
    // <group>
    Bool dispatch(SysEventTarget &);
    Bool dispatch(SysEventTarget &, SysEventSource &);
    // </group>

    // Is this event a <em>request</em> event?
    Bool isRequest() const;

    // Is this event a <em>reply</em> to a <em>request</em> event?
    Bool isReply() const;

    // This will return the <em>source</em> of this event.
    GlishSysEventSource *glishSource() 
	{ return (GlishSysEventSource*) source(); }

protected:

    // Used to construct a GlishEvent, when it arrives
    // from the Glish client.
    GlishSysEvent(GlishEvent *ne, SysEventSource *xs);

    // Returns a deep copy of this object.
    SysEvent *clone() const;
    // Internal function which fills the internal GlishValue 
    // object using the Glish Value from the event. This must
    // be called before the accessors can be used.
    void fillVal() const;

private:

    GlishEvent *event;
    GlishValue *val_;

};

typedef Bool (*GlishSysEventTargetProc)(GlishSysEvent &, void *);
// <summary>wrapper around Glish event generation</summary>
// <use visibility=export>
// <reviewed reviewer="pshannon" date="1994/10/28" demos="dGlish">
//
// <prerequisite>
//	<li> SysEventSource
//	<li> GlishSysEvent
// </prerequisite>
//
// <etymology>
//	<em>See <linkto class=SysEvent>SysEvent</linkto></em>
// </etymology>
//
// <synopsis> 
//	This class implements a wrapper around the generation of Glish
//	events. This allow Glish events to be used in a heterogeneous
//	event loop. The loop can either be a procedural type of loop
//      where the events are retrieved then processed, or an event driven
//      loop.
// </synopsis> 
//
// <h3> Procedural Example </h3>
//
//     This demonstrates an echo client which simply echoes its events
//     back through the GlishSysEventSource.
//
// <srcblock>
// GlishSysEventSource source(argc, argv);
// GlishSysEvent event;
// while (source.connected()) {
//     event = source.nextGlishEvent();
//     source.postEvent(event.type(),event.val());
// }
// </srcblock>
//     For another example of this type of event processing see
//     <linkto module=Glish#procedural_example>the example</linkto> in
//     the Glish module page. 
//
// <a name=event_example>
// <h3> Event Driven Example </h3>
//
//     This class can also be used in an event driven manner. This next example
//     demonstrates this:
// <srcblock>
// #include <ApplicationSupport/Glish.h>
//
// Bool helloCallback(GlishSysEvent &e, void *) {
//     GlishSysEventSource *src =  (GlishSysEventSource*)e.source();
//     (*src).postEvent("hello_got",e.type());
//     return True;
// }
// Bool defaultCallback(GlishSysEvent &e, void *) {
//     GlishSysEventSource *src =  (GlishSysEventSource*)e.source();
//     (*src).postEvent("default_got",e.type());
//     return True;
// }
//
// int main(int argc, char **argv)
// {
//     GlishSysEventSource eventStream(argc, argv);
//
//    eventStream.setDefault(defaultCallback);
//    eventStream.addTarget(helloCallback,"^hello");
//    eventStream.loop();
// }
// </srcblock>
//
//    In this example all of the events whose name begins with
//    <em>hello</em> get directed to the <src>helloCallback</src>.
//    The rest get directed to the <src>defaultCallback</src>. For
//    other examples of event driven processing see
//    <linkto module=Glish#event_example>the example</linkto> in the
//    Glish module page, or <linkto class=XSysEvent#event_example>
//    the example</linkto> in the <src>XSysEvent</src> page which
//    includes X Windows events in the loop.
// </a>
//
class GlishSysEventSource : public SysEventSource {
public: 

    // Takes the command line parameters, argc and argv, and initializes
    // the Glish Client to begin producing GlishEvents. Argc and argv are
    // required because Glish uses the command line parameters to pass port
    // information to the client.
    GlishSysEventSource(int &argc,char **argv);

    ~GlishSysEventSource();

    // Returns the group which this event source belongs to. The group indicates
    // which type of events this particular SysEventSource generates.
    SysEvent::Group group() const;

    // Gets the "raw" Glish Client for performing operations.
    // <note role=tip> If it is necessary to manipulate the underlying
    //		Glish Client, then the public interface of
    //		one or more of these classes probably needs
    //		extending.
    // </note>
    Client &client() {return *client_;}
    // Get the next event which is queued up. This will block.
    SysEvent nextEvent();

    // Get the next Glish event. This blocks until there is an event.
    GlishSysEvent nextGlishEvent();

    // Get the next event. If the event has not occurred in the timeout
    // period, return False, otherwise return True and set event. Timeout
    // of zero means return immediately if there is no pending event, less
    // than zero means to block until there is an event.
    Bool nextGlishEvent(GlishSysEvent &event, Int timeOutInMillisec);

    // Check to see if there are any Glish events waiting to be
    // handled.
    Bool waitingEvent();

    // Process one Glish event.
    void processEvent( GlishSysEvent );

    // Indicated if the source is still connected to the event
    // stream.
    Bool connected();
    // Indicates if the client is connected to a Glish interpreter
    // elsewhere. If False is returned, the client is running 
    // stand-alone; if True is returned it the client is connected
    // to a interpreter.
    Bool hasInterpreter();
    // This posts an event to the Glish system where the first
    // parameter is the event name, and the second parameter is
    // the event value.
    // <group>
    void postEvent(const String &name, const String &value);
    void postEvent(const String &name, const Value *val);
    void postEvent(const String &name, const GlishValue &val);
    void postEvent(const String &name, const GlishArray &val);
    void postEvent(const String &name, const GlishValue *val);
    void postEvent(const char *name, const char *val);
    void postEvent(const String &name, const char *val);
    void postEvent(const char *name, const String &val);
    // </group>

    // Post an event to the Glish system in reply to the last
    // event received.
    // <group>
    void reply(const GlishValue &val);
    void reply(const GlishValue *val);

    Bool replyPending() const;
    // </group>
  
    // Post an event to indicate that the last event was not
    // understood.
    void unrecognized();

    // get the client FDs
    Int addInputMask(fd_set *selectSet);

    // Called to report that the current event has an error.
    void Error( const String &msg );

    // Check to see <src>other</src> can be combined with this
    // event stream.
    // <group>
    Bool canCombine(const SysEventSource &other) const;
    Bool canCombine(const SysEventSource *other) const;
    // </group>

    // Combine <src>other</src> with this event stream. If this
    // is impossible, <src>False</src> is returned.
    // <group>
    Bool combine(SysEventSource &other);
    Bool combine(SysEventSource *other);
    // </group>

    // Invoke the specified target whenever the event matches the
    // regular expression (the second parameter). If the target
    // cannot be added <src>False</src> is returned.
    // <group>
    Bool addTarget(SysEventTarget &tgt, const String &regx);
    Bool addTarget(SysEventTarget *tgt, const String &regx, Bool ownTarget=False);
    Bool addTarget(SysEventTargetProc tgt, const String &regx, void *userData=0);
    Bool addTarget(GlishSysEventTargetProc tgt, const String &regx, void *userData=0)
		{ return addTarget((SysEventTargetProc) tgt, regx, userData); }
    // </group>

    // Specifies the default action. This target is invoked whenever
    // none of the other targets apply. If the target cannot be added
    // <src>False</src> is returned.
    // <group>
    Bool setDefault(SysEventTarget &tgt);
    Bool setDefault(SysEventTarget *tgt, Bool ownTarget=False);
    Bool setDefault(SysEventTargetProc tgt, void *userData=0);
    Bool setDefault(GlishSysEventTargetProc tgt, void *userData=0)
		{ return setDefault((SysEventTargetProc) tgt, userData); }
    // </group>

    // <em>loop</em> and deliver events to targets as they arrive.
    // The process will be <em>blocked</em> until an event arrives.
    Bool loop();

    // This is for <b>internal use</b>, and is invoked when an event
    // may be available for processing on this event source.
    void invokeTarget();

protected:
    Client *client_;

    List<CountedPtr<SysEventTargetInfo> > *tgtInfo;
    SysEventTargetInfo *defaultTgt;

    // Get the next base-level Glish event which is queued up. This will block.
    GlishEvent *nextPrimitiveEvent(::timeval *timeout, int &timedout);
    Bool invokeTarget(GlishEvent *ev);

private:

    Bool valid;

};


// 
// <summary>a generalized Glish event handler</summary>
// <use visibility=export>
// <reviewed reviewer="pshannon" date="1994/10/28" demos="dGlish">
//
// <prerequisite>
//   <li> SysGlishEvent
//   <li> SysGlishEventSource
// </prerequisite>
//
// <etymology>
//	<em>See <linkto class=SysEvent>SysEvent</linkto></em>
// </etymology>
//
// <synopsis> 
//	This class implements a generalized wrapper around Glish event
//	handlers. This allows events of different types to be accepted,
//	and dispatched within the same event loop.
// </synopsis> 
//
// <example>
//
//     This example simply takes an event, modifies it, i.e. sticks the
//     letters 'MOD' on the front of the event type, and then reposts
//     the event.
//
// <srcblock>
//     void eventhandler(GlishSysEvent &evt) {
//         GlishValue &val = evt.val();
//         GlishSysEventSource *s = (GlishSysEventSource *) evt.source();
//         String mod("MOD");
//         String sval = val.format();
//
//         mod += evt.type();
//
//         s->postEvent(mod,sval);
//     }
//
//     main(int argc, char **argv) {
//         GlishSysEventSource gsrc(argc,argv);
//         GlishSysEventTarget gtgt(eventhandler);
//         SysEvent event;
//
//         while (gsrc.connected()) {               // 1
//             if (gsrc.waitingEvent()) {           // 2
//                 event = gsrc.nextEvent();        // 3
//                 event.dispatch(gtgt);            // 4
//             }
//             // ...                               // 5
//         }
//     }
// </srcblock>
//
//    If an extra event source were added to this loop, the 'group()' and 
//    perhaps the 'type()' of the event could be used to choose among a
//    number of SysEventTargets. Here:
//    <ol>
//         <li> Loop while the event sources are connected. Here there
//                is only a GlishSysEventSource.
//         <li> Check to see if there is an event waiting from a 
//                particular event source.
//         <li> Get the event.
//         <li> Dispatch the event. There could be more than one target
//                for a particular SysEventSource, and the 'group()' and
//                the 'type()' of the SysEvent could be used to decide which
//                target should receive the event.
//         <li> Check any other SysEventSources for waiting events.
//     </ol>
// </example>
//
class GlishSysEventTarget : public SysEventTarget {
public: 

    // Construct an event with a target function which takes an event for handling.
    GlishSysEventTarget(Bool (*one)(GlishSysEvent &)) : cb_one(one), cb_two(0) {}
    // Construct an event with a target function which takes an event
    // and a SysEventSource. This is useful when the handling function
    // must know another event source. A case where this happens is when
    // a Glish handler function must post X events. In this case, the
    // XSysEventSource must be available.
    GlishSysEventTarget(Bool (*two)(GlishSysEvent &, SysEventSource &)) : cb_one(0), cb_two(two) {}
    // Constructs an event target which ignores events.
    GlishSysEventTarget() : cb_one(0), cb_two(0) {}

    ~GlishSysEventTarget();

    // Return the group to which this handler belongs.
    SysEvent::Group group() const;

    SysEventTarget *clone() const;

    // Handle the given event.
    // <group>
    virtual Bool handle(SysEvent);
    virtual Bool handle(GlishSysEvent &);
    virtual Bool handle(GlishSysEvent &, SysEventSource &);
    // </group>

protected:

    Bool (*cb_one)(GlishSysEvent &);
    Bool (*cb_two)(GlishSysEvent &, SysEventSource &);

};

// <summary><b>Internal</b> class</summary>
// <use visibility=local>
//
// This class is used as a container of information for dispatching
// targets. It facilitates invoking targets based on regular
// expressions.
//
// This class is <b>not</b> intended for external use.
class GlishSysEventProcTargetInfo : protected SysEventTargetInfo {
public:
    SysEventTargetInfo::getRegex;
    SysEventTargetInfo::getUserData;
    SysEventTargetInfo::type;

    GlishSysEventProcTargetInfo(GlishSysEventTargetProc t, const String &r,void *u=0) :
				SysEventTargetInfo((SysEventTargetProc) t,r,u) {}
    GlishSysEventProcTargetInfo(GlishSysEventTargetProc t, void *u=0) :
				SysEventTargetInfo((SysEventTargetProc) t,u) {}

    GlishSysEventProcTargetInfo &operator=(GlishSysEventProcTargetInfo &other);

    ~GlishSysEventProcTargetInfo();
    GlishSysEventTargetProc getGlishTargetProc() {return (GlishSysEventTargetProc) tgtProc;}
    uInt id() const;
};


} //# NAMESPACE CASA - END

#endif

