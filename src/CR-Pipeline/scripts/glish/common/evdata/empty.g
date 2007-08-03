# lopes event header and calibration file
#
# fills the global record "evmeta"
#
# valid since: epoch

evmeta := [=];

evmeta.allhead := [=];
evmeta.ch      := [=];

# global headers

evmeta.allhead.Location := 'somewhere?!?'
evmeta.allhead.AntennaCalFile := 'empty.tab';

# channel-specific headers

evmeta.ch['10101'].AntPos := '0,0,0'
evmeta.ch['10102'].AntPos := '0,0,0'
evmeta.ch['10201'].AntPos := '0,0,0'
evmeta.ch['10202'].AntPos := '0,0,0'