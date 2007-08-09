#-------------------------------------------------------------------------------
# $Id: generation-full.g,v 1.1 2007/05/10 09:35:30 snehls Exp $
#-------------------------------------------------------------------------------
#
# Start-up script for the LOPES-Tools. This will take care of including the
# required modules of the AIPS++ system as well as loading the Glish
# components of the LOPES-Tools themselves.
#
#-------------------------------------------------------------------------------

#Set the focus model for the TK windows and frames
system.tk := [=]
system.tk.focus := 'click'

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


########################################################################
#Includes
########################################################################

# -----------------------------------------------------------
# include general files
# -----------------------------------------------------------

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

# -----------------------------------------------------------
# include files from peoples subdirectories
# -----------------------------------------------------------

linclude('tBeamformer.g');
linclude('tSkymapper.g');
linclude('spike.g');
linclude('dynspectools.g');
linclude('dynspec.g');

# ------------------------------------------------------------------------------
# Create Skycatalogs; check if the default source-catalog is available
# ------------------------------------------------------------------------------

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

# ------------------------------------------------------------------------------
# Finally, after everything is started, give some feedback on the site
# dependent settings; this may proove useful for checking why your 
# installation may not work properly.
# ------------------------------------------------------------------------------

print "-----------------------------------------------------------------------"
print " Local settings"
print "-----------------------------------------------------------------------"
print "HomeDir ..... : ",globalconst.HomeDir;
print "LOPESCASA ... : ",globalconst.LOPESCASA;
print "LOPESBIND ... : ",globalconst.LOPESBIND;
print "progDir ..... : ",globalconst.progDir;
print "MyStart ..... : ",globalconst.MyStart;
print "WorkDir ..... : ",globalconst.WorkDir;
print "DefDataDir .. : ",globalpar.DefDataDir;
print "DefITSDataDir : ",globalpar.DefITSDataDir;
for (i in seq(len(globalpar.DefTabfiles))) {
  print sprintf('DefTabfiles[%i]:',i), globalpar.DefTabfiles[i];
}
print "cur_event_dir : ",globalpar.cur_event_dir;
print "-----------------------------------------------------------------------"


# ------------------------------------------------------------------------------
# If not done already, load a data file into the display panel.
# ------------------------------------------------------------------------------

if (!is_defined('data')) {
  print "No data loaded so far - loading example data set."
  input_event(data,add_dir('example.event'));
} else {
  print "Ok, there is already a data file loaded."
}

# ------------------------------------------------------------------------------
#  Start up the GUI.
# ------------------------------------------------------------------------------

if ((!is_defined('plotgui')) && globalpar.LOPESGUI) {
  gui();
}
else {};

# ------------------------------------------------------------------------------
# Load the personalized settings
# ------------------------------------------------------------------------------

if (fexist(globalconst.MyStart)) {
  include globalconst.MyStart;
}




# -----------------------------------------------
# automized way to generate SOS from KRETA output
# ----------------------------------------------

# ----------------------------------------------
#       ------------- 2005 -------------
# ----------------------------------------------
linclude('pipeline-funcGr.g')
ktable :=newKretain(['/home/snehls/radio/LOPES/incoming/event-listSEL10-05-a.dat','/home/snehls/radio/LOPES/incoming/event-listSEL10-05-b.dat','/home/snehls/radio/LOPES/incoming/event-listSEL10-05-c.dat','/home/snehls/radio/LOPES/incoming/event-listSEL10-05-d.dat'])

#       ---------- 01 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0501-060119.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0501.sos')
print('done   ---------- 01 - 2005 -------------      ')

#       ---------- 02 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0502-060119.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0502.sos')
print('done   ---------- 02 - 2005 -------------      ')

#       ---------- 03 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0503-060119.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0503.sos')
print('done   ---------- 03 - 2005 -------------      ')

#       ---------- 04 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0504-060119.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0504.sos')
print('done   ---------- 04 - 2005 -------------      ')

#       ---------- 05 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0505-060118.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0505.sos')
print('done   ---------- 05 - 2005 -------------      ')

#       ---------- 06 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0506-060118.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0506.sos')
print('done   ---------- 06 - 2005 -------------      ')

#       ---------- 07 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0507-060118.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0507.sos')
print('done   ---------- 07 - 2005 -------------      ')

#       ---------- 08 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0508-060118.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0508.sos')
print('done   ---------- 08 - 2005 -------------      ')

#       ---------- 09 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0509-060117.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0509.sos')
print('done   ---------- 09 - 2005 -------------      ')

#       ---------- 10 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0510-060117.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0510.sos')
print('done   ---------- 10 - 2005 -------------      ')

#       ---------- 11 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0511-060117.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0511.sos')
print('done   ---------- 11 - 2005 -------------      ')

#       ---------- 12 - 2005 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0512-060117.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0512.sos')
print('done   ---------- 12 - 2005 -------------      ')




# ----------------------------------------------
#       ------------- 2006 -------------
# ----------------------------------------------


ktable :=newKretain(['/home/snehls/radio/LOPES/incoming/event-listSEL10-06-a.dat','/home/snehls/radio/LOPES/incoming/event-listSEL10-06-b.dat','/home/snehls/radio/LOPES/incoming/event-listSEL10-06-c.dat','/home/snehls/radio/LOPES/incoming/event-listSEL10-06-d.dat'])

#       ---------- 01 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0601-060314.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0601.sos')
print('done   ---------- 01 - 2006 -------------      ')

#       ---------- 02 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0602-060314.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0602.sos')
print('done   ---------- 02 - 2006 -------------      ')

#       ---------- 03 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0603-060404.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0603.sos')
print('done   ---------- 03 - 2006 -------------      ')

#       ---------- 04 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0604-060503.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0604.sos')
print('done   ---------- 04 - 2006 -------------      ')

#       ---------- 05 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0605-060731.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0605.sos')
print('done   ---------- 05 - 2006 -------------      ')

#       ---------- 06 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0606-060801.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0606.sos')
print('done   ---------- 06 - 2006 -------------      ')

#       ---------- 07 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0607-061114.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0607.sos')
print('done   ---------- 07 - 2006 -------------      ')

#       ---------- 08 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0608-061115.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0608.sos')
print('done   ---------- 08 - 2006 -------------      ')

#       ---------- 09 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0609-061114.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0609.sos')
print('done   ---------- 09 - 2006 -------------      ')

#       ---------- 10 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0610-061110.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0610.sos')
print('done   ---------- 10 - 2006 -------------      ')

#       ---------- 11 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0611-061201.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0611.sos')
print('done   ---------- 11 - 2006 -------------      ')

#       ---------- 12 - 2006 -------------
ltable := QualListIn('/data/LOPES/cr/dataquality/qual-list0612-070115.dat')
mergetable := klmerge(ktable,ltable)
reducedtable := reduceTable(mergetable)
write_value(reducedtable,'/home/snehls/radio/LOPES/incoming/0612.sos')
print('done   ---------- 12 - 2006 -------------      ')

exit
