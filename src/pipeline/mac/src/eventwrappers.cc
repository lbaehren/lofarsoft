#include <lofar_config.h>
#include "echowrappers.h"

//
// Specific event types
//

// Ping event
EchoPingEventWrapper::EchoPingEventWrapper() {
    this->my_event = new EchoPingEvent;
    timeval ping_time;
    gettimeofday(&ping_time, 0);
    this->my_event->ping_time = ping_time;
}

double EchoPingEventWrapper::get_pt() {
    return 1.0 * this->my_event->ping_time.tv_sec + (this->my_event->ping_time.tv_usec / 1000000.0); 
}

void EchoPingEventWrapper::set_pt(double my_time) {
    this->my_event->ping_time.tv_sec = (int) my_time;
    this->my_event->ping_time.tv_usec = (int) (1000000.0 * (my_time - (int) my_time));
}

LOFAR::TYPES::uint16 EchoPingEventWrapper::get_seqnr() {
    return this->my_event->seqnr;
}

void EchoPingEventWrapper::set_seqnr(LOFAR::TYPES::uint16 my_seqnr) {
    this->my_event->seqnr = my_seqnr;
}

// Echo event
EchoEchoEventWrapper::EchoEchoEventWrapper(LOFAR::MACIO::GCFEvent* my_event) {
    this->my_event = new EchoEchoEvent(*my_event);
}

EchoEchoEventWrapper::EchoEchoEventWrapper(GenericEventWrapper& my_gev) {
    LOFAR::MACIO::GCFEvent* event_ptr;
    event_ptr = my_gev.get_event_ptr();
    this->my_event = new EchoEchoEvent(*event_ptr);
}

double EchoEchoEventWrapper::get_pt() {
    return 1.0 * this->my_event->ping_time.tv_sec + (this->my_event->ping_time.tv_usec / 1000000.0); 
}

double EchoEchoEventWrapper::get_et() {
    return 1.0 * this->my_event->echo_time.tv_sec + (this->my_event->echo_time.tv_usec / 1000000.0); 
}

LOFAR::TYPES::uint16 EchoEchoEventWrapper::get_seqnr() {
    return this->my_event->seqnr;
}
