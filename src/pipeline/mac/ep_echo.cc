#include <lofar_config.h>
#include <Common/LofarLogger.h>
#include <MACIO/EventPort.h>
#include <MACIO/MACServiceInfo.h>
#include "Echo_Protocol.ph"

#include <boost/python.hpp>
#include <string>

using namespace LOFAR;
using namespace LOFAR::MACIO;

struct EP_Interface
{
    EventPort* echoPort;
    EP_Interface(std::string host = "") {
        this->echoPort = new EventPort("EchoServer:test", false, ECHO_PROTOCOL, host, true); 
    }
    double time_echo()
    {
        EchoPingEvent pingEvent;
        timeval pingTime;
        gettimeofday(&pingTime, 0);
        pingEvent.ping_time = pingTime;

        this->echoPort->send(&pingEvent);

        GCFEvent* ackPtr;
        ackPtr = echoPort->receive();
        EchoEchoEvent ack(*ackPtr);

        double  delta =  (1.0 * ack.echo_time.tv_sec + (ack.echo_time.tv_usec / 1000000.0));
                delta -= (1.0 * ack.ping_time.tv_sec + (ack.ping_time.tv_usec / 1000000.0));

        return delta;
    }
};

BOOST_PYTHON_MODULE(ep_echo)
{
    using namespace boost::python;

    // These are exposed as module-level attributes in Python
    scope().attr("PING") = ECHO_PING;
    scope().attr("ECHO") = ECHO_ECHO;
    scope().attr("CLOCK") = ECHO_CLOCK;

    class_<EP_Interface>("EP_Interface", init<optional<std::string> >())
        .def("time_echo", &EP_Interface::time_echo)
    ;
}
