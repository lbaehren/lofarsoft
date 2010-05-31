#! /usr/bin/env python

## This is a template Python script.
#  Use this template for each script in the Pypeline.
#  Note that a script is not the same as a module, the difference can
#  already be seen by the #! on the first line.
#  For modules (e.g. if you want to import functions from this file into
#  another script) you should use modules/template.py instead.

"""
Style notes:
For Python, PEP 8  has emerged as the style guide that most projects adhere to; it promotes a very readable and eye-pleasing coding style. Every Python developer should read it at some point; here are the most important points extracted for you:

* Use 4-space indentation, and no tabs. 4 spaces are a good compromise between small indentation (allows greater nesting depth) and large indentation (easier to read). Tabs introduce confusion, and are best left out.

* Wrap lines so that they don't exceed 79 characters. This helps users with small displays and makes it possible to have several code files side-by-side on larger displays.

* Use blank lines to separate functions and classes, and larger blocks of code inside functions.

* When possible, put comments on a line of their own.

* Use docstrings.

* Use spaces around operators and after commas, but not directly inside bracketing constructs: a = f(1, 2) + g(3, 4).

* Name your classes and functions consistently; the convention is to use CamelCase for classes and lower_case_with_underscores for functions and methods. Always use self as the name for the first method argument (see A First Look at Classes for more on classes and methods).

* Don't use fancy encodings if your code is meant to be used in international environments. Plain ASCII works best in any case.
"""

## Imports
#  Only import modules that are actually needed and avoid
#  "from module import *" as much as possible to prevent name clashes.
import pycrtools
import numpy as np
import scipy as sp
import matplotlib.pyplot as plt

## Examples
def hello():
    """Say hello."""
    print "Hello world!"

# Run hello() function to say hello.
hello()

