"""
============
Module Tasks
============

What are tasks?
===============

Task are python classes that perform computations of a higher level of
complexity. They are intended as major building blocks of an automatic
pipeline or as complex tasks to be called interactively by a user.

The main parts of a pipeline are a 'run' function that performs the
main computations, and optional initialization part that is called
before the first time execution of the task, and a *workspace* which
holds all the input and outputsc.output parameters of the task. The workspace
can also hold large scratch arrays and derived parameters, which are
only calculated when needed.

If ``Task`` is an instance of a task and ``par`` is a parameter of
that task, then the parameter can be accessed through Task.par and set
through ``Task.par=value``. Alternatively one can use ``Task["par"]``
and ``Task["par"]=value``. (These are actually getter and setter
functions. The actual value is stored in ``Task._par``).

For interactive use there are a number of easy to remember command
line (i.e., Python prompt) functions that allow convenient loading of
tasks (``tload``), listing of available parameters (``tlist``)
inspecting of parameters (``tpars``), modifying them (``tpar
par=value``), storing and retrieving of parameters
(``tput``/``tget``), and execution of tasks (``go, trun``). The task
instance itself is retrieved with the function ``task()`` or simply
with the variable ``Task``.

How to use tasks?
=================

Executing tasks
---------------

The tasks are autmatically load when importing the pycrtools, e.g::

    import pycrtools as cr

The list of available tasks can be viewed with 'tlist' and a specific
task is loaded via ``tload tasknumber`` or ``tload taskname``. The
task stored last will be reloaded when the interactive session is
restarted the next time.

In general tasks can then be called in the conventional way simply
by calling::

    Task(par1=value1,par2=value2,....).

This actually executes a wrapper function which then puts those
parameters into the workspace and executes the ``Task.run()``
function.


You can also exectue them with ``trun"taskname", par1,par2=xyz,par3=...``.
This will run the task with name "taskname" and assign it to the global variable ``Task``


A task can return a return value (if the run function does return a
value), e.g.::

    value=Task()

The run function of the task can be called repeatedly without
re-running the initialization: ``Task.run()`` executes the run function
and ``Task.init()`` executes the init function.

To run the task one can also simply type ``go`` (also with extra
function parameters, i.e. ``go
positional_parameter1,par1=value1,par2=value2,....)``. This runs the
task one has loaded with ``tload`` and will also call ``tput`` to
store the current input parameters to a system database on disk (in
~/.pycrtools/task).

Here is an overview of interactive functions one can use at the
command prompt in ipython:

================================================== ==================================================================
``Task``                                           the currently loaded task instance
``Task.ws``                                        print the workspace and its parameters (like tpars)
``Task.parname (=value)``                          the access or set a parameter (without updating)
                                                   - parname is the name of the parameter
``Task(par1,par2,...)``                            run the task with parameters ``par1,par2,...``
``tlist``                                          to view the available tasks
``tlog``                                           list the log of recently run tasks, including execution times.
``tload 2``                                        to load the task #2 (can also provide a name)
``tload "averagespectrum"``                        i.e., this is safer in code since the task
                                                   number can change with time
``tpars``                                          to list all parameters
``tpar nchunks=2``                                 to set a parameter
``go``                                             to run the loaded task
``trun "taskname", pars...``                       load and run the task with name "taskname" -> ``Task``
``tpar parfile="avgspec_2011-02-15_23:52:15.par"`` to read back a parameter file
``treset``                                         to reset parameters to default values
``tget (name)``                                    to read back the parameters from the latest run
                                                   - will also be done at tload - or get the one stored under 'name'
``tput (name)``                                    store input parameters in database (under 'name')
``tinit``                                          run the initialization routine again (without resetting
                                                   the parameters to default values)
``thelp``                                          print documentation of task module
================================================== ==================================================================


Using go, the input parameters in the workspace will be stored
whenever a task is run and hence tasks can be continued across
sessions without having to retype the parameters. The workspace can be
accessed via ``myWorkSpace=Task.ws`` and a workspace can be provided
as input to a task, e.g. ``f(ws=myWorkSpace)``.

parfiles
--------

The task will also write all input and output parameters to a
parameter file (``taskname-TIME.par``) at execution time (and at the
end). This file is in python style and easy to read and to edit with a
normal text editor. Tasks can be run with this file as input to set
the parameters accordingly, using::

    Task(parfile=filename,par1=value1,...)

The location where the .par files are stored can be changed by setting
the global variable ``tasks.task_outputdir='dir...'``in the task
module. To inhibit writing .par files one can set
``tasks.task_write_parfiles = False``. This will affect all tasks. A
list of all task files created within the session, will be stored in
``tasks.task_parfiles``.

The task workspace is a relatively powerful construct. In the most
simplest case it just holds the input parameters. Of course, default
values can be provided so that not all parameters have to be
specified. The parameters can also have various properties determining
for what they are used (e.g. input/output/workarrays) and a
documentation string which is printed at output and included in the
``__doc__`` string of the task.

The default value, however, can also be a function which is executed
the first time the variable is accessed and the result is then stored
as the parameter value. This is one example of an 'output' parameter,
that can, however, be turned into an input parameter if the user
assigns it a new value.

Note, that since derived parameters are only calculated when they are
used, some parameters may never get set if they are not needed. So, do
not panic if the workspace shows a number of 'undefined'
parameters. They simply might no have been called yet and will be
filled during execution of the task.

This 'on-first-call evaluation' can save memory, since not all arrays
need always be created. Also, the order in which derived parameters
are defined does not matter as long as they only depend on other
defined parameters of the workspace. This functionality actually makes
the init function relatively superfluous.

All derived parameters know on which other parameters they depend. If
a parameter in the workspace is modified this information is
preserved. When executing 'Task.update()', then all the derived
parameters which depend directly or indirectly on a modified parameter
will be recalculated. Update will be called automatically if one uses
``tpar par=value`` (or ``tpar(par=value)`` if one does not use ipython).

The value will also be updated if the parameter is deleted ``tdel par``
(Note: when using simply ``del Task.par`` the recalculation happens upon
next calling of this parameter. If ws.update was not called explicitly
after using del (instead of tdel) then parameters which par depends on
might still have their old value!)

ATTENTION: If you use a dict, array or vector as input parameter (or
anything non-atomic) and you modify the elements in the array, then
the task has no chance of knowing that and the next time you start the
task, the input has changed. So, always COPY from an input array to a
work vector if you need to make such modifications, don't just assign
them the input array (you will just create a reference in this case)!

**Logging:**

Some basic logging and performance evaluation is built in. The
variable ``tasks.task_logger`` will contain a list of dicts with
recently run tasks, their names, start and execution times.

To reset the logger simply set ``tasks.task_logger=[]``.


How to program tasks?
=====================

The modules to import in a task module are::

  from pycrtools import *
  import pycrtools.tasks as tasks

If one wants to add a new task then it should either be defined in a
separate new file in the directory modules/tasks or it should be added
to one of the files in modules/tasks.

The four ingredients of a task are the parameters definition (a dict
stored in ``Task.parameters``), an init function, a call function, and a
run function. Of these only the run function is really required, but
either the parameters dict or the call function should be there to
define the input parameters.

A simple example is given below::

  class test(tasks.Task):
      \"""
      Documentation of task - parameters will be added automatically
      \"""
      parameters = dict(
          x=dict(default=None, doc='x-value - a positional parameter', positional=1),
          y=dict(default=2, doc='y-value - a normal keyword parameter'),
          xy=dict(default=lambda ws: ws.y * ws.x, doc='Example of a derived parameter.'))
      def init(self):
          print "Calling optional initialization routine - Nothing to do here."
      def run(self):
          print "Calling Run Function."
          print "self.x=",self.x,"self.y=",self.y,"self.xz=",self.xy


Defining parameters
-------------------

First the parameters are defined as a dict.  The parameter dict
consists of key value pairs, where the key is the variable name and
the value is again a dict with the various properties of that
particular variable. The following properties are defined

default
  Any default value that will be assigned at first access of the
  parameter. The default value can also be a function of the form
  (lambda ws: function body). The parameter ws is the workspace and
  the function body can make reference to any parameter in the
  workspace of the task through 'ws.par'.

doc
  A documentation string of the parameter

unit
  An optional unit string describing the unit of the values (for
  output only).

dependencies
  A dict with parameters the parameter depends on (will be generated
  automatically)

workarray
  True/False - this parameter is a large data array that will not be
  saved and is listed in a separate section.

export
 True/False - If False, then don't output this parameter to the .par
 file or list them. Also don't include it with getParameters()

output
  True/False - If True, explicitly consider this an output parameter

positional
  Integer - If > 0, this is a positional parameter at the indicated
  position (``positional=1``, means first positional parameter, 0
  would be ``self`` which cannot be used)

Yet another, and perhaps more recognizable form of defining input
parameters is to provide a dummy call function will all input
parameter in the definition. E.g.::

  class test(tasks.Task):
      \"""
      Documentation of task - parameters will be added automatically
      \"""
      def call(self,x,y=2,xy=lambda ws:ws.x*ws.y):
          pass
      def init(self):
          print "Calling optional initialization routine - Nothing to do here."
      def run(self):
          print "Calling Run Function."
          print "self.x=",self.x,"self.y=",self.y,"self.xz=",self.xy

This has the same effect, but has the disadvantage of not providing
documentation strings or other options. Both methods, however, can be
combined, where a parameters dict contains the missing properties or
additional parameters.

Note, that the call function is actually never called. You may,
however, provide it with any code and use it for testing purposes.


Run and init function
---------------------

The run function does the actual calculations. It will have no
parameters (other than self, of course). When it is called, run can
assume that all the parameters are available in the form
``self.par``. Filling those values with the input parameters and
calculating the derived parameters is done 'behind the scenes'.


Example
-------

Once the task is imported, e.g. here with::

  import tasks.averagespectrum

then an instance can be created with::

  t=tasks.averagespectrum.test1()

which can then be called, e.g.::

  >>> t(5)
  Calling Run Function.
  self.x= 5 self.y= 2 self.xz= 10

The parameters are accessed through t.par, i.e. here::

  >>> t.x
  5

Parameters can already be set at instantiation and provided as keyword
arguments::

  >>> t=tasks.averagespectrum.test1(y=3)

  >>> t(5)
  Calling optional initialization routine - Nothing to do here.
  Calling Run Function.
  self.x= 5 self.y= 3 self.xz= 15


Parameter file
--------------

If one types ``ls -rtl *.par`` one will find the latest parameter files
generated during execution time, e.g::

  -rw-r--r--  1 falcke  staff   696 Feb 17 00:22 test1_2011-02-17_00:22:13.par

We can inspect this with ``cat test1_2011-02-17_00:22:13.par``
or edit it.::

  # Task: averagespectrum saved on 2011-02-17 00:22:13
  # File: test1_2011-02-17_00:22:13.par
  #-----------------------------------------------------------------------
  # WorkSpace of test1
  #-----------------------------------------------------------------------
  x                      = 5                              #         x-value - a positional parameter
  y                      = 3                              #         y-value - a normal keyword parameter
  #------------------------Output Parameters------------------------------
  #                   xy = 15                             - Example of a derived parameter.
  #-----------------------------------------------------------------------


The task can be executed with the parameter file as input::

  t(parfile='test1_2011-02-17_00:22:13.par')

The parameters in the file can, however, be explicitly overwritten
using keyword arguments, i.e::

  >>> t(5,parfile='test1_2011-02-17_00:22:13.par',y=2)
  Calling Run Function.
  self.x= 5 self.y= 2 self.xz= 10


Shortcut example
----------------

To simplify running a task one can use the 't'-shortcuts.

Here is an example of using it::

  >>> taskload

  >>> tlist
  Available Tasks: [(0, 'test1'), (1, 'Imager'), (2, 'test2'), (3, 'averagespectrum')]

  >>> tload 0
  ------> tload(0)
  Parameters of task test1
  #-----------------------------------------------------------------------
  # WorkSpace of test1
  #-----------------------------------------------------------------------
  x                      = None                           #         x-value - a positional parameter
  y                      = 2                              #         y-value a normal keyword parameter
  #-----------------------------------------------------------------------

  >>> Task(5)              # call task directly
  Calling Run Function.
  self.x= 5 self.y= 2 self.xz= 10

  >>> Task(5,y=10)         # call it with keyword arguments
  Calling Run Function.
  self.x= 5 self.y= 10 self.xz= 50

  >>> go                # start task with go, which can't handle positional parameters well yet
  Starting task test1
  Number of positional arguments provided ( 0 ) is less than required number ( 1 ). Keeping previous values.
  Calling Run Function.
  self.x= 5 self.y= 10 self.xz= 50
  Task test1 run.

"""

import os
import math
import shelve
import types
import time

import pycrtools as cr
from sys import version_info
import matplotlib.pyplot as plt

# import pdb
# pdb.set_trace()
#       if hasattr(self,"trace") and self.trace: pdb.set_trace()

# Configuration (should be moved to config.py)
configdir = os.path.expanduser('~/.pycrtools')

if not os.path.isdir(configdir):
    os.mkdir(configdir)

dbfile = configdir + "/" + "task"

globals = None


def set_globals(var, val):
    if isinstance(globals, dict):
        globals[var] = val
    elif isinstance(globals, types.ModuleType):
        setattr(globals, var, val)

Task = None
task_instance = None
task_instances = {}  # Will store instances of tasks loaded with rerun
task_allloaded = {}  # list of all loaded tasks
task_name = ""  # Name of currently loaded task
task_class = None
task_list = set()
task_outputdir = ""  # Where to write the .par files
task_write_parfiles = True  # Write parfile during execution of a task
task_parfiles = []  # List of all parfiles created since launch of session
task_logger = []  # List of all parfiles created since launch of session


class TaskInit(type):
    """Metaclass for tasks.

    Should never be used direcly.
    All tasks should derive from ..class::`Task` instead.
    """

    def __init__(cls, name, bases, dct):
        """ Create a new class.
        """
        cls.__taskname__ = name
        if not name == "Task":
            if cls.__module__[:10] == "pycrtools.":
                task_allloaded[name] = cls.__module__[10:]
            else:
                task_allloaded[name] = cls.__module__
        super(TaskInit, cls).__init__(name, bases, dct)
        cls.addtask()

    def __call__(cls, *args, **kwargs):
        """ Create a new instance.
        """

        # Create instance with restored arguments
        obj = type.__call__(cls, *args, **kwargs)

        return obj

    def addtask(cls):
        """
        Adds a task to the library and adds its parameters to the documentation.
        """
        if cls.__taskname__ == 'Task':
            return
        task_list.add(cls.__module__ + "." + cls.__taskname__)
        if hasattr(cls, "parameters"):
            dct = cls.parameters
        else:
            dct = {}
            if hasattr(cls, "WorkSpace") and hasattr(cls.WorkSpace, "parameters"):
                dct.update(cls.WorkSpace.parameters)
        if (not isinstance(cls.__doc__, str)):
            cls.__doc__ = ""

        # Add parameter documentation
        par_doc_input, par_doc_output = cls.pardoc(dct, indent="  ")
        if par_doc_input:
            cls.__doc__ += "\n  **Input parameters**\n" + par_doc_input
        if par_doc_output:
            cls.__doc__ += "\n  **Output parameters**\n" + par_doc_output
        cls.__doc__ += "\n"

    def pardoc(cls, dct, indent=""):
        """
        Return a pretty string describing a parameter based on its
        properties. Typcially added to the __doc__ string of a class.
        """
        par_doc_input = ""
        par_doc_output = ""

        newline = "\n" + indent

        for p, v in sorted(dct.items()):
            par_doc = newline + "*" + p + "*"
            # Check for default values
            if ("default" in v and (not isinstance(v["default"], types.FunctionType))):
                par_doc_default = str(v["default"]).strip()
                par_doc += " [default value: "
                if isinstance(v["default"], (int, long, float)):  # Scalars
                    par_doc += par_doc_default
                elif isinstance(v["default"], (str, unicode, basestring)):  # Print string values in quotes
                    par_doc += "'" + par_doc_default + "'"
                elif v["default"] is None:  # None value
                    par_doc += "None"
                else:
                    if par_doc_default:
                        par_doc_default = par_doc_default.__repr__()
                        if ((par_doc_default.startswith("'") and par_doc_default.endswith("'")) or
                            (par_doc_default.startswith('"') and par_doc_default.endswith('"'))):
                            par_doc_default = par_doc_default[1:-1]
                        par_doc_default = par_doc_default.replace(r'\n', '')
                        par_doc += "``" + par_doc_default + "``"
                par_doc += " ]"
            par_doc += newline
            # Check for documentation
            if "doc" in v:
                par_doc += "  " + v["doc"].strip() + newline * 2
            else:
                par_doc += newline
            # Add parameter documentation to input- or output parameter documentation.
            if (("default" in v and (isinstance(v["default"], types.FunctionType))) or
                ("output" in v and v["output"])):
                par_doc_output += par_doc
            else:
                par_doc_input += par_doc

        return par_doc_input, par_doc_output


class Task(object):
    """Base class from which all tasks should be derived.

    *ws* - provide a workspace to initialize parameters
    *parfile* - provide a (python) parameter file defining variables, to initialize parameters
    *kwargs* - any parameter=value pair to set the value of the respective parameter in the workspace

    To create a task instance say::

        >>> Task = tasks.tasknam.TaskName()

    To run it call :func:`Task`, or directly instantiate and run it
    by::

        >>> Task = tasks.tasknam.TaskName()(pars ....)

    To (re)run the task with currently set parameters, simply use::

        >>> Task()

    If called for the first time the initialization
    routine will be called.

    To rerun the task with the positional parameters ``xN`` and the
    parameters ``parN`` set to the values provided and keeping the
    previous parameters the same, use::

        >>> Task(x1,x2,...,par1=val1,par2=val2)

    ``Task.ws`` returns the workspace (and print all the pararameters)

    Parameters for calling the Task:

    *init* = False - force the initalisation to run again

    *parfile* = filename - read parameters from file

    *pardict* - provide a dict with paramter value pairs or a
    taskname and a paramter dict. Parameters from the top level
    and in a dict with a taskname will be assigned. The dicts can
    be nested.

    *ws* - replace workspace with a different Workspace and then
    update parameters therein as provided in the file and the
    keywords.

    ``ws`` parameters will be overwritten by file parameter and they
    will be overwritten by keyword parameters (which thus have
    the highest priority).

    If the run function returns a value, this value will be
    returned otherwise the task instance itself will be
    returned. Hence you can access all parameters through the
    returned task object.
    """
    __metaclass__ = TaskInit

    def __init__(self, ws=None, parfile=None, **kwargs):

        self.__modulename__ = self.__module__.split(".")[-1]
        self.__taskname__ = self.__class__.__taskname__
        task_name = self.__taskname__
        task_instance = self

        margs = cr.readParfiles(parfile)  # Reading parameters from a file
        margs.update(**kwargs)

        if not hasattr(self, "WorkSpace"):  # Create a Default WorkSpace class if it does not exist yet
            if hasattr(self, "parameters"):
                property_dict = self.parameters
            else:
                property_dict = {}
            if hasattr(self, "call"):  # retrieve the parameters from the parameters of the callfunction
                if self.call.im_func.func_defaults == None:
                    n_named_pars = 0
                else:
                    n_named_pars = len(self.call.im_func.func_defaults)  # number of named parameters which have defaults
                n_positional_pars = self.call.im_func.func_code.co_argcount - n_named_pars
                named_pars = self.call.im_func.func_code.co_varnames[n_positional_pars:]
                positional_pars = self.call.im_func.func_code.co_varnames[1:n_positional_pars]  # start at 1 to exclude "self" argument
                npos = 0
                for p in positional_pars:
                    npos += 1
                    if p in property_dict:
                        property_dict[p].update({"default": None, "export": False, "positional": npos})
                    else:
                        property_dict[p] = {"default": None, "export": False, "positional": npos}
                if n_named_pars > 0:
                    for p, v in zip(named_pars, self.call.im_func.func_defaults):
                        if p in property_dict:
                            property_dict[p].update({"default": v})
                        else:
                            property_dict[p] = {"default": v}
            self.WorkSpace = WorkSpace(self.__taskname__, parameters=property_dict)  # This creates the work space class that is used to create the actual ws instance

        if ws == None:
            self.ws = self.WorkSpace(**margs)  # create default workspace of this task
        else:
            self.ws = ws(**margs)  # or take the one which was provided already

        # Add setter and getter functions for parameters to task class
        pp = set(self.ws.getParameterNames(self, all=True)).difference(set(dir(self)))
        for p in pp:
            self.addProperty(p, eval("lambda slf:slf.ws['" + p + "']"), eval("lambda slf,x:slf.ws.__setitem__('" + p + "',x)"), eval("lambda slf:slf.ws.delx('" + p + "')"), self.ws.getParameterDoc(p))

        self._initialized = False
        self.ws.evalInputParameters()

    def callinit(self, forceinit=False):
        """
        Calls the initialization routine if it wasn't run yet (or force it to run nonetheless)
        """
        if hasattr(self, "init") and (not self._initialized or forceinit):
            self.init()
            self._initialized = True

            # Add those parameters that were added during init phase
            pp = set(self.ws.getParameterNames(self, all=True)).difference(set(dir(self)))
            for p in pp:
                self.addProperty(p, eval("lambda slf:slf.ws['" + p + "']"), eval("lambda slf,x:slf.ws.__setitem__('" + p + "',x)"), eval("lambda slf:slf.ws.delx('" + p + "')"), self.ws.getParameterDoc(p))

    def saveOutputFile(self):
        """
        Save the parameters to a file that can be read back later with
        the option parfile=filename (e.g., tpar parfile=filename)
        """
        if not task_write_parfiles:
            return
        self.parfile = os.path.join(task_outputdir, self.oparfile)
        if len(task_parfiles) == 0 or not task_parfiles[-1] == self.parfile:
            task_parfiles.append(self.parfile)
        f = open(self.parfile, "w")
        f.write("# Task: " + self.__modulename__ + " saved on " + time.strftime("%Y-%m-%d %H:%M:%S") + "\n# File: " + self.oparfile + "\n")
        f.write(self.ws.__repr__(internals=False, workarrays=False))
        f.close()

    def addProperty(self, name, *funcs):
        """
        Add a python "property" to the class which contains getter and setter functions for methods.

        Example:

        self.addProperty(name,lambda self:self[name],lambda self,x:self.__setitem__(name,x),lambda self:self.delx(name),"This is parameter "+name)
        """
        setattr(self.__class__, name, property(*funcs))  # property(getx, setx, delx, "I'm the property.")

    def __call__(self, *args, **kwargs):
        """
        Usage:

        task() -> (re)run the task with currently set parameters.

        If called for the first time the initialization
        routine will be called.

        task(x1,x2,...,par1=val1,par2=val2) -> rerun the task with
        the positional parameters xN and the parameters parN set to
        the values provided and keeping the previous parameters the
        same

        *init* = False - force the initalisation to run again

        *parfile* = filename - read parameters from file

        *pardict* - provide a dict with paramter value pairs or a
         taskname and a paramter dict. Parameters from the top level
         and in a dict with a taskname will be assigned. The dicts can
         be nested.

        *ws* - replace workspace with a different Workspace and then
        update parameters therein as provided in the file and the
        keywords.

        ws parameters will be overwritten by file parameter and they
        will be overwritten by keyword parameters (which thus have
        the highest priority).

        If the run function returns a value, this value will be
        returned otherwise the task instance itself will be
        returned. Hence you can access all parameters through the
        returned task object.

        """
        ws = None
        init = False
        if "ws" in kwargs:
            ws = kwargs["ws"]
            del kwargs["ws"]
        if "init" in kwargs:
            init = kwargs["init"]
            del kwargs["init"]

        self._starttime = time.strftime("%Y-%m-%d_%H:%M:%S")
        self.oparfile = self.__taskname__ + "_" + self._starttime + ".par"
        self.ws.addParameterDefinition("t0", dict(default=time.clock(), doc="Unix start time of task", unit="s", output=True))
        self.ws.addParameterDefinition("tduration", dict(default=-1, doc="Execution time of task", unit="s", output=True))
        self["t0"] = time.clock()

        if not ws == None:
            self.ws = ws           # Updating WorkSpace
        self.ws(**kwargs)

        if len(self.ws._positionals) < len(args):
            print "Number of positional arguments provided (", len(args), ") is larger than required number (", len(self.ws._positionals), "). Stopping."
            return
        if len(self.ws._positionals) > len(args):
            print "Number of positional arguments provided (", len(args), ") is less than required number (", len(self.ws._positionals), "). Keeping previous values."
        if len(args) > 0:  # Setting positional parameters if provided
            for p, v in zip(self.ws._positionals[:len(args)], args):
                self.ws[p] = v

        self.ws.update()  # make sure all parameters are now up-to-date
        self.callinit(forceinit=init)  # Call initialization if not yet done

        self.saveOutputFile()

        print "[" + self.__taskname__ + "] started"

        retval = self.run()

        self["tduration"] = time.clock() - self["t0"]  # Execution time of task

        print "[" + self.__taskname__ + "] completed in {0:.3f} s".format(self["tduration"])

        self.saveOutputFile()  # to store final values

        # Add logging information
        task_logger.append(dict(name=self.__taskname__,
                                version=str(self._version_name) if hasattr(self, "_version_name") else "",
                                start_time=" ".join(self._starttime.split("_")),
                                start_time_cpu=self["t0"],
                                execution_time=self["tduration"])
                           )

        if retval == None:
            return self
        else:
            return retval

    def __getitem__(self, par):
        """
        Usage:

        task["parname"] -> Return value of a parameter in the workspace

        Access the parameter value using square brackets and a string
        of the parameter name. The basic "getter" function for workspace parameters.
        """
        return self.ws[par]

    def __setitem__(self, par, value):
        """
        Usage:

        task["parname"] = value -> Set value of a parameter in the workspace

        Set the parameter value using square brackets and a string
        of the parameter name. The basic "setter" function for workspace parameters.
        """
        self.ws[par] = value

    def put(self, name="", delete=False):
        """
        Stores the input parameters in the workspace to the parameter
        database (see also 'tput').  This can be restored with
        Task.get() (or 'tget' from the command line).

        Task.get(name) will retrieve the parameters stored under 'name'

        *delete* = False - If True the database entry will be deleted.

        """
        # # Open task database
        taskdb = shelve.open(dbfile)

        if not name == "":
            name = "_" + str(name)

        if delete:
            del taskdb[self.__taskname__ + name]
        else:
            taskdb[self.__taskname__ + name] = self.ws.getInputParametersDict()

        taskdb.close()

    def get(self, name=""):
        """
        Gets the input parameters in the workspace from the parameter
        database (see also 'tget').  This can be stored there with
        task.put() (or 'tput' from the command line).

        task.put(name) will store the parameters under the keyword
        name and can be retrieved with put under this name.

        If the name is not known a list of all known names is given.
        """
        # # Open task database
        taskdb = shelve.open(dbfile)

        if not name == "":
            name = "_" + str(name)

        if self.__taskname__ + name in taskdb:
            # Restoring from database
            args = taskdb[self.__taskname__ + name]
            self.ws(**args)
            retval = True
        else:
            print "Available parameter sets:\n-------------------------\n", ", ".join(taskdb.keys()), "\n"
            retval = False

        taskdb.close()
        return retval

    def reset(self, restorecallparameters=False, init=True, **args):
        """
        Usage:

        task.reset() -> Reset all parameter to the default state and rerun the initialization.

        task.reset(par1=val1,par2=val2,...) -> reset all parameters to
        default and initialze with parameters provided.

        task.reset(restorecallparameters=True,par1=val1,par2=val2,...)
        -> reset all parameters to their state at initialization, but
        keep the parameters provided during the last initialisation.

        *init* = True - If False, don't force a re-run of the init routine.
        """
        self.ws.reset(restorecallparameters=restorecallparameters)
        self.ws.__init__(**args)
        self.callinit(forceinit=init)

    def update(self, forced=False, workarrays=True):
        """
        Recalculates all existing derived parameters and assigns them
        their default values if they depend on a value that was
        modified. Note that parameters which had a default function at
        initialization but were set explicitly will not be
        recalculated. Use ws.reset() or del ws.par first.

        *forced* = False - If True, then update all parameters
         irrespective of whether they depend on modified parameters or
         not.
        """
        self.ws.update(forced=forced, workarrays=workarrays)

    def updateHeader(self, ary, parameters=[], **kwargs):
        """
        Usage:
        Task.updateHeader(harray,parameters=['parname1','parname2',...],newparname1=oldparname1,newparname2=oldparname2,....)

        Will set parameters in the header dict of the hArray
        'harray'.

        First of all, there will be a new dict named according to the
        current task, containing all exportable parameters.

        Secondly, one can set additional parameters of the
        Task as (top-level) header parameters, by providing their name
        in the list 'parameters' or as keyword arguments (the latter
        allows one to give them a different name in the header).

        *parameters* = [] - a list of task parameter names to be saved in the header

        *headerkeyword* = taskkeyword - assign the header keyword the
         value of the respective task keyword

        """
        ary.setHeader(**{self.__taskname__: self.ws.getParameters()})
        for p in parameters:
            ary.setHeader(**{p: getattr(self, p)})
        for p, v in kwargs.items():
            ary.setHeader(**{p: getattr(self, v)})

    def plotpause(self):
        """
        Function to be called after a plotting command. If
        ``self.plot_pause = True`` it will pause and ask for user input
        whether and how to continue calculation and plotting.
        May modify `self.plot_pause`` and ``self.doplot``.
        """
        if hasattr(self, "plot_pause") and self.plot_pause:
            plt.draw()
            k = raw_input("Press 'return' to continue. Press 'q+return' to proceed without pausing, 'n+return' to continue without plotting...")
            if k == "q":
                self.plot_pause = False
                print "Continue without pausing in this task (other tasks not affected)."
            elif k == "n":
                self.doplot = False
                print "Continue without plotting in this task (other tasks not affected)."
                plt.ion()
            else:
                print "Continue."

    def writehtml(self, results=None, parfiles=None, plotfiles=None, text=None, logfiles=None, output_dir=None, htmlfilename="index.html", filename=None, tduration=-1):
        """
        **Usage:**

        Task.writehtml(self,results=None,parfiles=None,plotfiles=None,text=[],logfile=None,output_dir=None,htmlfilename="index.html",filename=None, tduration=-1):

        **Description:**

        Will write an informative html page containing plots and output parameters from the task.

        The following parameters are availaible. Typically none of
        them needs to be specified, they will be automatically deduced
        from the task parameters.

        **Parameters:**


        *results* - a dict containing all results as keyword value
         pairs (will be printed as table). By default all output
         parameters are listed here

        *output_dir* - where to write the html file (use
         self['output_dir'] if exist and is not provide with the
         function)

        *text* - a list of strings that will be included in the html file as such

        *htmlfilename* - root file name of the output file (typically
          index.html)

        *filename* - data filename this html page belongs to

        *parfiles* - a list of parfile names that were produced by the task

        *logfiles* - a list of logfiles produced by the task

        *plotfiles* - a list of images that were plotted by the
         task. If an entry if a list of images, the put those in one
         row.

        *tduration* - execution time of task

        """

        if not results:
            results = {}
            for parameter_name in self.ws.getOutputParameters(workarrays=False, nonexport=True):
                if hasattr(self.ws, "_" + parameter_name):  # Take only those which have a value already defined
                    res = self[parameter_name]
                    if isinstance(res, tuple(cr.hAllContainerTypes)):
                        res = list(res)
                    results[parameter_name] = res

        if not output_dir:
            if hasattr(self, "output_dir"):
                output_dir = self["output_dir"]
            else:
                output_dir = ""

        if not filename:
            if hasattr(self, "filename"):
                filename = self["filename"]
            else:
                filename = ""

        if not text:
            if hasattr(self, "hprint"):
                text = self.hprint.textbuffer
            else:
                text = []

        if tduration < 0 and hasattr(self, "t0"):
            tduration = time.clock() - self["t0"]

        if not parfiles:
            if hasattr(self, "parfile"):
                parfiles = [self.parfile]
            else:
                parfiles = []

        if not plotfiles:
            if hasattr(self, "plot_finish"):
                plotfiles = self.plot_finish.plotfiles
            else:
                plotfiles = []

        if not logfiles:
            if hasattr(self, "logfile") and self.logfile:
                logfiles = [self.logfile]
            else:
                logfiles = []

        # Header information
        htmlfile = open(os.path.join(output_dir, htmlfilename), "w")
        htmlfile.write("<html><head><title>{0:s}</title></head><body>\n".format(filename))
        htmlfile.write("<h1>{0:s}</h1>\n".format(filename))
        htmlfile.write("<h2>Parameters</h2>\n".format())
        htmlfile.write("<i>File processed on {0:s}, by user {1:s}. Processing time = {2:5.2f}s</i><br>\n".format(
            time.strftime("%A, %Y-%m-%d at %H:%M:%S"), os.getlogin(), tduration))

        # Write results parameter dict
        l = results.items()
        l.sort()

        htmlfile.write('<table border="1">\n'.format())
        for k, v in l:
            htmlfile.write("<tr><td><b>{0:s}</b></td><td>{1:s}</td></tr>\n".format(k, str(v)))
        htmlfile.write("</table>\n".format())

        # Write parfile links
        if parfiles:
            htmlfile.write("<h2>Parfiles</h2>\n".format())
        for parfile in parfiles:
            if os.path.exists(parfile):
                if os.path.exists(parfile + ".txt"):
                    os.remove(parfile + ".txt")
                os.symlink(parfile, parfile + ".txt")
                htmlfile.write('<a type="text/http" href="{0:s}.txt">{1:s}</a><br>\n'.format(relpath(output_dir, parfile), os.path.split(parfile)[-1]))

        # Write logfile links
        if logfiles:
            htmlfile.write("<h2>Logfiles</h2>\n".format())
        for logfile in logfiles:
            if os.path.exists(logfile):
                htmlfile.write('<a type="text/http" href="{0:s}">{1:s}</a><br>\n'.format(relpath(output_dir, logfile), os.path.split(logfile)[-1]))

        # Include text log
        if text:
            htmlfile.write("<h2>Output</h2>\n".format())
            htmlfile.write("<XMP>\n".format())
            for txt in text:
                htmlfile.write("{0:s}\n".format(txt))
            htmlfile.write("</XMP>\n".format())

        # Include plotfiles
        if plotfiles:
            htmlfile.write("<h2>Plotfiles</h2>\n".format())
            htmlfile.write('<table>\n'.format())

            for plotfile in plotfiles:
                htmlfile.write('<tr>'.format())
                if isinstance(plotfile, list):
                    for plotfile_i in plotfile:
                        htmlfile.write('  <td><a href="{0:s}">{1:s}</a>:<br><a href="{0:s}"><img src="{0:s}" width=400></a></td>\n'.format(relpath(output_dir, plotfile_i), os.path.split(plotfile_i)[-1]))
                else:
                    htmlfile.write('<td><a href="{0:s}">{1:s}</a>:<br><a href="{0:s}"><img src="{0:s}" width=400></a></td>'.format(relpath(output_dir, plotfile_i), os.path.split(plotfile)[-1]))
                    htmlfile.write('</tr>\n'.format())

            htmlfile.write('</table>\n'.format())

        htmlfile.write("</body></html>\n")
        htmlfile.close()


#########################################################################
#                             Workspaces
#########################################################################
def printindent_string(txt, indentlen, width=80, prefix="# "):
    """
    Usage:

    printindent_string(txt,indentlen,width=80,prefix='# ')

    Description:

    Return a string to print a text as a block of with indentation and maximum width.

    Example:
    ::

        tasks.printindent_string('Hallo lieber Leser, dies ist ein Text, der umgebrochen werden soll!',10,width=20)

    """
    textsize = len(txt)
    blockwidth = width - indentlen - len(prefix)
    indentstr = "                                                                                                                                                                                                        "[:indentlen]
    start = 0
    end = blockwidth
    outstr = ""
    while start < textsize:
        if end >= textsize:
            if start > 0:
                outstr += indentstr + prefix + txt[start:textsize] + "\n"
            else:
                outstr += prefix + txt[start:end] + "\n"
            return outstr
        space_end = txt[start:end].rfind(" ")
        if space_end >= 0:
            end = start + space_end
        if start > 0:
            outstr += indentstr + prefix + txt[start:end] + "\n"
        else:
            outstr += prefix + txt[start:end] + "\n"
        if space_end >= 0:
            start = end + 1
        else:
            start = end
        end = start + blockwidth
    return outstr


class WorkSpaceType(type):
#    def __init__(cls, name, bases, dct):
#       """ Create a new class.
#        """
#       print "Initializing cls=",cls
#       print "Name=",name
#       print "Bases=",bases
#       print "dct=",dct
#       cls.name=name
#       cls.bases=bases
#       cls.dct=dct
#
#        cls.__taskname__ = name
#        super(WorkSpaceType, cls).__init__(name, bases, dct)

    def __call__(cls, taskname='Task', parameters={}, parfile=None, **kwargs):
        """ Create a new instance.

        *taskname* the name of the task for which the Workspace is
        created (only for output)

         *parameter* = a dict defining the parameters of the form
         dict(par1={default:val1,doc:"Documentation"},...)

         *parfile* - if a string, then open filename and read in
         workspace parameters from the file (which is a simple python
         file defining variables, i.e. x=1, y=2,... etc.). The
         parameters are then defined in the workspace with their
         values as defaults.

         *kwarg* - any keyword argument 'kwarg' can be added at will,
         to define additional parameters and their default values.

        """
#       print "Calling WorkSpaceClass"

#       pdb.set_trace()

        # Create instance with restored arguments
#        obj=type(taskname+cls.name,cls.bases,cls.dct)
        obj = type(taskname + cls.__name__, cls.__bases__, cls.__dict__.copy())

        for k, v in cr.readParfiles(parfile).items():
            parameters[k] = {"default": v}

        for k, v in kwargs.items():
            parameters[k] = {"default": v}
        obj.parameters = parameters
        obj.__taskname__ = taskname

        return obj

"""
wsc=WorkSpace("MyTask",x={"default":1},y={"default":2})
wsc=WorkSpace("MyTask",x=1,y=2)
ws=wsc(x=3)
"""


class WorkSpace(object):
    """
    This class holds all parameters, scratch arrays and vectors used
    by the various tasks. Hence this is the basic workspace in the
    memory.

    If 'ws' is the workspace the you can access parameters 'parname' in
    the workspace as ``ws.parname`` and set them with ``ws.parname=value``.

    The 'ws.parname' se are actually getter and setter functions. The
    actual value is stored in ws._parnanme and should not be accessed.

    Every workspace actually creates its own new class, which contains
    these getter and setter functions. The workspace is defined, e.g. by

    *pardict* - provide a dict with paramter value pairs or a
    taskname and a paramter dict. Parameters from the top level
    and in a dict with a taskname will be assigned. The dicts can
    be nested.

    *parfile* - provide a filename from which to read parameters
    in the form ``par1=val1, par2=val2``,...

     """
    __metaclass__ = WorkSpaceType

    def __init__(self, **args):
        if not hasattr(self, "parameters"):
            self.parameters = {}
        self.parameter_properties = self.parameters.copy()
        self.parameterlist = set(self.parameter_properties.keys())
        self._initparameters = args.copy()
        self._parameterlist = set()
        self._positionals = []
        self._modified_parameters = set()
        self._default_parameter_definition = {"doc": "", "unit": "", "default": None, "workarray": False, "export": True}
        self._default_parameter_order = ("default", "doc", "unit")
        self._known_methods = set()
        self._known_methods.update(set(dir(self)))
        self.addParameters(self.parameter_properties)
        self.evalInputParameters()
        if len(args) > 0:
            self(**args)  # assign parameter values from the parameter list

    def setParFromDict(self, d, root=True, taskname=None, follow_tree=False):
        """
        Set parameters in the workspace from parameters in a dict. If
        a taskname is provided and a key in the dict matches the
        taskname and the value is a dict again, then also apply all
        values in the dict. If follow_tree=True, then go recursively
        through all dicts to see if there is one key with an
        associated dict value that matches taskname.

        The function will not complain if a parameter in the dict is
        not known to the workspace. It will simply ignore it.

        *root* = True - This is the toplevel dict of global
        parameters. Set all toplevel key,value pares as parameters in
        the workspace. If False, only do so for a dict that is a
        value of a key matching taskname.

        *taskname* - Name of the task to search for in the recursive
        search. Set parameters in workspace if a key has that name
        and has a dict as value.

        *follow_tree* = False - If True, recursively go through all
        dicts on toplevel (and lower) which are not a parameter in
        the workspace.

        """
        if root:
            for k, v in d.items():
                if (k in self.parameterlist):  # This is a top-level, i.e. global parameter
                    self[k] = v
        for k, v in d.items():
            if (k == taskname) and (isinstance(v, dict)):  # This is a parameter for the current task, stop going through tree
                self.setParFromDict(v)
            elif taskname and follow_tree and (isinstance(v, dict)):  # A taskname is provided and a dict, then go through the tree and search for the taskname
                self.setParFromDict(v, root=False, taskname=self.__taskname__, follow_tree=True)

    def reset(self, restorecallparameters=False):
        """
        ws.reset() -> reset all parameters to their state at initialization

        ws.reset(restorecallparameters=True) -> reset all parameters
        to their state at initialization, but keep the parameters
        provided during initialisation.
        """
        for p in self._parameterlist:  # Delete all actually stored values
            if hasattr(self, p):
                delattr(self, p)
        if restorecallparameters and len(self._initparameters) > 0:
            self(**(self._initparameters))  # assign parameter values from the parameter list
        self.clearModifications()

    def __call__(self, pardict={}, parfile=None, **args):
        """
        Usage:

        ws(par1=val1, par2=val2,...)

        The call function lets one assign and/or update known
        parameters simply by calling the workspace with the parameters
        as arguments.

        *pardict* - provide a dict with paramter value pairs or a
         taskname and a paramter dict. Parameters from the top level
         and in a dict with a taskname will be assigned. The dicts can
         be nested.

        *parfile* - provide a filename from which to read parameters
         in the form par1=val1, par2=val2,....
        """
        self.setParFromDict(pardict, root=True, taskname=self.__taskname__, follow_tree=True)

        margs = cr.readParfiles(parfile)  # Reading parameters from a file
        margs.update(**args)

        for k, v in margs.items():
            if k in self.parameterlist:
                self[k] = v
            else:
                print "Warning ws.__call__: Parameter ", k, "not known."
        return self

    def __getitem__(self, par):
        """
        Usage:

        ws["parname"] -> Return parameter value

        Access the parameter value using square brackets and a string
        of the parametername. The basic "getter" function for parameters.
        """
        if hasattr(self, "_" + par):   # Return locally stored value
            return getattr(self, "_" + par)
        elif par in self.parameter_properties:
            if "default" in self.parameter_properties[par]:  # return default value or function
                f_or_val = self.parameter_properties[par]["default"]
                if isinstance(f_or_val, types.FunctionType):
                    setattr(self, "_" + par, f_or_val(self))
                else:
                    setattr(self, "_" + par, f_or_val)
            else:
                print "ERROR in Workspace", self.__module__, ": Parameter ", par, "does not have default values!"
            return getattr(self, "_" + par)
        elif hasattr(self, par):   # Return locally stored value
            return getattr(self, par)
        else:
            print "ERROR in Workspace", self.__module__, ": Parameter ", par, "not known!"
            return None

    def __setitem__(self, par, value):
        """
        Usage:

          ``ws["parname"] = value``

        Set the parameter value using square brackets and a string
        of the parameter name. The basic "setter" function for parameters.

        If a parmeter 'par' is explicitly set, the value will be
        stored in a local variable ws._par and retrieved with ws.par
        or ws['par']. Also, the default value will be set to this
        value. If the default value was previously a function
        (i.e. the parameter), then the paramaeter will become an input
        parameter that needs to be set explicitly. This will be
        reverted when calling ws.reset() or specifically for that
        parameter with del ws.par.
        """
#        if hasattr(self,"trace") and self.trace:
#            import pdb; pdb.set_trace()
        if hasattr(self, "_" + par) or par in self.parameterlist:  # replace stored value
            if hasattr(self, "_" + par):
                if getattr(self, "_" + par) is value:
                    return  # don't assign or considered modified if it is the same value
                delattr(self, "_" + par)  # Delete first in case it contains a large array which blocks memory
            setattr(self, "_" + par, value)
            self.parameter_properties[par]["default"] = value
        else:
            setattr(self, "_" + par, value)  # create new parameter with default parameters
            self.parameter_properties[par] = self._default_parameter_definition
            self.add(par)
        self._modified_parameters.add(par)

    def clearModifications(self):
        """
        Set all parameters to be unmodified.
        """
        self._modified_parameters = set([])

    def clearModification(self, p):
        """
        Remove parameter from modification list.
        """
        if p in self._modified_parameters:
            del self._modified_parameters[p]

    def addParameterDefinition(self, p, v):
        """
        Add the defintion dict of one parameter to the overall dict containing parameter definitions.
        """
        self.parameter_properties[p] = self._default_parameter_definition.copy()
        self.parameter_properties[p].update(v)  # then copy the ones explicitly provided

    def delx(self, name):
        """
        Delete a parameter from the workspace. If the parameter was
        hardcoded before initialization (i.e., provided through
        ws.parameters) then the value will be reset but the parameter
        remains and will be filled with its default value at the next
        retrieval. Otherwise the parameter is completely removed.
        """
        if name in self.parameterlist:
            if hasattr(self, "_" + name):
                delattr(self, "_" + name)
                if "_" + name in self._parameterlist:
                    self._parameterlist.remove("_" + name)
            if name in self.parameters:  # OK that is a pre-defined parameter
                self.parameter_properties[name] = self._default_parameter_definition.copy()
                self.parameter_properties[name].update(self.parameters[name])  # restore the properties with original properties
                if "default" in self.parameter_properties[name] and isinstance(self.parameter_properties[name]["default"], types.FunctionType):  # this is a function
                    self.parameter_properties[name]["dependencies"] = self.parameterlist.intersection(self.parameter_properties[name]["default"].func_code.co_names)  # reset dependencies
            else:  # parameter was added later, thus will be removed completely
                if name in self.parameter_properties:
                    del self.parameter_properties[name]
            self._modified_parameters.add(name)
        else:  # not in parameterlist
            if hasattr(self, name):  # was set explicitly as normal method
                delattr(self, name)
            else:  # does not exist
                print "Error WorkSpace: Did not delete parameter",

    def addProperty(self, name, *funcs):
        """
        Add a python "property" to the class which contains getter and setter functions for methods.

        Example:

        self.addProperty(name,lambda ws:ws[name],lambda ws,x:ws.__setitem__(name,x),lambda ws:ws.delx(name),"This is parameter "+name)
        """
        setattr(self.__class__, name, property(*funcs))  # property(getx, setx, delx, "I'm the property.")

    def partuple_to_pardict(self, tup):
        """
        Converts a tuple of parameter description values into a
        properly formatted dict. If the tuple is shorter than default
        values are used.

        Example: partuple_to_pardict(self,(value,"Parameter description","MHz")) -> {"default":value,"doc":"Parameter description","unit":"MHz"}
        """
        pardict = self._default_parameter_definition.copy()
        for i in range(len(tup)):
            pardict[self._default_parameter_order[i]] = tup[i]
        return pardict

    def addParameters(self, parlist):
        """
        This provides an easy interface to add a number of parameters,
        either as a list or as a dict with properties.

        ::

          >>> ws.addParameters(["par1","par2",...])

        will simply add the parameters ``parN`` without documentation
        and default values

        ::

          ws.addParameters([("par1",val1, doc1, unit1),(,"par2",...),...])

        will add the parameters ``parN`` with the respective
        properties. The properties are assigned based on their
        position in the tuple:

            ===== ================
            pos 0 parmeter name
            pos 1 default value
            pos 2 doc string
            pos 3 unit of values
            ===== ================

        ::

          >>> ws.addParameters({"par1":{"default":val1,"doc":doc1,"unit":unit1},"par2":{...},...})

        will add the parameters ``parN`` with the respective parameters.
        """
        if isinstance(parlist, dict):
            for p, v in parlist.items():
                self.add(p, **v)
        elif isinstance(parlist, list):
            for p in parlist:
                if isinstance(p, tuple):
                    self.add(p[0], **(self.partuple_to_pardict(p[1:])))
                else:
                    self.add(p)

    def add(self, par, **properties):
        """
        Add a new parameter to the workspace, providing additional
        information, such as documentation and default values. The
        named parameters describe properties of the parameters. If no
        named parameters are given default values are used and added.

        Example:
        ws.add(par,default=0,doc="Parameter documentation",unit="km/h")

        The default values can also be a function, which takes as
        argument the workspace itself,e.g.

        ws.add(par,default=lambda ws:ws["other_parameter"]+1,doc="This parameter is the value of another parameter plus one",unit="km/h")

        If another parameter is referenced it will be retrieved
        automatically, and set to a default value if necessary. This
        way one can recursively go through multiple parameters upon
        retrieval.
        """
        self._known_methods.add(par)
        self._known_methods.add("_" + par)
        self.parameterlist.add(par)
        self._parameterlist.add("_" + par)
        self.addProperty(par, lambda ws: ws[par], lambda ws, x: ws.__setitem__(par, x), lambda ws: ws.delx(par), self.getParameterDoc(par))
        if "positional" in properties and properties["positional"]:
            self._positionals.append(par)
        if "default" in properties and isinstance(properties["default"], types.FunctionType):  # this is a function
            properties["dependencies"] = self.parameterlist.intersection(properties["default"].func_code.co_names)  # check the variables it depends on
        self.addParameterDefinition(par, properties)

    def getDerivedParameters(self, workarrays=True, nonexport=True):
        """
        Return a python set which contains the parameters that are
        derived from input parameters through a default function at
        initialization. These are the parameters which were defined
        before initialization in ws.parameters and which do have a
        function as default value. Note, that the value is not
        recalculated again even if the input parameters changed! One
        can enforce recalculation by calling ws.recalc().

        *workarrays* = True - Include workarrays in the list
        *nonexport* = True - Include parameters which were not meant for export in the list
        """
        derivedparameters = set()
        for p in self.parameterlist:
            properties = self.parameter_properties[p]
            if (("default" in properties and (isinstance(properties["default"], types.FunctionType)))  # default is a function
                and (not nonexport or not ("export" in properties and not properties["export"]))  # export is true
                and (not ("workarray" in properties and properties["workarray"]) or workarrays)):  # not a workarray if requested
                derivedparameters.add(p)  # then it is a derived parameter we want to see
        return derivedparameters

    def getOutputParameters(self, workarrays=False, nonexport=True):
        """
        Return all parameters that are considered output parameters,
        i.e., those which are 'derived' parameters and those explicitly
        labelled as output.
        """
        l = set(self.getDerivedParameters(workarrays=workarrays, nonexport=nonexport))
        for p, v in self.parameter_properties.items():
            if ("output" in v and v["output"]):
                if ((workarrays or not ("workarray" in v and v["workarray"]))
                    and (not nonexport or not ("export" in v and not v["export"]))):
                    l.add(p)
        return l

# """
# p="quality"
# v=self.parameter_properties[p]
# workarray="workarray"
# default="default"
# export="export"
# import types
# derivedparameters=set()
# self=Task.ws
# nonexport=True
# """

    def getPositionalParameters(self):
        """
        Return all parameters that are used as postional parameters,
        i.e. those which don't have a default value or a keyword.
        """
        l1 = {}
        for p, v in self.parameter_properties.items():
            if ("positional" in v and v["positional"]):
                l1[p] = v["positional"]
        l2 = range(len(l1))
        for p, v in l1.items():
            l2[v - 1] = p
        return l2

    def getInputParameters(self):
        """
        Return a python set which contains the parameters that are
        considered input parameters. This are those parameters which
        were defined before initialization in ws.parameters and which
        do not have a function as default value.
        """
        inputparameters = set()
        for p in self.parameterlist:
            properties = self.parameter_properties[p]
            if (("default" in properties and (not isinstance(properties["default"], types.FunctionType)))  # is not a function
            and (("export" not in properties) or properties["export"])  # export is true
            and (("workarray" not in properties) or (not properties["workarray"]))  # not a workarray
            and (("output" not in properties) or (not properties["output"]))):  # not explicitly defined as output
                inputparameters.add(p)  # then it is an input parameter
        return inputparameters

    def getInputParametersDict(self):
        """
        Returns the input parameters as a dict that can be provided
        at startup to the function to restore the parameters.
        """
        dct = {}
        for p in self.getInputParameters():
            dct[p] = self[p]
        return dct

    def getInternalParameters(self):
        """
        Return a list that contains all method names that simply contain
        a value, but were not assigned through self.add(), i.e. which
        do not have a getter and setter function or any
        description. These are typically inetranl variables that are
        not well documented.
        """
        return set(dir(self)).difference(self._known_methods)

    def listInternalParameters(self):
        """
        Return a string that contains all methods that simply contain
        a value, but were not assigned through self.add(), i.e. which
        do not have a getter and setter function or any
        description. These are typically inetranl variables that are
        not well documented.
        """
        s = ""
        for p in self.getInternalParameters():
            if hasattr(self, p):
                val = getattr(self, p)
            else:
                val = "'UNDEFINED'"
            if version_info > (2, 5, 9):
                s += "# {0:>20} = {1!r:20} \n".format(p, val)
            else:
                s += str((p, val))
        return s

    def printall(self):
        """
        ws.printall()

        Print all parameters stored in the workspace including internal parameters.
        """
        print self.__repr__(True)

    def evalAll(self):
        """
        Evaluates all parameters and assigns them their default values if they are as yet undefined.
        """
        for p in self.parameterlist:
            self[p]

    def evalParameters(self):
        """
        Evaluates all input and output parameters and assigns them their default values if they are as yet undefined.
        """
        for p in self.getInputParameters().union(self.getOutputParameters()):
            self[p]

    def evalInputParameters(self):
        """
        Evaluates all input parameters and assigns them their default values if they are as yet undefined.
        """
        for p in self.getInputParameters():
            self[p]

    def isModified(self, par, firstcall=True):
        """
        Returns true or false whether a parameter was modified since
        the last update or recalc. The function will also add the
        parmameter to the modified_parameters list if it was modified.
        """
#        if hasattr(self,"trace") and self.trace:
#            import pdb; pdb.set_trace()
        if firstcall:
            self.checkedalready = set()
        if par in self._modified_parameters:
            return True
        if par in self.checkedalready:
            return False  # to avoid infinite loops
        self.checkedalready.add(par)
        if "dependencies" in self.parameter_properties[par] and len(self.parameter_properties[par]["dependencies"]) > 0:
#            print "Modified parameter -> ",par
            modified = reduce(lambda a, b: a | b, map(lambda p: self.isModified(p, firstcall=False), self.parameter_properties[par]["dependencies"]))
            if modified:
                self._modified_parameters.add(par)
            return modified
        return False

    def update(self, forced=False, workarrays=True, nonexport=False):
        """
        Recalculates all existing derived parameters and assigns them
        their default values if they depend on a value that was
        modified. Note that parameters which had a default function at
        initialization but were set explicitly will not be
        recalculated. Use ws.reset() or del ws.par first.

        *forced* = False - If True, then update all parameters
         irrespective of whether they depend on modified parameters or
         not.

         *workarrays* = True - also update work arrays, set to False
          if you want to avoid reinitializing them due to an update

        """
        pars = []
        for p in self.getDerivedParameters(workarrays=workarrays, nonexport=nonexport):  # first make sure all modified parameters are identified
            if (self.isModified(p) or forced) and hasattr(self, "_" + p) and (isinstance(self.parameter_properties[p]["default"], types.FunctionType)) and (workarrays or (("workarray" not in self.parameter_properties[p]) or not self.parameter_properties[p]["workarray"])):
                delattr(self, "_" + p)  # delete buffered value so that it will be recalculated
                pars.append(p)
        for p in pars:
            self[p]  # recalculate the parameters where the local value was deleted
        self.clearModifications()

    def updateParameter(self, p, forced=False):
        """
        Recalculates the named parameter and assign it
        the default values if it depends on a value that was
        modified. Note that parameters which had a default function at
        initialization but were set explicitly will not be
        recalculated. Use ws.reset() or del ws.par first.

        *forced* = False - If True, then update all parameters
         irrespective of whether they depend on modified parameters or
         not.

        """
        if ((p in self.getDerivedParameters()) and (self.isModified(p) or forced) and hasattr(self, "_" + p) and (isinstance(self.parameter_properties[p]["default"], types.FunctionType))):
            delattr(self, "_" + p)  # delete buffered value so that it will be recalculated
            self[p]  # recalculate the parameters where the local value was deleted
            self.clearModification(p)

    def getParameterDoc(self, name):
        """
        If parameter was defined in parameter_properties return the "doc" keyword, otherwise a default string.
        """
        if name in self.parameter_properties and "doc" in self.parameter_properties[name]:
            return self.parameter_properties[name]["doc"]
        else:
            return "This is parameter " + name + "."

    def getParameters(self, internals=False, excludeworkarrays=True, excludenonexports=True, all=False):
        """
        ws.getParameters(internals=False,excludeworkarrays=True,excludenonexports=True,all=False) -> {"par1":value1, "par2":value2,...}

        Returns a python dictionary containing all the parameters and their values as key/value pairs.

        *internals* = False - If True all stored parameters are returned,
        including those not added by ws.add and which are typically
        only used for internal purposes.

        *excludeworkarrays* = True - whether or not to exclude the data arary
        *excludenonexports* = True - whether or not to exclude parameters that are marked to not be printed
        *all* = False - really return all parameters (internals, workarrays, excludes)
        """
        pdict = {}
        for p in self.getParameterNames(internals, excludeworkarrays, excludenonexports, all):
            pdict[p] = self[p]
        return pdict

    def getParameterNames(self, internals=False, excludeworkarrays=True, excludenonexports=True, all=False):
        """
        ws.getParameterNames(internals=False,excludeworkarrays=True,excludenonexports=True,all=False) -> ["par1", "par2",...]

        Returns a python list containing all the parameter names

        *internals* = False - If True all stored parameters are returned,
        including those not added by ws.add and which are typically
        only used for internal purposes.

        *excludeworkarrays* = True - whether or not to exclude the data arary

        *excludenonexports* = True - whether or not to exclude parameters that are marked to not be printed

        *all* = False - really return all parameters (internals, workarrays, excludes)
        """
        if all:
            internals = True
            excludeworkarrays = False
            excludenonexports = False
        plist = []
        for p in self.parameterlist:
            if ((excludenonexports and "export" in self.parameter_properties[p] and (not self.parameter_properties[p]["export"])) or
                (excludeworkarrays and "workarray" in self.parameter_properties[p] and self.parameter_properties[p]["workarray"])):
                pass  # do not return parameter since it is a work array or is explicitly excluded
            else:
                plist.append(p)
        if internals:
            for p in self.getInternalParameters():
                plist.append(p)
        return plist

    def __repr__(self, internals=False, workarrays=True, noninputparameters=True):
        """
        String representation of a work space, listing all explicitly defined parameters.

        *internals* With internals=True also those parameters will be
         printed which were not properly defined in the
         parameter_properties or added with .add but simply assigned
         by ws.par=value.
        """
#        variables=set(Task.run.func_code.co_names)
        s = "#-------------------------------------------------------------------------------------------------------------------------\n# WorkSpace of " + self.__taskname__ + "(" + ",".join(self.getPositionalParameters()) + ")\n#-------------------------------------------------------------------------------------------------------------------------\n"
        s0 = ""
        s1 = ""
        s2 = ""
        pars = sorted(self.parameter_properties.items())
        for p, v in pars:
            if hasattr(self, "_" + p):
                val = getattr(self, "_" + p)
            else:
                val = "'UNDEFINED'"
            if type(val) in cr.hAllContainerTypes:
                val = val.__repr__(8)
            if ("positional" in v) and (v["positional"]):
                s += "# {0:s} = {1!r} - {2:s}".format(p, val, printindent_string(v["doc"], 66, width=120, prefix="# "))
            if ("export" in v) and (not v["export"]):
                continue
            if p in self.getInputParameters():
                if (v["unit"] == ""):
                    s0 += "{0:<22} = {1!r:30} #         {2:s}".format(p, val, printindent_string(v["doc"], 66, width=120, prefix="# "))
                else:
                    s0 += "{0:<22} = {1!r:30} # [{2:^5s}] {3:s}".format(p, val, v["unit"], printindent_string(v["doc"], 66, width=120, prefix="# "))
            elif ("workarray" in v) and (v["workarray"]):
                if workarrays:
                    if "dependencies" in v:
                        deps = " <- [" + ", ".join(v["dependencies"]) + "]"
                    else:
                        deps = ""
                    s2 += ("# {2:s}\n# {0:s} = {1!r}" + deps).format(p, val, printindent_string(v["doc"], 66, width=120, prefix="# "))
            elif noninputparameters:
                if "dependencies" in v:
                    deps = " <- [" + ", ".join(v["dependencies"]) + "]"
                else:
                    deps = ""
                if (v["unit"] == ""):
                    s1 += ("# {0:>20} = {1!r:30} - {2:s}").format(p, val, printindent_string(v["doc"] + deps, 58, width=120, prefix="# "))
                else:
                    s1 += ("# {0:>20} = {1:<30} - {2:s}").format(p, str(val) + " " + v["unit"], printindent_string(v["doc"] + deps, 58, width=120, prefix="# "))
        s += s0
        if not s1 == "":
            s += "#------------------------Output Parameters------------------------------\n" + s1
        if not s2 == "":
            s += "#---------------------------Work Arrays---------------------------------\n" + s2
        if internals:
            s += "#-----------------------Internal Parameters-----------------------------\n" + self.listInternalParameters()
        s += "#-----------------------------------------------------------------------\n"
        return s


def __raiseTaskDeprecationWarning(taskname=""):
    import warnings
    warnings.simplefilter("always")
    warnings.warn("""
    **************************************************************************
                             DEPRECATION WARNING
    **************************************************************************

    The Task {0} is marked as deprecated:

    In case this module is still actively used and/or developed,
    notify the developer/maintainer of this module.

    The developer/maintainer of this module should do the following:

    - Add the name and email address of the developer/maintainer of this
      module at the top of the documentation of this file, e.g.:

      .. moduleauthor:: Name of the module author <email address of the module author>

    - Check the documentation

    - Remove this DeprecationWarning from the module {0},
      e.g. the line containing:

      cr.tasks.__raiseTaskDeprecationWarning(__name__)

    """.format(taskname), DeprecationWarning)

