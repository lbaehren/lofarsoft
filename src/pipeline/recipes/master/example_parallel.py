#                                                         LOFAR IMAGING PIPELINE
#
#                                    Example recipe with simple job distribution
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

import sys
from lofarpipe.support.baserecipe import BaseRecipe
from lofarpipe.support.remotecommand import RemoteCommandRecipeMixIn
from lofarpipe.support.remotecommand import ComputeJob

class example_parallel(BaseRecipe, RemoteCommandRecipeMixIn):
    def go(self):
        super(example_parallel, self).go()
        node_command = "python %s" % (self.__file__.replace("master", "nodes"))
        job = ComputeJob("localhost", node_command, arguments=["example_argument"])
        self._schedule_jobs([job])
        if self.error.isSet():
            return 1
        else:
            return 0

if __name__ == "__main__":
    sys.exit(example_parallel().main())
