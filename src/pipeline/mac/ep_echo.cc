#include "ep_echo.h"
#include <boost/python.hpp>
        
EP_Interface::EP_Interface(std::string host = "") {
    this->echoPort = new LOFAR::MACIO::EventPort("EchoServer:test", false, ECHO_PROTOCOL, host, true);
}

void EP_Interface::send_ping() {
    EchoPingEvent pingEvent;
    timeval pingTime;
    gettimeofday(&pingTime, 0);
    std::cout <<1.0 * pingTime.tv_sec + (pingTime.tv_usec / 1000000.0); 
    pingEvent.ping_time = pingTime;

    this->echoPort->send(&pingEvent);
}

GenericEventWrapper EP_Interface::receive_event()
{
    LOFAR::MACIO::GCFEvent* ackPtr;
    ackPtr = echoPort->receive();
//    EchoEchoEventWrapper event(ackPtr);
    GenericEventWrapper event(ackPtr);
    std::cout << event.get_signal();
    return event;
}

double EP_Interface::time_echo()
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

//void EP_Interface::send_event(const PythonEvent& pev) {
//    this->echoPort->send(pev.evPtr);
//}

void EP_Interface::send_event(GenericEventWrapper* wrapped_event) {
//    this->echoPort->send(wrapped_event->get_event());
//    this->echoPort->send(&epe);
//    std::cout << "foo";
   this->echoPort->send(wrapped_event->get_event_ptr());
};

//void PythonEvent::send(EP_Interface& send_interface) {
//    send_interface.echoPort->send(this->evPtr);
//}

/*long PythonEvent::get_signal() {
    return this->evPtr->signal;
}

EchoEvent::EchoEvent(LOFAR::MACIO::GCFEvent* receivedPtr) {
    this->evPtr = receivedPtr;
}

double EchoEvent::get_pt() {
    EchoEchoEvent my_event(*this->evPtr);
    return 1.0 * my_event.ping_time.tv_sec + (my_event.ping_time.tv_usec / 1000000.0);
}

double EchoEvent::get_et() {
    EchoEchoEvent my_event(*this->evPtr);
    return 1.0 * my_event.echo_time.tv_sec + (my_event.echo_time.tv_usec / 1000000.0);
}

PingEvent::PingEvent() {
    this->evPtr = new EchoPingEvent();
    EchoPingEvent my_event(*this->evPtr);
    timeval pingTime;
    gettimeofday(&pingTime, 0);
    std::cout << pingTime.tv_sec << '\n';
    my_event.ping_time = pingTime;
    EchoPingEvent test(*this->evPtr);
    std::cout << test.ping_time.tv_sec;
}

double PingEvent::get_pt() {
    EchoPingEvent my_event(*this->evPtr);
    return 1.0 * my_event.ping_time.tv_sec + (my_event.ping_time.tv_usec / 1000000.0);
}*/

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


//    class_<EchoPingEvent, boost::noncopyable>("EchoPingEvent")
//        .def_readonly("signal", &EchoPingEvent::signal)
//        .def_readwrite("seqnr", &EchoPingEvent::seqnr)
//        .def_readwrite("ping_time", &EchoPingEvent::ping_time)
//    ;
/*    class_<PythonEvent>("PythonEvent", no_init)
        .add_property("signal", &PingEvent::get_signal)
    ;
    class_<timeval>("timeval")
        .def_readwrite("tv_sec", &timeval::tv_sec)
        .def_readwrite("tv_usec", &timeval::tv_usec)
    ;

    class_<PingEvent, bases<PythonEvent> >("PingEvent")
//        .def("send", &PingEvent::send)
        .add_property("ping_time", &PingEvent::get_pt)
    ;

    class_<EchoEvent, bases<PythonEvent> >("EchoEvent", no_init)
        .add_property("ping_time", &EchoEvent::get_pt)
        .add_property("echo_time", &EchoEvent::get_et)
    ;
*/

    class_<EP_Interface>("EP_Interface", init<optional<std::string> >())
//        .def("time_echo", &EP_Interface::time_echo)
//        .def("send_ping", &EP_Interface::send_ping)
        .def("receive_event", &EP_Interface::receive_event)
        .def("send_event", &EP_Interface::send_event)
    ;
}
