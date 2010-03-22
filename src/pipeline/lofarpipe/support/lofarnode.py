import platform
import logging, logging.handlers
import os

class LOFARnode(object):
    """
    Base class for node jobs called through IPython.
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
        if loghost:
            self.mySocketHandler = logging.handlers.SocketHandler(loghost, logport)
            self.logger.addHandler(self.mySocketHandler)

    def __del__(self):
        # Remove our log handler to avoid duplicate entries
        self.logger.removeHandler(self.mySocketHandler)

    def run(self):
        # Override in subclass.
        raise NotImplementedError
