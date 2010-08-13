#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                         Network logging system
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from contextlib import contextmanager

import cPickle
import logging.handlers
import SocketServer
import struct
import threading
import select
import socket
import signal # KeyboardInterrupt guaranteed to main thread if signal is "available". Hm.

@contextmanager
def clusterlogger(
    logger,
    host=None,
    port=0
):
    """
    Provides a context in which a network logging sever is available.

    Yields a host name & port to which log messages can be sent.
    """
    if not host:
        host = socket.gethostname()
    logserver = LogRecordSocketReceiver(logger, host=host, port=port)
    logserver.start()
    try:
        yield logserver.server_address
    except KeyboardInterrupt:
        logserver.stop()
        raise
    logserver.stop()

class LogRecordStreamHandler(SocketServer.StreamRequestHandler):
    """
    Handler for a streaming logging request.

    This basically logs the record using whatever logging policy is
    configured locally.
    """

    def handle(self):
        """
        Handle multiple requests - each expected to be a 4-byte length,
        followed by the LogRecord in pickle format. Logs the record
        according to whatever policy is configured locally.
        """
        while True:
            chunk = self.request.recv(4)
            try:
                slen = struct.unpack(">L", chunk)[0]
            except:
                break
            chunk = self.connection.recv(slen)
            while len(chunk) < slen:
                chunk = chunk + self.connection.recv(slen - len(chunk))
            obj = self.unPickle(chunk)
            record = logging.makeLogRecord(obj)
            self.handleLogRecord(record)

    def unPickle(self, data):
        return cPickle.loads(data)

    def handleLogRecord(self, record):
        # Manually check the level against the pipeline's root logger.
        # Not sure this should be necessary, but it seems to work...
        if self.server.logger.isEnabledFor(record.levelno):
            self.server.logger.handle(record)

class LogRecordSocketReceiver(SocketServer.ThreadingTCPServer):
    """
    simple TCP socket-based logging receiver suitable for testing.
    """
    def __init__(self,
        logger,
        host=None,
        port=logging.handlers.DEFAULT_TCP_LOGGING_PORT,
        handler=LogRecordStreamHandler
    ):
        # If not specified, asssume local hostname
        if not host:
            host = socket.gethostname()
        SocketServer.ThreadingTCPServer.__init__(self, (host, port), handler)
        self.abort = False
        self.timeout = 1
        self.logger = logger

    def start(self):
        def loop_in_thread():
            while not self.abort:
                rd, wr, ex = select.select(
                    [self.socket.fileno()], [], [], self.timeout
                )
                if rd:
                    self.handle_request()
        self.runthread = threading.Thread(target=loop_in_thread)
        self.runthread.start()

    def stop(self):
        self.abort = True
        self.runthread.join()
        self.server_close()
