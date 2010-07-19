from __future__ import with_statement
import sys, os, logging, tempfile, glob, shutil, numpy
from subprocess import check_call, CalledProcessError
from contextlib import closing
from collections import defaultdict

# Local helpers
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.lofaringredient import LOFARinput, LOFARoutput
from lofarpipe.support.group_data import group_files
import lofarpipe.support.utilities as utilities
from lofarpipe.support.clusterdesc import ClusterDesc, get_compute_nodes

class bbs(LOFARrecipe):
    def __init__(self):
        super(bbs, self).__init__()
        self.optionparser.add_option(
            '--executable',
            dest="executable",
            help="Executable to be run (ie, calibrate script)"
        )
        self.optionparser.add_option(
            '--initscript',
            dest="initscript",
            help="Initscript to source (ie, lofarinit.sh)"
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
            '--log',
            dest="log",
            help="Log file"
        )
        self.optionparser.add_option(
            '--makevds-exec',
            dest="makevds_exec",
            help="makevds executable"
        )
        self.optionparser.add_option(
            '--combinevds-exec',
            dest="combinevds_exec",
            help="combinevds executable"
        )
        self.optionparser.add_option(
            '--max-bands-per-node',
            dest="max_bands_per_node",
            help="Maximum number of subbands to farm out to a given cluster node",
            default="8"
        )

    def go(self):
        self.logger.info("Starting BBS run")
        super(bbs, self).go()

        clusterdesc = ClusterDesc(
            self.config.get('cluster', 'clusterdesc')
        )

        self.inputs['skymodel'] = os.path.join(
            self.config.get("layout", "parset_directory"),
            self.inputs['skymodel']
        )
        self.logger.info("Using %s for %s skymodel" %
            (self.inputs['skymodel'], "BBS")
        )
        if not os.access(self.inputs['skymodel'], os.R_OK):
            self.logger.info("couldn't find skymodel")
            raise IOError

        self.inputs['parset'] = os.path.join(
            self.config.get("layout", "parset_directory"),
            self.inputs['parset']
        )
        self.logger.info("Using %s for %s parset" %
            (self.inputs['parset'], "BBS")
        )
        if not os.access(self.inputs['parset'], os.R_OK):
            self.logger.info("couldn't find parset")
            raise IOError

        log_location = "%s/%s" % (
            self.config.get('layout', 'log_directory'),
            self.inputs['log'],
        )
        self.logger.debug("Logging to %s" % (log_location))

        # Given a limited number of processes per node, the first task is to
        # partition up the data for processing.
        for iteration, data_group in enumerate(group_files(
            self.logger,
            clusterdesc,
            os.path.join(self.inputs['working_directory'], self.inputs['job_name']),
            int(self.inputs['max_bands_per_node']),
            self.inputs['args']
        )):
            # Now set up a vdsmaker recipe to build a GDS file describing the
            # processed data
            self.logger.info("Calling vdsmaker")
            bbs_gvds = tempfile.mkstemp()[1]
            inputs = LOFARinput(self.inputs)
            inputs['directory'] = self.config.get('layout', 'vds_directory')
            inputs['gvds'] = bbs_gvds
            inputs['args'] = data_group
            inputs['makevds'] = self.inputs['makevds_exec']
            inputs['combinevds'] = self.inputs['combinevds_exec']
            outputs = LOFARoutput()
            if self.cook_recipe('vdsmaker', inputs, outputs):
                self.logger.warn("vdsmaker reports failure")
                return 1

            self.logger.info("Using %s for %s gvds" %
                (bbs_gvds, "BBS")
            )
            if not os.access(bbs_gvds, os.R_OK):
                self.logger.info("couldn't find gvds")
                raise IOError

            env = utilities.read_initscript(self.inputs['initscript'])
            
            self.logger.debug("Building BBS command string")
            bbs_cmd = [
                self.inputs['executable'],
                "--db", self.inputs['db_host'],
                "--db-name", self.inputs['db_name'],
                "--db-user", self.inputs['db_user'],
                "--cluster-desc", self.config.get('cluster', 'clusterdesc'),
                "--key", self.inputs['key'],
                bbs_gvds,
                self.inputs['parset'],
                self.inputs['skymodel'],
                os.path.join(
                    self.inputs['working_directory'],
                    self.inputs['job_name']
                )
            ]
            if self.inputs['force'] is True or self.inputs['force'] == "True":
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
                    with utilities.log_time(self.logger):
                        with closing(open(log_location + '-' + str(iteration), 'w')) as log:
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
            except CalledProcessError, e:
                self.logger.exception("Call to BBS failed")
                result = 1
                return result

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
            for node in get_compute_nodes(clusterdesc):
                self.logger.debug("Node: %s" % (node))
                try:
                    check_call(
                        [
                            "ssh",
                            node,
                            "--",
                            "mv",
                            "%s/%s/%s*log" % (
                                self.inputs['working_directory'],
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
                ms_name = "Unknown"
                with closing(open(log_file)) as file:
                    error_messages = {
                        'WARN': set(),
                        'ERROR': set(),
                        'FATAL': set()
                    }
                    totals = defaultdict(lambda: defaultdict(list))
                    counts = defaultdict(lambda: defaultdict(list))
                    for line in file:
                        if line.split(":") and line.split(":")[0] == "INFO - Observation part":
                            ms_name = os.path.basename(line.split()[6].rstrip())
                        sline = line.split()
                        try:
                            if sline[0] in error_messages.keys():
                                error_messages[sline[0]].add(line.rstrip())
                            elif sline[0] == "Step:":
                                step += 1
                            elif sline[1] == "NextChunk":
                                step = -1
                            elif sline[2] == "TIMER":
                                totals[step][sline[4]].append(float(sline[6]))
                                counts[step][sline[4]].append(int(sline[8]))
                        except IndexError:
                            pass
                if ms_name == "Unknown":
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

                for value in error_messages.itervalues():
                    for message in list(value):
                        self.logger.warn("BBS error in %s :: %s" % (ms_name, message))

                self.logger.info("%s profiling information ------------" % (ms_name))
                for step in totals.iterkeys():
                    for key, values in totals[step].iteritems():
                        self.logger.info(
                            "Step: %d :: %s: count: %d, total: %f, mean: %f, std: %f" %
                            (step, key, numpy.sum(counts[step][key]), numpy.sum(values), numpy.mean(values), numpy.std(values))
                        )
                self.logger.info("-------------------------------------")

            try:
                self.logger.debug("Removing temporary log directory")
                shutil.rmtree(log_root)
            except OSError, failure:
                self.logger.info("Failed to remove temporary directory")
                self.logger.debug(failure)

        # Output filenames are the same as the input
        self.outputs['data'] = self.inputs['args']

        return result

if __name__ == '__main__':
    sys.exit(bbs().main())
