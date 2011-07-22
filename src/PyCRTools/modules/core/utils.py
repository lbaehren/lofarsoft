"""Contains a few useful utility functions
"""

from hftools import *
from config import *
from harray import *
import glob


class hPrint():
    """
    **Usage:**
    
    hprint = hPrint(logfile="logfile.txt",textbuffer=[])

    hprint ('Example text',1,2,"ende")

    **Description:**

    Will print its arguments and store each line in a list (called
    ``hprint.textbuffer``) if a textbuffer (a list) was provided during construction.

    This can then later be printed to another outputfile.

    With a logfile as an argument, the output will also be written to
    a logfile on disk.
    
    """
    def __init__(self,logfile="",textbuffer=None):
        self.textbuffer=None
        self.file=None
        self.logfile=None
        self.open(logfile)
            
    def __call__(self,*args):
        """
        Print the arguments to stdout and to a logfile 
        """
        s=(" ".join([str(a) for a in args]))
        print s

        if isinstance(self.textbuffer,list):
            self.textbuffer.append(s)

        if self.file:
            self.file.write(s+"\n")
            
    def clear(self):
        """
        Clears the textbuffer and closes the logfile.
        """
        self.textbuffer=[]
        if self.file and not self.file.closed:
            self.file.close()

    def open(self,logfile=None):
        """
        **Usage:**
        hprint.open(logfile="filename")

        **Description:**
        Open a logfile if a filename is provided. Otherwise re-open
        previously opened logfile.

        Return the hprint object.
        
        """
        if not logfile and self.logfile:
            logfile=self.logfile
        else:
            self.logfile=logfile

        if self.file and not self.file.closed:
            self.file.close()
        
        if logfile:
            self.file=open(logfile,"w")
        else:
            print "Error hprint: no file opened!"
            self.file=None
        return self

            
def root_filename(filename,extension=".PCR"):
    """
    Will return a filename without the ending ".pcr"

    *extension* =".PCR" - extension to remove
    """
    splt=os.path.splitext(filename)
    if splt[1].upper()==extension:
        return splt[0]
    else:
        return filename

def listFiles(unix_style_filter):
    """
    Usage:

    listFiles(unix_style_filter) -> list of filenames

    listFiles takes a string or a list of strings as argument which
    contain unix-style filename filters that can include
    '.','..','*','?','~','$HOME', and other environement variables.

    It will return a single list of filenames (including paths if
    necessary) that match the specified filter(s).

    Example:

listFiles("$LOFARSOFT/*")

 ['/Users/falcke/LOFAR/usg/asap_data.tar',
 '/Users/falcke/LOFAR/usg/build',
 '/Users/falcke/LOFAR/usg/CMakeLists.txt',
 '/Users/falcke/LOFAR/usg/CMakeOptions.cmake',
 '/Users/falcke/LOFAR/usg/data',
 '/Users/falcke/LOFAR/usg/devel_common',
 '/Users/falcke/LOFAR/usg/doc',
 '/Users/falcke/LOFAR/usg/external',
 '/Users/falcke/LOFAR/usg/README',
 '/Users/falcke/LOFAR/usg/release',
 '/Users/falcke/LOFAR/usg/src']

listFiles(["$LOFARSOFT/*.txt","~/.ema*"])

['/Users/falcke/LOFAR/usg/CMakeLists.txt',
 '/Users/falcke/.emacs',
 '/Users/falcke/.emacs.d',
 '/Users/falcke/.emacs~']
    """
    ll=[]
    for l in asList(unix_style_filter):
        ll += glob.glob(os.path.expandvars(os.path.expanduser(l)))
    if ll.count("."): del ll[ll.index(".")]
    return ll


def readParfiles(parfile):
    """
    Open one or multipe parameter (i.e. python) files and return all
    values defined in there as a dict. If file does not exist or
    ``parfile==None``, simply return and empty dict.

    The filename can contain environemnt variables, ``HOME``, or pattern
    matching (e.g., ``*``).

    See also: listFiles

    Example::

      File.par:
      x=1
      y=2

      Python:
      >>> readFile("~/File.par")
      -> {"x":1,"y":2}

    """
    pardict={}
    if type(parfile)==str:
        lf=listFiles(parfile)
        if len(lf)>0:
            for l in lf:
                f=open(l,"r")
                if f:
                    exec f in globals(),pardict
                else:
                    print "Error: Opening parameter file",l,"failed."
        else:
            print "Error: parameter files",parfile,"not found."
    return pardict
