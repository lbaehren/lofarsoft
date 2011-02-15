"""Tasks

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

import os
import shelve
import types
import time
from pycrtools.core import config
from pycrtools import *

from tshortcuts import *

import pdb
#pdb.set_trace()

#import averagespectrum
#["averagespectrum","imager"]

#__all__ = ['Task', 'TaskLauncher', 'reset']

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
	if not name == "Task":
	    config.loaded_tasks[name]=cls.__module__
        super(TaskInit, cls).__init__(name, bases, dct)

    def __call__(cls, *args, **kwargs):
        """ Create a new instance.
        """

        # Create instance with restored arguments
        obj = type.__call__(cls, *args, **kwargs)

        return obj

class Task(object):
    """Base class from which all tasks should be derived.
    """
    __metaclass__ = TaskInit

    def __init__(self,ws=None,parfile=None,**args):

	self.__modulename__=self.__module__.split(".")[-1]
	self.__taskname__=self.__class__.__taskname__
	config.task_name=self.__taskname__
	config.task_instance=self
	
	margs=args.copy() #Reading parameters from a file
	if type(parfile)==str:
	    if os.path.exists(parfile):
		fargs={}
		f=open(parfile)
		exec f in fargs
		del fargs["__builtins__"]
		fargs.update(margs)
		margs=fargs.copy()
	    else:
		print "ERROR Task: Parameter file",parfile,"does not exist."

	if not hasattr(self,"WorkSpace"): #Create a Default WorkSpace class if it does not exist yet
	    if hasattr(self,"parameters"):
		property_dict=self.parameters
	    else:
		property_dict={}
	    if hasattr(self,"call"): # retrieve the parameters from the parameters of the callfunction
		n_named_pars=len(self.call.im_func.func_defaults) # number of named parameters which have defaults 
		n_positional_pars=self.call.im_func.func_code.co_argcount-n_named_pars
		named_pars=self.call.im_func.func_code.co_varnames[n_positional_pars:]
		positional_pars=self.call.im_func.func_code.co_varnames[1:n_positional_pars] # start at 1 to exclude "self" argument
		for p in positional_pars:
		    if property_dict.has_key(p):
			property_dict[p].update({default:None,export:False})
		    else:
			property_dict[p]={default:None,export:False}
		for p,v in zip(named_pars,self.call.im_func.func_defaults):
		    if property_dict.has_key(p):
			property_dict[p].update({default:v})
		    else:
			property_dict[p]={default:v}
	    self.WorkSpace=WorkSpace(self.__taskname__,parameters=property_dict) # This creates the work space class that is used to create the actual ws instance

        if ws==None: self.ws=self.WorkSpace(**margs) #create default workspace of this task
        else: self.ws=ws(**margs) # or take the one which was provided already

	#Add setter and getter functions for parameters to task class
	pp=set(self.ws.getParameterNames(self,all=True)).difference(set(dir(self)))
	for p in pp:
	    self.addProperty(p,eval("lambda slf:slf.ws['"+p+"']"),eval("lambda slf,x:slf.ws.__setitem__('"+p+"',x)"),eval("lambda slf:slf.ws.delx('"+p+"')"),self.ws.getParameterDoc(p))

	self._starttime=time.strftime("%Y-%m-%d_%H:%M:%S")
	self.oparfile=self.__taskname__+"_"+self._starttime+".par" 
        self.ws.t0=time.clock()

	self.ws.evalInputParameters()
	if hasattr(self,"init"): self.init()

	#Add those parameters that were added during init phase
	pp=set(self.ws.getParameterNames(self,all=True)).difference(set(dir(self)))
	for p in pp:
	    self.addProperty(p,eval("lambda slf:slf.ws['"+p+"']"),eval("lambda slf,x:slf.ws.__setitem__('"+p+"',x)"),eval("lambda slf:slf.ws.delx('"+p+"')"),self.ws.getParameterDoc(p))

    def saveOutputFile(self):
	"""
	Save the parameters to a file that can be read back later with
	the option parfile=filename (e.g., tpar parfile=filename)
	"""
	f=open(self.oparfile,"w")
	f.write("# Task: "+self.__modulename__+" saved on "+time.strftime("%Y-%m-%d %H:%M:%S")+"\n# File: "+self.oparfile+"\n")
	f.write(self.ws.__repr__(internals=False,workarrays=False))
	f.close()

    def addProperty(self, name, *funcs):
	"""
	Add a python "property" to the class which contains getter and setter functions for methods.

	Example:
	
	self.addProperty(name,lambda self:self[name],lambda self,x:self.__setitem__(name,x),lambda self:self.delx(name),"This is parameter "+name)
	"""
        setattr(self.__class__, name, property(*funcs)) #property(getx, setx, delx, "I'm the property.")

    def __call__(self,parfile=None,ws=None,**kwargs):
	"""
	task.run() -> rerun the task with currently set parameters
	task.run(par1=val1,par2=val2) -> rerun the task with the parameters parN set to the values provided and keeping the previous parameters the same
	"""

	self._starttime=time.strftime("%Y-%m-%d_%H:%M:%S")
	self.oparfile=self.__taskname__+"_"+self._starttime+".par" 

	if not ws==None: self.ws=ws
	self.ws(parfile=parfile,**kwargs)
#	if not parfile == None: self.ws(parfile=parfile)
#	if len(kwargs)>0: self.ws(**kwargs) # set parameters in workspace
	self.ws.update() # make sure all parameters are up-to-date
	self.saveOutputFile()
	self.run()
	self.saveOutputFile() # to store final values


    def put(self):
	"""
	Stores the input parameters in the workspace to the parameter
	database (see also 'tput').  This can be restored with
	task.get() (or 'tget' from the command line).
	"""
	# # Open task database
	taskdb = shelve.open(dbfile)

        taskdb[self.__taskname__] = self.ws.getInputParametersDict()

        taskdb.close()

    def get(self):
	"""
	Gets the input parameters in the workspace from the parameter
	database (see also 'tget').  This can be stored there with
	task.put() (or 'tput' from the command line).
	"""
	# # Open task database
	taskdb = shelve.open(dbfile)

        if self.__taskname__ in taskdb:
             # Restoring from database
             args = taskdb[self.__taskname__]
	     self.ws(**args)

        taskdb.close()

    def reset(self,restorecallparameters=False,**args):
	"""
	Usage:

	task.reset() -> Reset all parameter to the default state and rerun the initialization.

	task.reset(par1=val1,par2=val2,...) -> reset all parameters to
	default and initialze with parameters provided.
	
	task.reset(restorecallparameters=True,par1=val1,par2=val2,...)
	-> reset all parameters to their state at initialization, but
	keep the parameters provided during the last initialisation.
	"""
	self.ws.reset(restorecallparameters=restorecallparameters)
	self.ws.__init__(**args)
	self.init()
	
#########################################################################
#                             Workspaces
#########################################################################
class WorkSpaceType(type):
#    def __init__(cls, name, bases, dct):
#	""" Create a new class.
#        """
#	print "Initializing cls=",cls
#	print "Name=",name
#	print "Bases=",bases
#	print "dct=",dct
#	cls.name=name
#	cls.bases=bases
#	cls.dct=dct
#	
#        cls.__taskname__ = name
#        super(WorkSpaceType, cls).__init__(name, bases, dct)

    def __call__(cls, taskname='Task',parameters={},**kwargs):
        """ Create a new instance.
        """
#	print "Calling WorkSpaceClass"

#	pdb.set_trace()
	
        # Create instance with restored arguments
#        obj=type(taskname+cls.name,cls.bases,cls.dct)
        obj=type(taskname+cls.__name__,cls.__bases__,cls.__dict__.copy())
	
	for k,v in kwargs.items():
	    parameters[k]={default:v}
	obj.parameters = parameters
	obj.__taskname__=taskname
        return obj

"""
wsc=WorkSpace("MyTask",x={default:1},y={default:2})
wsc=WorkSpace("MyTask",x=1,y=2)
ws=wsc(x=3)
"""

class WorkSpace(object):
    """
    This class holds all parameters, scratch arrays and vectors used
    by the various tasks. Hence this is the basic workspace in the
    memory.
    """
    __metaclass__ = WorkSpaceType

    def __init__(self,**args):
#	pdb.set_trace()
	if not hasattr(self,"parameters"): self.parameters={}
	self.parameter_properties=self.parameters.copy()
        self.parameterlist=set(self.parameter_properties.keys())
	self._initparameters=args.copy()
        self._parameterlist=set()
	self._modified_parameters=set()
        self._default_parameter_definition={doc:"", unit:"", default:None, workarray:False, export:True}
        self._default_parameter_order=(default,doc,unit)
        self._known_methods=set()
        self._known_methods.update(set(dir(self)))
        self.addParameters(self.parameter_properties)
	if len(args)>0: self(**args) # assign parameter values from the parameter list
    
    def reset(self,restorecallparameters=False):
	"""
	ws.reset() -> reset all parameters to their state at initialization
	
	ws.reset(restorecallparameters=True) -> reset all parameters
	to their state at initialization, but keep the parameters
	provided during initialisation.
	"""
	for p in self._parameterlist: # Delete all actually stored values
	    if hasattr(self,p): delattr(self,p)
	if restorecallparameters and len(self._initparameters)>0:
	    self(**(self._initparameters)) # assign parameter values from the parameter list
	self.clearModifications()
    def __call__(self,parfile=None,**args):
	"""
	Usage:

	ws(par1=val1, par2=val2,...)

	The call function lets one assign and/or update known
	parameters simply by calling the workspace with the parameters
	as arguments.
	"""

	margs=args.copy() #Reading parameters from a file
	if type(parfile)==str:
	    if os.path.exists(parfile):
		fargs={}
		f=open(parfile)
		exec f in fargs
		del fargs["__builtins__"]
		fargs.update(margs)
		margs=fargs.copy()
	    else:
		print "ERROR Task: Parameter file",parfile,"does not exist."

	for k,v in margs.items():
	    if k in self.parameterlist:
		self[k]=v
	    else: print "Warning ws.__call__: Parameter ",k,"not known."
	return self
    def __getitem__(self,par):
	"""
	Usage:

	ws["parname"] -> Return parameter value

	Access the parameter value using square brackets and a string
	of the parametername. The basic "getter" function for parameters.	
	"""
        if hasattr(self,"_"+par):   # Return locally stored value
            return getattr(self,"_"+par) 
        elif self.parameter_properties.has_key(par):
            if self.parameter_properties[par].has_key(default):   #return default value or function
                f_or_val=self.parameter_properties[par][default]
                if type(f_or_val)==types.FunctionType:
		    setattr(self,"_"+par,f_or_val(self))
                else:
		    setattr(self,"_"+par,f_or_val)
            else:
                print "ERROR in Workspace",self.__module__,": Parameter ", par,"does not have default values!"
            return getattr(self,"_"+par)
        elif hasattr(self,par):   # Return locally stored value
            return getattr(self,par)
	else:
            print "ERROR in Workspace",self.__module__,": Parameter ", par,"not known!"
            return None
    def __setitem__(self,par,value):
	"""
	Usage:

	ws["parname"]=value 

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
	if hasattr(self,"trace") and self.trace: pdb.set_trace()
        if hasattr(self,"_"+par) or par in self.parameterlist:   #replace stored value
	    if hasattr(self,"_"+par):
		if getattr(self,"_"+par)==value: return  # don't assign or considered modified if it is the same value
		delattr(self,"_"+par) # Delete first in case it contains a large array which blocks memory
            setattr(self,"_"+par,value)
	    self.parameter_properties[par][default]=value
        else:
            setattr(self,"_"+par,value) # create new parameter with default parameters
            self.parameter_properties[par]=self._default_parameter_definition
            self.add(par)
	self._modified_parameters.add(par)
    def clearModifications(self):
	"""
	Set all parameters to be unmodified.
	"""
	self._modified_parameters=set([])
    def addParameterDefinition(self,p,v):
	"""
	Add the defintion dict of one parameter to the overall dict containing parameter definitions.
	"""
        self.parameter_properties[p]=self._default_parameter_definition.copy()
        self.parameter_properties[p].update(v) # then copy the ones explicitly provided 
    def delx(self,name):
	"""
	Delete a paramter from the workspace. If the parameter was
	hardcoded before initialization (i.e., provided through
	ws.parameters) then the value will be reset but the parameter
	remains and will be filled with its default value at the next
	retrieval. Othrerwise the parameter is completely removed.
	"""
	if name in self.parameterlist:
	    if hasattr(self,"_"+name):
		delattr(self,"_"+name)
		self._parameterlist.remove("_"+name)
	    if self.parameters.has_key(name): #OK that is a pre-defined parameter
		self.parameter_properties[name]=self._default_parameter_definition.copy()
		self.parameter_properties[name].update(self.parameters[name]) # restore the properties with original properties
		if self.parameter_properties[name].has_key(default) and type(self.parameter_properties[name][default])==types.FunctionType: # this is a function
		    self.parameter_properties[name][dependencies]=self.parameterlist.intersection(self.parameter_properties[name][default].func_code.co_names) #reset dependencies
	    else: # parameter was added later, thus will be removed completely
		if self.parameter_properties.has_key(name):
		    del self.parameter_properties[name]
	    self._modified_parameters.add(name)
	else: # not in parameterlist
	    if hasattr(self,name): # was set explicitly as normal method
		delattr(self,name)
	    else: #does not exist
		print "Error WorkSpace: Did not delete parameter",
    def addProperty(self, name, *funcs):
	"""
	Add a python "property" to the class which contains getter and setter functions for methods.

	Example:
	
	self.addProperty(name,lambda ws:ws[name],lambda ws,x:ws.__setitem__(name,x),lambda ws:ws.delx(name),"This is parameter "+name)
	"""
        setattr(self.__class__, name, property(*funcs)) #property(getx, setx, delx, "I'm the property.")
    def partuple_to_pardict(self,tup):
        """
	Converts a tuple of parameter description values into a
        properly formatted dict. If the tuple is shorter than default
        values are used.

        Example: partuple_to_pardict(self,(value,"Parameter description","MHz")) -> {"default":value,"doc":"Parameter description","unit":"MHz"}
        """
        pardict=self._default_parameter_definition.copy()
        for i in range(len(tup)): pardict[self._default_parameter_order[i]]=tup[i]
        return pardict
    def addParameters(self,parlist):
        """
        This provides an easy interface to add a number of parameters, either as a list or as a dict with properties.

        ws.addParameters(["par1","par2",...]) will simply add the parameters parN without documentation and default values

        ws.addParameters([("par1",val1, doc1, unit1),(,"par2",...),...]) will add the parameters parN with the respective
        properties. The properties are assigned based on their position in the tuple:
            pos 0 = parmeter name
            pos 1 = default value
            pos 2 = doc string
            pos 3 = unit of values

        ws.addParameters({"par1":{"default":val1,"doc":doc1,"unit":unit1},"par2":{...},...}) will add the parameters parN with the respective
        parameters.
        """
        if type(parlist)==dict:
            for p,v in parlist.items():
                self.add(p,**v)
        elif type(parlist)==list:
            for p in parlist:
                if type(p)==tuple:
                    self.add(p[0],**(self.partuple_to_pardict(p[1:])))
                else:
                    self.add(p)
    def add(self,name,**properties):
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
        self._known_methods.add(name)
        self._known_methods.add("_"+name)
        self.parameterlist.add(name)
        self._parameterlist.add("_"+name)
        self.addProperty(name,lambda ws:ws[name],lambda ws,x:ws.__setitem__(name,x),lambda ws:ws.delx(name),self.getParameterDoc(name))
	if properties.has_key(default) and type(properties[default])==types.FunctionType: # this is a function
	    properties[dependencies]=self.parameterlist.intersection(properties["default"].func_code.co_names)
        self.addParameterDefinition(name,properties)
    def getDerivedParameters(self,workarrays=True):
	"""
	Return a python set which contains the parameters that are
	derived from input parameters through a default function at
	initialization. This are those parameters which were defined
	before initialization in ws.parameters and which do have a
	function as default value. Note, that the value is not
	recalculated again even if the input parameters changed! One
	can enforce recalculation by calling ws.recalc().
	"""
	derivedparameters=set()
	for p in self.parameterlist:
	    properties=self.parameter_properties[p] 
	    if ((properties.has_key(default) and (type(properties[default])==types.FunctionType)) # default is a function
		and ((not properties.has_key(export)) or properties[export])  #export is true
		or (workarrays and (properties.has_key(workarray) and properties[workarray]))): # not a workarray if requested
		derivedparameters.add(p) # then it is a derived parameter
	return derivedparameters
    def getOutputParameters(self):
	"""
	Return all paramters that are considered output parameters,
	i.e. those which are 'derived' parameters and those explicitly
	labelled as output.
	"""
	l=set(self.getDerivedParameters(workarrays=False))
	for p,v in self.parameter_properties.items():
	    if (v.has_key(output) and v[output]): l.add(p)
	return l
    def getInputParameters(self):
	"""
	Return a python set which contains the parameters that are
	considered input parameters. This are those parameters which
	were defined before initialization in ws.parameters and which
	do not have a function as default value.
	"""
	inputparameters=set()
	for p in self.parameterlist:
	    properties=self.parameter_properties[p] 
	    if ((properties.has_key(default) and (not type(properties[default])==types.FunctionType)) # is not a function
	    and ((not properties.has_key(export)) or properties[export]) #export is true
	    and ((not properties.has_key(workarray)) or (not properties[workarray])) #not a workarray
	    and ((not properties.has_key(output)) or (not properties[output]))): #not explicitly defined as output
		inputparameters.add(p) # then it is an input parameter
	return inputparameters
    def getInputParametersDict(self):
	"""
	Returns the input parameters as a dict that can be provided
	at startup to the function to restore the parameters.
	"""
	dct={}
	for p in self.getInputParameters():
	    dct[p]=self[p]
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
        s=""
        for p in self.getInternalParameters():
            if hasattr(self,p): val=getattr(self,p)
            else: val="'UNDEFINED'"
            s+="# {0:>20} = {1!r:20} \n".format(p,val) 
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
        for p in self.parameterlist: self[p]
    def evalParameters(self):
        """
        Evaluates all input and output parameters and assigns them their default values if they are as yet undefined.
        """
        for p in self.getInputParameters().union(self.getOutputParameters()): self[p]
    def evalInputParameters(self):
        """
        Evaluates all input parameters and assigns them their default values if they are as yet undefined.
        """
        for p in self.getInputParameters(): self[p]
    def isModified(self,par):
	"""
	Returns true or false whether a parameter was modified since
	the last update or recalc. The function will also add the
	parmameter to the modified_parameters list if it was modified.
	"""
	if hasattr(self,"trace") and self.trace: pdb.set_trace()
	if par in self._modified_parameters: return True
	if self.parameter_properties[par].has_key(dependencies) and len(self.parameter_properties[par][dependencies])>0:
	    modified=reduce(lambda a,b:a | b,map(lambda p:self.isModified(p),self.parameter_properties[par][dependencies]))
	    if modified: self._modified_parameters.add(par)
	    return modified
	return False
    def update(self,forced=False):
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
	pars=[]
	for p in self.getDerivedParameters(): # first make sure all modified parameters are identified 
	    if (self.isModified(p) or forced) and hasattr(self,"_"+p):
		delattr(self,"_"+p) # delete buffered value so that it will be recalculated
		pars.append(p)
	for p in pars:
	    self[p] # recalculate the paramters where the local value was deleted
	self.clearModifications()
    def getParameterDoc(self,name):
	"""
	If parameter was defined in parameter_properties return the "doc" keyword, otherwise a default string.
	"""
	if self.parameter_properties.has_key(name) and self.parameter_properties[name].has_key(doc):
	    return self.parameter_properties[name][doc]
	else:
	    return "This is parameter "+name+"."
    def getParameters(self,internals=False,excludeworkarrays=True,excludenonexports=True,all=False):
        """
        ws.getParamters(internals=False,excludeworkarrays=True,excludenonexports=True,all=False) -> {"par1":value1, "par2":value2,...}

        Returns a python dictionary containing all the paramters and their values as key/value pairs.

        *internals* = False - If True all stored parameters are returned,
        including those not added by ws.add and which are typically
        only used for internal purposes.

	*excludeworkarrays* = True - whether or not to exclude the data arary
	*excludenonexports* = True - whether or not to exclude parameters that are marked to not be printed
	*all* = False - really return all parameters (internals, workarrays, excludes)
        """
	pdict={}
	for p in self.getParameterNames(internals,excludeworkarrays,excludenonexports,all):
	    pdict[p]=self[p]
        return pdict
    def getParameterNames(self,internals=False,excludeworkarrays=True,excludenonexports=True,all=False):
        """
        ws.getParamterNames(internals=False,excludeworkarrays=True,excludenonexports=True,all=False) -> ["par1", "par2",...]

        Returns a python list containing all the paramters names

        *internals* = False - If True all stored parameters are returned,
        including those not added by ws.add and which are typically
        only used for internal purposes.

	*excludeworkarrays* = True - whether or not to exclude the data arary

	*excludenonexports* = True - whether or not to exclude parameters that are marked to not be printed

	*all* = False - really return all parameters (internals, workarrays, excludes)
        """
	if all:
	    internals=True
	    excludeworkarrays=False
	    excludenonexports=False
        plist=[]
        for p in self.parameterlist:
            if ((excludenonexports and self.parameter_properties[p].has_key(export) and (not self.parameter_properties[p][export])) or
                (excludeworkarrays and self.parameter_properties[p].has_key(workarray) and self.parameter_properties[p][workarray])):
                pass # do not return parameter since it is a work array or is explicitly excluded
            else:
                plist.append(p)
        if internals:
            for p in self.getInternalParameters(): plist.append(p)
        return plist
    def __repr__(self,internals=False,workarrays=True,noninputparameters=True):
        """
        String representation of a work space, listing all explicitly defined parameters.

	*internals* With internals=True also those parameters will be
         printed which were not properly defined in the
         parameter_properties or added with .add but simply assigned
         by ws.par=value.
        """
        s="#-----------------------------------------------------------------------\n# WorkSpace of "+self.__taskname__+"\n#-----------------------------------------------------------------------\n"
	s1=""
        s2=""
	pars=self.parameter_properties.items()
	pars.sort()
	for p,v in pars:
	    if (v.has_key(export)) and (not v[export]):
		continue
            if hasattr(self,"_"+p): val=getattr(self,"_"+p)
            else: val="'UNDEFINED'"
	    if p in self.getInputParameters():
                if (v[unit]==""): s+="{0:<20} = {1!r:30} #         {2:s}\n".format(p,val,v[doc])
                else: s+="{0:<20} = {1!r:30} # [{2:^5s}] {3:s}\n".format(p,val,v[unit],v[doc]) 
	    elif (v.has_key(workarray)) and (v[workarray]):
		if workarrays: s2+="#{2:s}\n# {0:s} = {1!r}\n".format(p,val,v[doc])
            elif noninputparameters:
                if (v[unit]==""): s1+="# {0:>20} = {1!r:30} - {2:s}\n".format(p,val,v[doc])
                else: s1+="# {0:>20} = {1:<30} - {2:s}\n".format(p,str(val)+" "+v[unit],v[doc]) 
        if not s1=="": s+="#------------------------Output Parameters------------------------------\n"+s1
        if not s2=="": s+="#---------------------------Work Arrays---------------------------------\n"+s2
        if internals:  s+="#-----------------------Internal Parameters-----------------------------\n"+self.listInternalParameters()
        s += "#-----------------------------------------------------------------------\n"
        return s

def taskload2():
    """
    Used to import all tasks
    """
    import tasks.averagespectrum
    import tasks.imager

class taskloadClass():
    """
    Used to import all tasks
    """
    def __call__(self):
	print "Loading all available tasks"
	import pycrtools.tasks.averagespectrum
	import pycrtools.tasks.imager

    def __repr__(self):
	self.__call__()

taskload=taskloadClass()
