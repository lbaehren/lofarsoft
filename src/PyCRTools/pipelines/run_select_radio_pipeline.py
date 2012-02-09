#! /usr/bin/env python

"""This script runs the automatic pipeline (cr_event) on the LORA triggered LOFAR VHECR data.
"""

import os
import glob
import multiprocessing
import subprocess
from optparse import OptionParser


usage = "usage: %prog [options] datafile.h5 "
parser = OptionParser(usage=usage)
parser.add_option("-l","--local", action="store_true", help="Indicate we run locally on our laptop (instead of altair)")
(options, args) = parser.parse_args()

run_locally = options.local
max_threads = 22 # maximum number of threads to use
max_attempts = 1 # maximum number of times to attempt processing a file before adding to failed

if run_locally:
    input_dir = "/Users/acorstanje/triggering/fullLOFAR-12hr/cs002_caltable"
    output_dir = "/Users/acorstanje/triggering/CR/results_preselect"
    processed_files = os.path.join(output_dir, "processed.log")
    failed_files = os.path.join(output_dir, "failed.log")
    log_dir = output_dir
else: # Altair-specific
    input_dir = "/data/VHECR/Radiotriggered/data"
    output_dir = "/data/VHECR/Radiotriggered"
    processed_files = "/data/VHECR/Radiotriggered/processed.log"
    failed_files = "/data/VHECR/Radiotriggered/failed.log"
    log_dir = "/data/VHECR/Radiotriggered/log"

def call_pipeline(filename):
    """Function that actually calls the subprocess for each file.
    """
    with open(log_dir+"/"+filename.split("/")[-1].rstrip(".h5")+".logtemp", "w") as f:

        status = 1
        attempt = 1

        while (attempt <= max_attempts and status != 0):
            print filename
            if run_locally:
                status = subprocess.call("/Users/acorstanje/usg/src/PyCRTools/pipelines/select_radiotriggered_data.py "+filename+" --datadir=/Users/acorstanje/triggering/fullLOFAR-12hr/cs002_caltable --cabledelays --outputdir="+output_dir + " --nsigma=3 -R", stdout=f, stderr=subprocess.STDOUT, shell=True)
            else:
                status = subprocess.call("/Users/acorstanje/pipelines/select_radiotriggered_data.py "+filename+" --datadir=" + input_dir + " --outputdir="+output_dir + " --cabledelays --nsigma=3 -R", stdout=f, stderr=subprocess.STDOUT, shell=True)

            attempt += 1

    if status == 0:
        with open(processed_files, "a", 1) as processed:
            processed.write(filename+"\n")
    else:
        with open(failed_files, "a", 1) as failed:
            failed.write(filename+"\n")

# Read already processed files from list
done = []
if os.path.isfile(processed_files):
    f = open(processed_files, "r")
    done = [l.strip() for l in f.readlines()]
    f.close()

# Check wich files in the data directory have not been processed yet
files = [f for f in glob.glob(input_dir +"/*.h5") if f not in done]

# Check how many CPU's we have
count = multiprocessing.cpu_count()

print "Using", min(max_threads, count), "out of", count, "available CPU's."

# Create a thread pool
pool = multiprocessing.Pool(processes = min(max_threads,count)) # use all cores

# Execute the pipeline for each file
pool.map(call_pipeline, files)

