#############
# LOPESTART #
#############

# GLOBAL DIRS #
global nigldir:=spaste(environ.NIGLDIR,'/');
global dyndir:=spaste(environ.DYNDIR,'/');
global datadir:=spaste(environ.DATADIR,'/');
global lopesdatadir:=spaste(environ.LOPESDATADIR,'/');
global itsdatadir:=spaste(environ.ITSDATADIR,'/');
global workdir:=spaste(environ.WORKDIR,'/');
global sosfiledir:=spaste(nigldir,'sos/');
global psdir:=spaste(nigldir,'ps/');

# LOGGER #
system.output.log:=spaste(nigldir,'/log/nigl.log',time());

# TK # Set the focus model for the TK windows and frames
system.tk := [=]
system.tk.focus := 'click'

# LOPES #
include 'catalog.g'
include 'fftserver.g'; 
include 'functionfitter.g'           # Generate default dff tool
include "guicomponents.g" 
include 'image.g'
include "interpolate1d.g"
include 'mathematics.g';
include 'matrix.g'
include 'measures.g';
include 'misc.g'
include 'pgplotwidget.g'
include 'progress.g'
include 'quanta.g'
include 'sh.g'
include 'skycatalog.g'
include 'synclistboxes.g';
include 'timer.g'
include 'viewer.g'

if (has_field(environ,'LOPESCODE')){
  include spaste(environ.LOPESCODE,'/oldGlish/code/utilities.g');
  include spaste(environ.LOPESCODE,'/oldGlish/code/globals-const.g');
  include spaste(environ.LOPESCODE,'/oldGlish/code/observatory-data.g');
  include spaste(environ.LOPESCODE,'/oldGlish/code/globals.g');
} else {
  # The following files had to be added manually to "makedoc"!
  # Watch out if you make changes.
  include 'utilities.g'
  include 'globals-const.g'
  include 'observatory-data.g'
  include 'globals.g'
};

include add_dir('AviaryPlugins.g');
include add_dir('DataRecordIO.g');
include add_dir('DynamicSpectrum.g');
include add_dir('actions.g');
include add_dir('beamshape.g');
include add_dir('Calibration.g');
include add_dir('ccCubeGenerator.g');
include add_dir('cursor.g');
include add_dir('lopestables.g');
include add_dir('data-event.g');
include add_dir('data-event-func.g');
include add_dir('data-project.g');
include add_dir('data-func.g');
include add_dir('data-gui.g');
include add_dir('data-kern.g'); 
include add_dir('data-BEAM.g');
include add_dir('data-Nancay.g');
include add_dir('data-NOEMI.g');
include add_dir('data-TEK.g');
include add_dir('data-TIM40.g');
include add_dir('data-TIM40f.g');
include add_dir('data-TIM40rev.g');
include add_dir('data-TIM40revrev.g')
include add_dir('data-TIM40beam.g');
include add_dir('filterant.g');
include add_dir('ImageTools.g');
include add_dir('itsform.g');
include add_dir('Masking.g');
include add_dir('map.g');
include add_dir('moviefromimage.g');
include add_dir('phasing.g');
include add_dir('plotantennas.g');
include add_dir('plotpanel.g'); 
include add_dir('plotpanel-decoder.g');
include add_dir('plotpanel-func.g');
include add_dir('plotLocations.g');
include add_dir('Skymapper.g');
include add_dir('SkymapperGUI.g');
include add_dir('trigger.g');
include add_dir('itsform.g');
include add_dir('tvdec.g');
include add_dir('sounddec.g');

# INCLUDE SUBDIRS #
linclude('tBeamformer.g');
linclude('tSkymapper.g');
linclude('spike.g');
linclude('dynspectools.g');
linclude('dynspec.g');

# SKYCATALOG # check if the default source-catalog is available
if (dms.fileexists(add_dir("defsourcecat.tbl"),'-d')) {
  print 'WARNING: Default source catalog missing; building it now.';
  tab2skycat();
} else {
  print 'NORMAL: Found default source catalog.';
}

if (dms.fileexists(add_dir("Sources"),'-d')) { 
  print 'NORMAL: Customized source catalog found.';
} else {
  print 'WARNING: Customized source catalog missing; building it now.';
  addsourcesfromskycat();
}

# INFO #
print "-----------------------------------------------------------------------"
print " Local settings"
print "-----------------------------------------------------------------------"
print "HomeDir ..... : ",globalconst.HomeDir;
print "LOPESCASA ... : ",globalconst.LOPESCASA;
print "LOPESBIND ... : ",globalconst.LOPESBIND;
print "progDir ..... : ",globalconst.progDir;
print "WorkDir ..... : ",globalconst.WorkDir;
print "DefDataDir .. : ",globalpar.DefDataDir;
print "DefITSDataDir : ",globalpar.DefITSDataDir;
for (i in seq(len(globalpar.DefTabfiles))) print sprintf('DefTabfiles[%i]:',i), globalpar.DefTabfiles[i]
print "cur_event_dir : ",globalpar.cur_event_dir;
print "-----------------------------------------------------------------------"

# CONSTS #
global arylen:=2^16;
global fftlen:=arylen/2+1;
input_event(data,'example.event');
global refant:=data.get('RefAnt');
global samplerateunit:=data.headf('SamplerateUnit',refant);
global samplerate:=data.headf('Samplerate',refant)*samplerateunit;
global freqmin:=data.headf('FrequencyLow',refant)*samplerateunit;
global freqband:=data.headf('Bandwidth',refant)*samplerateunit;
global freqmax:=freqmin+freqband;

# FUNCTIONS #
##########################################
#lopesgui:=function() {plotgui:=plotguif(data)};
lopesgui:=function() {if (!is_defined('plotgui')) gui()};
doc:=function() {shell('konqueror http://aips2.nrao.edu/docs/search/search.html &'); return};
new:=function(name) {include spaste(nigldir,name,'.g')};
incl:=function(name) {include spaste(name,'.g')};
tools:=function() {include 'utility.g'};
##########################################
printopt:=function(handle)
{
  if (len(handle)==1) {return F};
	options:=handle.getoptions();
	print 'OPTIONS:';
	print '';
	for (optnum in seq(len(options)))
	{
		if (len(options[optnum])>1) {print options[optnum][1],':',options[optnum][4]}
		else {print options[optnum]};
	}
}
##########################################
plot:=function(ary,col=-1)
{
	global pgp
	if (!is_defined('pgp') || (len(pgp)==1)) pgp:=pgplotter(foreground='black',background='white')
	if (col>=0) pgp.sci(col);
	pgp.plotxy1(1:len(ary),ary)
}
##########################################
if (!is_defined('data')) {
  print "No data loaded so far - loading example data set."
  input_event(data,add_dir('example.event'));
} else {
  print "Ok, there is already a data file loaded."
}
##########################################
