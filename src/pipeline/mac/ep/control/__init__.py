# Note that we require GenericEvent, so ep.interface must be available first
# We'll use it to create a customised EventPort interface
from ep.interface import EventPort_Interface, EventNotFoundException

# Module level constants

# Protocol ID
from _ep_control import PROTOCOL

# Result/error states
from _ep_control import OK, LOST_CONN

# Event signals
from _ep_control import CONTROL_STARTED
from _ep_control import CONTROL_CONNECT
from _ep_control import CONTROL_CONNECTED
from _ep_control import CONTROL_RESYNC
from _ep_control import CONTROL_RESYNCED
from _ep_control import CONTROL_SCHEDULE
from _ep_control import CONTROL_SCHEDULED
from _ep_control import CONTROL_CLAIM
from _ep_control import CONTROL_CLAIMED
from _ep_control import CONTROL_PREPARE
from _ep_control import CONTROL_PREPARED
from _ep_control import CONTROL_RESUME
from _ep_control import CONTROL_RESUMED
from _ep_control import CONTROL_SUSPEND
from _ep_control import CONTROL_SUSPENDED
from _ep_control import CONTROL_RELEASE
from _ep_control import CONTROL_RELEASED
from _ep_control import CONTROL_QUIT
from _ep_control import CONTROL_QUITED

# Events we might receive
from _ep_control import ControlConnectEvent
from _ep_control import ControlClaimEvent
from _ep_control import ControlPrepareEvent
from _ep_control import ControlResumeEvent
from _ep_control import ControlSuspendEvent
from _ep_control import ControlReleaseEvent
from _ep_control import ControlQuitEvent
from _ep_control import ControlResyncEvent
from _ep_control import ControlScheduleEvent

# Events we can send
from _ep_control import ControlConnectedEvent
from _ep_control import ControlResyncedEvent
from _ep_control import ControlClaimedEvent
from _ep_control import ControlPreparedEvent
from _ep_control import ControlResumedEvent
from _ep_control import ControlSuspendedEvent
from _ep_control import ControlReleasedEvent
from _ep_control import ControlQuitedEvent
from _ep_control import ControlScheduledEvent

class ControllerPort_Interface(EventPort_Interface):
    def __init__(self, servicemask, hostname):
        event_mapping = {
            CONTROL_CONNECTED:  ControlConnectedEvent,
            CONTROL_RESYNC:   ControlResyncEvent,
            CONTROL_SCHEDULE: ControlScheduleEvent,
            CONTROL_CLAIM:    ControlClaimEvent,
            CONTROL_PREPARE:  ControlPrepareEvent,
            CONTROL_RESUME:   ControlResumeEvent,
            CONTROL_SUSPEND:  ControlSuspendEvent,
            CONTROL_RELEASE:  ControlReleaseEvent,
            CONTROL_QUIT:     ControlQuitEvent
        }
        super(ControllerPort_Interface, self).__init__(servicemask, PROTOCOL, event_mapping, hostname)
