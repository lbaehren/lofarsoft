#!/usr/bin/python
"""
This is a model imaging pipeline definition.

Although it should be runnable as it stands, the user is encouraged to copy it
to a job directory and customise it as appropriate for the particular task at
hand.
"""
from __future__ import with_statement
import sys, os
from pipeline.master.mac_control import MAC_control
from pipeline.support.utilities import log_time

class sip(MAC_control):
    def pipeline_logic(self):
        # Customise this to the requirements of a specific job.
        with log_time(self.logger):
            # Read filenames from a VDS file
#            datafiles = self.run_task("vdsreader")
            # Or as command line arguments
            datafiles = self.inputs['args']
            datafiles = self.run_task("ndppp", datafiles)
       
if __name__ == '__main__':
    sys.exit(sip().main())
