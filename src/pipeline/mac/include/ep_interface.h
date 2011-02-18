#ifndef EP_INTERFACE_H
#define EP_INTERFACE_H

/*!
  \file ep_interface.h
  \ingroup pipeline
*/

#include <boost/python.hpp>
#include <lofar_config.h>
#include <Common/LofarLogger.h>
#include <MACIO/EventPort.h>
#include <string>

#include "eventwrappers.h"

/*!
  \class EP_Interface
  \ingroup pipeline
  \brief Event Port Interface
*/
class EP_Interface {
private:
    LOFAR::MACIO::EventPort* my_EventPort;
public:
    EP_Interface(std::string servicename, short protocol_id, std::string host = "") {
        this->my_EventPort = new LOFAR::MACIO::EventPort(servicename, false, protocol_id, host, true);
    }
    GenericEventWrapper* receive_event() {
        LOFAR::MACIO::GCFEvent* ackPtr;
        Py_BEGIN_ALLOW_THREADS
        ackPtr = my_EventPort->receive();
        Py_END_ALLOW_THREADS
        return new GenericEventWrapper(ackPtr);
    }
    void send_event(GenericEventWrapper* wrapped_event) {
        this->my_EventPort->send(wrapped_event->get_event_ptr());
        }
    ~EP_Interface() { delete this->my_EventPort; }
};

#endif
