from __future__ import with_statement
from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time
import shutil, os.path

class parmdb(LOFARnode):
    def run(self, infile, pdb):
        with log_time(self.logger):
            self.logger.info("Processing %s" % (infile,))

            output = os.path.join(infile, os.path.basename(pdb))

            # Remove any old parmdb database
            shutil.rmtree(output, ignore_errors=True)

            # And copy the new one into place
            shutil.copytree(pdb, output)

        return 0

