import matplotlib as mpl
import matplotlib.pyplot as plt
import pycrtools as cr
import numpy as np

blocksize = 100
a = cr.hArray(float, blocksize)
b = a.new()
c = a.new()
d = a.new()

cr.hGetHanningFilterHalf(a, 1.0, 0.0, False)
cr.hGetHanningFilterHalf(b, 1.0, 0.0, True)
cr.hGetHanningFilterHalf(c, 1.0, 0.50, False)
cr.hGetHanningFilterHalf(d, 1.0, 0.50, True)

#cr.hGetHanningFilterHalf()

plt.close()

plt.title(r"Testing Hanning functions")
plt.xlabel("Index")
plt.ylabel("Value ")

plt.plot(a.toNumpy(),label=r"a: height=1.0, offset=0.0, falling=False")
plt.plot(b.toNumpy(),label=r"b: height=1.0, offset=0.0, falling=True")
plt.plot(c.toNumpy(),label=r"c: height=1.0, offset=0.5, falling=False")
plt.plot(d.toNumpy(),label=r"d: height=1.0, offset=0.5, falling=True")

plt.legend(loc='lower center')

plt.show()
