import unittest
import ep.echo
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
        self.assertEqual(self.epe.seqnr, self.eee.seqnr)
        self.assertTrue(self.eee.echo_time > self.eee.ping_time)

if __name__ == '__main__':
    unittest.main()

