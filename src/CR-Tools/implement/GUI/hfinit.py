#pdb.set_trace()
pyoblist=[]

PUSH=DIR.TO
PULL=DIR.FROM

maxnetlevel=999

def hfERR(txt):
    print "Error:",txt

def mem(size="rss"):
    """Generalization; memory sizes: rss, rsz, vsz."""
    return int(os.popen('ps -p %d -o %s | tail -1' %
                        (os.getpid(), size)).read())

#Extending mathgl to accept STL vectors 
def mglDataSetVec(self,vec):
    mglDataSet(self,vec)

mglData.SetVec = mglDataSetVec


mglGraphZB=0
mglGraphPS=1
mglGraphGL=2
mglGraphIDTF=3
#mglGraphAB is not available - ZB is default

def e(file): 
    "short form form execfile(file.py)"
    print "Loading",file+".py"
    execfile(file+".py")

help={}
help["verbose"]="Use d.AllVerbose(True/False) to watch the execution of the network. The output is {Objectid}Name(data_vector[0])flag, where flag is one of the following: blank=no function, just data; ~=just a function, no data; *=function and data present and is modified; $=function and data present and is not modified."


######Utilities

def list2str(l): 
    if len(l)==0: return ""
    else: return reduce(lambda x,y:str(x)+" "+str(y),l)

def flatten(l):
  out = []
  for item in l:
    if isinstance(item, (list, tuple)):
      out.extend(flatten(item))
    else:
      out.append(item)
  return out

def ListElement2IntVecElements(l):
    if len(l)==1: return int(l[0])
    elif len(l)==2: return range(int(l[0]),int(l[1])+1)
    else: return 0

def text2IntVec(s):
    return flatten(map(lambda x:ListElement2IntVecElements(x.split('..')),s.split(',')))

def IntVec2NiceString(l):
    s=""
    if len(l)==0: return s
    l.sort()
    prev=l[0]
    s+=str(prev)
    rng=False
    for i in l[1:]:
        if (i==prev+1): 
            rng=True
        else:
            if rng: s+=".."+str(prev)
            rng=False
            s+=","+str(i)
        prev=i
    return s
            
    
#####


def select(l,elems): 
    "Returns the elements of list l, which are given in the list of the second argument."
    return map(lambda i:l[i],elems)
def Identity(x,*arg): return x
def returnFalse(*arg): return False
def returnTrue(*arg): return True
def returnOne(*arg): return 1
def returnEmptyDatalist(*arg): return DataList()


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
    "Creates a new object an initializes it (see .create() and .initializeObject())"
    tup=(arg)
    if (len(tup)>1): return self.create(tup[0]).initializeObject(tup)
    else: return self.create(tup[0])

def strtovalue(s):
    "Converts a string to either an Integer or a Float if it starts with a digit. Otherwise return the string"
    if len(s)>0:
        if s[0].isdigit():
            val=float(s)
            if val.is_integer(): val=int(val)
        else: val=s
    else: val=s
    return val
        
def object_find_or_make(self,*arg):
    "Find an object of name 'name' or creates it if not found and initializes it fit the parameteres in the argument list\
    (see .create() and .initializeObject()).\
    You can use '=' for searching, however, more complicated search paths are not supported yet. \
    For example, the name of the object is considered to be the string until the first occurance of '='!\
    Example: object_find_or_make(d,''NAME=1'',_l(99)).\
    One can also specify a list of names: object_find_or_make(d,[''NAME=Eins'',''NAME=Zwei''],_l(99))."
    tup=(arg)
    names=tup[0]
    if type(names)!=list: names=[names]
    ret=DataList()
    for name in names:
        obj=self[name]
        if obj.notFound():
            if (PULL in tup): dir_type=PULL
            elif (PUSH in tup): dir_type=PUSH
            else: dir_type=DIR.NONE
            pinit=tuple(set(tup[1:]).difference([PUSH,PULL,DIR.NONE]))
            idx=name.find("=")
            if idx>0:
                val=strtovalue(name[idx+1:]) # value is everything after "="
                name=name[:idx] #Only take name until the first "="
                pinit=(name,val)+pinit
            else: pinit=(name,)+pinit
            ret.append(self.newObject(name,dir_type).initializeObject(pinit))
#            if (len(tup)>1): ret.append(self.newObject(name,dir_type).initializeObject(pinit))
#            else: ret.append(self.newObject(name,dir_type))
        else: ret.append(obj)
    return ret.unique()


def MakeChooser(d,name,fields,values):
    "Create a chooser, which is a collection of objects which act like a record of similar records, containgin a set of parameters among which you can choose in a menu. The first parameter is the name of the chooser object. The second parameter is a tuple of parameter names that form one set. The third parameter is a tuple of tuple containing the parameter values which belong to the respective parameters."
    chooser=d.new("Chooser",name,_l(100))
    for val in values:
        obj=(fields[0],val[0],_l(2000)) >> chooser
        for i in range(1,len(fields)):
          obj=(fields[i],val[i],_l(2000)) >> obj
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

def object_Chain(self,dir,names,include_endpoints=True,monotonic=False):
    if type(names)==str: names=[names]
    if type(names)==list: names=list2vec(names)
    ids=self.FindChainID(dir,names,include_endpoints,monotonic)
    return self[ids].unique()

def object_asDataList(self):
    return DataList([self])
def object_getNeighbours(self,dir):
    return self[self.listNeighbourIDs(dir)]
def object_getNeighbour(self,dir):
    return self[self.listNeighbourIDs(dir)].FirstObject()

def object_search(self,s):
    "Function to search all objects for one which contains the string s in its name"
    l=DataList()
    for dd in d.All():
        if s in dd.getName(): l.append(dd)
    if len(l)==1: return l[0]
    else: return l

def toList(val):
    "Returns val as a List, i.e leave it unchanged if it is already a list, or, for a scalar, create a one-element list"
    if type(val)==list: return val
    else: return [val]

def make_unique_new(seq):  
    x=set(seq)
    return list(x.union(x))
    
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
        if marker in seen: continue 
        seen[marker] = 1 
        result.append(item) 
    return result

def datalist_unique(self):
    return make_unique(self,object_id).unList()

def datalist_unique_new(self): #Does not work for Data objects
    x=set(self)
    return DataList(x.union(x)).unList()

def object_move(self,objfrom,objto):
    "Moves this object linked to objfrom to be linked to  objto. The link to objto is cut."
    sil0=self.Silent(True)
    sil1=objfrom.Silent(True)
    sil2=objto.Silent(True)
    self >> objto
    self // objfrom
    self.wakeUp()
    objto.wakeUp()
    objfrom.wakeUp()
    self.Silent(sil0)
    objfrom.Silent(sil1)
    objto.Silent(sil2)

class DataList(list):
    def datalist(self,*args): map(lambda d:d.printStatus(1,*args),self)
    def __getitem__(self,idx):
        "If index is  a string then retrieve a list of data objects with that sting index from every data object in the list and create a new (flat) data list, otherwise return element idx"
        if type(idx)==str:  
            dl=DataList()
            for d in self:
                el=d[idx]
                if type(el)==DataList: dl.extend(el) ##make sure the list remains flat ...
                else: dl.append(el)
            return dl.unique()
        else:
            return list(self)[idx]
    def isDataList(self):
        "Used for determining whether this is a DataList object (which it is) and not a DataObject."
        return True
    def isSingleDataObject(self):
        "Used for determining whether this is a single Data object (which it isn't) and not a DataList."
        return False
    def notFound(self):
        "Used for determining whether a search was successful. If not, an empty DataList will be returned that returns true  for this function."
        return len(self)==0
    def Found(self):
        "Used for determining whether a search was successful and the Datalist contains one or more elements."
        return len(self)>0
    def NFound(self):
        "Number of objects in the DataList. Same as len. Used for determining whether a search was successful. If not, an empty DataList will be returned that returns zero for this function. If one object is found an Object is returned."
        return len(self)
    def __floordiv__(self,other):
        " d1 // d2 : Delete the link between the other objects and each object in the DataList. If other is also a DataLists, then remove the links pairwise." 
        if other.isDataList(): 
            map(lambda d1,d2:d1//d2,self,other)
        else: 
            for d in self: d // other
        return other
    def __ifloordiv__(self,other):
        map(lambda d1,d2:d1.erase(d2),self,other)
        return self
    def erase(self,other):
        map(lambda d1,d2:d1.erase(d2),self,other)
        return self
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
    def resetLink(self,value,*args):
        "This will create a link between self and the other object (value).\
        If the link already exists it will be overwritten with the \
        current parameters or remain unchanged if the parameters are \
        the same. Parameters are those of SetLink, i.e. the other\
        object, followed by the the type of connection (PUSH/PULL) and \
        the direction (DIR.TO/DIR.FROM). If there are two DataLists a link\
        will be created between each corresponding element. If there is\
        one object and a DataList, there will be a link between the object\
        and each element in the DataList."
        if (type(value)==DataList):  ## This means DataList >> DataList - here each member will be linked to the corresponding member in the other list
            if len(self)==1 | len(value)==1: return self.unList().resetLink(value.unList(),*args) # here we convert a list to a single object and try again.
            if not len(self)==len(value): 
                hfERR("resetLink - Unable to set links between unequally long DataLists:" + str(self)+ "and" + str(value))
                return DataList()
            return DataList(map(lambda d1,d2:d1.resetLink_cc(d2,*args),self,value)) # here link every element of self with the corresponging element in value
        return DataList(map(lambda d:d.resetLink_cc(value,*args),self)) # here link the single object value with every element of self 
    def delObject(self):
        "Will delete objects in the data list silently."
        map(lambda d:d.delObject_silent(),self)
        return DataList()
    def __invert__(self): #    =>   ~[obj1,obj2,....]
        "Will delete objects in the data list silently."
        map(lambda d:d.delObject_silent(),self)
    def find_or_make(self,*arg): #  
        "Find an object of name 'name' or creates it if not found and initializes it fit the parameteres in the argument list\
        (see .create() and .initializeObject()).\
        You can use '=' for searching, however, more complicated search paths are not supported yet. \
        For example, the name of the object is considered to be the string until the first occurance of '='!\
        Example: object_find_or_make(d,''NAME=1'',_l(99)).\
        One can also specify a list of names: object_find_or_make(d,[''NAME=Eins'',''NAME=Zwei''],_l(99))."
        return DataList(map(lambda d:d.find_or_make(*arg),self))
    def getAllIDs(self): #  
        "Retrievs all objects IDs in the list."
        return map(lambda d:d.getOid(),self)
    def Tail(self,start=1): #  
        "Retrievs the first object in the list."
        if len(self)>start: return self[start:]
        else: return DataList()
    def FirstObject(self): #  
        "Retrievs the first object in the list."
        if len(self)>0: return self[0]
        else: return DataList()
    def LastObject(self): #  
        "Retrievs the last object in the list."
        if len(self)>0: return self[-1]
        else: return DataList()
    def unList(self):
        "If DataList contains only a single element, return that one, rather than a list"
        if len(self)==1: return self[0]
        else: return self
    def asList(self):
        "Return a datalist, even if it is a single object."
        return self
    def getType(self): #  
        "Retrievs the internally stored datatypes of all objects."
        return map(lambda d:d.getType(),self)
    def getPy(self): #  
        "Retrievs all Python objects from teh objects in the list."
        return map(lambda d:d.getPy(),self)
    def connect(self,method,slot="setText",Type="QString",isSlot=True):
        self[0].connect(method,slot,Type,isSlot)
        return self
    def update(self):
        map(lambda d:d.update(),self)
        return self
    def wakeUp(self):
        map(lambda d:d.wakeUp(),self)
        return self
    def touch(self):
        map(lambda d:d.touch(),self)
        return self
    def noSignal(self):
        map(lambda d:d.noSignal(),self)
        return self
    def set(self,value):
        map(lambda d:d.set(value),self)
        return self
    def set_silent(self,value):
        map(lambda d:d.set_silent(value),self)
        return self
    def put_silent(self,value):
        map(lambda d:d.put_silent(value),self)
        return self
    def storePyDBG(self,value):
        map(lambda d:d.storePyDBG(value),self)
        return self
    def setVerbose(self,value):
        map(lambda d:d.setVerbose(value),self)
        return self
    def setDebug(self,value):
        map(lambda d:d.setDebug(value),self)
        return self
    def setDebugGUI(self,value):
        map(lambda d:d.setDebugGUI(value),self)
        return self
    def setNetLevel(self,value):
        map(lambda d:d.setNetLevel(value),self)
        return self
    def printStatus(self,value=True):
        map(lambda d:d.printStatus(value),self)
        return self
    def clearModification(self):
        map(lambda d:d.clearModification(),self)
        return self
    def setFunc_f_silent(self,f):
        "Assigns a function to each data object in the list using the _f mechanism to specify which function to assign."
        map(lambda d:d.setFunc_f_silent(f),self)
        return self
    def setList(self,l):
        "Assigns each member of the datalist the corresponding value in the input list l."
        return DataList(map(lambda d,v:d.set(v),self,l))
    def doSilent(self):
        map(lambda d:d.doSilent(),self)
        return self
    def Silent(self,sil):
        "Sets the silent flag of an object which can inhibit the update chain (but not the modification flagging): True or False."
        ret=[]
        if type(sil)==list:
            for i in range(len(self)): ret.append(self[i].Silent(sil[i]))
        else:
            for d in self: ret.append(d.Silent(sil))
        return ret
    def val(self):
        "Returns a list of the values in the Data object list"
        return map(lambda d:d.val(),self)
    def getS(self):
        "Returns a list of the values in the Data object list in String format"
        return map(lambda d:d.getS(),self)
    def getI(self):
        "Returns a list of the values in the Data object list in Integer format"
        return map(lambda d:d.getI(),self)
    def getN(self):
        "Returns a list of the values in the Data object list in floating point Number format"
        return map(lambda d:d.getN(),self)
    def getC(self):
        "Returns a list of the values in the Data object list in Complex number format"
        return map(lambda d:d.getC(),self)
    def getDebugGUI(self):
        "Returns a list of the GUI verbose flag in the Data object list."
        return map(lambda d:d.getDebugGUI(),self)
    def getDebug(self):
        "Returns a list of the Debug flag in the Data object list."
        return map(lambda d:d.getDebug(),self)
    def getNetLevel(self):
        "Returns a list of the NetLevels in the Data object list."
        return map(lambda d:d.getNetLevel(),self)
    def getOid(self):
        "Returns a list of the IDs in the Data object list."
        return map(lambda d:d.getOid(),self)
    def getName(self,longname=False):
        "Returns a list of the Names in the Data object list."
        return map(lambda d:d.getName(longname),self)
    def getSearchName(self,longname=False):
        "Returns a list of the Names in the Data object list."
        return map(lambda d:d.getSearchName(longname),self)
    def getLinkDirection(self,neighbour):
        "Returns the link direction between the current and its neighbouring object, given as an argument."
        return map(lambda d:d.getLinkDirection(neighbour),self)
    def getLinkDirectionType(self,neighbour):
        "Returns the link direction type (PUSH or PULL) between the current and its neighbouring object, given as an argument."
        return map(lambda d:d.getLinkDirectionType(neighbour),self)
    def getNeighbours(self,dir):
        "Returns the immediate neighbours in direction DIR"
        return DataList(map(lambda d:d.getNeighbours(dir),self))
    def getNeighbour(self,dir):
        "Returns the first immediate neighbour in direction DIR"
        return DataList(map(lambda d:d.getNeighbour(dir),self))
    def Chain(self,dir,names,include_endpoints=True,monotonic=False):
        "Returns a list of objects between the current one and the ones specified in names. If monotnic=True the chain only looks for objects in the specified direction ,otherwise it will take the direction only for the first object into account. If endpoints=true, they will be included in the return list(s)"
        return DataList(map(lambda d:d.Chain(dir,names,include_endpoints,monotonic),self))



def DataUnion(*dlist):
    "This is a class derived from DataList which is used in the construction of networks. The idea is that the objects in the DataUnion are treates as if they were one object. Hence if a link is created between a newobject and a DataUnion only one new object will be created, which is the linked to all elements of the Union. In contrast for a DataList, there would be a new Object created for each element in the List and linked to ist corresponding parent."
    if len(dlist)==1: 
        if type(dlist[0])==Data:
            return dlist[0]
    return DataUnionClass(dlist)


class DataUnionClass(DataList):
    def __init__(self,dlist):
        for dl in dlist:
            if type(dl)==DataList:
                self.extend(dl)
            elif type(dl)==Data:
                self.append(dl)
            else: print "DataUnionClass: unknown member type:",dl
    def __getitem__(self,idx):
        "If index is a string then retrieve a list of data objects with that sting index from every data object in the list and create a new (flat) data list, otherwise return element idx"
        return DataUnion(DataList(self)[idx])
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
    def newObject(self,*arg):
        no=DataList()
        if len(self)>0:
            no=self.FirstObject().newObject(*arg) 
        if len(self)>1:
            no >> self.Tail()
        return no    
    def find_or_make(self,*arg):
        no=DataList()
        if len(self)>0:
            no=self.FirstObject().find_or_make(*arg) 
        if len(self)>1:
            ld=self.getLinkDirection(no)
            ldt=self.getLinkDirectionType(no)
            map(lambda d:d.resetLink(no,ldt[0],ld[0]),self[1:])
        return no    


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

def object_all(self):
    return self[self.getAllIDs()]

def object_asList(self):
    "Returns the DataObject as a Data(Object)List."
    return DataList([self])

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
            return DataList(map(lambda v:self.Object(v),ids))
    if (type(value)==list) | (type(value)==IntVec) | (type(value)==IntVec):
        return DataList(map(lambda v:self.Object(v),value))

def object_setitem(self,key,value):
    "Used to overload the assign operation for a data object with [] to set the content of the vector."
    return self[key].set(value)

def object_set_silent(self,value):
    sil=self.Silent(True)
    object_set(self,value)
    self.Silent(sil)
    return self

def object_setList(self,l):
    self.set(l[0])
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
    if type(vec)==list: return vec
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
    elif type(value)==bool:
        self.putI(int(value))
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

def object_getIV(self):
    "Return the contents of the data vector as an integer vector"
    vec=IntVec()
    self.get(vec)
    return vec

def object_getNV(self):
    "Return the contents of the data vector as a number vector"
    vec=FloatVec()
    self.get(vec)
    return vec

def object_getCV(self):
    "Return the contents of the data vector as a complex vector"
    vec=ComplexVec()
    self.get(vec)
    return vec

def object_getSV(self):
    "Return the contents of the data vector as a string vector"
    vec=StringVec()
    self.get(vec)
    return vec

def object_getIL(self):
    "Return the contents of the data vector as an integer list"
    return vec2list(self.getIV())

def object_getNL(self):
    "Return the contents of the data vector as a number list"
    return vec2list(self.getNV())

def object_getCL(self):
    "Return the contents of the data vector as a complex list"
    return vec2list(self.getCV())

def object_getSL(self):
    "Return the contents of the data vector as an string list"
    return vec2list(self.getSV())

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
            ret=self.getI()
        return ret
    else:
        if ((typ==TYPE.INTEGER) | (typ==TYPE.POINTER)):
            v=IntVec()
        elif typ==TYPE.NUMBER:
            v=FloatVec()
        elif typ==TYPE.STRING:
            v=StringVec()
        elif typ==TYPE.COMPLEX:
            v=ComplexVec()
        else:
            v=FloatVec()
            hfERR("Object "+self.getName()+": Type undefined.")
        self.get(v)
        return v


def object_print(self):
    return "<hfObject: "+self.Status()+">";

class _f():
    "This encapsulates a function name and library that will be used by Data.setFunction"
    def __init__(self,funcname="Copy",library="",functype=TYPE.UNDEF):
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

def object_hasPyDBG(self):
    return not (self.getStoredPyDBGObject() == None)

def getStoredPyFuncObject(self):
    "Retrieves the user defined python function from the data object."
    oid=self.getOid()
    if self.stored_pyfuncobj.has_key(oid): return self.stored_pyfuncobj[oid]
    else: return None

def setStoredPyFuncObject(self, pyob):
    "Keeps a python reference to the user defined python function in the data object, so that Python does not delete it."
    self.stored_pyfuncobj[self.getOid()]=pyob

def getStoredPyDBGObject(self):
    "Retrieves the user defined python DBG Object from the data object."
    oid=self.getOid()
    if self.stored_pydbgobj.has_key(oid): return self.stored_pydbgobj[oid]
    else: return None
    
def setStoredPyDBGObject(self, pyob):
    "Keeps a python reference to the user defined python DBG Object in the data object, so that Python does not delete it."
    self.stored_pydbgobj[self.getOid()]=pyob

def getStoredPyQtObject(self):
    "Retrieves the user defined python Qt Object from the data object."
    oid=self.getOid()
    if self.stored_pyqtobj.has_key(oid): return self.stored_pyqtobj[oid]
    else: return None
    
def setStoredPyQtObject(self, pyob):
    "Keeps a python reference to the user defined python Qt Object in the data object, so that Python does not delete it."
    self.stored_pyqtobj[self.getOid()]=pyob


def initializePyQtObjects(self):
    for obj in self.stored_pyqtobj.values(): 
        obj.obj.signalPyQt("updated")


def object_get_bool(self): return bool(self.getI())
def object_get_QString(self): return QtCore.QString(self.getS())

object_Type2Data_map={"int":"I","QString":"Q","float":"N","bool":"B","str":"S"}

def object_getT(self,Type):
    "Returns the value of an object as type 'Type'. The type name is based on the type definition used by PyQt."
    exec("ret=self.get"+object_Type2Data_map[Type]+"()")
    return ret

class hfPyQtObject(QtCore.QObject):
    "Base class for PyQtObjects that can be assigned to a Data object and take care of the PyQt Signals and Slots. The class has one slot: hfput(value), which receives a new value and assigns it to the object. It emits a signal hfupdated(QString) which contains the new value in an object, if it was changed. The method updated(object) is called from c++ and has as argument the object which was updated. self.obj should contain a reference to the Data object and needs to be set explicitly. This is done by activatePyQt."
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
        if len(object)==1:
            self.hfupdated(object.getT(self.type))
        else:
            if (self.type!="QString") | (object.getType()!=TYPE.INTEGER): 
                print object
                hfERR("Only Conversion from Integer Lists to QString are supported for connecting an object to the GUI! (SLOT="+str(self.type)+", object="+str(object.getType())+")")
                return -1
            self.hfupdated(IntVec2NiceString(vec2list(object.val())))
        return 0
    def hfupdated(self,value):
        self.emit(QtCore.SIGNAL("hfupdated("+self.type+")"),value)
        return 0


class hfPyDBGObject(QtCore.QObject):
    "Base class for PyDBGObjects that can be assigned to a Data object and take care of interfacing with Python through user input."
    def execute(self,d,s=""):
        print eval(s);
        app.processEvents()
        return 0
    def gui(self,d,s=""):        
        d.nv(maxnetlevel)
        d.globals["guifuncs"].DBGContinueButtonPressed=False
        while not d.globals["guifuncs"].DBGContinueButtonPressed:
            app.processEvents()
        return 0

dbg=hfPyDBGObject()

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
    
def object_PyDBG(self):
    "Returns the Python object which interfaces between the Data object and Python."
    return self.getStoredPyDBGObject()
    
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


def object_setFunc(self,funcname="Copy",library="",functype=TYPE.UNDEF):
    "Assigns a function to a data object and returns it"
    self.setFunction(funcname,library,functype)
    return self

#self //= other  - deletes the other objects but keeps the links to its neighbour
def object_ifloordiv(self,other): 
    if type(other)==str: return self.erase(self[other])
    return self.erase(other)

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
#    obj.touch()
#    self.touch()
    return obj

def object_invert(self):  # delObject
    "~data deletes an object."
    self.delObject()

def object_delObject_silent(self):  
    "~data deletes an object."
    self.Silent(True)
    self.delObject()

def object_resetLink(self,value,*args):
    "This will create a link between self and the other object (value).\
    If the link already exists it will be overwritten with the \
    current parameters or remain unchanged if the parameters are \
    the same. Parameters are those of SetLink, i.e. the other\
    object, followed by the the type of connection (PUSH/PULL) and \
    the direction (DIR.TO/DIR.FROM). If there are two DataLists a link\
    will be created between each corresponding element. If there is\
    one object and a DataList, there will be a link between the object\
    and each element in the DataList."
    if (type(value)==DataList):  ## This means Object >> DataList 
        if len(value)==1: return self.resetLink_cc(value.unList(),*args) # here we convert a list to a single object and try again.
        return DataList(map(lambda d:self.resetLink_cc(d,*args),value)) # here link the single object value with every element of value
    return self.resetLink_cc(value,*args)

def object_xxshift(self,pre,other,dir):
    "Used to overload the shift operators. Is called by functions like object_rrshift etc. The >> or << operators can be used to connect two data objetcs or a dataobject and a string or tuple. In the latter case a new object is created with the name of the string the data object links to. The Tuple contains the name as first element and then additional initialization parameters, e.g. a value to be put in, a function (bracketed by _f()), or a NetLevel (use _l(level))"
    if type(other) in [tuple,list,DataParameterList]:
        newobj=self.newObject(pre+other[0],dir).initializeObject(other)
        return newobj
    elif type(other)==Data:    # if both arguments are data objects, then only set a link
        other.setLink(self,dir)
#        other.touch()
        return other
    elif (type(other)==DataList) | (type(other)==DataUnionClass):
        for d in other:
            d.setLink(self,dir)
#        other.touch()
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

def object_getDOT(self,maxnetlevel,details=1,reverse=False,highlight=False):
    "Returns the network connections of the object in a form suitable for exporting it in the DOT language format (www.graphviz.org)"
    name=DOTname(self)
    typ=type2str(self.getType())
    shape="egg"
    fontname="Times"
    fontsize=10
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
    if fontsize>0: 
        props+=",fontname="+fontname
        props+=",fixedsize=true"
        props+=",height="+str(0.8*fontsize/10.)
        props+=",width="+str(1.4*fontsize/10.)
        props+=",fontsize="+str(fontsize)
    if highlight: 
        props+=",style=bold"
        color="blue"
    props+=",shape="+shape
    props+=",color="+color
    out = name +" ["+props+"];"
    if not reverse:
        l=IntVec(); dirs=DirVec(); modflags=StringVec(); names=StringVec();
        self.getNeighboursList(DIR.TO,maxnetlevel,l,names,dirs,modflags)
        for i in range(len(l)):
            if len(out)>0:
                out+="\n"
                arrowhead="normal"
                arrowcolor="red"
                if dirs[i]==PULL: arrowhead="inv"
                if modflags[i]=="NULL.0": arrowcolor="black"
                out += name + " -> " + DOTname(self[l[i]])+"[arrowhead="+arrowhead+",color="+arrowcolor+"];"
    else:
        l=IntVec(); dirs=IntVec(); modflags=StringVec(); names=StringVec();
        self.getNeighbourList(DIR.FROM,maxnetlevel,l,names,dirs,modflags)
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
    if self.isSingleDataObject(): l=self.All()
    else: l=self
    myself=self.FirstObject()
    if destination=="FILE": 
        name=myself.getName()
        fn=name+".dot" 
        OUT=hfWrite_File(fn)
        OUT.write("#dot -Tgif "+fn+" -o "+name+".gif; xview "+name+".gif\n")
        OUT.write("#dotty "+fn+"\n")
        OUT.write("#/Applications/Graphviz.app/Contents/MacOS/Graphviz  ROOT.dot "+fn+"\n")
    elif destination=="Qt": 
        OUT=hfWrite_QByte()
    elif destination=="Str": 
        OUT=hfWrite_Str()
    else: 
        print "Error - object_exportDOT: Destination",destination,"unknown!"
        return
    OUT.write('digraph G {\n')
#   OUT.write('digraph G {\nsize="8.25,11.6"; ratio=compress\n')
#    OUT.write('digraph G {\nsize="8.25,11.6"; ratio=fill;\n')
    map(lambda d:OUT.write(d.getDOT(maxnetlevel,details,reverse,myself==d)+"\n"),l)
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

def object_nview(self,maxnetlevel=999,tofile=False):
    "Allows one to display the network an object is embedded in, in a QtWidget. self is the object, and maxnetlevel says how much detail is to be shown - increases in steps of powers of ten: 9,99,999,9999"
    if tofile:
        fn=self.exportDOT("FILE",maxnetlevel)
        print "Dot-file written to ",fn," use GraphViz to view. The following command will only work on MacOS in its current implementation."
        os.system("open "+fn+" -a /Applications/Graphviz.app") 
        return
    docreate=True
    if ("qsvg" in self.FirstObject().globals): docreate=type(self.FirstObject().globals["qsvg"])!=QtSvg.QSvgWidget
    if docreate:
        self.FirstObject().globals["qsvg"]=QtSvg.QSvgWidget()
        self.FirstObject().globals["qsvg"].resize(800,600)
        self.FirstObject().globals["qsvg"].raise_()
        self.FirstObject().globals["qsvg"].setWindowTitle(self.FirstObject().getName()+" Network Display")
    if not self.FirstObject().globals["qsvg"].isVisible(): self.FirstObject().globals["qsvg"].show()
    self.FirstObject().globals["qsvg"].load(object_net2qb(self,maxnetlevel))

def object_graphviz(self,maxnetlevel=999): 
    "Allows one to view the network in GraphViz - launched as an external application."
    self.nview(maxnetlevel,True)


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
Data.delObject_silent=object_delObject_silent  
Data.__rrshift__=object_rrshift   #newobject+setlink  >> obj
Data.__rshift__=object_rshift     #newobject+setlink  obj >>
Data.__rlshift__=object_rlshift   #newobject+setlink  <<  obj
Data.__lshift__=object_lshift     #newobject+setlink  obj <<
Data.__setitem__=object_setitem
Data.__getitem__=object_getitem
Data.move=object_move
Data.put_silent=object_set_silent
Data.set_silent=object_set_silent
Data.__floordiv__=object_floordiv # // - dellink
Data.__ifloordiv__=object_ifloordiv #self //= other   - erase and keep link
Data.__xor__=insert_object        #d ^ (d1,d2) - insert 
Data.new=object_new
Data.initializeObject=initializeObject
Data.netsvg=object_net2qb

Data.getitem=object_getitem
Data.setFunc=object_setFunc
Data.set=object_set
Data.val=object_val
Data.getIV=object_getIV
Data.getNV=object_getNV
Data.getCV=object_getCV
Data.getSV=object_getSV
Data.getIL=object_getIL
Data.getNL=object_getNL
Data.getCL=object_getCL
Data.getSL=object_getSL
Data.setFunc_f=object_setFunc_f
Data.setFunc_f_silent=object_setFunc_f_silent
Data.getDOT=object_getDOT
Data.exportDOT=object_exportDOT
Data.nview=object_nview
Data.nv=object_nview

def datalist(self,maxnetlevel=9999,printdeletedobjects=False):
    self.printAllStatus(1,maxnetlevel,printdeletedobjects)

#Used for determining whether a search was successful If not, an
#DataList will be returned that returns true and 0 for these
#functions.
Data.notFound=returnFalse
Data.Found=returnTrue
Data.NFound=returnOne

Data.isSingleDataObject=returnTrue
Data.isDataList=returnFalse
Data.list = datalist
Data.ls = datalist
Data.stored_pyfuncobj={}
Data.stored_pyqtobj={}
Data.getStoredPyFuncObject=getStoredPyFuncObject
Data.setStoredPyFuncObject=setStoredPyFuncObject
Data.getStoredPyQtObject=getStoredPyQtObject
Data.setStoredPyQtObject=setStoredPyQtObject
Data.getStoredPyDBGObject=getStoredPyDBGObject
Data.setStoredPyDBGObject=setStoredPyDBGObject
Data.PyFunc=Data.retrievePyFuncObject
Data.activatePyQt=object_activatePyQt
Data.hasPyQt=object_hasPyQt
Data.hasPyDBG=object_hasPyDBG
Data.connect=object_connect
Data.PyQt=object_PyQt
Data.PyDBG=object_PyDBG
Data.SIGNAL=object_SIGNAL
Data.SLOT=object_SLOT
Data.getT=object_getT
Data.getB=object_get_bool
Data.getQ=object_get_QString
Data.All=object_all
Data.search=object_search
Data.find_or_make=object_find_or_make
Data.Chain=object_Chain
Data.resetLink=object_resetLink
Data.LastObject=Identity
Data.FirstObject=Identity
Data.Tail=returnEmptyDatalist
Data.unList=Identity
Data.setList=object_setList
Data.asList=object_asList
Data.getNeighbours=object_getNeighbours
Data.getNeighbour=object_getNeighbour
Data.asDataList=object_asDataList
DataList.asDataList=Identity

#This isued to store some global shortcuts to Qt objects and GUIs that are used by all objects
Data.globals={}


def debugon(self,level=9999):
    self.All().storePyDBG(dbg).setVerbose(level).setDebug(True)

def debugoff(self):
    self.All().setVerbose(0).setDebug(False)
Data.debugon=debugon
Data.debugoff=debugoff

def debugguion(self,level=9999):
    self.All().storePyDBG(dbg).setVerbose(level).setDebugGUI(True).setDebug(False)

def debugguioff(self):
    self.All().setVerbose(0).setDebugGUI(False)
Data.debugguion=debugguion
Data.debugguioff=debugguioff

def verboseon(self,level=9999):
    self.All().storePyDBG(dbg).setVerbose(level)

def verboseoff(self):
    self.All().setVerbose(0)

Data.verboseon=verboseon
Data.verboseoff=verboseoff


DataList.unique=datalist_unique
DataList.exportDOT=object_exportDOT
DataList.nview=object_nview
DataList.nv=object_nview
DataList.ls=DataList.datalist

Data.gv=object_graphviz
DataList.gv=object_graphviz
