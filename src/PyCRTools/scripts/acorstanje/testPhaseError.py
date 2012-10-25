
_ip.magic("colors Linux")
from pycrtools import *
from pycrtools import srcfind as sf
f = open('/Users/acorstanje/triggering/CR/L28318_D20110613T051515.914Z_CS003_R000_tbb.h5')
pos = f["ANTENNA_POSITIONS"]
pos
pos = pos.toNumpy().ravel()
pos
deg2rad = np.pi * 2 / 360.0
sf.phasesFromDirection(200.0, 0.0)
sf.phasesFromDirection(pos, (200.0, 0.0))
sf.phasesFromDirection(pos, (200.0, 0.0), 30.0e6)
ph = Out[12]
sf.phaseError(200.0*deg2rad, 0.0, pos, ph, 30.0e6)
sf.phasesFromDirection(pos, (200.0*deg2rad, 0.0), 30.0e6)
ph = Out[15]
sf.phaseError(200.0*deg2rad, 0.0, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 0.1, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 0.5, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 0.2, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 0.1, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 0.2, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 0.2*deg2rad, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 1*deg2rad, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 5*deg2rad, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 10*deg2rad, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 7*deg2rad, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 7*deg2rad, pos, ph, 30.0e6)
sf.phaseError(201*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
sf.phaseError(0.1*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
sf.phaseError(200.1*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
sf.phaseError(200.1*deg2rad, 0*deg2rad, pos, ph+0.3, 30.0e6)
sf.phaseError(200.1*deg2rad, 0*deg2rad, pos, ph+3, 30.0e6)
sf.phaseError(200.1*deg2rad, 0*deg2rad, pos, ph+0.1, 30.0e6)
sf.phaseError(200.1*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
ph[0]
ph[1]
ph[2]
ph[0] += 0.01
sf.phaseError(200.0*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
sf.phaseError(200.0*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
ph[0] += 0.1
sf.phaseError(200.0*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
ph -= 0.11
sf.phaseError(200.0*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
ph += 0.11
ph[0] -= 0.11
sf.phaseError(200.0*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
ph[10] += 0.1
sf.phaseError(200.0*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
ph[10] += 0.01
sf.phaseError(200.0*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
ph[10] -= 0.11
sf.phaseError(200.0*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
ph[1] += 0.11
sf.phaseError(200.0*deg2rad, 0*deg2rad, pos, ph, 30.0e6)
ph[0] += 0.11
