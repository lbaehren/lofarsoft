# lopes event header and calibration file
#
# fills the global record "evmeta"
#
# valid since: 2003/04/08/00:00:01

evmeta := [=];

evmeta.allhead := [=];
evmeta.ch      := [=];

# global headers

evmeta.allhead.Location := 'Dwingeloo next to the THEA-Platform'
evmeta.allhead.Observatory := 'DWL';
evmeta.allhead.AntennaCalFile := 'empty.tab';

# channel-specific headers

evmeta.ch['10101'].AntPos := '0.000  ,0.000   ,33.384';
evmeta.ch['10102'].AntPos := '5.112  ,6.273   ,33.461';
evmeta.ch['10201'].AntPos := '9.945  ,-12.489 ,33.449';
evmeta.ch['10202'].AntPos := '19.176 ,2.243   ,33.598';
evmeta.ch['20101'].AntPos := '18.169 ,-6.910  ,32.916';
evmeta.ch['20102'].AntPos := '11.529 ,0.268   ,32.921';
evmeta.ch['20201'].AntPos := '13.784 ,7.392   ,33.012';
evmeta.ch['20202'].AntPos := '2.526  ,-8.773  ,32.856';

