#include "Controller_Protocol.ph"
#include "controlwrappers.h"
#include <boost/python.hpp>

BOOST_PYTHON_MODULE(_ep_control)
{
    using namespace boost::python;
    
    // Module level attributes in Python
    
    // Protocol ID
    scope().attr("PROTOCOL")          = (short) CONTROLLER_PROTOCOL;

    // Possible errors
    scope().attr("OK")                = (short) CONTROL_OK_ERR;
    scope().attr("LOST_CONN")         = (short) CONTROL_LOST_CONN_ERR;

    // Possible signals
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

    // Events

    class_<GenericEventWrapper>("GenericEvent")
        .add_property("signal", &GenericEventWrapper::get_signal)
    ;

    // These events may be sent
    class_<CONTROLStartedEventWrapper, bases<GenericEventWrapper> >("ControlStartedEvent", init<std::string, bool>())
        .add_property("controller_name", &CONTROLStartedEventWrapper::get_cntlrName)
        .add_property("successful", &CONTROLStartedEventWrapper::get_successful)
    ;

    class_<CONTROLConnectedEventWrapper, bases<GenericEventWrapper> >("ControlConnectedEvent", init<std::string, uint16>())
        .add_property("controller_name", &CONTROLConnectedEventWrapper::get_cntlrName)
        .add_property("result", &CONTROLConnectedEventWrapper::get_result)
    ;

    class_<CONTROLResyncedEventWrapper, bases<GenericEventWrapper> >("ControlResyncedEvent", init<std::string, uint16>())
        .add_property("controller_name", &CONTROLResyncedEventWrapper::get_cntlrName)
        .add_property("result", &CONTROLResyncedEventWrapper::get_result)
    ;

    class_<CONTROLClaimedEventWrapper, bases<GenericEventWrapper> >("ControlClaimedEvent", init<std::string, uint16>())
        .add_property("controller_name", &CONTROLClaimedEventWrapper::get_cntlrName)
        .add_property("result", &CONTROLClaimedEventWrapper::get_result)
    ;

    class_<CONTROLPreparedEventWrapper, bases<GenericEventWrapper> >("ControlPreparedEvent", init<std::string, uint16>())
        .add_property("controller_name", &CONTROLPreparedEventWrapper::get_cntlrName)
        .add_property("result", &CONTROLPreparedEventWrapper::get_result)
    ;

    class_<CONTROLResumedEventWrapper, bases<GenericEventWrapper> >("ControlResumedEvent", init<std::string, uint16>())
        .add_property("controller_name", &CONTROLResumedEventWrapper::get_cntlrName)
        .add_property("result", &CONTROLResumedEventWrapper::get_result)
    ;

    class_<CONTROLSuspendedEventWrapper, bases<GenericEventWrapper> >("ControlSuspendedEvent", init<std::string, uint16>())
        .add_property("controller_name", &CONTROLSuspendedEventWrapper::get_cntlrName)
        .add_property("result", &CONTROLSuspendedEventWrapper::get_result)
    ;

    class_<CONTROLReleasedEventWrapper, bases<GenericEventWrapper> >("ControlReleasedEvent", init<std::string, uint16>())
        .add_property("controller_name", &CONTROLReleasedEventWrapper::get_cntlrName)
        .add_property("result", &CONTROLReleasedEventWrapper::get_result)
    ;

    class_<CONTROLQuitedEventWrapper, bases<GenericEventWrapper> >("ControlQuitedEvent", init<std::string, uint32, uint16, std::string>())
        .add_property("controller_name", &CONTROLQuitedEventWrapper::get_cntlrName)
        .add_property("result", &CONTROLQuitedEventWrapper::get_result)
        .add_property("treeID", &CONTROLQuitedEventWrapper::get_treeID)
        .add_property("error_message", &CONTROLQuitedEventWrapper::get_errorMsg)
    ;

    // These events may be received
    class_<CONTROLConnectEventWrapper, bases<GenericEventWrapper> >("ControlConnectEvent", init<GenericEventWrapper&>())
        .add_property("controller_name", &CONTROLConnectEventWrapper::get_cntlrName)
    ;

    class_<CONTROLClaimEventWrapper, bases<GenericEventWrapper> >("ControlClaimEvent", init<GenericEventWrapper&>())
        .add_property("controller_name", &CONTROLClaimEventWrapper::get_cntlrName)
    ;

    class_<CONTROLPrepareEventWrapper, bases<GenericEventWrapper> >("ControlPrepareEvent", init<GenericEventWrapper&>())
        .add_property("controller_name", &CONTROLPrepareEventWrapper::get_cntlrName)
    ;

    class_<CONTROLResumeEventWrapper, bases<GenericEventWrapper> >("ControlResumeEvent", init<GenericEventWrapper&>())
        .add_property("controller_name", &CONTROLResumeEventWrapper::get_cntlrName)
    ;

    class_<CONTROLSuspendEventWrapper, bases<GenericEventWrapper> >("ControlSuspendEvent", init<GenericEventWrapper&>())
        .add_property("controller_name", &CONTROLSuspendEventWrapper::get_cntlrName)
    ;

    class_<CONTROLReleaseEventWrapper, bases<GenericEventWrapper> >("ControlReleaseEvent", init<GenericEventWrapper&>())
        .add_property("controller_name", &CONTROLReleaseEventWrapper::get_cntlrName)
    ;
        
    class_<CONTROLQuitEventWrapper, bases<GenericEventWrapper> >("ControlQuitEvent", init<GenericEventWrapper&>())
        .add_property("controller_name", &CONTROLQuitEventWrapper::get_cntlrName)
    ;

    class_<CONTROLResyncEventWrapper, bases<GenericEventWrapper> >("ControlResyncEvent", init<GenericEventWrapper&>())
        .add_property("controller_name", &CONTROLResyncEventWrapper::get_cntlrName)
        .add_property("current_state", &CONTROLResyncEventWrapper::get_curState)
        .add_property("hostname", &CONTROLResyncEventWrapper::get_hostname)
    ;

    class_<CONTROLScheduleEventWrapper, bases<GenericEventWrapper> >("ControlScheduleEvent", init<GenericEventWrapper&>())
        .add_property("controller_name", &CONTROLScheduleEventWrapper::get_cntlrName)
        .add_property("start_time", &CONTROLScheduleEventWrapper::get_startTime)
        .add_property("stop_time", &CONTROLScheduleEventWrapper::get_stopTime)
    ;

}
