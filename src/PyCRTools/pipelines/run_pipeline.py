#! /usr/bin/env python

"""This script runs the automatic pipeline (cr_event) on the LORA triggered LOFAR VHECR data.
"""

import os
import glob
import subprocess

input_directory = "/data/VHECR/LORAtriggered/data"
processed_files = "/data/VHECR/LORAtriggered/processed.log"
failed_files = "/data/VHECR/LORAtriggered/failed.log"
log_dir = "/data/VHECR/LORAtriggered/log"

# Read already processed files from list
done = []
if os.path.isfile(processed_files):
    f = open(processed_files, "r")
    done = [l.strip() for l in f.readlines()]
    f.close()

# Open processed files list for appending
processed = open(processed_files, "a", 1)

# Open failed files list for appending
failed = open(failed_files, "a", 1)

for file in glob.glob(input_directory+"/*.h5"):

    if file not in done:
        print "processing", file
        with open(log_dir+"/"+file.split("/")[-1].rstrip(".h5")+".log", "w") as f:
            status = subprocess.call("/opt/lofarsoft/src/PyCRTools/pipelines/cr_event.py "+file+" --datadir=/data/VHECR/LORAtriggered/data --outputdir=/data/VHECR/LORAtriggered/results --loradir /data/VHECR/LORAtriggered/LORA --lora_logfile LORAtime4 --max_data_length=12289024 --min_data_length=1 --search_window_width=5000 --nsigma=3 -R", stdout=f, stderr=subprocess.STDOUT, shell=True)

        if status == 0:
            processed.write(file + "\n")
        else:
            failed.write(file + "\n")

    else:
        print "skipping already processed", file

# Close files
processed.close()
failed.close()

