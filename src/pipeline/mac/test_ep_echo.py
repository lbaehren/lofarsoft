# Simple test program for ep_echo Python module
import ep_echo

event_mapping = {
    ep_echo.ECHO: ep_echo.EchoEchoEvent
}

if __name__ == "__main__":
    ep_interface = ep_echo.EP_Interface()
    ping_event = ep_echo.EchoPingEvent()
    ep_interface.send_event(ping_event)
    generic_event = ep_interface.receive_event()

    my_event = event_mapping[generic_event.signal](generic_event)

    # In this test, we only know about echo events
    if my_event.signal == ep_echo.ECHO:
        print "Ping time: %f s" % (my_event.echo_time - my_event.ping_time)
    else:
        print "Did not receive an echo in reply!"
