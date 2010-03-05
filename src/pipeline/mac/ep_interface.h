#ifndef EP_INTERFACE_H
#define EP_INTERFACE_H

#include <lofar_config.h>
#include <Common/LofarLogger.h>
#include <MACIO/EventPort.h>
#include <string>
//#include <MACIO/MACServiceInfo.h>

#include "eventwrappers.h"

//
// Event Port Interface
//

class EP_Interface {
private:
    LOFAR::MACIO::EventPort* echoPort;
public:
    EP_Interface(std::string host = "") {
        this->echoPort = new LOFAR::MACIO::EventPort("EchoServer:test", false, ECHO_PROTOCOL, host, true);
    }
    GenericEventWrapper receive_event() {
        LOFAR::MACIO::GCFEvent* ackPtr;
        ackPtr = echoPort->receive();
        GenericEventWrapper event(ackPtr);
        return event;
    }
    void send_event(GenericEventWrapper* wrapped_event) {
        this->echoPort->send(wrapped_event->get_event_ptr());
        }
};

#endif
