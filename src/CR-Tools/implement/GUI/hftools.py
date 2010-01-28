from libhftools import *

#Some definitons to make pretty (and short) output of vectors
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


hRealVectorTypes=[IntVec,FloatVec]
hNumericalVectorTypes=hRealVectorTypes+[ComplexVec]
hOtherVectorTypes=[BoolVec,StringVec]
hAllVectorTypes=hOtherVectorTypes+hNumericalVectorTypes

#Operator Overloading
def Vec_iadd(vec1,vec2):
    "Provides the += operator for adding two vectors in place. \
    vec1+=vec2 will add all elements in vec2 to the corresponding elements in vec1\
    and store the result in vec1."
    hiAdd(vec1,vec2)
    return vec1

def Vec_isub(vec1,vec2):
    "Provides the -= operator for addig two vectors in place. \
    vec1-=vec2 will subtract all elements in vec2 from the corresponding elements in vec1\
    and store the result in vec1."
    hiSub(vec1,vec2)
    return vec1

def Vec_idiv(vec1,vec2):
    "Provides the /= operator for addig two vectors in place. \
    vec1/=vec2 will divide all elements in vec1 by the corresponding elements in vec2\
    and store the result in vec1."
    hiDiv(vec1,vec2)
    return vec1

def Vec_imul(vec1,vec2):
    "Provides the *= operator for addig two vectors in place. \
    vec1=vec2 will multiply all elements in vec1 with the corresponding elements in vec2\
    and store the result in vec1."
    hiMul(vec1,vec2)
    return vec1

def Vec_add(vec1,val):
    "Provides the + operator for adding two vectors or a vector and a scalar. The result will be a new vector."
    vecout=vec1.new()
    hAdd(vec1,val,vecout)
    return vecout

def Vec_sub(vec1,val):
    "Provides the - operator for subtracting two vectors or a vector and a scalar. The result will be a new vector."
    vecout=vec1.new()
    hAdd(vec1,val,vecout)
    return vecout

def Vec_mul(vec1,val):
    "Provides the * operator for multiplying two vectors or a vector and a scalar. The result will be a new vector."
    vecout=vec1.new()
    hAdd(vec1,val,vecout)
    return vecout

def Vec_div(vec1,val):
    "Provides the / operator for dividing two vectors or a vector by a scalar. The result will be a new vector."
    vecout=vec1.new()
    hAdd(vec1,val,vecout)
    return vecout

setattr(ComplexVec,"SpectralPower",hSpectralPower)

#for v in hNumericalVectorTypes:
#    setattr
for v in hAllVectorTypes:
    setattr(v,"__repr__",VecToPrintString)
    for s in ["hResize","hFill","hNew","hCopy"]:     
        setattr(v,s[1:].lower(),eval(s))

for v in hRealVectorTypes:
    for s in ["hAcos","hAsin","hAtan","hCeil","hFloor"]:     
        setattr(v,s[1:].lower(),eval(s))

for v in hNumericalVectorTypes:
    setattr(v,"__add__",Vec_add)
    setattr(v,"__sub__",Vec_sub)
    setattr(v,"__div__",Vec_div)
    setattr(v,"__mul__",Vec_mul)
    setattr(v,"__iadd__",Vec_iadd)
    setattr(v,"__imul__",Vec_imul)
    setattr(v,"__idiv__",Vec_idiv)
    setattr(v,"__isub__",Vec_isub)
    for s in ["hAbs","hConvert","hMul","hDiv","hSub","hAdd","hiMul","hiDiv","hiSub","hiAdd","hCos","hCosh","hExp","hLog","hLog10","hSin","hSinh","hSqrt","hSquare","hTan","hTanh","hSum","hMean","hSortMedian","hMedian","hStdDev","hDownsample"]:
        setattr(v,s[1:].lower(),eval(s))

