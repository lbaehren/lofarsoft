"""Script to convert legacy results.py files to results.xml files.
"""

import sys
import xmldict

if len(sys.argv)<2:
    print "USAGE: results2xml filename"
    sys.exit(1)

filename = sys.argv[1]

# Execute results.py to load dictionary
r = {}
execfile(filename, r)

# Extract dictionary from loaded file
results = r["results"]

# Write dictionary to xml
xmldict.dump(filename.strip(".py"), results)

