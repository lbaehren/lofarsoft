from _ep_interface import *

class EventPortException(Exception):
    pass

class EventNotFoundException(EventPortException):
    pass

class EventPort_Interface(EP_Interface):

    def __init__(self, servicemask, protocol, mapping, hostname='localhost'):
        super(EventPort_Interface, self).__init__(servicemask, protocol, hostname)
        self.__event_mapping = mapping

    def receive_event(self):
        gev = super(EventPort_Interface, self).receive_event()
        try:
            return self.__event_mapping[gev.signal](gev)
        except KeyError:
            raise EventNotFoundException


