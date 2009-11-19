import sys, datetime, logging
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.lofaringredient import LOFARinput, LOFARoutput
import pipeline.support.utilities as utilities

class sip(LOFARrecipe):
    """
    The LOFAR Standard Imaging Pipeline.
    """
    def go(self):
        # Let's log to file before we get started
        handler = logging.FileHandler('pipeline.log_%s_%s' % (self.inputs['job_name'], str(datetime.datetime.now())))
        formatter = logging.Formatter(
            "%(asctime)s - %(levelname)s - %(name)s:  %(message)s",
            "%Y-%m-%d %H:%M:%S"
        )
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)

        self.logger.info("Standard Imaging Pipeline starting up")
        super(sip, self).go()

        self.logger.info("Calling DPPP")
        inputs = LOFARinput(self.inputs)
        outputs = LOFARoutput()
        if self.cook_recipe('dppp', inputs, outputs):
            self.logger.warn("DPPP reports failure")
            return 1

        self.logger.info("Calling BBS")
        inputs = LOFARinput(self.inputs)
        inputs['force'] = True
        outputs = LOFARoutput()
        if self.cook_recipe('bbs', inputs, outputs):
            self.logger.warn("BBS reports failure")
            return 1

        self.logger.info("Calling MWImager")
        inputs = LOFARinput(self.inputs)
        outputs = LOFARoutput()
        if self.cook_recipe('mwimager', inputs, outputs):
            self.logger.warn("MWImager reports failure")
            return 1
        # The SIP will return the names of the generated images
        self.outputs['images'] = outputs['images']

        self.logger.info("Calling source extractor")
        inputs = LOFARinput(self.inputs)
        inputs['args'] = outputs['images']
        outputs = LOFARoutput()
        if self.cook_recipe('sextractor', inputs, outputs):
            self.logger.warn("sextractor reports failure")
            return 1

        return 0

if __name__ == '__main__':
    sys.exit(sip().main())
