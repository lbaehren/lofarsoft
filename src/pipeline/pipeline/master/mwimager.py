import sys, os
from tempfile import mkstemp
from subprocess import check_call
from ConfigParser import SafeConfigParser as ConfigParser

# Cusine core
from cuisine.WSRTrecipe import WSRTrecipe
from cuisine.ingredient import WSRTingredient
from cuisine.parset import Parset

# Local helpers
import utilities

# Root directory for config file
from pipeline import __path__ as config_path

class mwimager(WSRTrecipe):
    def __init__(self):
        super(mwimager, self).__init__()
        self.optionparser.add_option(
            '-j', '--job-name', 
            dest="job_name",
            help="Job Name"
        )
        self.optionparser.add_option(
            '-r', '--runtime-directory', 
            dest="runtime_directory",
            help="Runtime Directory"
        )
        self.optionparser.add_option(
            '-p', '--parset', 
            dest="parset",
            help="MWImager configuration parset"
        )
        self.optionparser.add_option(
            '-w', '--working-directory', 
            dest="working_directory",
            help="Working directory used on compute nodes"
        )
        self.optionparser.add_option(
            '-g', '--g(v)ds-file',
            dest="gvds",
            help="G(V)DS file describing data to be processed"
        )

    def go(self):
        self.logger.info("Starting MWImager run")
        print self.inputs['gvds']

        config = ConfigParser({
            "job_name": self.inputs["job_name"],
            "runtime_directory": self.inputs["runtime_directory"]
        })
        config.read("%s/pipeline.cfg" % (config_path[0],))

        if not self.inputs['working_directory']:
            self.inputs['working_directory'] = config.get(
                'mwimager', 'working_directory'
            )
            self.logger.info("Using %s for working directory" % 
                (self.inputs['working_directory'],)
            )

        if not self.inputs['parset']:
            self.inputs['parset'] = "%s/mwimager.parset" % (
                config.get("layout", "parset_directory"),
            )
            self.logger.info("Using %s for MWImager parset" % 
                (self.inputs['parset'],)
            )
        try:
            if not os.access(self.inputs['parset'], os.R_OK):
                raise IOError
        except IOError:
            self.logger.error("MWImager parset not found")
            raise

        # Patch GVDS filename into parset
        self.logger.info("Setting up MWImager configuration")
        temp_parset_filename = mkstemp(
            dir=config.get('layout', 'parset_directory')
        )[1]
        temp_parset = Parset()
        temp_parset.readFromFile(self.inputs['parset'])
        temp_parset['dataset'] = self.inputs['gvds']
        temp_parset.writeToFile(temp_parset_filename)

        env = os.environ
        env.update({
            "PATH": config.get('mwimager', 'env_path'),
            "LD_LIBRARY_PATH": config.get('mwimager', 'env_ld_library_path')
        })
        
        log_location = "%s/%s" % (
            config.get('layout', 'log_directory'),
            "/mwimager.log"
        )
        try:
            self.logger.info("Running MWImager")
            with closing(open(log_location, 'w')) as log:
                result = check_call(
                    [
                        config.get('mwimager', 'executable'),
                        temp_parset_filename,
                        config.get('cluster', 'clusterdesc'),
                        self.inputs['working_directory'],
                        log_location,
                        'dry'
                    ],
                    env=env,
                    stdout=log
                )
            return result
        finally:
            os.unlink(temp_parset_filename)

if __name__ == '__main__':
    sys.exit(mwimager().main())
