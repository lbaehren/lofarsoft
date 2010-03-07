#ifndef EP_EVENTWRAPPERS_H
#define EP_EVENTWRAPPERS_H

#include <lofar_config.h>
#include <Common/LofarLogger.h>
#include <sys/time.h>

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

#endif
