#ifndef EP_ECHO_H
#define EP_ECHO_H

#include <lofar_config.h>
#include <Common/LofarLogger.h>
#include <MACIO/EventPort.h>
#include <MACIO/MACServiceInfo.h>
#include "Echo_Protocol.ph"

#include <boost/python.hpp>
#include <string>
#include <sys/time.h>
#include <iostream>

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
    EchoPingEventWrapper() {
        this->my_event = new EchoPingEvent;
    }
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
    EchoEchoEventWrapper(LOFAR::MACIO::GCFEvent* my_event) {
        this->my_event = new EchoEchoEvent(*my_event);
    }
    EchoEchoEventWrapper(GenericEventWrapper& my_gev) {
        LOFAR::MACIO::GCFEvent* event_ptr;
        event_ptr = my_gev.get_event_ptr();
        this->my_event = new EchoEchoEvent(*event_ptr);
    }
    double get_pt();
    double get_et();
    LOFAR::TYPES::uint16 get_seqnr();
    virtual LOFAR::TYPES::uint16 get_signal() { return this->my_event->signal; }
    virtual EchoEchoEvent* get_event_ptr() { return this->my_event; }
};
        
class EP_Interface {
public:
    LOFAR::MACIO::EventPort* echoPort;
    EP_Interface(std::string);

    GenericEventWrapper receive_event();
    void send_event(GenericEventWrapper*);
};

#endif
