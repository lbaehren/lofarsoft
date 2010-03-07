#include "ep_interface.h"
#include <boost/python.hpp>

BOOST_PYTHON_MODULE(_ep_interface) {
    using namespace boost::python;

    class_<EP_Interface>("EP_Interface", "EP_Interface(ServiceMask, Protocol, Host=localhost)", init<std::string, short, optional<std::string> >())
        .def("receive_event", &EP_Interface::receive_event, return_value_policy<manage_new_object>())
        .def("send_event", &EP_Interface::send_event)
    ;
}
