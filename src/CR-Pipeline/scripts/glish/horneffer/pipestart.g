########################################################################
#
# script to automaically compute tables from kretafiles
#
########################################################################


#-----------------------------------------------------------------------
# parameter section
#-----------------------------------------------------------------------

### directories including trailing "/"!
pipepar.pipestartdir := '/home/horneff/lopes-pipeline'
pipepar.eventarchdir := 'none needed on ray'
pipepar.eventdatadir := '/data/LOPES/'
pipepar.tabledir := '/home/horneff/sos/'
pipepar.newtabledir := '/home/horneff/sos/incoming/'
pipepar.savedatadir := '/data/LOPES/cr/save_selected/'

pipepar.kretapattern := 'LOPES-daten-*'
pipepar.eventsperblock := 50;
pipepar.calcscript := './calc_script.csh';

#-----------------------------------------------------------------------
# work section
#-----------------------------------------------------------------------
environ.LOPESCODEDIR := pipepar.pipestartdir #Fix for bad design!

include 'mathematics.g'; include 'fftserver.g'; include 'measures.g';

include 'utilities.g'
include 'observatory-data.g'
include 'globals.g'
include 'data-func.g'
include 'data-event.g'

include 'pipeline-func.g'

kfiles := shell(spaste('ls ',pipepar.newtabledir,pipepar.kretapattern));
for (i in seq(len(kfiles))){
  file :=  kfiles[i];
  postfix := file~s/.*\.//
  if (postfix == 'gz'){
    print 'Unziping file',file;
    shell(paste('gzip -d',file));
    file := file~s/\.gz$//
    kfiles[i] := file;
  };
};

for (kretafile in kfiles){ ####The big loop!
print 'Working on Kretafile:', kretafile;
#####read in the kretafile
kretatable := kretain(kretafile);

#####get a list of corresponding lopes directories
###(unpack archives if necessary)
lopesdirs := ldirs_from_ktable_ray(kretatable);
if (lopesdirs == F){
  print 'Didn\'t find any LOPES-Archives for File:',kretafile;
  print 'Think about deleting the file.';
  next;
};

#####read in the times of the lopes-events
lopestable := lopesin(lopesdirs);

#####merge both tables
mergetable := klmerge(kretatable,lopestable);

print 'matched',sum(mergetable.filename!=''),'out of',len(mergetable.filename);
if (sum(mergetable.filename!='') == 0) {
  print 'Didn\'t find any LOPES-Events for File:',kretafile;
  print 'Think about deleting the file.';
  next;
}

kretafilename := kretafile~globalconst.FilenameExtractor;
mt_savename := spaste(pipepar.tabledir,kretafilename,'.sos');
write_value(mergetable,mt_savename);


#mt_savename := argv[3];
#mergetable := read_value(argv[3]);
#pipepar.eventsperblock := 10;
#numevents := 50;

#####calculate partial data records from merged table
###arrange parameters
seltable := pipeSelectEvents(mt_savename,maxrad=1000);
numevents := len(seltable.filename);
noblocks := ceiling(numevents/pipepar.eventsperblock);
splitevent := ceiling(noblocks/2)*pipepar.eventsperblock;
recordsprefix := spaste(pipepar.tabledir,'run-',as_string(mergetable.run[1]),'-temp-');

###start the clients
shellstring := paste('nice -n 15',pipepar.calcscript,'-s 1 -e',as_string(splitevent),
                     '-i',as_string(pipepar.eventsperblock),'-f',mt_savename,
                     '-pref',recordsprefix,'-lc',pipepar.pipestartdir);
print 'Executing first client:"',shellstring,'"';
shellclient1 := shell(shellstring,async=T);
shellstring := paste('sleep 300;nice -n 15',pipepar.calcscript,'-s',
                     as_string(splitevent+1),'-e',as_string(numevents),
                     '-i',as_string(pipepar.eventsperblock),'-f',mt_savename,
                     '-pref',recordsprefix,'-lc',pipepar.pipestartdir);
print 'Executing second client:"',shellstring,'"';
shellclient2 := shell(shellstring,async=T);

###wait for clients to finish
shcl1out := '';
shcl2out := '';
whenever shellclient1->stdout do {
  global shcl1out := $value;
  if ((split(shcl1out)[1]=='Done')||(split(shcl1out)[2]=='seems')) {
    print "shcl1out:",shcl1out;
  };
};
whenever shellclient2->stdout do {
  global shcl2out := $value;
  if ((split(shcl2out)[1]=='Done')||(split(shcl2out)[2]=='seems')) {
    print "shcl2out:",shcl2out;
  };
};

while(shcl1out!='done!') {await shellclient1->stdout;};
while(shcl2out!='done!') {await shellclient2->stdout;};


###merge the temporary files
tmpfiles := shell(spaste('ls ',recordsprefix,'*'));
 #dosen't work because glish starts less and the script hangs...
 #print 'generated temporary files:',tmpfiles; 
print 'generated',len(tmpfiles),' temporary files';
mrecname := spaste(pipepar.tabledir,'run-',as_string(mergetable.run[1]),'-pipelined.sos');

if (len(tmpfiles)>1) {
  mergedrecord := merge_data_records(tmpfiles);
  print 'Writing merged record as:',mrecname;
  write_value(mergedrecord,mrecname);
} else if (len(tmpfiles)==1) {
  shell(paste('mv',tmpfiles,mrecname));
} else {
  print 'It seems there isn\'t any temporary file....';
};

#####Cleaning up
print 'Deleting temporary files';
shell(paste('rm',tmpfiles));
print 'Moving file',kretafile;
shell(paste('mv',kretafile,pipepar.tabledir));

####Generate selection-files
oldselectedtable := pipeSelectEvents(mrecname,maxze=(40/180*pi),minsize=5e6,minlogeguess=8);
oldselectedname := spaste(pipepar.tabledir,'run-',as_string(mergetable.run[1]),'-oldsel.sos');
if (len(oldselectedtable) > 1){
  write_value(oldselectedtable,oldselectedname);
  for (name in oldselectedtable.filename){
    shell(paste('cp',name,pipepar.savedatadir));
  };
};
newselectedtable := pipeSelectEvents(mrecname,minsize=5e6,minlmuo=2e5,totalsizes=T);
newselectedname := spaste(pipepar.tabledir,'run-',as_string(mergetable.run[1]),'-newsel.sos');
if (len(newselectedtable) > 1){
  write_value(newselectedtable,newselectedname);
  for (name in newselectedtable.filename){
    shell(paste('cp',name,pipepar.savedatadir));
  };
};

};####The big loop!

exit
