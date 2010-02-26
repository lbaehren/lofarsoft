#include <lofar_config.h>
#include <Common/LofarLogger.h>
#include <MACIO/EventPort.h>
#include <MACIO/MACServiceInfo.h>
#include "Controller_Protocol.ph"

#include <boost/python.hpp>
#include <string>

//using namespace LOFAR;
//using namespace LOFAR::MACIO;

BOOST_PYTHON_MODULE(ep_control)
{
    using namespace boost::python;

    // These are exposed as module-level attributes in Python
    scope().attr("CONTROL_STARTED")   = CONTROL_STARTED;
    scope().attr("CONTROL_CONNECT")   = CONTROL_CONNECT;
    scope().attr("CONTROL_CONNECTED") = CONTROL_CONNECTED;
    scope().attr("CONTROL_RESYNC")    = CONTROL_RESYNC;
    scope().attr("CONTROL_RESYNCED")  = CONTROL_RESYNCED;
    scope().attr("CONTROL_SCHEDULE")  = CONTROL_SCHEDULE;
    scope().attr("CONTROL_SCHEDULED") = CONTROL_SCHEDULED;
    scope().attr("CONTROL_CLAIM")     = CONTROL_CLAIM;
    scope().attr("CONTROL_CLAIMED")   = CONTROL_CLAIMED;
    scope().attr("CONTROL_PREPARE")   = CONTROL_PREPARE;
    scope().attr("CONTROL_PREPARED")  = CONTROL_PREPARED;
    scope().attr("CONTROL_RESUME")    = CONTROL_RESUME;
    scope().attr("CONTROL_RESUMED")   = CONTROL_RESUMED;
    scope().attr("CONTROL_SUSPEND")   = CONTROL_SUSPEND;
    scope().attr("CONTROL_SUSPENDED") = CONTROL_SUSPENDED;
    scope().attr("CONTROL_RELEASE")   = CONTROL_RELEASE;
    scope().attr("CONTROL_RELEASED")  = CONTROL_RELEASED;
    scope().attr("CONTROL_QUIT")      = CONTROL_QUIT;
    scope().attr("CONTROL_QUITED")    = CONTROL_QUITED;
    scope().attr("CONTROL_COMMON")    = CONTROL_COMMON;
}
