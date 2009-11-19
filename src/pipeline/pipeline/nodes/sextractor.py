# Python standard library
from __future__ import with_statement
from subprocess import check_call
from tempfile import mkdtemp
from ConfigParser import SafeConfigParser as ConfigParser
from shutil import rmtree
import os.path

# Root directory for config file
from pipeline import __path__ as config_path

from tkp_lib.accessors import FitsFile
from tkp_lib.image import ImageData

def sextract(filename):
    config = ConfigParser()
    config.read("%s/pipeline.cfg" % (config_path[0],))
    image2fits = config.get('sextractor', 'image2fits')

    tempdir = mkdtemp()
    fitsfile = "%s/temp.fits" % (tempdir)

    command_line = [image2fits, "in=%s" % (os.path.basename(filename)), "out=%s" % (fitsfile)]
    cwd = os.path.dirname(filename)

    check_call(
        command_line,
        cwd=os.path.dirname(filename)
    )

    image = ImageData(FitsFile(fitsfile))
    sr = image.sextract()
#    sr.savetoDB()
    
    rmtree(tempdir)
    return (len(sr))

if __name__ == "__main__":
    from sys import argv
    try:
        sextract(argv[1])
    except:
        print "Usage: sextractor [filename]"
