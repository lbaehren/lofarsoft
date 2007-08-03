# lopes event header and calibration file
#
# fills the global record "evmeta"
#
# valid since: 2003/03/01/00:00:01

evmeta := [=];

evmeta.allhead := [=];
evmeta.ch      := [=];

# global headers

evmeta.allhead.Location := 'Effelsberg noerdliche Wiiese'
evmeta.allhead.Observatory := 'EFFEL';
evmeta.allhead.AntennaCalFile := 'empty.tab';

# channel-specific headers

evmeta.ch['10101'].AntPos := '  0.  , -22.0, 0';
evmeta.ch['10102'].AntPos := ' 27.40,   0. , 0';
evmeta.ch['10201'].AntPos := ' 27.10, -20.5, 0';
evmeta.ch['10202'].AntPos := '  0.  ,   0. , 0';
