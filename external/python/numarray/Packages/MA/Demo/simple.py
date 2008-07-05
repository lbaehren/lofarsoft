from MA import *
x=[1.0, 2.0, 1.e20, 4.0]
y = masked_values(x, 1.e20)
print "y", y
print "filled(y, 0.)", filled(y, 0.)
print "y+y", y+y
