#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                            Compute node system
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

import os
import socket
import struct
import platform
import logging
import logging.handlers
import cPickle as pickle

def run_node(*args):
    """
    Run on node to automatically locate, instantiate and execute the
    correct LOFARnode class.
    """
    import imp
    control_script = getattr(
        imp.load_module(recipename, *imp.find_module(recipename, [nodepath])),
        recipename
    )
    return control_script(loghost=loghost, logport=logport).run_with_logging(*args)

class LOFARnode(object):
    """
    Base class for node jobs called through IPython or directly via SSH.

    Sets up TCP based logging.
    """
    def __init__(
        self,
        loghost=None,
        logport=logging.handlers.DEFAULT_TCP_LOGGING_PORT
    ):
        self.logger = logging.getLogger(
            'node.%s.%s' % (platform.node(), self.__class__.__name__)
        )
        self.logger.setLevel(logging.DEBUG)
        self.loghost = loghost
        self.logport = int(logport)
        self.outputs = {}

    def run_with_logging(self, *args):
        """
        Calls the run() method, ensuring that the logging handler is added
        and removed properly.
        """
        if self.loghost:
            my_tcp_handler = logging.handlers.SocketHandler(self.loghost, self.logport)
            self.logger.addHandler(my_tcp_handler)
        try:
            return self.run(*args)
        finally:
            if self.loghost:
                my_tcp_handler.close()
                self.logger.removeHandler(my_tcp_handler)

    def run(self):
        # Override in subclass.
        raise NotImplementedError

class LOFARnodeTCP(LOFARnode):
    """
    This node script extends :class:`~lofarpipe.support.lofarnode.LOFARnode`
    to receive instructions via TCP from a
    :class:`~lofarpipe.support.jobserver.JobSocketReceiver`.
    """
    def __init__(self, job_id, host, port):
        self.job_id, self.host, self.port = int(job_id), host, int(port)
        self.__fetch_arguments()
        super(LOFARnodeTCP, self).__init__(self.host, self.port)

    def run_with_stored_arguments(self):
        """
        After fetching arguments remotely, use them to run the standard
        run_with_logging() method.
        """
        returnvalue = self.run_with_logging(*self.arguments)
        self.__send_results()
        return returnvalue

    def __fetch_arguments(self):
        """
        Connect to a remote job dispatch server (an instance of
        jobserver.JobSocketReceive) and obtain all the details necessary to
        run this job.
        """
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.connect((self.host, self.port))
        except Exception, e:
            print "Could not connect to %s:%s (got %s)" % (self.host, str(self.port), str(e))
            raise
        message = "GET %d" % self.job_id
        s.send(struct.pack(">L", len(message)) + message)
        chunk = s.recv(4)
        slen = struct.unpack(">L", chunk)[0]
        chunk = s.recv(slen)
        while len(chunk) < slen:
            chunk += s.recv(slen - len(chunk))
        self.arguments = pickle.loads(chunk)

    def __send_results(self):
        """
        Send the contents of self.outputs to the originating job dispatch
        server.
        """
        message = "PUT %d %s" % (self.job_id, pickle.dumps(self.outputs))
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.connect((self.host, int(self.port)))
        except Exception, e:
            print "Could not connect to %s:%s (got %s)" % (self.host, str(self.port), str(e))
            raise
        s.send(struct.pack(">L", len(message)) + message)
