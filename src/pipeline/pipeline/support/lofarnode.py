import platform
import logging, logging.handlers

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

        # Log messages are filtered on the server side: we have plenty of
        # bandwidth, and it's conceptually simpler.
        self.logger.setLevel(logging.DEBUG)
        if loghost:
            socketHandler = logging.handlers.SocketHandler(loghost, logport)
            self.logger.addHandler(socketHandler)

    def run(self):
        # Override in subclass.
        raise NotImplementedError
