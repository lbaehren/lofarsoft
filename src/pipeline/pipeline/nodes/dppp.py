# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import check_call, CalledProcessError
from tempfile import mkstemp
from ConfigParser import SafeConfigParser as ConfigParser
import os.path

# External
from cuisine.parset import Parset
from pipeline.support.utilities import patch_parset, create_directory
from pipeline.support.lofarexceptions import ExecutableMissing

# Root directory for config file
from pipeline import __path__ as config_path

def run_dppp(infile, outfile, parset, log_location):
    config = ConfigParser()
    config.read(os.path.join(config_path[0], 'pipeline.cfg'))
    executable = config.get('dppp', 'executable')

    # We need to patch the parset with the correct input/output MS names.
    temp_parset_filename = patch_parset(
        parset, {
            'msin': infile,
            'msout': outfile,
        }
    )

    create_directory(os.path.dirname(log_location))
    create_directory(os.path.dirname(outfile))

    env = utilities.read_initscript(
        self.config.get('dppp', 'initscript')
    )

    try:
        if not os.access(executable, os.X_OK):
            raise ExecutableMissing(executable)
        with closing(open(log_location, 'w')) as log:
            # What is the '1' for? Required by DP3...
            cmd = [executable, temp_parset_filename, '1']
            log.write(' '.join(cmd))
            result = check_call(
                cmd,
                stdout=log,
                env=env
            )
        return result
    except ExecutableMissing, e:
        with closing(open(log_location, 'a')) as log:
            log.write("%s not found" % (e.args[0]))
        raise
    except CalledProcessError, e:
        # For CalledProcessError isn't properly propagated by IPython
        # Temporary workaround...
        with closing(open(log_location, 'a')) as log:
            log.write(str(e))
        raise Exception

    finally:
        os.unlink(temp_parset_filename)

if __name__ == "__main__":
    from sys import argv
    try:
        run_dppp(argv[1], argv[2], argv[3], argv[4])
    except:
        print "Usage: dppp [infile] [outfile] [parset] [logfile]"
