import unittest
import ep.echo
import ep.control
import time

class TestPingEvent(unittest.TestCase):
    def setUp(self):
        self.epe = ep.echo.EchoPingEvent()

    def test_init_time(self):
        self.assertTrue(self.epe.ping_time < time.time())

    def test_change_time(self):
        now = time.time()
        self.epe.ping_time = now
        self.assertAlmostEqual(self.epe.ping_time, now, 5)

    def test_changq_seqnr(self):
        self.epe.seqnr = 10
        self.assertEqual(self.epe.seqnr, 10)

class TestReceiveEcho(unittest.TestCase):
    def setUp(self):
        interface = ep.echo.EchoPort_Interface()
        self.epe = ep.echo.EchoPingEvent()
        interface.send_event(self.epe)
        self.eee = interface.receive_event()

    def test_ping_time(self):
        self.assertEqual(self.epe.ping_time, self.eee.ping_time)

    def test_seqnr(self):
        self.assertEqual(self.epe.seqnr, self.eee.seqnr)

    def test_long_ping(self):
        self.assertTrue(self.eee.echo_time > self.eee.ping_time)

class TestControllerSendables(unittest.TestCase):
    def test_control_started(self):
        event = ep.control.ControlStartedEvent("controller name", True)
        self.assertEqual(event.controller_name, "controller name")
        self.assertTrue(event.successful)
        self.assertEqual(event.signal, ep.control.CONTROL_STARTED)

    def test_control_connected(self):
        event = ep.control.ControlConnectedEvent("controller name", ep.control.OK)
        self.assertEqual(event.controller_name, "controller name")
        self.assertEqual(event.result, ep.control.OK)
        self.assertEqual(event.signal, ep.control.CONTROL_CONNECTED)

    def test_control_resynced(self):
        event = ep.control.ControlResyncedEvent("controller name", ep.control.OK)
        self.assertEqual(event.controller_name, "controller name")
        self.assertEqual(event.result, ep.control.OK)
        self.assertEqual(event.signal, ep.control.CONTROL_RESYNCED)

    def test_control_claimed(self):
        event = ep.control.ControlClaimedEvent("controller name", ep.control.OK)
        self.assertEqual(event.controller_name, "controller name")
        self.assertEqual(event.result, ep.control.OK)
        self.assertEqual(event.signal, ep.control.CONTROL_CLAIMED)

    def test_control_prepared(self):
        event = ep.control.ControlPreparedEvent("controller name", ep.control.OK)
        self.assertEqual(event.controller_name, "controller name")
        self.assertEqual(event.result, ep.control.OK)
        self.assertEqual(event.signal, ep.control.CONTROL_PREPARED)

    def test_control_resumed(self):
        event = ep.control.ControlResumedEvent("controller name", ep.control.OK)
        self.assertEqual(event.controller_name, "controller name")
        self.assertEqual(event.result, ep.control.OK)
        self.assertEqual(event.signal, ep.control.CONTROL_RESUMED)

    def test_control_suspended(self):
        event = ep.control.ControlSuspendedEvent("controller name", ep.control.OK)
        self.assertEqual(event.controller_name, "controller name")
        self.assertEqual(event.result, ep.control.OK)
        self.assertEqual(event.signal, ep.control.CONTROL_SUSPENDED)

    def test_control_released(self):
        event = ep.control.ControlReleasedEvent("controller name", ep.control.OK)
        self.assertEqual(event.controller_name, "controller name")
        self.assertEqual(event.result, ep.control.OK)
        self.assertEqual(event.signal, ep.control.CONTROL_RELEASED)

    def test_control_quited(self):
        event = ep.control.ControlQuitedEvent("controller name", 1, ep.control.OK, "no error")
        self.assertEqual(event.controller_name, "controller name")
        self.assertEqual(event.result, ep.control.OK)
        self.assertEqual(event.treeID, 1)
        self.assertEqual(event.error_message, "no error")
        self.assertEqual(event.signal, ep.control.CONTROL_QUITED)

if __name__ == '__main__':
    unittest.main()

