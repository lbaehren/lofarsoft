
#-------------------------------------------------------------------------------
# $Id: startCG.g,v 1.1 2007/07/26 14:37:22 horneff Exp $
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
include add_dir('cguipipeline.g');

# -----------------------------------------------------------
# include files from peoples subdirectories
# -----------------------------------------------------------

linclude('tBeamformer.g');
linclude('tSkymapper.g');
linclude('spike.g');
linclude('dynspectools.g');
linclude('dynspec.g');


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
# Load the personalized settings
# ------------------------------------------------------------------------------

if (fexist(globalconst.MyStart)) {
  include globalconst.MyStart;
}
