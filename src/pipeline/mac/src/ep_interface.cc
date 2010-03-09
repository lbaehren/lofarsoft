#include "ep_interface.h"
#include <boost/python.hpp>

BOOST_PYTHON_MODULE(_ep_interface) {
    using namespace boost::python;

    // We export GenericEventWrapper here, but it is required by all the
    // protocols. Hence, ep.interface must always be imported first.
    class_<GenericEventWrapper>("GenericEvent")
        .add_property("signal", &GenericEventWrapper::get_signal)
    ;

    class_<EP_Interface>("EP_Interface", "EP_Interface(ServiceMask, Protocol, Host=localhost)", init<std::string, short, optional<std::string> >())
        .def("receive_event", &EP_Interface::receive_event, return_value_policy<manage_new_object>())
        .def("send_event", &EP_Interface::send_event)
    ;
}
