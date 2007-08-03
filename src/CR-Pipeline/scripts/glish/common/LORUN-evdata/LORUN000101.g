# lorun event header and calibration file
#
# fills the global record "evmeta"
#
# valid since: 2000/01/01/00:00:01

evmeta := [=];

evmeta.allhead := [=];
evmeta.ch      := [=];

# global headers

evmeta.allhead.Location := 'Radboud University Nijmegen';
evmeta.allhead.Observatory := 'LORUN';
evmeta.allhead.AntennaCalFile := 'empty.tab';
evmeta.allhead.Project := 'LORUN event measurements';
evmeta.allhead.presync := '32768';

# channel-specific headers
#.AntPos in northing, easting, height

# Antenna 0 Polarisation Plane 0
evmeta.ch['10101'].AntPos := '0,0,107.938';
evmeta.ch['10101'].ClusterPos := 'Cluster 1 Quadrant 1  ';
evmeta.ch['10101'].AntennaCalFile := 'empty.tab';
evmeta.ch['10101'].TIMoffset := 0;

# Antenna 0 Polarisation Plane 1
evmeta.ch['10102'].AntPos := '0,0,107.938';
evmeta.ch['10102'].ClusterPos := 'Cluster 1 Quadrant 1  ';
evmeta.ch['10102'].AntennaCalFile := 'empty.tab';
evmeta.ch['10102'].TIMoffset := 0;

# Antenna 1 Polarisation Plane 0
evmeta.ch['10201'].AntPos := '38.262,-3.858,107.921';
evmeta.ch['10201'].ClusterPos := 'Cluster 1 Quadrant 1  ';
evmeta.ch['10201'].AntennaCalFile := 'empty.tab';
evmeta.ch['10201'].TIMoffset := 0;

# Antenna 1 Polarisation Plane 1
evmeta.ch['10202'].AntPos := '38.262,-3.858,107.921';
evmeta.ch['10202'].ClusterPos := 'Cluster 1 Quadrant 1  ';
evmeta.ch['10202'].AntennaCalFile := 'empty.tab';
evmeta.ch['10202'].TIMoffset := 0;

# Antenna 2 Polarisation Plane 0
evmeta.ch['20101'].AntPos := '44.184,-7.879,107.929';
evmeta.ch['20101'].ClusterPos := 'Cluster 1 Quadrant 1  ';
evmeta.ch['20101'].AntennaCalFile := 'empty.tab';
evmeta.ch['20101'].TIMoffset := 0;

# Antenna 2 Polarisation Plane 1
evmeta.ch['20102'].AntPos := '44.184,-7.879,107.929';
evmeta.ch['20102'].ClusterPos := 'Cluster 1 Quadrant 1  ';
evmeta.ch['20102'].AntennaCalFile := 'empty.tab';
evmeta.ch['20102'].TIMoffset := 0;

# Antenna 3 Polarisation Plane 0 (NOT REAL COORDINATES!!!)
evmeta.ch['20201'].AntPos := '-5.922,4.021,107.938';
evmeta.ch['20201'].ClusterPos := 'Cluster 1 Quadrant 1  ';
evmeta.ch['20201'].AntennaCalFile := 'empty.tab';
evmeta.ch['20201'].TIMoffset := 0;

# Antenna 3 Polarisation Plane 1 (NOT REAL COORDINATES!!!)
evmeta.ch['20202'].AntPos := '-5.922,4.021,107.938';
evmeta.ch['20202'].ClusterPos := 'Cluster 1 Quadrant 1  ';
evmeta.ch['20202'].AntennaCalFile := 'empty.tab';
evmeta.ch['20202'].TIMoffset := 0;
