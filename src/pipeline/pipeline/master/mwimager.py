from __future__ import with_statement
import sys, os, tempfile, glob, shutil, errno
from subprocess import check_call, CalledProcessError
from contextlib import closing

# Cusine core
from cuisine.parset import Parset

# Local helpers
from pipeline.support.lofarrecipe import LOFARrecipe
from pipeline.support.lofaringredient import LOFARinput, LOFARoutput
import pipeline.support.utilities as utilities

class mwimager(LOFARrecipe):
    def __init__(self):
        super(mwimager, self).__init__()
        self.optionparser.add_option(
            '--executable',
            dest="executable",
            help="Executable to be run (ie, mwimager script)"
        )
        self.optionparser.add_option(
            '--initscript',
            dest="initscript",
            help="Initscript to source (ie, lofarinit.sh)"
        )
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
        self.optionparser.add_option(
            '--log',
            dest="log",
            help="Log file"
        )
        self.optionparser.add_option(
            '--askapsoft-path',
            dest="askapsoft_path",
            help="Path to cimager.sh"
        )
        self.optionparser.add_option(
            '--casa',
            dest="casa",
            help="Use the CASA lwimager",
            action="store_true"
        )

    def go(self):
        self.logger.info("Starting MWImager run")
        super(mwimager, self).go()

        self.logger.info("Calling vdsmaker")
        inputs = LOFARinput(self.inputs)
        inputs['directory'] = self.config.get('layout', 'vds_directory')
        inputs['gvds'] = self.inputs['gvds']
        inputs['args'] = self.inputs['args']
        outputs = LOFARoutput()
        if self.cook_recipe('vdsmaker', inputs, outputs):
            self.logger.warn("vdsmaker reports failure")
            return 1

        self.outputs["data"] = []

        # Patch GVDS filename into parset
        self.logger.debug("Setting up MWImager configuration")
        temp_parset_filename = utilities.patch_parset(
            self.inputs['parset'],
            {'dataset': os.path.join(
                self.config.get('layout', 'vds_directory'), self.inputs['gvds']
                )
            },
            self.config.get('layout', 'parset_directory')
        )

        # Initscript for basic LOFAR utilities
        env = utilities.read_initscript(self.inputs['initscript'])
        # Also add the path for cimager.sh
        env['PATH'] = "%s:%s" % (self.inputs['askapsoft_path'], env['PATH'])
        
        # For the overall MWimgager log
        log_location = "%s/%s" % (
            self.config.get('layout', 'log_directory'),
            self.inputs['log']
        )
        self.logger.debug("Logging to %s" % (log_location))
        # Individual subband logs go in a temporary directory
        # to be sorted out later.
        log_root = os.path.join(tempfile.mkdtemp(), self.inputs['log'])
        self.logger.debug("Logs dumped with root %s" % (log_root))

        mwimager_cmd = [
            self.inputs['executable'],
            temp_parset_filename,
            self.config.get('cluster', 'clusterdesc'),
            os.path.join(
                self.inputs['working_directory'],
                self.inputs['job_name']
            ),
            log_root
        ]
        if self.inputs['casa'] is True or self.inputs['casa'] == "True":
            mwimager_cmd.insert(1, '-casa')
        try:
            self.logger.info("Running MWImager")
            self.logger.debug("Executing: %s" % " ".join(mwimager_cmd))
            if not self.inputs['dry_run']:
                with utilities.log_time(self.logger):
                    with closing(open(log_location, 'w')) as log:
                        result = check_call(
                            mwimager_cmd,
                            env=env,
                            stdout=log,
                            stderr=log,
                            close_fds=True
                            )
            else:
                self.logger.info("Dry run: execution skipped")
                result = 0
        except CalledProcessError:
            self.logger.exception("Call to mwimager failed")
            result = 1
        finally:
            os.unlink(temp_parset_filename)

        # Now parse the log files to:
        # 1: find the name of the images that have been written
        # 2: save the logs in appropriate places
        # This is ugly!
        self.logger.info("Parsing logfiles")
        for log_file in glob.glob("%s%s" % (log_root, "*")):
            self.logger.debug("Processing %s" % (log_file))
            ms_name, image_name = "", ""
            with closing(open(log_file)) as file:
                for line in file.xreadlines():
                    if 'Cimager.Images.Names' in line.strip():
                        try:
                            image_name = line.strip().split("=")[1].lstrip("['").rstrip("]'")
                            break
                        except IndexError:
                            pass
                file.seek(0)
                for line in file.xreadlines():
                    split_line = line.split('=')
                    if split_line[0] == "Cimager.dataset":
                        ms_name = os.path.basename(split_line[1].rstrip())
                        break
            if not image_name:
                self.logger.info("Couldn't identify image for %s "% (log_file))
            else:
                self.logger.debug("Found image: %s" % (image_name))
                self.outputs["data"].append(image_name)
            if not ms_name:
                self.logger.info("Couldn't identify file for %s" % (log_file))
            else:
                destination = "%s/%s/%s" % (
                    self.config.get('layout', 'log_directory'),
                    ms_name,
                    self.inputs['log']
                )
                self.logger.debug(
                    "Moving logfile %s to %s" % (log_file, destination)
                )
                utilities.move_log(log_file, destination)
        try:
            self.logger.debug("Removing temporary log directory")
            os.rmdir(os.path.dirname(log_root))
        except OSError, failure:
            self.logger.info("Failed to remove temporary directory")
            self.logger.debug(failure)

        return result

if __name__ == '__main__':
    sys.exit(mwimager().main())
