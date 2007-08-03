########################################################################
########################################################################
#  Global definitions for the lopestools.
#
#  All "globalpar" parameters are to be specified
#  in this file for the standard(TM) installation and then redefined
#  with site specific values in "globalconst.LocalGlobal".
#
#  The parameters in "globalconst" are defined in globals-const.g
#  The cosmic ray event parameters are defined in globals-par-cr.g
# 
# (In other words a standard installation should work without changes
# from the cvs-server. Any changed pathnames etc. should be put into
# "globalconst.LocalGlobal" and _not_ put onto the cvs-server! See also
# "global-local.g.example".)
########################################################################
########################################################################

########################################################################
#
#Section: Global Parameters
#
########################################################################

#-----------------------------------------------------------------------
#Name: globalpar.LOPESGUI
#-----------------------------------------------------------------------
#Type: Boolean
#Doc:  Decides whether to start up the GUI at the beginning
#-----------------------------------------------------------------------
globalpar.LOPESGUI:=T;
if (has_field(environ,'LOPESGUI')) {globalpar.LOPESGUI:=(environ.LOPESGUI=='T' || environ.LOPESGUI=='t')}

#-----------------------------------------------------------------------
#Name: globalpar.DefDataDir
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default directory for data files
#      Can be updated by user; please add a comment regarding the machine
#      the settings are used on - this makes it easier to channel this
#      file through the CVS.
#-----------------------------------------------------------------------
globalpar.DefDataDir:=paste(globalconst.DirSeparator,'data',sep='');
if (has_field(environ,'LOPESDATADIR')) {globalpar.DefDataDir:=environ.LOPESDATADIR}

#-----------------------------------------------------------------------
#Name: globalpar.DefITSDataDir
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default directory for data files
#      Can be updated by user through an environment variable LOPESITSDATADIR
#-----------------------------------------------------------------------
globalpar.DefITSDataDir:=concat_dir('/data/ITS');
if (has_field(environ,'LOPESITSDATADIR')) {globalpar.DefITSDataDir:=environ.LOPESITSDATADIR}

#-----------------------------------------------------------------------
#Name: globalpar.DefLOPESDataDir
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default directory for LOPES data files
#      Can be updated by user through an environment variable LOPESLOPESDATADIR
#-----------------------------------------------------------------------
globalpar.DefLOPESDataDir:=concat_dir(globalpar.DefDataDir,'LOPES');
if (has_field(environ,'LOPESLOPESDATADIR')) {globalpar.DefLOPESDataDir:=environ.LOPESLOPESDATADIR}

#-----------------------------------------------------------------------
#Name: globalpar.DefLogfile
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default logfile for data files
#      Can be updated by user.
#-----------------------------------------------------------------------
globalpar.DefLogfile:='*.hdr'

#-----------------------------------------------------------------------
#Name: globalpar.DefLogfile
#-----------------------------------------------------------------------
#Type: String
#Doc:  Contains details about the last read/write activity. Currently only
#      updated by data-TIM40 for project files.
#-----------------------------------------------------------------------
globalpar.lastaction:=""

#-----------------------------------------------------------------------
#Name: globalpar.PlotterSizeAtStart
#-----------------------------------------------------------------------
#Type:  Array of 2 ints
#Doc:  Set the initial plotter-widget [x, y] size in pixels
#-----------------------------------------------------------------------
globalpar.PlotterSizeAtStart := [300, 400];

#-----------------------------------------------------------------------
#Name: globalpar.PlotterResize
#-----------------------------------------------------------------------
#Type:  boolean
#Doc:  Resize the plotter-widget when then windowsizes changes?
#-----------------------------------------------------------------------
globalpar.PlotterResize := T;

#-----------------------------------------------------------------------
#Name: globalpar.PrintingPlots
#-----------------------------------------------------------------------
#Type:  boolean
#Doc:  Set this to True to get plots suitable for printing in papers etc.
#-----------------------------------------------------------------------
globalpar.PrintingPlots := F;

#-----------------------------------------------------------------------
#Name: globalpar.PrintCharHeight
#-----------------------------------------------------------------------
#Type:  integer
#Doc:  Character height for printing plots.
#      (Used only if globalpar.PrintingPlots set to True)
#-----------------------------------------------------------------------
globalpar.PrintCharHeight:=2;
#-----------------------------------------------------------------------
#Name: globalpar.PrintLineWidth
#-----------------------------------------------------------------------
#Type:  integer
#Doc:  Line width for printing plots.
#      (Used only if globalpar.PrintingPlots set to True)
#-----------------------------------------------------------------------
globalpar.PrintLineWidth:=3;
#-----------------------------------------------------------------------
#Name: globalpar.PrintLabelCol
#-----------------------------------------------------------------------
#Type:  integer
#Doc:  Color index of the labels for printing plots.
#      (Used only if globalpar.PrintingPlots set to True)
#-----------------------------------------------------------------------
globalpar.PrintLabelCol:=1;
#-----------------------------------------------------------------------
#Name: globalpar.PrintToplbl
#-----------------------------------------------------------------------
#Type:  boolean
#Doc:  Print the top label for printing plots.
#      (Used only if globalpar.PrintingPlots set to True)
#-----------------------------------------------------------------------
globalpar.PrintToplbl := F;

#-----------------------------------------------------------------------
#Name: globalpar.PrintPlotind
#-----------------------------------------------------------------------
#Type:  boolean
#Doc:  Print the plot indicators at the right side of printing plots.
#      (Used only if globalpar.PrintingPlots set to True)
#-----------------------------------------------------------------------
globalpar.PrintPlotind := F;

#-----------------------------------------------------------------------
#Name: globalpar.plotantennas.phasecenter
#-----------------------------------------------------------------------
#Type:  boolean
#Doc:  Plot antennas in antenna layout with phasecenter at 0,0 or with
#      0,0 as defined in the original coordinate system.
#-----------------------------------------------------------------------
globalpar.plotantennas:=[=];
globalpar.plotantennas.phasecenter:=T;

#-----------------------------------------------------------------------
#Name: globalpar.timerstart
#-----------------------------------------------------------------------
#Type:  boolean
#Doc:  If true, start the timer which updates some limited information in the 
#      plotpanel (gui)
#
#-----------------------------------------------------------------------
globalpar.timerstart := T;

#-----------------------------------------------------------------------
#Name: globalpar.timermute
#-----------------------------------------------------------------------
#Type:  boolean
#Doc:  If true, don't execute the functions associated with the timer.
#      The timer itself will still be active.
#-----------------------------------------------------------------------
globalpar.timermute := F;

#-----------------------------------------------------------------------
#Name: globalpar.timerinterval
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Number of seconds between subsequent calls of the timer.
#-----------------------------------------------------------------------
globalpar.timerinterval := 15;

#-----------------------------------------------------------------------
#Name: globalpar.DefTabfileNo
#-----------------------------------------------------------------------
#Type: String
#Doc:  The default tabel file (number) from globalpar.DefTabfiles 
#      to be used by loadcr. 
#-----------------------------------------------------------------------
globalpar.DefTabfileNo:=1;

#-----------------------------------------------------------------------
#Name: globalpar.DefSelDataDir
#-----------------------------------------------------------------------
#Type: string
#Doc:  Directory for LOPES event selection files.
#      Can be updated by user through an environment variable LOPESSELDATADIR
#-----------------------------------------------------------------------
globalpar.DefSelDataDir:=concat_dir(globalpar.DefLOPESDataDir,'cr/sel');
if (has_field(environ,'LOPESSELDATADIR')) {globalpar.DefSelDataDir:=environ.LOPESSELDATADIR}

#-----------------------------------------------------------------------
#Name: globalpar.DefTabfiles
#-----------------------------------------------------------------------
#Type: vector of strings
#Doc:  Filenames for glish tables that store information about selected
#      events.
#-----------------------------------------------------------------------

globalpar.DefTabfiles:=[
  concat_dir(globalpar.DefSelDataDir,'bright-events.sos'),
  concat_dir(globalpar.DefSelDataDir,'bright-events-newselected.sos')]

#-----------------------------------------------------------------------
#Name: evtab
#-----------------------------------------------------------------------
#Type: String
#Doc:  Table with event informations (filename, direction, enerfy, etc.)
#-----------------------------------------------------------------------
evtab:=[=]

#-----------------------------------------------------------------------
#Name: globalpar.DefFileFilter
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default filter to select data files used by readdata.
#      Can be updated by user.
#-----------------------------------------------------------------------
globalpar.DefFileFilter:=globalconst.DefFileFilter

#-----------------------------------------------------------------------
#Name: globalpar.DefObservatory
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default observatory (name) used for initialization
#-----------------------------------------------------------------------
globalpar.DefObservatory:="LOPES"

#-----------------------------------------------------------------------
#Name: globalpar.SpawnID
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Current number of spawned data sets. This is a copy of the current
#      data set that is linked to its mother set by a cross reference.
#-----------------------------------------------------------------------
globalpar.SpawnID:=0

#-----------------------------------------------------------------------
#Name: globalpar.plotpanelID
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Current number of plotpanels that have been started in this session
#      so far.
#-----------------------------------------------------------------------
globalpar.plotpanelID:=0

#-----------------------------------------------------------------------
#Name: globalpar.BlocksizeForRead
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Default size of chunk to be read upon reading a file. Can be
#      updated by user.
#-----------------------------------------------------------------------
globalpar.BlocksizeForRead:=-1

#-----------------------------------------------------------------------
#Name: globalpar.MaxDataSize
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Maximum size of data array to be read from TIM board
#-----------------------------------------------------------------------
#globalpar.MaxDataSize:=24*2^20;
globalpar.MaxDataSize:=16*2^20;

#-----------------------------------------------------------------------
#Name: globalpar.DataSize
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Size of data array to be read from TIM board
#-----------------------------------------------------------------------
globalpar.DataSize:=8^6;

#-----------------------------------------------------------------------
#Name: globalpar.BlocksizeForDisplay
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Default size of chunk to be displayed. Can be updated by user.
#-----------------------------------------------------------------------
globalpar.BlocksizeForDisplay:=65536;

#-----------------------------------------------------------------------
#Name: globalpar.OffsetForRead
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Default offset to start read a file. Can be
#      updated by user.
#-----------------------------------------------------------------------
globalpar.OffsetForRead:=0

#-----------------------------------------------------------------------
#Name: globalpar.DataID
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Current ID for a block of data. Identifies files belonging to
#      an identical setup.
#-----------------------------------------------------------------------
globalpar.DataID:=0

#-----------------------------------------------------------------------
#Name: globalpar.evmaster
#-----------------------------------------------------------------------
#Type: String
#Doc:  master-file with the date -> eventheader connections
#      Can be updated by user.
#-----------------------------------------------------------------------
globalpar.evmaster := spaste(globalconst.progDir,globalconst.DirSeparator,'evmaster.table');
globalpar.LORUNevmaster := spaste(globalconst.progDir,globalconst.DirSeparator,'LORUN-evmaster.table');

#-----------------------------------------------------------------------
#Name: globalpar.evsave_client
#-----------------------------------------------------------------------
#Type: client
#Doc:  Default client for reading and writing event files
#-----------------------------------------------------------------------
globalpar.evsave_client  := client(concat_dir(globalconst.LOPESBIND,'eventsaveclient'));

#-----------------------------------------------------------------------
#Name: globalpar.readdat_client
#-----------------------------------------------------------------------
#Type: client
#Doc:  Default client for reading and writing raw data files ("project" files)
#-----------------------------------------------------------------------
globalpar.readdat_client := client(concat_dir(globalconst.LOPESBIND,'readdat'));

#-----------------------------------------------------------------------
#Name: global_lopestools_client
#-----------------------------------------------------------------------
#Type: client
#Doc:  Default client for miscellaneous (mathematical) routines
#-----------------------------------------------------------------------
global_lopestools_client := client(concat_dir(globalconst.LOPESBIND,'lopestools'));

#-----------------------------------------------------------------------
#Name: globalpar.CalTable_client 
#-----------------------------------------------------------------------
#Type: client record
#Doc:  Default clients for accessing CalTables 
#      One client for every observatory
#-----------------------------------------------------------------------
for (obs in obspar.observatories) {
  if (is_dir(obspar.obs[obs].CalTablePath[1])){
    globalpar.CalTable_client[obs] := 
          client(concat_dir(globalconst.LOPESBIND,'CalTableClient'));
    globalpar.CalTable_client[obs]->openTable(obspar.obs[obs].CalTablePath[1]);
  } else {
    globalpar.CalTable_client[obs] := F;
  }
}

#-----------------------------------------------------------------------
#Name: globalpar.skymapwatch
#Type: Client
#Doc: Sends a signal "done" whenever the skymapper has completed a task.
#-----------------------------------------------------------------------
subsequence skymapperwatchclient(){
  whenever self->done do {self->done(T)}
}
globalpar.skymapwatch:=skymapperwatchclient();

#-----------------------------------------------------------------------
#Name: obspar
#-----------------------------------------------------------------------
#Type: record
#Doc:  a record which contains observatory specific calibration information
#      The fields will be copied to globalpar
#-----------------------------------------------------------------------
obspar.toglobalpar(globalpar.DefObservatory);

#-----------------------------------------------------------------------
#Name: globalpar.maxnpanels
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Maximum nunper of sub-panels in the plotgui.
#ref: plotpanel.g, plotguif
#-----------------------------------------------------------------------
globalpar.maxnpanels := 100;

#-----------------------------------------------------------------------
#Name: globalpar.xbeam_nsigma
#-----------------------------------------------------------------------
#Type: float
#Doc:  Determines below how many sigma the X-beam amplification factor is clipped
#      Parameters are also partly used in plotpanel.
#ref: data-TIM40.g,TIM40Xbeamget, plotpanel
#-----------------------------------------------------------------------
globalpar.xbeam_nsigma:=1
globalpar.xbeam_l1:=1/8.
globalpar.xbeam_n_avg:=3
globalpar.xbeam_print:=F
globalpar.xbeam_method:=2

#-----------------------------------------------------------------------
#Name: globalpar.DefaultCREventNum
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Determines which event to load upon startup of the GUIs
#Ref: crguis, plotguif
#-----------------------------------------------------------------------
globalpar.DefaultCREventNum:=68

#-----------------------------------------------------------------------
#Name:   globalpar.event_loaded
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  Contains the ID number (evtab,crtab) of the currently loaded event.
#Ref: loadcr, plotpanel, data.load
#-----------------------------------------------------------------------
  globalpar.event_loaded:=-1;


# startstrings for the paralelport sync client
globalpar.ppsyncclient := ['ssh root@lopes1 ','/home/lopes/bin/ppsync'];

# number of installed TIM-Boards
globalpar.TIMno := 1;

# strings to start the tim-client
globalpar.TIMstart := ['ssh root@lopes1 ']
globalpar.TIMexec := ['/home/lopes/bin/timclient']

# device files for the boards
globalpar.TIMfiles := ['/dev/tim0']

#number of channels on the resp. TIM-Board
globalpar.TIMchannels := [2];

#positions of the Antennas
globalpar.TIMpos := array('',1);

globalpar.TIMpos[1] := '0.000  ,0.000   ,33.384';

#Section: Definitions for keeping track of the current open event file

#-----------------------------------------------------------------------
#Name: globalpar.cur_event_file
#-----------------------------------------------------------------------
#Type: String
#Doc:  The filename without directory of the currently opened event file
#-----------------------------------------------------------------------
globalpar.cur_event_file := "";

#-----------------------------------------------------------------------
#Name: globalpar.cur_event_dir
#-----------------------------------------------------------------------
#Type: String
#Doc: The directory where the current event file is; should be defined
#       using the already available directory settings.
#-----------------------------------------------------------------------
globalpar.cur_event_dir  := globalpar.DefLOPESDataDir;
if (has_field(environ,'LOPESDATADIR')) {globalpar.cur_event_dir:=environ.LOPESDATADIR}

#-----------------------------------------------------------------------
#Name: globalpar.cur_event_num
#-----------------------------------------------------------------------
#Type: Integer
#Doc:  The number of the current event file in the current directory
#-----------------------------------------------------------------------
globalpar.cur_event_num  := 1;

#-----------------------------------------------------------------------
#Name: globalpar.corrected_ant
#-----------------------------------------------------------------------
#Type: vector of integers
#Doc:  Contains a list of the recently delay-corrected antennas when
#      calibrating with the TV transmitter.
#Ref: phasecmp
#-----------------------------------------------------------------------
globalpar.corrected_ant  := [=];

########################################################################
#Section: External Programs
########################################################################
program:=[=];
                
#-----------------------------------------------------------------------
#Name: cmd
#-----------------------------------------------------------------------
#Type: Record of functions
#Doc: A number of simple shell commands:
#------------------------------------------------------------------------
cmd:=[=];
cmd.shell:=function(...){shell(paste(...,sep='; '))};
cmd.run:=function(script,dirvar='.',exec=T){s:=concat_dir(dirvar,script); if (exec) {cmd.shell(s)} else {return s}};
cmd.cd:=function(dirvar='.',exec=T){s:=spaste('cd ',dirvar); if (exec) {cmd.shell(s)} else {return s}};
cmd.rm:=function(...){shell(paste('rm -rf',...))};
cmd.dir:=function(...){shell(paste('ls -l',...))};
cmd.files:=function(...){shell(paste('ls -1',...))};
cmd.mkdir:=function(...){shell(paste('mkdir',...))};
cmd.cp:=function(f1,f2){shell(paste('cp -r',f1,f2))};
cmd.mv:=function(f1,f2){shell(paste('mv -rf',f1,f2))};
cmd.awk:=function(f1,f2,print_result=F){
  global program
  s:=shell(paste(program.awk,'-f',f1,f2)); 
  if (print_result==T) {for (i in seq(len(s))) {print s[i]}};
  return s
}

#-----------------------------------------------------------------------
#Name: program.awk
#-----------------------------------------------------------------------
#Type: Program
#Doc:  Unix awk
#-----------------------------------------------------------------------
program.awk:=spaste('gawk')

#-----------------------------------------------------------------------
#Name: program.logprocess
#-----------------------------------------------------------------------
#Type: Program
#Doc:  Converts a user written logfile into an inputfile for a program
#Ref:  Used by readproject
#-----------------------------------------------------------------------
program.logprocess:=spaste(program.awk,' -f ',globalconst.progDir,globalconst.DirSeparator,'logprocess.awk')

#-----------------------------------------------------------------------
#Name: program.catheaderprocess
#-----------------------------------------------------------------------
#Type: Program
#Doc:  Concats header files into a directory and treats them as one
#Par:  infile -	      File filter for choosing the header files
#Ref:  readproject
#-----------------------------------------------------------------------
program.catheaderprocess:=function(infile=globalconst.DefHeaderFileFilter){
return spaste('cat ',infile,' |')
}

#-----------------------------------------------------------------------
#Name: program.datestring
#-----------------------------------------------------------------------
#Type: Program
#Doc:  Returns a properly formatted date string in the form
#      yyyy.mm.dd
#      Used to assign filenames for data sets
#-----------------------------------------------------------------------
#program.datestring:=function(){return shell('date +%Y.%m.%d')}
program.datestring:=function(){return (qnt.time(qnt.quantity('today'),form="ymd no_time") ~ s/\//./g)}

#-----------------------------------------------------------------------
#Name: program.timestring
#-----------------------------------------------------------------------
#Type: Program
#Doc:  Returns a properly formatted UTC time string in the form
#      hh:mm:ss
#      Used to assign filenames for data sets
#-----------------------------------------------------------------------
#program.timestring:=function(){return shell('date +%H:%M:%S')}
program.timestring:=function(){return qnt.time(qnt.quantity('today'))}


########################################################################
#
#Global Constant Numbers
#
########################################################################
#uses the quanta.g tools
lightspeed := qnt.getvalue(qnt.constants('c'))
degree := pi/180.
neginf:=log(0)
inf:=1/0
