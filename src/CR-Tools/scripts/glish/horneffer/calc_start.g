print 'asdkljguiabd z978sdfgtsdf789by6dsruæ?|',environ.LOPESCODEDIR


include 'catalog.g'
#include 'image.g';
include 'mathematics.g';
include 'fftserver.g'; 
include 'measures.g';
include "guicomponents.g" 
include 'quanta.g'
include "interpolate1d.g"
include 'progress.g'

########################################################################
#Includes
########################################################################

 
if (has_field(environ,'LOPESCODEDIR')){
  include spaste(environ.LOPESCODEDIR,'/utilities.g');
  include spaste(environ.LOPESCODEDIR,'/observatory-data.g');
  include spaste(environ.LOPESCODEDIR,'/globals.g');
} else {
  include 'utilities.g'
  include 'observatory-data.g'
  include 'globals.g'
};

#include add_dir('cursor.g');
include add_dir('lopestables.g');
include add_dir('data-event.g');
include add_dir('data-event-func.g');
include add_dir('data-func.g');
include add_dir('data-gui.g');
include add_dir('data-kern.g'); 
#include add_dir('data-NOEMI.g');
include add_dir('data-TIM40.g');
include add_dir('data-TIM40f.g');
include add_dir('data-TIM40rev.g');
#include add_dir('data-TIM40beam.g');
#include add_dir('data-TEK.g');
include add_dir('filterant.g');
include add_dir('map.g');
#include add_dir('moviefromimage.g');
include add_dir('phasing.g');
#include add_dir('dynspectools.g');
include add_dir('dynspec.g');
#include add_dir('plotantennas.g');
include add_dir('plotpanel.g'); 
#include add_dir('Skymapper.g');
#include add_dir('spike.g');
include add_dir('plotpanel-func.g');
#include add_dir('wcslib.g');
include add_dir('pipeline-calc.g');

plotgui:=F;

print argv[3],argv[4],argv[5],argv[6]


erg := selectEvents(argv[3],minsize=1,maxrad=1000,allquadrants=T)

if (fexist(argv[6])) {
  blubb := read_value(argv[6]);
  if (has_field(blubb,'filtrange') && has_field(blubb,'shfilt') &&
      (len(blubb.filtrange)>1) &&
      (len(blubb.filtrange)==len(blubb.shfilt[as_integer(argv[4])])) && 
      (len(blubb.filtrange)==len(blubb.shfilt[as_integer(argv[5])])) ) {
    print argv[6],'seems to exist. Not calculating again!';
    exit;
  };
};

erg2 := generate_data_record(data,erg,[as_integer(argv[4]),as_integer(argv[5])],keepfilt=1)

write_value(erg2,argv[6]);

exit;
