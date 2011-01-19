#include <lofar_config.h>
#include <MACIO/Echo_Protocol.ph>
#include <boost/python.hpp>
#include "echowrappers.h"

BOOST_PYTHON_MODULE(_ep_echo) {
    using namespace boost::python;

    // These are exposed as module-level attributes in Python
    scope().attr("PROTOCOL") = (short) ECHO_PROTOCOL;
    scope().attr("PING") = ECHO_PING;
    scope().attr("ECHO") = ECHO_ECHO;
    scope().attr("CLOCK") = ECHO_CLOCK;

    class_<EchoPingEventWrapper, boost::noncopyable, bases<GenericEventWrapper> >("EchoPingEvent")
        .add_property("ping_time", &EchoPingEventWrapper::get_pt, &EchoPingEventWrapper::set_pt)
        .add_property("seqnr", &EchoPingEventWrapper::get_seqnr, &EchoPingEventWrapper::set_seqnr)
    ;

    class_<EchoEchoEventWrapper, bases<GenericEventWrapper> >("EchoEchoEvent", init<GenericEventWrapper&>())
        .add_property("ping_time", &EchoEchoEventWrapper::get_pt)
        .add_property("echo_time", &EchoEchoEventWrapper::get_et)
        .add_property("seqnr", &EchoEchoEventWrapper::get_seqnr)
    ;
}
