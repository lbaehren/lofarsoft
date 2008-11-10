"""Module image.

Instancess of class Image are a primary data-holders for all 
PyBDSM operations. They store the image itself together with
some meta-information (such as headers), options for processing
modules and all data generated during processing.

This module also defines class Op, which is used as a base class
for all PyBDSM operations.
"""

import numpy as N
from opts import *

class Image(object):
    """Image is a primary data container for PyBDSM.

    All the run-time data (such as image data, mask, etc.) 
    is stored here. A number of type-checked properties 
    are defined for the most basic image attributes, such
    as image data, mask, header, user options.

    There is little sense in declaring all possible attributes
    right here as it will introduce unneeded dependencies 
    between modules, thus most other attributes (like island lists,
    gaussian lists, etc) are inserted at run-time by the specific
    PyBDSM modules.
    """
    opts   = Instance(Opts, doc="User options")

    image  = NArray(doc="Image data")
    header = Any(doc="Image header")
    mask   = NArray(doc="Image mask (if present and attribute masked is set)")
    masked = Bool(False, doc="Flag if mask is present")


    def __init__(self, opts):
        self.opts = Opts(opts)


class Op(object):
    """Common base class for all PyBDSM operations.

    At the moment this class is empty and only defines placeholder
    for method __call__, which should be redefined in all derived
    classes.
    """
    def __call__(self, img):
        raise NotImplementedError("This method should be redifined")
