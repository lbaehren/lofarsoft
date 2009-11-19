from __future__ import with_statement
import sys, os
from subprocess import check_call, CalledProcessError
from contextlib import closing

# Cusine core
from cuisine.parset import Parset

# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities

class bbs(LOFARrecipe):
    def __init__(self):
        super(bbs, self).__init__()
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
            '-s', '--skymodel', 
            dest="skymodel",
            help="initial sky model (in makesourcedb format)"
        )
        self.optionparser.add_option(
            '-w', '--working-directory', 
            dest="working_directory",
            help="Working directory used on compute nodes"
        )
        self.optionparser.add_option(
            '--no-force',
            dest="no_force",
            help="Disable automatic cleanof control database, sky model parmdb, and instrument model parmdb"
        )
        self.optionparser.add_option(
            '--db-host',
            dest="db_host",
            help="Database host with optional port"
        )
        self.optionparser.add_option(
            '--db-user',
            dest="db_user",
            help="Database user"
        )
        self.optionparser.add_option(
            '--db-name',
            dest="db_name",
            help="Database name"
        )

    def go(self):
        self.logger.info("Starting MWImager run")
        super(bbs, self).go()

        if not self.inputs['working_directory']:
            self.inputs['working_directory'] = self.config.get(
                'bbs', 'working_directory'
            )
            self.logger.info("Using %s for working directory" % 
                (self.inputs['working_directory'],)
            )

        env = os.environ
        env.update({
            "PATH": self.config.get('bbs', 'env_path'),
            "LD_LIBRARY_PATH": self.config.get('bbs', 'env_ld_library_path'),
            "LOFARROOT": self.config.get('bbs', 'env_lofarroot')
        })
        
        log_location = "%s/%s" % (
            self.config.get('layout', 'log_directory'),
            self.config.get('bbs', 'log')
        )
        self.logger.debug("Logging to %s" % (log_location))
        self.logger.debug("Building BBS command string")
        bbs_cmd = [
            self.config.get('bbs', 'executable'),
            "--db", self._input_or_default['db_host'],
            "--db-name", self._input_or_default['db_name'],
            "--db-user", self._input_or_default['db_user'],
            "--cluster-desc", self.config.get('cluster', 'clusterdesc'),
            self._input_or_default('gvds'),
            self._input_or_default('parset'),
            self._input_or_default('skymodel'),
            self._input_or_default('working_directory')
        ]
        # if not self.inputs['no_force']:
        bbs_cmd.insert(1, '-f')
        # Check our verbosity
        bbs_cmd.insert(1, '-v')
        print bbs_cmd

#        try:
#            self.logger.info("Running BBS")
#            with closing(open(log_location, 'w')) as log:
#                result = check_call(
#                    [
#                        self.config.get('bbs', 'executable'),
#                        temp_parset_filename,
#                        self.config.get('cluster', 'clusterdesc'),
#                        self.inputs['working_directory'],
#                        log_location,
#                    ],
#                    env=env,
#                    stdout=log,
#                    stderr=log
#                    )
#            return result
#        except CalledProcessError:
#            self.logger.exception("Call to mwimager failed")
#            return 1
#        finally:
#            os.unlink(temp_parset_filename)

        return 0

if __name__ == '__main__':
    sys.exit(mwimager().main())
