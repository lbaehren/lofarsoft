import os, sys, inspect, logging, errno
import utilities
from lofarpipe.support.lofarexceptions import PipelineException, ClusterError
from lofarpipe.cuisine.WSRTrecipe import WSRTrecipe
from lofarpipe.support.lofaringredient import LOFARinput, LOFARoutput
from IPython.kernel import client as IPclient
from ConfigParser import NoOptionError, NoSectionError
from ConfigParser import SafeConfigParser as ConfigParser

class LOFARrecipe(WSRTrecipe):
    """
    Provides standard boiler-plate used in the various LOFAR pipeline recipes.
    """
    def __init__(self):
        super(LOFARrecipe, self).__init__()
        self.optionparser.add_option(
            '-j', '--job-name', 
            dest="job_name",
            help="Job name (required)"
        )
        self.optionparser.add_option(
            '-r', '--runtime-directory', 
            dest="runtime_directory",
            help="Runtime directory"
        )
        self.optionparser.add_option(
            '--working-directory',
            dest="default_working_directory",
            help="Default working directory"
        )
        self.optionparser.add_option(
            '--lofarroot',
            dest="lofarroot",
            help="Root of LOFAR software tree"
        )
        self.optionparser.add_option(
            '-c', '--config', 
            dest="config",
            help="Configuration file"
        )
        self.optionparser.add_option(
            '--task-file',
            dest="task_files",
            help="Task definition file"
        )
        self.optionparser.add_option(
            '-n', '--dry-run',
            dest="dry_run",
            help="Dry run",
            action="store_true"
        )
        self.optionparser.add_option(
            '--start-time',
            dest="start_time",
            help="[Expert use] Pipeline start time"
        )

    @property
    def __file__(self):
        import inspect
        full_location = os.path.abspath(inspect.getsourcefile(self.__class__))
        # DANGER WILL ROBINSON!
        # On the lofar cluster frontend (lfe001), home directories are in
        # /data/users, but on the nodes they are in /home. This workaround
        # means things work like one might expect for now, but this is not a
        # good long-term solution.
        return full_location.replace('/data/users', '/home')


    def run_task(self, configblock, datafiles=[], **kwargs):
        self.logger.info("Running task: %s" % (configblock,))
        try:
            recipe = self.task_definitions.get(configblock, "recipe")
        except NoSectionError:
            raise PipelineException("%s not found -- check your task definitions" % configblock)
        inputs = LOFARinput(self.inputs)
        inputs['args'] = datafiles
        inputs.update(self.task_definitions.items(configblock))
        # Any kwargs supplied by the caller override (or supplement) the configblock
        for key, value in kwargs.iteritems():
            inputs[key] = value
        # These inputs are never required:
        for inp in ('recipe', 'recipe_directories', 'lofarroot', 'default_working_directory', 'cwd'):
            del(inputs[inp])
        outputs = LOFARoutput()
        if self.cook_recipe(recipe, inputs, outputs):
            self.logger.warn(
                "%s reports failure (using %s recipe)" % (configblock, recipe)
            )
            raise PipelineRecipeFailed("%s failed", configblock)
        return outputs

    def _setup_logging(self):
        # Set up logging to file
        try:
            os.makedirs(self.config.get("layout", "log_directory"))
        except OSError, failure:
            if failure.errno != errno.EEXIST:
                raise

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

    def go(self):
        # Every recipe needs a job identifier
        if not self.inputs["job_name"]:
            raise PipelineException("Job undefined")

        if not self.inputs["start_time"]:
            import datetime
            self.inputs["start_time"] = datetime.datetime.utcnow().replace(microsecond=0).isoformat()

        # If a config file hasn't been specified, use the default
        if not self.inputs["config"]:
            # Possible config files, in order of preference:
            conf_locations = (
                os.path.join(sys.path[0], 'pipeline.cfg'),
                os.path.join(os.path.expanduser('~'), '.pipeline.cfg')
            )
            for path in conf_locations:
                if os.access(path, os.R_OK):
                    self.inputs["config"] = path
                    break
            if not self.inputs["config"]:
                raise PipelineException("Configuration file not found")

        self.logger.debug("Pipeline start time: %s" % self.inputs['start_time'])

        self.config = ConfigParser({
            "job_name": self.inputs["job_name"],
            "runtime_directory": self.inputs["runtime_directory"],
            "start_time": self.inputs["start_time"],
            "cwd": os.getcwd()
        })
        self.config.read(self.inputs["config"])

        if not self.inputs["task_files"]:
            try:
                self.inputs["task_files"] = utilities.string_to_list(
                    self.config.get('DEFAULT', "task_files")
                )
            except NoOptionError:
                self.inputs["task_files"] = []

        self.task_definitions = ConfigParser(self.config.defaults())
        self.task_definitions.read(self.inputs["task_files"])

        self.recipe_path = [
            os.path.join(root, 'master') for root in utilities.string_to_list(
                self.config.get('DEFAULT', "recipe_directories")
            )
        ]

        if not self.inputs['runtime_directory']:
            self.inputs["runtime_directory"] = self.config.get(
                "DEFAULT", "runtime_directory"
            )
            if not os.access(self.inputs['runtime_directory'], os.F_OK):
                raise IOError, "Runtime directory doesn't exist"

        if isinstance(self.logger.parent, logging.RootLogger):
            # Only configure handlers if our parent is the root logger.
            # Otherwise, our parent should have done it for us.
            self._setup_logging()

    def _get_cluster(self):
        self.logger.info("Connecting to IPython cluster")
        try:
            tc  = IPclient.TaskClient(self.config.get('cluster', 'task_furl'))
            mec = IPclient.MultiEngineClient(self.config.get('cluster', 'multiengine_furl'))
        except NoSectionError:
            self.logger.error("Cluster not definied in configuration")
            raise ClusterError
        except:
            self.logger.error("Unable to initialise cluster")
            raise ClusterError
        return tc, mec
