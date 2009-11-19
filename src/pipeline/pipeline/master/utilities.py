from cuisine.parset import Parset

class ClusterError(Exception):
    pass

def get_parset(parset):
    p = Parset()
    p.readFromFile(parset)
    return p

