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
import Queue
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
    [handler.flush() for handler in logger.handlers]

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
        self.server.queue.put_nowait(record)

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
        self.queue = Queue.Queue()

    def start(self):
        # Messages are received in one thread, and appended to an instance of
        # Queue.Queue. Another thread picks messages off the queue and sends
        # them to the log handlers. We keep the latter thread running until
        # the queue is empty, thereby avoiding the problem of falling out of
        # the clusterlogger threads before all log messages have been handled.
        def loop_in_thread():
            while True:
                rd, wr, ex = select.select(
                    [self.socket.fileno()], [], [], self.timeout
                )
                if rd:
                    self.handle_request()
                elif self.abort:
                    break
        def log_from_queue():
            while True:
                try:
                    record = self.queue.get(True, 1)
                    # Manually check the level against the pipeline's root logger.
                    # Not sure this should be necessary, but it seems to work...
                    if self.logger.isEnabledFor(record.levelno):
                        self.logger.handle(record)
                except Queue.Empty:
                    if self.abort:
                        break

        self.runthread = threading.Thread(target=loop_in_thread)
        self.logthread = threading.Thread(target=log_from_queue)
        self.runthread.start()
        self.logthread.start()

    def stop(self):
        self.abort = True
        self.runthread.join()
        self.logthread.join()
        self.server_close()
