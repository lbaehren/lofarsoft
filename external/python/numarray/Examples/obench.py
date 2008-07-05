import timing
import compiler
import Numeric
import numarray.objects 
import sys
import string
import re
import profile
import hotshot
import hotshot.stats

import numarray.arrayprint

numarray.arrayprint.summary_off()

params = """a=array(["1"]*10**i,typecode='O') b=array(["2"]*10**i,typecode='O') c=arange(10**i) d=array("this") e=array(range(10**i),typecode='O')
"""

benchmarks = """
array(c,typecode='O')
a.tolist()
setitem(a)
getitem(a)
a+b
a==b
a[:]=b
a[::2]
a.copy()
repr(a)
choose(arange(10**i)%2,(a,b))
sort(a)
argsort(a)
take(a,c)
# put(a,c,d)    Numeric-23.0 crashes here...
add(a,b)
add.reduce(a)
add.accumulate(a)
abs(e)
"""

def setitem(a):
    for i in xrange(len(a)):
        a[i] = 0

def getitem(a):
    for i in xrange(len(a)):
        j = a[i]

def dummy(*args):
    pass

MIN_TIME_RESOLUTION = 0.100

def otime(gs, ls, N):
    timing.start()
    for i in range(N):
        pass
    timing.finish()
    return timing.micro()
    
def time(bmark, gs, ls, N=1, force_exit=0):
    code = compiler.compile(bmark,"benchmarks","exec")
    try:
        timing.start()
        for i in range(N):
            exec(code, ls, gs)
        timing.finish()
        t = timing.micro()/10.0**6
        if t > MIN_TIME_RESOLUTION or force_exit:
            return t/N  # (t - otime(gs, ls, N))/N
        else:
            return time(bmark, gs, ls, N*2)
    except:
        return None

def run(num, bmark, p):
    locals = {}
    locals["setitem"] = setitem
    locals["getitem"] = getitem
    for i in string.split(p," "):
        exec i in num.__dict__, locals
    t = time(bmark, locals, num.__dict__)
    # print num.__name__, bmark, t
    return t

"""
vector scalar Float32/Float32 add
add(a1,1.0)
   Overhead in us [ratio]    per element time (ns) [ratio]
        200       [22.1]          80               [0.5]

"""

def report(line):
    p0 = "i=0 " + params # 10**0 element case
    p5 = "i=4 " + params # 10**5 element case

    i=4
    count = 10**i

    bmark = line.split(" ")[0]
    
    m0 = run(Numeric, bmark, p0)
    n0 = run(numarray.objects, bmark, p0)
    m5 = run(Numeric, bmark, p5)
    n5 = run(numarray.objects, bmark, p5)

    if m0 and m0:
        ne = ((n5-n0)/count*10**9)
        me = ((m5-m0)/count*10**9)
        print line
        print "%20s%8s%20s%8s" % ("overhead (usec)","[ratio]","per element (nsec)","[ratio]")
        try:
            r1 = n0/m0
        except:
            r1 = 0
        try:
            r2 = ne/me
        except:
            r2 = 0
        print "%20d%8.1f%20d%8.1f" % (n0*count, r1, ne, r2)
    elif n0:
        ne = ((n5-n0)/count*10**9)
        print bmark
        print "%20s%8s%20s%8s" % ("overhead (usec)","[ratio]","per element (nsec)","[ratio]")
        print "%20d%8s%20d%8s" % (n0*count, None, ne, None)


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

def benchmark(pattern=None, f=report):
    for m in marks(pattern):
        if m:
            f(m)

def prof_do(mark, m, verbose=1):
    pr = hotshot.Profile("prof.out", 1, 1)
    # pr.calibrate(100, verbose=1)
    pr.run(mark)
    if verbose:
        print "profile for: ", m
    pr.close()
    pr = hotshot.stats.load("prof.out")
    pr.strip_dirs().sort_stats("time")
    pr.print_stats()

def prof(m, verbose=1):
    globals().update(numarray.objects.__dict__)
    globals()["i"] = 4
    for p in params.split():
        exec p in globals()
    line = m.split(" ")
    mark, comment = line[0], " ".join(line[1:])
    prof_do(mark, m, verbose)

def profall(pattern=None):
    for m in marks(pattern):
        if m:
            print "-"*70
            prof(m)
            prof(m + " (cached)")

if __name__ == "__main__":
    if len(sys.argv) == 1:
        benchmark()
    else:
        if sys.argv[1] == "prof":
            for p in sys.argv[1:]:
                profall(p)
        elif "times" == sys.argv[1]:
            for b in sys.argv[1:]:
                benchmark(b, f=case)
        else:
            for b in sys.argv[1:]:
                benchmark(b, f=report)
