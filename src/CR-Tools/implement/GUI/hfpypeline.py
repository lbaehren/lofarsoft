execfile("hftools.py")


def demo(descr,code):
    print descr
    print code,
    exec (code)

n=IntVec()
v1=FloatVec()
v1=FloatVec()
v1.extend([1,2,3,4,0,5])
v2=FloatVec()
v2.resize(5)
n.resize(7)
print v1
hNegate(v1)
print v1
hAbs(v1)
print v1
hNegate(v1)
print v1
hAbs(v1,v2)
print v2
v2+=v2
print v2
v2+=3
print v2

v3=FloatVec()
v3.resize(10)
v2.mul(3.5,v3)
print v2,v3
