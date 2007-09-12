import sys, os

fortran = sys.stdout.fileno()

def fwrite(*args):
    s = " ".join(map(str, args))
    os.fstat(fortran)  # stat does an fsync and accepts an integer fd
    os.write(fortran, s+"\n")
    os.fstat(fortran)

def dump(a):
     fwrite(str(a))

def cr():  
    fwrite("\n");

def in_f(a, b):
    fwrite("foo.in_f:");
    ra = sys.getrefcount(a)
    foo.in_f(a);
    if ra != sys.getrefcount(a):
        fwrite("ref count error:", ra, sys.getrefcount(a))
               

def in_c(a, b):
    fwrite("foo.in_c:"); 
    ra = sys.getrefcount(a)
    foo.in_c(a); 
    if ra != sys.getrefcount(a):
        f.write("ref count error:", ra, sys.getrefcount(a))

def in_comma_out_f(a, b):
    fwrite("foo.in_comma_out_f:")
    ra = sys.getrefcount(a)
    rb = sys.getrefcount(b)
    a1, b1 = foo.in_comma_out_f(a, b); cr()
    dump(a1)
    dump(b1)
    if ra != sys.getrefcount(a):
        fwrite("a ref count error:", ra, sys.getrefcount(a))
    if rb != sys.getrefcount(b):
        fwrite("b ref count error:", rb, sys.getrefcount(b))        

def in_comma_out_c(a, b):
    fwrite("foo.in_comma_out_c:")
    ra = sys.getrefcount(a)
    rb = sys.getrefcount(b)
    a1, b1 = foo.in_comma_out_c(a, b); cr()
    dump(a1)
    dump(b1)
    del a1
    del b1
    if ra != sys.getrefcount(a):
        fwrite("a ref count error:", ra, sys.getrefcount(a))
    if rb != sys.getrefcount(b):
        fwrite("b ref count error:", rb, sys.getrefcount(b))


def inout_f(a, b):
    fwrite("foo.inout_f:")
    # The following copy is necessary because the strided array
    # output for Numeric... isn't strided.
    a, b = a.copy(), b.copy()
    ra = sys.getrefcount(a)
    rb = sys.getrefcount(b)
    # print a.iscontiguous(), b.iscontiguous()
    a = array.transpose(a)
    b = array.transpose(b)
    # print a.iscontiguous(), b.iscontiguous()
    foo.inout_f(a, b); cr()
    if ra != sys.getrefcount(a):
        fwrite("ref count error:", ra, sys.getrefcount(a))
    if rb != sys.getrefcount(b):
        fwrite("ref count error:", rb, sys.getrefcount(b))
    dump(a);
    dump(b)

def inout_c(a, b):
    fwrite("foo.inout_c:")
    ra = sys.getrefcount(a)
    rb = sys.getrefcount(b)
    foo.inout_c(a, b); cr()
    if ra != sys.getrefcount(a):
        fwrite("ref count error:", ra, sys.getrefcount(a))
    if rb != sys.getrefcount(b):
        fwrite("ref count error:", rb, sys.getrefcount(b))
    dump(a);
    dump(b)


def out_c(a, b):
    rb = sys.getrefcount(b)
    b = foo.out_c(b, b.shape[0], b.shape[1])
    if rb != sys.getrefcount(b):
        fwrite("ref count error:", rb, sys.getrefcount(b))
    dump(b)

def out_f(a, b):
    rb = sys.getrefcount(b)
    b = foo.out_f(b, b.shape[0], b.shape[1])
    if rb != sys.getrefcount(b):
        fwrite("ref count error:", rb, sys.getrefcount(b))
    dump(b)


def standard():
    a = array.arange(80.)
    a.shape = (8,10)
    b = a.copy()
    return a,b 

def byteswapped():
    a = array.arange(80., shape=(8,10))
    a.byteswap()
    a.togglebyteorder()
    b = array.arange(80., shape=(8,10))
    b.byteswap()
    b.togglebyteorder()
    assert a.isbyteswapped()
    assert b.isbyteswapped()
    return a,b

def misaligned():
    a0 = array.arange(80., shape=(8,10))
    a  = array.NumArray(shape=(90,), type='Float64')
    a._shape = (8,10)
    a._strides = (80, 8)
    a._byteoffset = 1
    a[:]  = a0
    b  = array.NumArray(shape=(90,), type='Float64')
    b._shape = (8, 10)
    b._strides = (80, 8)
    b._byteoffset = 1
    b[:]  = a0
    assert not a.isaligned()
    assert not b.isaligned()
    return a, b

def strided():
    a0 = array.arange(80., shape=(8,10))
    a = array.arange(320., shape=(32,10))[::4,:]
    a[:] = a0
    b = array.arange(320., shape=(32,10))[::4,:]
    b[:] = a0
    assert not a.iscontiguous()
    assert not b.iscontiguous()
    return a, b

def main(array_pkg, behavior, kind):
    if array_pkg == "Numeric":
        import Numeric as array0, Numeric_foo as foo0
    else:
        import numarray as array0, numarray_foo as foo0
    global array, foo
    array = array0
    foo = foo0
    kind_f     = eval(kind)
    a, b = eval(behavior+"()")
    kind_f(a, b)    

if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2], sys.argv[3])
