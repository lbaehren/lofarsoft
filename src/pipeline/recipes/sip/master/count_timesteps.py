#                                                         LOFAR IMAGING PIPELINE
#
#                                             Return total length of observation
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
import os

import lofarpipe.support.utilities as utilities
import lofarpipe.support.lofaringredient as ingredient
from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
from lofarpipe.support.group_data import load_data_map
from lofarpipe.support.remotecommand import ComputeJob

class count_timesteps(BaseRecipe, RemoteCommandRecipeMixIn):
    """
    Accept a list of baselines (in the format used by NDPPP logging).

    Flag them in all MeasurementSets.
    """
    inputs = {
        'nproc': ingredient.IntField(
            '--nproc',
            help="Maximum number of simultaneous processes per compute node",
            default=8
        )
    }
    outputs = {
        'start_time': ingredient.FloatField(),
        'end_time': ingredient.FloatField()
    }

    def go(self):
        self.logger.info("Starting count_timesteps run")
        super(count_timesteps, self).go()

        self.logger.debug("Loading map from %s" % self.inputs['args'][0])
        data = load_data_map(self.inputs['args'][0])

        command = "python %s" % (self.__file__.replace('master', 'nodes'))
        jobs = []
        for host, ms in data:
            jobs.append(
                ComputeJob(
                    host, command, arguments=[ms]
                )
            )
        jobs = self._schedule_jobs(jobs, max_per_node=self.inputs['nproc'])

        self.outputs['start_time'] = min(job.results['start_time'] for job in jobs.itervalues())
        self.outputs['end_time'] = max(job.results['end_time'] for job in jobs.itervalues())

        if self.error.isSet():
            return 1
        else:
            return 0

if __name__ == '__main__':
    sys.exit(count_timesteps().main())
