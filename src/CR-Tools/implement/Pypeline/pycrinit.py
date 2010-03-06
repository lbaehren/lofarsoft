#A file to load the pycrtools run it with
#python -i $LOFARSOFT/src/CR-Tools/implement/Pypeline/pycrinit.py 
#or with
#python -i $LOFARSOFT/src/CR-Tools/implement/Pypeline/pycrinit.py -i myfile.py


from pycrtools import *

#execute yet another script if added in the parameter list
if sys.argv[-2] == "-i": execfile(sys.argv[-1])
