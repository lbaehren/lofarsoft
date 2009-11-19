import os
import utilities
from cuisine.WSRTrecipe import WSRTrecipe
from IPython.kernel import client as IPclient
from ConfigParser import SafeConfigParser as ConfigParser

class PipelineException(Exception):
    pass

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
            '--dry-run',
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

        # If our inputs have a parset field, but one isn't specified, try
        # using a default
        if self.inputs.has_key('parset'):
            if not self.inputs['parset']:
                self.inputs['parset'] = os.path.join(
                    self.config.get("layout", "parset_directory"),
                    self.config.get(self.__class__.__name__, "parset")
                )
                self.logger.info("Using %s for %s parset" % 
                    (self.inputs['parset'], self.__class__.__name__)
                )
            if not os.access(self.inputs['parset'], os.R_OK):
                raise IOError, "Can't read parset"

        # If our inputs have a gvds field, but one isn't specified, try
        # using a default.
        if self.inputs.has_key('gvds'):
            if not self.inputs['gvds']:
                self.inputs['gvds'] = os.path.join(
                    self.config.get("layout", "vds_directory"),
                    self.config.get(self.__class__.__name__, "default_gvds")
                )
                self.logger.info("Using %s for %s GVDS" %
                    (self.inputs['gvds'], self.__class__.__name__)
                )
            if not os.access(self.inputs['gvds'], os.R_OK):
                raise IOError, "Can't read GVDS"
            

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
        
