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
evmeta.ch['10101'].AntennaCalFile := 'empty.tab';
evmeta.ch['10101'].TIMoffset := 0;

evmeta.ch['10102'].AntPos := '111.415,-64.108,125.694'; #C13Q4
evmeta.ch['10102'].ClusterPos := 'Cluster 13 Quadrant 4  ';
evmeta.ch['10102'].AntennaCalFile := 'empty.tab';
evmeta.ch['10102'].TIMoffset := 0;

evmeta.ch['10201'].AntPos := '36.110,-84.535,125.812'; #C9Q1  
evmeta.ch['10201'].ClusterPos := 'Cluster 9 Quadrant 1  ';
evmeta.ch['10201'].AntennaCalFile := 'empty.tab';
evmeta.ch['10201'].TIMoffset := 0;

evmeta.ch['10202'].AntPos := '54.611,-52.726,126.099'; #C9Q3  
evmeta.ch['10202'].ClusterPos := 'Cluster 9 Quadrant 3  ';
evmeta.ch['10202'].AntennaCalFile := 'empty.tab';
evmeta.ch['10202'].TIMoffset := 0;

evmeta.ch['20101'].AntPos := '22.670,-34.404,126.079'; #C10Q1 
evmeta.ch['20101'].ClusterPos := 'Cluster 10 Quadrant 1  ';
evmeta.ch['20101'].AntennaCalFile := 'empty.tab';
evmeta.ch['20101'].TIMoffset := 0;

evmeta.ch['20102'].AntPos := '40.831,-2.463,126.226'; #C10Q3 
evmeta.ch['20102'].ClusterPos := 'Cluster 10 Quadrant 3  ';
evmeta.ch['20102'].AntennaCalFile := 'empty.tab';
evmeta.ch['20102'].TIMoffset := 0;

evmeta.ch['20201'].AntPos := '2.104,40.951,126.168'; #C11Q2 
evmeta.ch['20201'].ClusterPos := 'Cluster 11 Quadrant 2  ';
evmeta.ch['20201'].AntennaCalFile := 'empty.tab';
evmeta.ch['20201'].TIMoffset := 2;

evmeta.ch['20202'].AntPos := '34.335,22.658,126.216'; #C11Q4 
evmeta.ch['20202'].ClusterPos := 'Cluster 11 Quadrant 4  ';
evmeta.ch['20202'].AntennaCalFile := 'empty.tab';
evmeta.ch['20202'].TIMoffset := 2;

evmeta.ch['30101'].AntPos := '72.789,-20.850,126.072'; #C14Q1 
evmeta.ch['30101'].ClusterPos := 'Cluster 14 Quadrant 1  ';
evmeta.ch['30101'].AntennaCalFile := 'empty.tab';
evmeta.ch['30101'].TIMoffset := 1;

evmeta.ch['30102'].AntPos := '91.011,11.155,125.968'; #C14Q3 
evmeta.ch['30102'].ClusterPos := 'Cluster 14 Quadrant 3  ';
evmeta.ch['30102'].AntennaCalFile := 'empty.tab';
evmeta.ch['30102'].TIMoffset := 1;

evmeta.ch['40101'].AntPos := '-64.345,-112.076,125.883'; #C1Q1
evmeta.ch['40101'].ClusterPos := 'Cluster 1 Quadrant 1  ';
evmeta.ch['40101'].AntennaCalFile := 'empty.tab';
evmeta.ch['40101'].TIMoffset := 0;

evmeta.ch['40102'].AntPos := '-45.991,-79.854,125.919'; #C1Q3
evmeta.ch['40102'].ClusterPos := 'Cluster 1 Quadrant 3  ';
evmeta.ch['40102'].AntennaCalFile := 'empty.tab';
evmeta.ch['40102'].TIMoffset := 0;

evmeta.ch['40201'].AntPos := '-84.675,-36.610,125.965'; #C2Q2
evmeta.ch['40201'].ClusterPos := 'Cluster 2 Quadrant 2  ';
evmeta.ch['40201'].AntennaCalFile := 'empty.tab';
evmeta.ch['40201'].TIMoffset := 0;

evmeta.ch['40202'].AntPos := '-52.079,-54.421,127.701'; #C2Q4
evmeta.ch['40202'].ClusterPos := 'Cluster 2 Quadrant 4  ';
evmeta.ch['40202'].AntennaCalFile := 'empty.tab';
evmeta.ch['40202'].TIMoffset := 0;

evmeta.ch['50101'].AntPos := '-91.573,-11.433,126.013'; #C3Q1
evmeta.ch['50101'].ClusterPos := 'Cluster 3 Quadrant 1  ';
evmeta.ch['50101'].AntennaCalFile := 'empty.tab';
evmeta.ch['50101'].TIMoffset := 0;

evmeta.ch['50102'].AntPos := '-73.148,20.312,126.079'; #C3Q3
evmeta.ch['50102'].ClusterPos := 'Cluster 3 Quadrant 3  ';
evmeta.ch['50102'].AntennaCalFile := 'empty.tab';
evmeta.ch['50102'].TIMoffset := 0;

evmeta.ch['50201'].AntPos := '-112.227,63.594,126.325'; #C4Q2
evmeta.ch['50201'].ClusterPos := 'Cluster 4 Quadrant 2  ';
evmeta.ch['50201'].AntennaCalFile := 'empty.tab';
evmeta.ch['50201'].TIMoffset := 0;

evmeta.ch['50202'].AntPos := '-80.274,45.327,126.222'; #C4Q4
evmeta.ch['50202'].ClusterPos := 'Cluster 4 Quadrant 4  ';
evmeta.ch['50202'].AntennaCalFile := 'empty.tab';
evmeta.ch['50202'].TIMoffset := 0;

evmeta.ch['60101'].AntPos := '-34.731,-23.109,126.020'; #C6Q1
evmeta.ch['60101'].ClusterPos := 'Cluster 6 Quadrant 1  ';
evmeta.ch['60101'].AntennaCalFile := 'empty.tab';
evmeta.ch['60101'].TIMoffset := 0;

evmeta.ch['60102'].AntPos := '-2.723,-41.349,125.923'; #C6Q2
evmeta.ch['60102'].ClusterPos := 'Cluster 6 Quadrant 2  ';
evmeta.ch['60102'].AntennaCalFile := 'empty.tab';
evmeta.ch['60102'].TIMoffset := 0;

evmeta.ch['70101'].AntPos := '-13.871,-97.969,126.171'; #C5Q1
evmeta.ch['70101'].ClusterPos := 'Cluster5  Quadrant 1  ';
evmeta.ch['70101'].AntennaCalFile := 'empty.tab';
evmeta.ch['70101'].TIMoffset := 0;

evmeta.ch['70102'].AntPos := '4.325,-66.075,126.161'; #C5Q3
evmeta.ch['70102'].ClusterPos := 'Cluster 5 Quadrant 3  ';
evmeta.ch['70102'].AntennaCalFile := 'empty.tab';
evmeta.ch['70102'].TIMoffset := 0;

evmeta.ch['70201'].AntPos := '-41.364,2.065,125.966'; #C7Q1
evmeta.ch['70201'].ClusterPos := 'Cluster 7 Quadrant 1  ';
evmeta.ch['70201'].AntennaCalFile := 'empty.tab';
evmeta.ch['70201'].TIMoffset := 0;

evmeta.ch['70202'].AntPos := '-23.055,34.108,125.853'; #C7Q3
evmeta.ch['70202'].ClusterPos := 'Cluster 7 Quadrant 3  ';
evmeta.ch['70202'].AntennaCalFile := 'empty.tab';
evmeta.ch['70202'].TIMoffset := 0;

evmeta.ch['80101'].AntPos := '-61.704,77.551,126.047'; #C8Q2
evmeta.ch['80101'].ClusterPos := 'Cluster 8 Quadrant 2  ';
evmeta.ch['80101'].AntennaCalFile := 'empty.tab';
evmeta.ch['80101'].TIMoffset := 0;

evmeta.ch['80102'].AntPos := '-29.850,59.024,125.887'; #C8Q4
evmeta.ch['80102'].ClusterPos := 'Cluster 8 Quadrant 4  ';
evmeta.ch['80102'].AntennaCalFile := 'empty.tab';
evmeta.ch['80102'].TIMoffset := 0;

evmeta.ch['80201'].AntPos := '-21.860,-152.363,125.808'; #WS
evmeta.ch['80201'].ClusterPos := 'Wiese Sueden   ';
evmeta.ch['80201'].AntennaCalFile := 'empty.tab';
evmeta.ch['80201'].TIMoffset := 0;

evmeta.ch['80202'].AntPos := '-7.836,-127.748,125.980'; #WO
evmeta.ch['80202'].ClusterPos := 'Wiese Osten   ';
evmeta.ch['80202'].AntennaCalFile := 'empty.tab';
evmeta.ch['80202'].TIMoffset := 0;

evmeta.ch['90101'].AntPos := '16.836,-141.780,126.144'; #WN
evmeta.ch['90101'].ClusterPos := 'Wiese Norden  ';
evmeta.ch['90101'].AntennaCalFile := 'empty.tab';
evmeta.ch['90101'].TIMoffset := 0;

evmeta.ch['90102'].AntPos := '2.616,-166.447,125.760'; #WW
evmeta.ch['90102'].ClusterPos := 'Wiese Westen  ';
evmeta.ch['90102'].AntennaCalFile := 'empty.tab';
evmeta.ch['90102'].TIMoffset := 0;


