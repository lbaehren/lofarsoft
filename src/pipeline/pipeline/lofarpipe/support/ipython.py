from IPython.kernel.task import StringTask

class LOFARTask(StringTask):
    """
    Extended version of IPython's StringTask, allowing external
    arguments for dependency resolution.
    """
    def __init__(self, expression, pull=None, push=None,
        clear_before=False, clear_after=False, retries=0,
        recovery_task=None, depend=None, dependargs=None
    ):
        self.dependargs = dependargs
        return super(LOFARTask, self).__init__(
            expression, pull, push, clear_before, clear_after,
            retries, recovery_task, depend
        )

    def check_depend(self, properties):
        """
        Calls self.depend(properties, self.dependargs)
        to see if a task should be run.
        """
        if self.depend is not None:
            return self.depend(properties, self.dependargs)
        else:
            return True

