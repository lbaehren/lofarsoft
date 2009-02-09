pyoblist=[]

PUSH=DIR.TO
PULL=DIR.FROM

def hfERR(txt):
    print "Error:",txt

help={}
help["verbose"]="Use d.AllVerbose(True/False) to watch the execution of the network. The output is {Objectid}Name(data_vector[0])flag, where flag is one of the following: blank=no function, just data; ~=just a function, no data; *=function and data present and is modified; $=function and data present and is not modified."

def insert_object(self,l):
    if (type(l) in [tuple,list,DataParameterList]):
        l[0].insert(self,l[1])
        return (self,l[1])

def initializeObject(self,value):
    "This function initializes a newly created data object with values and parameters. value can be a tuple, list, or DataParameterList. The first value of the tuple is the new, which is not used for an already created object. _f(func) defines a function, _l(n) defines the netlevel of the object for displaying. Everything else is used to set the value of the object (e.g. a number or a string)."
    if (type(value) in [tuple,list,DataParameterList]):
        for elem in value[1:]:
            
            if hasattr(elem,"__hftype__"):
                if (elem.__hftype__()=="_f"):
                    self.setFunc_f_silent(elem)
                if (elem.__hftype__()=="_l"):
                    self.setNetLevel(elem.level)
            else:
                object_set_silent(self,elem)
    return self

def object_new(self,*arg):
    "Creates a new object an initializes ist (see .create() and .initializeObject())"
    tup=(arg)
    if (len(tup)>1): return self.create(tup[0]).initializeObject(tup)
    else: return self.create(tup[0])


def MakeChooser(d,name,fields,values):
    "Create a chooser, which is a collection of objects which act like a record of similar records, containgin a set of parameters among which you can choose in a menu. The first parameter is the name of the chooser object. The second parameter is a tuple of parameter names that form one set. The third parameter is a tuple of tuple containing the parameter values which belong to the respective parameters."
    chooser=d.new("Chooser",name,_l(200))
    for val in values:
        obj=(fields[0],val[0],_l(200)) >> chooser
        for i in range(1,len(fields)):
          obj=(fields[i],val[i],_l(200)) >> obj
    return chooser


class DataParameterList(list):
    "Container class to keep initialization parameters for creating new objects. Used by >> and << or ^ (i.e., insert). This can be created, e.g., by using _d(name,par,_f(func),_l(netlevel))."
    def __init__(self,*dlist):
        self.extend((dlist))
    def __xor__(self,value):   #
        obj=value[0].insertNew(self[0],value[1])
        obj.initializeObject(self)
        return obj;


def _d(*arg): 
    x=DataParameterList()
    x.extend(arg)
    return x

def NewObjectRange(name,n1,n2=None,inc=1):
    'Creates a list of the form [("Name",i1),("Name",i2),("Name",i3),...], where the ii are generated accordingto the input parameters n1,n2,inc which are the same parameters as used for range(). The result can be used to create multiple new objects, e.g. d >> NeObjectRange("Name",5)], to create multiple paths in the network. Name obviously is the name of the objects.'
    if (n2==None): r=range(n1)
    else: r=range(n1,n2,inc)
    return map(lambda n:(name,n), r) 

def object_id(self):
    "Function to return ID number of an object. Used for make_uniqe" 
    return self.getOid()

def toList(val):
    "Returns val as a List, i.e leave it unchanged if it is already a list, or, for a scalar, create a one-element list"
    if type(val)==list: return val
    else: return [val]


def make_unique(seq, idfun=None):  
    "Returns a list which only contains unique members"
    # order preserving 
    if idfun is None: 
        def idfun(x): return x 
    seen = {} 
    if type(seq)==DataList: result = DataList()
    else: result = [] 
    for item in seq: 
        marker = idfun(item) 
        # in old Python versions: 
        # if seen.has_key(marker) 
        # but in new ones: 
        if marker in seen: continue 
        seen[marker] = 1 
        result.append(item) 
    return result

def datalist_unique(self):
    l=make_unique(self,object_id)
    if len(l)==1: return l[0]
    else: return l

#Used for determining whether a search was successful If not, an
#DataList will be returned that returns true and 0 for these
#functions.
def object_notFound(self): return False;
def object_NFound(self): return 1;

class DataList(list):
    def datalist(self): return
    def __getitem__(self,idx):
        "If index is  a string then retrieve a list of data objects with that sting index from every data object in the list and create a new (flat) data list, otherwise return element idx"
        if type(idx)==str:  
            dl=DataList()
            for d in self:
                el=d[idx]
                if type(el)==DataList: dl.extend(el) ##make sure the list remains flat ...
                else: dl.append(el)
            return datalist_unique(dl)
        else:
            return list(self)[idx]
    def notFound(self):
        "Used for determining whether a search was successful. If not, an empty DataList will be returned that returns true  for this function."
        return len(self)==0
    def NFound(self):
        "Number of objects in the DataList. Same as len. Used for determining whether a search was successful. If not, an empty DataList will be returned that returns zero for this function. If one object is found an Object is returned."
        return len(self)
    def __floordiv__(self,other):
        for d in self: d // other
        return d
    def __rrshift__(self,value):
        dl=DataList()
        if (type(value)==DataList) & (len(self)>1): ## This means DataList >> DataList - here each member will be linked to the corresponding member in the other list
            for i in range(l): dl.append(self[i].__rrshift__(value[i]))
        else:
            for d in self: dl.append(d.__rrshift__(value))
        return dl
    def __rlshift__(self,value):
        dl=DataList()
        if (type(value)==DataList) & (len(self)>1): ## This means DataList << DataList - here each member will be linked to the corresponding member in the other list
            l=min(len(self),len(value))
            for i in range(l): dl.append(self[i].__rlshift__(value[i]))
        else:
            for d in self: dl.append(d.__rlshift__(value))
        return dl
    def __rshift__(self,value):
        dl=DataList()
        if (type(value)==DataList) & (len(self)>1): ## This means DataList >> DataList - here each member will be linked to the corresponding member in the other list
            l=min(len(self),len(value))
            for i in range(l): dl.append(self[i].__rshift__(value[i]))
        else:
            for d in self: dl.append(d.__rshift__(value))
        return dl
    def __lshift__(self,value):
        dl=DataList()
        if (type(value)==DataList) & (len(self)>1): ## This means DataList << DataList - here each member will be linked to the corresponding member in the other list
            l=min(len(self),len(value))
            for i in range(l): dl.append(self[i].__lshift__(value[i]))
        else:
            for d in self: dl.append(d.__lshift__(value))
        return dl
    def delObject(self):
        for d in self:
            d.delObject()
    def update(self):
        for d in self: d.update()
        return self
    def touch(self):
        for d in self: d.touch()
        return self
    def noSignal(self):
        for d in self: d.noSignal()
        return self
    def set(self,value):
        for d in self: d.set(value)
        return self
    def put_silent(self,value):
        for d in self: d.put_silent(value)
        return self
    def val(self):
        "Returns a list of the values in the Data object list"
        ret=[]
        for d in self: ret.append(d.val())
        return ret

class DataUnion(DataList):
    def __init__(self,*dlist):
        for dl in dlist:
            for d in dl:
                self.append(d)
#value >> self
    def __rrshift__(self,value):
        first=None
        if len(self)>=1: first=value >> self[0]
        for d in self[1:]: first >> d
        if len(first)>1: return first[0]
        else: return first
#value << self
    def __rlshift__(self,value):
        first=None
        if len(self)>=1: first=value << self[0]
        for d in self[1:]: first << d
        if len(first)>1: return first[0]
        else: return first
#self >> value
    def __rshift__(self,value):
        first=None
        if len(self)>=1: first=self[0] >> value
        for d in self[1:]: d >> first 
        if len(first)>1: return first[0]
        else: return first
#self << value
    def __lshift__(self,value):
        first=None
        if len(self)>=1: first=self[0] << value
        for d in self[1:]: d << first 
        if len(first)>1: return first[0]
        else: return first

def stauchString(string,maxlen=20):
    if len(string)>maxlen:
        return string[0:maxlen/2-1]+"..."+string[len(string)-maxlen/2:len(string)-1]
    else: return string

    
def VecToString(self,maxlen=10):
    veclen=len(self)
    stringval=""
    if veclen>0:
        stringval+=str(self[0])
    for i in range(1,min(len(self),maxlen)):
        stringval+=","+str(self[i])
    if veclen>maxlen:
        stringval+=",..."
    return stringval

def VecToPrintString(self,maxlen=10):
    return "Vec("+str(len(self))+")=[" +VecToString(self)+"]"

DirVec.__repr__=VecToPrintString        
FloatVec.__repr__=VecToPrintString                
IntVec.__repr__=VecToPrintString                
BoolVec.__repr__=VecToPrintString                
ComplexVec.__repr__=VecToPrintString                
StringVec.__repr__=VecToPrintString


def type2str(type):
    return {TYPE.POINTER:"ptr",TYPE.INTEGER:"int",TYPE.NUMBER:"float",TYPE.STRING:"str",TYPE.COMPLEX:"cmplx",TYPE.UNDEF:"undef"}[type]

def object_getitem(self,value):
    if type(value)==int:
        return self.Object(value)
    if type(value)==str:
        ids=self.IDs(value)
        if len(ids)==0:
            return DataList()
        elif len(ids)==1:
            return self.Object(ids[0])
        else:
            l=list()
            for i in ids:
               l.append(self.Object(i))
            return DataList(l)

def object_setitem(self,key,value):
    "Used to overload the assign operation for a data object with [] to set the content of the vector."
    return self[key].set(value)

def object_set_silent(self,value):
    sil=self.Silent(True)
    object_set(self,value)
    self.Silent(sil)
    return self

def list2vec(lst):
    if len(lst)==0: return FloatVec()
    if type(lst[0])==int:
        ret=IntVec()
    elif type(lst[0])==float:
        ret=FloatVec()
    elif type(lst[0])==str:
        ret=StringVec()
    elif type(lst[0])==complex:
        ret=ComplexVec()
    else: ret=[]    
    ret.extend(lst)
    return ret
    
def vec2list(vec):
    "Convert a stl vector to a python list"
    ret=[]
    ret.extend(vec)
    return ret
    
def object_set_list(self,lst):
    "Sets the Data object vector using a Python list as input. The type of the vector is determined by the first element."
    vec=list2vec(lst)
    if type(vec) in [FloatVec,IntVec,StringVec,ComplexVec]:
        self.set(vec)
    else: 
        self.putPyList(vec)
    return self

def object_set(self,value):
    if type(value)==int:
        self.putI(value)
    elif type(value)==float:
        self.putN(value)
    elif type(value)==str:
        self.putS(value)
    elif type(value)==complex:
        self.putC(value)
    elif type(value) in (IntVec,FloatVec,ComplexVec,StringVec):
        self.put(value)
    elif hasattr(value,"funcname"):
        self.setFunc_f(value)
    elif type(value)==list:
        object_set_list(self,value)
    else:
        self.putPy(value)
    return self

def object_val(self):
    typ=self.getType()
    if len(self)==1:
        if typ==TYPE.INTEGER:
            ret=self.getI()
        elif typ==TYPE.NUMBER:
            ret=self.getN()
        elif typ==TYPE.STRING:
            ret=self.getS()
        elif typ==TYPE.COMPLEX:
            ret=self.getC()
        elif typ==TYPE.POINTER:
            ret=self.getPy()
        return ret
    else:
        if (typ==TYPE.INTEGER | typ==TYPE.POINTER):
            v=IntVec()
        elif typ==TYPE.NUMBER:
            v=FloatVec()
        elif typ==TYPE.STRING:
            v=StringVec()
        elif typ==TYPE.COMPLEX:
            v=ComplexVec()
        self.get(v)
        return v


def object_print(self):
    return "<hfObject: "+self.Status()+">";

class _f():
    "This encapsulates a function name and library that will be used by Data.setFunction"
    def __init__(self,funcname="Copy",library="Sys",functype=TYPE.UNDEF):
        if type(funcname)==str:
            self.funcname=funcname
            self.library=library
            self.functype=functype
        else:
            self.funcname="PyFunc"
            self.library="Py"
            self.functype=funcname
    def __hftype__(self):
        return "_f"

class _l():
    "This encapsulates the Netlevel that can be used as a parameter during creation"
    def __init__(self,level=1):
        self.level=level
    def __repr__(self):
        return "<_l("+str(self.level)+")>"
    def __hftype__(self):
        return "_l"

def object_hasPyQt(self):
    return not (self.getStoredPyQtObject() == None)

def getStoredPyFuncObject(self):
    "Retrieves the user defined python function from the data object."
    oid=self.getOid()
    if self.stored_pyfuncobj.has_key(oid): return self.stored_pyfuncobj[oid]
    else: return None

def setStoredPyFuncObject(self, pyob):
    "Keeps a python reference to the user defined python function in the data object, so that Python does not delete it."
    self.stored_pyfuncobj[self.getOid()]=pyob

def getStoredPyQtObject(self):
    "Retrieves the user defined python Qt Object from the data object."
    oid=self.getOid()
    if self.stored_pyqtobj.has_key(oid): return self.stored_pyqtobj[oid]
    else: return None
    
def setStoredPyQtObject(self, pyob):
    "Keeps a python reference to the user defined python Qt Object in the data object, so that Python does not delete it."
    self.stored_pyqtobj[self.getOid()]=pyob


def initializePyQtObjects(self):
    for obj in self.stored_pyqtobj.values(): obj.obj.signalPyQt("updated")


def object_get_bool(self): return bool(self.getI())
def object_get_QString(self): return QtCore.QString(self.getS())

object_Type2Data_map={"int":"I","QString":"Q","float":"N","bool":"B","str":"S"}

def object_getT(self,Type):
    exec("ret=self.get"+object_Type2Data_map[Type]+"()")
    return ret

class hfPyQtObject(QtCore.QObject):
    "Base class for PyQtObjects that can be assigned to a Data object and take care of the PyQt Signals and Slots. The class has one slot: hfput(value), which receives a new value and assigns it to the object. It emits a signal hfupdated(QString) which contains the new value in an object, if it was changed. The method updated(object) is called from c++ and has as argument the object which was updated. self.obj should contain a reference to the Data object and needs to be set explicitly."
#    def __init__(self,Type="QString"):   The presence of the __init__ routine cause problems ..., like: RuntimeError: underlying C/C++ object has been deleted
#        self.type=Type
#        return None
    def hfput(self,value=None):
        print "Clicked ...!"
        if type(self.obj)==Data: 
            if value==None: self.obj.update()
            else: self.obj.object_set(value)
            return 0
        else:
            hfERR("hfPyQtObject - no Data Object set")
            return -1
    def updated(self,object):
        self.hfupdated(object.getT(self.type))
        return 0
    def hfupdated(self,value):
        self.emit(QtCore.SIGNAL("hfupdated("+self.type+")"),value)
        return 0


def object_activatePyQt(self,Type="QString"):
    "Creates and stores a hfPyQtObject for communication via the SIGNAL/SLOT mechanism between PyQt and a Data object."
    obj=hfPyQtObject()
    obj.type=Type
    obj.obj=self
    self.setStoredPyQtObject(obj)
    self.storePyQt(obj)

def object_PyQt(self):
    "Returns the Python object which interfaces between the Data object and PyQt."
    return self.getStoredPyQtObject()
    
def object_SIGNAL(self,Type="QString"):
    "Returns the SIGNAL the Data Object will send when updated. Can be used for QObject.connect()."
    return QtCore.SIGNAL("hfupdated("+Type+")")
    
def object_SLOT(self):
    "Returns the method of the Data object that can be used as a SLOT method for PyQt."
    return self.PyQt().hfput

def connect_action(guiaction,plottermethod):
    return QtCore.QObject.connect(gui.__getattribute__(guiaction),QtCore.SIGNAL("triggered()"),gui.HMainPlotter.__getattribute__(plottermethod))

def object_connect(self,method,slot="setText",Type="QString",isSlot=True):
    "Conects the data object's update signal to a Slot of a QtObject. The Update signal will contain the content of the object as a QString parameter. If slot=(), then connect to a method and not a SLOT."
    if not self.hasPyQt(): self.activatePyQt(Type)
    if isSlot: return QtCore.QObject.connect(self.PyQt(),self.SIGNAL(Type),method,QtCore.SLOT(slot+"("+Type+")")) 
    else: return QtCore.QObject.connect(self.PyQt(),self.SIGNAL(Type),method.__getattribute__(slot))

    
def object_setFunc_f_silent(self,f):
    "Assining a function using the _f class as input but not causing an update."
    sil=self.Silent(True)
    if type(f.functype)==TYPE:
        self.setFunc(f.funcname,f.library,f.functype)
    elif f.library=="Py": 
#       In this case we have specified a Python function
        self.setStoredPyFuncObject(f.functype()) ##Store Python function to keep the instance under python
        self.storePyFunc(self.getStoredPyFuncObject()) ##Store it also on the c++ level (this can perhaps smarter ...)
        self.setFunc(f.funcname,f.library,TYPE.STRING) 
#        print self.getStoredPyObjecdt()
#        print type(self.getStoredPyFuncObject())
##        pyobj=self.newObject("'PythonObject",PUSH)
##        pyobj.putPy_silent(self.getStoredPyFuncObject())
##        pyobj.setNetLevel(1000)
    self.Silent(sil)
    return self

def object_setFunc_f(self,f):
    "Assining a function using the _f class as input."
    if type(f.functype)==TYPE:
        self.setFunc(f.funcname,f.library,f.functype)
    elif f.library=="Py": 
#       In this case we have specified a Python function
        self.setStoredPyFuncObject(f.functype()) ##Store Python function to keep the instance under python
        self.storePyFunc(self.getStoredPyFuncObject()) ##Store it also on the c++ level (this can perhaps smarter ...)
        self.setFunc(f.funcname,f.library,TYPE.STRING) 
##        self.setStoredPyFuncObject(f.functype())
#        print self.getStoredPyFuncObject()
#        print type(self.getStoredPyFuncObject())
##        pyobj=self.newObject("'PythonObject",PUSH)
##        pyobj.putPy(self.getStoredPyFuncObject())
##        pyobj.setNetLevel(1000)
        self.setFunc(f.funcname,f.library,TYPE.STRING)
    return self


def object_setFunc(self,funcname="Copy",library="Sys",functype=TYPE.UNDEF):
    "Assigns a function to a data object and returns it"
    self.setFunction(funcname,library,functype)
    return self

#self // other  - deletes a link in the to direction
def object_floordiv(self,other): 
    "data // data deletes a link between two objects in eitehr FROM or TO direction. The function returns the first data object."
    if type(other)==str:
        obj=self["other"]
    elif type(other)==Data:
        obj=other
    elif type(other)==DataList:
        return other // self
    self.delLink(obj)
    obj.touch()
    self.touch()
    return obj

def object_invert(self):  # delObject
    "~data deletes an object."
    self.delObject()


def object_xxshift(self,pre,other,dir):
    "Used to overload the shift operators. Is called by functions like object_rrshift etc. The >> or << operators can be used to connect two data objetcs or a dataobject and a string or tuple. In the latter case a new object is created with the name of the string the data object links to. The Tuple contains the name as first element and then additional initialization parameters, e.g. a value to be put in, a function (bracketed by _f()), or a NetLevel (use _l(level))"
    if type(other) in [tuple,list,DataParameterList]:
        newobj=self.newObject(pre+other[0],dir).initializeObject(other)
        return newobj
    elif type(other)==Data:    # if both arguments are data objects, then only set a link
        other.setLink(self,dir)
        other.touch()
        return other
    else:
        return self.newObject(pre+other,dir)

def object_xxshift_list(self,pre,other,dir):
    "Used to overload the shift operators. Is called by object_xxshift."
    if type(other)==list:
        dl=DataList()
        for o in other:
            dl.append(object_xxshift(self,pre,o,dir))
        return dl
    else:
        return object_xxshift(self,pre,other,dir)

#other >> self
def object_rrshift(self,other): 
    " 'NAME'>>data object creates a new predecessor object with PUSH connection. ('NAME',value)>>data also assigns a value to the new object. This can also be a function, using _f(funcname,library,type).  A list of tuples [('Name',val),('Name',val),...] can be given to create multiple objects at the same time. The function returns the (last) new object."
    return object_xxshift_list(self,"'",other,PUSH)

#self>>other
def object_rshift(self,other):
    " data>>'NAME' object creates a new successor object with PUSH connection. data>>('NAME',value) also assigns a value to the new object. This can also be a function, using _f(funcname,library,type).  A list of tuples [('Name',val),('Name',val),...] can be given to create multiple objects at the same time. The function returns the (last) new object."
    return object_xxshift_list(self,"",other,PUSH)

#other<<self
def object_rlshift(self,other):
    " 'NAME'<<data object creates a new predecessor object with PULL connection. ('NAME',value)<<data also assigns a value to the new object. This can also be a function, using _f(funcname,library,type).  A list of tuples [('Name',val),('Name',val),...] can be given to create multiple objects at the same time. The function returns the (last) new object."
    return object_xxshift_list(self,"'",other,PULL)

#self<<other
def object_lshift(self,other): 
    " data<<'NAME' creates a new successor object with PULL connection. data<<('NAME',value) also assigns a value to the new object. This can also be a function, using _f(funcname,library,type). A list of tuples [('Name',val),('Name',val),...] can be given to create multiple objects at the same time. The funciton returns the (last) new object."
    return object_xxshift_list(self,"",other,PULL)

#other >> self
def object_rrshift(self,other): 
    " 'NAME'>>data object creates a new predecessor object with PUSH connection. ('NAME',value)>>data also assigns a value to the new object. This can also be a function, using _f(funcname,library,type).  A list of tuples [('Name',val),('Name',val),...] can be given to create multiple objects at the same time. The function returns the (last) new object."
    return object_xxshift_list(self,"'",other,PUSH)

#self>>other
def object_rshift(self,other):
    " data>>'NAME' object creates a new successor object with PUSH connection. data>>('NAME',value) also assigns a value to the new object. This can also be a function, using _f(funcname,library,type).  A list of tuples [('Name',val),('Name',val),...] can be given to create multiple objects at the same time. The function returns the (last) new object."
    return object_xxshift_list(self,"",other,PUSH)

#other<<self
def object_rlshift(self,other):
    " 'NAME'<<data object creates a new predecessor object with PULL connection. ('NAME',value)<<data also assigns a value to the new object. This can also be a function, using _f(funcname,library,type).  A list of tuples [('Name',val),('Name',val),...] can be given to create multiple objects at the same time. The function returns the (last) new object."
    return object_xxshift_list(self,"'",other,PULL)

#self<<other
def object_lshift(self,other): 
    " data<<'NAME' creates a new successor object with PULL connection. data<<('NAME',value) also assigns a value to the new object. This can also be a function, using _f(funcname,library,type). A list of tuples [('Name',val),('Name',val),...] can be given to create multiple objects at the same time. The funciton returns the (last) new object."
    return object_xxshift_list(self,"",other,PULL)

def DOTname(self):
    return self.getName()+"_"+str(self.getOid())

def object_getDOT(self,maxnetlevel,details=1,reverse=False):
    "Returns the network connections of the object in a form suitable for exporting it in the DOT language format (www.graphviz.org)"
    name=DOTname(self)
    typ=type2str(self.getType())
    shape="egg"
    label=self.getName().replace("-","__")
    netlevel=self.getNetLevel()
    if netlevel>maxnetlevel:
        return ""
    if details==1: label+=" ["+str(self.getOid())+"]"
    if details>=2: label+="\\n["+str(self.getOid())+"] "+typ+"("+str(len(self))+")"
    color="black"
    modified=self.isModified()
    if (modified & (not self.Empty())): color="red"
    vs=StringVec()
    if not self.Empty():
        if self.hasFunc(): 
            shape="ellipse"
            if details>=1: label+="\\n("+self.getFuncName()+")"
        if self.hasData(): 
            shape="box"
            if details>=1: 
                self.inspect(vs)
                label+="\\n"+stauchString(VecToString(vs))
            if not modified: color="green"
        if self.hasFunc() & self.hasData(): shape="octagon"
    props='label="'+label+'"'
    props+=",shape="+shape
    props+=",color="+color
    out = name +" ["+props+"];"
    if not reverse:
        l= self.listIDs(DIR.TO)
        dirs=self.listDirs(DIR.TO)
        modflags=self.listModFlags(DIR.TO)
        for i in range(len(l)):
            if len(out)>0:
                out+="\n"
                arrowhead="normal"
                arrowcolor="red"
                if dirs[i]==PULL: arrowhead="inv"
                if modflags[i]=="NULL.0": arrowcolor="black"
                out += name + " -> " + DOTname(self[l[i]])+"[arrowhead="+arrowhead+",color="+arrowcolor+"];"
    else:
        l= self.listIDs(DIR.FROM)
        dirs=self.listDirs(DIR.FROM)
        modflags=self.listModFlags(DIR.FROM)
        for i in range(len(l)):
            if len(out)>0:
                out+="\n";
                arrowhead="normal"
                arrowcolor="red"
                if dirs[i]==PULL: arrowhead="inv"
                if modflags[i]=="NULL.0": arrowcolor="black"
                out += name + " <- " + DOTname(self[l[i]])+"[arrowhead="+arrowhead+",color=",arrowcolor+"];"
    return out

class hfWrite_File():
    def __init__(self,fn):
        self.fn=fn
        self.FILE=open(fn,"w")
    def write(self,s):
        self.FILE.write(s)
    def close(self):
        self.FILE.close()
        return self.fn

class hfWrite_QByte():
    def __init__(self):
        self.qb=QtCore.QByteArray()
    def write(self,s):
        self.qb.append(s)
    def close(self):
        return self.qb

class hfWrite_Str():
    def __init__(self):
        self.s=""
    def write(self,s):
        self.s+=s
    def close(self):
        return self.s

def object_exportDOT(self,destination,maxnetlevel=999,details=1,reverse=False):
    "Exports the entire network structure into a .dot file or QByte array that can be viewed with graphviz or dotty"
    
    l = self.getAllIDs()
    if destination=="FILE": 
        name=self.getName()
        fn=name+".dot" 
        OUT=hfWrite_File(fn)
        OUT.write("#dot -Tgif "+fn+" -o "+name+".gif; xview "+name+".gif\n")
        OUT.write("#dotty "+fn+"\n")
        OUT.write("#/Applications/Graphviz.app/Contents/MacOS/Graphviz  LOPES.dot "+fn+"\n")
    elif destination=="Qt": 
        OUT=hfWrite_QByte()
    elif destination=="Str": 
        OUT=hfWrite_Str()
    else: 
        print "Error - object_exportDOT: Destination",destination,"unknown!"
        return
    OUT.write('digraph G {\nsize="8.25,11.6"; ratio=compress;\n')
    n=0
    for i in l:
        if ((n==0) | (i!=0)): OUT.write(self[i].getDOT(maxnetlevel,details,reverse)+"\n")
        n+=1
    OUT.write("}\n")
    return OUT.close()

def object_net2qb(self,maxnetlevel=999):
    qb=QtCore.QByteArray()
    p=subprocess.Popen("dot -Tsvg",stdin=subprocess.PIPE,stdout=subprocess.PIPE,shell=True,close_fds=True)
    p.stdin.write(self.exportDOT("Str",maxnetlevel))
    p.stdin.close()
    qb.append(p.stdout.read())
    p.terminate()
    return qb

def object_nview(self,maxnetlevel=999):
    "Allows one to dsiplay the network an object is embedded in, in a QtWidget. self is the object, and maxnetlevel says how much detail is to be shown - increases in steps of powers of ten: 9,99,999,9999"
    self.qsvg=QtSvg.QSvgWidget()
    self.qsvg.setWindowTitle(self.getName()+" Network Display")
    self.qsvg.load(object_net2qb(self,maxnetlevel))
    self.qsvg.raise_()
    self.qsvg.show()
#    fn=self.exportDOT("FILE",maxnetlevel)
#    os.system("dot -Tsvg "+fn+" -o "+fn+".svg")


def object_putPyList_silent(self,l):
    sil=self.Silent(True)
    self.putPyList(l)
    self.Silent(sil)


def object_equal(self,other):
    if type(other)==Data:
        return self.getOid()==other.getOid()
    else: return False

Data.putPyList_silent=object_putPyList_silent
Data.__eq__=object_equal
Data.__repr__=object_print
Data.__invert__=object_invert  #~data = delObject
DataList.__invert__=object_invert  #~data = delObject
Data.__rrshift__=object_rrshift   #newobject+setlink
Data.__rshift__=object_rshift     #newobject+setlink
Data.__rlshift__=object_rlshift   #newobject+setlink 
Data.__lshift__=object_lshift     #newobject+setlink
Data.__setitem__=object_setitem
Data.__getitem__=object_getitem
Data.put_silent=object_set_silent
Data.__floordiv__=object_floordiv # // = dellink
Data.__xor__=insert_object        #d ^ (d1,d2) = insert 
Data.new=object_new
Data.initializeObject=initializeObject
Data.netsvg=object_net2qb

Data.getitem=object_getitem
Data.setFunc=object_setFunc
Data.set=object_set
Data.val=object_val
Data.setFunc_f=object_setFunc_f
Data.setFunc_f_silent=object_setFunc_f_silent
Data.getDOT=object_getDOT
Data.exportDOT=object_exportDOT
Data.nview=object_nview
Data.nv=object_nview

def datalist(self,maxnetlevel=9999):
    self.printAllStatus(1,maxnetlevel)

Data.notFound=object_notFound
Data.NFound=object_NFound
Data.list = datalist
Data.ls = datalist
Data.stored_pyfuncobj={}
Data.stored_pyqtobj={}
Data.getStoredPyFuncObject=getStoredPyFuncObject
Data.setStoredPyFuncObject=setStoredPyFuncObject
Data.getStoredPyQtObject=getStoredPyQtObject
Data.setStoredPyQtObject=setStoredPyQtObject
Data.PyFunc=Data.retrievePyFuncObject
Data.activatePyQt=object_activatePyQt
Data.hasPyQt=object_hasPyQt
Data.connect=object_connect
Data.PyQt=object_PyQt
Data.SIGNAL=object_SIGNAL
Data.SLOT=object_SLOT
Data.getT=object_getT
Data.getB=object_get_bool
Data.getQ=object_get_QString

#Extending mathgl to accept STL vectors 
def mglDataSetVec(self,vec):
    mglDataSet(self,vec)

mglData.SetVec = mglDataSetVec


mglGraphZB=0
mglGraphPS=1
mglGraphGL=2
mglGraphIDTF=3

def e(file): 
    "short form form execfile(file)"
    print "Loading",file,"."
    execfile(file)
