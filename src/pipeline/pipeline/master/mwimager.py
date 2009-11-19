from __future__ import with_statement
import sys, os, tempfile, glob, shutil, errno
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
            "LD_LIBRARY_PATH": self.config.get('mwimager', 'env_ld_library_path'),
            "LOFARROOT": self.config.get('mwimager', 'env_lofarroot')
        })
        
        # For the overall MWimgager log
        log_location = "%s/%s" % (
            self.config.get('layout', 'log_directory'),
            self.config.get('mwimager', 'log')
        )
        self.logger.debug("Logging to %s" % (log_location))
        # Individual subband logs go in a temporary directory
        # to be sorted out later.
        log_root = "%s/%s" % (tempfile.mkdtemp(), self.config.get('mwimager', 'log'))
        self.logger.debug("Logs dumped with root %s" % (log_root))

        mwimager_cmd = [
            self.config.get('mwimager', 'executable'),
            temp_parset_filename,
            self.config.get('cluster', 'clusterdesc'),
            self._input_or_default('working_directory'),
            log_root
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
        except CalledProcessError:
            self.logger.exception("Call to mwimager failed")
            result = 1
        finally:
            os.unlink(temp_parset_filename)

        # Now sort the log files into
        # appropriate places
        # This is ugly!
        self.logger.info("Moving logfiles")
        for log_file in glob.glob("%s%s" % (log_root, "*")):
            self.logger.debug("Processing %s" % (log_file))
            ms_name = ""
            with closing(open(log_file)) as file:
                for line in file.xreadlines():
                    split_line = line.split('=')
                    if split_line[0] == "Cimager.dataset":
                        ms_name = os.path.basename(split_line[1].rstrip())
                        break
            if not ms_name:
                self.logger.info("Couldn't identify file for %s" % (log_file))
            else:
                destination = "%s/%s/%s" % (
                    self.config.get('layout', 'log_directory'),
                    ms_name,
                    self.config.get('mwimager', 'log')
                )
                self.logger.debug(
                    "Moving logfile %s to %s" % (log_file, destination)
                )
                # Make sure the destination dir exists
                try:
                    os.makedirs(os.path.dirname(destination))
                except OSError, failure:
                    if failure.errno != errno.EEXIST:
                        raise
                shutil.move(log_file, destination)
        try:
            self.logger.debug("Removing temporary log directory")
            os.rmdir(os.path.dirname(log_root))
        except OSError, failure:
            self.logger.info("Failed to remove temporary directory")
            self.logger.debug(failure)

        return result

if __name__ == '__main__':
    sys.exit(mwimager().main())
