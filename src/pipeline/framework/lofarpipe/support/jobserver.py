#                                                       LOFAR PIPELINE FRAMEWORK
#
#                          Network services for sending/receiving job parameters
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
from contextlib import contextmanager

import signal
import threading
import struct
import socket
import select
import logging
import logging.handlers
import Queue
import SocketServer
import cPickle as pickle

from lofarpipe.support.pipelinelogging import log_process_output
from lofarpipe.support.utilities import spawn_process

class JobStreamHandler(SocketServer.StreamRequestHandler):
    """
    Networked job server.

    This will listen for:

    * GET <jobid>           -- reply with a list of all job arguments
    * PUT <jobid> <results> -- receive and unpickle job results
    * Pickled log records

    Log records are logs using whatever local logger is supploed to the
    SocketReceiver.
    """
    def handle(self):
        """
        Each request is expected to be a 4-bute length followed by either a
        GET/PUT request or a pickled LogRecord.
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
            input_msg = chunk.split(" ", 2)
            try:
                # Can we handle this message type?
                if input_msg[0] == "GET":
                    self.send_arguments(int(input_msg[1]))
                elif input_msg[0] == "PUT":
                    self.read_results(input_msg[1], input_msg[2])
                else:
                    self.handle_log_record(chunk)
            except:
                # Otherwise, fail.
                self.server.error.set()
                self.server.logger.error("Protocol error; received %s" % chunk)
                self.server.logger.error("Aborting.")

    def send_arguments(self, job_id):
        job_id = int(job_id)
        self.server.logger.debug(
            "Request for job %d from %s" %
            (job_id, self.request.getpeername())
        )
        args = self.server.jobpool[job_id].arguments
        pickled_args = pickle.dumps(args)
        length = struct.pack(">L", len(pickled_args))
        self.request.send(length + pickled_args)

    def read_results(self, job_id, pickled_results):
        job_id = int(job_id)
        self.server.logger.debug(
            "Results for job %d submitted by %s" %
            (job_id, self.request.getpeername())
        )
        results = pickle.loads(pickled_results)
        self.server.jobpool[job_id].results = results

    def handle_log_record(self, chunk):
        record = logging.makeLogRecord(pickle.loads(chunk))
        self.server.queue.put_nowait(record)

class JobSocketReceiver(SocketServer.ThreadingTCPServer):
    """
    Simple TCP socket-based job dispatch and results collection as well as
    network logging.
    """
    def __init__(
        self,
        logger,
        jobpool,
        error,
        host=None,
        port=logging.handlers.DEFAULT_TCP_LOGGING_PORT
    ):
        if not host:
            host = socket.gethostname()
        SocketServer.ThreadingTCPServer.__init__(self, (host, port), JobStreamHandler)
        self.abort = False
        self.timeout = 1
        self.queue = Queue.Queue()
        self.logger = logger
        self.jobpool = jobpool
        self.error = error

    def start(self):
        # Log messages are received in one thread, and appended to an instance
        # of Queue.Queue. Another thread picks messages off the queue and
        # sends them to the log handlers. We keep the latter thread running
        # until the queue is empty, thereby avoiding the problem of falling
        # out of the logger threads before all log messages have been handled.
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

@contextmanager
def job_server(logger, jobpool, error):
    """
    Provides a context in which job dispatch is available.

    Yields a host name & port which clients can connect to for job details.
    """
    jobserver = JobSocketReceiver(logger, jobpool, error, port=0)
    jobserver.start()
    try:
        yield jobserver.server_address
    except KeyboardInterrupt:
        jobserver.stop()
        raise
    jobserver.stop()
    [handler.flush() for handler in logger.handlers]
