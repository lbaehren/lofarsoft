#ifndef EP_ECHO_H
#define EP_ECHO_H

#include <lofar_config.h>
#include <Common/LofarLogger.h>
#include <sys/time.h>
#include "Echo_Protocol.ph"

class GenericEventWrapper {
private:
    LOFAR::MACIO::GCFEvent* my_event;
public:
    GenericEventWrapper() {
        this->my_event = new LOFAR::MACIO::GCFEvent;
    }
    GenericEventWrapper(LOFAR::MACIO::GCFEvent* event_ptr) {
        this->my_event = event_ptr;
    }
    virtual LOFAR::TYPES::uint16 get_signal() { return this->my_event->signal; }
    virtual LOFAR::MACIO::GCFEvent* get_event_ptr() { return my_event;}
};

class EchoPingEventWrapper : public GenericEventWrapper {
private:
    EchoPingEvent* my_event;
public:
    EchoPingEventWrapper();
    double get_pt();
    void set_pt(double);
    LOFAR::TYPES::uint16 get_seqnr();
    void set_seqnr(LOFAR::TYPES::uint16);
    virtual LOFAR::TYPES::uint16 get_signal() { return this->my_event->signal; }
    virtual EchoPingEvent* get_event_ptr() { return this->my_event; }
};

class EchoEchoEventWrapper : public GenericEventWrapper {
private:
    EchoEchoEvent* my_event;
public:
    EchoEchoEventWrapper(LOFAR::MACIO::GCFEvent*);
    EchoEchoEventWrapper(GenericEventWrapper&);
    double get_pt();
    double get_et();
    LOFAR::TYPES::uint16 get_seqnr();
    virtual LOFAR::TYPES::uint16 get_signal() { return this->my_event->signal; }
    virtual EchoEchoEvent* get_event_ptr() { return this->my_event; }
};

#endif
