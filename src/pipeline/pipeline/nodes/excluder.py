from twisted.python import log
from pyrap.tables import table

def exclude_stations(input, output, *stations):
    # Remove data the given stations from the input MS.
    # Output is clobbered.
    try:
        t = table(input)
    except Exception, e:
        log.msg(str(e))
        raise
    try:
        a = table(t.getkeyword('ANTENNA').split()[1])
        station_ids = [a.getcol('NAME').index(name) for name in stations]
        selection = t.query(
            "ANTENNA1 not in %s and ANTENNA2 not in %s" %
            (str(station_ids), str(station_ids))
        )
        selection.copy(output, deep=True)
    except Exception, e:
        log.msg(str(e))
        raise

if __name__ == "__main__":
    from sys import argv
    try:
        exclude_stations(*argv[1:])
    except:
        print "Usage: excluder [input] [output] [station1...stationN]"
