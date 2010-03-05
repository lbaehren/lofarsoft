#ifndef EP_ECHOWRAPPERS_H
#define EP_ECHOWRAPPERS_H

#include "Echo_Protocol.ph"
#include "eventwrappers.h"

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
