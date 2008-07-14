
#python -i hfget.py
#or in python
#execfile("hfget.py")


def pvec(v):
    "Print a vector"
    for i in v:
        print i


from hfget import *
d=Data("LOFAR")
d.printStatus(0)
v=d.newObjects("Header",DIR.FROM)
v=d.newObjects("Header:X",DIR.FROM)
len(v)
d.printDecendants()
DIR.TO+1

v2=IntVec()
v2[:]=[0,1,2,3,4,5,6,7]
v2[3:4]=[8,9]
d["X"].put(v2)
v3=FloatVec()
d[2].get(v3)
d2=d.Object("X")


v=d.newObjects("Header:Range",DIR.FROM)
v=d.newObjects("Header:Range:end",DIR.FROM)
v[:]=[11]
d["end"].put(v)
d["end"].printStatus()
d["Range"].setFunction("Range",TYPE.INTEGER)
d["Range"].printStatus()
v4=IntVec()
vi=IntVec()
vf=FloatVec()
vc=ComplexVec()
vs=StringVec()
d.Object("Range").get(vi)
d["Range"].get(v4)
d["end"].printStatus()



v3=FloatVec()
d[2].get(v3)
d2=d.Object("X")


v=d.newObjects("Header:Range",DIR.FROM)


v[:]=[11]
d["end"].put(v)
d["end"].printStatus()
d["Range"].printStatus()
v4=IntVec()
d.Object("Range").get(vi)
d["Range"].get(v4)
d["end"].printStatus()