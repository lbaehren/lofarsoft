import libhftools as ht

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

ht.FloatVec.__repr__=VecToPrintString                
ht.IntVec.__repr__=VecToPrintString                
ht.BoolVec.__repr__=VecToPrintString                
ht.ComplexVec.__repr__=VecToPrintString                
ht.StringVec.__repr__=VecToPrintString

v=ht.FloatVec()
v.extend([1,2,3,4,0,5])
v2=ht.FloatVec()
v2.extend([0,0,0,0,0])
print v
ht.hNegate(v)
print v
ht.hAbs(v)
print v
ht.hNegate(v)
print v
ht.hAbs(v,v2)
print v2

