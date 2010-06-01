#!/usr/bin/python

import sys, os
import datetime
import subprocess
from ConfigParser import SafeConfigParser as ConfigParser
from lofar.parameterset import parameterset
from lofarpipe.support.utilities import create_directory, string_to_list

# USER EDITABLE PARAMETERS ----------------------------------------------------

# NB: task file is defined in configuration file, not here.
pipeline_definition = '/please/provide/path/to/sip.py'
config_file         = '/please/provide/path/to/pipeline.cfg'

# Set up environment for pipeline run
pipeline_environment = {
    "PYTHONPATH": "/opt/pipeline/dependencies/lib/python2.5/site-packages:/opt/pipeline/framework/lib/python2.5/site-packages:/opt/LofIm/daily/pyrap/lib:/opt/LofIm/daily/lofar/lib/python2.5/site-packages:/opt/pythonlibs/lib/python/site-packages",
    "LD_LIBRARY_PATH": "/opt/pipeline/dependencies/lib:/opt/LofIm/daily/pyrap/lib:/opt/LofIm/daily/casacore/lib:/opt/LofIm/daily/lofar/lib:/opt/wcslib/lib/:/opt/hdf5/lib:/opt/LofIm/daily/casarest/lib:/data/sys/opt/lofar/external/log4cplus/lib", 
    "PATH": "/opt/pipeline/dependencies/bin:/home/swinbank/sw/bin:/opt/pipeline/dependencies/bin:/usr/local/bin:/usr/bin:/usr/X11R6/bin:/bin:/usr/games:/opt/LofIm/daily/casarest/bin:/opt/LofIm/daily/casarest/bin",
}

# -----------------------------------------------------------------------------

# To ensure consistency in the configuration between this wrapper and the
# pipeline, we will set the start time here.
start_time = datetime.datetime.utcnow().replace(microsecond=0).isoformat()

# We should always be called with standard command line arguments:
# tree ID, parset, ... others?
input_parset = parameterset(sys.argv[1])
tree_id      = sys.argv[2] # check this!

# Extract runtime, working, results directories from input parset
runtime_directory = input_parset.getString("ObsSW.Observation.ObservationControl.PythonControl.runtimeDirectory")
working_directory = input_parset.getString("ObsSW.Observation.ObservationControl.PythonControl.workingDirectory")
results_directory = input_parset.getString("ObsSW.Observation.ObservationControl.PythonControl.resultDirectory")

# Set up configuration for later processing stages
config = ConfigParser({
    "job_name": tree_id,
    "cwd": os.getcwd(),
    "start_time": start_time,
})
config.read(config_file)
config.set('DEFAULT', 'runtime_directory', runtime_directory)
config.set('DEFAULT', 'default_working_directory', working_directory)

# Extract input file list from parset
to_process = input_parset.getStringVector('ObsSW.Observation.DataProducts.measurementSets')

# Read config file to establish location of parset directory to use
parset_directory = config.get("layout", "parset_directory")
create_directory(parset_directory)

# For each task (currently only ndppp), extract and write parset
tasks = ConfigParser(config.defaults())
tasks.read(string_to_list(config.get("DEFAULT", "task_files")))
ndppp_parset_location = tasks.get("ndppp", "parset")
input_parset.makeSubset("ObsSW.Observation.ObservationControl.PythonControl.DPPP.").writeFile(ndppp_parset_location)

# Run pipeline & wait for result
subprocess.check_call(['python', pipeline_definition, '-j', tree_id, '-d', '--config', config_file, '--runtime-directory', runtime_directory, '--default-working-directory', working_directory, '--start-time', start_time])
