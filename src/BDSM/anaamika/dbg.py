from pylab import *
from matplotlib.patches import Ellipse, Circle
a = subplot(111, aspect='equal')
el1 = Ellipse((483.79017284 , 748.11867995), 12.10966850180224, 3.7955465792446432, 229.00677968078799)
el2 = Ellipse(( 483.84242161 , 741.96593175), 5.8200529903897156, 3.5940088240787582, 131.169788560365248)
circ1 = Circle((483.79017284 , 748.11867995), 4.707)
ells = [el1, el2, circ1]

for e in ells:
    e.set_clip_box(a.bbox)
    e.set_alpha(0.1)
    a.add_artist(e)


xlim(475.0, 490)
ylim(740.0, 755.0)

figure()
a = subplot(111, aspect='equal')
el1 = Ellipse((0.0,0.0), 12.10966850180224, 3.7955465792446432, 0.0)
ells = [el1]

for e in ells:
    e.set_clip_box(a.bbox)
    e.set_alpha(0.1)
    a.add_artist(e)

xlim(-7.0, 7.0)
ylim(-4.0,4.0)

show()



