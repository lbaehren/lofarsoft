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
parser.add_option("-d", "--database", default="cr.db", help = "filename of database")
parser.add_option("--max-threads", default = 1, type = int, help = "maximum number of threads to use")
parser.add_option("--max-attempts", default = 1, type = int, help = "maximum number of times to attempt processing a file before adding to failed")
parser.add_option("--log-dir", default = "./log", help = "directory to store logs")

(options, args) = parser.parse_args()

db_filename = options.database
dbManager = crdb.CRDatabase(db_filename)
db = dbManager.db

event_ids = dbManager.getEventIDs(status = "NEW")

def call_pipeline(event_id):
    """Function that actually calls the subprocess for each event.
    """

    print "processing event", event_id

    with open(options.log_dir+"/"+"cr_event-"+str(event_id)+".log", "w", buffering = 1) as f:

        status = 1
        attempt = 1

        while (attempt <= options.max_attempts and status != 0):

            command = "python "+os.environ["LOFARSOFT"]+"/src/PyCRTools/pipelines/cr_event.py "+"--database="+options.database+" --id="+str(event_id)+" --max_data_length=12289024 --min_data_length=1 --search_window_width=5000 --nsigma=3 -R"

            print command

            status = subprocess.call(command, stdout=f, stderr=subprocess.STDOUT, shell=True)

            attempt += 1

# Check how many CPU's we have
count = multiprocessing.cpu_count()

print "Using", min(options.max_threads, count), "out of", count, "available CPU's."

# Create a thread pool
pool = multiprocessing.Pool(processes=min(options.max_threads,count))

# Execute the pipeline for each file
pool.map(call_pipeline, event_ids)

