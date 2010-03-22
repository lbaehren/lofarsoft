# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import check_call
from tempfile import mkdtemp
from ConfigParser import SafeConfigParser as ConfigParser
from shutil import rmtree
import os.path

# Root directory for config file
from pipeline import __path__ as config_path

from tkp_lib.accessors import FitsFile
from tkp_lib.image import ImageData
from tkp_lib.database import connection

from tkp_lib.dataset import DataSet

def sextract(filename, dataset):
    raise NotImplementedError
    # Hack around MonetDB concurrency issues(!)
    import time, random
    time.sleep(random.randint(0,60))

    try:
        config = ConfigParser()
        config.read("%s/pipeline.cfg" % (config_path[0],))
        image2fits = config.get('sextractor', 'image2fits')

        tempdir = mkdtemp(dir='/data/swinbank')
        fitsfile = os.path.join(tempdir, os.path.basename(filename) + ".fits")

        command_line = [image2fits, "in=%s" % (os.path.basename(filename)), "out=%s" % (fitsfile)]
        cwd = os.path.dirname(filename)

        check_call(
            command_line,
            cwd=os.path.dirname(filename),
            close_fds=True
        )

        image = ImageData(FitsFile(fitsfile), dataset=dataset)
    except Exception, inst:
        return "ERROR: %s on %s, %s" % (str((type(inst))), platform.node(), fitsfile)

    sr = image.sextract()
    with closing(connection()) as con:
        sr.savetoDB(con)
    
    rmtree(tempdir)
    return "%s found %d sources" % (filename, len(sr))

if __name__ == "__main__":
    from sys import argv
    dataset = DataSet("command line")
    try:
        sextract(argv[1], dataset)
    except:
        print "Usage: sextractor [filename]"
