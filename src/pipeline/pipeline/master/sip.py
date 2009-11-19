import sys, datetime, logging, os
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.lofaringredient import LOFARinput, LOFARoutput
import pipeline.support.utilities as utilities

class SIPException(Exception):
    pass

class SIPRecipeFailed(SIPException):
    pass

class SIPReceipeNotFound(SIPException):
    pass

class sip(LOFARrecipe):
    """
    The LOFAR Standard Imaging Pipeline.
    """
    def _setup(self):
        # Set up logging to file
        os.makedirs(self.config.get("layout", "log_directory"))
        handler = logging.FileHandler('%s/pipeline.log' % (
                self.config.get("layout", "log_directory")
            )
        )
        formatter = logging.Formatter(
            "%(asctime)s - %(levelname)s - %(name)s:  %(message)s",
            "%Y-%m-%d %H:%M:%S"
        )
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)

    def run_task(self, configblock, datafiles=[]):
        self.logger.info("Running task: %s" % (configblock,))
        recipe = self.config.get(configblock, "recipe")
        inputs = LOFARinput(self.inputs)
        inputs['args'] = datafiles
        inputs.update(self.config.items(configblock))
        del(inputs['recipe']) # Never a required input
        outputs = LOFARoutput()
        if self.cook_recipe(recipe, inputs, outputs):
            self.logger.warn(
                "%s reports failure (using %s recipe)" % (configblock, recipe)
            )
            raise SIPRecipeFailed("%s failed", configblock)
        return outputs['data']

    def go(self):
        super(sip, self).go()
        self._setup()
        self.logger.info("Standard Imaging Pipeline starting.")

        try:
            datafiles = self.run_task("vdsreader")
            datafiles = self.run_task("dppp", datafiles)
            datafiles = self.run_task("exclude_DE001LBA", datafiles)
            datafiles = self.run_task("trim_300", datafiles)
            datafiles = self.run_task("bbs", datafiles)
            datafiles = self.run_task("local_flag", datafiles)
            self.outputs['images'] = self.run_task("mwimager", datafiles)
            self.outputs['average'] = self.run_task("collector")
            self.run_task("sourcefinder", self.outputs['average'])
        except SIPException, message:
            self.logger.error(message)
            return 1

        return 0

if __name__ == '__main__':
    sys.exit(sip().main())
