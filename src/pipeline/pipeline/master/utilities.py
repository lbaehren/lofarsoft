from cuisine.parset import Parset

class ClusterError(Exception):
    pass

def get_parset(parset):
    p = Parset()
    p.readFromFile(parset)
    return p

def check_for_path(properties, path):
    return path in properties['available']
