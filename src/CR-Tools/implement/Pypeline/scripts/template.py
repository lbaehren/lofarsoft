#! /usr/bin/env python

"""This is a template Python script.

Use this template for each script in the Pypeline.
Note that a script is not the same as a module, the difference can
already be seen by the #! on the first line.
For modules (e.g. files that are part of the library)
you should use modules/template.py instead.

Each script should have a documentation string (such as this block)
describing its purpose.

Style notes:

* Use 4-space indentation, and no tabs. 4 spaces are a good compromise between small indentation (allows greater nesting depth) and large indentation (easier to read). Tabs introduce confusion, and are best left out.

* Wrap lines so that they don't exceed 79 characters. This helps users with small displays and makes it possible to have several code files side-by-side on larger displays.

* Use blank lines to separate functions and classes, and larger blocks of code inside functions.

* When possible, put comments on a line of their own.

* Use docstrings with `reStructuredText <http://sphinx.pocoo.org/rest.html>`_ for markup.

* Use spaces around operators and after commas, but not directly inside bracketing constructs: ``a = f(1, 2) + g(3, 4)``.

* Name your classes and functions consistently, our convention is to use:

    * ``CamelCase`` for classes
    * ``mixedCase()`` for functions and methods.
    * ``lower_case_with_underscores`` for variables.

  Always use ``self`` as the name for the first method argument.

* Don't use fancy encodings if your code is meant to be used in international environments. Plain ASCII works best in any case.
"""

## Imports
#  Only import modules that are actually needed and avoid
#  "from module import *" as much as possible to prevent name clashes.
#  Furthermore always use the following abreviations.
import pycrtools as cr
import numpy as np
import scipy as sp
import matplotlib.pyplot as plt

## Commandline arguments
from optparse import OptionParser

usage = "usage: %prog [options] arg1 arg2"
parser = OptionParser(usage=usage)
parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=True,
                  help="make lots of noise [default]")
parser.add_option("-q", "--quiet",
                  action="store_false", dest="verbose",
                  help="be vewwy quiet (I'm hunting wabbits)")
parser.add_option("-f", "--filename",
                  metavar="FILE", help="write output to FILE")
parser.add_option("-m", "--mode",
                  default="intermediate",
                  help="interaction mode: novice, intermediate, "
                       "or expert [default: %default]")

(options, args) = parser.parse_args()

## Script body

# You can use the commandline options like so:
if option.verbose:
    print "I am being verbose"

# And the remaining positional arguments (typically for a list of filenames)
for i, arg in enumerate(args):
    print "positional argument", i, arg
