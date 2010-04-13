"""
This is a model imaging pipeline definition.

Although it should be runnable as it stands, the user is encouraged to copy it
to a job directory and customise it as appropriate for the particular task at
hand.
"""
from __future__ import with_statement
import sys, os
from lofarpipe.support.control import control
from lofarpipe.support.utilities import log_time

class sip(control):
    def pipeline_logic(self):
        # Customise this to the requirements of a specific job.
        with log_time(self.logger):
            # vdsreader returns a list of MeasurementSets to process, as well
            # as start and end times for the observation.
            vdsinfo = self.run_task("vdsreader")

            # ndppp returns a list of processed MeasurementSets
            datafiles = self.run_task(
                "ndppp",
                vdsinfo['data'],
                start_time=vdsinfo['start_time'],
                end_time=vdsinfo['end_time']
            )['data']

            # bbs works on the MeasurementSets produced by ndppp without
            # changing their names
            self.run_task("bbs", datafiles)

            # mwimager works on the MeasurementSets processed by ndppp and
            # bbs, returning a list of resultant images
            self.outputs['images'] = self.run_task("mwimager", datafiles)['data']

            # collector searches for results using globs (specified in
            # tasks.cfg), and returns the filename of an averaged image
            self.outputs['average'] = self.run_task("collector")['data']
       
if __name__ == '__main__':
    sys.exit(sip().main())
