import sys
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.lofaringredient import LOFARinput, LOFARoutput
import pipeline.support.utilities as utilities

class sip(LOFARrecipe):
    """
    The LOFAR Standard Imaging Pipeline.
    """
    def __init__(self):
        super(sip, self).__init__()

    def go(self):
        self.logger.info("Standard Imaging Pipeline starting up")
        super(sip, self).go()

        self.logger.info("Calling DPPP")
        inputs = LOFARinput(self.inputs)
        inputs['gvds']   = None # Use default
        inputs['parset'] = None # Use default
        outputs = LOFARoutput()
        if self.cook_recipe('dppp', inputs, outputs):
            self.logger.warn("DPPP reports failure")
            return 1

        self.logger.info("Calling MWImager")
        inputs = LOFARinput(self.inputs)
        inputs['gvds']              = None # Use default
        inputs['parset']            = None # Use default
        inputs['working_directory'] = None # Use default
        outputs = LOFARoutput()
        if self.cook_recipe('mwimager', inputs, outputs):
            self.logger.warn("MWImager reports failure")
            return 1

        return 0


if __name__ == '__main__':
    sys.exit(sip().main())
