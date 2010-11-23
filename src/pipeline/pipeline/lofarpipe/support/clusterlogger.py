#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                         Network logging system
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from contextlib import contextmanager

import socket
import threading

from lofarpipe.support.jobserver import JobSocketReceiver

@contextmanager
def clusterlogger(
    logger,
    host=None,
    port=0
):
    """
    Provides a context in which a network logging sever is available. Note
    that the logging server is just a JobSocketReceiver with no jobpool or
    error flag.

    Yields a host name & port to which log messages can be sent.
    """
    if not host:
        host = socket.gethostname()
    logserver = JobSocketReceiver(logger, {}, threading.Event(), host=host, port=port)
    logserver.start()
    try:
        yield logserver.server_address
    except KeyboardInterrupt:
        logserver.stop()
        raise
    logserver.stop()
    [handler.flush() for handler in logger.handlers]
