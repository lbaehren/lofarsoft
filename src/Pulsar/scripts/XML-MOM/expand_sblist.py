#!/usr/bin/env python

import sys
import numpy

sb=numpy.array([])
for line in sys.stdin:
        sline = line.split(',')
        for srange in sline:
                num = srange.split('..')
                sb=numpy.append(sb,numpy.arange(int(num[0]),int(num[1])+1))

output='%d'%sb[0]
for n in sb[1:]:
        output+=',%d'%n

print output
