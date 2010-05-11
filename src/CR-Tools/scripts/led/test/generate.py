#! /usr/bin/env python

import sys
import time
import random

random.seed()

t=251919456188449696.0
while True:
    dt=random.random()*5

    print "{0:18.0f}".format(t), random.randint(0,365), random.randint(0,300)

    sys.stdout.flush()

    time.sleep(dt)

    t+=dt*1e9/5

