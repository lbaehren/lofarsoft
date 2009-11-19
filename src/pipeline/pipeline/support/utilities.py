from cuisine.parset import Parset
from tempfile import mkstemp
import os

class ClusterError(Exception):
    pass

def get_parset(parset):
    p = Parset()
    p.readFromFile(parset)
    return p

def build_available_list(listname):
    # Run on engine to construct list of locally-stored data
    import os
    from IPython.kernel.engineservice import get_engine
    available = [
        ms_name for ms_name in ms_names if os.access(ms_name, os.R_OK)
    ]
    properties = get_engine(id).properties
    properties[listname] = available

def clear_available_list(listname):
    from IPython.kernel.engineservice import get_engine
    del(get_engine(id).properties[listname])

def check_for_path(properties, dependargs):
    try:
        return dependargs[0] in properties[dependargs[1]]
    except NameError:
        return False

def patch_parset(parset, data, output_dir=None):
    # We want to be able to update parsets on the fly, for instance to add the
    # names of data files which need processing into a standard configuration
    temp_parset_filename = mkstemp(dir=output_dir)[1]
    temp_parset = get_parset(parset)
    for k, v in data.items():
        temp_parset[k] = v
    temp_parset.writeToFile(temp_parset_filename)
    return temp_parset_filename

def move_log(log, destination):
    try:
        os.makedirs(destination)
    except OSError, failure:
        if failure.errono != errno.EEXIST:
            raise
    shutil.move(log_file, destination)

