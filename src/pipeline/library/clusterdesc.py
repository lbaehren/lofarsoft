from __future__ import with_statement
from contextlib import closing

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
