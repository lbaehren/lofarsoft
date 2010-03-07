# First import relevant protocol & event names
from _ep_echo import PROTOCOL
from _ep_echo import ECHO, PING, CLOCK

# Events we can handle
from _ep_echo import EchoPingEvent, EchoEchoEvent

# Customized EventPort interface
from ep.interface import EventPort_Interface, EventNotFoundException


class EchoPort_Interface(EventPort_Interface):
    def __init__(self):
        event_mapping = {
            ECHO: EchoEchoEvent
        }
        super(EchoPort_Interface, self).__init__("EchoServer:test", PROTOCOL, event_mapping)
