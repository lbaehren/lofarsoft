#include "ep_echo.h"
#include <boost/python.hpp>
        
EP_Interface::EP_Interface(std::string host = "") {
    this->echoPort = new LOFAR::MACIO::EventPort("EchoServer:test", false, ECHO_PROTOCOL, host, true);
}

GenericEventWrapper EP_Interface::receive_event()
{
    LOFAR::MACIO::GCFEvent* ackPtr;
    ackPtr = echoPort->receive();
    GenericEventWrapper event(ackPtr);
    return event;
}

void EP_Interface::send_event(GenericEventWrapper* wrapped_event) {
   this->echoPort->send(wrapped_event->get_event_ptr());
};


double EchoPingEventWrapper::get_pt() {
    return 1.0 * this->my_event->ping_time.tv_sec + (this->my_event->ping_time.tv_usec / 1000000.0); 
}

void EchoPingEventWrapper::set_pt(double my_time) {
    this->my_event->ping_time.tv_sec = (int) my_time;
    this->my_event->ping_time.tv_usec = 1000000.0 * (my_time - (int) my_time);
}

LOFAR::TYPES::uint16 EchoPingEventWrapper::get_seqnr() {
    return this->my_event->seqnr;
}

void EchoPingEventWrapper::set_seqnr(LOFAR::TYPES::uint16 my_seqnr) {
    this->my_event->seqnr = my_seqnr;
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


BOOST_PYTHON_MODULE(ep_echo) {
    using namespace boost::python;

    // These are exposed as module-level attributes in Python
    scope().attr("PING") = ECHO_PING;
    scope().attr("ECHO") = ECHO_ECHO;
    scope().attr("CLOCK") = ECHO_CLOCK;

    class_<GenericEventWrapper>("GenericEvent")
        .add_property("signal", &GenericEventWrapper::get_signal)
    ;

    class_<EchoPingEventWrapper, boost::noncopyable, bases<GenericEventWrapper> >("EchoPingEvent")
        .add_property("ping_time", &EchoPingEventWrapper::get_pt, &EchoPingEventWrapper::set_pt)
        .add_property("seqnr", &EchoPingEventWrapper::get_seqnr, &EchoPingEventWrapper::set_seqnr)
    ;

    class_<EchoEchoEventWrapper, bases<GenericEventWrapper> >("EchoEchoEvent", init<GenericEventWrapper&>())
        .add_property("ping_time", &EchoEchoEventWrapper::get_pt)
        .add_property("echo_time", &EchoEchoEventWrapper::get_et)
        .add_property("seqnr", &EchoEchoEventWrapper::get_seqnr)
    ;

    class_<EP_Interface>("EP_Interface", init<optional<std::string> >())
        .def("receive_event", &EP_Interface::receive_event)
        .def("send_event", &EP_Interface::send_event)
    ;
}
