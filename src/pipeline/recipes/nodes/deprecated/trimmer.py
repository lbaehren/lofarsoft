from __future__ import with_statement
from pyrap.tables import table

from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time

class trimmer(LOFARnode):
    """
    Remove data from the start and/or end of a MeasurementSet.
    """
    def run(self, input, output, start_seconds, end_seconds):
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
            self.logger.error(str(e))
            raise e
