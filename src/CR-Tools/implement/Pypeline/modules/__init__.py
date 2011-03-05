"""A Python library for analyzing LOFAR TBB data.
"""

# Import core functionality
#import core

# Make core functionality available in local namespace
from core import *
import tasks

# Ensure from pycrtools import * works correctly
# for all functions exported from core
#__all__ = [func for func in dir(core) if "__" not in  func]

#import pdb; pdb.set_trace()

#Now store the interactive namespace in tasks, so that we can assign to global variables in the interactive session
if __builtins__.has_key("get_ipython"):
#    print "IPython detected - importing user namespace"
    tasks.globals=__builtins__["get_ipython"]().user_ns
else:
    import sys
    if sys.modules.has_key("__main__") and hasattr(sys.modules["__main__"],"__builtins__"):
#	print "Python detected - importing user namespace"
	tasks.globals=sys.modules["__main__"]#.__builtins__.globals()
    else:
	print "Unknown Python version detected - not importing user namespace into tasks module."
"""
Some info on tasks:

An example to use tasks is give in tasks/averagespectrum.py

to try it start ipython and type

Task                    # the currently loaded task instance
Task.par (=value)       # access or set a parameter (without updating)
Task(par1,par2,...)     # run the task with parameters par1,par2,....
tlist                   # to view the available tasks
tload 2                 # to load the task #2 (can also provide a name)
tload "averagespectrum" # i.e., this is safer in code since the task number can change with time
tpars                   # to list all parameters
tpar nchunks=2          # to set a parameter
go                      # to run the task
tpar parfile="averagespectrum_2011-02-15_23:52:15.par"     # to read back a parameter file
treset                  # to reset parameters to default values
tget                    # to read back the parameters from the latest run (will also be done at tload)
tput                    # store input parameters in database
tinit                   # run the initialization routine again (without resetting the parameters to default values)
thelp                   # print documentation of task module
"""


def tload(name,**args):
    """
    Loads a specific task as the current task, you you can start it
    with 'go' and set parameters with 'par x=value'.

    One can also provide a number, which refers to the number
    associated with the task when typing 'tlist'.
    """
    import pycrtools.tasks
    if type(name)==int: name=tasks.task_allloaded.keys()[name]
    tasks.task_class=eval("pycrtools."+tasks.task_allloaded[name]+"."+name)
    tasks.task_instance=tasks.task_class(**args)
    tasks.set_globals("Task",tasks.task_instance)
    tget()
    tpars(False)

def task():
    """
    Return the currently loaded task.
    """
    return tasks.task_instance

class t_class():
    """
    Dummy base class which redefined the __repr__ object such that the
    user can call a function by simply typing the name of an instance
    without brackets.
    """
    def __repr__(self):
#	if tasks.task_instance == None: return ""
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
#	if tasks.task_instance == None: return ""
	print "Starting task",tasks.task_name
	tput()
	tasks.task_instance()
	print "Task",tasks.task_name,"run."
	return ""
    def __call__(self,*args,**kwargs):
	tput()
	if kwargs.has_key('task') and not kwargs["task"]==None:
	    ttask=kwargs["task"]
	    del kwargs["task"]
	else:
	    ttask=tasks.task_instance
	ttask(*args,**kwargs)
#	    if len(args)>0:
#	else:
#	    ttask()
	print "Task",ttask.__taskname__,"run."
	
go = go_class()

class tpars_class(t_class):
    """
    Dummy class to let the user view the current parameter set by simply typing 'go'.
    """
    def __call__(self,noninputparameters=True,workarrays=False):
	print "Parameters of task",tasks.task_name
	print tasks.task_instance.ws.__repr__(workarrays=workarrays,noninputparameters=noninputparameters)

tpars = tpars_class()

class tinit_class(t_class):
    """
    Class to let the user run the initialization part of a task. This
    will result in a forced initialization, irrespective of whether
    this has been run before, unless force=False.
    """
    def __call__(self,force=True):
	tasks.task_instance.callinit(forceinit=force)

tinit = tinit_class()

class thelp_class(t_class):
    """
    Print documentation of tasks
    """
    def __call__(self,what=0):
	import pycrtools.tasks
	if what == 0:
	    print  tasks.task_instance.__doc__
	    print """\nTo get help on tasks type 'thelp 1'"""
	if what == 1:
	    print pycrtools.tasks.__doc__
	    print """\nTo get help on current task type 'thelp 0'"""

thelp = thelp_class()

class tlist_class(t_class):
    """
    Class to let the user list the available tasks that can be loaded with tload.
    """
    def __call__(self):
	print "Available Tasks:"
	print "----------------"
	for tpl in zip(range(len(tasks.task_allloaded)),tasks.task_allloaded.keys()):
	    print "{0:2}: {1}".format(*tpl)

tlist = tlist_class()

class tget_class(t_class):
    """
    Usage:

    tget -> Class to let the user retrieve the input parameters from the
    system data base on disk. See also 'tput'
    """
    def __call__(self):
	tasks.task_instance.get()
	tasks.task_instance.ws.update()
	
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
    def __call__(self,restorecallparameters=False,**args):
	tasks.task_instance.reset(restorecallparameters=False,**args)
	tpars(False,False)
	
treset = treset_class()

class tput_class(t_class):
    """
    Usage:

    tput -> Class to let the user store the input parameters to the
    system data base on disk. See also 'tget'
    """
    def __call__(self):
	tasks.task_instance.put()

tput = tput_class()

class tpar_class():
    """Sets the parameters in the workspace of a task.

    Example:

    In ipython you can say something like
    
    par x=2

    to set parameter x of the current task.

    If no parameters are provided, just print the input parameters.
    """
    def __call__(self,**args):
	tasks.task_instance.ws(**args)
	tasks.task_instance.ws.update()
    def __repr__(self):
#	if tasks.task_instance == None: return ""
	print "Input parameters of task",tasks.task_name
	print tasks.task_instance.ws.__repr__(workarrays=False,noninputparameters=False)
	return ""
    
tpar=tpar_class()

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
	exec("del tasks.task_instance.ws."+p)
    tasks.task_instance.ws.update()


#Now importing all modules with tasks
for mn in tasks.task_modules:
    m=__import__("tasks",fromlist=[mn])
    setattr(tasks,mn,getattr(m,mn))
