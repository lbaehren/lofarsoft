"""Configuration and settings.
"""

import os

from hftools import *

LOFARSOFT=os.environ["LOFARSOFT"]
PYCRBIN=LOFARSOFT+"/release/bin/python/"
PYCR=LOFARSOFT+"/src/CR-Tools/implement/Pypeline/scripts/"

pydocpyfilename=LOFARSOFT +"/build/cr/implement/Pypeline/hftools.doc.py"
pydoctxtfilename=LOFARSOFT +"/build/cr/implement/Pypeline/hftools.doc.tex"

filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecond.h5"
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"
filename_lopes_test=LOFARSOFT+"/data/lopes/test.event"
filename_lofar=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename_lopes=LOFARSOFT+"/data/lopes/example.event"

DEFAULTDATAFILE=filename_lopes

#Current tasks
#import pycrtools.tasks #as tasks
#import pycrtools.tasks.averagespectrum
#import pycrtools.tasks.imager


"""
Some info on tasks:

An example to use tasks is give in tasks/averagespectrum.py

to try it start ipython and type

taskload                # to load the tasks
tlist                   # to view the available tasks
tload 2                 # to load the task #2 (can alsO providE a name)
tload "averagespectrum" # is safer
tpars                   #to list all parameters
tpar nchunks=2          # to set a parameter
go                      #to run the task
tpar parfile="averagespectrum_2011-02-15_23:52:15.par"     # to read back a parameter file
treset                  #to reset parameters to default values
tget                    #to read back the parameters from the latest run (will also be done at tload)
tput                    #store input parameters in database

"""
task_instance = None
task_name = ""
task_class= None

#list of all loaded tasks
loaded_tasks={}

import pdb
#pdb.set_trace()

def tload(name,**args):
    """
    Loads a specific task as the current task, you you can start it
    with 'go' and set parameters with 'par x=value'.

    One can also provide a number, which refers to the number
    associated with the task when typing 'tlist'.
    """
    import pycrtools.tasks
    if type(name)==int: name=loaded_tasks.keys()[name]
    global task_class,task_instance
    task_class=eval(loaded_tasks[name]+"."+name)
    task_instance=task_class(**args)
    tget()
    tpars(False)

def task():
    """
    Return the currently loaded task.
    """
    return task_instance

class t_class():
    """
    Dummy base class which redefined the __repr__ object such that the
    user can call a function by simply typing the name of an instance
    without brackets.
    """
    def __repr__(self):
	self()

class go_class():
    """
    Dummy class to let the user start the current task by simply typing 'go'.
    """
    def __repr__(self):
	print "Starting task",task_name
	tput()
	task_instance()
	print "Task",task_name,"run."
    def __call__(self,tsk=None,**args):
	tput()
	if not tsk==None:
	    ttask=tsk
	else:
	    ttask=task_instance
	if len(args)>0:
	    ttask(**args)
	else:
	    ttask()
	print "Task",ttask.__taskname__,"run."
	
go = go_class()

class tpars_class(t_class):
    """
    Dummy class to let the user view the current parameter set by simply typing 'go'.
    """
    def __call__(self,noninputparameters=True,workarrays=False):
	print "Parameters of task",task_name
	print task_instance.ws.__repr__(workarrays=workarrays,noninputparameters=noninputparameters)

tpars = tpars_class()

class tlist_class(t_class):
    """
    Dummy class to let the user list the available tasks that can be loaded with tload.
    """
    def __call__(self):
	print "Available Tasks:",zip(range(len(loaded_tasks)),loaded_tasks.keys())

tlist = tlist_class()

class tget_class(t_class):
    """
    Usage:

    tget -> Class to let the user retrieve the input parameters from the
    system data base on disk. See also 'tput'
    """
    def __call__(self):
	task_instance.get()
	task_instance.ws.update()
	
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
	task_instance.reset(restorecallparameters=False,**args)
	tpars(False,False)
	
treset = treset_class()

class tput_class(t_class):
    """
    Usage:

    tput -> Class to let the user store the input parameters to the
    system data base on disk. See also 'tget'
    """
    def __call__(self):
	task_instance.put()

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
	task_instance.ws(**args)
	task_instance.ws.update()
    def __repr__(self):
	print "Input parameters of task",task_name
	print task_instance.ws.__repr__(workarrays=False,noninputparameters=False)

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
	exec("del task_instance.ws."+p)
    task_instance.ws.update()

# Read in the doc strings for the functions in hftools
execfile(pydocpyfilename)
f=open(pydoctxtfilename)
pycralldocstring=f.read()
f.close()

trackHistory.__doc__="""
Function for PYCRTOOLS module. Globally switch history tracking of
hArrays on (True) or off (False). Default at start-up is "on". This
setting is valid for all arrays created after the function is
called. If history tracking is on, you can use array.history() to see
a list of actions that were performed with the array."""

