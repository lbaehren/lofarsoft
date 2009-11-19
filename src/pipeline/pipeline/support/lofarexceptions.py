class ExecutableMissing(Exception):
    pass

class PipelineException(Exception):
    pass

class PipelineRecipeFailed(PipelineException):
    pass

class PipelineReceipeNotFound(PipelineException):
    pass
