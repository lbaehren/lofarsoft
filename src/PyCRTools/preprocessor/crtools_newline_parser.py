#!/usr/bin/env python

import sys
import re
from optparse import OptionParser

## =============================================================================
##
##  Function definitions
##
## =============================================================================

def parseOptions():
    """
    Parse the options from the command line.
    """
    usage = "usage: %prog inputfile outputfile"
    parser = OptionParser(usage)

    (options, args) = parser.parse_args()

    if (len(args) != 2):
        parse.error("Incorrect number of arguments")

    return (options, args)


def parseCode(input_filename, output_filename):
    """
    Parse the code file
    """
    # Open input file
    input_file = open(input_filename, 'r')

    # Open output file
    output_file = open(output_filename, 'w')

    # Loop over all line
    for line in input_file:
        # Replace new lines
        line = re.sub("_H_NL_", "\n", line)
        #print line
        output_file.write(line)

    # Close output file
    output_file.close()

    # Close input file
    input_file.close()

    pass

## =============================================================================
##
##  Main
##
## =============================================================================

def main():
    """
    Main function
    """
    (options, args) = parseOptions()

    input_filename  = args[0]
    output_filename = args[1]

    parseCode(input_filename, output_filename)


if __name__ == '__main__':
    main()
