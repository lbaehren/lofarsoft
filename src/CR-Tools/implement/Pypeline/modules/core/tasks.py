"""Tasks
"""

class Parameter():
    """A parameter base class.
    """
    def __init__(self, value=None, description=None):
        """Constructor.
        """
        self.value = value
        self.description = description

    def __repr__(self):
        return repr(self.value)

class Task():
    """A base class for creating tasks.
    """

    def __init__(self):
        """Constructor.
        """
        pass

    def __call__(self, **kwargs):
        """Runs the task (do not overload this method).
        """
        self.run()

    def run(self):
        """Method for running the task.

        This should be overloaded when subclassed to create a Task.
        """

