import cPickle, logging.handlers, SocketServer, struct, threading, select
from ConfigParser import SafeConfigParser as ConfigParser

from pipeline.support.clusterdesc import ClusterDesc
from pipeline import __path__ as config_path
config = ConfigParser()
config.read("%s/pipeline.cfg" % (config_path[0],))
clusterdesc = ClusterDesc(config.get('cluster', 'clusterdesc'))
head_node = clusterdesc.get('HeadNode')[0]

class LogRecordStreamHandler(SocketServer.StreamRequestHandler):
    """Handler for a streaming logging request.

    This basically logs the record using whatever logging policy is
    configured locally.
    """

    def handle(self):
        """
        Handle multiple requests - each expected to be a 4-byte length,
        followed by the LogRecord in pickle format. Logs the record
        according to whatever policy is configured locally.
        """
        while 1:
            chunk = self.connection.recv(4)
            if len(chunk) < 4:
                break
            slen = struct.unpack(">L", chunk)[0]
            chunk = self.connection.recv(slen)
            while len(chunk) < slen:
                chunk = chunk + self.connection.recv(slen - len(chunk))
            obj = self.unPickle(chunk)
            record = logging.makeLogRecord(obj)
            self.handleLogRecord(record)

    def unPickle(self, data):
        return cPickle.loads(data)

    def handleLogRecord(self, record):
        self.server.logger.handle(record)

class LogRecordSocketReceiver(SocketServer.ThreadingTCPServer):
    """simple TCP socket-based logging receiver suitable for testing.
    """

    allow_reuse_address = 1

    def __init__(self, logger, host=head_node,
                 port=logging.handlers.DEFAULT_TCP_LOGGING_PORT,
                 handler=LogRecordStreamHandler):
        SocketServer.ThreadingTCPServer.__init__(self, (host, port), handler)
        self.abort = 0
        self.timeout = 1
        self.logger = logger

    def serve_until_stopped(self):
        def loop_in_thread():
            while not self.abort:
                rd, wr, ex = select.select([self.socket.fileno()],
                                           [], [],
                                           self.timeout)
                if rd:
                    self.handle_request()
        self.runthread = threading.Thread(target=loop_in_thread)
        self.runthread.start()

    def stop(self):
        self.abort = 1
        self.runthread.join()
