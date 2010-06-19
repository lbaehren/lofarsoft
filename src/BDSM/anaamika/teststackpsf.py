
import numpy as N

gxcens_pix=N.array([1138.7])
gycens_pix=N.array([1246.3])
cs2=10
cc = cutoutcen_ind=[cs2-0.5, cs2-0.5]

for i in range(len(gxcens_pix)):
   gcp=N.array([gxcens_pix[i], gycens_pix[i]])
   print 'gcp = ',gcp
   gcen_ind=gcp-1
   rc=rcen_ind = N.asarray(N.round(gcen_ind), dtype=int)
   print 'rc = ',rc
   shift=cc-(gcen_ind-(rc-cs2))
   print 'shift = ',shift
   print rc-cs2
   print gcen_ind
   print gcen_ind-(rc-cs2)
   print rc-cs2,rc+cs2
