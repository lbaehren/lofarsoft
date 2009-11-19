from IPython.kernel.task import StringTask

class LOFARTask(StringTask):
    def __init__(self, expression, pull=None, push=None, 
        clear_before=False, clear_after=False, retries=0, 
        recovery_task=None, depend=None, dependargs=None
    ):
    super(LOFARTask, self).__init__(
        self, expression, pull=None, push=None, clear_before=False, 
        clear_after=False, retries=0, recovery_task=None, depend=None
    )
    self.dependargs = dependargs

    def check_depend(self, properties):
        """
        Calls self.depend(properties, self.dependargs)
        to see if a task should be run.
        """
        if self.depend is not None:
            return self.depend(properties, self.dependargs)
        else:
            return True

