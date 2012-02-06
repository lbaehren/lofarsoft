#! /usr/bin/env python

"""This script runs the automatic pipeline (cr_event) on the LORA triggered LOFAR VHECR data.
"""

import os
import glob
import multiprocessing
import subprocess

max_threads = 12 # maximum number of threads to use
max_attempts = 2 # maximum number of times to attempt processing a file before adding to failed
input_dir = "/data/VHECR/LORAtriggered/data"
processed_files = "/data/VHECR/LORAtriggered/processed.log"
failed_files = "/data/VHECR/LORAtriggered/failed.log"
log_dir = "/data/VHECR/LORAtriggered/log"

def call_pipeline(filename):
    """Function that actually calls the subprocess for each file.
    """
    with open(log_dir+"/"+filename.split("/")[-1].rstrip(".h5")+".logtemp", "w") as f:

        status = 1
        attempt = 1

        while (attempt <= max_attempts and status != 0):
            print filename
            status = subprocess.call("/opt/lofarsoft/src/PyCRTools/pipelines/cr_event.py "+filename+" --datadir=/data/VHECR/LORAtriggered/data --outputdir=/data/VHECR/LORAtriggered/results --loradir /data/VHECR/LORAtriggered/LORA --lora_logfile LORAtime4 --max_data_length=12289024 --min_data_length=1 --search_window_width=5000 --nsigma=3 -R", stdout=f, stderr=subprocess.STDOUT, shell=True)
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
pool = multiprocessing.Pool(processes=min(max_threads,count))

# Execute the pipeline for each file
pool.map(call_pipeline, files)

