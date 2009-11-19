from __future__ import with_statement
import sys, os, logging, tempfile
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
            help="BBS configuration parset"
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
            '-f', '--force',
            dest="force",
            help="Automatically clean control database, sky model parmdb, and instrument model parmdb",
            action="store_true"
        )
        self.optionparser.add_option(
            '--key',
            dest="key",
            help="Key to identify BBS session",
            default="default_bbs"
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
        self.logger.info("Starting BBS run")
        super(bbs, self).go()

        if not self.inputs['skymodel']:
            self.inputs['skymodel'] = "%s/%s" % (
                self.config.get("layout", "parset_directory"),
                self.config.get("bbs", "skymodel")
            )
            self.logger.info("Using %s for %s skymodel" % 
                (self.inputs['parset'], "BBS")
            )
        if not os.access(self.inputs['skymodel'], os.R_OK):
            raise IOError

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
            "--db", self._input_or_default('db_host'),
            "--db-name", self._input_or_default('db_name'),
            "--db-user", self._input_or_default('db_user'),
            "--cluster-desc", self.config.get('cluster', 'clusterdesc'),
            "--key", self.inputs['key'],
            self.inputs['gvds'],
            self.inputs['parset'],
            self._input_or_default('skymodel'),
            self._input_or_default('working_directory')
            ]
        if self.inputs['force']:
            bbs_cmd.insert(1, '-f')
        # Should BBS verbosity be linked to that of the pipeline, or should be
        # be a separate setting?
        if self.logger.level <= logging.INFO and self.logger.level != logging.NOTSET:
            bbs_cmd.insert(1, '-v')

        # Use a temporary directory to grab all the logs.
        working_dir = tempfile.mkdtemp()
        self.logger.debug("Logs dumped to %s" % (working_dir))

        try:
            self.logger.info("Running BBS")
            self.logger.debug("Executing: %s" % " ".join(bbs_cmd))
            if not self.inputs['dry_run']:
                with closing(open(log_location, 'w')) as log:
                    result = check_call(
                        bbs_cmd,
                        env=env,
                        stdout=log,
                        stderr=log,
                        cwd=working_dir
                        )
            else:
                self.logger.info("Dry run: execution skipped")
                result = 0
            return result
        except CalledProcessError:
            self.logger.exception("Call to BBS failed")
            return 1

        return 0

if __name__ == '__main__':
    sys.exit(bbs().main())
