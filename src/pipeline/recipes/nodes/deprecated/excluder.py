from __future__ import with_statement
from pyrap.tables import table

from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time

class excluder(LOFARnode):
    """
    Remove data from the given station from the input MS.
    """
    def run(self, input, output, *stations):
        try:
            t = table(input)
        except Exception, e:
            self.logger.error(str(e))
            raise e
        try:
            a = table(t.getkeyword('ANTENNA').split()[1])
            station_ids = [a.getcol('NAME').index(name) for name in stations]
            selection = t.query(
                "ANTENNA1 not in %s and ANTENNA2 not in %s" %
                (str(station_ids), str(station_ids))
            )
            selection.copy(output, deep=True).close()
        except Exception, e:
            self.logger.error(str(e))
            raise e
