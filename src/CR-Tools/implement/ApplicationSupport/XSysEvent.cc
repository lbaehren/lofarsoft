//# XSysEvent.h: Wrappers for X event classes
//# Copyright (C) 1994,1995,1996,2000,2001,2002
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

#if defined(AIPS_SUN4_X11R4)
#define XTVAAPPINITIALIZE_CAST (Cardinal*)
#define XTFUNCPROTO
#else
#define XTVAAPPINITIALIZE_CAST
#endif

#include <ApplicationSupport/XSysEvent.h>
#include <ApplicationSupport/GlishEvent.h>
#include <casa/BasicSL/String.h>
#include <casa/iostream.h>

#ifdef List
#undef List
#endif
#define List GlishList
#include <Glish/Client.h>
#undef List

#include <graphics/Graphics/X11Intrinsic.h>

namespace casa { //# NAMESPACE CASA - BEGIN

#define UNKNOWNSYSEVENT	"unknown"

static char *XSysEventTypeMap[] = {
UNKNOWNSYSEVENT,
UNKNOWNSYSEVENT,
"KeyPress",
"KeyRelease",
"ButtonPress",
"ButtonRelease",
"MotionNotify",
"EnterNotify",
"LeaveNotify",
"FocusIn",
"FocusOut",
"KeymapNotify",
"Expose",
"GraphicsExpose",
"NoExpose",
"VisibilityNotify",
"CreateNotify",
"DestroyNotify",
"UnmapNotify",
"MapNotify",
"MapRequest",
"ReparentNotify",
"ConfigureNotify",
"ConfigureRequest",
"GravityNotify",
"ResizeRequest",
"CirculateNotify",
"CirculateRequest",
"PropertyNotify",
"SelectionClear",
"SelectionRequest",
"SelectionNotify",
"ColormapNotify",
"ClientMessage",
"MappingNotify",
"LASTEvent",
};


struct XSysEventSourceInfo_XGUTS {
    XtInputId input_ids[FD_SETSIZE];
    static void callback(XtPointer data, int * /*fd*/, XtInputId *);
};

void XSysEventSourceInfo_XGUTS::callback(XtPointer data, int * /*fd*/, XtInputId *) {
    XSysEventSourceInfo *info = (XSysEventSourceInfo *) data;
    if ( info && info->src )
	info->src->invokeTarget();
    else
	cerr << "Error bad info for XSysEventSource target in " <<
	    __FILE__ << " line " << __LINE__ << endl;
}

XSysEventSourceInfo::XSysEventSourceInfo(SysEventSource *s) : src(s) , 
					x(new XSysEventSourceInfo_XGUTS) {
    memset(&fdset, 0, sizeof(fd_set));
    memset(x->input_ids, 0, FD_SETSIZE * sizeof(XtInputId));
}

XSysEventSourceInfo::~XSysEventSourceInfo() {
    delete x;
}

XSysEvent::XSysEvent() : SysEvent(), event(new XEvent) {}

SysEvent::Group XSysEvent::group() const {
    return SysEvent::XGroup;
}

String XSysEvent::type() const {
    if (xtype() >= LASTEvent)
	return String(UNKNOWNSYSEVENT);
    else
	return String(XSysEventTypeMap[xtype()]);
}

int XSysEvent::xtype() const { return event->type;}


Bool XSysEvent::dispatch(SysEventTarget &ev) {
    if (ev.group() == group())
	return ((XSysEventTarget &)ev).handle(*this);
    else
	return (0);
}

Bool XSysEvent::dispatch(SysEventTarget &ev, SysEventSource &xs) {
    if (ev.group() == group())
	return ((XSysEventTarget &)ev).handle(*this,xs);
    else
	return (0);
}

XSysEvent::~XSysEvent() {}


XSysEventSource::XSysEventSource(int argc, char **argv, const char *name) : 
                           context_(new XtAppContext), toplevel_(new Widget),
			   srcInfo(0) {
    *context_ = XtCreateApplicationContext();
    *toplevel_ = XtVaAppInitialize (&context(),name,NULL, 0,
				  XTVAAPPINITIALIZE_CAST &argc,
				  argv, NULL, NULL, 0);
}

XSysEventSource::XSysEventSource(int argc, char **argv, void *options, char **fallback_resources,
				 const char *name) : 
                           context_(new XtAppContext), toplevel_(new Widget),
			   srcInfo(0) {
    *context_ = XtCreateApplicationContext();
    *toplevel_ = XtVaAppInitialize (&context(),name, (XrmOptionDescRec*) options,
				    XtNumber( ((XrmOptionDescRec*)options) ),
				    XTVAAPPINITIALIZE_CAST &argc,
				    argv, fallback_resources, NULL, 0);
}


SysEvent::Group XSysEventSource::group() const {
    return SysEvent::XGroup;
}

XtAppContext &XSysEventSource::context() {return *context_;}

SysEvent XSysEventSource::nextEvent() {
    XEvent *event = new XEvent;
    XtAppNextEvent(context(),event);
    return SysEvent(new XSysEvent(event,this),0);
}

Bool XSysEventSource::waitingEvent() {
    return(XtAppPending(context()) ? (1) : (0));
}

Bool XSysEventSource::connected() {
    return((1));
}

XSysEventSource::~XSysEventSource() {
    if ( srcInfo ) delete srcInfo;
}

SysEvent::Group XSysEventTarget::group() const {
    return SysEvent::XGroup;
}

SysEventTarget *XSysEventTarget::clone() const {
    return new XSysEventTarget();
}

Bool XSysEventSource::canCombine(const SysEventSource &other) const {
    if ( other.group() == SysEvent::GlishGroup )
	return (1);
    else
	return (0);
}

Bool XSysEventSource::canCombine(const SysEventSource *other) const {
    if ( other && other->group() == SysEvent::GlishGroup )
	return (1);
    else
	return (0);
}

Bool XSysEventSource::combine(SysEventSource &src)
	{ return combine( &src ); }
Bool XSysEventSource::combine(SysEventSource *src) {
    if ( src && src->group() == SysEvent::GlishGroup )
	return addSource((GlishSysEventSource *) src);
    else
	return (0);
}

Bool XSysEventSource::addTarget(SysEventTarget &, const String &)
	{ return (0); }
Bool XSysEventSource::addTarget(SysEventTarget *, const String &, Bool)
	{ return (0); }
Bool XSysEventSource::addTarget(SysEventTargetProc, const String &, void *)
	{ return (0); }

void XSysEventSource::invokeTarget() { }

Bool XSysEventSource::setDefault(SysEventTarget &)
	{ return (0); }
Bool XSysEventSource::setDefault(SysEventTarget *, Bool)
	{ return (0); }
Bool XSysEventSource::setDefault(SysEventTargetProc, void *)
	{ return (0); }

void XSysEventSource::setFD(int fd, XSysEventSourceInfo *info) {
    FD_SET(fd, &(*info).fdset);
    (*info).x->input_ids[fd] = XtAppAddInput(*context_, fd, (XtPointer) XtInputReadMask, 
					     XSysEventSourceInfo_XGUTS::callback, info);
}

void XSysEventSource::clrFD(int fd, XSysEventSourceInfo *info) {
    FD_CLR(fd, &(*info).fdset);
    XtRemoveInput((*info).x->input_ids[fd]);
}

Bool XSysEventSource::addSource(GlishSysEventSource *other) {
    XSysEventSourceInfo *info = new XSysEventSourceInfo(other);
    if ( ! srcInfo )
	srcInfo = new ListIter<CountedPtr<XSysEventSourceInfo> >( 
					new List<CountedPtr<XSysEventSourceInfo> >(),
					(1)
	    				);

    int num = (*other).client().AddInputMask(&(*info).fdset);
    for (int cnt=0; num && cnt < FD_SETSIZE; cnt++)
	if ( FD_ISSET(cnt, &(*info).fdset )) {
	    setFD(cnt,info);
	    --num;
	}
    (*srcInfo).toStart();
    (*srcInfo).addRight(info);
    return (1);
}

Bool XSysEventSource::loop() {
    XtAppMainLoop(*context_);
    return (1);
}

Bool XSysEventTarget::handle(SysEvent ev) {
    return ev.dispatch(*this);
}

Bool XSysEventTarget::handle(XSysEvent &ev) {
    XtDispatchEvent(ev.xevent());
    return (1);
}
Bool XSysEventTarget::handle(XSysEvent &ev, SysEventSource &) {
    XtDispatchEvent(ev.xevent());
    return (1);
}

XSysEventTarget::~XSysEventTarget() {}


} //# NAMESPACE CASA - END

