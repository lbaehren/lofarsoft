from twisted.python import log
from pyrap.tables import table

def trim_ms(input, output, start_seconds, end_seconds):
    # Remove data from the start and/or end of a MeasurementSet.
    copy_columns = ",".join([
        'UVW', 'FLAG', 'FLAG_CATEGORY', 'WEIGHT', 'SIGMA', 'ANTENNA1',
        'ANTENNA2', 'ARRAY_ID', 'DATA_DESC_ID', 'EXPOSURE', 'FEED1', 'FEED2',
        'FIELD_ID', 'FLAG_ROW', 'INTERVAL', 'OBSERVATION_ID', 'PROCESSOR_ID',
        'SCAN_NUMBER', 'STATE_ID', 'TIME', 'TIME_CENTROID', 'DATA',
        'WEIGHT_SPECTRUM'
    ])
    try:
        t = table(input)
        selection = t.query(
            "TIME > %.16f AND TIME < %.16f" % (
                t.getcol('TIME')[0] + float(start_seconds),
                t.getcol('TIME')[-1] - float(end_seconds)
            ),
            columns=copy_columns
        )
        selection.copy(output, deep=True)
    except Exception, e:
        log.msg(str(e))
        raise

if __name__ == "__main__":
    from sys import argv
    try:
        trim_ms(argv[1], argv[2], argv[3], argv[4])
    except:
        print "Usage: trim_ms [input] [output] [start_seconds] [end_seconds]"
