#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                        Pipeline control recipe
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from lofarpipe.support.stateful import StatefulRecipe
from lofarpipe.support.lofarexceptions import PipelineException

#                                             Standalone Pipeline Control System
# ------------------------------------------------------------------------------

class control(StatefulRecipe):
    """
    Basic pipeline control framework.
    """
    def pipeline_logic(self):
        # Define pipeline logic here in subclasses
        raise NotImplementedError

    def go(self):
        super(control, self).go()

        self.logger.info(
            "LOFAR Pipeline (%s) starting." %
            (self.name,)
        )

        try:
            self.pipeline_logic()
        except PipelineException, message:
            self.logger.error(message)
            return 1

        return 0
