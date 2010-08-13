#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                                     Exceptions
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

class ExecutableMissing(Exception):
    pass

class PipelineException(Exception):
    pass

class PipelineRecipeFailed(PipelineException):
    pass

class PipelineReceipeNotFound(PipelineException):
    pass

class PipelineQuit(PipelineException):
    """
    If this exception is raised during a pipeline run, we skip over all
    subsequent steps and exit cleanly.
    """
    pass

class ClusterError(PipelineException):
    pass
