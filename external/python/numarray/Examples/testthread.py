import threading
import numarray
import random
import sys

class computer(threading.Thread):
    def __init__(self, name=None, target=None, args=(), kwargs={}):
        threading.Thread.__init__(self, name=name, target=target,
                                  args=args, kwargs=kwargs)
        self.args = args
        self.kwargs = kwargs
        self.setDaemon(1)

    def run(self):
        x = self.args[0]*self.args[0]
        for i in range(10000):
            SIZE = 10**int(random.random()*3)
            a = numarray.arange(1, SIZE*SIZE+1, shape=(SIZE,SIZE))
            a.transpose()
            b = a.copy()
            a = a * x
            assert numarray.logical_and.reduce(numarray.ravel((a / x) == b))
            # if (i % 100 == 0):
            print self.getName(),
            sys.stdout.flush()

my_threads = []
for i in xrange(10):
    print "creating", i
    my_threads.append(computer(name="t%d" % i, args=(i+1,)))

for i in xrange(10):
    print "starting", i
    my_threads[i].start()

