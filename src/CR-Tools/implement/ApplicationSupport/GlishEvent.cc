//# GlishEvent.cc: Wrapper for a Glish events 
//# Copyright (C) 1995,1994,1995,1996,1997,1998,1999,2000,2001,2002,2003
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

#include <casa/Containers/List.h>
#include <casa/BasicSL/String.h>
#include <casa/Utilities/Regex.h>

#include <ApplicationSupport/GlishEvent.h>
#include <ApplicationSupport/GlishArray.h>
#include <casa/Exceptions/Error.h>

#include <casa/Utilities/CountedPtr.h>
#include <casa/Utilities/Assert.h>


#include <Glish/Value.h>
#include <Glish/Client.h>

#include <sys/time.h> // For timeval - hopefully this is portable!

namespace casa { //# NAMESPACE CASA - BEGIN

//# To shut up cpp
#undef List



GlishEvent *GlishSysEventSource::nextPrimitiveEvent(::timeval *timeout, 
						    int &timedout) {

    GlishEvent *ev = client_->NextEvent(timeout, timedout);

    if ( !ev && !timedout ) {
	static GlishEvent endit(strdup("shutdown"),new Value(0));
	valid = False;
	return &endit;
    }
	
    return(ev);
}
  
SysEvent GlishSysEventSource::nextEvent() {
    int timedout;
    GlishEvent *ev = nextPrimitiveEvent(0, timedout);
    return SysEvent(new GlishSysEvent(ev,this),0);
}

GlishSysEvent GlishSysEventSource::nextGlishEvent() {
    int timedout;
    GlishEvent *ev = nextPrimitiveEvent(0, timedout);
    AlwaysAssert(ev != 0, AipsError);
    return GlishSysEvent(ev,this);
}

Bool GlishSysEventSource::nextGlishEvent(GlishSysEvent &event, 
					 Int timeOutInMillisec)
{
    if (timeOutInMillisec < 0) {
	event = nextGlishEvent();
	return True;
    } else {
	int timedout;
	struct timeval timeout;
	timeout.tv_usec = (timeOutInMillisec % 1000) * 1000;
	timeout.tv_sec = (timeOutInMillisec / 1000);
	GlishEvent *ev = nextPrimitiveEvent(&timeout, timedout);
	if (ev && !timedout) {
	    event = GlishSysEvent(ev, this);
	    return True;
	} else {
	    return False;
	}
    }
}


Bool GlishSysEventSource::waitingEvent() {

    fd_set selectSet;
    timeval tv;
    FD_ZERO(&selectSet);
    client_->AddInputMask(&selectSet);
    tv.tv_sec = 0;
    tv.tv_usec = 0;

#if defined(AIPS_HPUX9) || defined(HPUX11)
    if (valid && select(FD_SETSIZE, (int*) &selectSet, 0, 0, &tv) > 0)
#else
    if (valid && select(FD_SETSIZE, &selectSet, 0, 0, &tv) > 0)
#endif
	return(client_->HasClientInput(&selectSet) ? True : False);
    else
	return False;
}

Bool GlishSysEventSource::connected() {
    return valid;
}

Bool GlishSysEventSource::hasInterpreter() {
    if ( connected() && client_->HasInterpreterConnection() )
	return True;
    else
	return False;
}

GlishSysEventSource::GlishSysEventSource(int &argc,char **argv) :
                   SysEventSource(), tgtInfo(0), defaultTgt(0), valid(True) { 
    client_ = new Client(argc,argv);
    if (!client_)
	throw_sysevent_init_error();
}

void GlishSysEventSource::postEvent(const String &name, const String &value) {
    if (valid) {
	if (name.length() < 1) 
	    throw_invalid_event();
	client_->PostEvent(name.chars(), value.chars());
      }
}
    
void GlishSysEventSource::postEvent(const char *name, const char *value) {
    if (valid) {
	if (strlen(name) < 1) 
	    throw_invalid_event();
        client_->PostEvent(name, value);
    }
}

void GlishSysEventSource::postEvent(const String &name, const char *value) {
    if (valid) {
	if (name.length() < 1) 
	    throw_invalid_event();
	client_->PostEvent(name.chars(), value);
    }
}
    
void GlishSysEventSource::postEvent(const char *name, const String &value) {
    if (valid) {
	if (strlen(name) < 1)
	    throw_invalid_event();
	client_->PostEvent(name, value.chars());
    }
}

void GlishSysEventSource::postEvent(const String &name, const Value *val) {
    if (valid) {
	if (name.length() < 1)
	    throw_invalid_event();
	client_->PostEvent(name.chars(), val);
    }
}

void GlishSysEventSource::postEvent(const String &name, const GlishValue &val) {
    if (valid) {
	if (name.length() < 1) 
	    throw_invalid_event();
	client_->PostEvent(name.chars(), val.value_p);
    }
}

void GlishSysEventSource::postEvent(const String &name, const GlishArray &val) {
    if (valid) { 
	if (name.length() < 1)
	    throw_invalid_event();
	client_->PostEvent(name.chars(), val.value_p);
    }
}

void GlishSysEventSource::postEvent(const String &name, const GlishValue *val) {
    if (valid && val) { 
        if (name.length() < 1) 
	    throw_invalid_event();
	client_->PostEvent(name.chars(), val->value_p);
    }
}

void GlishSysEventSource::reply(const GlishValue &val) {
    if (valid)
	client_->Reply(val.value_p);
}

void GlishSysEventSource::reply(const GlishValue *val) {
    if (val && valid)
	client_->Reply(val->value_p);
}

Bool GlishSysEventSource::replyPending() const {
    if (valid)
	return client_->ReplyPending() ? True : False;
    return False;
}

void GlishSysEventSource::unrecognized() {
    if (valid)
	client_->Unrecognized();
}

Int GlishSysEventSource::addInputMask(fd_set *selectSet) {
    Int num = client_->AddInputMask(selectSet);
    return num;
}

void GlishSysEventSource::Error( const String &msg ) {
    if (valid)
	client_->Error(msg.chars());
}

SysEvent::Group GlishSysEventSource::group() const {
    return SysEvent::GlishGroup;
}

GlishSysEventSource::~GlishSysEventSource() {
    if ( tgtInfo ) delete tgtInfo;
    if ( defaultTgt ) delete defaultTgt;
    if ( client_ ) delete client_;
}

//#  ***  Start event driven interface ***
Bool GlishSysEventSource::canCombine(const SysEventSource &) const 
	{ return False; }
Bool GlishSysEventSource::canCombine(const SysEventSource *) const 
	{ return False; }
Bool GlishSysEventSource::combine(SysEventSource &)
	{ return False; }
Bool GlishSysEventSource::combine(SysEventSource *)
	{ return False; }

Bool GlishSysEventSource::addTarget(SysEventTarget &tgt, const String &regx) {
    if ( tgt.group() != SysEvent::GlishGroup )
	return False;
    return addTarget( tgt.clone(), regx, True);
}
Bool GlishSysEventSource::addTarget(SysEventTarget *tgt, const String &regx,
				    Bool ownTarget) {
    if ( ! tgt || (*tgt).group() != SysEvent::GlishGroup )
	return False;
    SysEventTarget *newTgt = ownTarget ? tgt : (*tgt).clone();
    if ( ! tgtInfo )
	tgtInfo = new List<CountedPtr<SysEventTargetInfo> >;

    ListIter<CountedPtr<SysEventTargetInfo> > iter(tgtInfo);
    iter.toEnd();
    iter.addRight(new SysEventTargetInfo(newTgt,regx));
    return True;
}

Bool GlishSysEventSource::addTarget(SysEventTargetProc tgt, const String &regx,
				    void *userData) {
    if ( !tgt )
	return False;
    if ( ! tgtInfo )
	tgtInfo = new List<CountedPtr<SysEventTargetInfo> >;

    ListIter<CountedPtr<SysEventTargetInfo> > iter(tgtInfo);
    iter.toEnd();
    iter.addRight(new SysEventTargetInfo(tgt,regx,userData));
    return True;
}

Bool GlishSysEventSource::setDefault(SysEventTarget &tgt) {
    if ( tgt.group() != SysEvent::GlishGroup )
	return False;
    return setDefault(tgt.clone(),True);
}

Bool GlishSysEventSource::setDefault(SysEventTarget *tgt, Bool ownTarget) {
    if ( ! tgt || (*tgt).group() != SysEvent::GlishGroup )
	return False;
    SysEventTarget *newTgt = ownTarget ? tgt : (*tgt).clone();
    if (defaultTgt)
	delete defaultTgt;
    defaultTgt = new SysEventTargetInfo(newTgt);
    return True;
}

Bool GlishSysEventSource::setDefault(SysEventTargetProc tgt, void *userData) {
    if ( !tgt )
	return False;
    if (defaultTgt)
	delete defaultTgt;
    defaultTgt = new SysEventTargetInfo(tgt,userData);
    return True;
}

Bool GlishSysEventSource::invokeTarget(GlishEvent *ev) {
    Bool Done = False;
    if ( tgtInfo ) {

	ListIter<CountedPtr<SysEventTargetInfo> > iter(tgtInfo);

	for (iter.toStart(); ! iter.atEnd() && ! Done; iter++) {
	    SysEventTargetInfo &cur = *iter.getRight();
	    const char *str = (*ev).Name();
	    if ( cur.getRegex().match(str,strlen(str)) != String::npos ) {

#define GLISHSYSEVENTSOURCE_INVOKETARGET_ACTION					\
	if ( cur.id() == Register(static_cast<GlishSysEventProcTargetInfo*>(0)) ) {		\
	    GlishSysEvent temp(ev,this);					\
	    Done = (*((GlishSysEventProcTargetInfo&)cur).getGlishTargetProc())(temp,\
							 cur.getUserData());	\
	} else {								\
	    switch ( cur.type() ) {						\
	    case SysEventTargetInfo::ProcTgt:					\
		{  GlishSysEvent temp(ev,this);					\
		   Done = (*cur.getTargetProc())(temp, cur.getUserData());	\
		}								\
		break;								\
	    case SysEventTargetInfo::ClassTgt:					\
		Done = cur.getTarget().handle(SysEvent(new GlishSysEvent(ev,this),0));\
		break;								\
	    default: break;							\
	    }									\
	}

		GLISHSYSEVENTSOURCE_INVOKETARGET_ACTION
	    } 
	}

    }
    if ( ! Done && defaultTgt ) {
	SysEventTargetInfo &cur = *defaultTgt;
	GLISHSYSEVENTSOURCE_INVOKETARGET_ACTION
    }
    return Done;
}

void GlishSysEventSource::processEvent( GlishSysEvent e ) {
    if ( valid && ! invokeTarget(e.event) && strcmp(e.event->Name(),"shutdown" ) )
	client_->Unrecognized();
}

Bool GlishSysEventSource::loop() {
    GlishEvent *ev;
    int timedout;
    while ( valid ) {
	ev = nextPrimitiveEvent(0, timedout);
	if ( ! invokeTarget(ev) && strcmp(ev->Name(),"shutdown" ) )
	    client_->Unrecognized();
    }
    return True;
}

void GlishSysEventSource::invokeTarget() {
    if ( valid && waitingEvent() ) {
	int timedout;
	GlishEvent *ev = nextPrimitiveEvent(0, timedout);
	if ( ! invokeTarget(ev) && strcmp(ev->Name(),"shutdown") )
	    client_->Unrecognized();
    }
}

//#  ***  End event driven interface ***

SysEvent::Group GlishSysEvent::group() const {
    return SysEvent::GlishGroup;
}

GlishSysEvent::GlishSysEvent(GlishEvent *ne, SysEventSource *xs) : SysEvent(xs), event(ne), val_(0) { 
    Ref(event);
}

GlishSysEvent::GlishSysEvent(const GlishSysEvent &other) : SysEvent(other.src), event(other.event), val_(0) { 
    Ref(event);
}
GlishSysEvent::GlishSysEvent(const GlishSysEvent *other) : SysEvent((*other).src), event((*other).event), val_(0) {
    Ref(event);
}

GlishSysEvent &GlishSysEvent::operator=(const GlishSysEvent &other) {
    if (this == &other)
	return *this;

    if (event) {
	Unref(event);
	event = 0;
    }
    if (val_) {
	delete val_;
	val_ = 0;
    }
    event = other.event;
    if (event)
	Ref(event);
    return *this;
}

GlishSysEvent::~GlishSysEvent() { 
    if (event)
	Unref(event);
    if (val_)
	delete val_;
}

void GlishSysEvent::fillVal() const {
    if (event && !val_) {
	Value *v = event->value->Deref();
	GlishSysEvent *This = (GlishSysEvent *)this;
	This->val_ = new GlishValue(v);
    }
}

String GlishSysEvent::type() const {
    if (event)
	return String(event->name);
    else
	return String("");
}

Bool GlishSysEvent::dispatch(SysEventTarget &ev) {
    if (ev.group() == group())
	return ((GlishSysEventTarget &)ev).handle(*this);
    else
	return False;
}

Bool GlishSysEvent::dispatch(SysEventTarget &ev, SysEventSource &xs) {
    if (ev.group() == group())
	return ((GlishSysEventTarget &)ev).handle(*this,xs);
    else
	return False;
}

Bool GlishSysEvent::isRequest() const {
    if ( event )
	return event->IsRequest() ? True : False;
    else
	return False;
}

Bool GlishSysEvent::isReply() const {
    if ( event )
	return event->IsReply() ? True : False;
    else
	return False;
}

SysEvent *GlishSysEvent::clone() const {
    return new GlishSysEvent(this);
}

SysEvent::Group GlishSysEventTarget::group() const {
    return SysEvent::GlishGroup;
}

SysEventTarget *GlishSysEventTarget::clone() const {
    if (cb_one)
	return new GlishSysEventTarget(cb_one);
    else
	return new GlishSysEventTarget(cb_two);
}

Bool GlishSysEventTarget::handle(SysEvent ev) {
    return ev.dispatch(*this);
}
Bool GlishSysEventTarget::handle(GlishSysEvent &ev) {
    if (cb_one)
	return (*cb_one)(ev);
    return False;
}
Bool GlishSysEventTarget::handle(GlishSysEvent &ev, SysEventSource &es) {
    if (cb_two)
	return (*cb_two)(ev,es);
    else if (cb_one)
	return (*cb_one)(ev);
    return False;
}

GlishSysEventTarget::~GlishSysEventTarget() {}

uInt GlishSysEventProcTargetInfo::id() const { return Register(this); }

GlishSysEventProcTargetInfo &GlishSysEventProcTargetInfo::operator=(GlishSysEventProcTargetInfo &other) {
    tgtProc = other.tgtProc;
    userData = other.userData;
    regex = other.regex;
    return *this;
}

GlishSysEventProcTargetInfo::~GlishSysEventProcTargetInfo() {}

void throw_invalid_access() {
    throw(AipsError("Invalid Accessor Used."));
}

void throw_uninitialzed_glishsysevent() {
    throw(AipsError("Uninitialized GlishSysEvent."));
}

void throw_invalid_event() {
    throw(AipsError("Invalid Event Name Used."));
}

} //# NAMESPACE CASA - END

