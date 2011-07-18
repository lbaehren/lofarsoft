"""
This file contains a few simple shortcuts that make programming a little easier

"""

current_task = None

#Shortcuts used for Workspaces in task
default = "default"
dependencies = "dependencies"
doc = "doc"
export = "export"
output = "output"
positional = "positional"
unit = "unit"
workarray = "workarray"

def p_(default=None, doc="", unit="", **args):
    """
    Turn a tuple into a parameter definition used in WorkSpace.

    Example:

    class TaskWorkSpace(pycrtools.tasks.WorkSpace):
        parameter_definitions = {
            "lofarmode":{default:"LBA_OUTER",doc:"Which LOFAR mode was used (HBA/LBA_OUTER/LBA_INNER)"},
            "stride_n":p_(0,"if >0 then divide the FFT processing in n=2**stride_n blocks. This is slower but uses less memory.")
            }
    """
    d = {"default":default,
         "doc":doc,
         "unit":unit}

    if len(args)>0:
        d.update(args)

    return d
