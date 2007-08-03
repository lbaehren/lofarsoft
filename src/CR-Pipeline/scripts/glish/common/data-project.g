#######################################################################
#
# Functions and other stuff to load project files
#
########################################################################

#-----------------------------------------------------------------------
#Name:    input_project
#-----------------------------------------------------------------------
#Type:    Function
#Doc:     reads in data from a directory with antenna data & header files
#         usable instead of readprojectfiles_gui
#Example: input_project(data,'example.its/',head='*.hdr')
#-----------------------------------------------------------------------
#<!- Andreas Nigl @ 01.05.04 @ KUN ->
#----------------------------------------------------------------------
input_project := function(ref rec, dir='', head='', ants=-1, blocksize=2^11)
{
  # include #
  include add_dir('data-event.g')  

  # globals #
  global globalpar

  # parameters #
  if (dir=='') {dir:=globalpar.DefDataDir}
  if (head=='') {head:=globalpar.DefLogfile}

  # project file pattern #
  headfile:=spaste(dir,head)

  # directory check #
  files:=shell(spaste(['ls ',dir]))
  if (len(files)==0)
  {print 'directory ',dir,' does not exist, is not accessable or empty !!!'};

  # header file extraction #
  #if (fexist(files[1])) {
  proj:=[=]; readproject(proj,headfile,headerfiles=T)
  #} else {print 'project files do not exist or are not accessible !!!'};

  # data information extraction # (for redfilesH)
  filelist:=[=]
  headerlist:=[=]
  proj:=proj[1]
  if (ants==-1) {ants:=1:len(proj)};
  for (ant in 1:len(ants))
  {
    filedir:=proj[ants[ant]]["Files"] ~ globalconst.DirExtractor
    filename:=proj[ants[ant]]["Files"] ~ globalconst.FilenameExtractor
    if (filedir=='') {filelist[ant]:=spaste(dir,filename)}
    else {filelist[ant]:=spaste(filedir,filename)}
    headerlist[ant]:=proj[ants[ant]]
    headerlist[ant]["Files"]:=filelist[ant]
  }

  # new globals #
  globalpar.DefDataDir:=(headfile ~ globalconst.DirExtractor)
  globalpar.DefLogfile:=(headfile ~ globalconst.FilenameExtractor)

  # project opening # [Antenna files are opened, first block without offset and blocksize one is read. Data can be extracted with:
  # rec.setregion([lowlimit,highlimit],'T',antennanumber) and data.get('FIELD',antennanumber)]
  offset:=0
  readfilesH(rec,headerlist,blocksize=blocksize,offset=offset)
}
