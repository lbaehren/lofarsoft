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
parser.add_option("-f","--filelist", type="str", default="",help="Optional filelist, e.g. output from select_radio_pipeline")
(options, args) = parser.parse_args()

run_locally = options.local
filelist = options.filelist

max_threads = 12 # maximum number of threads to use (tighter limit due to large memory use)
max_attempts = 1 # maximum number of times to attempt processing a file before adding to failed

if run_locally:
    input_dir = "/Users/acorstanje/triggering/fullLOFAR-12hr/cs002_caltable"
    output_dir = "/Users/acorstanje/triggering/CR/results_preselect"
    processed_files = os.path.join(output_dir, "pipeline_processed.log")
    failed_files = os.path.join(output_dir, "pipeline_failed.log")
    log_dir = os.path.join(output_dir, 'log')
else: # Altair-specific
    input_dir = "/data/VHECR/Radiotriggered/data"
    output_dir = "/data/VHECR/Radiotriggered/results"
    processed_files = "/data/VHECR/Radiotriggered/pipeline_processed.log"
    failed_files = "/data/VHECR/Radiotriggered/pipeline_failed.log"
    log_dir = "/data/VHECR/Radiotriggered/log"

#Create output dir if needed
if not os.path.exists(log_dir):
    print "# Creating log directory: ", log_dir
    os.makedirs(log_dir)

def call_pipeline(filename):
    """Function that actually calls the subprocess for each file.
    """
    with open(log_dir+"/"+filename.split("/")[-1].rstrip(".h5")+".logtemp", "w") as f:

        status = 1
        attempt = 1

        while (attempt <= max_attempts and status != 0):
            print filename
            if run_locally:
                status = subprocess.call("/Users/acorstanje/usg/src/PyCRTools/pipelines/cr_event_radio.py "+filename+" --datadir=/Users/acorstanje/triggering/fullLOFAR-12hr/cs002_caltable --outputdir="+output_dir + " --nsigma=3 -R --blocksize=131072 --max_data_length=150000 --sample_number=66048 --search_window_width=2048", stdout=f, stderr=subprocess.STDOUT, shell=True)
            else:
                status = subprocess.call("/Users/acorstanje/pipelines/cr_event_radio.py "+filename+" --datadir=" + input_dir + " --outputdir="+output_dir + " --nsigma=3 -R --blocksize=131072 --max_data_length=150000 --sample_number=66048 --search_window_width=2048", stdout=f, stderr=subprocess.STDOUT, shell=True)

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
if filelist == "":
    files = [f for f in glob.glob(input_dir +"/*.h5") if f not in done]
else:
    lst = open(filelist, "r")
    allfiles = [l.strip().split()[0] for l in lst.readlines()] # more info is on the line after the file name, remove here
    files = [f for f in allfiles if f not in done]

print files
# Check how many CPU's we have
count = multiprocessing.cpu_count()

print "Using", min(max_threads, count), "out of", count, "available CPU's."

# Create a thread pool
pool = multiprocessing.Pool(processes = min(max_threads,count)) # use all cores

# Execute the pipeline for each file
pool.map(call_pipeline, files)

