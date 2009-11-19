# Root directory for config file
from pipeline import __path__ as config_path

from twisted.python import log
from pyrap.tables import table
import numpy

def flag_data(input, output, max_value):
    try:
        t = table(input)
        t2 = t.copy(output, deep=True)
        t2.close()
        t = table(output, readonly=False)
    except Exception, e:
        log.msg(str(e))
        raise
    try:
        for i, data in enumerate(t.getcol('CORRECTED_DATA')):
            if max([abs(val) for val in data[0]]) > max_value:
                t.putcell('FLAG', i, numpy.array([[True, True, True, True]]))
                t.putcell('FLAG_ROW', i, True)
        t.close()
    except Exception, e:
        log.msg(str(e))
        raise

if __name__ == "__main__":
    from sys import argv
    try:
        flag_data(*argv[1:])
    except:
        print "Usage: flagger [input] [output]"
