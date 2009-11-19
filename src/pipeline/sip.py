"""
This is a model imaging pipeline definition.

Although it should be runnable as it stands, the user is encouraged to copy it
to a job directory and customise it as appropriate for the particular task at
hand.
"""

import sys, os
from pipeline.master.control import control
from pipeline.support.utilities import log_time

class sip(control):
    def pipeline_logic(self):
        # Customise this to the requirements of a specific job.
        with log_time(self.logger):
            datafiles = self.run_task("vdsreader")
            datafiles = self.run_task("dppp", datafiles)
            datafiles = self.run_task("exclude_DE001LBA", datafiles)
            datafiles = self.run_task("trim_300", datafiles)
            datafiles = self.run_task("bbs", datafiles)
            datafiles = self.run_task("local_flag", datafiles)
            self.outputs['images'] = self.run_task("mwimager", datafiles)
            self.outputs['average'] = self.run_task("collector")
            self.run_task("qcheck", self.outputs['images'])
            self.run_task("sourcefinder", self.outputs['average'])
       
if __name__ == '__main__':
    sys.exit(eval(os.path.splitext(os.path.basename(sys.argv[0]))[0])().main())
