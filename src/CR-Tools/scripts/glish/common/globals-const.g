########################################################################
########################################################################
#  Global definitions and constants for the lopestools.
#
#  All "globalconst" parameters are to be specified
#  in this file for the standard(TM) installation and then redefined
#  with site specific values in "globalconst.LocalGlobal".
#
#  The parameters in "globalpar" are defined in globals.g
# 
# (In other words a standard installation should work without changes
# from the cvs-server. Any changed pathnames etc. should be put into
# "globalconst.LocalGlobal" and _not_ put onto the cvs-server! See also
# "global-local.g.example".)
########################################################################
########################################################################

cnv:=measures()
qnt:=quanta()
interp:=interpolate1d()
fit := polyfitter()

########################################################################
#
#Section: Global Constants
#
########################################################################

globalconst:=[=]

#-----------------------------------------------------------------------
#Name: globalconst.UnitPrefixName,globalconst.UnitPrefixSymbol,
#      globalconst.UnitPrefixFactor
#-----------------------------------------------------------------------
#Type: String,String,Float
#Doc: Prefixes to use for Display of Units
#-----------------------------------------------------------------------
globalconst.UnitPrefixName:=['none','nano','micro','milli','Kilo','Mega','Giga','Tera'];
globalconst.UnitPrefixSymbol:=['','n','µ','m','K','M','G','T'];
globalconst.UnitPrefixFactor:=[1,1e-9,1e-6,1e-3,1e3,1e6,1e9,1e12];

#-----------------------------------------------------------------------
#Name: globalconst.DirSeparator
#-----------------------------------------------------------------------
#Type: String
#Doc: Default separator for directories
#-----------------------------------------------------------------------
globalconst.DirSeparator:='/'

#-----------------------------------------------------------------------
#Name: globalconst.DirExtractor
#-----------------------------------------------------------------------
#Type: Regex
#Doc: Default Regular Expression to split the directory off a filename.
#-----------------------------------------------------------------------
globalconst.DirExtractor:=s/[^\/]*(|\/)$//

#-----------------------------------------------------------------------
#Name: globalconst.DeleteDirSeparator
#-----------------------------------------------------------------------
#Type: Regex
#Doc: Default Regular Expression to delte the DirSeparator from the end
#-----------------------------------------------------------------------
globalconst.DeleteDirSeparator:=s/\/+$//g

#-----------------------------------------------------------------------
#Name: globalconst.DeleteDoupleDirSeparator
#-----------------------------------------------------------------------
#Type: Regex
#Doc: Default Regular Expression to delete double DirSeparators
#-----------------------------------------------------------------------
globalconst.DeleteDoubelDirSeparator:=s/\/\/+/\//g

#-----------------------------------------------------------------------
#Name: globalconst.FilenameExtractor
#-----------------------------------------------------------------------
#Type: Regex
#Doc: Default Regular Expression to separate a filename from
#     the directory.
#-----------------------------------------------------------------------
globalconst.FilenameExtractor:=s/.*\///

#-----------------------------------------------------------------------
#Name: globalconst.DefFileFilter
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default filter to select all files in a directory
#-----------------------------------------------------------------------
globalconst.DefFileFilter:='*'

#-----------------------------------------------------------------------
#Name: globalconst.DefDataExtension
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default file extension for data files
#-----------------------------------------------------------------------
globalconst.DefDataExtension:='.dat'

#-----------------------------------------------------------------------
#Name: globalconst.DefDataFileFilter
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default filter to select all data files in a directory
#-----------------------------------------------------------------------
globalconst.DefDataFileFilter:=spaste(globalconst.DefFileFilter,globalconst.DefDataExtension)

#-----------------------------------------------------------------------
#Name: globalconst.DefHeaderExtension
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default file extension for header files
#-----------------------------------------------------------------------
globalconst.DefHeaderExtension:='.hdr'

#-----------------------------------------------------------------------
#Name: globalconst.DefHeaderFileFilter
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default filter to select all header files in a directory
#-----------------------------------------------------------------------
globalconst.DefHeaderFileFilter:=spaste(globalconst.DefFileFilter,globalconst.DefHeaderExtension)

########################################################################
#
#Section: Directories for the program files
#
########################################################################

#-----------------------------------------------------------------------
#Name: globalconst.LocalGlobal
#-----------------------------------------------------------------------
#Type: String
#Doc: File with site specific changes of global variables e.g. pathnames.
#-----------------------------------------------------------------------
globalconst.LocalGlobal := 'global-local.g'

#-----------------------------------------------------------------------
#Name: globalconst.HomeDir
#-----------------------------------------------------------------------
#Type: String
#Doc: Default Home directory for LOPES; utilzes user home directory as
#       reference point point, any further adjustment can be added as
#       relative path
#-----------------------------------------------------------------------
globalconst.HomeDir := paste(environ.HOME,'',sep='');
if (has_field(environ,'LOPESHOME')) {globalconst.HomeDir:=environ.LOPESHOME}

#-----------------------------------------------------------------------
#Name: globalconst.AIPS[ROOT|ARCH|SITE|HOST]
#-----------------------------------------------------------------------
#Type: String
#Doc:  Environment variables of the AIPS++/CASA installation:
#      <ul>
#        <li>AIPSROOT - Root directory of the AIPS++/CASA installation.
#        <li>AIPSARCH - Architecture for which AIPS++/CASA has been built.
#        <li>AIPSSITE - Site to which the computer belongs.
#        <li>AIPSHOST - Name of the computer.
#      </ul>
#-----------------------------------------------------------------------

if (has_field(environ,'AIPSPATH')) {
  globalconst.AIPSROOT := split(environ.AIPSPATH)[1];
  globalconst.AIPSARCH := split(environ.AIPSPATH)[2];
  globalconst.AIPSSITE := split(environ.AIPSPATH)[3];
  globalconst.AIPSHOST := split(environ.AIPSPATH)[4];
  globalconst.AIPSVERSION := shell(spaste('cat ',
                                        globalconst.AIPSROOT,'/code/VERSION ',
                                        '| awk \'{print $1}\'',
                                        '| sed s/"19\."//',
                                        '| sed s/"\.00"//'));
  #
  if (as_float(globalconst.AIPSVERSION) > 810) {
    print 'NORMAL: AIPS++/CASA installation found, build',globalconst.AIPSVERSION;
  } else {
    print 'NORMAL: AIPS++ installation found, build',globalconst.AIPSVERSION;
  }
} else {
  print 'WARNING: Environment variable AIPSPATH not set!';
}

#-----------------------------------------------------------------------
#Name: globalconst.LOPESCASA
#-----------------------------------------------------------------------
#Type: String
#Doc:  Root directory of the LOPES-Tools/CASA code tree.
#-----------------------------------------------------------------------

if (has_field(environ,'LOPESCASA')) {
  globalconst.LOPESCASA := environ.LOPESCASA;
  #
  print 'NORMAL: Variable LOPESCASA found:',globalconst.LOPESCASA;
} else {
  print 'WARNING: Environment variable LOPESCASA not set; using default.';
  #
  globalconst.LOPESCASA := concat_dir(globalconst.HomeDir,'lopescasa');
}

#-----------------------------------------------------------------------
#Name: globalconst.LOPESBIND
#-----------------------------------------------------------------------
#Type: String
#Doc:  Directory of the LOPES-Tools binaries.
#-----------------------------------------------------------------------

if (has_field(environ,'LOPESBIND')) {
  globalconst.LOPESBIND := environ.LOPESBIND;
} else {
  globalconst.LOPESBIND := concat_dir(globalconst.LOPESCASA,
                                      globalconst.AIPSARCH,
                                      'bin');
}

#-----------------------------------------------------------------------
#Name: globalconst.LOPESCODE
#-----------------------------------------------------------------------
#Type: String
#Doc: Directory were the lopestools code can be found
#-----------------------------------------------------------------------
globalconst.LOPESCODE := concat_dir(globalconst.LOPESCASA,'code/lopes');
if (has_field(environ,'LOPESCODE')) {globalconst.LOPESCODE:=environ.LOPESCODE}

#-----------------------------------------------------------------------
#Name: globalconst.progDir
#-----------------------------------------------------------------------
#Type: String
#Doc:  Default directory for program codes (awk, shell scripts, etc.) 
#-----------------------------------------------------------------------
globalconst.progDir:=concat_dir(globalconst.LOPESCODE,'/oldGlish/code/');

#-----------------------------------------------------------------------
#Name: globalconst.SysDataDir
#-----------------------------------------------------------------------
#Type: String
#Doc: Default directory for system data files, tables, images, 
#-----------------------------------------------------------------------
globalconst.SysDataDir := concat_dir(globalconst.progDir,'Data')
globalconst.ImageDir := concat_dir(globalconst.progDir,'icons/');

#-----------------------------------------------------------------------
#Name: globalconst.WorkDir
#-----------------------------------------------------------------------
#Type: String
#Doc: Default work directory for lopestools; utilzes user home directory as
#       reference point point, any further adjustment can be added as
#       relative path
#-----------------------------------------------------------------------
globalconst.WorkDir := concat_dir(globalconst.HomeDir,'/work');
if (has_field(environ,'LOPESWORK')) {globalconst.WorkDir:=environ.LOPESWORK}

#-----------------------------------------------------------------------
#Name: globalconst.MyStart
#-----------------------------------------------------------------------
# Type: String
# Doc : Default personal file to run after start-up
#-----------------------------------------------------------------------
globalconst.MyStart := concat_dir(globalconst.WorkDir,'mystart.g');
if (has_field(environ,'LOPESMYSTART')) {globalconst.MyStart:=environ.LOPESMYSTART}

#-----------------------------------------------------------------------
#Name: globalconst.ProjectParNameExtractor
#-----------------------------------------------------------------------
#Type: Regex
#Doc: Default Regular Expression to extract the name of a parameter in a
#     project file.
#-----------------------------------------------------------------------
globalconst.ProjectParNameExtractor:=[s/^ *#//,s/:.*\n//]

#-----------------------------------------------------------------------
#Name: globalconst.ProjectParNameFinder
#-----------------------------------------------------------------------
#Type: Regex
#Doc: Default Regular Expression to test whether a Parameter is in a
#     line of a project file.
#-----------------------------------------------------------------------
globalconst.ProjectParNameFinder:=m/^#[A-Z][A-Za-z]*: */

#-----------------------------------------------------------------------
#Name: globalconst.ProjectParExtractor
#-----------------------------------------------------------------------
#Type: Regex
#Doc: Default Regular Expression to extract the parameters in a
#     line of a project file.
#-----------------------------------------------------------------------
globalconst.ProjectParExtractor:=[s/^#[A-Z][A-Za-z]*: *//,s/ *\n//]

########################################################################
#
#Load local adjustments
#
########################################################################
tmp := stat(globalconst.LocalGlobal);
if (has_field(tmp,'type')&&(tmp.type=='ascii')) {
  include globalconst.LocalGlobal
} else {
  tmppath := concat_dir(globalconst.WorkDir,globalconst.LocalGlobal);
  tmp := stat(tmppath);
  if (has_field(tmp,'type')&&(tmp.type=='ascii')) { include tmppath };
}

