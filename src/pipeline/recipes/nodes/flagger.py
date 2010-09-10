from __future__ import with_statement
from pyrap.tables import table
import numpy

from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time

class flagger(LOFARnode):
    """
    Flag out CORRECTED_DATA greater than some maximum value.
    """
    def run(self, input, output, max_value):
        with log_time(self.logger):
            self.logger.info("Processing: %s" % (input))
            try:
                t = table(input)
                t2 = t.copy(output, deep=True)
                t2.close()
                t = table(output, readonly=False)
            except Exception, e:
                self.logger.error(str(e))
                raise e
            try:
                for i, data in enumerate(t.getcol('CORRECTED_DATA')):
                    if max([abs(val) for val in data[0]]) > max_value:
                        t.putcell('FLAG', i, numpy.array([[True, True, True, True]]))
                        t.putcell('FLAG_ROW', i, True)
                t.close()
            except Exception, e:
                self.logger.error(str(e))
                raise e
