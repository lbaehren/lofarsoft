import sys, datetime, logging, os
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.lofaringredient import LOFARinput, LOFARoutput
import pipeline.support.utilities as utilities

class control(LOFARrecipe):
    """
    The LOFAR Standard Imaging Pipeline.
    """
    def _setup(self):
        # Set up logging to file
        os.makedirs(self.config.get("layout", "log_directory"))
        stream_handler = logging.StreamHandler(sys.stdout)
        file_handler = logging.FileHandler('%s/pipeline.log' % (
                self.config.get("layout", "log_directory")
            )
        )
        formatter = logging.Formatter(
            "%(asctime)s %(levelname)-7s %(name)s: %(message)s",
            "%Y-%m-%d %H:%M:%S"
        )
        stream_handler.setFormatter(formatter)
        file_handler.setFormatter(formatter)
        self.logger.addHandler(stream_handler)
        self.logger.addHandler(file_handler)

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
            raise PipelineRecipeFailed("%s failed", configblock)
        return outputs['data']

    def pipeline_logic(self):
        # Define pipeline logic here in subclasses
        raise NotImplementedError

    def go(self):
        try:
            super(control, self).go()
            self._setup()
        except Exception, e:
            # Until _setup() completes, we have no log handlers
            print "**** Pipeline setup failed"
            print "**** Error was: %s" % (str(e))

        self.logger.info(
            "Standard Imaging Pipeline (%s) starting." %
            (self.name,)
        )

        try:
            self.pipeline_logic()
        except PipelineException, message:
            self.logger.error(message)
            return 1

        return 0

if __name__ == '__main__':
    sys.exit(control().main())
