# lopes event header and calibration file
#
# fills the global record "evmeta"
#
# valid since: 2003/07/14/15:59:01

evmeta := [=];

evmeta.allhead := [=];
evmeta.ch      := [=];

# global headers

evmeta.allhead.Location := 'Karlsruhe at KASCADE'
evmeta.allhead.Observatory := 'LOPES';
evmeta.allhead.AntennaCalFile := 'empty.tab';

# channel-specific headers

evmeta.ch['10101'].AntPos := '13.573 ,-50.195 ,124.729'; #C9Q1  #pos3
evmeta.ch['10101'].ClusterPos := 'Cluster 9 Quadrant 1  ';
evmeta.ch['10102'].AntPos := '31.937 ,-18.275 ,124.667'; #C9Q3  #pos4
evmeta.ch['10102'].ClusterPos := 'Cluster 9 Quadrant 3  ';
evmeta.ch['10201'].AntPos := '0.000  ,0.000   ,124.597'; #C10Q1 #pos5
evmeta.ch['10201'].ClusterPos := 'Cluster 10 Quadrant 1  ';
evmeta.ch['10202'].AntPos := '18.161 ,31.969  ,124.726'; #C10Q3 #pos6
evmeta.ch['10202'].ClusterPos := 'Cluster 10 Quadrant 3  ';
evmeta.ch['20101'].AntPos := '50.007 ,13.485  ,124.574'; #C14Q1 #pos9
evmeta.ch['20101'].ClusterPos := 'Cluster 14 Quadrant 1  ';
evmeta.ch['20102'].AntPos := '-25.058,-6.951  ,124.621'; #C6Q4 #pos2
evmeta.ch['20102'].ClusterPos := 'Cluster 6 Quadrant 4  ';

