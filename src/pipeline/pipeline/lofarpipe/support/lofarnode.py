import platform
import logging, logging.handlers
import os

def run_node(*args):
    import imp
    control_script = getattr(imp.load_module(recipename, *imp.find_module(recipename, [nodepath])), recipename)
    return control_script(loghost=loghost, logport=logport).run(*args)

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
            # Remove old, stale handlers and install a new one.
            for handler in self.logger.handlers:
                self.logger.removeHandler(handler)
            self.logger.addHandler(
                logging.handlers.SocketHandler(loghost, logport)
            )

    def run(self):
        # Override in subclass.
        raise NotImplementedError
