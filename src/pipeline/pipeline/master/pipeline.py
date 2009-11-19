import sys, datetime, logging, os
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.lofaringredient import LOFARinput, LOFARoutput
import pipeline.support.utilities as utilities

class PipelineException(Exception):
    pass

class PipelineRecipeFailed(PipelineException):
    pass

class PipelineReceipeNotFound(PipelineException):
    pass

class pipeline(LOFARrecipe):
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
            raise PipelineRecipeFailed("%s failed", configblock)
        return outputs['data']

    def pipeline_logic(self):
        # Define pipeline logic here in subclasses
        raise NotImplementedError

    def go(self):
        super(pipeline, self).go()
        self._setup()
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
    sys.exit(pipeline().main())
