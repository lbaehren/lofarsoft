# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import check_call, CalledProcessError
from tempfile import mkstemp
from ConfigParser import SafeConfigParser as ConfigParser
import os.path
import logging, logging.handlers

# External
from cuisine.parset import Parset
from pipeline.support.utilities import patch_parset, create_directory
from pipeline.support.lofarexceptions import ExecutableMissing
import pipeline.support.utilities as utilities

class dppp_node(object):
    def __init__(self, loghost="lfe001", logport=logging.handlers.DEFAULT_TCP_LOGGING_PORT, loglevel=logging.INFO):
        import platform
        self.logger = logging.getLogger('node.%s.%s' % (platform.node(), self.__class__.__name__))
        self.logger.setLevel(logging.DEBUG)
        if loghost:
            socketHandler = logging.handlers.SocketHandler(loghost, logport)
            self.logger.addHandler(socketHandler)

    def run(self, infile, outfile, parset, log_location, executable, initscript):
        # We need to patch the parset with the correct input/output MS names.
        self.logger.info("Processing %s" % (infile,))
        temp_parset_filename = patch_parset(
            parset, {
                'msin': infile,
                'msout': outfile,
            }
        )

        create_directory(os.path.dirname(log_location))
        create_directory(os.path.dirname(outfile))

        env = utilities.read_initscript(initscript)

        try:
            if not os.access(executable, os.X_OK):
                raise ExecutableMissing(executable)
            with closing(open(log_location, 'w')) as log:
                # What is the '1' for? Required by DP3...
                cmd = [executable, temp_parset_filename, '1']
                self.logger.info("Running: %s" % (' '.join(cmd),))
                result = check_call(
                    cmd,
                    stdout=log,
                    env=env
                )
                self.logger.debug(
                    "%s returned exit status %d" % (executable, result)
                )
            return result
        except ExecutableMissing, e:
            self.logger.error("%s not found" % (e.args[0]))
            raise
        except CalledProcessError, e:
            # For CalledProcessError isn't properly propagated by IPython
            # Temporary workaround...
            with closing(open(log_location, 'a')) as log:
                self.logger.error(str(e))
            raise Exception

        finally:
            os.unlink(temp_parset_filename)

#if __name__ == "__main__":
#    from sys import argv
#    try:
#        run_dppp(argv[1], argv[2], argv[3], argv[4])
#    except:
#        print "Usage: dppp [infile] [outfile] [parset] [logfile]"
