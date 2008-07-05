import compiler
import Numeric
import numarray
import numarray._numarray as _numarray
import numarray.memory as memory
import numarray._ufunc as _ufunc
import sys
import string
import re
import profile
import hotshot
import hotshot.stats
import timeit

from optparse import OptionParser

parameters = """
p=int(10.0**i); a=arange(p); j=int(10.0**(i/4.0)); b=reshape(arange(j*j),(j,j));   c=a.copy(); d=a.copy(); e=transpose(b); f=transpose(b); af=a.astype('d'); cf=a.astype('d'); z=array(0); w=array(p); t=[1]*p;
"""

param_funcs = ["arange", "array", "reshape", "transpose"]

benchmarks = """
take(a,c)
put(a,c,d)
transpose(b)
putmask(a,c%2,d)
repeat(a,2)
choose(a%2,(a,c))
ravel(b)
nonzero(a)
where(a%2,a,c)
compress(a%2,a)
diagonal(b)
trace(b)
searchsorted(a,a)
searchsorted(a,w/5)
searchsorted(a,w/2)
sort(a)
sort(a,kind='quicksort')
sort(a,kind='mergesort')
sort(a,kind='heapsort')
argsort(a)
argsort(a,kind='quicksort')
argsort(a,kind='mergesort')
argsort(a,kind='heapsort')
argmax(b)
array(t)
array(t,typecode='i')
dot(b,b)
innerproduct(b,b)
clip(a,50,100)
indices((100,100))
swapaxes(b,0,-1)
concatenate((a,a,a))
innerproduct(b,b)
outerproduct(b[0],b[0])
resize(a,(int(2*10**i),))
a[0]                                   # single element
b[0]                                   # single row
b[0,0]                                 # multi-index single element
b[0][0]                                # multi-index by view
a[1000:2000]                           # 1D slice
b[400:600,400:600]                     # 2D slice
arange(int(10.**i))
identity(int(i+2))
a+c
a+1
a+=c
a+=1
a<c
a==c
a<1
a!=1
add(a,c)
add(a,c,d)
add(a,1)
add(a,1,d)
add(b,e)
cos(a)
add.reduce(a)
add.reduce(b,0)
add.reduce(b,-1)
add.accumulate(a)
add.accumulate(b,0)
add.accumulate(b,1)
zeros((int(10**i),))
ones((int(10**i),))
"""

parameters = parameters.strip()

def run(benchmark, *args):
    _setup = "; ".join(args)
    # print _setup
    t = timeit.Timer(benchmark, setup=_setup)
    number = 1
    repeats = 3
    return (min(t.repeat(repeats,number))/number)*10.0**6

def report(line,i,show_bench):
    bmark = line.split(" ")[0]
    function = bmark.split("(")[0]
    function = function.split(".")[0]
    if function == bmark:
        function = "array"

    Numeric_imp  = "from Numeric import "  + ", ".join(param_funcs+[function])
    numarray_imp = "from numarray import " + ", ".join(param_funcs+[function])
    
    try :
        m = run(bmark, Numeric_imp,  "i=%f" % i, parameters)
    except :
        m = 0
    n = run(bmark, numarray_imp, "i=%f" % i, parameters)

    if m and n:
        try:
            r = n/m
        except:
            r = 0
    elif n:
        r = 0
    else:
        raise RuntimeError("Invalid benchmark")

    if not show_bench:
        bmark = ""
        
    print "%-28s%5.1f%18d%18d%18.1f" % (bmark, i, n, m, r)

def marks(pattern):
    if pattern != None:
        l = []
        lines = string.split(benchmarks,"\n")
        for i in range(len(lines)):
            if re.match(pattern, lines[i]):
                l.append(lines[i])
    else:
        l = benchmarks.split("\n")
    return l

def benchmark(pattern=None, powers=[0,5]):
    print "%-28s10**x%18s%18s%18s" % ("benchmark","numarray (usec)","Numeric (usec)","numarray:Numeric")
    for m in marks(pattern):
        if m:
            if not isinstance(powers, (list,tuple)):
                powers = [powers]
            for p in powers:
                report(m, p, show_bench=(p is powers[0]))

def prof(m, i):
    print "profiling",m,"at power",i
    params = parameters.split()
    globals().update(numarray.__dict__)
    globals().update(locals())
    for p in params:
        exec p in globals()
    words = m.split()
    mark, comment = words[0], " ".join(words[1:])
    pr = hotshot.Profile("prof.out", 1, 1)
    pr.run(mark)
    pr.close()
    pr = hotshot.stats.load("prof.out")
    pr.strip_dirs().sort_stats("time")
    pr.print_stats()


def profall(pattern, powers):
    for m in marks(pattern):
        if m:
            if not isinstance(powers, (tuple,list)):
                powers = [powers]                
            for i in powers:
                print "-"*70
                prof(m, i)
                prof(m + " (cached)", i)

def ratio(f1name, f2name):
    "compares two bench outputs to see version to version changes"
    f1 = open(f1name)
    f2 = open(f2name)
    for l1 in f1.readlines():
        l2 = f2.readline()
        words1 = l1.split()
        words2 = l2.split()
        if words1[0] == "benchmark":
            print l1.strip(), "\t", f1name +" : "+ f2name
            continue
        ratio1 = eval(words1[-1])
        ratio2 = eval(words2[-1])
        change = ratio1/ratio2
        if change > 1.0:
            alert = "-"
        elif change < 1.0:
            alert = "+"
        else:
            alert = "."
        if not re.match("\d+", words1[0]):
            print l1.strip()
            print l2.strip(), "\t", alert, change
        else:
            print " "*29, l1.strip()
            print " "*29, l2.strip(), "\t", alert, change

def main():
    parser = OptionParser()
    parser.add_option("-p", "--powers", dest="powers",
                      help="powers of 10 to bencmark", metavar="POWERS",
                      default="[0,5.0]")
    (options, args) = parser.parse_args()

    POWERS = eval(options.powers)

    if len(args) == 0:
        benchmark(powers=POWERS)
    else:
        if sys.argv[1] == "prof":
            for b in sys.argv[1:]:
                profall(b, powers=POWERS)
        elif sys.argv[1] == "ratio":
            ratio(sys.argv[2], sys.argv[3])
        else:
            for b in args:
                benchmark(b, powers=POWERS)

if __name__ == "__main__":
    main()

