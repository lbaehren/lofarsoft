#include "Echo_Protocol.ph"
#include "ep_interface.h"
#include "eventwrappers.h"
#include <boost/python.hpp>

BOOST_PYTHON_MODULE(ep_echo) {
    using namespace boost::python;

    // These are exposed as module-level attributes in Python
    scope().attr("PROTOCOL") = (short) ECHO_PROTOCOL;
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

    class_<EP_Interface>("EP_Interface", "EP_Interface(ServiceMask, Protocol, Host=localhost)", init<std::string, short, optional<std::string> >())
        .def("receive_event", &EP_Interface::receive_event)
        .def("send_event", &EP_Interface::send_event)
    ;
}
