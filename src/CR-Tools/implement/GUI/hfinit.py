pyoblist=[]

PUSH=DIR.TO
PULL=DIR.FROM


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



class DataList(list):
    def datalist(self): return
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
        for d in self:
            d.update()
        return self

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
            return None
        elif len(ids)==1:
            return self.Object(ids[0])
        else:
            l=list()
            for i in ids:
               l.append(self.Object(i))
            return DataList(l)

def object_set_list(self,value):
    if hasattr(self,"datalist"):
        for d in self:
            object_set(d,value)
    else:
            object_set(self,value)

def object_set_silent(self,value):
    sil=self.Silent(True)
    object_set(self,value)
    self.Silent(sil)
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
    else:
        self.putPy(value)
    return self

def object_setitem(self,key,value):
    "Used to overload the assign operation for a data object with [] to set the content of the vector."
    return object_set_list(self[key],value)

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
        
def getStoredPyObject(self):
    return self.stored_pyobj[self.getOid()]

def setStoredPyObject(self, pyob):
    self.stored_pyobj[self.getOid()]=pyob

def object_setFunc_f_silent(self,f):
    "Assining a function using the _f class as input but not causing an update."
    sil=self.Silent(True)
    if type(f.functype)==TYPE:
        self.setFunc(f.funcname,f.library,f.functype)
    elif f.library=="Py": 
#       In this case we have specified a Python function
        self.setStoredPyObject(f.functype())
#        print self.getStoredPyObjecdt()
#        print type(self.getStoredPyObject())
        pyobj=self.newObject("'PythonObject",PUSH)
        pyobj.putPy_silent(self.getStoredPyObject())
        pyobj.setNetLevel(1000)
        self.setFunc(f.funcname,f.library,TYPE.STRING)
    self.Silent(sil)
    return self

def object_setFunc_f(self,f):
    "Assining a function using the _f class as input."
    if type(f.functype)==TYPE:
        self.setFunc(f.funcname,f.library,f.functype)
    elif f.library=="Py": 
#       In this case we have specified a Python function
        self.setStoredPyObject(f.functype())
#        print self.getStoredPyObject()
#        print type(self.getStoredPyObject())
        pyobj=self.newObject("'PythonObject",PUSH)
        pyobj.putPy(self.getStoredPyObject())
        pyobj.setNetLevel(1000)
        self.setFunc(f.funcname,f.library,TYPE.STRING)
    return self


def object_setFunc(self,funcname="Copy",library="Sys",functype=TYPE.UNDEF):
    "Assigns a function to a data object and returns it"
    self.setFunction(funcname,library,functype)
    return self

#self // other  - deltes a link in the to direction
def object_floordiv(self,other): 
    "data // data deletes a link between two objects in eitehr FROM or TO direction. The function returns the first data object."
    if type(other)==str:
        self.delLink(self["other"])
        self["other"].touch()
    elif type(other)==Data:
        self.delLink(other)
        other.touch()
    self.touch()
    return self

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



Data.__repr__=object_print
Data.__invert__=object_invert  #~data = delObject
Data.__rrshift__=object_rrshift   #newobject+setlink
Data.__rshift__=object_rshift     #newobject+setlink
Data.__rlshift__=object_rlshift   #newobject+setlink 
Data.__lshift__=object_lshift     #newobject+setlink
Data.__setitem__=object_setitem
Data.__getitem__=object_getitem
Data.put_silent=object_set_silent
Data.__floordiv__=object_floordiv # // = dellink
Data.__xor__=insert_object        #d ^ (d1,d2) = insert 
Data.initializeObject=initializeObject
Data.netsvg=object_net2qb

Data.getitem=object_getitem
Data.setFunc=object_setFunc
Data.setFunc_f=object_setFunc_f
Data.setFunc_f_silent=object_setFunc_f_silent
Data.getDOT=object_getDOT
Data.exportDOT=object_exportDOT
Data.nview=object_nview
Data.nv=object_nview

def datalist(self,maxnetlevel=9999):
    self.printAllStatus(1,maxnetlevel)

Data.list = datalist
Data.ls = datalist
Data.stored_pyobj={}
Data.getStoredPyObject=getStoredPyObject
Data.setStoredPyObject=setStoredPyObject

#Extending mathgl to accept STL vectors 
def mglDataSetVec(self,vec):
    mglDataSet(self,vec)

mglData.SetVec = mglDataSetVec


mglGraphZB=0
mglGraphPS=1
mglGraphGL=2
mglGraphIDTF=3
