from __future__ import with_statement
import sys, os
from lofarpipe.support.pyraprunner import pyraprunner

# Quick n dirty tool to read ASCII sourcelists used as input by BBS.
# The format for these doesn't seem all that well specified: see the
# makesourcedb tool, which vaguely refers to a format string which might have
# spaces, commas, ...
# We'll do our best.

class Source(dict):
    pass

class SourceList(list):
    def __init__(self, filename):
        # Default format if we can't read one from the file
        format = (
            "Name", "Type", "Ra", "Dec", "I", "Q", "U", "V",
            "ReferenceFrequency='60e6'", "SpectralIndexDegree='0'",
            "SpectralIndex:0='0.0'", "Major", "Minor", "Phi"
        )
        with open(filename, 'r') as file:
            try:
                # Maybe the first line is a comma-separated format string...
                first_line = file.readline().strip().split()
                if first_line.split()[-1] == "format":
                    format = map(str.strip, first_line[3:-10].split(","))
                else:
                    raise
            except:
                # ...or maybe not.
                file.seek(0)
            for line in file:
                if len(line.strip()) == 0 or line.strip()[0] == '#': continue
                data = map(str.strip, line.split(','))
                self.append(Source(zip(format, data)))

class flagger(pyraprunner):
    def __init__(self):
        super(flagger, self).__init__()
        self.optionparser.add_option(
            '-s', '--skymodel',
            dest="skymodel",
            help="initial sky model (in makesourcedb format)"
        )
        self.optionparser.add_option(
            '--n-factor',
            dest="n_factor",
            type="float",
            help="Custom factor for flagging threshold"
        )

    def _generate_arguments(self):
        self.inputs['skymodel'] = os.path.join(
            self.config.get("layout", "parset_directory"),
            self.inputs['skymodel']
        )
        self.logger.info("Using %s for %s skymodel" %
            (self.inputs['skymodel'], "flagger")
        )
        if not os.access(self.inputs['skymodel'], os.R_OK):
            raise IOError

        sl = SourceList(self.inputs['skymodel'])
        return float(self.inputs['n_factor']) * sum(float(s['I']) for s in sl)

if __name__ == '__main__':
    sys.exit(flagger().main())
