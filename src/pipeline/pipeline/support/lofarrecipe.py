import os
import utilities
from lofarexceptions import PipelineException
from cuisine.WSRTrecipe import WSRTrecipe
from IPython.kernel import client as IPclient
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
            '-c', '--config', 
            dest="config",
            help="Configuration file"
        )
        self.optionparser.add_option(
            '-n', '--dry-run',
            dest="dry_run",
            help="Dry run",
            action="store_true"
        )

    def go(self):
        # Every recipe needs a job identifier
        if not self.inputs["job_name"]:
            raise PipelineException("Job undefined")

        # If a config file hasn't been specified, use the default
        if not self.inputs["config"]:
            from pipeline import __path__ as config_path
            self.inputs["config"] = os.path.join(config_path[0], 'pipeline.cfg')
            self.logger.debug("Using default configuration file")

        self.config = ConfigParser({
            "job_name": self.inputs["job_name"],
            "runtime_directory": self.inputs["runtime_directory"]
        })
        self.config.read(self.inputs["config"])

        if not self.inputs['runtime_directory']:
            self.inputs["runtime_directory"] = self.config.get(
                "DEFAULT", "runtime_directory"
            )
            if not os.access(self.inputs['runtime_directory'], os.F_OK):
                raise IOError, "Runtime directory doesn't exist"

    def _get_cluster(self):
        self.logger.info("Connecting to IPython cluster")
        try:
            tc  = IPclient.TaskClient(self.config.get('cluster', 'task_furl'))
            mec = IPclient.MultiEngineClient(self.config.get('cluster', 'multiengine_furl'))
        except:
            self.logger.error("Unable to initialise cluster")
            raise utilities.ClusterError
        self.logger.info("Cluster initialised")
        return tc, mec

    def _input_or_default(self, key, section=None):
        # If the input array has a given key which doesn't have a value, see
        # if we can pull one out of the config file & return that.
        if not self.inputs.has_key(key):
            self.logger.debug("Not populating non-extant input key")
        if not self.inputs[key]:
            self.inputs[key] = self.config.get(section or self.__class__.__name__, key)
        return self.inputs[key]
