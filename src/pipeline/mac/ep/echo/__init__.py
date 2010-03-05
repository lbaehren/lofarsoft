from _ep_echo import ECHO, PROTOCOL, EchoPingEvent, EchoEchoEvent
from ep.interface import EventPort_Interface, EventNotFoundException


class EchoPort_Interface(EventPort_Interface):
    def __init__(self):
        event_mapping = {
            ECHO: EchoEchoEvent
        }
        super(EchoPort_Interface, self).__init__("EchoServer:test", PROTOCOL, event_mapping)
