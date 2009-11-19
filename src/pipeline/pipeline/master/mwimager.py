from __future__ import with_statement
import sys, os
from subprocess import check_call
from contextlib import closing

# Cusine core
from cuisine.parset import Parset

# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities

class mwimager(LOFARrecipe):
    def __init__(self):
        super(mwimager, self).__init__()
        self.optionparser.add_option(
            '-g', '--g(v)ds-file',
            dest="gvds",
            help="G(V)DS file describing data to be processed"
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

    def go(self):
        self.logger.info("Starting MWImager run")
        super(mwimager, self).go()

        if not self.inputs['working_directory']:
            self.inputs['working_directory'] = self.config.get(
                'mwimager', 'working_directory'
            )
            self.logger.info("Using %s for working directory" % 
                (self.inputs['working_directory'],)
            )

        # Patch GVDS filename into parset
        self.logger.info("Setting up MWImager configuration")
        temp_parset_filename = utilities.patch_parset(
            self.inputs['parset'],
            {'dataset': self.inputs['gvds']},
            self.config.get('layout', 'parset_directory')
        )

        env = os.environ
        env.update({
            "PATH": self.config.get('mwimager', 'env_path'),
            "LD_LIBRARY_PATH": self.config.get('mwimager', 'env_ld_library_path')
        })
        
        log_location = "%s/%s" % (
            self.config.get('layout', 'log_directory'),
            self.config.get('mwimager', 'log')
        )
        try:
            self.logger.info("Running MWImager")
            with closing(open(log_location, 'w')) as log:
                result = check_call(
                    [
                        self.config.get('mwimager', 'executable'),
                        temp_parset_filename,
                        self.config.get('cluster', 'clusterdesc'),
                        self.inputs['working_directory'],
                        log_location,
                    ],
                    env=env,
                    stdout=log,
                    stderr=log
                    )
            return result
        except CalledProcessError:
            self.logger.exception("Call to mwimager failed")
            return 1
        finally:
            os.unlink(temp_parset_filename)

        return 0

if __name__ == '__main__':
    sys.exit(mwimager().main())
