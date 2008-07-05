"""numarray: array processing for numbers, strings, records and objects.

Numarray is an array processing package designed to efficiently
manipulate large multi-dimensional arrays.  Numarray is modelled after
Numeric and features c-code generated from python template scripts,
the capacity to operate directly on arrays in files, and improved type
promotions.  Numarray provides support for manipulating arrays
consisting of numbers, strings, records, or objects using the same
basic infrastructure and syntax.
"""

DOCLINES = __doc__.split("\n")


from distutils.core import setup
from distutils.command.install_data import install_data
from distutils.command.config import config
from distutils.sysconfig import get_config_var, customize_compiler
from distutils.cygwinccompiler import CygwinCCompiler, Mingw32CCompiler
from distutils.bcppcompiler import BCPPCompiler
from distutils.msvccompiler import MSVCCompiler
try:
  from distutils.command.config import log
except:
  pass


import sys, os, os.path
import glob

from generate import VERSION, EXTRA_COMPILE_ARGS, EXTRA_LINK_ARGS, SELFTEST

from generate import NUMARRAY_PACKAGES, NUMARRAY_PACKAGE_DIRS, NUMARRAY_EXTENSIONS, NUMARRAY_DATA_FILES
from cfg_examples import EXAMPLES_PACKAGES, EXAMPLES_PACKAGE_DIRS, EXAMPLES_EXTENSIONS, EXAMPLES_DATA_FILES
from cfg_packages import ADDONS_PACKAGES,   ADDONS_PACKAGE_DIRS,   ADDONS_EXTENSIONS,   ADDONS_DATA_FILES

PACKAGE_DIRS = {}
PACKAGE_DIRS.update(NUMARRAY_PACKAGE_DIRS)
PACKAGE_DIRS.update(EXAMPLES_PACKAGE_DIRS)
PACKAGE_DIRS.update(ADDONS_PACKAGE_DIRS)

# =================================================================
# Configuration:
# Running "python setup.py config" determines some machine
# parameters that are used in dlapack_lite.c and zlapack_lite.c.

def prepend_path(p):
  try:
    os.environ["PATH"] = p + os.pathsep + os.environ["PATH"]
  except KeyError:
    pass

class config_numarray(config):
    def run (self):
        # Get a compiler
        self._check_compiler()
        try: log.set_verbosity(0)
        except: pass
        self.dump_source = 0
        # Switch off optimization
        if isinstance(self.compiler, BCPPCompiler):
            self.compiler.compile_options.remove('/O2')
        elif isinstance(self.compiler, MSVCCompiler):
	    if sys.version_info < (2,4):
              self.compiler.compile_options.remove('/Ox')
              self.compiler.compile_options.remove('/GX')
        else:
            if isinstance(self.compiler, CygwinCCompiler):
                cc = 'gcc'
            elif isinstance(self.compiler, Mingw32CCompiler):
                cc = 'gcc -mno-cygwin'
            else: # UnixCCompiler
                cc = get_config_var('CC')
            if os.environ.has_key('CC'):
                cc = os.environ['CC']
            self.compiler.set_executable('compiler_so',cc)
        directory = os.path.join("Packages","LinearAlgebra2","Src")
        testcode = """\
#include "%s"
""" % os.path.join(directory,"config.c")
        prepend_path(".")

        if sys.version_info < (2,3):
          self.compiler.exe_extension = ""
        
        if self.try_run(testcode, include_dirs=[directory]):
            print "Wrote config.h"
            if os.path.isfile(os.path.join(directory,"config.h")):
                os.remove(os.path.join(directory,"config.h"))
            os.rename("config.h",os.path.join(directory,"config.h"))
        else:
            print "Configuration failed, using default compilation"
            if os.path.isfile(os.path.join(directory,"config.h")):
                os.remove(os.path.join(directory,"config.h"))

        # Restore usual compiler flags
        if isinstance(self.compiler, BCPPCompiler):
            self.compiler.compile_options.append('/O2')
        elif isinstance(self.compiler, MSVCCompiler):
            self.compiler.compile_options.append('/Ox')
            self.compiler.compile_options.append('/GX')
        else:
            customize_compiler(self.compiler)

        try: log.set_verbosity(1)
        except: pass

# =================================================================

#  Pete Shinner's distutils data file fix... from distutils-sig
#data installer with improved intelligence over distutils
#data files are copied into the project directory instead
#of willy-nilly
class smart_install_data(install_data):   
    def run(self):
        #need to change self.install_dir to the library dir
        install_cmd = self.get_finalized_command('install')
        self.install_dir = getattr(install_cmd, 'install_lib')
        return install_data.run(self)

# =================================================================

def headers():
    return glob.glob(os.path.join("Include","numarray","*.h"))

def dolocal():
    """Adds a command line parameter --local=<dir> which is an abbreviation for
    'put all of numarray in <dir>/numarray'."""
    if "--help" in sys.argv:
        print >>sys.stderr
        print >>sys.stderr, "Numarray options:"
        print >>sys.stderr, "--local=<dir>    same as --install-lib=<dir> --install-headers=<dir>/numarray"
    for a in sys.argv:
        if a.startswith("--local="):
            dir = a.split("=")[1]
            sys.argv.extend([
                "--install-lib="+dir,
                "--install-headers="+os.path.join(dir,"numarray")
                ])
            sys.argv.remove(a)
        if a.startswith("--genapi") or a.startswith("--gencode") or \
               a.startswith("--genapi") or a.startswith("--timing"):
            sys.argv.remove(a)
        if a.startswith("--gencode"):
            sys.argv.remove(a)

CLASSIFIERS = """\
Development Status :: 4 - Beta
Intended Audience :: Science/Research
Intended Audience :: Developers
License :: OSI Approved 
Programming Language :: C
Programming Language :: Python
Topic :: Software Development 
Topic :: Scientific/Engineering
Operating System :: Microsoft :: Windows
Operating System :: POSIX
Operating System :: Unix
Operating System :: MacOS
"""

def find_name(base = 'numarray'):
    '''If "--name-with-python-version" is on the command line then
    append "-pyX.Y" to the base name'''
    name = base
    if '--name-with-python-version' in sys.argv:
        name += '-py%i.%i'%(sys.version_info[0],sys.version_info[1])
        sys.argv.remove('--name-with-python-version')
    return name

CLASSIFIERS = """\
Development Status :: 4 - Beta
Intended Audience :: Science/Research
Intended Audience :: Developers
License :: OSI Approved 
Programming Language :: C
Programming Language :: Python
Topic :: Software Development 
Topic :: Scientific/Engineering
Operating System :: Microsoft :: Windows
Operating System :: POSIX
Operating System :: Unix
Operating System :: MacOS
"""

def getparams(**p):
    return p

def main():
    if not hasattr(sys, 'version_info') or sys.version_info < (2,2,0,'final'):
        raise SystemExit, "Python 2.2.0 or later required to build numarray."
    dolocal()

    name = find_name()

    p = getparams(name = name,
          cmdclass = {"install_data":smart_install_data,
                      "config":config_numarray},
          version = VERSION,
          description = DOCLINES[0],
          long_description = "\n".join(DOCLINES[2:]),
          url = "http://www.stsci.edu/resources/software_hardware/numarray/",
          download_url = "http://sourceforge.net/projects/numpy",
          author = "Perry Greenfield",
          author_email  = "perry@stsci.edu",
          maintainer = "Todd Miller",
          maintainer_email = "jmiller@stsci.edu",
          classifiers = filter(None, CLASSIFIERS.split("\n")),
          license = "http://www.stsci.edu/resources/software_hardware/pyraf/LICENSE",
          platforms = ["Windows","Linux","Solaris","Mac OS-X"],
          packages    = NUMARRAY_PACKAGES + ADDONS_PACKAGES + EXAMPLES_PACKAGES,
          package_dir = PACKAGE_DIRS,
          headers     = headers(),
          data_files  = NUMARRAY_DATA_FILES + ADDONS_DATA_FILES + EXAMPLES_DATA_FILES,
          ext_modules = NUMARRAY_EXTENSIONS + ADDONS_EXTENSIONS + EXAMPLES_EXTENSIONS)

    if sys.version_info <= (2,3):
        del p["classifiers"]
        del p["download_url"]

    setup(**p)
    
    if SELFTEST:
        import numarray.testall as testall
        testall.test()
#
# Do the installation.
#
if __name__ == "__main__":
  if "config" not in sys.argv and ("build" in sys.argv or "install" in sys.argv):
    sys.argv.insert(sys.argv.index("setup.py")+1, "config")
  main()
