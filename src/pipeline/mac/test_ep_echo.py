# Simple test program for ep_echo Python module
import ep_echo
import time

if __name__ == "__main__":
    ep_interface = ep_echo.EP_Interface()
    ping_event = ep_echo.EchoPingEvent()
    ping_event.ping_time = time.time()
    ep_interface.send_event(ping_event)
    generic_event = ep_interface.receive_event()
    if generic_event.signal == ep_echo.ECHO:
        echo_event = ep_echo.EchoEchoEvent(generic_event)
        print "Ping time: %f s" % (echo_event.echo_time - echo_event.ping_time)
    else:
        print "Did not receive an echo in reply!"
