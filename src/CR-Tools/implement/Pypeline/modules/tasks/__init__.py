"""Tasks
"""

import os
import shelve

__all__ = ['Task', 'TaskLauncher', 'default']

# Configuration (should be moved to config.py)
configdir = os.path.expanduser('~/.pycrtools')

if not os.path.isdir(configdir):
    os.mkdir(configdir)

dbfile = configdir+"/"+"task"

class TaskInit(type):
    """Metaclass for tasks.

    Should never be used direcly.
    All tasks should derive from ..class::`Task` instead.
    """

    def __init__(cls, name, bases, dct):
        """ Create a new class.
        """
        cls.__taskname__ = name
        super(TaskInit, cls).__init__(name, bases, dct)

    def __call__(cls, *args, **kwargs):
        """ Create a new instance.
        """

        # Open task database
        taskdb = shelve.open(dbfile)

        if cls.__taskname__ in taskdb:
            # Restoring from database
            nkwargs = taskdb[cls.__taskname__]

            # Restore positional arguments
            #args = args + nargs[len(args):]

            # Restore named arguments
            for key in kwargs.keys():
                nkwargs[key] = kwargs[key]
            kwargs = nkwargs

        # Create instance with restored arguments
        obj = type.__call__(cls, *args, **kwargs)

        # Saving to database
        taskdb[cls.__taskname__] = kwargs

        taskdb.close()
 
        # Return the new object
        return obj

class Task(object):
    """Base class from which all tasks should be derived.
    """
    __metaclass__ = TaskInit

class TaskLauncher(object):
    """Task launcher.
    """

    def __init__(self, T):
        self.Task = T
        self.__taskname__ = T.__taskname__

    def __call__(self, *args, **kwargs):

        self.Instance = self.Task(*args, **kwargs)

        return Instance.run()

def default(task):
    """Reset task to default state.

    Arguments:

    :param task: a :class:`Task` instance to reset

    """
    # Open task database
    taskdb = shelve.open(dbfile)

    if task.__taskname__ in taskdb:
        del taskdb[task.__taskname__]

    taskdb.close()

