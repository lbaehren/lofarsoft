from __future__ import with_statement
import pyrap.tables

from pipeline.support.lofarnode import LOFARnode
from pipeline.support.utilities import log_time

class makecolumns_node(LOFARnode):
    """
    Add imaging columns to a given MS using pyrap.
    """
    def run(self, file):
        with log_time(self.logger):
            self.logger.info("Processing: %s" % (file))
            try:
                pyrap.tables.addImagingColumns(file)
            except ValueError:
                self.logger.debug('Add imaging columns failed: already exist?')
