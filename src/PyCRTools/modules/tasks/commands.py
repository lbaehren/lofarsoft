"""
Comands to work with tasks

An example to use tasks is give in tasks/averagespectrum.py

to try it start ipython and type

Task                    # the currently loaded task instance
Task.par (=value)       # access or set a parameter (without updating)
Task(par1,par2,...)     # run the task with parameters par1,par2,....
trun "taskname", pars   # run the task with name "taskname" and assign it to the global variable `Task``
trerun "taskname", pars # like trun, but store task and do not reload it during a 2nd call, but rerun.
tlist                   # to view the available tasks
tlog                    # list the log of recently run tasks, including execution times.
tload 2                 # to load the task #2 (can also provide a name)
tload "taskname"        # i.e., this is safer in code since the task number can change with time
tpars                   # to list all parameters
tpar nchunks=2          # to set a parameter
go                      # to run the task
tpar parfile="averagespectrum_2011-02-15_23:52:15.par"     # to read back a parameter file
treset                  # to reset parameters to default values
tget (name)             # to read back the parameters from the latest run - will also be done at tload - or get the one stored under 'name'
tput (name)             # store input parameters in database (under 'name')
tinit                   # run the initialization routine again (without resetting the parameters to default values)
thelp                   # print documentation of task module
"""

import shelve
from pycrtools import tasks

def trun(name, *args, **kwargs):
    """
    Usage:

    ``trun(taskname,parameter,keyword1=....)``

    Run the taks with the given tasknname and with the parameters provided as argument list.

    The task itself will be available via the global variable ``Task`` afterwards.
    """
    if not name in tasks.task_allloaded.keys():
		raise ValueError("Task name {0} unknown. See 'tlist' for full list.".format(name))
    tasks.task_class = eval(tasks.task_allloaded[name] + "." + name)
    tasks.task_instance = tasks.task_class()
    tasks.set_globals("Task", tasks.task_instance)
    return tasks.task_instance(*args, **kwargs)


def trerun(name, version, *args, **kwargs):
    """
    Usage:

    ``trerun(taskname,version,parameters,keyword1=....)``

    Run the taks with the given task name and with the parameters provided as argument list.

    *name*       - task name to load (see :func:`tlist`)
    *version*    - assign the task a unique version ID, so that you can store
                 and rerun different instances of the same task

    The task itself will be available via the global variable ``Task`` afterwards.

    All stored tasks can be found in the dict tasks.task_instances.
    """
    if not name in tasks.task_allloaded.keys():
		raise ValueError("Task name {0} unknown. See 'tlist' for full list.".format(name))
    taskinstancename = name + str(version)
    if taskinstancename in tasks.task_instances:  # reload existing instance
        tasks.task_class = None
        tasks.task_instance = tasks.task_instances[taskinstancename]
    else:  # create a new task instance
        tasks.task_class = eval(tasks.task_allloaded[name] + "." + name)
        tasks.task_instance = tasks.task_class()
        tasks.task_instances[taskinstancename] = tasks.task_instance
    tasks.task_instance._version_name = version
    tasks.set_globals("Task", tasks.task_instance)
    return tasks.task_instance(*args, **kwargs)


def tnorerun(name, version, *args, **kwargs):
    """
    Usage:

    ``tnorerun(taskname,version,parameters,keyword1=....)``

    Like trerun, just don't reload the task - for testing purposes only ...

    Run the taks with the given task name and with the parameters provided as argument list.

    *name*       - task name to load (see :func:`tlist`)
    *version*    - assign the task a unique version ID, so that you can store
                 and rerun different instances of the same task

    The task itself will be available via the global variable ``Task`` afterwards.

    All stored tasks can be found in the dict tasks.task_instances.
    """
    if not name in tasks.task_allloaded.keys():
        print "ERROR: trerun - Task name", name, "unknown. See 'tlist' for full list."
        return
    taskinstancename = name + str(version)
    tasks.task_class = eval(tasks.task_allloaded[name] + "." + name)
    tasks.task_instance = tasks.task_class()
    tasks.task_instances[taskinstancename] = tasks.task_instance
    tasks.task_instance._version_name = version
    tasks.set_globals("Task", tasks.task_instance)
    return tasks.task_instance(*args, **kwargs)


def tload(name, get=True, quiet=False, **args):
    """
    Loads a specific task as the current task, you you can start it
    with 'go' and set parameters with 'par x=value'.

    One can also provide a number, which refers to the number
    associated with the task when typing 'tlist'.

    The loaded task will be saved and reloaded the next time the
    program is started.
    """
    if type(name) == int:
        if name >= len(tasks.task_allloaded.keys()) or (name < 0):
            print "ERROR: tload - Index number", name, "out of range (should be <", len(tasks.task_allloaded.keys()), "). See 'tlist'"
            return
        name = tasks.task_allloaded.keys()[name]
    elif type(name) == str:
        if not name in tasks.task_allloaded.keys():
            print "ERROR: tload - Task name", name, "unknown. See 'tlist' for full list."
            return
    else:
        print "ERROR: tload - Task name", name, "unknown - wrong data type (should be string or int). See 'tlist' for full list."
        return
    tasks.task_class = eval(tasks.task_allloaded[name] + "." + name)
    tasks.task_instance = tasks.task_class(**args)
    tasks.set_globals("Task", tasks.task_instance)
    taskdb = shelve.open(tasks.dbfile)
    taskdb["last_taskname"] = name
    taskdb.close()
    if get:
        tget(quiet=quiet)
    return tasks.task_instance


def task(*args, **kwargs):
    """
    **Usage:**

    ``task() -> return current loaded task``

    or

    ``task("taskname",par,par1=X,par2=Y,...) - > return task named taskname``

    **Description:**

    With no parameters return the currently loaded task. Otherwise
    return the task with the name given as first argument and the
    other parameters as arguments.

    **See also:**

    tload
    """
    if not args and not kwargs:
        return tasks.task_instance
    else:
        name = args[0]
        return eval(tasks.task_allloaded[name] + "." + name)(*args[1:], **kwargs)


class t_class():
    """
    Dummy base class which redefined the __repr__ object such that the
    user can call a function by simply typing the name of an instance
    without brackets.
    """
    def __repr__(self):
#       if tasks.task_instance == None: return ""
        self()
        return ""


class go_class():
    """
    Simple command to let the user start the current task by simply typing 'go'.

    *task* - You can also provide a task as argument which will then be
    run, by stating saying 'go task=tasks.task_instance'.

    Positional and keyword parameters can be provided as parameters, e.g,

    go data,par1=123

    Before starting the task, go will store the current parameters with "tput".
    """
    def __repr__(self):
#       if tasks.task_instance == None: return ""
        print "Starting task", tasks.task_name
        tput()
        tasks.task_instance()
        print "Task", tasks.task_name, "run."
        return ""

    def __call__(self, *args, **kwargs):
        tput()
        if 'task' in kwargs and not kwargs["task"] == None:
            ttask = kwargs["task"]
            del kwargs["task"]
        else:
            ttask = tasks.task_instance
        ttask(*args, **kwargs)
#           if len(args)>0:
#       else:
#           ttask()
        print "Task", ttask.__taskname__, "run."

go = go_class()


class tpars_class(t_class):
    """
    Usage:

    tpars -> list parameter workspace

    Class to let the user view the current parameter set by simply
    typing 'tpars'. Use 'tpars 0' or 'tpars False' to just see input
    parameters (this only works in ipython, python requires brackets).

    *noninputparameters*=True - If False, don't list the derived paramters.

    *workarrays*=False - If True also list the work arrays.
    """
    def __call__(self, noninputparameters=True, workarrays=False):
        print "Parameters of task", tasks.task_name
        print tasks.task_instance.ws.__repr__(workarrays=workarrays, noninputparameters=noninputparameters)

tpars = tpars_class()


class tinit_class(t_class):
    """
    Class to let the user run the initialization part of a task. This
    will result in a forced initialization, irrespective of whether
    this has been run before, unless force=False.
    """
    def __call__(self, force=True):
        tasks.task_instance.callinit(forceinit=force)

tinit = tinit_class()


class thelp_class(t_class):
    """
    **Usage:**

    thelp what

    **Description:**

    Print documentation of tasks

    **Parameters:**

    *what* - 0 = help on current task, 1 = help on tasks
             'taskname' - help on named task

    """
    def __call__(self, what=0):
        import pycrtools.tasks
        if what == 0:
            print  tasks.task_instance.__doc__
            print """\nTo get help on tasks in general type: thelp 1"""
            print """\nTo get help on a specific task type: thelp 'taskname'"""
        elif what == 1:
            print pycrtools.tasks.__doc__
            print """\nTo get help on current task type: thelp"""
            print """\nTo get help on a specific task type: thelp 'taskname'"""
        elif isinstance(what, str):
            if not what in tasks.task_allloaded.keys():
                print "ERROR: thelp - Task name", name, "unknown. See 'tlist' for full list."
                return
            taskclass = eval(tasks.task_allloaded[what] + "." + what)

            print  taskclass.__doc__
            print """\nTo get help on current task type: thelp"""
            print """\nTo get help on tasks in general type: thelp 1"""
        else:
            print """ERROR: thelp - wrong input."""
            print """\nTo get help on current task type: thelp"""
            print """\nTo get help on tasks in general type: thelp 1"""
            print """\nTo get help on a specific task type: thelp 'taskname'"""

thelp = thelp_class()


class tlist_class(t_class):
    """
    Class to let the user list the available tasks that can be loaded with tload.

    """
    def __call__(self):
        print "Available Tasks:"
        print "------------------------------------------------------------"
        for tpl in zip(range(len(tasks.task_allloaded)), tasks.task_allloaded.keys(), tasks.task_allloaded.values()):
            from sys import version_info
            if version_info > (2, 5, 9):
                print "{0:2}: {1:30s} ({2})".format(*tpl)
            else:
                print tpl

tlist = tlist_class()


class tlog_class(t_class):
    """
    Class to let the user list the log of recently run tasks, including execution times.

    **Parameter:**

    *n* = 20 - Display the last 20 entries. n=-1 means display all
    *doprint* = True - print the result otherwise return the string to be printed
    *loglist* = None - If provided use this list of log entries

    x=[{'execution_time': 0.22922900000000002,
  'name': 'AverageSpectrum',
  'start_time': '2011-08-04 14:24:19',
  'start_time_cpu': 0.71958299999999997,
  'version': 'cr_event'},
 {'execution_time': 4.6105419999999997,
  'name': 'FitBaseline',
  'start_time': '2011-08-04 14:24:19',
  'start_time_cpu': 0.97520499999999999,
  'version': ''}]

    """
    def __call__(self, n=20, doprint=True, loglist=None, colsep=" | "):
        s = ""
        s = "Log of recently run Tasks:\n\n"
        if not loglist:
            loglist = tasks.task_logger
        l = len(loglist)
        if l == 0:
            return
        if n > 0 and n < l:
            loglist = loglist[-n:]

        keys = ('name', 'version', 'start_time', 'start_time_cpu', 'execution_time', 'cummulative_time')
        size = (25, 10, 19, 14, 14, 16)
        format_keys = ("s", "s", "s", ".3f", ".3f", ".3f")
        nkeys = len(keys)

        s += " | ".join([("{0:" + str(size[i]) + "s}").format(keys[i]) for i in range(nkeys)]) + "\n"
        s += "".join(["-" for i in range(Vector(size).sum() + len(colsep) * nkeys)]) + "\n"
        format_string = colsep.join([("{" + str(i) + ":" + str(size[i]) + format_keys[i] + "}") for i in range(nkeys)])
        summed_time = 0
        for d in loglist:
            summed_time += d["execution_time"]
            d["cummulative_time"] = summed_time
            values = [d[keys[i]] for i in range(nkeys)]
            s += format_string.format(*values) + "\n"
        if doprint:
            print s
        else:
            return s

tlog = tlog_class()


class tget_class(t_class):
    """
    Usage:

    tget -> Class to let the user retrieve the input parameters from the
    system data base on disk. See also 'tput'

    tget name -> Retrieve the parameters stored under the respective name

    tget 0 (or any unknown name) -> show a list of all available parameter sets
    """
    def __call__(self, name="", quiet=False):
        if not isinstance(tasks.task_instance, tasks.Task):
            print "ERROR: No valid task loaded yet - use tload first."
            return
        found = tasks.task_instance.get(name)
        if found:
            tasks.task_instance.ws.update()
            if not quiet:
                tpars(False, False)

tget = tget_class()


class treset_class(t_class):
    """
    Usage:

    treset -> Reset all parameters to orginal values.

    treset(par1=val1,par2=val2,...) -> reset all parameters to
    default and initialze with parameters provided.

    treset(restorecallparameters=True,par1=val1,par2=val2,...)
    -> reset all parameters to their state at initialization, but
    keep the parameters provided during the last initialisation.

    """
    def __call__(self, restorecallparameters=False, **args):
        tasks.task_instance.reset(restorecallparameters=False, **args)
        tasks.task_instance.ws.evalInputParameters()
        tpars(False, False)

treset = treset_class()


class tput_class(t_class):
    """
    Usage:

    tput -> Class to let the user store the input parameters to the
    system data base on disk. See also 'tget'

    tput name -> Store parameters stored under 'name' (retrieve with tget name)

    *delete* - If True, delete the parameter set from data base.

    """
    def __call__(self, name="", delete=False):
        tasks.task_instance.put(name=name, delete=delete)

tput = tput_class()


class tpar_class():
    """Sets the parameters in the workspace of a task.

    Example:

    In ipython you can say something like

    par x=2

    to set parameter x of the current task.

    If no parameters are provided, just print the input parameters.
    """
    def __call__(self, **args):
        tasks.task_instance.ws(**args)
        tasks.task_instance.ws.update()

    def __repr__(self):
#       if tasks.task_instance == None: return ""
        print "Input parameters of task", tasks.task_name
        print tasks.task_instance.ws.__repr__(workarrays=False, noninputparameters=False)
        return ""

tpar = tpar_class()


def tdel(*args):
    """Deletes the parameters in the workspace of a task.

    Example:

    In ipython you can say something like

    par x=2

    to set parameter x of the current task. If that was initially an ouput parameter,  it now becomes
    and input parameter. This can be reverted by typing

    tdel "x"
    """
    for p in args:
        exec("del tasks.task_instance.ws." + p)
    tasks.task_instance.ws.update()


