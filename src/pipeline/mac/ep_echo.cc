#include <lofar_config.h>
#include <Common/LofarLogger.h>
#include <MACIO/EventPort.h>
#include <MACIO/MACServiceInfo.h>
#include "Echo_Protocol.ph"

#include <boost/python.hpp>
#include <string>
#include <iostream>
#include <sys/time.h>

        
struct EP_Interface {
    LOFAR::MACIO::EventPort* echoPort;
    EP_Interface(std::string host = "") {
        this->echoPort = new LOFAR::MACIO::EventPort("EchoServer:test", false, ECHO_PROTOCOL, host, true);
    }
    void send_ping()
    {
        EchoPingEvent pingEvent;
        timeval pingTime;
        gettimeofday(&pingTime, 0);
        std::cout <<1.0 * pingTime.tv_sec + (pingTime.tv_usec / 1000000.0); 
        pingEvent.ping_time = pingTime;

        this->echoPort->send(&pingEvent);
    }
//    EchoEvent receive_event()
//    {
//        LOFAR::MACIO::GCFEvent* ackPtr;
//        ackPtr = echoPort->receive();
//        EchoEvent event(ackPtr);
//        return event;
//    }
    double time_echo()
    {
        EchoPingEvent pingEvent;
        timeval pingTime;
        gettimeofday(&pingTime, 0);
        pingEvent.ping_time = pingTime;

        this->echoPort->send(&pingEvent);

        LOFAR::MACIO::GCFEvent* ackPtr;
        ackPtr = echoPort->receive();
        if (ackPtr->signal == ECHO_ECHO) {
            EchoEchoEvent ack(*ackPtr);

            double  delta =  (1.0 * ack.echo_time.tv_sec + (ack.echo_time.tv_usec / 1000000.0));
                    delta -= (1.0 * ack.ping_time.tv_sec + (ack.ping_time.tv_usec / 1000000.0));

            return delta;
        }
    }
};

class PythonEvent {
public:
    void send(EP_Interface& send_interface) {
        send_interface.echoPort->send(this->evPtr);
    }
    long get_signal() {
        return this->evPtr->signal;
    }
protected:
    LOFAR::MACIO::GCFEvent* evPtr;
};

class EchoEvent: public PythonEvent {
public:
    EchoEvent(LOFAR::MACIO::GCFEvent* receivedPtr) {
        this->evPtr = receivedPtr;
    }
    double get_pt() {
        EchoEchoEvent my_event(*this->evPtr);
        return 1.0 * my_event.ping_time.tv_sec + (my_event.ping_time.tv_usec / 1000000.0);
    }
    double get_et() {
        EchoEchoEvent my_event(*this->evPtr);
        return 1.0 * my_event.echo_time.tv_sec + (my_event.echo_time.tv_usec / 1000000.0);
    }
};

class PingEvent : public PythonEvent {
public:
    PingEvent() {
        this->evPtr = new EchoPingEvent();
        EchoPingEvent my_event(*this->evPtr);
        timeval pingTime;
        gettimeofday(&pingTime, 0);
        my_event.ping_time = pingTime;
    }
    double get_pt() {
        EchoPingEvent my_event(*this->evPtr);
        return 1.0 * my_event.ping_time.tv_sec + (my_event.ping_time.tv_usec / 1000000.0);
    }
    //
    // Copy constructor... probably doing the wrong thing!
    PingEvent(const PingEvent&) {};
};

BOOST_PYTHON_MODULE(ep_echo) {
    using namespace boost::python;

    // These are exposed as module-level attributes in Python
    scope().attr("PING") = ECHO_PING;
    scope().attr("ECHO") = ECHO_ECHO;
    scope().attr("CLOCK") = ECHO_CLOCK;

    class_<PythonEvent>("PythonEvent", no_init)
        .add_property("signal", &PingEvent::get_signal)
    ;

    class_<PingEvent, bases<PythonEvent> >("PingEvent")
        .def("send", &PingEvent::send)
        .add_property("ping_time", &PingEvent::get_pt)
    ;

    class_<EchoEvent, bases<PythonEvent> >("EchoEvent", no_init)
        .add_property("ping_time", &EchoEvent::get_pt)
        .add_property("echo_time", &EchoEvent::get_et)
    ;

    class_<EP_Interface>("EP_Interface", init<optional<std::string> >())
        .def("time_echo", &EP_Interface::time_echo)
        .def("send_ping", &EP_Interface::send_ping)
//        .def("receive_event", &EP_Interface::receive_event)
    ;
}
