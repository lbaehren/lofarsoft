## This is a template Python module.
#  Use this template for each module in the Pypeline.
#  Note that a script is not the same as a module, the difference can
#  already be seen by the #! on the first line.
#  For scripts (e.g. files that you want to execute from the commandline)
#  you should use scripts/template.py instead.

"""Each module should have a documentation string describing it.

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
import pycrtools as hf
## Examples
class PPF():
    """Class documentation"""

    def __init__(self):
        """Constructor"""
        
        weights_filename=hf.LOFARSOFT+'/src/CR-tools/data/ppfWeights16384.dat'
# Initialize arrays and values
        weights=hf.hArray(float,[16,1024])
        weights.readdump(weights_filename)

        if weights[0,0].val()==0.0:
            print "Obtaining Kaiser coefficient from file"
            # Reading of weights failed
            f=open(hf.LOFARSOFT+'/src/CR-Tools/data/Coeffs16384Kaiser-quant.dat')
            weights.setDim([16*1024])
            f.seek(0)
            for i in range(0,16*1024):
                weights[i]=float(f.readline())
            weights.setDim([16,1024])
        
        self.weights=weights
        self.buffer=hf.hArray(float,[16,1024])
        self.startrow=15
        self.total_rows_added=0

    def add(self,input):
        self.buffer[self.startrow].copy(input)
        input.fill(0)
        for row in range(0,16):
            input.muladd(self.weights[row],self.buffer[(row+self.startrow)%16])
        
        self.total_rows_added+=1
        self.startrow-=1
        if self.total_rows_added < 16:
            input.fill(0)
            return False
        
        return True


        

## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__=='__main__':
    import doctest
    doctest.testmod()

