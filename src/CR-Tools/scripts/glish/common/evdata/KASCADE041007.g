# lopes event header and calibration file
#
# fills the global record "evmeta"
#
# valid since: 2004/10/07/12:00:00

evmeta := [=];

evmeta.allhead := [=];
evmeta.ch      := [=];

# global headers

evmeta.allhead.Location := 'Karlsruhe at KASCADE'
evmeta.allhead.Observatory := 'LOPES';
evmeta.allhead.AntennaCalFile := 'empty.tab';

# channel-specific headers
#.AntPos in northing, easting, height

evmeta.ch['10101'].AntPos := '79.600,-45.771,126.159'; #C13Q2
evmeta.ch['10101'].ClusterPos := 'Cluster 13 Quadrant 2  ';
evmeta.ch['10101'].AntennaCalFile := 'lopes10-neueich-lang.tab';
evmeta.ch['10101'].TIMoffset := 0;

evmeta.ch['10102'].AntPos := '111.415,-64.108,125.694'; #C13Q4
evmeta.ch['10102'].ClusterPos := 'Cluster 13 Quadrant 4  ';
evmeta.ch['10102'].AntennaCalFile := 'lopes10-neueich-lang.tab';
evmeta.ch['10102'].TIMoffset := 0;

evmeta.ch['10201'].AntPos := '36.110,-84.535,125.812'; #C9Q1  
evmeta.ch['10201'].ClusterPos := 'Cluster 9 Quadrant 1  ';
evmeta.ch['10201'].AntennaCalFile := 'lopes10-neueich.tab';
evmeta.ch['10201'].TIMoffset := 0;

evmeta.ch['10202'].AntPos := '54.611,-52.726,126.099'; #C9Q3  
evmeta.ch['10202'].ClusterPos := 'Cluster 9 Quadrant 3  ';
evmeta.ch['10202'].AntennaCalFile := 'lopes10-neueich.tab';
evmeta.ch['10202'].TIMoffset := 0;

evmeta.ch['20101'].AntPos := '22.670,-34.404,126.079'; #C10Q1 
evmeta.ch['20101'].ClusterPos := 'Cluster 10 Quadrant 1  ';
evmeta.ch['20101'].AntennaCalFile := 'lopes10-neueich.tab';
evmeta.ch['20101'].TIMoffset := 0;

evmeta.ch['20102'].AntPos := '40.831,-2.463,126.226'; #C10Q3 
evmeta.ch['20102'].ClusterPos := 'Cluster 10 Quadrant 3  ';
evmeta.ch['20102'].AntennaCalFile := 'lopes10-neueich.tab';
evmeta.ch['20102'].TIMoffset := 0;

evmeta.ch['20201'].AntPos := '2.104,40.951,126.168'; #C11Q2 
evmeta.ch['20201'].ClusterPos := 'Cluster 11 Quadrant 2  ';
evmeta.ch['20201'].AntennaCalFile := 'noant.tab';
evmeta.ch['20201'].TIMoffset := 2;

evmeta.ch['20202'].AntPos := '34.335,22.658,126.216'; #C11Q4 
evmeta.ch['20202'].ClusterPos := 'Cluster 11 Quadrant 4  ';
evmeta.ch['20202'].AntennaCalFile := 'lopes10-neueich.tab';
evmeta.ch['20202'].TIMoffset := 2;

evmeta.ch['30101'].AntPos := '72.789,-20.850,126.072'; #C14Q1 
evmeta.ch['30101'].ClusterPos := 'Cluster 14 Quadrant 1  ';
evmeta.ch['30101'].AntennaCalFile := 'lopes10-neueich.tab';
evmeta.ch['30101'].TIMoffset := 1;

evmeta.ch['30102'].AntPos := '91.011,11.155,125.968'; #C14Q3 
evmeta.ch['30102'].ClusterPos := 'Cluster 14 Quadrant 3  ';
evmeta.ch['30102'].AntennaCalFile := 'lopes10-neueich.tab';
evmeta.ch['30102'].TIMoffset := 1;



