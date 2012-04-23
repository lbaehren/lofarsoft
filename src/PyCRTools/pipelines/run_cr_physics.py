#! /usr/bin/env python

"""This script runs the automatic pipeline (cr_event) on the LORA triggered LOFAR VHECR data.
"""

import os
import multiprocessing
import subprocess
from pycrtools import crdatabase as crdb

from optparse import OptionParser

# Parse commandline options
parser = OptionParser()
parser.add_option("-d", "--database", default="cr.db", help="Filename of database")
parser.add_option("--max-threads", default = 12, help="Maximum number of threads to use.")
parser.add_option("--log-dir", default = "./", help="Directory to store logs.")

(options, args) = parser.parse_args()

db_filename = options.database
dbManager = crdb.CRDatabase(db_filename)
db = dbManager.db

event_ids = dbManager.getEventIDs() #status = "CR_FOUND")

def call_pipeline(event_id):
    """Function that actually calls the subprocess for each event.
    """

    with open(options.log_dir+"/"+"event-"+str(event_id)+".log", "w") as f:

        status = subprocess.call("python "+os.environ["LOFARSOFT"]+"/src/PyCRTools/pipelines/cr_physics.py --id="+str(event_id)+" --database="+options.database, stdout=f, stderr=subprocess.STDOUT, shell=True)

# Check how many CPU's we have
count = multiprocessing.cpu_count()

print "Using", min(options.max_threads, count), "out of", count, "available CPU's."

# Create a thread pool
pool = multiprocessing.Pool(processes=min(options.max_threads,count))

# Execute the pipeline for each file
pool.map(call_pipeline, event_ids)

