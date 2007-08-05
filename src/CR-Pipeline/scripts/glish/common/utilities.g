
#-------------------------------------------------------------------------------
# $Id$
#-------------------------------------------------------------------------------
#
# Collection of utilities:
# <ol>
#  <li><a href="#filesystem">Query to the filesystem</a>
#  <li><a href="#catalogs">I/O and manipulation of sky catalogs</a>
# </ol>
#
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Section: <a name="filesystem">Query to the filesystem</a>
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Type: Function
#Doc:  include a glish script from the code tree (whithout giving the 
#Ret:  Boolean (T if it is an existing directory F otherwise)
#Par:  path = ''             - Input string
#-------------------------------------------------------------------------------

linclude := function(name) {
  path := shell(spaste('ls ',globalconst.LOPESCODE,'/oldGlish/*/',name));
  if( len(path) != 1 ) {
    print spaste('Found no or more than one "',name,'" in ',globalconst.LOPESCODE,'/oldGlish/*/');
    print 'Cannot load file :',name;
  } else {
    include path;
  }
};


#-------------------------------------------------------------------------------
#Name: concat_dir
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Adds directory names at the front of a filename. There can be 
#      multiple directories. The function will automatically include
#      the dir separator.
#Par:  ...        - The names of the directories to go in front
#      file=""    - filename at the end
#-------------------------------------------------------------------------------

concat_dir := function(...) {
  return (paste(...,sep=globalconst.DirSeparator) ~ globalconst.DeleteDoubelDirSeparator);
}


#-------------------------------------------------------------------------------
#Name: add_dir
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Adds a directory at the front of a filename. The directory is 
#      stored as a string field in the record "globalconst".
#Par:  file               - filename
#      dirvar = "CodeDir" - the name of the record field in globalconst that
#                           contains the directory
#-------------------------------------------------------------------------------

add_dir := function (file, dirvar="progDir") {
  return spaste(globalconst[dirvar],globalconst.DirSeparator,file);
}

#-------------------------------------------------------------------------------
#Name: is_dir
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  tests whether a string denotes an existing directory
#Ret:  Boolean (T if it is an existing directory F otherwise)
#Par:  path = ''             - Input string
#-------------------------------------------------------------------------------

is_dir := function(path) {
  a := stat(path)
  if (has_field(a,'type')&&(a.type == 'directory')) {
    return T;
  }
  return F;
};


#-------------------------------------------------------------------------------
#Name: fexist
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Tests whether a file exists
#Ret:  T if string exists, F otherwise
#Par:  file = ''       - Filename of file to check
#-------------------------------------------------------------------------------

fexist := function(file) {
  return dms.fileexists(file)
}

#-------------------------------------------------------------------------------
#Name: dexist
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Tests whether a file exists and is a directory
#Ret:  T if directory exists, F otherwise
#Par:  directory = ''       - Filename of directory to check
#-------------------------------------------------------------------------------

dexist := function(directory) {
  return dms.fileexists(directory,'-d')
}

#-------------------------------------------------------------------------------
#Name: username
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Get the name of the user running the current process
#Ret:  The username.
#-------------------------------------------------------------------------------

username := function () {
 return shell('finger \`whoami\` | grep Name | tr \':\' \'\\n\' | grep -v Login | grep -v Name | sed s/\" \"//');
}


#-------------------------------------------------------------------------------
#Name: selectedItemsFromList
#-------------------------------------------------------------------------------
#Type: GLish function
#Doc:  Graphical tool for the selection of items from a list. At the moment this
#      basically is intended for selection of files (since the AIPS++ catalog
#      does not allow for the selection of multiple entries in the displayed
#      file/directory list).
#Par:  listrec  - Record used to pass back the outcome of the selection;
#                 relevant fields are:
#                 <ul>
#                  <li><i>list</i> - list (of files) from which the slection was
#                      made
#                  <li><i>mask</i> - boolean array that can be applied to the
#                      full list of items
#                  <li><i>selection</i> - array containing the subset of items
#                      selected from <i>list</i>.
#                 </ul>
#Par:  dir = '' - Root directory in which to perform the file selection
#Ret:  listrec  - Modified record filled with the selection and selection mask
#Example: selectedItemsFromList (myrec, '/data/ITS/2005.02')
#Created: 2005/02/08 (L. B&auml;hren)
#-------------------------------------------------------------------------------

selectedItemsFromList := function(ref listrec, 
                                  dir="") 
{

  if (!is_record(listrec)) {
    print '[selectedItemsFromList] No record pass for returning results!'
    fail;
  }

  if (dir == '') {
    dir := "/data";
  }

#  #-------------------------------------------------------------------
#  # Determine the type of the input (this can be used later for
#  # generalization to allow selection from lists of different objects
#
#  # is the input a string? - if yes we assume it is either (a) the path
#  # to a file directory or (b) a set of file names
#  if (is_string(dir)) {
#    # get the length of string
#    lenInput := len(dir);
#    # if input is single string we assume path
#    if (lenInput == 1) {
#      if (dexist(dir)) {
#        listrec.dir := dir;
#      } else {
#        listrec.dir := '/data'
#      }
#      listrec.list := dc.list(listrec.dir);
#    } 
#    # if input is a list of strings, we assume list of files
#    else {
#      for (i in 1:lenInput) {
#        if () {
#        }
#      }
#    }
#  } else if () {
#  }

  # get the list of files in the directory
  listrec.dir       := dir;
  listrec.list      := dc.list(listrec.dir);
  listrec.listOrig  := listrec.list;
  listrec.nofFiles  := len(listrec.list);
  listrec.sel       := array(T,listrec.nofFiles);
  listrec.selOrig   := listrec.sel;

  tk_hold();

  f  := frame(title='Operations File Selection')
  mf := frame(f,side='left',expand='x')
  m  := message(mf, 'File selection', width=500)
  h:=min(20,listrec.nofFiles);

  lbf := frame(f, side='left', expand='both', relief='sunken', borderwidth=1)

  lbfiles := synclistboxes(parent=lbf,
                           nboxes=4, 
                           labels=['Nr.','On','Name','Type'],
                           vscrollbarright=T,
                           hscrollbar=F,
                           height=h, 
                           width=[4,2,40,10],
                           mode='extended',
                           background=['lightgrey','white','white','white'],
                           relief='flat', borderwidth=0,
                           fill='both');

  listf := function() {
    # [1] File numerator
    lbfiles.listbox(1)->delete("start","end")
    lbfiles.listbox(1)->insert(array2string(seq(listrec.nofFiles)))
    # [2] File selection status
    lbfiles.listbox(2)->delete("start","end")
    for (i in 1:listrec.nofFiles) {
      if (listrec.sel[i]) {
        lbfiles.listbox(2)->insert('*')
      } else {
        lbfiles.listbox(2)->insert('')
      }
    }
    # [3] Files names
    lbfiles.listbox(3)->delete("start","end")
    lbfiles.listbox(3)->insert(array2string(listrec.list))
    # [4] File type
    lbfiles.listbox(4)->delete("start","end")
    for (i in 1:listrec.nofFiles) {
      filename := spaste(listrec.dir,'/',listrec.list[i]);
      if (dexist(filename)) {
        lbfiles.listbox(4)->insert('Directory');
      } else if (fexist(filename)) {
        lbfiles.listbox(4)->insert('File');
      }
    }
  }
  listf()

  whenever lbfiles->select do {
    for (i in $value) {
      listrec.sel[i+1] := !listrec.sel[i+1] 
     }
     listf()
  }

  # Buttons and their functionality

  ef := frame(f,side='left',expand='x')

  but_allselect   := button(ef,'Select All',
                            borderwidth=1, width=9)
  but_alldeselect := button(ef,'Deselect All',
                            borderwidth=1, width=9)
  but_deletelast  := button(ef,'Delete Last',
                            borderwidth=1, width=9)
  but_reset       := button(ef,'Reset',
                            borderwidth=1, width=9)
  spacer          := frame(ef,width=10,height=1,expand='x')
  but_accept      := button(ef,'Accept',
                            borderwidth=1, width=9, background='limegreen')
  but_dismiss     := button(ef,'Dismiss',
                            borderwidth=1, width=9, background='orange',
                            foreground='white')
  tk_release();

  whenever but_allselect->press do {
    listrec.sel[1:listrec.nofFiles] := T;
    listf();
  }

  whenever but_alldeselect->press do {
    listrec.sel[1:listrec.nofFiles] := F;
    listf();
  }

  whenever but_deletelast->press do {
    # adjust to the new number of files
    nofFiles := listrec.nofFiles-1;
    listrec.nofFiles := nofFiles;
    # remove the last file
    tmp := listrec.list[1:nofFiles];
    listrec.list := tmp;
    # remove the mask for the last file
    tmp := listrec.sel[1:nofFiles];
    listrec.sel := tmp;
    # update the GUI
    listf();
  }

  whenever but_reset->press do {
    listrec.list     := listrec.listOrig;
    listrec.nofFiles := len(listrec.listOrig);
    listrec.sel      := listrec.selOrig;
    listf()
  }

  whenever but_accept->press do {
    listrec.mask      := listrec.sel;
    listrec.selection := listrec.list[listrec.mask];
    val f := F;
  }

  whenever but_dismiss->press do {
    val f := F;
  }

}

#-------------------------------------------------------------------------------
#Section: <a name="catalogs">I/O and manipulation of sky catalogs</a>
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: tab2skycat
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  convert ASCII file with radio sources to aips++ viewer-tool skycatalog
#      table<br>
#      Example format of sources.tab:
#      <pre>
#      Name    RA               DEC           DirType
#      A       D                D             A
#      M1      83.633212deg     22.014460deg  J2000
#      3C123   69.2682292deg    29.6705167deg J2000
#      </pre>
#
#Par: infile	= 'sources.tab'		- Name of the ascii file
#Par: outtable	= "defsourcecat.tbl"	- Name of the skycatalog table
#-------------------------------------------------------------------------------

tab2skycat := function(infile='sources.tab',outtable="defsourcecat.tbl") {
  global cmd;
  infile:=add_dir(infile);  outtable:=add_dir(outtable);
  print ' ';
  print '------------------------------------------------------------------------';
  print 'tab2skycat: Generating new skycatalog ',outtable;
  print 'Input list from file ',infile;
  print '------------------------------------------------------------------------';
  if (!fexist(infile)) {print 'ERROR:',infile,'does not exist!'; return};
  if (dexist(outtable)) cmd.rm(outtable);
  include 'skycatalog.g';
  sca := skycatalog(outtable);
  sca.fromascii(infile,hasheader=T,longcol='RA',latcol='DEC',dirtype='J2000');
  sca.done();
  print '-------------------End tab2skycat---------------------------------------';
};


#-------------------------------------------------------------------------------
#Name: addsourcesfromskycat
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Add sources from the LOPES source list (sources.tab) to
#      a local copy of the AIPS++ sources table (Sources).<br>
#      Example format of sources.tab:
#      <pre>
#      Name    RA               DEC           DirType
#      A       D                D             A
#      M1      83.633212deg     22.014460deg  J2000
#      3C123   69.2682292deg    29.6705167deg J2000
#      </pre>
#Par: infile	= 'sources.tab'		- Name of the ascii file
#Par: outile	= "Sources"	        - Name of Source table
#
#-------------------------------------------------------------------------------

addsourcesfromskycat := function(infile="sources.tab",outfile='Sources') {
  global globalconst,cmd
  infile:=add_dir(infile);  outfile:=add_dir(outfile);
  sources := concat_dir(globalconst.AIPSROOT,'data/ephemerides/Sources');
  print ' ';
  print '------------------------------------------------------------------------'
  print 'addsourcesfromskycat: Generating new Sourcecatalog ',outfile;
  print 'Copying system source table from ',sources;
  print 'Adding local sources from private table ',infile;
  print '------------------------------------------------------------------------'
  if (!dexist(sources)) {print 'ERROR:',sources,'does not exist!'; return};
  if (!fexist(infile)) {print 'ERROR:',infile,'does not exist!'; return};
  if (dexist(outfile)) {cmd.rm(outfile)};
  cmd.cp(sources,outfile);
  include 'measuresdata.g'
  file:=open(spaste('<',infile)); read(file); read(file);
  while ((line := read(file))!='') {
    s:=split(line);
    print 'Adding:',s[1],s[4],s[2],s[3]
    addsource(s[1],s[4],s[2],s[3])
  }
  file:=F;
  print '-------------------End addsourcesfromskycat-----------------------------';
};


#-------------------------------------------------------------------------------
#Name: list_visible_sources
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Prints a lits of visible sources for the current frame. Input is
#      from sources.tab<br>
#      Example format of sources.tab:
#      <pre>
#      Name    RA               DEC           DirType
#      A       D                D             A
#      M1      83.633212deg     22.014460deg  J2000
#      3C123   69.2682292deg    29.6705167deg J2000
#      </pre>
#
#Par: rec                               - data record used to set the frame
#Par: infile	  = 'sources.tab'       - Name of the ascii file
#Par: outile	  = "Sources"           - Name of Source table
#Par: elevation   = 0                   - Elevation limit in degrees
#Par: planetlist  = "sun moon jupiter"	- list of "planets" to include
#-------------------------------------------------------------------------------

list_visible_sources := function (ref rec, 
                                  infile="sources.tab",
                                  outfile='Sources',
                                  elevation=0,
                                  planetlist="Sun Moon Jupiter Venus Mars",
                                  toepoch='J2000') 
{
  global globalconst,cmd
  infile:=add_dir(infile);
  print ' ';
  print '-------------------List of Visible Radio Sources-----------------------';
  if (!fexist(infile)) {print 'ERROR:',infile,'does not exist!'; return};
  file:=open(spaste('<',infile)); read(file); read(file);
  while ((line := read(file))!='') {
    s:=split(line);
    azel:=dirtoazel(rec, coord=[s[2],s[3]],epoch=s[4]);
    j2000:=dirtoazel(rec, coord=[s[2],s[3]],epoch=s[4],toepoch=toepoch,prettystring=T);
    if (azel[2]>elevation) {
       printf('%-10.10s: RA=%12s, DEC=%12s, Az=%7.2f, El=%6.2f',s[1],j2000[1],j2000[2],azel[1],azel[2])
       if (azel[1]<0) printf(' (Az=%7.2f, El=%6.2f)',azel[1]+360,azel[2])
       print;
    }
  }
  file:=F;
  print '---------------------------Planets-------------------------------------';
  for (p in planetlist) {
    azel:=dirtoazel(rec, planet=p);
    j2000:=dirtoazel(rec, planet=p,toepoch=toepoch,prettystring=T);
    if (azel[2]>elevation) {
       printf('%-10.10s: RA=%12s, DEC=%12s, Az=%7.2f, El=%6.2f',p,j2000[1],j2000[2],azel[1],azel[2])
       if (azel[1]<0) printf(' (Az=%7.2f, El=%6.2f)',azel[1]+360,azel[2]);
       print;
    }
  }
  print '-------------------------End of List-----------------------------------';
  print 'Epoch of first Coordinates:',toepoch;
  print '';
};

#-------------------------------------------------------------------------------
#Name: showGlishObject
#-------------------------------------------------------------------------------
#Type: Glish Function
#Doc:  Prints the structure of a glish variable of any type in ascii format.
#      This is useful when going through untransparent glish records like
#      plotgui or data.<br>
#      Examples:
#      <pre>
#      - a := 2
#      - showGlishObject(a)
#      (integer): 2
#      - b := [first=2.3, second="foo bar", third=[2,3.3], fourth=1:1e5]
#      - showGlishObject(b)
#      (record[4]):
#         first (double): 2.3
#         second (string): "foo" "bar"
#         third (double[2]): [2 3.3]
#         fourth (integer[100000]): [1 2 3 4 5 6 7 8 9 10]  ...
#      -
#      </pre>
#Par:  level=0   - For internal use; should not be supplied by the user.
#Par:  max=5     - Maximum depth to recurse into records.
#-------------------------------------------------------------------------------

showGlishObject := function(ref obj, level=0, max=5) {
  local i;
  local indent := '';
  local lenstr := '';

  for (i in 0:level) {
    indent:=spaste('   ', indent);
  }

  if (len(obj) > 1) lenstr := spaste("[",len(obj),"]");
  printf('(%s%s): ', type_name(obj), lenstr);
  if (level > max) {
    printf('[max recursion]\n');
    return;
  }
  
  if (is_numeric(obj)) {
    if (len(obj) > 10) {
      print obj[1:10], '...';
    } else {
      print obj;
    }
    return;
  }

  if (is_string(obj)) {
    for (i in 1:len(obj)) {
      printf('"%s" ', obj[i]);
    }
    printf('\n');
    return;
  }

  if (is_regex(obj)) {
    print obj;
    return;
  }

  if (is_record(obj)) {
    printf('\n');
    i := 0;
    for (n in field_names(obj)) {
      i +:= 1;
      if (n =~ m/\*/) n := spaste("[",i,"]");
      printf('%s%s ', indent, n);
      showGlishObject(obj[i], level+1, max);
    }
    return;
  }

  if (is_agent(obj)) {
    print obj;
    return;
  }

  if (is_function(obj)) {
    printf('\n');
    return;
  }

  if (is_file(obj)) {
    print spaste(obj) ~ s/<FILE: (.*)>/$1/;
    return;
  }
}
