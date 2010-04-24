import platform
import logging, logging.handlers
import os

def run_node(*args):
    import imp
    control_script = getattr(imp.load_module(recipename, *imp.find_module(recipename, [nodepath])), recipename)
    return control_script(loghost=loghost, logport=logport).run_with_logging(*args)

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
        self.loghost = loghost
        self.logport = logport

    def run_with_logging(self):
        # Call the run() method, ensuring that the logging handler is added
        # and removed properly.
        if self.loghost:
            my_tcp_handler = logging.handlers.SocketHandler(self.loghost, self.logport)
            self.logger.addHandler(my_tcp_handler)
        self.run()
        if self.loghost:
            my_tcp_handler.close()
            self.logger.removeHandler(my_tcp_handler)

    def run(self):
        # Override in subclass.
        raise NotImplementedError
