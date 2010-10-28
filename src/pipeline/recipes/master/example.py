#                                                         LOFAR IMAGING PIPELINE
#
#                                                                 Example recipe
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

import sys
import subprocess

from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.lofaringredient import ExecField, StringField
from lofarpipe.support.pipelinelogging import log_process_output

class example(BaseRecipe):
    inputs = {
        'executable': ExecField(
            '--executable',
            help="Command to run",
            default="/bin/ls"
        )
    }

    outputs = {
        'stdout': StringField()
    }

    def go(self):
        self.logger.info("Starting example recipe run")
        super(example, self).go()

        self.logger.info("This is a log message")

        my_process = subprocess.Popen(
            [
                self.inputs['executable']
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        sout, serr = my_process.communicate()
        self.outputs['stdout'] = sout
        log_process_output(
            self.inputs['executable'],
            sout,
            serr,
            self.logger
        )

        if my_process.returncode == 0:
            return 0
        else:
            self.logger.warn(
                "Return code (%d) is not 0." % my_process.returncode
            )
            return 1


if __name__ == '__main__':
    sys.exit(example().main())
