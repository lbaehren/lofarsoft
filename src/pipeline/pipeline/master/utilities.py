from cuisine.parset import Parset

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
