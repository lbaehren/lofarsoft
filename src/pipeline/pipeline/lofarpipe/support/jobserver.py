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
import SocketServer
import cPickle as pickle

from lofarpipe.support.pipelinelogging import log_process_output
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.utilities import spawn_process

class JobStreamHandler(SocketServer.StreamRequestHandler):
    """
    Networked job dispatcher/receiver.

    This will listen for:

    GET <jobid>           -- reply with a list of all job arguments
    PUT <jobid> <results> -- receive and unpickle job results
    """
    def handle(self):
        while True:
            chunk = self.request.recv(4)
            try:
                slen = struct.unpack(">L", chunk)[0]
            except:
                break
            chunk = self.connection.recv(slen)
            while len(chunk) < slen:
                chunk = chunk + self.connection.recv(slen - len(chunk))
            input = chunk.split(" ", 2)
            if input[0] == "GET":
                self.send_arguments(int(input[1]))
            elif input[0] == "PUT":
                self.read_results(input[1], input[2])
            else:
                self.server.error.set()
                self.server.logger.error("Protocol error; received %s" % chunk)
                self.server.logger.error("Aborting.")

    def send_arguments(self, job_id):
        args = [self.server.loghost, self.server.logport] + self.server.jobpool[int(job_id)].arguments
        pickled_args = pickle.dumps(args)
        length = struct.pack(">L", len(pickled_args))
        self.request.send(length + pickled_args)

    def read_results(self, job_id, pickled_results):
        job_id = int(job_id)
        results = pickle.loads(pickled_results)
        self.server.jobpool[job_id].results = results

class JobSocketReceiver(SocketServer.ThreadingTCPServer):
    """
    Simple TCP socket-based job dispatch and results collection.
    """
    def __init__(self, logger, jobpool, error, loghost, logport, port=0, handler=JobStreamHandler):
        SocketServer.ThreadingTCPServer.__init__(self, (socket.gethostname(), port), handler)
        self.abort = False
        self.timeout = 1
        self.logger = logger
        self.jobpool = jobpool
        self.error = error
        self.loghost = loghost
        self.logport = logport

    def start(self):
        def loop_in_thread():
            while True:
                rd, wr, ex = select.select(
                    [self.socket.fileno()], [], [], self.timeout
                )
                if rd:
                    self.handle_request()
                elif self.abort:
                    break

        self.runthread = threading.Thread(target=loop_in_thread)
        self.runthread.start()

    def stop(self):
        self.abort = True
        self.runthread.join()
        self.server_close()

@contextmanager
def job_dispatcher(logger, jobpool, error):
    """
    Provides a context in which job dispatch is available.

    Yields a host name & port which clients can connect to for job details.
    """
    with clusterlogger(logger) as (loghost, logport):
        logger.debug("Logging to %s:%d" % (loghost, logport))
        jobserver = JobSocketReceiver(logger, jobpool, error, loghost, logport)
        jobserver.start()
        try:
            yield jobserver.server_address
        except KeyboardInterrupt:
            jobserver.stop()
            raise
        jobserver.stop()
