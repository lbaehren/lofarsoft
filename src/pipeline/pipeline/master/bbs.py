from __future__ import with_statement
import sys, os, logging, tempfile, glob, shutil
from subprocess import check_call, CalledProcessError
from contextlib import closing

# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
import pipeline.support.utilities as utilities
from pipeline.support.clusterdesc import ClusterDesc

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
            help="Key to identify BBS session"
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

        self.optionparser.add_option(
            '--instrument-db',
            dest="instrument_db",
            help="Instrumnet database location"
        )

    def go(self):
        self.logger.info("Starting BBS run")
        super(bbs, self).go()

        clusterdesc = ClusterDesc(
            self.config.get('cluster', 'clusterdesc')
        )

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

        env = utilities.read_initscript(
            self.config.get('bbs', 'initscript')
        )
        
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
            "--key", self._input_or_default('key'),
            "--instrument-db", self._input_or_default('instrument_db'),
            self._input_or_default('gvds'),
            self._input_or_default('parset'),
            self._input_or_default('skymodel'),
            os.path.join(
                self._input_or_default('working_directory'),
                self.inputs['job_name']
            )
        ]
        if self.inputs['force']:
            bbs_cmd.insert(1, '-f')
        # Should BBS verbosity be linked to that of the pipeline, or should be
        # be a separate setting?
        if self.logger.level <= logging.INFO and self.logger.level != logging.NOTSET:
            bbs_cmd.insert(1, '-v')

        # Use a temporary directory to grab all the logs.
        log_root = tempfile.mkdtemp(dir=self.config.get('layout', 'log_directory'))
        self.logger.debug("Logs dumped to %s" % (log_root))

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
                        cwd=log_root
                    )
            else:
                self.logger.info("Dry run: execution skipped")
                result = 0
        except CalledProcessError:
            self.logger.exception("Call to BBS failed")
            result = 1

        self.logger.info("Moving logfiles")
        for log_file in glob.glob("%s/%s_%s" % (
            log_root, self.inputs["key"], "control*log")
        ):
            self.logger.debug("Processing %s" % (log_file))
            shutil.move(log_file, self.config.get('layout', 'log_directory'))
        for log_file in glob.glob("%s/%s_%s" % (
            log_root, self.inputs["key"], "calibrate*log*")
        ):
            self.logger.debug("Processing %s" % (log_file))
            ms_name = ""
            with closing(open(log_file)) as file:
                for line in file.xreadlines():
                    if line.split() and line.split()[0] == "part:":
                        ms_name = os.path.basename(line.split()[1].rstrip())
                        break
            if not ms_name:
                self.logger.info("Couldn't identify file for %s" % (log_file))
            else:
                destination = "%s/%s" % (
                    self.config.get('layout', 'log_directory'),
                    ms_name
                )
                self.logger.debug(
                    "Moving logfile %s to %s" % (log_file, destination)
                )
                utilities.move_log(log_file, destination)

        for log_file in glob.glob("%s/%s_%s" % (
            log_root, self.inputs["key"], "setupparmdb*log*")
        ):
            self.logger.debug("Processing %s" % (log_file))
            ms_name = ""
            with closing(open(log_file)) as file:
                ms_name = os.path.basename(file.readline().split()[5])
            if not ms_name:
                self.logger.info("Couldn't identify file for %s" % (log_file))
            else:
                destination = "%s/%s" % (
                    self.config.get('layout', 'log_directory'),
                    ms_name
                )
                self.logger.debug(
                    "Moving logfile %s to %s" % (log_file, destination)
                )
                utilities.move_log(log_file, destination)

        for log_file in glob.glob("%s/%s_%s" % (
            log_root, self.inputs["key"], "setupsourcedb*log*")
        ):
            self.logger.debug("Processing %s" % (log_file))
            ms_name = ""
            with closing(open(log_file)) as file:
                for line in file.xreadlines():
                    if line.split() and line.split()[0] == "Create":
                        ms_name = os.path.basename(os.path.dirname(line.split()[1]))
                        break
            if not ms_name:
                self.logger.info("Couldn't identify file for %s" % (log_file))
            else:
                destination = "%s/%s" % (
                    self.config.get('layout', 'log_directory'),
                    ms_name
                )
                self.logger.debug(
                    "Moving logfile %s to %s" % (log_file, destination)
                )
                utilities.move_log(log_file, destination)

        # Now pull in the logs from the individual cluster nodes
        self.logger.debug("Copying remote logs to %s"  % (log_root))
        for node in clusterdesc.get('ComputeNodes'):
            self.logger.debug("Node: %s" % (node))
            try:
                check_call(
                    [
                        "ssh",
                        node,
                        "--",
                        "mv",
                        "%s/%s/%s*log" % (
                            self._input_or_default('working_directory'),
                            self.inputs['job_name'],
                            self.inputs['key']
                        ),
                        log_root
                    ])
            except CalledProcessError:
                self.logger.warn("No logs moved on %s" % (node))
        for log_file in glob.glob("%s/%s_%s" % (
            log_root, self.inputs["key"], "kernel*log*")
        ):
            self.logger.debug("Processing %s" % (log_file))
            ms_name = ""
            with closing(open(log_file)) as file:
                for line in file.xreadlines():
                    if line.split(":") and line.split(":")[0] == "INFO - Observation part":
                        ms_name = os.path.basename(line.split()[6].rstrip())
                        break
            if not ms_name:
                self.logger.info("Couldn't identify file for %s" % (log_file))
            else:
                destination = "%s/%s" % (
                    self.config.get('layout', 'log_directory'),
                    ms_name
                )
                self.logger.debug(
                    "Moving logfile %s to %s" % (log_file, destination)
                )
                utilities.move_log(log_file, destination)

        try:
            self.logger.debug("Removing temporary log directory")
            shutil.rmtree(log_root)
        except OSError, failure:
            self.logger.info("Failed to remove temporary directory")
            self.logger.debug(failure)

        return result

if __name__ == '__main__':
    sys.exit(bbs().main())
