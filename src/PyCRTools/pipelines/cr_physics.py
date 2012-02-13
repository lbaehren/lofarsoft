"""CR pipeline.
"""

import pycrtools as cr

from optparse import OptionParser

# Parse commandline options
parser = OptionParser()
parser.add_option("-i", "--id", type="int", help="Event ID")

(options, args) = parser.parse_args()

