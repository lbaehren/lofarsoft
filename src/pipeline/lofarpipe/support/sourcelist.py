# Quick n dirty tool to read ASCII sourcelists used as input by BBS.
# The format for these doesn't seem all that well specified: see the
# makesourcedb tool, which vaguely refers to a format string which might have
# spaces, commas, ...
# We'll do our best.
from __future__ import with_statement

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
