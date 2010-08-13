#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                            Compute node system
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

import os
import platform
import logging
import logging.handlers

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

    def run_with_logging(self, *args):
        # Call the run() method, ensuring that the logging handler is added
        # and removed properly.
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
