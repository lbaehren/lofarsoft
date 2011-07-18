.. _python_level:

==============
Python modules
==============

This document should describe how to add *Python* modules to the :mod:`pycrtools`.

All *Python* modules should be based on the :mod:`~pycrtools.template` module::

    """ This is a template Python module.
    
    Use this template for each module in the Pypeline.
    Note that a script is not the same as a module, the difference can
    already be seen by the #! on the first line.
    For scripts (e.g. files that you want to execute from the commandline)
    you should use scripts/template.py instead.
    
    Each module should have a documentation string (such as this block)
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
    
    All modules should contain a list of authors as the last element in the module block like so:
    
    .. moduleauthor:: Pim Schellart <P.Schellart@astro.ru.nl>
    """
    
    ## Imports
    #  Only import modules that are actually needed and avoid
    #  "from module import *" as much as possible to prevent name clashes.
    #  Furthermore always use the following abreviations.
    import pycrtools as cr
    import numpy as np
    import scipy as sp
    import matplotlib.pyplot as plt
    class Test(object):
        """This is just a dummy class to illustrate class level
        documentation.
    
        If this was a real class more extensive documentation should be
        placed here.
        """
    
        def __init__(self, a, b, c=True):
            """Initializes the object.
    
            Required arguments:
            
            ========= =================
            Parameter Description
            ========= =================
            *a*       does nothing
            *b*       also does nothing
            ========= =================
    
            Optional arguments:
            
            ========= =======================
            Parameter Description
            ========= =======================
            *c*       [True (default)| False]
            ========= =======================
    
            """
    
    def test(x, y):
        """ Returns x+y
    
        Required arguments:
        
        ========= ====================
        Parameter Description
        ========= ====================
        *x*       can be anything
        *y*       can also be anything
        ========= ====================
    
        Example::
    
            >>> test(324., 176)
            500.
    
        Or an example as doctest:
    
        .. doctest::
    
            >>> test(1, 2)
            3
    
        """
    
        return x + y

    # Execute doctests if module is executed as script
    if __name__ == "__main__":
        import doctest
        doctest.testmod()

