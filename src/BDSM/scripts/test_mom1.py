

import numpy as N
import functions as func

x, y = N.indices((100,100))
image = N.exp(-0.5*((x-50.)*(x-50.)+(y-53.)*(y-53.))/(9.5*9.5))

for i in range(0, 40, 2):
  mask = N.ones(image.shape, bool)
  mask[51-3-i:51+3+i+1,53-3-i:53+3+i+1] = False
  mom = func.momanalmask_gaus(image, mask, 0, 1.0, True)
  print i+3, mom[3]/2.35482, mom[4]/2.35482




