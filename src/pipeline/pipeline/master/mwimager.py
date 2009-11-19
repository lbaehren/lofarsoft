from __future__ import with_statement
import sys, os
from subprocess import check_call, CalledProcessError
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

        # Patch GVDS filename into parset
        self.logger.debug("Setting up MWImager configuration")
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
        self.logger.debug("Logging to %s" % (log_location))
        mwimager_cmd = [
            self.config.get('mwimager', 'executable'),
            temp_parset_filename,
            self.config.get('cluster', 'clusterdesc'),
            self._input_or_default('working_directory'),
            log_location,
        ]
        try:
            self.logger.info("Running MWImager")
            self.logger.debug("Executing: %s" % " ".join(mwimager_cmd))
            if not self.inputs['dry_run']:
                with closing(open(log_location, 'w')) as log:
                    result = check_call(
                        mwimager_cmd,
                        env=env,
                        stdout=log,
                        stderr=log
                        )
            else:
                self.logger.info("Dry run: execution skipped")
                result = 0
            return result
        except CalledProcessError:
            self.logger.exception("Call to mwimager failed")
            return 1
        finally:
            os.unlink(temp_parset_filename)

        return 0

if __name__ == '__main__':
    sys.exit(mwimager().main())
