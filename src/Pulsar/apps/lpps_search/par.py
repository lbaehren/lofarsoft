'''
Find P0 given a TEMPO .par file. 

Note : Temporary solution until proper .par parser is available.
'''
import re

t = 'F0                 1.351932457770e           7.000E-12'
t = 'F0                 1.351932457770e+2'#           7.000E-12'

REGEXP = re.compile(r'(?P<key>(P|F)0)\s+(?P<value>(\.\d+|\d+\.\d+|\d+)((e|E)(\+|-)\d+)?)(\s+(?P<error>(\.\d+|\d+\.\d+|\d+)((e|E)(\+|-)\d+)?))?')

def match_p0(line):
    m = REGEXP.match(line)
    if m:
        value = float(m.group('value'))
        if m.group('key') == 'P0':
            return value
        else:
            return 1 / value
    return None
    

def get_p0(par_filename):
    try:
        f = open(par_filename, 'r')
        try:
            for l in f:
                p0 = match_p0(l)
                if p0 != None:
                    break
        finally:
            f.close()
    except IOError, e:
        raise
    
    if p0 == None:
        raise ValueError('.par file does not contain a rotational period or frequency.')
    return p0

