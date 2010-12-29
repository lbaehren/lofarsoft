#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                              Base LOFAR Recipe
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

from ConfigParser import NoOptionError, NoSectionError
from ConfigParser import SafeConfigParser as ConfigParser
from threading import Event
from functools import partial

import os
import sys
import inspect
import logging
import errno

import lofarpipe.support.utilities as utilities
import lofarpipe.support.lofaringredient as ingredient
from lofarpipe.support.lofarexceptions import PipelineException
from lofarpipe.cuisine.WSRTrecipe import WSRTrecipe
from lofarpipe.support.lofaringredient import RecipeIngredients, LOFARinput, LOFARoutput
from lofarpipe.support.remotecommand import run_remote_command

class BaseRecipe(RecipeIngredients, WSRTrecipe):
    """
    Provides standard boiler-plate used in the various LOFAR pipeline recipes.
    """
    # Class ordering is important here.
    # WSRTrecipe.__init__ does not call a superclass, hence BaseIngredients
    # must go first.
    # Further, BaseIngredients.__init__ overwrites the inputs dict provided by
    # WSRTrecipe, so it must call super() before setting up inputs.
    inputs = {} # No inputs to add to defaults
    def __init__(self):
        """
        Subclasses should define their own parameters, but remember to call
        this __init__() method to include the required defaults.
        """
        super(BaseRecipe, self).__init__()
        self.error = Event()
        self.error.clear()

    @property
    def __file__(self):
        """
        Provides the file name of the currently executing recipe.
        """
        import inspect
        full_location = os.path.abspath(inspect.getsourcefile(self.__class__))
        # DANGER WILL ROBINSON!
        # On the lofar cluster frontend (lfe001), home directories are in
        # /data/users, but on the nodes they are in /home. This workaround
        # means things work like one might expect for now, but this is not a
        # good long-term solution.
        return full_location.replace('/data/users', '/home')

    def _setup_logging(self):
        """
        Set up logging.

        We always produce a log file and log to stdout. The name of the file
        and the logging format can be set in the pipeline configuration file.
        """
        try:
            logfile = self.config.get("logging", "log_file")
        except:
            logfile = os.path.join(
                self.config.get("layout", "job_directory"), 'logs/pipeline.log'
            )

        try:
            format = self.config.get("logging", "format", raw=True)
        except:
            format = "%(asctime)s %(levelname)-7s %(name)s: %(message)s"

        try:
            datefmt = self.config.get("logging", "datefmt", raw=True)
        except:
            datefmt = "%Y-%m-%d %H:%M:%S"

        try:
            os.makedirs(os.path.dirname(logfile))
        except OSError, failure:
            if failure.errno != errno.EEXIST:
                raise

        stream_handler = logging.StreamHandler(sys.stdout)
        file_handler = logging.FileHandler(logfile)

        formatter = logging.Formatter(format, datefmt)

        stream_handler.setFormatter(formatter)
        file_handler.setFormatter(formatter)
        self.logger.addHandler(stream_handler)
        self.logger.addHandler(file_handler)

    def run_task(self, configblock, datafiles=[], **kwargs):
        """
        A task is a combination of a recipe and a set of parameters.
        Tasks can be prefedined in the task file set in the pipeline
        configuration (default: tasks.cfg).

        Here, we load a task configuration and execute it.
        This is a "shorthand" version of
        :meth:`lofarpipe.cuisine.WSRTrecipe.WSRTrecipe.cook_recipe`.
        """
        self.logger.info("Running task: %s" % (configblock,))

        # Does the task definition exist?
        try:
            recipe = self.task_definitions.get(configblock, "recipe")
        except NoSectionError:
            raise PipelineException(
                "%s not found -- check your task definitions" % configblock
            )

        # Build inputs dict.
        # First, take details from caller.
        inputs = LOFARinput(self.inputs)
        inputs['args'] = datafiles

        # Add parameters from the task file.
        # Note that we neither need the recipe name nor any items from the
        # DEFAULT config.
        parameters = dict(self.task_definitions.items(configblock))
        del parameters['recipe']
        for key in dict(self.config.items("DEFAULT")).keys():
            del parameters[key]
        inputs.update(parameters)

        # Update inputs with provided kwargs, if any.
        inputs.update(kwargs)

        # Default outputs dict.
        outputs = LOFARoutput()

        # Cook the recipe and return the results"
        if self.cook_recipe(recipe, inputs, outputs):
            self.logger.warn(
                "%s reports failure (using %s recipe)" % (configblock, recipe)
            )
            raise PipelineRecipeFailed("%s failed", configblock)
        return outputs

    def _read_config(self):
        # If a config file hasn't been specified, use the default
        if not self.inputs.has_key("config"):
            # Possible config files, in order of preference:
            conf_locations = (
                os.path.join(sys.path[0], 'pipeline.cfg'),
                os.path.join(os.path.expanduser('~'), '.pipeline.cfg')
            )
            for path in conf_locations:
                if os.access(path, os.R_OK):
                    self.inputs["config"] = path
                    break
            if not self.inputs.has_key("config"):
                raise PipelineException("Configuration file not found")

        config = ConfigParser({
            "job_name": self.inputs["job_name"],
            "start_time": self.inputs["start_time"],
            "cwd": os.getcwd()
        })
        config.read(self.inputs["config"])
        return config

    def go(self):
        """
        This is where the work of the recipe gets done.
        Subclasses should define their own go() method, but remember to call
        this one to perform necessary initialisation.
        """
        # Every recipe needs a job identifier
        if not self.inputs.has_key("job_name"):
            raise PipelineException("Job undefined")

        if not self.inputs.has_key("start_time"):
            import datetime
            self.inputs["start_time"] = datetime.datetime.utcnow().replace(microsecond=0).isoformat()

        self.logger.debug("Pipeline start time: %s" % self.inputs['start_time'])

        # Config is passed in from spawning recipe. But if this is the start
        # of a pipeline, it won't have one.
        if not hasattr(self, "config"):
            self.config = self._read_config()

        # Ensure we have a runtime directory
        if not self.inputs.has_key('runtime_directory'):
            self.inputs["runtime_directory"] = self.config.get(
                "DEFAULT", "runtime_directory"
            )
        else:
            self.config.set('DEFAULT', 'runtime_directory', self.inputs['runtime_directory'])
        if not os.access(self.inputs['runtime_directory'], os.F_OK):
            raise IOError, "Runtime directory doesn't exist"

        # ...and task files, if applicable
        if not self.inputs.has_key("task_files"):
            try:
                self.inputs["task_files"] = utilities.string_to_list(
                    self.config.get('DEFAULT', "task_files")
                )
            except NoOptionError:
                self.inputs["task_files"] = []
        self.task_definitions = ConfigParser(self.config.defaults())
        self.task_definitions.read(self.inputs["task_files"])

        try:
            self.recipe_path = [
                os.path.join(root, 'master') for root in utilities.string_to_list(
                    self.config.get('DEFAULT', "recipe_directories")
                )
            ]
        except NoOptionError:
            self.recipe_path = []

        # At this point, the recipe inputs must be complete. If not, exit.
        if not self.inputs.complete():
            raise PipelineException(
                "Required inputs not available: %s" %
                " ".join(self.inputs.missing())
            )

        # Only configure handlers if our parent is the root logger.
        # Otherwise, our parent should have done it for us.
        if isinstance(self.logger.parent, logging.RootLogger):
            self._setup_logging()
