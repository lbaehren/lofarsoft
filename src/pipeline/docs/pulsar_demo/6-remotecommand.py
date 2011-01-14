#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                              "Thumbnail combine" pipeline demo
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

import sys

from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
from lofarpipe.support.remotecommand import ComputeJob

class thumbnail_combine(BaseRecipe, RemoteCommandRecipeMixIn):
    def go(self):
        self.logger.info("Starting thumbnail_combine run")
        super(thumbnail_combine, self).go()

        # Hosts on which to execute
        hosts = ['lce019']

        # Path to node script
        command = "python %s" % (self.__file__.replace('master', 'nodes'))

        # Build a list of jobs
        jobs = []
        for host in hosts:
            jobs.append(
                ComputeJob(
                    host, command,
                    arguments=[
                        "/usr/bin/montage",     # executable
                        "*.th.png",             # file_pattern
                        "/path/to/png/files",   # input_dir
                        "/path/to/output.png",  # output_dir
                        True                    # clobber
                    ]
                )
            )

        # And run them
        self._schedule_jobs(jobs)

        # The error flag is set if a job failed
        if self.error.isSet():
            self.logger.warn("Failed compute job process detected")
            return 1
        else:
            return 0

if __name__ == '__main__':
    sys.exit(thumbnail_combine().main())
