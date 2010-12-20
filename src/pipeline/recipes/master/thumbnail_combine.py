#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                              "Thumbnail combine" pipeline demo
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from __future__ import with_statement
import sys

import lofarpipe.support.lofaringredient as ingredient
from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
from lofarpipe.support.remotecommand import ComputeJob
from lofarpipe.support.clusterdesc import ClusterDesc, get_compute_nodes

class thumbnail_combine(BaseRecipe, RemoteCommandRecipeMixIn):
    inputs = {
        'executable': ingredient.ExecField(
            '--executable',
            default="/usr/bin/montage",
            help="montage executable"
        ),
        'file_pattern': ingredient.StringField(
            '--file-pattern',
            default="*.th.png",
            help="File search pattern (glob)",
        ),
        'input_dir': ingredient.StringField(
            '--input-dir',
            help="Directory containing input files"
        ),
        'output_file': ingredient.StringField(
            '--output-file',
            help="Output filename"
        ),
        'clobber': ingredient.BoolField(
            '--clobber',
            default=False,
            help="Clobber pre-existing output files"
        ),
        'target_hosts': ingredient.ListField(
            '--target-hosts',
            help="Remote hosts on which to execute"
        )
    }

    def go(self):
        self.logger.info("Starting thumbnail_combine run")
        super(thumbnail_combine, self).go()

        #hosts = get_compute_nodes(
        #    ClusterDesc(self.config.get('cluster', "clusterdesc"))
        #)
        hosts = self.inputs['target_hosts']
        command = "python %s" % (self.__file__.replace('master', 'nodes'))
        jobs = []
        for host in hosts:
            jobs.append(
                ComputeJob(
                    host, command,
                    arguments=[
                        self.inputs['executable'],
                        self.inputs['file_pattern'],
                        self.inputs['input_dir'],
                        self.inputs['output_file'],
                        self.inputs['clobber']
                    ]
                )
            )
        self._schedule_jobs(jobs)

        if self.error.isSet():
            self.logger.warn("Failed compute job process detected")
            return 1
        else:
            return 0

if __name__ == '__main__':
    sys.exit(thumbnail_combine().main())
