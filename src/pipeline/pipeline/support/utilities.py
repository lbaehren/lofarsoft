from __future__ import with_statement
from contextlib import closing
from cuisine.parset import Parset
from tempfile import mkstemp
import os, errno, shutil

class ClusterDesc(dict):
    def __init__(self, filename):
        super(ClusterDesc, self).__init__()
        with closing(open(filename)) as file:
            for line in file:
                line = line.strip()
                if line and not line[0] == '#':
                    key, value = line.split('=')
                    value = value.strip()
                    if value[0] == '[' and value[-1] == ']':
                        value = [val.strip() for val in value[1:-1].split(',')]
                    self[key.strip()] = value

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
        filename for filename in filenames if os.access(filename, os.R_OK)
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

def create_directory(dirname):
    # Recursively create a directory, without failing if it already exists
    try:
        os.makedirs(dirname)
    except OSError, failure:
        if failure.errno != errno.EEXIST:
            raise

def move_log(log_file, destination):
    create_directory(destination)
    shutil.move(log_file, destination)

