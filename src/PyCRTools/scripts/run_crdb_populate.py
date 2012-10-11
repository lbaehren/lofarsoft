#! /usr/bin/env python
"""
Script to populate the CR database
"""
import os
from optparse import OptionParser

class CRDatabasePopulator(object):

    def __init__(self, db_filename="", options=None):
        """Initialisation of the CRDatabasePopulator class."""
        self.db_filename = db_filename
        self.options = options
        self.filename_list = []

        if "" == self.db_filename:
            raise ValueError("No database filename provided.")

        if not self.options:
            raise ValueError("No options provided.")

        self.dbManager = crdb.CRDatabase(filename=self.db_filename,
                                         datapath=options.datapath,
                                         resultspath=options.resultspath,
                                         lorapath=options.lorapath)

        if self.dbManager:
            self.db = self.dbManager.db
            self.settings = self.dbManager.settings
        else:
            raise ValueError("Unable to set CRDatabase.")


    def getFileList(self):
        """Get a list of filenames that need to be added to the CR database."""
        datapath = self.settings.datapath

        self.filename_list = []

        if "" != options.datafile:
            self.filename_list = [options.datafile]
        else:
            print "  Building list of files to process..."
            self.filename_list = os.listdir(datapath)

            # Include: h5
            print "filtering h5..."
            self.filename_list = [f for f in self.filename_list if ("h5" in f)]
            # Exclude: test
            print "filtering test..."
            self.filename_list = [f for f in self.filename_list if ("test" not in f)]
            # Exclude: bkp
            print "filtering bkp..."
            self.filename_list = [f for f in self.filename_list if ("bkp" not in f)]
            # Exclude: R001-R009
            print "filtering R00x..."
            r_exclude = ["R00%1d"%(i) for i in range(1,10)]
            for r in r_exclude:
                print "filtering %s..." %(r)
                self.filename_list = [f for f in self.filename_list if (str(r) not in f)]
            # Exclude: R01x-R09x
            print "filtering R0xx..."
            r_exclude = ["R0%1d"%(i) for i in range(1,10)]
            for r in r_exclude:
                print "filtering %s..." %(r)
                self.filename_list = [f for f in self.filename_list if (str(r) not in f)]


def parseOptions():
    """Parse options from the command line."""
    options = None
    args = None

    usage = "usage: %prog [options] <databasefile>"
    parser = OptionParser(usage=usage)

    # parser.add_option("-a", "--archive", type="str", dest="archivepath", default="", help="directory where the archive can be found.")
    parser.add_option("-d", "--datapath", type="str", dest="datapath", default="", help="directory where the datafiles can be found.")
    parser.add_option("-r", "--resultspath", type="str", dest="resultspath", default="", help="directory where the results files are written.")
    parser.add_option("-l", "--lorapath", type="str", dest="lorapath", default="", help="directory where the lora files are written.")
    parser.add_option("-u", "--update", action="store_true", dest="update", default=False, help="Update instead of populating database.")
    parser.add_option("-s", "--hdf5", type="str", dest="datafile", default="", help="name of a single hdf5 file that needs to be registered in the database.")
    parser.add_option("-p", "--parameters", action="store_true", dest="parameters", default=False, help="Enable storing polarization parameter information if available [default=False].")
    parser.add_option("--max-threads", default=1, type=int, help="Maximum number of threads to use.")
    parser.add_option("--max-attempts", default=1, type=int, help="Maximum number of attempts to use.")
    parser.add_option("--logpath", type="str", dest="logpath", default="./log", help="directory where logged information should be written.")

    (options, args) = parser.parse_args()

    if len(args) != 1:
        parser.error("Incorrect number of arguments.")

    options.basepath = os.path.dirname(os.path.abspath(args[0]))

    print "datapath = %s" %(options.datapath)
    print "resultspath = %s" %(options.resultspath)
    print "lorapath = %s" %(options.lorapath)
    print "datafile = %s" %(options.datafile)
    print "logpath = %s" %(options.logpath)

    print "max. threads = %d" %(options.max_threads)
    print "max. attempts = %d" %(options.max_attempts)

    return (options, args)


def call_populate_script(data_filename=""):
    """Call the script that adds *data_filename* to the database
    stored in *db_filename*."""
    db_filename = args[0]

    print "Processing file {0}...".format(data_filename)

    log_filename = options.logpath + "/" + data_filename + ".log"
    with open(log_filename, "w", buffering=1) as log_file:
        status = 1
        attempt = 1

        while ((status != 0) and (attempt <= options.max_attempts)):
            commandline = commandline_base + "{0} {1}".format(db_filename, data_filename)
            print commandline

            status = subprocess.call(commandline, stdout=log_file, stderr=subprocess.STDOUT, shell=True)

            attempt += 1


(options, args) = parseOptions()

# Create command to execute
commandline_base = "python "+os.environ["LOFARSOFT"]+"/src/PyCRTools/scripts/crdb_populate2.py "
if ("" != options.datapath): commandline_base += "-d {0} ".format(options.datapath)
if ("" != options.resultspath): commandline_base += "-r {0} ".format(options.resultspath)
if ("" != options.lorapath): commandline_base += "-l {0} ".format(options.lorapath)
if ("" != options.datafile): commandline_base += "-s {0} ".format(options.datafile)
if (options.update): commandline_base += "-u "
if (options.parameters): commandline_base += "-p "


if __name__ == '__main__':
    print "Populating the CR database..."

    from pycrtools import crdatabase as crdb
    import multiprocessing
    import subprocess

    db_filename = args[0]
    dbp = CRDatabasePopulator(db_filename=db_filename, options=options)

    # Create a list of all files
    dbp.getFileList()


    # Loop over all files
    count_cpu = multiprocessing.cpu_count()

    print "Using", min(options.max_threads, count_cpu), "out of", count_cpu, "available CPU's."

    # Create a thread pool
    pool = multiprocessing.Pool(processes=min(options.max_threads, count_cpu))

    # Populate the CR Database for each file
    pool.map(call_populate_script, dbp.filename_list)
